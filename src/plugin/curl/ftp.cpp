#include "ftp.h"
#include <iostream>
#include "v8pp/convert.hpp"
#include "v8pp/call_v8.hpp"
#include "v8.h"
#include "StringConvert.hpp"

namespace curl{
CFtp::CFtp()
{
	m_bCancel = FALSE;
}


CFtp::~CFtp()
{
}

int CFtp::ProgressCallback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	if (dltotal > -0.1 && dltotal < 0.1)
	{
		return 0;
	}
	int nPos = (int)((dlnow / dltotal) * 100);
	//通知进度条更新下载进度  
//	std::cout << "dltotal: " << (long)dltotal << " ---- dlnow:" << (long)dlnow << "---- " << nPos <<"%" << std::endl;

//	if (*dd->m_bCancel)
	{
		//1. 返回非0值就会终止 curl_easy_perform 执行  
//		return -2;
	}

	///////////////////////
	CFtp* dd = (CFtp*)clientp;
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
		v8::Local<v8::Value> args[2];

		v8::Local<v8::Number> numNow = v8::Number::New(isolate, dlnow);
		v8::Local<v8::Number> numTotal = v8::Number::New(isolate, dltotal);

		args[0] = numNow;
		args[1] = numTotal;
		fun_execute->Call(gObj, 2, args);
	}

	return 0;
}

size_t CFtp::DownloadCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam)
{
	FILE* fp = (FILE*)pParam;
	size_t nWrite = fwrite(pBuffer, nSize, nMemByte, fp);

	return nWrite;
}

/* parse headers for Content-Length */
size_t getcontentlengthfunc(void *ptr, size_t size, size_t nmemb, void *stream)
{
	int r;
	long len = 0;
	/* _snscanf() is Win32 specific */
	//r = _snscanf(ptr, size * nmemb, "Content-Length: %ld\n", &len);  
	r = sscanf((const char*)ptr, "Content-Length: %ld\n", &len);
	if (r) /* Microsoft: we don't read the specs */
		*((long *)stream) = len;
	return size * nmemb;
}

int CFtp::startDownload(const char* szUrl, const char* szPath, const char* szUser, const char* szPwd/*, UINT nTimeout*/ /*= 10*/)
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

		string sFile;
		Utf8ToMb((char*)szPath, strlen(szPath), sFile);

		FILE* file = NULL;
		errno_t err = fopen_s(&file, sFile.c_str(), "wb");
		if (err != 0)
		{
			char chErrMsg[1024] = { 0 };
			sprintf_s(chErrMsg, 1024, "[downupload] file write FAIL(code=%d), file:[%s]", ::GetLastError(), sFile.c_str());
			ShowResult(res, chErrMsg);

			///////////
			curl_easy_cleanup(curl);
			curl_slist_free_all(headers);
			return -4;
		}

		////////////////////////////////
		//初始化cookie引擎
		//curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
		res = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		res = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);

		res = curl_easy_setopt(curl, CURLOPT_URL, szUrl);

		char chUserInfo[MAX_PATH] = { 0 };
		sprintf_s(chUserInfo, MAX_PATH, "%s:%s", szUser, szPwd);
		res = curl_easy_setopt(curl, CURLOPT_USERPWD, chUserInfo);

		//res = curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, nTimeout);

		//设置头处理函数  
		long filesize = 0;
		res = curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, getcontentlengthfunc);
		res = curl_easy_setopt(curl, CURLOPT_HEADERDATA, &filesize);

		//这里限速 XX KB/s  
		//res = curl_easy_setopt(curl, CURLOPT_MAX_RECV_SPEED_LARGE, (curl_off_t)down_speed * 1024);

		//设置进度回调
		res = curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, ProgressCallback);
		res = curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, this);

		//设置回调函数
		res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, DownloadCallback);
		res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

		//执行http请求
		res = curl_easy_perform(curl);

		///////////////
		char chErrMsg[1024] = { 0 };
		const char* pError = curl_easy_strerror(res);

		sprintf_s(chErrMsg, 1024, "[download] %s, file:[%s]", pError, sFile.c_str());
		ShowResult(res, chErrMsg);

		if (res != CURLE_OK)
		{
			fclose(file);
			curl_easy_cleanup(curl);
			curl_slist_free_all(headers);
			return res;
		}

		fclose(file);

		//释放资源
		curl_easy_cleanup(curl);
		curl_slist_free_all(headers);
		headers = NULL;
	}

	return res;
}

