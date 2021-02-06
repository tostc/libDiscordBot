cmake_minimum_required(VERSION 3.3.0)
project(discordbot VERSION 2.2.3 LANGUAGES CXX)

#----------------------------Setup any needed variable and include any needed module.----------------------------#

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set(VERSION_SUFFIX "-beta")

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

set(ADDITIONAL_LIBS "")
set(ZLIB_LIB "")
set(ZLIB_ROOT ${PROJECT_BINARY_DIR}/zlib) # IXWebsocket doesn't deliver zlib anymore, so this is the new build path.
set(ZLIB_PROJECT_ROOT ${PROJECT_SOURCE_DIR}/externals/zlib-1.2.11)
set(LINK_DIRS "")
set(ZLIB_BINARY_DIR ${PROJECT_BINARY_DIR}/externals/zlib-1.2.11)

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
	
	add_definitions(-DSODIUM_STATIC=1 -DSODIUM_EXPORT=. -DDLL_BUILD)
endif(MSVC)

set(SKIP_INSTALL_LIBRARIES on)
set(SKIP_INSTALL_HEADERS on)
set(SKIP_INSTALL_FILES on)
set(SKIP_INSTALL_ALL on)

file(MAKE_DIRECTORY ${ZLIB_ROOT}/include)
file(MAKE_DIRECTORY ${ZLIB_ROOT}/lib)

set(TOOLCHAIN_PARAM "")
set(EXTERNAL_ENV "")
set(EXTERNAL_HOST_PARAM "")
if(CMAKE_CROSSCOMPILING)
  get_filename_component(TOOLCHAIN "${CMAKE_TOOLCHAIN_FILE}"
                        REALPATH BASE_DIR "${PROJECT_BINARY_DIR}")

  set(TOOLCHAIN_PARAM "-DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN}")
  set(EXTERNAL_ENV ${CMAKE_COMMAND} -E env PATH=${ROOT_PATH}/bin:$ENV{PATH})
  set(EXTERNAL_HOST_PARAM --host=${HOST_NAME})
endif()

#----------------------------Create build targets----------------------------#

add_subdirectory(${ZLIB_PROJECT_ROOT})
                              
if(UNIX)
  set(ZLIB_LIBS ${ZLIB_BINARY_DIR}/libz.a)
else(UNIX)
  set(ZLIB_LIBS ${ZLIB_BINARY_DIR}/Release/zlibstatic.lib)
endif(UNIX)

add_custom_target(zlib_copy ALL
                  COMMAND ${CMAKE_COMMAND} -E copy ${ZLIB_LIBS} ${ZLIB_ROOT}/lib
                  COMMAND ${CMAKE_COMMAND} -E copy ${ZLIB_PROJECT_ROOT}/zlib.h ${ZLIB_BINARY_DIR}/zconf.h ${ZLIB_ROOT}/include
                  DEPENDS zlibstatic)

#Workaround for dependencies.
ExternalProject_Add(mbedtls_build
                    SOURCE_DIR ${mbedtls_src}
                    BINARY_DIR ${PROJECT_BINARY_DIR}/externals/mbedtls
                    CONFIGURE_COMMAND ${CMAKE_COMMAND} ${mbedtls_src} -G ${CMAKE_GENERATOR} -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=${PROJECT_ARCHIVE_OUTPUT_DIRECTORY} -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${PROJECT_LIBRARY_OUTPUT_DIRECTORY} -DCMAKE_INSTALL_PREFIX=${PROJECT_BINARY_DIR} -DENABLE_PROGRAMS=OFF -DENABLE_TESTING=OFF -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_CONFIGURATION_TYPES=RELEASE ${TOOLCHAIN_PARAM}
                    BUILD_COMMAND ${BUILD_CMD_MBED}
                    INSTALL_COMMAND ""
                    TEST_COMMAND "")

ExternalProject_Add(IXWebSocket_build
                    SOURCE_DIR ${IXWebSocket_src}
                    BINARY_DIR ${PROJECT_BINARY_DIR}/externals/IXWebSocket
                    CONFIGURE_COMMAND ${CMAKE_COMMAND} ${IXWebSocket_src} -G ${CMAKE_GENERATOR} -DZLIB_ROOT=${ZLIB_ROOT} -DUSE_TLS=ON -DUSE_MBED_TLS=ON -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_CONFIGURATION_TYPES=RELEASE -DMBEDTLS_INCLUDE_DIRS=${mbedtls_src}/include -DMBEDCRYPTO_LIBRARY=${MBEDCRYPTO_LIBRARY} -DMBEDTLS_LIBRARY=${MBEDTLS_LIBRARY} -DMBEDX509_LIBRARY=${MBEDX509_LIBRARY} -DMBEDTLS_LIBRARIES=${MBEDTLS_LIBRARIES} -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=${PROJECT_ARCHIVE_OUTPUT_DIRECTORY} -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${PROJECT_LIBRARY_OUTPUT_DIRECTORY} -DCMAKE_INSTALL_PREFIX=${PROJECT_BINARY_DIR} ${TOOLCHAIN_PARAM}
                    BUILD_COMMAND ${BUILD_CMD_WEBSOCKET}
                    INSTALL_COMMAND ""
                    TEST_COMMAND ""
                    DEPENDS mbedtls_build
                    DEPENDS zlib_copy)

