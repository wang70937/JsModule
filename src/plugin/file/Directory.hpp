#pragma once
#include <v8pp/module.hpp>
#include <v8pp/class.hpp>
#include <v8pp/config.hpp>

#include <string>
#include <shlwapi.h>
#include <shlwapi.h>
#include <tchar.h>
#include <shlobj.h>
#include "../../../../../vs2013-demos/JsModule/src/plugin/socket/Common/Src/GeneralHelper.h"
using namespace std;

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Shell32.lib")

namespace file{
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

	BOOL _DeleteDirectory(const TCHAR* sDirName)
	{
		TCHAR chDirFind[2 * MAX_PATH] = { 0 };
		_stprintf_s(chDirFind, 2 * MAX_PATH, _T("%s\\*.*"), sDirName);

		WIN32_FIND_DATA wfd;
		HANDLE hFind = ::FindFirstFile(chDirFind, &wfd);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}

		while (TRUE)
		{
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (_tcscmp(wfd.cFileName, _T(".")) != 0 && _tcscmp(wfd.cFileName, _T("..")) != 0 )
				{
					TCHAR chSubDir[2 * MAX_PATH] = { 0 };
					_stprintf_s(chSubDir, 2 * MAX_PATH, _T("%s\\%s"), sDirName, wfd.cFileName);
					_DeleteDirectory(chSubDir);
				}
			}
			else
			{
				TCHAR chFile[2 * MAX_PATH] = { 0 };
				_stprintf_s(chFile, 2 * MAX_PATH, _T("%s\\%s"), sDirName, wfd.cFileName);

				if (wfd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
				{
					DWORD dwRet = wfd.dwFileAttributes;
					dwRet &= ~FILE_ATTRIBUTE_READONLY;
					SetFileAttributes(chFile, dwRet);
				}
				
				::DeleteFile(chFile);
			}

			if(!FindNextFile(hFind, &wfd))
			{
				break;
			}
		}

		BOOL bRemove = ::RemoveDirectory(sDirName);
		if (!bRemove)
		{
			DWORD dwErr = ::GetLastError();
			return FALSE;
		}

		return TRUE;
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

	///////////////////
	//创建目录
	///////////////////
	void CreateDir(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		v8::String::Utf8Value str(args[0]);
		string sTmp = *str;
		wstring sDest = UTF8ToUnicode((char*)sTmp.c_str());

		if (sDest.empty())
		{
			return;
		}

		//////////////
		TCHAR chPath[2 * MAX_PATH] = { 0 };
		GetModuleFileName(NULL, chPath, 2 * MAX_PATH);
		PathRemoveFileSpec(chPath);
		_tcscat_s(chPath, 2 * MAX_PATH, _T("\\"));

		if (sDest[0] == _T('.') || sDest[0] == _T('\\'))
		{
			//相对路径
			_tcscat_s(chPath, 2 * MAX_PATH, sDest.c_str());
		}
		else
		{
			//绝对路径
			_stprintf_s(chPath, 2 * MAX_PATH, _T("%s"), sDest.c_str());
		}

		::SHCreateDirectory(NULL, chPath);
	}

	///////////////////
	//删除目录
	///////////////////
	void DelDir(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		v8::String::Utf8Value str(args[0]);
		string sTmp = *str;
		wstring sDest = UTF8ToUnicode((char*)sTmp.c_str());

		if (sDest.empty())
		{
			return;
		}

		//////////////
		TCHAR chPath[2 * MAX_PATH] = { 0 };
		GetModuleFileName(NULL, chPath, 2 * MAX_PATH);
		PathRemoveFileSpec(chPath);
		_tcscat_s(chPath, 2 * MAX_PATH, _T("\\"));

		if (sDest[0] == _T('.') || sDest[0] == _T('\\'))
		{
			//相对路径
			_tcscat_s(chPath, 2 * MAX_PATH, sDest.c_str());
		}
		else
		{
			//绝对路径
			_stprintf_s(chPath, 2 * MAX_PATH, _T("%s%s"), chPath,  sDest.c_str());
		}

		_DeleteDirectory(chPath);
	}

	///////////////////
	//删除文件
	///////////////////
	void DelFile(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		v8::String::Utf8Value str(args[0]);
		string sTmp = *str;
		wstring sDest = UTF8ToUnicode((char*)sTmp.c_str());

		if (sDest.empty())
		{
			return;
		}

		//////////////
		TCHAR chPath[2 * MAX_PATH] = { 0 };
		GetModuleFileName(NULL, chPath, 2 * MAX_PATH);
		PathRemoveFileSpec(chPath);
		_tcscat_s(chPath, 2 * MAX_PATH, _T("\\"));

		if (sDest[0] == _T('.') || sDest[0] == _T('\\'))
		{
			//相对路径
			_tcscat_s(chPath, 2 * MAX_PATH, sDest.c_str());
		}
		else
		{
			//绝对路径
			_stprintf_s(chPath, 2 * MAX_PATH, _T("%s"), sDest.c_str());
		}

		DeleteFile(chPath);
	}

};//namespace file