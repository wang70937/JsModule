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
#include "unzip.h"
#include "zip.h"
#include <tchar.h>
#include <shlwapi.h>
using namespace std;

#pragma comment(lib, "shlwapi.lib")

namespace zip {

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

	//////////////
	
	ZRESULT _AddFileToZip(const TCHAR *zipfn, const char *pwd, const TCHAR *zename, const TCHAR *zefn)
	{
		if (GetFileAttributes(zipfn) == 0xFFFFFFFF || (zefn != 0 && GetFileAttributes(zefn) == 0xFFFFFFFF)) 
			return ZR_NOFILE;
		// Expected size of the new zip will be the size of the old zip plus the size of the new file
		HANDLE hf = CreateFile(zipfn, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0); 
		if (hf == INVALID_HANDLE_VALUE) 
			return ZR_NOFILE; 
		DWORD size = GetFileSize(hf, 0); 
		CloseHandle(hf);

		if (zefn != 0) 
		{
			hf = CreateFile(zefn, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0); 
			if (hf == INVALID_HANDLE_VALUE) 
				return ZR_NOFILE; 
			size += GetFileSize(hf, 0); 
			CloseHandle(hf); }
		size *= 2; // just to be on the safe side.
		//
		HZIP hzsrc = OpenZip(zipfn, pwd);
		if (hzsrc == 0) 
			return ZR_READ;
		HZIP hzdst = CreateZip(0, size, pwd);
		if (hzdst == 0) 
		{
			CloseZip(hzsrc); 
			return ZR_WRITE; 
		}
		// hzdst is created in the system pagefile
		// Now go through the old zip, unzipping each item into a memory buffer, and adding it to the new one
		char *buf = 0; 
		unsigned int bufsize = 0; // we'll unzip each item into this memory buffer
		ZIPENTRY ze; 
		ZRESULT zr = GetZipItem(hzsrc, -1, &ze); 
		int numitems = ze.index; 
		if (zr != ZR_OK) 
		{ 
			CloseZip(hzsrc); 
			CloseZip(hzdst); 
			return zr; 
		}
		for (int i = 0; i < numitems; i++)
		{
			zr = GetZipItem(hzsrc, i, &ze); 
			if (zr != ZR_OK)
			{ 
				CloseZip(hzsrc); 
				CloseZip(hzdst); return zr; 
			}
			if (_tcscmp(ze.name, zename) == 0) 
				continue; // don't copy over the old version of the file we're changing
			if (ze.attr&FILE_ATTRIBUTE_DIRECTORY) 
			{ 
				zr = ZipAddFolder(hzdst, ze.name); 
				if (zr != ZR_OK) 
				{ 
					CloseZip(hzsrc);
					CloseZip(hzdst); 
					return zr; 
				}
				continue; 
			}
			if (ze.unc_size > (long)bufsize)
			{ 
				if (buf != 0) 
					delete[] buf; 
				bufsize = ze.unc_size/* * 2*/; 
				buf = new char[bufsize]; 
			}
			zr = UnzipItem(hzsrc, i, buf, bufsize); 
			if (zr != ZR_OK) 
			{ 
				CloseZip(hzsrc); 
				CloseZip(hzdst); 
				return zr; 
			}
			zr = ZipAdd(hzdst, ze.name, buf, bufsize); 
			if (zr != ZR_OK) 
			{ 
				CloseZip(hzsrc); 
				CloseZip(hzdst); 
				return zr;
			}
		}
		delete[] buf;
		// Now add the new file
		if (zefn != 0) 
		{ 
			zr = ZipAdd(hzdst, zename, zefn); 
			if (zr != ZR_OK) 
			{ 
				CloseZip(hzsrc); 
				CloseZip(hzdst); 
				return zr; 
			} 
		}
		zr = CloseZip(hzsrc); 
		if (zr != ZR_OK) 
		{ 
			CloseZip(hzdst); 
			return zr; 
		}
		//
		// The new file has been put into pagefile memory. Let's store it to disk, overwriting the original zip
		zr = ZipGetMemory(hzdst, (void**)&buf, &size); 
		if (zr != ZR_OK) 
		{ 
			CloseZip(hzdst);
			return zr; 
		}
		hf = CreateFile(zipfn, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0); 
		if (hf == INVALID_HANDLE_VALUE) 
		{ 
			CloseZip(hzdst); 
			return ZR_WRITE; 
		}
		DWORD writ; 
		WriteFile(hf, buf, size, &writ, 0);
		CloseHandle(hf);
		zr = CloseZip(hzdst); 
		if (zr != ZR_OK)
			return zr;
		return ZR_OK;
	}

