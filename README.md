# JFK Mario World
A remake/fangame of Super Mario World in C++/C. Sort of an engine for making mario games that need to be based off super mario world.

The code hosted here contains no copyrighted assets or any stolen code. Most things have been reverse engineered and recoded completely from the original game, while also trying to keep compatibility with custom resources made for the original game.

## The features?
* Fully working engine with tons of features.
* Physics reverse engineered and recoded from the original game to be as close as possible to the original (except collision, which was recoded entirely).
* Compatibility with custom resources made for the original Super Mario World, except levels made with Lunar Magic.
* Unfinished, but working ASM 65c18 processor/emulator for running ASM scripts.
* Coding similar to making resources for the original game, except with remapped addresses and different logic to work with.
* Lua Support for scripting levels and sprites.
* Selling Feature : Multiplayer. Play the game with other people! This is a unstable and not fully tested feature though.
* A level editor for making levels for the game similar to Lunar Magic, Solar Energy (https://github.com/johnfortnitekennedy/SolarEnergy)
* Almost no limitations, 128 sprites on screen at once without any FPS drops.
* 4bpp/2bpp GFX rendering, OAM with more features like scaling, rotation, size, all palette rows.

## How can I help?
If you know how to code, submit fixes. If you like to test stuff, make levels and test the craziest stuff you can think of and try to find bugs, then report them in issues. Everything helps! We especially need help on making the game physics accurate. Also if a multiplayer server is up, help us test the netcode by just playing the game.

## Requirements / Dependencies
* SDL2
* SDL mixer X (or just SDL Mixer for linux users, linux users don't get SPC playback)
* SDL_image
* SFML
* Lua 5.3

## Social
Get releases of the game and contact developers at https://discord.gg/6zwZxH6

## Videos
* Custom Level testing in multiplayer https://www.youtube.com/watch?v=EYXP11JxQpc
* A level made for the game https://www.youtube.com/watch?v=xXrQK5AH2sQ