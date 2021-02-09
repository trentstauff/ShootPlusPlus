
#include <iostream>
#include <chrono>
#include <Windows.h>
#include <string>
#include <vector>
#include <algorithm>
#include "olcConsoleGameEngine.h"

class FPS : public olcConsoleGameEngine 
{

private:

    float fPlayerX = 8.0f;
    float fPlayerY = 8.0f;
    float fPlayerAngle = 0.0f;
    float fSpeed = 5.0;

    int map_height = 32;
    int map_width = 32;

    float fFOV = 3.14159 / 4.0;
    // Coresponds to the map size
    // TODO: Dont forget to change this when doing dynamic maps
    float fDepth = 16.0f;

    std::wstring map;

    olcSprite* spriteWall;
    olcSprite* spriteLamp;
    olcSprite* spriteBullet;

    float* fDepthBuffer = nullptr;

    struct sObject
    {
        float x;
        float y;
        float vx;
        float vy;
        bool bRemove;
        olcSprite* sprite;
    };

    std::list<sObject> listObjects;

public:
    FPS() 
    {
        m_sAppName = L"First Person Shooter";
    }

    virtual bool OnUserCreate()
    {
        map += L"#########.......#########.......";
        map += L"#...............#...............";
        map += L"#.......#########.......########";
        map += L"#..............##..............#";
        map += L"#......##......##......##......#";
        map += L"#......##..............##......#";
        map += L"#..............##..............#";
        map += L"###............####............#";
        map += L"##.............###.............#";
        map += L"#............####............###";
        map += L"#..............................#";
        map += L"#..............##..............#";
        map += L"#..............##..............#";
        map += L"#...........#####...........####";
        map += L"#..............................#";
        map += L"###..####....########....#######";
        map += L"####.####.......######..........";
        map += L"#...............#...............";
        map += L"#.......#########.......##..####";
        map += L"#..............##..............#";
        map += L"#......##......##.......#......#";
        map += L"#......##......##......##......#";
        map += L"#..............##..............#";
        map += L"###............####............#";
        map += L"##.............###.............#";
        map += L"#............####............###";
        map += L"#..............................#";
        map += L"#..............................#";
        map += L"#..............##..............#";
        map += L"#...........##..............####";
        map += L"#..............##..............#";
        map += L"################################";

        spriteWall = new olcSprite(L"../fps_wall1.spr");
        spriteLamp = new olcSprite(L"../fps_lamp1.spr");
        spriteBullet = new olcSprite(L"../fps_fireball1.spr");

        fDepthBuffer = new float[ScreenWidth()];

        listObjects = {
            { 8.5f, 8.5f, 0.0f, 0.0f, false, spriteLamp },
            { 7.5f, 7.5f, 0.0f, 0.0f, false, spriteLamp },
            { 10.5f, 3.5f, 0.0f, 0.0f, false, spriteLamp },
        };

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime)
    {
        // Player Control
        // Handle CCW Rotation
        if (m_keys[L'A'].bHeld)
            fPlayerAngle -= (0.8f) * fElapsedTime;

        if (m_keys[L'D'].bHeld)
            fPlayerAngle += (0.8f) * fElapsedTime;

        if (m_keys[L'W'].bHeld)
        {
            fPlayerX += sinf(fPlayerAngle) * fSpeed * fElapsedTime;
            fPlayerY += cosf(fPlayerAngle) * fSpeed * fElapsedTime;

            if (map.c_str()[(int)fPlayerX * map_width + (int)fPlayerY] == '#')
            {
                fPlayerX -= sinf(fPlayerAngle) * fSpeed * fElapsedTime;
                fPlayerY -= cosf(fPlayerAngle) * fSpeed * fElapsedTime;
            }
        }

        if (m_keys[L'S'].bHeld)
        {
            fPlayerX -= sinf(fPlayerAngle) * fSpeed * fElapsedTime;
            fPlayerY -= cosf(fPlayerAngle) * fSpeed * fElapsedTime;

            if (map.c_str()[(int)fPlayerX * map_width + (int)fPlayerY] == '#')
            {
                fPlayerX += sinf(fPlayerAngle) * fSpeed * fElapsedTime;
                fPlayerY += cosf(fPlayerAngle) * fSpeed * fElapsedTime;
            }
        }

        // Handle Strafe Right movement & collision
        if (m_keys[L'E'].bHeld)
        {
            fPlayerX += cosf(fPlayerAngle) * fSpeed * fElapsedTime;
            fPlayerY -= sinf(fPlayerAngle) * fSpeed * fElapsedTime;
            if (map.c_str()[(int)fPlayerX * map_width + (int)fPlayerY] == '#')
            {
                fPlayerX -= cosf(fPlayerAngle) * fSpeed * fElapsedTime;
                fPlayerY += sinf(fPlayerAngle) * fSpeed * fElapsedTime;
            }
        }

        // Handle Strafe Left movement & collision
        if (m_keys[L'Q'].bHeld)
        {
            fPlayerX -= cosf(fPlayerAngle) * fSpeed * fElapsedTime;
            fPlayerY += sinf(fPlayerAngle) * fSpeed * fElapsedTime;
            if (map.c_str()[(int)fPlayerX * map_width + (int)fPlayerY] == '#')
            {
                fPlayerX += cosf(fPlayerAngle) * fSpeed * fElapsedTime;
                fPlayerY -= sinf(fPlayerAngle) * fSpeed * fElapsedTime;
            }
        }

        // Fire Bullets
        if (m_keys[VK_SPACE].bReleased)
        {
            sObject o;
            o.x = fPlayerX;
            o.y = fPlayerY;
            float fNoise = (((float)rand() / (float)RAND_MAX) - 0.5f) * 0.1f;
            o.vx = sinf(fPlayerAngle + fNoise) * 8.0f;
            o.vy = cosf(fPlayerAngle + fNoise) * 8.0f;
            o.sprite = spriteBullet;
            o.bRemove = false;
            listObjects.push_back(o);
        }

        // TODO: Compatibility for Y axis
        for (int x = 0; x < ScreenWidth(); x++)
        {
            // for each column on the screen, calculate the projected ray angle into the space
            float fFOVAngle = (fPlayerAngle - fFOV / 2.0f) + ((float)x / (float)ScreenWidth()) * fFOV;

            float fDistanceToWall = 0;
            float fStepSize = 0.01f;

            bool bHitWall = false;
            bool bBoundary = false;

            // Unit vectors for direction player is looking
            float fEyeX = sinf(fFOVAngle);
            float fEyeY = cosf(fFOVAngle);

            float fSampleX = 0.0f;

            // this (poorly) calculates the distance to the wall for a ray trace
            // TODO: improve this
            while (!bHitWall && fDistanceToWall < fDepth)
            {
                fDistanceToWall += fStepSize;

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

                        // determines if ray has hit wall
                        // gets midpoint of cell
                        float fBlockMidX = (float)nTestX + 0.5f;
                        float fBlockMidY = (float)nTestY + 0.5f;

                        float fTestPointX = fPlayerX + fEyeX * fDistanceToWall;
                        float fTestPointY = fPlayerY + fEyeY * fDistanceToWall;

                        float fTestAngle = atan2f((fTestPointY - fBlockMidY), (fTestPointX - fBlockMidX));

                        if (fTestAngle >= -3.14159f * 0.25f && fTestAngle < 3.14159f * 0.25f)
                            fSampleX = fTestPointY - (float)nTestY;
                        if (fTestAngle >= 3.14159f * 0.25f && fTestAngle < 3.14159f * 0.75f)
                            fSampleX = fTestPointX - (float)nTestX;
                        if (fTestAngle < -3.14159f * 0.25f && fTestAngle >= -3.14159f * 0.75f)
                            fSampleX = fTestPointX - (float)nTestX;
                        if (fTestAngle >= 3.14159f * 0.75f || fTestAngle < -3.14159f * 0.75f)
                            fSampleX = fTestPointY - (float)nTestY;


                    }
                }
            }

