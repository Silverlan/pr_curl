cmake_minimum_required(VERSION 3.12)

include("../CMakeShared.txt")

set(PROJ_NAME pr_curl)

project(${PROJ_NAME} CXX)

# set(PRECOMPILED_HEADER "stdafx")

link_external_library(iclient)
link_external_library(curl)

link_generic_module_libraries()
register_pragma_module(${PROJ_NAME})

add_subdirectory(third_party_libs/curl)

set_target_properties(curl PROPERTIES FOLDER third_party_libs/curl)
set_target_properties(libcurl PROPERTIES FOLDER third_party_libs/curl)

add_dependencies(pr_curl libcurl)
