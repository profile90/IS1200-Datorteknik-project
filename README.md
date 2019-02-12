

# Project plan


#### General Overview

The project will consist of replicating the old Atari game Missile Command in all ways with reservations for memory and processing power. As such we will not put a historical max on either processing speed nor memory usage (as a real port might have). 

The game will have historical highscores (even if the device is turned off) going back at least three games, and also a simple menus for inputting a three character name. Each time a level is cleared a splash screen appear congratulating the player on clearing the level, before the next one starts. Levels are determined by a multiplier on both the score and the speed of the enemy rockets. Further investigation of the original game is required to determine if the player missiles get any faster as well.

The controls will consist of a simple i/o shield with a joystick and buttons arrayed in the classic control scheme (a turned about square). With the joystick the player will control both aim and menus, three of the buttons will be fire while playing (where each one corresponds to a specific missile silo) and one of these will be select while in the menus. A 128x128 pixel OLED display will display the game, which consists of animated sprites, simple 2x1 pixel missiles and static landscapes.

## Game architecture

The game will be written in C primarily and in assembly where deemed neccessary. A simple architecture consisting of structs and buffers will serve as the core, utilizing interupts for input and display updates.


The main buffer will be the display buffer where most of the graphics will be written to, there all game logic for that frame will play out. Since the display is monochrome with a limited number of shades, most of


## Other resources

A design document on the another similar project, with descriptions of game features on pixel level, along with links to sprites that will prove very useful. [Found here](https://static1.squarespace.com/static/555e5d9ae4b0b614ae7344b0/t/55679970e4b0e9e465255184/1432852848109/MissileCommand.pdf)

A description of missile command gameplay elements, like difficulty progression and control schemes. [Found here](https://arcadeclassics.net/80s-game-videos/missile-command/)

