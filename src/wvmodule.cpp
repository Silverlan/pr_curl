// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <curl/curl.h>
#include <string>
#include <pragma/pragma_module.hpp>



extern "C" {

PRAGMA_EXPORT bool wv_curl_download_file(const std::string &url,size_t(*fData)(void*,size_t,size_t,void*),void *userData,std::string &err)
{
	auto *curl = curl_easy_init();
	if(curl == nullptr)
		return false;
	curl_easy_setopt(curl,CURLOPT_URL,url.c_str());
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,fData);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,userData);
	auto res = curl_easy_perform(curl);
	if(res != CURLE_OK)
		err = curl_easy_strerror(res);
	curl_easy_cleanup(curl);
	return true;
}

};