	ZRESULT _RemoveFileFromZip(const TCHAR *zipfn, const TCHAR *zename)
	{
		return _AddFileToZip(zipfn, 0, zename, 0);
	}

	///////////////////////
	void CreateZipFile(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		v8::HandleScope handle_scope(args.GetIsolate());

		int nArgsCount = args.Length();

		v8::String::Utf8Value str(args[0]);
		string sUtf8Name = *str;
		string sFileName;
		Utf8ToMb((char*)sUtf8Name.c_str(), strlen(sUtf8Name.c_str()), sFileName);


		//
		string sPwd;
		bool bHasPassword = false;
		if (nArgsCount >= 2)
		{
			v8::String::Utf8Value strPwd(args[1]);
			string sTmp = *strPwd;
			Utf8ToMb((char*)sTmp.c_str(), strlen(sTmp.c_str()), sPwd);
			if (sPwd.empty() || strcmp(sPwd.c_str(), "0") == 0)
			{
				bHasPassword = false;
			}
			else
			{
				bHasPassword = true;
			}
		}

		wstring sName = AnsiToUnicode(sFileName);
		HZIP hz = CreateZip(sName.c_str(), (bHasPassword ? sPwd.c_str() : 0));

		for (int i = 2; i < nArgsCount; i++)
		{
			v8::String::Utf8Value strItem(args[i]);
			string sTmp = *strItem;
// 			string sItem;
// 			Utf8ToMb((char*)sTmp.c_str(), strlen(sTmp.c_str()), sItem);

			//
// 			char chTmp[MAX_PATH] = { 0 };
// 			string sUtf8Item;
// 			Convert(sItem.c_str(), chTmp, CP_ACP, CP_UTF8);
// 			sUtf8Item = chTmp;

			wstring sFileItem = UTF8ToUnicode((char*)/*sUtf8Item*/sTmp.c_str());
			ZipAdd(hz, sFileItem.c_str(), sFileItem.c_str());
		}
		
		CloseZip(hz);
	}

	void CreateZipFileFromDir(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		int nArgsCount = args.Length();

		//zip file name
		v8::String::Utf8Value str1(args[0]);
		string sFileName = *str1;

		//password
		v8::String::Utf8Value str2(args[1]);
		string sPwd = *str2;

		//directory
		v8::String::Utf8Value str3(args[2]);
		string sDir = *str3;
		wstring strDir = AnsiToUnicode(sDir);

		//////////////
		wstring sName = AnsiToUnicode(sFileName);
		HZIP hz = CreateZip(sName.c_str(), (strcmp(sPwd.c_str(), "0") == 0) ? 0 : sPwd.c_str());

		//////////////////
		TCHAR chDir[2 * MAX_PATH] = { 0 };
		GetModuleFileName(NULL, chDir, 2 * MAX_PATH);
		PathRemoveFileSpec(chDir);
		_tcscat_s(chDir, 2 * MAX_PATH, _T("\\"));
		_tcscat_s(chDir, 2 * MAX_PATH, strDir.c_str());

		TCHAR chFind[2 * MAX_PATH] = { 0 };
		_stprintf_s(chFind, 2 * MAX_PATH, _T("%s\\*.*"), chDir);

		WIN32_FIND_DATA wfd;
		HANDLE hFind = ::FindFirstFile(chFind, &wfd);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			return;
		}

		vector<wstring> vecFiles;

		TCHAR chFile[2 * MAX_PATH] = { 0 };

