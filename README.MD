# Read me

You are currently on the development branch of the libDiscordBot library. Keep in mind that you can encounter bugs/errors, the API isn't at this point fixed and/or the project might not build.

# libDiscordBot
libDiscordBot is an easy-to-use C++ library which helps you to create quickly a Discord bot. It's inspired by [Discord.Net](https://github.com/discord-net/Discord.Net) for C#.

Need help? Feel free to join our Discord Server.

[![Discord Server](https://discordapp.com/api/guilds/711169228320931840/embed.png?style=banner2)](https://discord.gg/C79vGZb)

If you like this library please give me a ![star](star.png)

## Feeling adventurous?
  You can clone the development branch if you want the latest and probably buggy stuff. This is not recommended for productivity programs.

## Features
- Built In help command
- Built In rights management system for commands.
- Audio streaming
- Playlist API
- Easy to use API
- Member and Guild management

### Prerequisites
First you need following software and libraries.

* CMake
* C++ compiler which supports C++14
    * Compiles with clang and gcc under Linux.
    * Compiles with Visual Studio 2019 under Windows.
* Maybe runs flawless on the Raspberry Pi (not Tested)
    * For more informations please visit [this](https://github.com/tostc/libDiscordBot/issues/9#issue-715171899) issue, and look under `Compiling the bot with the lib:`

## Getting Started
These instructions will help you compile and use the library to build your own bot.

1. Cloning the repository:

```
git clone --recurse-submodules -j8 --single-branch https://github.com/tostc/libDiscordBot.git
```
side note: if you want to clone a specfic branch, you can include `-b *name of the branch you want to clone*` anywhere in the command (obviously, only after `git clone`).

2. Building the project:

```
cd libDiscordBot/
mkdir build
cd build/
cmake ../
```

For Linux execute following command inside the build directory
```
make
```

For Windows open the *.sln file inside the build directory

3. Copy and rename the project_template directory somewhere to get starting. Follow the introduction inside the README.MD of the template.

## Crosscompiling

To crosscompile the library there are toolchain files inside the crosscompile folder.
How to use

1. Do the same steps (1 - 2) as describe above except the `cmake ../` step.
2. The current way to set the root of your compiler toolchain is currently sadly to insert the path inside the `crosscompile/TOOLCHAINFILE.cmake` file as ROOT_PATH.
4. Call cmake with the following parameters.
```
cmake ../ -DCMAKE_TOOLCHAIN_FILE=../crosscompile/TOOLCHAINFILE.cmake
```
5. You can now compile your programm.

### First bot

Please visit the [wiki page](https://github.com/tostc/libDiscordBot/wiki/Your-first-bot).

## Checklist
- [ ] Administration
- [x] Rights management(with Roles)
- [ ] Wrap all Discord objects
- [ ] Wrap all Discord events

## Applications which uses libDiscordBot

Send a pull request or open an issue if you like to add your application here.

- [NightcoreBot](https://github.com/tostc/NightcoreBot) - Discord music bot which turns all music to nightcore. Perfect for all nighcore lovers. 

## Libraries
* There's no need to manually install each of these if you used the command in the "Cloning the repository: " step.
- [opus](https://github.com/xiph/opus)
- [sodium](https://github.com/jedisct1/libsodium)
- [IXWebSocket](https://github.com/machinezone/IXWebSocket)
- [CJSON](https://github.com/tostc/CJSON)
- [CLog](https://github.com/tostc/CLog)
- [mbedtls](https://github.com/ARMmbed/mbedtls)

## License
This library is under the [MIT License](LICENSE.txt)
