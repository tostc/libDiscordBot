cmake_minimum_required(VERSION 3.3.0)
project(YOURPROJECTNAME VERSION 0.1.0)

include_directories("${CMAKE_SOURCE_DIR}/include"
                    "${CMAKE_SOURCE_DIR}/external/include")

link_directories("${CMAKE_SOURCE_DIR}/external/lib")

add_executable(${PROJECT_NAME} main.cpp src/BotController.cpp src/HelloCommand.cpp)
target_link_libraries(${PROJECT_NAME} discordbot)