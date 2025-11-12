include(${CMAKE_SOURCE_DIR}/cmake/pr_common.cmake)

set(PROJ_NAME pr_curl)
pr_add_module(${PROJ_NAME} SHARED "modules/curl/")

pr_add_dependency(${PROJ_NAME} ishared TARGET PUBLIC)

pr_init_module(${PROJ_NAME})

# Curl
set(CURL_USE_LIBSSH2 OFF CACHE BOOL "Curl SSH2" FORCE)
set(CURL_USE_LIBSSH OFF CACHE BOOL "Curl SSH" FORCE)
set(CURL_ZLIB OFF CACHE BOOL "Curl ZLib" FORCE)

if(WIN32)
	set(CURL_USE_OPENSSL OFF CACHE BOOL "Use OpenSSL code. Experimental" FORCE)
	set(CURL_USE_WINSSL OFF CACHE BOOL "enable Windows native SSL/TLS" FORCE) # Obsolete?
	set(CURL_USE_SCHANNEL ON CACHE BOOL "enable Windows native SSL/TLS" FORCE)
	set(CMAKE_DISABLE_FIND_PACKAGE_LibSSH2 ON CACHE BOOL "Curl SSH2" FORCE)
endif()
# Make sure curl doesn't pick up zlib
set(_old_zlib_found ${ZLIB_FOUND})
set(ZLIB_FOUND FALSE)
add_subdirectory(third_party_libs/curl)
set(ZLIB_FOUND ${_old_zlib_found})

set(IDE_FOLDER "modules/curl/")
pr_set_target_folder(libcurl_object "${IDE_FOLDER}")
pr_set_target_folder(libcurl_static "${IDE_FOLDER}")
pr_set_target_folder(curl_uninstall "${IDE_FOLDER}")
pr_set_target_folder(curltool "${IDE_FOLDER}")
pr_set_target_folder(curlu "${IDE_FOLDER}")

set_target_properties(curl PROPERTIES FOLDER third_party_libs/curl)
set_target_properties(libcurl_shared PROPERTIES FOLDER third_party_libs/curl)

pr_add_dependency(${PROJ_NAME} libcurl_shared TARGET)

pr_finalize(${PROJ_NAME} FOLDER "${IDE_FOLDER}")
