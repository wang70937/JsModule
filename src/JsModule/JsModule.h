#pragma once
#ifndef	_DLL_JSMODULE_H
#define _DLL_JSMODULE_H


#ifdef JSMODULE_EXPORTS
#define API_JS_MODULE			__declspec(dllexport)
#else
#define API_JS_MODULE			__declspec(dllimport)
#endif

#define API_JS_MODULE_FUN		extern "C" API_JS_MODULE

#include <v8.h>
#include <libplatform/libplatform.h>

#include "v8pp/context.hpp"

API_JS_MODULE_FUN void V8_init();

API_JS_MODULE_FUN void V8_uninit();

class API_JS_MODULE CJsModule
{
public:
	CJsModule();
	virtual ~CJsModule();

	static CJsModule* GetInstance();	

	template<typename T> 
	T run_script(v8pp::context& context, std::string const& source);

	int V8_execute(const char* str);

	void V8_executeFile(const char* strFile);

	void V8_executeFileFunction(const char* strFile, const char* szFuncName, const char *fmt, .../*const char* szParam*/);

	v8::Isolate* GetIsolate();

 private:
 	v8pp::context*	m_context;
 	v8::Isolate*	m_isolate;

	static CJsModule* s_pJsModule;
 } ;


#endif