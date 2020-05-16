# libDiscordBot
Simple to use Discord bot library to create fast a bot. (WIP)

## Note
This is the development version of the library. There are bugs unstable API and maybe the project won't build.

### Prerequisites
First you need following softwares and librarys.

- cmake
- C++ compiler which supports C++14

## Getting Started
These instructions will help you compile and use the library to build your own bot.

1. Cloning the repository:

```git clone --recurse-submodules -j8 -b development --single-branch https://github.com/tostc/libDiscordBot.git```

2. Building the project:
```cd ProjectRoot
mkdir build
cd build/
cmake ..
```

## Libs
- [opus](https://github.com/xiph/opus)
- [sodium](https://github.com/jedisct1/libsodium)
- [IXWebSocket](https://github.com/machinezone/IXWebSocket)
- [CJSON](https://github.com/tostc/CJSON)
- [CLog](https://github.com/tostc/CLog)
- [mbedtls](https://github.com/ARMmbed/mbedtls)

## License
This library is under the [MIT License](LICENSE.txt)