void CFtp::setCallbackDownload(v8::Local<v8::Function> js_callback)
{
	v8::Local<v8::Value> vFuncName = js_callback->GetName();
	v8::String::Utf8Value vStrName(vFuncName);
	const char* strName = *vStrName;
	m_strCallbackName = strName;

	m_isolate = js_callback->GetIsolate();

	return;
}

///////////////////////////////////////
///////////////////////////////////////
int CFtp::ProgressCallbackUpload(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	if (dltotal > -0.1 && dltotal < 0.1)
	{
		return 0;
	}
	int nPos = (int)((dlnow / dltotal) * 100);

	///////////////////////
	CFtp* dd = (CFtp*)clientp;
	if (dd && !dd->m_strCallbackNameUpload.empty())
	{
		v8::Isolate* isolate = dd->m_isolate;
		v8::Local<v8::Context> context = isolate->GetCurrentContext();

		v8::Local<v8::String> funName = v8::String::NewFromUtf8(isolate, dd->m_strCallbackNameUpload.c_str(), v8::NewStringType::kNormal).ToLocalChecked();

		v8::Context::Scope context_scope(context);

		v8::Local<v8::Object> gObj = context->Global();
		v8::Local<v8::Value> value = gObj->Get(funName);
		v8::Local<v8::Function> fun_execute = v8::Local<v8::Function>::Cast(value);

		//
		v8::Local<v8::Value> args[2];

		v8::Local<v8::Number> numNow = v8::Number::New(isolate, dlnow);
		v8::Local<v8::Number> numTotal = v8::Number::New(isolate, dltotal);

		args[0] = numNow;
		args[1] = numTotal;
		fun_execute->Call(gObj, 2, args);
	}

	return 0;
}

size_t CFtp::UploadCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam)
{
	FILE* fp = (FILE*)pParam;
	size_t nWrite = fread(pBuffer, nSize, nMemByte, fp);

	return nWrite;
}

/* discard downloaded data */
size_t discardfunc(void *ptr, size_t size, size_t nmemb, void *stream)
{
	return size * nmemb;
}

/* read data to upload */
size_t readfunc(void *ptr, size_t size, size_t nmemb, void *stream)
{
	FILE *f = (FILE*)stream;
	size_t n;
	if (ferror(f))
		return CURL_READFUNC_ABORT;
	n = fread(ptr, size, nmemb, f) * size;
	return n;
}

