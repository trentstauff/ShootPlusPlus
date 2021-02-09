# ShootPlusPlus

I present to you, the most sought after game in the 1980s: ShootPlusPlus!

![](../main/imgs/ShooterPlusPlus.gif)

So, how did I get to here?

Let's walk through it together!

# Version 1.0

Firstly, lets cover some math. I know, you get (or have had) enough of it in school, but let's face it. Without linear algebra, games wouldn't exist.
So, instead of studying unit vectors from a text book- let's see them in action!

Starting off, we have to figure out how far away things are from the player so that we can render it correctly. We can accomplish this through a great sounding buzz word- Raycasting!

Raycasting isn't as difficult as it seems to be on paper. At first, it can be intimidating. But at its core, it's simple math!
Effectively, we send out rays from the player (you could call them unit vectors), and increment their size piece by piece until they've reached a spot that can tell us some information.

The player will always only see the objects in their FOV. This FOV dictates how many rays we will need to cast to render whats in their view!
We can simply iterate over the FOV, starting from left and going to the right, determining the distance to the object in question.

With this distance found, we can then decide how we'd like to show things to the user. We can also set rending limitations to allow a ceiling and a floor to show up, giving a sense of things being 3D.

Here's an example of raycasting in Wolfenstein- exactly what we are trying to replicate:

![](../main/imgs/raycasting.gif)

Here's a pretty drawing I did showcasing this idea into a diagram:

![](../main/imgs/raycasting.png)

Effectively, with this math, you have a working project! But, there is always more to improve on. As you can see, at this point the project is looking a little blurry. Edges arent sharp, and things are a little bland! Most importantly, we can't shoot anything yet!

![](../main/imgs/ShadingofASCII.jpg)
![](../main/imgs/HallwayViewASCII.jpg)

The biggest problem we face right now though, is difficulty to make things out in the distance, and things at a sharp angle to the player:

![](../main/imgs/ProblemHardToSeeFar.jpg)

But of course, some linear algebra is all we need.
What we can do to fix this issue, is to define the location between individual blocks. This will ensure it is clear where unit blocks start and stop, also allowing for better rending at sharp angles!

Basically, all we need to do is see if a ray has hit a corner of a unit block. We can do this by sending rays from the corner to the player, and if the angle between the players ray to the corner and the corners ray to the player is small enough, we're dealing with a corner.
After we have found that- we take the 2 closest corner rays, since we only want the corners visible to be rendered specially. 

![](../main/imgs/cornerdetection.png)

As you can see, after implementing this, up close we can easily tell where the unit blocks are, alongside being able to easily see the corners!

![](../main/imgs/upcloseblockviewASCII.jpg)

This ends version #1 - where things are strictly rendered on the command prompt through ASCII and Math. It's pretty amazing what you can do with just ASCII characters, but let's upgrade things.

# Version 2.0

After learning of the OLC Game Engine (a console game rendering engine), I knew things could be taken up a notch.

After porting Version 1.0 to this new game engine, things drastically improved. 

![](../main/imgs/PortedToEngineASCII.jpg)

Now, we can even specify the number of pixels we want to render, instead of being limited by the size of the ASCII characters!

![](../main/imgs/PORTEDBUTCONSOLEPIXELRAISED.jpg)
![](../main/imgs/PortedandUpscaled.jpg)

Still, while ASCII is cool, sprites are cooler. 
After gathering some sprites, let's make this look like a real early 90's game.
The engine that is being used has a nice draw function that takes in sprites- we'll use this to replace the ASCII characters. After doing this, look how cool things are coming along!

![](../main/imgs/Textures2.jpg)
![](../main/imgs/Textures.jpg)

I'll skip the math- but after some time, some Billboarded (things that always face the player) lamps were added. Checkout the code if you're interested!

![](../main/imgs/LampSprite.jpg)

And finally, I couldn't call this a shooter without being able to shoot. Basically, a bullet is just a ray cast vector that moves slower! Using this logic, and one more sprite later, we can finally shoot in our shooter:

![](../main/imgs/Bullets.jpg)

Finally, once again, we have the "finished" product:

![](../main/imgs/ShooterPlusPlus.gif)

# Whats Next?

- I'd love to add HP to the lamps- or even introduce some enemies that the bullets can do damage to and destroy.
- Random map generation: Would be super easy since the map is just a wstring!
- Multiplayer: As I learn more about Networking in C++, it would be SUPER sick to see a very basic multiplayer version.
- And who knows what else! Sounds, jumping, bhopping, the possibilities are endless.


HUGE shout out to this channel below for all of their AMAZING C++ videos. They are also the creator of the game engine used.
Inspiration: https://www.youtube.com/channel/UC-yuWVUplUJZvieEligKBkA

