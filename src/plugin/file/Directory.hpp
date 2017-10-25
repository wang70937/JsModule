#pragma once
#include <v8pp/module.hpp>
#include <v8pp/class.hpp>
#include <v8pp/config.hpp>

#include <string>
#include <shlwapi.h>
using namespace std;

#pragma comment(lib, "shlwapi.lib")

namespace file{

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

	///////////////////
	//获取当前目录
	///////////////////
	string GetCurDir(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		v8::HandleScope handle_scope(args.GetIsolate());

		char chPath[2 * MAX_PATH] = { 0 };
		GetModuleFileNameA(NULL, chPath, 2 * MAX_PATH);
		PathRemoveFileSpecA(chPath);

		string strPath = chPath;
 		char chOut[2 * MAX_PATH] = { 0 };
 		Convert(chPath, chOut, CP_ACP, CP_UTF8);
 		strPath = chOut;

// 		v8::Local<v8::String> obj = v8::String::NewFromUtf8(args.GetIsolate(), chPath, v8::NewStringType::kNormal).ToLocalChecked();
// 		v8::String::Utf8Value v(obj);
//		string sOut = *v;
		return strPath;
	}


};//namespace file