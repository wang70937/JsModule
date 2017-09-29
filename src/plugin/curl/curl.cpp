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

#include <curl/curl.h>

namespace curl {

static long writer(void *data, int size, int nmemb, string &content)
{
	long sizes = size * nmemb;

	//unsigned char* pTemp = (unsigned char*)data;
	string temp((char*)data, sizes);
	content += temp;

	return sizes;
}

wstring UTF8ToUnicode(char* szSrc)
{
	int  len = 0;
	string str = szSrc;
	len = str.length();
	int  unicodeLen = ::MultiByteToWideChar(CP_UTF8,
		0,
		str.c_str(),
		-1,
		NULL,
		0);
	wchar_t *  pUnicode;
	pUnicode = new  wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1)*sizeof(wchar_t));
	::MultiByteToWideChar(CP_UTF8,
		0,
		str.c_str(),
		-1,
		(LPWSTR)pUnicode,
		unicodeLen);
	wstring  rt;
	rt = (wchar_t*)pUnicode;
	delete  pUnicode;

	return  rt;
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

// 返回http header回调函数    
size_t header_callback(const char  *ptr, size_t size, size_t nmemb, std::string *stream)
{
	//	assert(stream != NULL);
	size_t len = size * nmemb;
	stream->append(ptr, len);
	return len;
}

int PostUrl(const char* szRefrence, const char* szUrl, const char* szPostData,
	string& sRet, int& nLen, BOOL bPost /*= TRUE*/, BOOL bFirst /*= FALSE*/)
{
	string sData;
	CURL *curl;
	CURLcode res;
	struct curl_slist *headers = NULL;
	//curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	if (curl){
		res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
		res = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);

		//curl_easy_setopt(curl, CURLOPT_ENCODING, "gzip, deflate");

		//		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);   //只需要设置一个秒的数量就可以  

		//初始化cookie引擎
		//curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
		res = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

		//http请求头
		headers = curl_slist_append(headers, szRefrence);

		res = curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:13.0) Gecko/20100101 Firefox/13.0.1");

		if (bFirst)
		{
			res = curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "cookie.txt");//把服务器发过来的cookie保存到cookie.txt
		}
		else
		{
			res = curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "cookie.txt");
		}

		//发送http请求头
		res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		res = curl_easy_setopt(curl, CURLOPT_URL, szUrl);

		if (bPost)
		{
			res = curl_easy_setopt(curl, CURLOPT_POST, 1);
			res = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, szPostData);
		}


		string content;
		//设置回调函数
		res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
		res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &content);

		//抓取头信息，回调函数  
		std::string szheader_buffer;
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &szheader_buffer);

		//执行http请求
		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
		{
			curl_easy_cleanup(curl);
			curl_slist_free_all(headers);
			return res;
		}


		wstring sTmp = UTF8ToUnicode((char*)content.c_str());
		//wstring sOutput = (wstring)(_T("\r\n[post url]:")) + sTmp + (wstring)(_T("\r\n"));
		OutputDebugString(sTmp.c_str());

		nLen = content.length();
		sRet = content;


		//释放资源
		curl_easy_cleanup(curl);
		curl_slist_free_all(headers);
		headers = NULL;
	}

	return res;
}

string post(v8::FunctionCallbackInfo<v8::Value> const& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());

	int nArgsCount = args.Length();
	if (nArgsCount < 2 )
	{
		return "";
	}

	//////////////////
	//param 1
	v8::String::Utf8Value strParam1(args[0]);
	//param 2
	v8::String::Utf8Value strParam2(args[1]);
	//param 5
	v8::Handle<v8::Value> strParam5(args[4]);

	bool bPost = true;
	bool bFirst = false;
	if (nArgsCount >= 3)
	{
		//param 3
		v8::Local<v8::Value> bParam3(args[2]);
		bPost = bParam3->BooleanValue();
	}
	
	if (nArgsCount >= 4)
	{
		//param 4
		v8::Local<v8::Value> bParam4(args[3]);
		bFirst = bParam4->BooleanValue();
	}
	

	/////////////////
	string sRet;
	int nLen = 0;
	PostUrl(*strParam1, *strParam1, *strParam2, sRet, nLen, bPost, bFirst);

	OutputDebugStringA(sRet.c_str());
	//
	string sUtf8;
	Utf8ToMb((char*)sRet.c_str(), nLen, sUtf8);
	int nLen2 = sUtf8.length();

	/////////////////
	strParam5 = v8pp::to_v8(args.GetIsolate(), sUtf8);

	return sRet;
}

v8::Handle<v8::Value> init(v8::Isolate* isolate)
{
	v8pp::module m(isolate);
	m.set("post", &post);

	return m.new_instance();
}

} // namespace console

V8PP_PLUGIN_INIT(v8::Isolate* isolate)
{
	return curl::init(isolate);
}
