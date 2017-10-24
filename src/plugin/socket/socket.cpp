//
// Copyright (c) 2013-2016 Pavel Medvedev. All rights reserved.
//
// This file is part of v8pp (https://github.com/pmed/v8pp) project.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include "stdafx.h"

#include <v8pp/module.hpp>
#include <v8pp/class.hpp>
#include <v8pp/config.hpp>

#include <fstream>
#include <iostream>

#include <string>
using namespace std;

#include "Global/helper.h"
//#include "src/SSLServer.h"
#include "src/SSLClient.h"

#include "JsTcpClient.h"


namespace hpsocket {

	wstring AnsiToUnicode(string strA)
	{
		wstring strW;
		if (strA.empty() || strA.size() < 1)
		{
			return _T("");
		}
		WCHAR *pwcString;

		pwcString = new WCHAR[strA.size() + 1];
		MultiByteToWideChar(CP_ACP, 0, strA.c_str(), -1, pwcString, strA.size() + 1);
		strW = pwcString;
		delete pwcString;

		return strW;
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

//	EnAppState enState = ST_STARTING;

	

//	CCLientListener* g_pListener = new CCLientListener();
//	CTcpClient g_client(g_pListener);

// 	void send(const char* p, int nLen)
// 	{
// 		int nLen2 = strlen(p);
// 		g_client.Send((const BYTE*)p, nLen);
// 	}

/*	void start(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		v8::HandleScope handle_scope(args.GetIsolate());

		int nArgsCount = args.Length();

		v8::String::Utf8Value strHost(args[0]);
		v8::Local<v8::Value> vPort(args[1]);
		std::string sHost = *strHost;

		v8::Local<v8::Uint32> uPort = vPort->ToUint32();
		uint32_t nPort = uPort->Value();

		for (int i = 0; i < args.Length(); ++i)
		{
			if (i > 0) std::cout << ' ';
			v8::String::Utf8Value str(args[i]);
			string s = *str;
			string sOut;
			Utf8ToMb((char*)s.c_str(), s.length(), sOut);
			std::cout << sOut;
		}

		/////////
		
		//g_client.CleanupSSLContext();
		//g_client.SetupSSLContext(3, L"ssl-cert\\server.cer", L"ssl-cert\\server.key", L"123456", L"ssl-cert\\ca.crt");
		//g_client.SetupSSLContext();

		wstring strIp = AnsiToUnicode(sHost);
		g_client.Start(strIp.c_str(), nPort);
		//g_client.Start(L"127.0.0.1", 5555);

		std::cout << std::endl;
	}
*/
v8::Handle<v8::Value> init(v8::Isolate* isolate)
{
	v8::EscapableHandleScope scope(isolate);

	///////////////////
// 	v8pp::class_<IClientListenerT> IClientListenerT_class(isolate);
// 	IClientListenerT_class
// 		.inherit<ISocketListenerT>()
// 		;

	///////////////////
	v8pp::class_<ITcpClientListener> ITcpClientListener_class(isolate);
	ITcpClientListener_class
		//.inherit<IClientListenerT<ITcpClient>>()
		;

	///////////////////
	v8pp::class_<IClient> IClient_class(isolate);
	IClient_class
// 		.set("Start", &IClient::Start)
// 		.set("Stop", &IClient::Stop)
// 		.set("Send", &IClient::Send)
// 		.set("SendPackets", &IClient::SendPackets)
		;

	///////////////////
	v8pp::class_<ITcpClient> ITcpClient_class(isolate);
	ITcpClient_class
		.inherit<IClient>()
		//.set("", )
		;

	///////////////////
	v8pp::class_<CTcpClient> CTcpClient_class(isolate);
	CTcpClient_class
		.ctor<ITcpClientListener*>()
		.inherit<ITcpClient>()
		.set("CleanupSSLContext", &CTcpClient::CleanupSSLContext)
		.set("SetupSSLContext", &CTcpClient::SetupSSLContext)
		.set("Start", &CTcpClient::Start)
		;

	///////////////////
	v8pp::class_<CSSLClient> CSSLClient_class(isolate);
	CSSLClient_class
		.ctor<ITcpClientListener*>()
		.inherit<CTcpClient>()
		.set("CleanupSSLContext", &CSSLClient::CleanupSSLContext)
		.set("SetupSSLContext", &CSSLClient::SetupSSLContext)
		.set("Start", &CSSLClient::Start)
		;

	///////////////////
	v8pp::class_<CTcpClientListener> CTcpClientListener_class(isolate);
	CTcpClientListener_class
		.inherit<ITcpClientListener>()
		;

	///////////////////
	v8pp::class_<CCLientListener> CCLientListener_class(isolate);
	CCLientListener_class
		.ctor()
		.inherit<CTcpClientListener>()
		//.set("OnSend", &CCLientListener::OnSend)
		;

	///////////////////
	v8pp::class_<CJsTcpClient> CJsTcpClient_class(isolate);
	CJsTcpClient_class
		.ctor<CCLientListener*>()
		//.set("Create", &CJsTcpClient::Create)
		.set("Start", &CJsTcpClient::Start)
		.set("Send", &CJsTcpClient::Send)
		.set("SetCallbackOnSend", &CJsTcpClient::SetCallbackOnSend)
		;

	// Create a module to add classes and functions to and return a
	// new instance of the module to be embedded into the v8 context
	v8pp::module m(isolate);
	m.set("itcpclient_listener", ITcpClientListener_class);
 	m.set("tcpclient_listener", CTcpClientListener_class);
	m.set("client_listener", CCLientListener_class);
 	m.set("iclient", IClient_class);
 	m.set("ssl_client", CSSLClient_class);
 	m.set("tcp_client", CTcpClient_class);
 	m.set("itcp_client", ITcpClient_class);
	m.set("jstcpclient", CJsTcpClient_class);
//	m.set("start", start);
//	m.set("send", send);


	return scope.Escape(m.new_instance());
}

} // namespace hpsocket

V8PP_PLUGIN_INIT(v8::Isolate* isolate)
{
	return hpsocket::init(isolate);
}