int CFtp::startUpload(const char* szUrl, const char* szPath, const char* szUser, const char* szPwd)
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

		long uploaded_len = 0;
		string sFile;
		Utf8ToMb((char*)szPath, strlen(szPath), sFile);

		FILE* file = NULL;
		errno_t err = fopen_s(&file, sFile.c_str(), "rb");
		if (err != 0)
		{
			char chErrMsg[1024] = { 0 };
			sprintf_s(chErrMsg, 1024, "[upload] file read FAIL(code=%d), file:[%s]", ::GetLastError(), sFile.c_str());
			ShowResult(res, chErrMsg);

			//////////
			curl_easy_cleanup(curl);
			curl_slist_free_all(headers);
			return -4;
		}

		////////////////////////////////
		//初始化cookie引擎
		//curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
		//res = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		//res = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);

		res = curl_easy_setopt(curl, CURLOPT_URL, szUrl);

		char chUserInfo[MAX_PATH] = { 0 };
		sprintf_s(chUserInfo, MAX_PATH, "%s:%s", szUser, szPwd);
		res = curl_easy_setopt(curl, CURLOPT_USERPWD, chUserInfo);

		//res = curl_easy_setopt(curl, CURLOPT_FTP_RESPONSE_TIMEOUT, nTimeout);

		//设置上传
		res = curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

		//设置头处理函数  
		res = curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, getcontentlengthfunc);
		res = curl_easy_setopt(curl, CURLOPT_HEADERDATA, &uploaded_len);
		res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, discardfunc);
		res = curl_easy_setopt(curl, CURLOPT_READFUNCTION, readfunc);

		//这里限速 XX KB/s  
		//res = curl_easy_setopt(curl, CURLOPT_MAX_RECV_SPEED_LARGE, (curl_off_t)down_speed * 1024);

		//设置进度回调
		res = curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, ProgressCallbackUpload);
		res = curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, this);

		//设置回调函数
		res = curl_easy_setopt(curl, CURLOPT_READFUNCTION, UploadCallback);
		res = curl_easy_setopt(curl, CURLOPT_READDATA, file);

		res = curl_easy_setopt(curl, CURLOPT_FTPPORT, "-"); /* disable passive mode */
		res = curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, 1L);
		res = curl_easy_setopt(curl, CURLOPT_VERBOSE, TRUE);

		/////////////////////
		res = curl_easy_setopt(curl, CURLOPT_APPEND, 0L);
		res = curl_easy_perform(curl);

		///////////////
		char chErrMsg[1024] = { 0 };
		const char* pError = curl_easy_strerror(res);

		sprintf_s(chErrMsg, 1024, "[upload] %s, file:[%s]", pError, sFile.c_str());
		ShowResult(res, chErrMsg);

		//执行http请求
		if (res != CURLE_OK)
		{
			fclose(file);
			curl_easy_cleanup(curl);
			curl_slist_free_all(headers);
			return res;
		}

		fclose(file);

		//释放资源
		curl_easy_cleanup(curl);
		curl_slist_free_all(headers);
		headers = NULL;
	}

	return res;
}

void CFtp::setCallbackUpload(v8::Local<v8::Function> js_callback)
{
	v8::Local<v8::Value> vFuncName = js_callback->GetName();
	v8::String::Utf8Value vStrName(vFuncName);
	const char* strName = *vStrName;
	m_strCallbackNameUpload = strName;

	m_isolate = js_callback->GetIsolate();

	return;
}

void CFtp::SetCallbackResult(v8::Local<v8::Function> js_callback)
{
	v8::Local<v8::Value> vFuncName = js_callback->GetName();
	v8::String::Utf8Value vStrName(vFuncName);
	const char* strName = *vStrName;
	m_strCallbackNameResult = strName;

	m_isolate = js_callback->GetIsolate();

	return;
}

void CFtp::ShowResult(int nCurlCode, const char* szErrMsg)
{
	if (!m_strCallbackNameResult.empty())
	{
		v8::Isolate* isolate = m_isolate;
		v8::Local<v8::Context> context = isolate->GetCurrentContext();

		v8::Local<v8::String> funName = v8::String::NewFromUtf8(isolate, m_strCallbackNameResult.c_str(), v8::NewStringType::kNormal).ToLocalChecked();

		v8::Context::Scope context_scope(context);

		v8::Local<v8::Object> gObj = context->Global();
		v8::Local<v8::Value> value = gObj->Get(funName);
		v8::Local<v8::Function> fun_execute = v8::Local<v8::Function>::Cast(value);

		//
		v8::Local<v8::Value> args[2];

		v8::Local<v8::Number> nCode = v8::Number::New(isolate, nCurlCode);

		string str = MbToUtf8(szErrMsg);
		v8::Local<v8::String> strMsg = v8::String::NewFromUtf8(m_isolate, str.c_str(), v8::NewStringType::kNormal).ToLocalChecked();

		args[0] = nCode;
		args[1] = strMsg;
		fun_execute->Call(gObj, 2, args);
	}
}

};//namespace curl