/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "curl_handler.hpp"
#include <luainterface.hpp>
#include <luasystem.h>
#include <pragma/engine.h>
#include <pragma/pragma_module.hpp>
#include <pragma/lua/ldefinitions.h>
#include <pragma/lua/libraries/lfile.h>
#include <sharedutils/util_parallel_job.hpp>

extern DLLENGINE Engine *engine;

#pragma optimize("",off)
class CurlRequest
	: public util::ParallelWorker<const std::string&>
{
public:
	CurlRequest(const std::string &url,const std::unordered_map<std::string,std::string> &postValues);

	virtual const std::string &GetResult() override {return m_result;}
private:
	std::string m_result;
	std::shared_ptr<CurlHandler> m_curlHandler = nullptr;
};

CurlRequest::CurlRequest(const std::string &url,const std::unordered_map<std::string,std::string> &postValues)
{
	m_curlHandler = std::make_shared<CurlHandler>();
	AddThread([this,url,postValues]() {
		std::atomic<double> progress = 0.0;
		int32_t resultCode = -1;
		std::string result;
		std::atomic<bool> complete = false;
		m_curlHandler->SetErrorHandler([](CurlHandler::ResultCode resultCode) {
			// TODO
		});
		m_curlHandler->AddRequest(url,postValues,[&resultCode,&result,&complete](int32_t c,const std::string &r) {
			resultCode = c;
			result = r;
			complete = true;
		},[&progress](int64_t dltotal,int64_t dlnow,int64_t ultotal,int64_t ulnow) {
			if(dltotal > 0)
				progress = dlnow /static_cast<double>(dltotal);
		});
		m_curlHandler->StartDownload();
		while(complete == false && IsCancelled() == false)
		{
			UpdateProgress(progress);
			std::this_thread::sleep_for(std::chrono::milliseconds{250}); // TODO: Use a condition variable instead
		}
		if(IsCancelled())
		{
			m_curlHandler->CancelDownload();
			SetStatus(util::JobStatus::Cancelled);
		}
		else
		{
			if(resultCode != 0)
				SetStatus(util::JobStatus::Failed,"Result code " +std::to_string(resultCode));
			else
			{
				UpdateProgress(1.f);
				SetStatus(util::JobStatus::Successful);
				m_result = result;
			}
		}
	});
}

#include <iostream>
static void register_lua_library(Lua::Interface &l)
{
	Lua::RegisterLibrary(l.GetState(),"curl",{
		/*{"create_instance",static_cast<int32_t(*)(lua_State*)>([](lua_State *l) -> int32_t {
			auto curlHandler = std::make_shared<CurlHandler>();
			Lua::Push(l,curlHandler);
			return 1;
		})},*/
		{"request",static_cast<int32_t(*)(lua_State*)>([](lua_State *l) -> int32_t {
			std::string url = Lua::CheckString(l,1);
			Lua::CheckTable(l,2);
			luabind::object t {luabind::from_stack{l,2}};
			std::unordered_map<std::string,std::string> postValues {};
			for(auto it=luabind::iterator{t},end=luabind::iterator{};it!=end;++it)
			{
				auto key = luabind::object_cast_nothrow<std::string>(it.key(),std::string{});
				auto val = luabind::object_cast_nothrow<std::string>(*it,std::string{});
				postValues.insert(std::make_pair(key,val));
			}
			auto job = util::create_parallel_job<CurlRequest>(url,postValues);
			Lua::Push(l,job);
			return 1;
		})}
	});

	auto &modCurl = l.RegisterLibrary("curl");
	auto classDefCurl = luabind::class_<CurlHandler>("Instance");
#if 0
	classDefCurl.def("AddRequest",static_cast<void(*)(lua_State*,CurlHandler&,const std::string&,luabind::table<>,luabind::function<void>,luabind::function<void>)>(
		[](lua_State *l,CurlHandler &curlHandler,const std::string &url,luabind::table<> lPostValues,luabind::function<void> lOnComplete,luabind::function<void> lProgressCallback) {
			std::unordered_map<std::string,std::string> postValues {};
			for(auto it=luabind::iterator{lPostValues},end=luabind::iterator{};it!=end;++it)
			{
				auto key = luabind::object_cast_nothrow<std::string>(it.key(),std::string{});
				auto val = luabind::object_cast_nothrow<std::string>(*it,std::string{});
				postValues.insert(std::make_pair(key,val));
			}
			std::function<void(int32_t,const std::string&)> onComplete = [lOnComplete](int32_t code,const std::string &result) mutable {
				std::cout<<"OnComplete: "<<code<<","<<result<<std::endl;
				//lOnComplete(code,result);
			};
			std::function<void(int64_t,int64_t,int64_t,int64_t)> progressCallback = [lProgressCallback](int64_t dltotal,int64_t dlnow,int64_t ultotal,int64_t ulnow) mutable {
				std::cout<<"Progress: "<<dltotal<<","<<dlnow<<","<<ultotal<<","<<ulnow<<std::endl;
				//lProgressCallback(dltotal,dlnow,ultotal,ulnow);
			};
			curlHandler.SetErrorHandler([](CurlHandler::ResultCode result) {
				std::cout<<"Result: "<<umath::to_integral(result)<<std::endl;
			});
			curlHandler.AddRequest(url,postValues,onComplete,progressCallback);
		}
	));
#endif
	classDefCurl.def("AddRequest",static_cast<void(*)(lua_State*,CurlHandler&,const std::string&)>(
		[](lua_State *l,CurlHandler &curlHandler,const std::string &url) {
			std::unordered_map<std::string,std::string> postValues {};
			std::function<void(int32_t,const std::string&)> onComplete = [](int32_t code,const std::string &result) mutable {
				std::cout<<"OnComplete: "<<code<<","<<result<<std::endl;
				//lOnComplete(code,result);
			};
			std::function<void(int64_t,int64_t,int64_t,int64_t)> progressCallback = [](int64_t dltotal,int64_t dlnow,int64_t ultotal,int64_t ulnow) mutable {
				std::cout<<"Progress: "<<dltotal<<","<<dlnow<<","<<ultotal<<","<<ulnow<<std::endl;
				//lProgressCallback(dltotal,dlnow,ultotal,ulnow);
			};
			curlHandler.SetErrorHandler([](CurlHandler::ResultCode result) {
				std::cout<<"Result: "<<umath::to_integral(result)<<std::endl;
				});
			curlHandler.AddRequest(url,postValues,onComplete,progressCallback);
		}
	));
	classDefCurl.def("StartDownload",static_cast<void(*)(lua_State*,CurlHandler&)>(
		[](lua_State *l,CurlHandler &curlHandler) {
			curlHandler.StartDownload();
		}
	));
	classDefCurl.def("CancelDownload",static_cast<void(*)(lua_State*,CurlHandler&)>(
		[](lua_State *l,CurlHandler &curlHandler) {
			curlHandler.CancelDownload();
		}
	));
	classDefCurl.def("CancelDownload",static_cast<bool(*)(lua_State*,CurlHandler&)>(
		[](lua_State *l,CurlHandler &curlHandler) -> bool {
			return curlHandler.IsComplete();
		}
	));
	modCurl[classDefCurl];
}

extern "C"
{
	void PRAGMA_EXPORT pragma_initialize_lua(Lua::Interface &l)
	{
		register_lua_library(l);
	}
};
#pragma optimize("",on)
