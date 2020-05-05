cmake_minimum_required(VERSION 3.0.0)
project(discordbot VERSION 0.1.0)

find_package(OpenSSL REQUIRED)

include_directories("${PROJECT_SOURCE_DIR}/externals/IXWebSocket"
                    "${PROJECT_SOURCE_DIR}/externals/json"
                    "${PROJECT_SOURCE_DIR}/externals/Log"
                    "${PROJECT_SOURCE_DIR}/include"
                    "${OPENSSL_INCLUDE_DIR}")

#Import the websocket library.

ADD_LIBRARY(ixwebsocket STATIC IMPORTED)
SET_TARGET_PROPERTIES(ixwebsocket PROPERTIES IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/externals/IXWebSocket/lib/libixwebsocket.a")

set(SRCS
    "${PROJECT_SOURCE_DIR}/src/DiscordClient.cpp"
    "${PROJECT_SOURCE_DIR}/src/VoiceSocket.cpp"
    "${PROJECT_SOURCE_DIR}/src/controller/ICommand.cpp"
    "${PROJECT_SOURCE_DIR}/src/controller/IController.cpp")

add_executable(${PROJECT_NAME} main.cpp ${SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ixwebsocket sodium opus ${OPENSSL_CRYPTO_LIBRARIES} ${OPENSSL_SSL_LIBRARIES} ${OPENSSL_LIBRARIES} pthread z)