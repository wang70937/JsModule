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

#include "CrackCaptchaAPI.h"

#include <string>
#include <stdio.h>
using namespace std;

#pragma comment(lib, "CrackCaptchaAPI.lib")

namespace damatu {
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

/////////////////////////////////////////
/////////////////////////////////////////
void _init(v8::FunctionCallbackInfo<v8::Value> const& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());

	//pszSoftwareName
	v8::String::Utf8Value str(args[0]);
	string sTmp;
	char chValue[MAX_PATH] = { 0 };
	Convert(*str, chValue, CP_UTF8, CP_ACP);
	//string sSoftwareName = chValue;

	//pszSoftwareID
	v8::String::Utf8Value str2(args[1]);
	char chValue2[MAX_PATH] = { 0 };
	Convert(*str2, chValue2, CP_UTF8, CP_ACP);
	//string sSoftwareID = chValue;

	int nRet = ::Init(chValue, chValue2);

	args.GetReturnValue().Set(nRet);
}

void _uninit(v8::FunctionCallbackInfo<v8::Value> const& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());

	::Uninit();
}

void _login(v8::FunctionCallbackInfo<v8::Value> const& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());

	//pszUserName
	v8::String::Utf8Value str(args[0]);
	string sTmp;
	char chValue[MAX_PATH] = { 0 };
	Convert(*str, chValue, CP_UTF8, CP_ACP);
	//string sSoftwareName = chValue;

	//pszPassword
	v8::String::Utf8Value str2(args[1]);
	char chValue2[MAX_PATH] = { 0 };
	Convert(*str2, chValue2, CP_UTF8, CP_ACP);
	//string sSoftwareID = chValue;

	bool bHasVerifiCode = false;
	char chValue3[MAX_PATH] = { 0 };
	if (args.Length() >= 3)
	{
		//pszVerificationCode
		v8::String::Utf8Value str3(args[2]);
		Convert(*str3, chValue3, CP_UTF8, CP_ACP);
	}
	
	/////////////////////
	char szSysAnnURL[4096] = { 0 }, szAppAnnURL[4096] = { 0 };
	int nRet = ::Login(chValue, chValue2, (bHasVerifiCode ? chValue2 : NULL),  szSysAnnURL, szAppAnnURL);

	args.GetReturnValue().Set(nRet);
}

void _logoff(v8::FunctionCallbackInfo<v8::Value> const& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());

	int nRet = ::Logoff();
	args.GetReturnValue().Set(nRet);
}

void _readinfo(v8::FunctionCallbackInfo<v8::Value> const& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());

	//int nRet = ::Logoff();
	//args.GetReturnValue().Set(nRet);
}


void _querybalance(v8::FunctionCallbackInfo<v8::Value> const& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	
	uint32_t nValue = 0;// args[0]->Uint32Value();

	int nRet = ::QueryBalance((unsigned long*)&nValue);
	if (nRet == ERR_CC_SUCCESS)
	{
		args.GetReturnValue().Set(nValue);
	}
	else
	{
		args.GetReturnValue().Set(nRet);
	}
// 	v8::Local<v8::Integer> n = v8::Integer::NewFromUnsigned(args.GetIsolate(), nValue);
//  	args[0] = n;
}

