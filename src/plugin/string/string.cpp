//
// Copyright (c) 2013-2016 Pavel Medvedev. All rights reserved.
//
// This file is part of v8pp (https://github.com/pmed/v8pp) project.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <v8pp/module.hpp>
#include <v8pp/config.hpp>

#include <string>
using namespace std;

namespace jstring {
	//Convert(strA_in,strB_out,CP_UTF8,CP_ACP)//UTF8转换ANSI
	//Convert(strA_out, strB_in, CP_ACP, CP_UTF8)//ANSI转换UTF8
	void Convert(const char* strIn, char* strOut, int sourceCodepage, int targetCodepage)
	{
		int len = strlen(strIn);
		int unicodeLen = MultiByteToWideChar(sourceCodepage, 0, strIn, -1, NULL, 0);
		wchar_t* pUnicode;
		pUnicode = new wchar_t[unicodeLen + 1];
		memset(pUnicode, 0, (unicodeLen + 1)*sizeof(wchar_t));
		MultiByteToWideChar(sourceCodepage, 0, strIn, -1, (LPWSTR)pUnicode, unicodeLen);
		BYTE * pTargetData = NULL;
		int targetLen = WideCharToMultiByte(targetCodepage, 0, (LPWSTR)pUnicode, -1, (char *)pTargetData, 0, NULL, NULL);
		pTargetData = new BYTE[targetLen + 1];
		memset(pTargetData, 0, targetLen + 1);
		WideCharToMultiByte(targetCodepage, 0, (LPWSTR)pUnicode, -1, (char *)pTargetData, targetLen, NULL, NULL);
		strcpy(strOut, (char*)pTargetData);
		delete pUnicode;
		delete pTargetData;
	}

	bool Utf8ToMb(char* strStcText, int nLen, std::string &strDstText)
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

	///////////////////////////////////
	std::string AnsiToUtf8(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		v8::HandleScope handle_scope(args.GetIsolate());

		v8::String::Utf8Value str(args[0]);

		std::string sIn = *str;
		char chOut[1024] = { 0 };
		Convert(sIn.c_str(), chOut, CP_UTF8, CP_ACP);

		char chOut2[1024] = { 0 };
		Convert(chOut, chOut2, CP_ACP, CP_UTF8);

		std::string sOut = chOut2;
		//Utf8ToMb((char*)sIn.c_str(), sIn.length(), sOut);
		return sOut;
	}

	std::string Utf8ToAnsi(v8::FunctionCallbackInfo<v8::Value> const& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());

	v8::String::Utf8Value str(args[0]);
	std::string s = *str;
	std::string sOut;
	Utf8ToMb((char*)s.c_str(), s.length(), sOut);
		
	return sOut;
}

	void _replace(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		v8::HandleScope handle_scope(args.GetIsolate());

		//src
		v8::String::Utf8Value str1(args[0]);
		std::string s1 = *str1;
		std::string sOut1;
		Utf8ToMb((char*)s1.c_str(), s1.length(), sOut1);

		//old sub
		v8::String::Utf8Value str2(args[1]);
		std::string s2 = *str2;
		std::string sOut2;
		Utf8ToMb((char*)s2.c_str(), s2.length(), sOut2);

		//new sub
		v8::String::Utf8Value str3(args[2]);
		std::string s3 = *str3;
		std::string sOut3;
		Utf8ToMb((char*)s3.c_str(), s3.length(), sOut3);

		///////////
		while (true)  
		{
			string::size_type   pos(0);
			if ((pos = sOut1.find(sOut2)) != string::npos)
				sOut1.replace(pos, sOut2.length(), sOut3);
			else   
				break;
		}

		//
		char chRet[1024] = { 0 };
		Convert(sOut1.c_str(), chRet, CP_ACP, CP_UTF8);

		args.GetReturnValue().Set(v8pp::to_v8(args.GetIsolate(), chRet));
	}

v8::Handle<v8::Value> init(v8::Isolate* isolate)
{
	v8pp::module m(isolate);
	m.set("Utf8ToAnsi", &Utf8ToAnsi);
	m.set("AnsiToUtf8", &AnsiToUtf8);
	m.set("replace", &_replace);
	return m.new_instance();
}

} // namespace string

V8PP_PLUGIN_INIT(v8::Isolate* isolate)
{
	return jstring::init(isolate);
}