            // Calculate the distance to ceiling and floor
            // Take the midpoint of screen, subtract relative height WRT distance to wall
            int nCeiling = (float)(ScreenHeight() / 2.0) - ScreenHeight() / ((float)fDistanceToWall);
            int nFloor = ScreenHeight() - nCeiling;

            // update depth buffer
            fDepthBuffer[x] = fDistanceToWall;

            for (int y = 0; y < ScreenHeight(); y++)
            {
                // Each Row
                if (y <= nCeiling)
                    Draw(x, y, L' ');
                else if (y > nCeiling && y <= nFloor)
                {
                    if (fDistanceToWall < fDepth)
                    {
                        // Wall
                        float fSampleY = ((float)y - (float)nCeiling) / ((float)nFloor - (float)nCeiling);
                        Draw(x, y, spriteWall->SampleGlyph(fSampleX, fSampleY), spriteWall->SampleColour(fSampleX, fSampleY));
                    }
                    else {
                        Draw(x, y, PIXEL_SOLID, 0);
                    }
                    
                }
                else // Floor
                {
                    
                    Draw(x, y, PIXEL_SOLID, FG_DARK_BLUE);
                }
            }

        }

        // Update & Draw Objects		
        for (auto& object : listObjects)
        {
            // Update Object Physics
            object.x += object.vx * fElapsedTime;
            object.y += object.vy * fElapsedTime;

            // Check if object is inside wall - set flag for removal
            if (map.c_str()[(int)object.x * map_width + (int)object.y] == '#')
                object.bRemove = true;

            // Can object be seen?
            float fVecX = object.x - fPlayerX;
            float fVecY = object.y - fPlayerY;
            float fDistanceFromPlayer = sqrtf(fVecX * fVecX + fVecY * fVecY);

            float fEyeX = sinf(fPlayerAngle);
            float fEyeY = cosf(fPlayerAngle);

            // Calculate angle between lamp and players feet, and players looking direction
            // to determine if the lamp is in the players field of view
            float fObjectAngle = atan2f(fEyeY, fEyeX) - atan2f(fVecY, fVecX);
            if (fObjectAngle < -3.14159f)
                fObjectAngle += 2.0f * 3.14159f;
            if (fObjectAngle > 3.14159f)
                fObjectAngle -= 2.0f * 3.14159f;

            bool bInPlayerFOV = fabs(fObjectAngle) < fFOV / 2.0f;

            if (bInPlayerFOV && fDistanceFromPlayer >= 0.5f && fDistanceFromPlayer < fDepth && !object.bRemove)
            {
                float fObjectCeiling = (float)(ScreenHeight() / 2.0) - ScreenHeight() / ((float)fDistanceFromPlayer);
                float fObjectFloor = ScreenHeight() - fObjectCeiling;
                float fObjectHeight = fObjectFloor - fObjectCeiling;
                float fObjectAspectRatio = (float)object.sprite->nHeight / (float)object.sprite->nWidth;
                float fObjectWidth = fObjectHeight / fObjectAspectRatio;
                float fMiddleOfObject = (0.5f * (fObjectAngle / (fFOV / 2.0f)) + 0.5f) * (float)ScreenWidth();

                // Draw Lamp
                for (float lx = 0; lx < fObjectWidth; lx++)
                {
                    for (float ly = 0; ly < fObjectHeight; ly++)
                    {
                        float fSampleX = lx / fObjectWidth;
                        float fSampleY = ly / fObjectHeight;
                        wchar_t c = object.sprite->SampleGlyph(fSampleX, fSampleY);
                        int nObjectColumn = (int)(fMiddleOfObject + lx - (fObjectWidth / 2.0f));
                        if (nObjectColumn >= 0 && nObjectColumn < ScreenWidth())
                        {
                            // dont render lamps if:
                            if (c != L' ' && fDepthBuffer[nObjectColumn] >= fDistanceFromPlayer) 
                            {
                                Draw(nObjectColumn, fObjectCeiling + ly, c, object.sprite->SampleColour(fSampleX, fSampleY));
                                fDepthBuffer[nObjectColumn] = fDistanceFromPlayer;
                            }
                                
                        }
                    }
                }
            }
        }

        // Remove dead objects from object list
        listObjects.remove_if([](sObject& o) {return o.bRemove; });


        // display Map
        for (int nx = 0; nx < map_width; nx++)
            for (int ny = 0; ny < map_width; ny++)
            {
                Draw(nx + 1, ny + 1, map[ny * map_width + nx]);
            }

        Draw(1 + (int)fPlayerY, 1 + (int)fPlayerX, L'P');

        return true;
    }

};

int main()
{
    
    FPS game;
    game.ConstructConsole(320, 240, 4, 4);
    game.Start();

    return 0;
}
