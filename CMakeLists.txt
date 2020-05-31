cmake_minimum_required(VERSION 3.3.0)
project(discordbot VERSION 2.0.0 LANGUAGES CXX)

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set(VERSION_SUFFIX "-alpha")

set(PROJECT_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(PROJECT_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(mbedtls_src ${PROJECT_SOURCE_DIR}/externals/mbedtls)

# Needed for IXWebSocket
set(IXWebSocket_src ${PROJECT_SOURCE_DIR}/externals/IXWebSocket)
set(MBEDCRYPTO_LIBRARY ${PROJECT_BINARY_DIR}/${LINK_SUB_DIR}${CMAKE_STATIC_LIBRARY_PREFIX}mbedcrypto${CMAKE_STATIC_LIBRARY_SUFFIX})
set(MBEDTLS_LIBRARY ${PROJECT_BINARY_DIR}/${LINK_SUB_DIR}${CMAKE_STATIC_LIBRARY_PREFIX}mbedtls${CMAKE_STATIC_LIBRARY_SUFFIX})
set(MBEDX509_LIBRARY ${PROJECT_BINARY_DIR}/${LINK_SUB_DIR}${CMAKE_STATIC_LIBRARY_PREFIX}mbedx509${CMAKE_STATIC_LIBRARY_SUFFIX})
set(MBEDTLS_LIBRARIES "${MBEDCRYPTO_LIBRARY};${MBEDTLS_LIBRARY};${MBEDX509_LIBRARY}")

set(PTHREAD "")
set(ZLIB_LIB "")
set(LINK_DIRS "")
            #     "${PROJECT_BINARY_DIR}/externals/mbedtls/library/${LINK_SUB_DIR}"
            #   "${PROJECT_BINARY_DIR}/externals/IXWebSocket/${LINK_SUB_DIR}"
            #   "${PROJECT_BINARY_DIR}/externals/IXWebSocket/third_party/zlib/${LINK_SUB_DIR}"
            #   "${PROJECT_BINARY_DIR}/externals/opus/${LINK_SUB_DIR}")

set(libsodium_src ${PROJECT_SOURCE_DIR}/externals/libsodium)

#Needed for opus because the check for this folder is relative to CMAKE_SOURCE_DIR.
file(COPY ${PROJECT_SOURCE_DIR}/externals/opus/cmake DESTINATION ${CMAKE_SOURCE_DIR}/)

set(BUILD_CMD_MBED ${CMAKE_MAKE_PROGRAM})
set(BUILD_CMD_WEBSOCKET ${CMAKE_MAKE_PROGRAM})
set(ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

if(MSVC)
	set(CMAKE_DEBUG_POSTFIX "d")
	set(BUILD_CMD_MBED msbuild /p:OutputPath=${PROJECT_BINARY_DIR} /p:OutDir=${PROJECT_BINARY_DIR} "${PROJECT_BINARY_DIR}/externals/mbedtls/mbed TLS.sln")
    set(BUILD_CMD_WEBSOCKET msbuild /p:OutputPath=${PROJECT_BINARY_DIR} /p:OutDir=${PROJECT_BINARY_DIR} "${PROJECT_BINARY_DIR}/externals/IXWebSocket/ixwebsocket.sln")
	
	add_definitions(-DSODIUM_STATIC=1 -DSODIUM_EXPORT=.)
endif(MSVC)

#Workaround for dependencies.
ExternalProject_Add(mbedtls_build
                    SOURCE_DIR ${mbedtls_src}
                    BINARY_DIR ${PROJECT_BINARY_DIR}/externals/mbedtls
                    CONFIGURE_COMMAND cmake ${mbedtls_src} -G ${CMAKE_GENERATOR} -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=${PROJECT_ARCHIVE_OUTPUT_DIRECTORY} -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${PROJECT_LIBRARY_OUTPUT_DIRECTORY} -DCMAKE_INSTALL_PREFIX=${PROJECT_BINARY_DIR} -DENABLE_PROGRAMS=OFF -DENABLE_TESTING=OFF -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_CONFIGURATION_TYPES=RELEASE
                    BUILD_COMMAND ${BUILD_CMD_MBED}
                    INSTALL_COMMAND ""
                    TEST_COMMAND "")

ExternalProject_Add(IXWebSocket_build
                    SOURCE_DIR ${IXWebSocket_src}
                    BINARY_DIR ${PROJECT_BINARY_DIR}/externals/IXWebSocket
                    CONFIGURE_COMMAND cmake ${IXWebSocket_src} -G ${CMAKE_GENERATOR} -DUSE_TLS=ON -DUSE_MBED_TLS=ON -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_CONFIGURATION_TYPES=RELEASE -DMBEDTLS_INCLUDE_DIRS=${mbedtls_src}/include -DMBEDCRYPTO_LIBRARY=${MBEDCRYPTO_LIBRARY} -DMBEDTLS_LIBRARY=${MBEDTLS_LIBRARY} -DMBEDX509_LIBRARY=${MBEDX509_LIBRARY} -DMBEDTLS_LIBRARIES=${MBEDTLS_LIBRARIES} -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=${PROJECT_ARCHIVE_OUTPUT_DIRECTORY} -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${PROJECT_LIBRARY_OUTPUT_DIRECTORY} -DCMAKE_INSTALL_PREFIX=${PROJECT_BINARY_DIR}
                    BUILD_COMMAND ${BUILD_CMD_WEBSOCKET}
                    INSTALL_COMMAND ""
                    TEST_COMMAND ""
                    DEPENDS mbedtls_build)

add_subdirectory(${PROJECT_SOURCE_DIR}/externals/opus EXCLUDE_FROM_ALL)

if(NOT WIN32)
	set(PTHREAD pthread)
	set(LINK_DIRS "${PROJECT_BINARY_DIR}/src/libsodium/.libs/")
	
	if (NOT ZLIB_FOUND)
	  find_package(ZLIB)
	endif()

	if (ZLIB_FOUND)
		set(ZLIB_LIB "z")
	else()
		set(ZLIB_LIB "${CMAKE_STATIC_LIBRARY_PREFIX}z${CMAKE_STATIC_LIBRARY_SUFFIX}")
    endif()
    
    ExternalProject_Add(libsodium_build
                        SOURCE_DIR ${libsodium_src}
                        BINARY_DIR ${PROJECT_BINARY_DIR}
                        PATCH_COMMAND "${libsodium_src}/autogen.sh"
                        CONFIGURE_COMMAND "${libsodium_src}/configure" "--disable-pie" "--with-pic=\"yes\""
                        BUILD_COMMAND "make"
                        INSTALL_COMMAND cp -TR "${PROJECT_BINARY_DIR}/src/libsodium/include/" "${libsodium_src}/src/libsodium/include/"
                        TEST_COMMAND "")
else(NOT WIN32)
	if(MSVC)
		set(LINK_DIRS ${LINK_DIRS} "${PROJECT_BINARY_DIR}")
		set(ZLIB_LIB "zlibstatic${CMAKE_STATIC_LIBRARY_SUFFIX}")
	
		ExternalProject_Add(libsodium_build
                    SOURCE_DIR ${libsodium_src}
                    BINARY_DIR ${PROJECT_BINARY_DIR}
                    CONFIGURE_COMMAND devenv /upgrade "${libsodium_src}/libsodium.sln"
                    BUILD_COMMAND msbuild /p:OutputPath=${PROJECT_BINARY_DIR} /p:OutDir=${PROJECT_BINARY_DIR} /p:Platform=x64 "${libsodium_src}/libsodium.sln"
                    INSTALL_COMMAND ""
                    TEST_COMMAND "")
	endif(MSVC)
endif(NOT WIN32)

message(${PROJECT_VERSION})

configure_file("${PROJECT_SOURCE_DIR}/include/config.h.in" config.h @ONLY) 
configure_file("${PROJECT_SOURCE_DIR}/docs/Doxyfile.in" Doxyfile @ONLY) 

include_directories("${PROJECT_BINARY_DIR}"
                    "${PROJECT_SOURCE_DIR}/externals/IXWebSocket"
                    "${PROJECT_SOURCE_DIR}/externals/CJSON"
                    "${PROJECT_SOURCE_DIR}/externals/CLog"
                    "${libsodium_src}/src/libsodium/include/"
                    "${PROJECT_SOURCE_DIR}/externals/opus/include"
                    "${PROJECT_SOURCE_DIR}/include")

link_directories(${PROJECT_BINARY_DIR}
                 ${LINK_DIRS})

set(SRCS
    "${PROJECT_SOURCE_DIR}/src/controller/DiscordClient.cpp"
    "${PROJECT_SOURCE_DIR}/src/controller/VoiceSocket.cpp"
    "${PROJECT_SOURCE_DIR}/src/controller/ICommand.cpp"
    "${PROJECT_SOURCE_DIR}/src/controller/IController.cpp"
    "${PROJECT_SOURCE_DIR}/src/controller/IMusicQueue.cpp"
    "${PROJECT_SOURCE_DIR}/src/controller/JSONCmdsConfig.cpp"
    "${PROJECT_SOURCE_DIR}/src/commands/RightsCommand.cpp"
    "${PROJECT_SOURCE_DIR}/src/commands/HelpCommand.cpp")

add_library(${PROJECT_NAME} SHARED ${SRCS})
add_dependencies(${PROJECT_NAME} IXWebSocket_build)
add_dependencies(${PROJECT_NAME} libsodium_build)

# https://stackoverflow.com/a/48214719
install(DIRECTORY "include/" # source directory
        DESTINATION "${CMAKE_INSTALL_PREFIX}/discordbot" # target directory
        FILES_MATCHING # install only matched files
        PATTERN "*.hpp" # select header files
)

install(FILES "${PROJECT_BINARY_DIR}/config.h" DESTINATION "${CMAKE_INSTALL_PREFIX}/discordbot")

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION}${VERSION_SUFFIX})
target_link_libraries(${PROJECT_NAME} libsodium${CMAKE_STATIC_LIBRARY_SUFFIX} ixwebsocket ${CMAKE_STATIC_LIBRARY_PREFIX}mbedtls${CMAKE_STATIC_LIBRARY_SUFFIX} ${CMAKE_STATIC_LIBRARY_PREFIX}mbedcrypto${CMAKE_STATIC_LIBRARY_SUFFIX} ${CMAKE_STATIC_LIBRARY_PREFIX}mbedx509${CMAKE_STATIC_LIBRARY_SUFFIX} ${ZLIB_LIB} opus ${PTHREAD})