cmake_minimum_required(VERSION 3.12)

include("./CMakeShared.txt")

set(PROJ_NAME pr_curl)

project(${PROJ_NAME} CXX)

# set(PRECOMPILED_HEADER "stdafx")

set(DEPENDENCY_CURL_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/third_party_libs/curl/include/curl/ CACHE PATH "Path to include directory.")


if(WIN32)
    search_lib_recursive(DEPENDENCY_CURL_LIBRARY third_party_libs/curl/lib/${BINARY_PRAGMA_DIR} libcurl_imp)
else()
    search_lib_recursive(DEPENDENCY_CURL_LIBRARY third_party_libs/curl/lib/${BINARY_PRAGMA_DIR} curl)
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

set(CURL_ZLIB OFF CACHE BOOL "Curl ZLib" FORCE)
set(CURL_USE_LIBSSH OFF CACHE BOOL "Curl SSH" FORCE)
if(WIN32)
	set(CMAKE_USE_OPENSSL OFF CACHE BOOL "Use OpenSSL code. Experimental" FORCE)
	set(CMAKE_USE_WINSSL ON CACHE BOOL "enable Windows native SSL/TLS" FORCE)
endif()
set(CMAKE_DISABLE_FIND_PACKAGE_LibSSH2 ON CACHE BOOL "Curl SSH2" FORCE)
add_subdirectory(third_party_libs/curl)
unset(CMAKE_DISABLE_FIND_PACKAGE_LibSSH2 CACHE)

set_target_properties(curl PROPERTIES FOLDER third_party_libs/curl)
set_target_properties(libcurl PROPERTIES FOLDER third_party_libs/curl)

add_dependencies(pr_curl libcurl)
