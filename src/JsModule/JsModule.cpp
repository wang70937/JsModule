// JsModule.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "JsModule.h"
#include <iostream>
#include <string>

using namespace v8;
using namespace std;

CJsModule* CJsModule::s_pJsModule = NULL;

CJsModule::CJsModule()
{
	m_context = new v8pp::context;
	m_isolate = m_context->isolate();

	m_context->set_lib_path("./plugin");
}

CJsModule::~CJsModule()
{
}

CJsModule* CJsModule::GetInstance()
{
// 	if (s_pJsModule == NULL)
// 	{
// 		s_pJsModule = new CJsModule();
// 	}
// 
// 	return s_pJsModule;
	CJsModule js;
	return &js;
}

void V8_init()
{
  	v8::V8::InitializeICU();
  
	//platform不能为局部对象，否则编译脚本有问题
  	static std::unique_ptr<v8::Platform> platform(v8::platform::CreateDefaultPlatform());
  	v8::V8::InitializePlatform(platform.get());
  	v8::V8::Initialize();
}

void V8_uninit()
{
 	v8::V8::Dispose();
 	v8::V8::ShutdownPlatform();
}

template<typename T>
T CJsModule::run_script(v8pp::context& context, std::string const& source)
{
	v8::Isolate* isolate = context.isolate();

	v8::HandleScope scope(isolate);
	v8::TryCatch try_catch;
	v8::Handle<v8::Value> result = context.run_script(source);
	if (try_catch.HasCaught())
	{
		std::string const msg = v8pp::from_v8<std::string>(isolate,
			try_catch.Exception()->ToString());
		std::cout<<std::endl << msg << std::endl;
		throw std::runtime_error(msg);
	}
	return v8pp::from_v8<T>(isolate, result);
}

int CJsModule::V8_execute(const char* str)
{
	v8::Local<Value> v = run_script<v8::Local<Value>>(*m_context, str);
	return 1;
}

void CJsModule::V8_executeFile(const char* strFile)
{
	//v8pp::context* context = new v8pp::context(m_context->isolate());
	//v8::HandleScope scope(m_context->isolate());

	FILE* file = NULL;
	errno_t err = fopen_s(&file, strFile, "rb");
	if (err)
	{
		return;
	}

	fseek(file, 0, SEEK_END);
	int nLen = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* p = new char[nLen + 1];
	memset(p, 0, nLen + 1);
	fread_s(p, nLen, 1, nLen, file);
	fclose(file);

	std::string strContent(p, nLen);
	delete[] p;
	p = NULL;

	
	v8::Local<Value> a = run_script<v8::Local<Value>>(*m_context, strContent);
	
//	v8pp::context context;
//	context.run_file(strFile);
}

void CJsModule::V8_executeFileFunction(const char* strFile, const char* szFuncName, const char *fmt, .../*const char* szParam*/)
{
	char pszDest[4096] = { 0 };
	int DestLen = 4096;

	va_list args;
	va_start(args, fmt);
	_vsnprintf_s(pszDest, 4096,  DestLen, fmt, args);
	va_end(args);

	/////////////
	vector<string> vecParam;
	string sTmp(pszDest);
	while (sTmp.length() > 0)
	{
		int nPos = sTmp.find('|');
		if (nPos == -1)
		{
			break;
		}

		if ( nPos > 1 )
		{
			string sParam = sTmp.substr(0, nPos);
			vecParam.push_back(sParam);
		}

		sTmp = sTmp.substr(nPos + 1, sTmp.length() - nPos - 1);
	}

	if (!sTmp.empty())
	{
		vecParam.push_back(sTmp);
	}

	/////////////
//	V8_executeFile(strFile);

	HandleScope handle_scope(m_isolate);

	Local<Context> context = m_isolate->GetCurrentContext();

	// 关联context
	Context::Scope context_scope(context);

	Local<Object> gObj = context->Global();
	Local<Value> value = gObj->Get(String::NewFromUtf8(m_isolate, szFuncName, NewStringType::kNormal).ToLocalChecked());
	if (!value.IsEmpty())
	{
		bool bObj = value->IsObject();
		bool bString = value->IsString();
		bool bName = value->IsName();
		bool bUndefined = value->IsUndefined();
		bool bNull = value->IsNull();
		bool bSymbol = value->IsSymbol();
		bool bExt = value->IsExternal();
		if (value->IsFunction())
		{
			Local<Function> fun_execute = Local<Function>::Cast(value);

			int nParamCount = (int)(vecParam.size());

			switch (nParamCount)
			{
			case 1:
			{
				Local<Value> args[1];

				vector<string>::iterator itVector = vecParam.begin();
				for (int i = 0; itVector != vecParam.end(); ++itVector, ++i)
				{
					string& sItem = (*itVector);
					Local<String> obj = String::NewFromUtf8(m_isolate, sItem.c_str(), NewStringType::kNormal).ToLocalChecked();
					args[i] = obj;
				}

				fun_execute->Call(gObj, 1, args);
			}
				break;
			case 2:
			{
				Local<Value> args[2];

				vector<string>::iterator itVector = vecParam.begin();
				for (int i = 0; itVector != vecParam.end(); ++itVector, ++i)
				{
					string& sItem = (*itVector);
					Local<String> obj = String::NewFromUtf8(m_isolate, sItem.c_str(), NewStringType::kNormal).ToLocalChecked();
					args[i] = obj;
				}

				fun_execute->Call(gObj, 2, args);
			}
				break;
			case 0:
			default:
			{
				Local<Value> args[1];
				fun_execute->Call(gObj, 0, args);
			}
			break;
				break;
			}			
		}
	}
}

v8::Isolate* CJsModule::GetIsolate()
{
	return m_isolate;
}

