cmake_minimum_required(VERSION 3.3.0)
project(discordbot VERSION 0.1.0)

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(mbedtls_src ${PROJECT_SOURCE_DIR}/externals/mbedtls)

# Needed for IXWebSocket
set(IXWebSocket_src ${PROJECT_SOURCE_DIR}/externals/IXWebSocket)
set(MBEDCRYPTO_LIBRARY ${PROJECT_BINARY_DIR}/externals/mbedtls/library/libmbedcrypto.a)
set(MBEDTLS_LIBRARY ${PROJECT_BINARY_DIR}/externals/mbedtls/library/libmbedtls.a)
set(MBEDX509_LIBRARY ${PROJECT_BINARY_DIR}/externals/mbedtls/library/libmbedx509.a)
set(MBEDTLS_LIBRARIES "${MBEDCRYPTO_LIBRARY};${MBEDTLS_LIBRARY};${MBEDX509_LIBRARY}")

set(libsodium_src ${PROJECT_SOURCE_DIR}/externals/libsodium)

#Needed for opus because the check for this folder is relative to CMAKE_SOURCE_DIR.
file(COPY ${PROJECT_SOURCE_DIR}/externals/opus/cmake DESTINATION ${CMAKE_SOURCE_DIR}/)

#Workaround for dependencies.
ExternalProject_Add(mbedtls_build
                    SOURCE_DIR ${mbedtls_src}
                    BINARY_DIR ${PROJECT_BINARY_DIR}/externals/mbedtls
                    CONFIGURE_COMMAND cmake ${mbedtls_src} -G ${CMAKE_GENERATOR} -DCMAKE_POSITION_INDEPENDENT_CODE=ON
                    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
                    INSTALL_COMMAND ""
                    TEST_COMMAND "")

ExternalProject_Add(IXWebSocket_build
                    SOURCE_DIR ${IXWebSocket_src}
                    BINARY_DIR ${PROJECT_BINARY_DIR}/externals/IXWebSocket
                    CONFIGURE_COMMAND cmake ${IXWebSocket_src} -G ${CMAKE_GENERATOR} -DUSE_TLS=ON -DUSE_MBED_TLS=ON -DMBEDTLS_INCLUDE_DIRS=${mbedtls_src}/include -DMBEDCRYPTO_LIBRARY=${MBEDCRYPTO_LIBRARY} -DMBEDTLS_LIBRARY=${MBEDTLS_LIBRARY} -DMBEDX509_LIBRARY=${MBEDX509_LIBRARY} -DMBEDTLS_LIBRARIES=${MBEDTLS_LIBRARIES}
                    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
                    INSTALL_COMMAND ""
                    TEST_COMMAND ""
                    DEPENDS mbedtls_build)

add_subdirectory(${PROJECT_SOURCE_DIR}/externals/opus)

#TODO: Windows
if(NOT WIN32)
    ExternalProject_Add(libsodium_build
                        SOURCE_DIR ${libsodium_src}
                        BINARY_DIR ${PROJECT_BINARY_DIR}
                        PATCH_COMMAND "${libsodium_src}/autogen.sh"
                        CONFIGURE_COMMAND "${libsodium_src}/configure" "--disable-pie" "--with-pic=\"yes\""
                        BUILD_COMMAND "make"
                        INSTALL_COMMAND cp -TR "${PROJECT_BINARY_DIR}/src/libsodium/include/" "${libsodium_src}/src/libsodium/include/"
                        TEST_COMMAND "")
endif()

set(ZLIB_LIB "")

if (NOT ZLIB_FOUND)
  find_package(ZLIB)
endif()

if (ZLIB_FOUND)
    set(ZLIB_LIB "z")
else()
    set(ZLIB_LIB "libz.a")
endif()


include_directories("${PROJECT_SOURCE_DIR}/externals/IXWebSocket"
                    "${PROJECT_SOURCE_DIR}/externals/CJSON"
                    "${PROJECT_SOURCE_DIR}/externals/CLog"
                    "${libsodium_src}/src/libsodium/include/"
                    "${PROJECT_SOURCE_DIR}/externals/opus/include"
                    "${PROJECT_SOURCE_DIR}/include")

link_directories("${PROJECT_BINARY_DIR}/src/libsodium/.libs/"
                 "${PROJECT_BINARY_DIR}/externals/mbedtls/library/"
                 "${PROJECT_BINARY_DIR}/externals/IXWebSocket/"
                 "${PROJECT_BINARY_DIR}/externals/IXWebSocket/third_party/zlib/"
                 "${PROJECT_BINARY_DIR}/externals/opus/")

set(SRCS
    "${PROJECT_SOURCE_DIR}/src/controller/DiscordClient.cpp"
    "${PROJECT_SOURCE_DIR}/src/controller/VoiceSocket.cpp"
    "${PROJECT_SOURCE_DIR}/src/controller/ICommand.cpp"
    "${PROJECT_SOURCE_DIR}/src/controller/IController.cpp"
    "${PROJECT_SOURCE_DIR}/src/controller/IMusicQueue.cpp")

add_library(${PROJECT_NAME} SHARED ${SRCS})
add_dependencies(${PROJECT_NAME} IXWebSocket_build)
add_dependencies(${PROJECT_NAME} libsodium_build)
target_link_libraries(${PROJECT_NAME} ixwebsocket libmbedtls.a libmbedcrypto.a libmbedx509.a ${ZLIB_LIB} opus libsodium.a pthread)