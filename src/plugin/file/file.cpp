//
// Copyright (c) 2013-2016 Pavel Medvedev. All rights reserved.
//
// This file is part of v8pp (https://github.com/pmed/v8pp) project.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <v8pp/module.hpp>
#include <v8pp/class.hpp>
#include <v8pp/config.hpp>

#include <fstream>
#include <string>
using namespace std;

#include "Directory.hpp"


///////////////
//#define  ELPP_STL_LOGGING
#ifndef ELPP_NO_DEFAULT_LOG_FILE
#define ELPP_NO_DEFAULT_LOG_FILE
#endif

#define ELPP_DISABLE_LOG_FILE_FROM_ARG
#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

namespace file {

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

	void LogTrace(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		v8::HandleScope scope(args.GetIsolate());

		std::string sWrite;

		for (int i = 0; i < args.Length(); ++i)
		{
			//if (i > 0) stream_ << ' ';
			v8::String::Utf8Value str(args[i]);
			sWrite += *str;
		}

		std::string strConvert;
		std::string strTmp = sWrite;
		Utf8ToMb((char*)strTmp.c_str(), strlen(strTmp.c_str()), strConvert);

		LOG(TRACE) << strConvert;
	}

	void LogDebug(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		v8::HandleScope scope(args.GetIsolate());

		std::string sWrite;

		for (int i = 0; i < args.Length(); ++i)
		{
			//if (i > 0) stream_ << ' ';
			v8::String::Utf8Value str(args[i]);
			sWrite += *str;
		}

		std::string strConvert;
		std::string strTmp = sWrite;
		Utf8ToMb((char*)strTmp.c_str(), strlen(strTmp.c_str()), strConvert);

		LOG(DEBUG) << strConvert;
	}

	void LogError(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		v8::HandleScope scope(args.GetIsolate());

		std::string sWrite;

		for (int i = 0; i < args.Length(); ++i)
		{
			//if (i > 0) stream_ << ' ';
			v8::String::Utf8Value str(args[i]);
			sWrite += *str;
		}

		std::string strConvert;
		std::string strTmp = sWrite;
		Utf8ToMb((char*)strTmp.c_str(), strlen(strTmp.c_str()), strConvert);

		LOG(ERROR) << strConvert;
	}

	void LogWarning(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		v8::HandleScope scope(args.GetIsolate());

		std::string sWrite;

		for (int i = 0; i < args.Length(); ++i)
		{
			//if (i > 0) stream_ << ' ';
			v8::String::Utf8Value str(args[i]);
			sWrite += *str;
		}

		std::string strConvert;
		std::string strTmp = sWrite;
		Utf8ToMb((char*)strTmp.c_str(), strlen(strTmp.c_str()), strConvert);

		LOG(WARNING) << strConvert;
	}

	void LogInfo(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		v8::HandleScope scope(args.GetIsolate());

		std::string sWrite;

		for (int i = 0; i < args.Length(); ++i)
		{
			//if (i > 0) stream_ << ' ';
			v8::String::Utf8Value str(args[i]);
			sWrite += *str;
		}

		std::string strConvert;
		std::string strTmp = sWrite;
		Utf8ToMb((char*)strTmp.c_str(), strlen(strTmp.c_str()), strConvert);

		LOG(INFO) << strConvert;
	}

	//////////////////////////

	

class file_base
{
public:
	bool is_open() const { return stream_.is_open(); }
	bool good() const { return stream_.good(); }
	bool eof() const { return stream_.eof(); }
	void close() { stream_.close(); }

protected:
	std::fstream stream_;
};

class file_writer : public file_base
{
public:
	explicit file_writer(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		if (args.Length() == 1)
		{
			v8::String::Utf8Value str(args[0]);
			open(*str);
		}
	}

	bool open(char const* path)
	{
		stream_.open(path, std::ios_base::out);
		return stream_.good();
	}

