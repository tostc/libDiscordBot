# Work In Progress (WIP)
# libDiscordBot
libDiscordBot is an easy-to-use C++ Discord bot library that you can use to quickly create a bot. It's inspired by [Discord.Net](https://github.com/discord-net/Discord.Net) for C#.

[Official Discord Server](https://discord.gg/4yu7uR2)

![Discord Logo](https://discord.com/assets/fc0b01fe10a0b8c602fb0106d8189d9b.png)

## Feeling adventurous?
  You can clone the development branch if you want the latest and probably buggy stuff. This is not recommended for productivity programs.

## Features
* Send and read messages
* Stream music (it has a playlist API)
* Implementing commands is now way easier!

### Prerequisites
First you need following software and libraries.

* CMake
* C++ compiler which supports C++14
    * Compiles with clang and gcc under Linux.
    * Compiles with Visual Studio 2019 under Windows.

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

### First bot

Please visit the [wiki page](https://github.com/tostc/libDiscordBot/wiki/Your-first-bot).

## Checklist
- [ ] Administration
- [ ] Rights management(with Roles)
- [ ] Wrap all Discord objects
- [ ] Wrap all Discord events

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
