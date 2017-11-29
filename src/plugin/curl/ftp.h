#pragma once

#include <curl/curl.h>
#include <string>
#include "v8.h"
using namespace std;

namespace curl{
	class CFtp
	{
	public:
		explicit CFtp();
		~CFtp();
	private:
		bool *m_bCancel;
		
		v8::Local<v8::Function> m_js_callback;//download
		v8::Local<v8::Function> m_js_callbackUpload;
		v8::Local<v8::Function> m_js_callbackResult;

		v8::Isolate*	m_isolate;
		string			m_strCallbackName;//download
		string			m_strCallbackNameUpload;

		string			m_strCallbackNameResult;

	public:
		//ÏÂÔØ
		static int ProgressCallback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow);
		static size_t DownloadCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam);
		int startDownload(const char* szUrl, const char* szPath, const char* szUser, const char* szPwd);

		void setCallbackDownload(v8::Local<v8::Function> js_callback);

		//ÉÏ´«
		static int ProgressCallbackUpload(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow);
		static size_t UploadCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam);
		int startUpload(const char* szUrl, const char* szPath, const char* szUser, const char* szPwd);

		void setCallbackUpload(v8::Local<v8::Function> js_callback);

		//
		void SetCallbackResult(v8::Local<v8::Function> js_callback);
		void ShowResult(int nCurlCode, const char* szErrMsg);
	};

};