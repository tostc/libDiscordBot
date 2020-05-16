# Work In Progress (WIP)
# libDiscordBot
LibDiscordBot is an easy-to-use C++ Discord bot library that you can use to quickly create a bot. 

Discord server: https://discord.gg/KDa6MKy

# Read me!
This is the development version of the library. You might encounter bugs/errors, the API is probably broken, and the project might not build.

## Features
* Send and read messages
* Stream music (it has a playlist API)
* Implementing commands is now way easier!

### Prerequisites
First you need following software and libraries.

* Cmake
* Any C++ compiler which supports C++14

## Getting Started
These instructions will help you compile and use the library to build your own bot.

1. Cloning the repository:

```
git clone --recurse-submodules -j8 -b development --single-branch https://github.com/tostc/libDiscordBot.git
```

2. Building the project:
```
cd libDiscordBot/
mkdir build
cd build/
cmake ../
```

## Checklist
- [ ] Role Management
- [ ] Message Management
- [ ] Events

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