# There is an issue on arm processors for opus (https://github.com/xiph/opus/issues/203)                    
if(CMAKE_SYSTEM_PROCESSOR MATCHES "(arm|aarch64)")
  set(OPUS_DISABLE_INTRINSICS ON CACHE BOOL "")
endif()

add_subdirectory(${PROJECT_SOURCE_DIR}/externals/opus EXCLUDE_FROM_ALL)

set(SRCS "")

if(NOT WIN32)
	set(ADDITIONAL_LIBS pthread)
	set(LINK_DIRS "${PROJECT_BINARY_DIR}/src/libsodium/.libs/")

  set(ZLIB_LIB "${ZLIB_ROOT}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}z${CMAKE_STATIC_LIBRARY_SUFFIX}")
    
  ExternalProject_Add(libsodium_build
                      SOURCE_DIR ${libsodium_src}
                      BINARY_DIR ${PROJECT_BINARY_DIR}
                      PATCH_COMMAND "${libsodium_src}/autogen.sh"
                      CONFIGURE_COMMAND ${EXTERNAL_ENV} "${libsodium_src}/configure" "--disable-pie" --with-pic="yes" ${EXTERNAL_HOST_PARAM}
                      BUILD_COMMAND ${EXTERNAL_ENV} "make"
                      INSTALL_COMMAND cp -TR "${PROJECT_BINARY_DIR}/src/libsodium/include/" "${libsodium_src}/src/libsodium/include/"
                      TEST_COMMAND "")
else(NOT WIN32)
	if(MSVC)
		set(LINK_DIRS ${LINK_DIRS} "${PROJECT_BINARY_DIR}")
    set(ZLIB_LIB "zlibstatic${CMAKE_STATIC_LIBRARY_SUFFIX}")
    set(ADDITIONAL_LIBS "crypt32")
	
		ExternalProject_Add(libsodium_build
                    SOURCE_DIR ${libsodium_src}
                    BINARY_DIR ${PROJECT_BINARY_DIR}
                    CONFIGURE_COMMAND devenv /upgrade "${libsodium_src}/libsodium.sln"
                    BUILD_COMMAND msbuild /p:OutputPath=${PROJECT_BINARY_DIR} /p:OutDir=${PROJECT_BINARY_DIR} /p:Platform=x64 "${libsodium_src}/libsodium.sln"
                    INSTALL_COMMAND ""
                    TEST_COMMAND "")
	endif(MSVC)
	
	configure_file("${PROJECT_SOURCE_DIR}/version.rc.in" version.rc @ONLY)
	set(SRCS "${CMAKE_CURRENT_BINARY_DIR}/version.rc")

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
	  ${SRCS}
    "${PROJECT_SOURCE_DIR}/src/controller/DiscordClient.cpp"
    "${PROJECT_SOURCE_DIR}/src/controller/VoiceSocket.cpp"
    "${PROJECT_SOURCE_DIR}/src/controller/ICommand.cpp"
    "${PROJECT_SOURCE_DIR}/src/controller/IController.cpp"
    "${PROJECT_SOURCE_DIR}/src/controller/IMusicQueue.cpp"
    "${PROJECT_SOURCE_DIR}/src/controller/JSONCmdsConfig.cpp"
    "${PROJECT_SOURCE_DIR}/src/controller/GuildAdmin.cpp"
    "${PROJECT_SOURCE_DIR}/src/commands/RightsCommand.cpp"
    "${PROJECT_SOURCE_DIR}/src/commands/HelpCommand.cpp"
    "${PROJECT_SOURCE_DIR}/src/commands/PrefixCommand.cpp")

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
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION}${VERSION_SUFFIX})
target_link_libraries(${PROJECT_NAME} libsodium${CMAKE_STATIC_LIBRARY_SUFFIX} ixwebsocket ${CMAKE_STATIC_LIBRARY_PREFIX}mbedtls${CMAKE_STATIC_LIBRARY_SUFFIX} ${CMAKE_STATIC_LIBRARY_PREFIX}mbedcrypto${CMAKE_STATIC_LIBRARY_SUFFIX} ${CMAKE_STATIC_LIBRARY_PREFIX}mbedx509${CMAKE_STATIC_LIBRARY_SUFFIX} zlibstatic opus ${ADDITIONAL_LIBS})
