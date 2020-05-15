cmake_minimum_required(VERSION 3.0.0)
project(discordbot VERSION 0.1.0)

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set(USE_TLS ON)
add_definitions(-DCMAKE_POSITION_INDEPENDENT_CODE=ON)

add_subdirectory(${PROJECT_SOURCE_DIR}/externals/IXWebSocket)
add_subdirectory(${PROJECT_SOURCE_DIR}/externals/opus)

set(libsodium_src ${PROJECT_SOURCE_DIR}/externals/libsodium)

if(NOT WIN32)
ExternalProject_Add(libsodium_build
                    SOURCE_DIR ${libsodium_src}
                    BINARY_DIR ${PROJECT_BINARY_DIR}
                    PATCH_COMMAND "${libsodium_src}/autogen.sh"
                    CONFIGURE_COMMAND "${libsodium_src}/configure"
                    BUILD_COMMAND "make"
                    INSTALL_COMMAND ""
                    TEST_COMMAND "")
endif()

find_package(OpenSSL REQUIRED)

include_directories("${PROJECT_SOURCE_DIR}/externals/IXWebSocket"
                    "${PROJECT_SOURCE_DIR}/externals/CJSON"
                    "${PROJECT_SOURCE_DIR}/externals/CLog"
                    "${PROJECT_SOURCE_DIR}/include"
                    "${OPENSSL_INCLUDE_DIR}")

#Import the websocket library.

# ADD_LIBRARY(ixwebsocket STATIC IMPORTED)
# SET_TARGET_PROPERTIES(ixwebsocket PROPERTIES IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/externals/IXWebSocket/lib/libixwebsocket.a")

set(SRCS
    "${PROJECT_SOURCE_DIR}/src/controller/DiscordClient.cpp"
    "${PROJECT_SOURCE_DIR}/src/controller/VoiceSocket.cpp"
    "${PROJECT_SOURCE_DIR}/src/controller/ICommand.cpp"
    "${PROJECT_SOURCE_DIR}/src/controller/IController.cpp"
    "${PROJECT_SOURCE_DIR}/src/controller/IMusicQueue.cpp")


add_library(${PROJECT_NAME} SHARED ${SRCS})
add_dependencies(${PROJECT_NAME} libsodium_build)
# add_executable(${PROJECT_NAME} main.cpp ${SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${ixwebsocket} ${Opus} ${OPENSSL_CRYPTO_LIBRARIES} ${OPENSSL_SSL_LIBRARIES} ${OPENSSL_LIBRARIES} pthread z)