#include "DownloadFile.h"
#include <iostream>
#include "v8pp/convert.hpp"
#include "v8pp/call_v8.hpp"
#include "v8.h"

namespace curl{
CDownloadFile::CDownloadFile()
{
	m_bCancel = FALSE;
}


CDownloadFile::~CDownloadFile()
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

int CDownloadFile::ProgressCallback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	CDownloadFile* dd = (CDownloadFile*)clientp;

	if (dltotal > -0.1 && dltotal < 0.1)
	{
		return 0;
	}
	int nPos = (int)((dlnow / dltotal) * 100);
	//通知进度条更新下载进度  
	std::cout << "dltotal: " << (long)dltotal << " ---- dlnow:" << (long)dlnow << "---- " << nPos <<"%" << std::endl;

//	if (*dd->m_bCancel)
	{
		//1. 返回非0值就会终止 curl_easy_perform 执行  
//		return -2;
	}

	v8::Isolate* isolate = dd->m_isolate;
	v8::Handle<v8::Function> fun = v8::Function::New(isolate, v8_arg_count);
	v8::Local<v8::String> funName = v8::String::NewFromUtf8(isolate, dd->m_strCallbackName.c_str(), v8::NewStringType::kNormal).ToLocalChecked();
	fun->SetName(funName);

	v8::String::Utf8Value vStrName(fun);
	const char* strName = *vStrName;

	int nLineNo = fun->GetScriptLineNumber();
	int nColNo = fun->GetScriptColumnNumber();
	bool bBuiltin = fun->IsBuiltin();
	int nId = fun->ScriptId();
	v8::Local<v8::Value> va = fun->GetDisplayName();
	v8::String::Utf8Value vStrName2(va);
	const char* strName2 = *vStrName2;
	
	v8::Handle<v8::Value> vRet = v8pp::call_v8(isolate, fun, fun, nPos);
	v8::Local<v8::Int32> n = vRet->ToInt32();
	int nRet = n->Int32Value();
	return 0;
}

size_t CDownloadFile::DownloadCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam)
{
	FILE* fp = (FILE*)pParam;
	size_t nWrite = fwrite(pBuffer, nSize, nMemByte, fp);

	return nWrite;
}

int CDownloadFile::start(const char* szUrl, const char* szPath)
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

		FILE* file = NULL;
		errno_t err = fopen_s(&file, szPath, "wb");
		if (err != 0)
		{
			curl_easy_cleanup(curl);
			curl_slist_free_all(headers);
			return -4;
		}

		////////////////////////////////
	/*	v8::Handle<v8::Function> fun = v8::Function::New(js_callback->GetIsolate(), v8_arg_count);
		// invoke the callback to notify JS side as js_callback(error, result)
		v8::Local<v8::Object> receiver = js_callback;
		v8::Local<v8::Value> error = v8::Undefined(js_callback->GetIsolate());
		v8::Local<v8::Object> result; // fill the connect result ...      
		v8pp::call_v8(js_callback->GetIsolate(), js_callback, receiver, error, result);
		////////////////////////////////
*/
		//初始化cookie引擎
		//curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
		res = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		res = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);

		res = curl_easy_setopt(curl, CURLOPT_URL, szUrl);

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
		if (res != CURLE_OK)
		{
			const char* pError = curl_easy_strerror(res);
			OutputDebugStringA(pError);

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

void CDownloadFile::setCallback(v8::Local<v8::Function> js_callback)
{
	v8::Local<v8::Value> vFuncName = js_callback->GetName();
	v8::String::Utf8Value vStrName(vFuncName);
	const char* strName = *vStrName;
	m_strCallbackName = strName;

	/*v8::Handle<v8::Function> fun = v8::Function::New(js_callback->GetIsolate(), v8_arg_count);
	// invoke the callback to notify JS side as js_callback(error, result)
	v8::Local<v8::Object> receiver = js_callback;
	v8::Local<v8::Value> error = v8::Undefined(js_callback->GetIsolate());
	v8::Local<v8::Object> result; // fill the connect result ...      
	v8pp::call_v8(js_callback->GetIsolate(), js_callback, receiver, error, result);*/
	m_isolate = js_callback->GetIsolate();

	int nLineNo = js_callback->GetScriptLineNumber();
	int nColNo = js_callback->GetScriptColumnNumber();
	bool bBuiltin = js_callback->IsBuiltin();
	int nId = js_callback->ScriptId();
	v8::Local<v8::Value> va = js_callback->GetDebugName();
	
	return;
}

};//namespace curl