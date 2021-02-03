// ASCII Shooter.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <chrono>
#include <Windows.h>
#include <string>
#include <vector>
#include <algorithm>

int screen_width = 120;
int screen_height = 40;

float fPlayerX = 8.0f;
float fPlayerY = 8.0f;
float fPlayerAngle = 0.0f;

int map_height = 16;
int map_width = 16;

float fFOV = 3.14159 / 4.0;
// Coresponds to the map size
// TODO: Dont forget to change this when doing dynamic maps
float fDepth = 16.0f;

int main()
{
    // Screen Buffer - this is so I can directly write to the console
    // wide char chosen as we will have unicode characters in our array
    int screen_area = screen_width * screen_height;
    wchar_t* screen = new wchar_t[screen_area];

    // setting up of the textmode buffer
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

    // tells the buffer it will be the target of our console
    SetConsoleActiveScreenBuffer(hConsole);

    // useless variable, nessecary for the following action
    DWORD dwBytesWritten = 0;

    // TODO: Randomly generated maps
    std::wstring map;

    map += L"################";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#...#..........#";
    map += L"#........#.....#";
    map += L"#........#.....#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#........#######";
    map += L"#..............#";
    map += L"#........#######";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"################";

    auto tp1 = std::chrono::system_clock::now();
    auto tp2 = std::chrono::system_clock::now();

    //Writeup:

    // using console handler
    // field of view variable, explain distance and how shading will work
    // basic raytracing, include gif from Wolfinstine wiki page
    // chrono to make sure console inputs are correct - 18:23


    // TODO: Make exit clause
    while (true) 
    {
        // make sure inputs are in sync with computer speed
        tp2 = std::chrono::system_clock::now();
        std::chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float fElapsedTime = elapsedTime.count();

        // Player Control
        // Handle CCW Rotation
        if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
            fPlayerAngle -= (0.8f) * fElapsedTime;

        if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
            fPlayerAngle += (0.8f) * fElapsedTime;

        if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
        {
            fPlayerX += sinf(fPlayerAngle) * 5.0f * fElapsedTime;
            fPlayerY += cosf(fPlayerAngle) * 5.0f * fElapsedTime;

            if (map.c_str()[(int)fPlayerX * map_width + (int)fPlayerY] == '#')
            {
                fPlayerX -= sinf(fPlayerAngle) * 5.0f * fElapsedTime;
                fPlayerY -= cosf(fPlayerAngle) * 5.0f * fElapsedTime;
            }
        }
            
        if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
        {
            fPlayerX -= sinf(fPlayerAngle) * 5.0f * fElapsedTime;
            fPlayerY -= cosf(fPlayerAngle) * 5.0f * fElapsedTime;

            if (map.c_str()[(int)fPlayerX * map_width + (int)fPlayerY] == '#')
            {
                fPlayerX += sinf(fPlayerAngle) * 5.0f * fElapsedTime;
                fPlayerY += cosf(fPlayerAngle) * 5.0f * fElapsedTime;
            }
        }

        // TODO: Compatibility for Y axis
        for (int x = 0; x < screen_width; x++)
        {
            // for each column on the screen, calculate the projected ray angle into the space
            float fFOVAngle = (fPlayerAngle - fFOV / 2.0f) + ((float)x / (float)screen_width) * fFOV;

            float fDistanceToWall = 0;
            bool bHitWall = false;
            bool bBoundary = false;

            // Unit vectors for direction player is looking
            float fEyeX = sinf(fFOVAngle);
            float fEyeY = cosf(fFOVAngle);

            // this (poorly) calculates the distance to the wall for a ray trace
            // TODO: improve this
            while (!bHitWall && fDistanceToWall < fDepth)
            {
                fDistanceToWall += 0.1f;

                int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
                int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);

                // Test if ray is out of bounds
                // TODO: update for dynamic map
                if (nTestX < 0 || nTestX >= map_width || nTestY < 0 || nTestY >= map_height) 
                {
                    bHitWall = true;
                    fDistanceToWall = fDepth;
                }
                else
                {
                    // Ray is inbounds so test to see if the ray cell is a wall block
                    if (map.c_str()[nTestX * map_width + nTestY] == '#')
                    {
                        bHitWall = true;

                        // to highlight tile boundaries, cast a ray from each corner
                        // of the tile, to the player. The more coincident this ray
                        // is to the rendering ray, the closer we are to a tile 
                        // boundary, which we'll shade to add detail to the walls
                        std::vector<std::pair<float, float>> p;
                        
                        // Test each corner of hit tile, storing the distance from
                        // the player, and the calculated dot product of the two rays
                        for (int tx = 0; tx < 2; tx++)
                            for (int ty = 0; ty < 2; ty++)
                            {
                                // Angle of corner to eye
                                float vy = (float)nTestY + ty - fPlayerY;
                                float vx = (float)nTestX + tx - fPlayerX;
                                float d = sqrt(vx * vx + vy * vy);
                                float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
                                p.push_back(std::make_pair(d, dot));
                            }

                        // Sort Pairs from closest to farthest
                        sort(p.begin(), p.end(), [](const std::pair<float, float>& left, const std::pair<float, float>& right) {return left.first < right.first; });

                        // First two/three are closest (we will never see all four)
                        float fBound = 0.01;
                        if (acos(p.at(0).second) < fBound) bBoundary = true;
                        if (acos(p.at(1).second) < fBound) bBoundary = true;
                        if (acos(p.at(2).second) < fBound) bBoundary = true;
                    }
                }
            }

            // Calculate the distance to ceiling and floor
            // Take the midpoint of screen, subtract relative height WRT distance to wall
            int nCeiling = (float)(screen_height / 2.0) - screen_height / ((float)fDistanceToWall);
            int nFloor = screen_height - nCeiling;

            short nShade = ' ';
            if (fDistanceToWall <= fDepth / 4.0f)       nShade = 0x2588;  // close to wall
            else if (fDistanceToWall < fDepth / 3.0f)   nShade = 0x2593;
            else if (fDistanceToWall < fDepth / 2.0f)   nShade = 0x2592;
            else if (fDistanceToWall < fDepth)          nShade = 0x2591;
            else                                        nShade = ' ';     // too far away from wall

            if (bBoundary) nShade = ' ';

            for (int y = 0; y < screen_height; y++)
            {
                // Each Row
                if (y <= nCeiling)
                    screen[y * screen_width + x] = ' ';
                else if (y > nCeiling && y <= nFloor)
                    screen[y * screen_width + x] = nShade;
                else // Floor
                {
                    // Shade floor based on distance
                    float b = 1.0f - (((float)y - screen_height / 2.0f) / ((float)screen_height / 2.0f));
                    if (b < 0.25)		nShade = '#';
                    else if (b < 0.5)	nShade = 'x';
                    else if (b < 0.75)	nShade = '.';
                    else if (b < 0.9)	nShade = '-';
                    else				nShade = ' ';
                    screen[y * screen_width + x] = nShade;
                }
            }

        }

        // display Stats
        swprintf_s(screen, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f ", fPlayerX, fPlayerY, fPlayerAngle, 1.0f / fElapsedTime);

        // display Map
        for (int nx = 0; nx < map_width; nx++)
            for (int ny = 0; ny < map_width; ny++)
            {
                screen[(ny + 1) * screen_width + nx] = map[ny * map_width + nx];
            }
        screen[((int)fPlayerX + 1) * screen_width + (int)fPlayerY] = 'P';

        screen[screen_area - 1] = '\0';
        // allows me to specify the coordinate of where to write the text
        WriteConsoleOutputCharacter(hConsole, screen, screen_area, { 0,0 }, &dwBytesWritten);
    }
    

    return 0;
}
