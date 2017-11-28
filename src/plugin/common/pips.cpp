#include "pips.h"
#include <iostream>
#include "v8pp/convert.hpp"
#include "v8pp/call_v8.hpp"
#include "v8.h"
#include <windows.h>


namespace common{
	CPips::CPips()
	{
		m_bCancel = false;
	}


	CPips::~CPips()
	{
	}

	static void v8_arg_count(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		v8::HandleScope handle_scope(args.GetIsolate());

		for (int i = 0; i < args.Length(); ++i)
		{
			if (i > 0) std::cout << ' ';
			v8::String::Utf8Value str(args[i]);
			string s = *str;

			continue;
		}


		args.GetReturnValue().Set(args.Length());
	}

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

	std::string MbToUtf8(string strIn)
	{
		char chOut2[10240] = { 0 };
		Convert(strIn.c_str(), chOut2, CP_ACP, CP_UTF8);
		string strOut = chOut2;
		return strOut;
	}

	std::string Utf8ToMb(string strIn)
	{
		char chOut2[10240] = { 0 };
		Convert(strIn.c_str(), chOut2, CP_UTF8, CP_ACP);
		string strOut = chOut2;
		return strOut;
	}

	int CPips::PipsCallback(void *clientp, const char* szContent)
	{
		///////////////////////
		CPips* dd = (CPips*)clientp;
		if (dd && !dd->m_strCallbackName.empty())
		{
			v8::Isolate* isolate = dd->m_isolate;
			v8::Local<v8::Context> context = isolate->GetCurrentContext();

			v8::Local<v8::String> funName = v8::String::NewFromUtf8(isolate, dd->m_strCallbackName.c_str(), v8::NewStringType::kNormal).ToLocalChecked();

			v8::Context::Scope context_scope(context);

			v8::Local<v8::Object> gObj = context->Global();
			v8::Local<v8::Value> value = gObj->Get(funName);
			v8::Local<v8::Function> fun_execute = v8::Local<v8::Function>::Cast(value);

			//
			v8::Local<v8::Value> args[1];
			
			string strUtf8 = MbToUtf8(szContent);

			v8::Local<v8::String> str = v8::String::NewFromUtf8(isolate, strUtf8.c_str(), v8::NewStringType::kNormal).ToLocalChecked();

			args[0] = str;
			fun_execute->Call(gObj, 1, args);
		}

		return 0;
	}


	int CPips::start(const char* szParam1, const char* szParam2)
	{
		/////////////
		SECURITY_ATTRIBUTES sa;//创建一个安全属性的变量
		HANDLE hRead, hWrite; //管道的读写句柄声明
		sa.nLength = sizeof(SECURITY_ATTRIBUTES); //获取安全属性的长度
		sa.lpSecurityDescriptor = NULL;  //使用系统默认的安全描述符 
		sa.bInheritHandle = TRUE;  //创建的进程允许继承句柄

		if (!CreatePipe(&hRead, &hWrite, &sa, 0))  //创建匿名管道
		{
			return 1;
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

		//////////////////////
		string strParam1 = Utf8ToMb(szParam1);
		string strParam2 = Utf8ToMb(szParam2);

		char cmdline[200] = { 0 };
		sprintf(cmdline, "cmd /C %s %s", strParam1.c_str(), strParam2.c_str());

		if (!CreateProcessA(NULL, cmdline, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))  //创建子进程
		{
			return 2;
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

			
			PipsCallback(this, buffer);

		}
		CloseHandle(hRead); //关闭读取句柄

		return 0;
	}

	void CPips::setCallback(v8::Local<v8::Function> js_callback)
	{
		v8::Local<v8::Value> vFuncName = js_callback->GetName();
		v8::String::Utf8Value vStrName(vFuncName);
		const char* strName = *vStrName;
		m_strCallbackName = strName;

		m_isolate = js_callback->GetIsolate();

		return;
	}

};//namespace common