void _decodefile(v8::FunctionCallbackInfo<v8::Value> const& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());

	//pic-path
	v8::String::Utf8Value str(args[0]);
	char chValue[MAX_PATH] = { 0 };
	Convert(*str, chValue, CP_UTF8, CP_ACP);

	//timeout
	uint32_t nTimeout = args[1]->Uint32Value();

	//type
	uint32_t nType = args[2]->Uint32Value();

	///////////////
	FILE* ff = NULL;
	errno_t err = fopen_s(&ff, chValue, "rb");
	if (err)
	{
		args.GetReturnValue().Set(-999);
		return;
	}

	fseek(ff, 0, SEEK_END);
	int nLen = ftell(ff);
	fseek(ff, 0, SEEK_SET);
	
	if (nLen <= 0 )
	{
		fclose(ff);
		args.GetReturnValue().Set(-998);
		return;
	}

	///////////////
	char* p = new char[nLen];
	if (!p )
	{
		fclose(ff);
		args.GetReturnValue().Set(-997);
		return;
	}

	memset(p, 0, nLen);
	fread_s(p, nLen, 1, nLen, ff);
	fclose(ff);

	///////////////
	//ext
	string sExt("png");
	string sFilePath = chValue;
	int nPos = sFilePath.find_last_of('\\');
	if (nPos != -1)
	{
		sExt = sFilePath.substr(nPos + 1, sFilePath.length() - nPos - 1);
	}

	///////////////
	ULONG ulRequestID = 0;
	int nRet = ::DecodeBuf(p, nLen, sExt.c_str(), 0, (unsigned short)nTimeout, (unsigned long)nType, &ulRequestID);
	if (nRet == ERR_CC_SUCCESS)
	{
		args.GetReturnValue().Set((uint32_t)ulRequestID);
	}
	else
	{
		args.GetReturnValue().Set(nRet);
	}

	delete[] p;
}

void _getresult(v8::FunctionCallbackInfo<v8::Value> const& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());
	//requestID
	uint32_t nReqestID = args[0]->Uint32Value();

	//nTimeout
	uint32_t nTimeout = args[1]->Uint32Value();

	char szVCode[1000] = { 0 };
	char szRetCookie[4096] = { 0 };
	ULONG ulVCodeID = 0;
	try
	{
		int nRet = ::GetResult(nReqestID, nTimeout * 1000,
			szVCode, sizeof(szVCode), &ulVCodeID, szRetCookie, sizeof(szRetCookie));
		if (nRet == ERR_CC_SUCCESS)
		{
			string sResult = szVCode;
			args.GetReturnValue().Set(v8pp::to_v8(args.GetIsolate(), sResult));
		}
		else
		{
			args.GetReturnValue().Set(nRet);
		}
	}
	catch (std::exception& e)
	{
		args.GetReturnValue().Set(-900);
	}
}

void _d2file(v8::FunctionCallbackInfo<v8::Value> const& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());

	//const char *pszSoftwareId,
	v8::String::Utf8Value str1(args[0]);
	char chValue1[MAX_PATH] = { 0 };
	Convert(*str1, chValue1, CP_UTF8, CP_ACP);

	//const char *pszUserName,
	v8::String::Utf8Value str2(args[1]);
	char chValue2[MAX_PATH] = { 0 };
	Convert(*str2, chValue2, CP_UTF8, CP_ACP);

	//const char *pszUserPassword,
	v8::String::Utf8Value str3(args[2]);
	char chValue3[MAX_PATH] = { 0 };
	Convert(*str3, chValue3, CP_UTF8, CP_ACP);

	//const char *pszFilePath,
	v8::String::Utf8Value str4(args[3]);
	char chValue4[MAX_PATH] = { 0 };
	Convert(*str4, chValue4, CP_UTF8, CP_ACP);

	//unsigned short usTimeout,
	uint32_t nTimeout = args[4]->Uint32Value();

	//unsigned long ulVCodeTypeID,
	uint32_t nVCodeTypeID = args[5]->Uint32Value();


	/////////////////
	char chVCodeText[4096] = { 0 };

	int nRet = ::D2File(chValue1, chValue2, chValue3, chValue4, nTimeout, nVCodeTypeID , chVCodeText);
	args.GetReturnValue().Set(nRet);
}


//////////////////////////////
v8::Handle<v8::Value> init(v8::Isolate* isolate)
{
	v8pp::module m(isolate);
	m.set("init", &_init);
	m.set("uninit", &_uninit);
	m.set("login", &_login);
	m.set("logoff", &_logoff);
	m.set("querybalance", &_querybalance);
	m.set("decodefile", &_decodefile);
	m.set("getresult", &_getresult);
	m.set("d2file", &_d2file);
	return m.new_instance();
}

} // namespace console

V8PP_PLUGIN_INIT(v8::Isolate* isolate)
{
	return damatu::init(isolate);
}