	void print(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		v8::HandleScope scope(args.GetIsolate());

		std::string sWrite;

		for (int i = 0; i < args.Length(); ++i)
		{
			//if (i > 0) stream_ << ' ';
			v8::String::Utf8Value str(args[i]);
			sWrite += *str;
		}

		std::string strConvert;
		std::string strTmp = sWrite;
		Utf8ToMb((char*)strTmp.c_str(), strlen(strTmp.c_str()), strConvert);

		//
		SYSTEMTIME tm;
		GetLocalTime(&tm);
		char chTime[64] = { 0 };
		sprintf_s(chTime, 64, "[%02d:%02d:%02d:%3d] ", tm.wHour, tm.wMinute, tm.wSecond, tm.wMilliseconds);
		stream_ << chTime << strConvert.c_str();

		LOG(TRACE) << strConvert;
	}

	void println(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		print(args);
		stream_ << std::endl;
	}
};

class file_reader : public file_base
{
public:
	explicit file_reader(char const* path)
	{
		open(path);
	}

	bool open(const char* path)
	{
		stream_.open(path, std::ios_base::in);
		return stream_.good();
	}

	v8::Handle<v8::Value> getline(v8::Isolate* isolate)
	{
		if ( stream_.good() && ! stream_.eof())
		{
			std::string line;
			std::getline(stream_, line);
			return v8pp::to_v8(isolate, line);
		}
		else
		{
			return v8::Undefined(isolate);
		}
	}
};

v8::Handle<v8::Value> init(v8::Isolate* isolate)
{
	v8::EscapableHandleScope scope(isolate);

	// file_base binding, no .ctor() specified, object creation disallowed in JavaScript
	v8pp::class_<file_base> file_base_class(isolate);
	file_base_class
		.set("close", &file_base::close)
		.set("good", &file_base::good)
		.set("is_open", &file_base::is_open)
		.set("eof", &file_base::eof)
		;

	// .ctor<> template arguments declares types of file_writer constructor
	// file_writer inherits from file_base_class
	v8pp::class_<file_writer> file_writer_class(isolate);
	file_writer_class
		.ctor<v8::FunctionCallbackInfo<v8::Value> const&>()
		.inherit<file_base>()
		.set("open", &file_writer::open)
		.set("print", &file_writer::print)
		.set("println", &file_writer::println)
		;

	// .ctor<> template arguments declares types of file_reader constructor.
	// file_base inherits from file_base_class
	v8pp::class_<file_reader> file_reader_class(isolate);
	file_reader_class
		.ctor<char const*>()
		.inherit<file_base>()
		.set("open", &file_reader::open)
		.set("getln", &file_reader::getline)
		;

	// Create a module to add classes and functions to and return a
	// new instance of the module to be embedded into the v8 context
	v8pp::module m(isolate);
	m.set("rename", [](char const* src, char const* dest) -> bool
	{
		return std::rename(src, dest) == 0;
	});
	m.set("writer", file_writer_class);
	m.set("reader", file_reader_class);
	//
	m.set("trace", &LogTrace);
	m.set("info", &LogInfo);
	m.set("debug", &LogDebug);
	m.set("error", &LogError);
	m.set("warning", &LogWarning);
	//
	m.set("GetCurDir", &GetCurDir);

	return scope.Escape(m.new_instance());
}

} // namespace file

#include <vector>
V8PP_PLUGIN_INIT(v8::Isolate* isolate)
{
 	el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);
	el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
 	el::Configurations conf("my_log.conf");
	el::Loggers::reconfigureAllLoggers(conf);
	

	//el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "%datetime %level: %msg");

// 	LOG(TRACE) << "***** trace log  *****";
// 	LOG(DEBUG) << "***** debug log  *****";
// 	LOG(ERROR) << "***** error log  *****";
// 	LOG(WARNING) << "***** warning log  *****";
// 	LOG(INFO) << "***** info log  *****";

	/*el::Logger* defaultLogger = el::Loggers::getLogger("default");

	std::vector<int> i;
	i.push_back(1);
	i.push_back(2);

	/// 记录STL容器数据  
	defaultLogger->warn("My first ultimate log message %v %v %v", 123, 222, i);

	// 利用转义字符输出 % 和 %v  
	defaultLogger->info("My first ultimate log message %% %%v %v %v", 123, 222);
	*/
	return file::init(isolate);
}
