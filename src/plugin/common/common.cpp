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
#include <v8pp/class.hpp>

#include <string>
using namespace std;

#include "pips.h"
#include "StringConvert.hpp"
#include <winuser.h>
#include <shellapi.h>


#pragma comment(lib, "User32.lib")
#pragma comment(lib, "shell32.lib")

namespace common 
{
	v8::Isolate* g_timerIsolate = NULL;

	////////////////////////
	////// MessageBox
	////////////////////////
	void msgbox(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		v8::HandleScope handle_scope(args.GetIsolate());

		//text
		string sText = utf8Value2Mb(args[0]);

		//title
		string sTitle = utf8Value2Mb(args[1]);

		//type
		uint32_t uType = utf8Value2uint32(args[2]);
		
		///////////////////
		UINT nRet = ::MessageBoxA(NULL, sText.c_str(), sTitle.c_str(), uType);

		args.GetReturnValue().Set(nRet);
	}

	void CALLBACK TimerProcCallback(HWND hwnd, UINT nTimerID, UINT_PTR pData, DWORD n)
	{
		///////////////////
		string strCallbackName = "TimerProc";
		v8::Isolate* isolate = g_timerIsolate;
		v8::Local<v8::Context> context = isolate->GetCurrentContext();

		v8::Local<v8::String> funName = v8::String::NewFromUtf8(isolate, strCallbackName.c_str(), v8::NewStringType::kNormal).ToLocalChecked();

		v8::Context::Scope context_scope(context);

		v8::Local<v8::Object> gObj = context->Global();
		v8::Local<v8::Value> value = gObj->Get(funName);
		v8::Local<v8::Function> fun_execute = v8::Local<v8::Function>::Cast(value);

		//
// 		v8::Local<v8::Value> args[1];
// 
// 		v8::Local<v8::Number> numNow = v8::Number::New(isolate, dlnow);
// 		args[0] = numNow;

		fun_execute->Call(gObj, 0, NULL);
	}

	void settimer(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		v8::HandleScope handle_scope(args.GetIsolate());
		//
		g_timerIsolate = args.GetIsolate();
		
		//param 1
		uint32_t uDelay = utf8Value2uint32(args[0]);

		//param 2
		uint32_t uResolution = utf8Value2uint32(args[1]);

		///////////////////
		UINT nRet = ::SetTimer(NULL, uDelay, uResolution, TimerProcCallback);

		args.GetReturnValue().Set(nRet);
	}

	void killtimer(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		v8::HandleScope handle_scope(args.GetIsolate());

		uint32_t uTimerId = utf8Value2uint32(args[0]);

		::KillTimer(NULL, uTimerId);
	}

	void exec(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		v8::HandleScope handle_scope(args.GetIsolate());

		//
		string str1 = utf8Value2Mb(args[0]);
		string str2 = utf8Value2Mb(args[1]);

		::ShellExecuteA(NULL, "open", str1.c_str(), str2.c_str(), NULL, SW_SHOW);
	}

	///////////////////////////////////////////
	void getiniint(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		v8::HandleScope handle_scope(args.GetIsolate());

		//
		string strSection = utf8Value2Mb(args[0]);
		string strKey = utf8Value2Mb(args[1]);
		string strIniFile = utf8Value2Mb(args[2]);

		int nValue = GetPrivateProfileIntA(strSection.c_str(), strKey.c_str(), 0, strIniFile.c_str());
		args.GetReturnValue().Set(nValue);
	}

	void setintstring(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		v8::HandleScope handle_scope(args.GetIsolate());

		//
		string strSection = utf8Value2Mb(args[0]);
		string strKey = utf8Value2Mb(args[1]);
		string strValue = utf8Value2Mb(args[2]);
		string strIniFile = utf8Value2Mb(args[3]);

		BOOL bRet = WritePrivateProfileStringA(strSection.c_str(), strKey.c_str(), strValue.c_str(), strIniFile.c_str());
		args.GetReturnValue().Set(bRet);
	}

	void getintstring(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		v8::HandleScope handle_scope(args.GetIsolate());

		//
		string strSection = utf8Value2Mb(args[0]);
		string strKey = utf8Value2Mb(args[1]);
		string strIniFile = utf8Value2Mb(args[2]);

		char chValue[MAX_PATH] = { 0 };
		GetPrivateProfileStringA(strSection.c_str(), strKey.c_str(), "", chValue, MAX_PATH, strIniFile.c_str());

		args.GetReturnValue().Set(v8pp::to_v8(args.GetIsolate(), chValue));
	}

	//cmd pip
	void cmdpip(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		v8::HandleScope handle_scope(args.GetIsolate());

		//
		string strParam1 = utf8Value2Mb(args[0]);
		string strParam2 = utf8Value2Mb(args[1]);


		/////////////
		SECURITY_ATTRIBUTES sa;//创建一个安全属性的变量
		HANDLE hRead, hWrite; //管道的读写句柄声明
		sa.nLength = sizeof(SECURITY_ATTRIBUTES); //获取安全属性的长度
		sa.lpSecurityDescriptor = NULL;  //使用系统默认的安全描述符 
		sa.bInheritHandle = TRUE;  //创建的进程允许继承句柄

		if (!CreatePipe(&hRead, &hWrite, &sa, 0))  //创建匿名管道
		{
			return;
		}
		STARTUPINFOA si; //启动信息结构体变量
		PROCESS_INFORMATION pi;//需要传入的进程信息的变量

		ZeroMemory(&si, sizeof(STARTUPINFOA)); //因为要传入参数，所以先清空该变量
		si.cb = sizeof(STARTUPINFOA); //结构体的长度
		GetStartupInfoA(&si);
		si.hStdError = hWrite;
		si.hStdOutput = hWrite;  //新创建进程的标准输出连在写管道一端
		si.wShowWindow = SW_HIDE;  //隐藏窗口 
		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;


		char cmdline[200] = { 0 };
		sprintf(cmdline, "cmd /C %s %s", strParam1.c_str(), strParam2.c_str());

		if (!CreateProcessA(NULL, cmdline, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))  //创建子进程
		{
			return ;
		}
		CloseHandle(hWrite);  //关闭管道句柄

		char buffer[4096] = { 0 };
		DWORD bytesRead;

		while (true)
		{
			if (ReadFile(hRead, buffer, 4095, &bytesRead, NULL) == NULL)  //读取管道内容到buffer中  
				break;

			OutputDebugStringA(buffer);
			OutputDebugStringA("\r\n");

		}
		CloseHandle(hRead); //关闭读取句柄
	}

	///////////////////////////////////////////////
	v8::Handle<v8::Value> init(v8::Isolate* isolate)
	{
		v8::EscapableHandleScope scope(isolate);

		v8pp::class_<CPips> CPips_class(isolate);
		CPips_class
			.ctor()
			.set("start", &CPips::start)
			.set("callback", &CPips::setCallback)
			;

		v8pp::module m(isolate);
		m.set("settimer", &settimer);
		m.set("killtimer", &killtimer);
		m.set("msgbox", &msgbox);
		m.set("exec", &exec);
		m.set("getiniint", &getiniint);
		m.set("getinistring", &getintstring);
		m.set("setinistring", &setintstring);

		m.set("cmdpip", &cmdpip);
		m.set("pips", CPips_class);

		//return m.new_instance();
		return scope.Escape(m.new_instance());
	}

} // namespace common

V8PP_PLUGIN_INIT(v8::Isolate* isolate)
{
	return common::init(isolate);
}

