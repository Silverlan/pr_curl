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

link_generic_module_libraries()
register_pragma_module(${PROJ_NAME})

add_subdirectory(third_party_libs/curl)

set_target_properties(curl PROPERTIES FOLDER third_party_libs/curl)
set_target_properties(libcurl PROPERTIES FOLDER third_party_libs/curl)

add_dependencies(pr_curl libcurl)