		while (TRUE)
		{
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (_tcscmp(wfd.cFileName, _T(".")) != 0 && _tcscmp(wfd.cFileName, _T("..")) != 0)
				{
					//
				}
			}
			else
			{
				_stprintf_s(chFile, 2 * MAX_PATH, _T("%s\\%s"), chDir, wfd.cFileName);
				vecFiles.push_back(chFile);
			}
			if(!FindNextFile(hFind, &wfd))
				break;
		}

		FindClose(hFind);

		////////////////
		vector<wstring>::iterator it = vecFiles.begin();
		for (; it != vecFiles.end(); ++it)
		{
			wstring& sItem = (*it);
			wstring sTmp = sItem;
			int nPos = sItem.find_last_of(_T('\\'));
			if (nPos != -1)
			{
				sTmp = sItem.substr(nPos + 1, sItem.length() - nPos - 1);
			}
			
			ZipAdd(hz, sTmp.c_str(), sItem.c_str());
		}

		CloseZip(hz);
	}

	void AddFileToZip(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		v8::HandleScope handle_scope(args.GetIsolate());

		v8::String::Utf8Value str1(args[0]);
		string sZipFileName = *str1;
		wstring strParam1 = AnsiToUnicode(sZipFileName);

		v8::String::Utf8Value str2(args[1]);
		string sPwd = *str2;

		v8::String::Utf8Value str3(args[2]);
		string sZeName = *str3;
		wstring strParam3 = AnsiToUnicode(sZeName);

		v8::String::Utf8Value str4(args[3]);
		string sZeFileName = *str4;
		wstring strParam4 = AnsiToUnicode(sZeFileName);

		/////////////
		_AddFileToZip(strParam1.c_str(), sPwd.c_str(), strParam3.c_str(), strParam4.c_str());
	}

	void UncompressZip(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		v8::HandleScope handle_scope(args.GetIsolate());

		v8::String::Utf8Value str(args[0]);
		string sUtf8Name = *str;
		string sZipFileName;
		Utf8ToMb((char*)sUtf8Name.c_str(), strlen(sUtf8Name.c_str()), sZipFileName);

		wstring sFile = AnsiToUnicode(sZipFileName);

		//
		v8::String::Utf8Value str2(args[1]);
		string sPwd = *str2;

		//
		string sUncompressDir;
		if (args.Length() == 3)
		{
			v8::String::Utf8Value str3(args[2]);
			string sTmp = *str3;
			Utf8ToMb((char*)sTmp.c_str(), strlen(sTmp.c_str()), sUncompressDir);
		}
		wstring sDir = AnsiToUnicode(sUncompressDir);
		if (sDir.empty())
		{
			sDir = sFile;
		}

		/////////////
		TCHAR chFullPath[2 * MAX_PATH] = { 0 };
		GetModuleFileName(NULL, chFullPath, 2 * MAX_PATH);
		PathRemoveFileSpec(chFullPath);
		_tcscat_s(chFullPath, 2 * MAX_PATH, _T("\\"));
		_tcscat_s(chFullPath, 2 * MAX_PATH, sFile.c_str());

		/////////////
		HZIP hz = OpenZip(sFile.c_str()/*chFullPath*/, (strcmp(sPwd.c_str(), "0") == 0) ? 0 : sPwd.c_str());
		SetUnzipBaseDir(hz, sDir.c_str());
		ZIPENTRY ze; 
		GetZipItem(hz, -1, &ze); 
		int numitems = ze.index;
		for (int i = 0; i < numitems; i++)
		{
			GetZipItem(hz, i, &ze);
			UnzipItem(hz, i, ze.name);
		}
		CloseZip(hz);
	}

v8::Handle<v8::Value> init(v8::Isolate* isolate)
{
	v8pp::module m(isolate);
	//m.set("AddFileToZip", &AddFileToZip);
	m.set("CreateZipFile", &CreateZipFile);
	m.set("CreateZipFileFromDir", &CreateZipFileFromDir);
	m.set("UncompressZip", &UncompressZip);
	return m.new_instance();
}

} // namespace zip

V8PP_PLUGIN_INIT(v8::Isolate* isolate)
{
	return zip::init(isolate);
}
