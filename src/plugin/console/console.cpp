//
// Copyright (c) 2013-2016 Pavel Medvedev. All rights reserved.
//
// This file is part of v8pp (https://github.com/pmed/v8pp) project.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <iostream>
#include <v8pp/module.hpp>
#include <v8pp/config.hpp>

#include <string>
using namespace std;

namespace console {

	bool Utf8ToMb(char* strStcText, int nLen, string &strDstText)
	{
		// convert an UTF8 string to widechar
		int nWLen = MultiByteToWideChar(CP_UTF8, 0, strStcText, nLen, NULL, 0);
		WCHAR *strTemp = (WCHAR*)malloc(sizeof(WCHAR)* nWLen);
		if (NULL == strTemp)
		{
			return false;
		}
		int nRtn = MultiByteToWideChar(CP_UTF8, 0, strStcText, nLen, strTemp, nWLen);
		if (nRtn == 0)
		{
			free(strTemp);
			return false;
		}
		// convert an widechar string to Multibyte  
		int MBLen = WideCharToMultiByte(CP_ACP, 0, strTemp, nWLen, NULL, 0, NULL, NULL);
		if (0 == MBLen)
		{
			free(strTemp);
			return false;
		}
		char *str = (char*)malloc(sizeof(char)*MBLen + 1);
		if (NULL == str)
		{
			return false;
		}
		nRtn = WideCharToMultiByte(CP_ACP, 0, strTemp, nWLen, str, MBLen, NULL, NULL);
		if (0 == nRtn)
		{
			free(strTemp);
			free(str);
			return false;
		}
		//最后释放所有的变量
		str[nRtn] = '\0';
		strDstText = str;
		free(strTemp);
		free(str);
		return true;
	}

	void cin(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		v8::HandleScope handle_scope(args.GetIsolate());

		std::string line;
		std::cin >> line;
		args.GetReturnValue().Set(v8pp::to_v8(args.GetIsolate(), line));
	}

void log(v8::FunctionCallbackInfo<v8::Value> const& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());

	for (int i = 0; i < args.Length(); ++i)
	{
		if (i > 0) std::cout << ' ';
		v8::String::Utf8Value str(args[i]);
		string s = *str;
		string sOut;
		Utf8ToMb((char*)s.c_str(), s.length(), sOut);
		std::cout <<  sOut;
	}
	std::cout << std::endl;
}

v8::Handle<v8::Value> init(v8::Isolate* isolate)
{
	v8pp::module m(isolate);
	m.set("log", &log);
	m.set("cin", &cin);
	return m.new_instance();
}

} // namespace console

V8PP_PLUGIN_INIT(v8::Isolate* isolate)
{
	return console::init(isolate);
}
