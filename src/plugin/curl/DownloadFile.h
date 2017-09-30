#pragma once

#include <curl/curl.h>
#include <string>
#include "v8.h"
using namespace std;

namespace curl{
	class CDownloadFile
	{
	public:
		explicit CDownloadFile();
		~CDownloadFile();
	private:
		bool *m_bCancel;
		
		v8::Local<v8::Function> m_js_callback;
		v8::Isolate*	m_isolate;
		string			m_strCallbackName;

	public:
		static int ProgressCallback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow);
		static size_t DownloadCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam);
		int start(const char* szUrl, const char* szPath);

		void setCallback(v8::Local<v8::Function> js_callback);
	};

};