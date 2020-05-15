cmake_minimum_required(VERSION 3.0.0)
project(discordbot VERSION 0.1.0)

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set(USE_TLS ON)
set(USE_MBED_TLS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(${PROJECT_SOURCE_DIR}/externals/mbedtls)
add_subdirectory(${PROJECT_SOURCE_DIR}/externals/IXWebSocket)
add_subdirectory(${PROJECT_SOURCE_DIR}/externals/opus)

set(libsodium_src ${PROJECT_SOURCE_DIR}/externals/libsodium)

#TODO: Windows
if(NOT WIN32)
ExternalProject_Add(libsodium_build
                    SOURCE_DIR ${libsodium_src}
                    BINARY_DIR ${PROJECT_BINARY_DIR}
                    PATCH_COMMAND "${libsodium_src}/autogen.sh"
                    CONFIGURE_COMMAND "${libsodium_src}/configure" "--disable-pie" "--with-pic=\"yes\""
                    BUILD_COMMAND "make"
                    INSTALL_COMMAND ""
                    TEST_COMMAND "")
endif()

include_directories("${PROJECT_SOURCE_DIR}/externals/IXWebSocket"
                    "${PROJECT_SOURCE_DIR}/externals/CJSON"
                    "${PROJECT_SOURCE_DIR}/externals/CLog"
                    "${PROJECT_SOURCE_DIR}/include")

#Import the sodium library.
ADD_LIBRARY(sodium STATIC IMPORTED GLOBAL)
SET_TARGET_PROPERTIES(sodium PROPERTIES IMPORTED_LOCATION "${PROJECT_BINARY_DIR}/src/libsodium/.libs/${CMAKE_STATIC_LIBRARY_PREFIX}sodium${CMAKE_STATIC_LIBRARY_SUFFIX}")

set(SRCS
    "${PROJECT_SOURCE_DIR}/src/controller/DiscordClient.cpp"
    "${PROJECT_SOURCE_DIR}/src/controller/VoiceSocket.cpp"
    "${PROJECT_SOURCE_DIR}/src/controller/ICommand.cpp"
    "${PROJECT_SOURCE_DIR}/src/controller/IController.cpp"
    "${PROJECT_SOURCE_DIR}/src/controller/IMusicQueue.cpp")

add_library(${PROJECT_NAME} SHARED ${SRCS})
add_dependencies(${PROJECT_NAME} libsodium_build)
target_link_libraries(${PROJECT_NAME} mbedtls mbedx509 ixwebsocket opus sodium pthread z)