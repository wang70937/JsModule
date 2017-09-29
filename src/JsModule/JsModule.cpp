// JsModule.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "JsModule.h"
#include <iostream>

using namespace v8;

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
//	v8pp::context context;
//	v8::HandleScope scope(context.isolate());

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

