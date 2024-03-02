cmake_minimum_required(VERSION 3.12)

include("./CMakeShared.txt")

set(PROJ_NAME pr_curl)

project(${PROJ_NAME} CXX)

# set(PRECOMPILED_HEADER "stdafx")

set(DEPENDENCY_CURL_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/third_party_libs/curl/include/curl/ CACHE PATH "Path to include directory.")


if(WIN32)
    search_lib_recursive(DEPENDENCY_CURL_LIBRARY third_party_libs/curl/lib/${BINARY_PRAGMA_DIR} libcurl_imp$<$<CONFIG:Debug>:-d>)
else()
    search_lib_recursive(DEPENDENCY_CURL_LIBRARY third_party_libs/curl/lib/${BINARY_PRAGMA_DIR} curl$<$<CONFIG:Debug>:-d>)
endif()

add_external_library(luajit)
if(WIN32)
	add_external_library(lua)
endif()




link_external_library(curl)

add_include_dir(glm)
set(DEPENDENCY_CURL_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/third_party_libs/curl/include CACHE PATH "" FORCE)

link_generic_module_libraries()
register_pragma_module(${PROJ_NAME})

if(WIN32)
	set(CURL_ZLIB OFF CACHE BOOL "Curl ZLib" FORCE)
else()
	set(CURL_USE_LIBSSH2 OFF CACHE BOOL "Curl SSH2" FORCE)
endif()
set(CURL_USE_LIBSSH OFF CACHE BOOL "Curl SSH" FORCE)
set(CURL_ZLIB OFF CACHE BOOL "Curl ZLib" FORCE)


if(WIN32)
	set(CURL_USE_OPENSSL OFF CACHE BOOL "Use OpenSSL code. Experimental" FORCE)
	set(CURL_USE_WINSSL OFF CACHE BOOL "enable Windows native SSL/TLS" FORCE) # Obsolete?
	set(CURL_USE_SCHANNEL ON CACHE BOOL "enable Windows native SSL/TLS" FORCE)
endif()
if(WIN32)
	set(CMAKE_DISABLE_FIND_PACKAGE_LibSSH2 ON CACHE BOOL "Curl SSH2" FORCE)
endif()
add_subdirectory(third_party_libs/curl)
if(WIN32)
	unset(CMAKE_DISABLE_FIND_PACKAGE_LibSSH2 CACHE)
endif()

set_target_properties(curl PROPERTIES FOLDER third_party_libs/curl)
set_target_properties(libcurl_shared PROPERTIES FOLDER third_party_libs/curl)

add_dependencies(pr_curl libcurl)
