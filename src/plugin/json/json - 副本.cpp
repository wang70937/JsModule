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
#include <v8pp/class.hpp>

#include <string>
using namespace std;

#include "json/Json.h"

using namespace v8;

namespace json {

class json_parse
{
public:
	json_parse(){};
	~json_parse(){};

	bool IsJson(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		//m_isolate = args.GetIsolate();
		v8::HandleScope handle_scope(args.GetIsolate());

		int nArgsCount = args.Length();
		if (nArgsCount != 1)
		{
			return false;
		}

		//////////////////
		//param 1
		v8::String::Utf8Value strParam(args[0]);

		char* p = *strParam;
		int nLen = strParam.length();

		return m_reader.parse(*strParam, m_root);
	}


	v8::Handle<v8::Value> Get(v8::Isolate* isolate, const char* szItem)
	{
		Json::ValueType vType = m_root[szItem].type();

		string ss = m_root["data"].toStyledString();

		if (vType == Json::stringValue)
		{
			string str = m_root[szItem].asString();
			return v8pp::to_v8(isolate, str);
		}
		else if (vType == Json::intValue)
		{
			int n = m_root[szItem].asInt();
			return v8pp::to_v8(isolate, n);
		}
		else if (vType == Json::arrayValue)
		{
			Json::Value vArr = m_root[szItem];
			return v8pp::to_v8(isolate, vArr);
		}
		
		return v8pp::to_v8(isolate, NULL);
	}

private:
	Json::Reader	m_reader;
	string			m_str;
	Json::Value		m_root;
	//v8::Isolate*	m_isolate;
};

v8::Handle<v8::Object> JsonValueCtor(v8::FunctionCallbackInfo<v8::Value> const& args)
{
	v8::Isolate* isolate = args.GetIsolate();

	if (args[0]->IsString())
	{
		//Json::Value* v = new Json::Value(Json::ValueType::stringValue);
		v8::Local<String> str = args[0]->ToString();
		String::Utf8Value utf8(str);
		Json::Value* v = new Json::Value(*utf8);

		string s = v8pp::from_v8<string>(isolate, str);
		*v = s;
		return v8pp::class_<Json::Value>::import_external(isolate, v);
	}
	else if (args[0]->IsBoolean())
	{
		Json::Value* v = new Json::Value(Json::ValueType::booleanValue);
		v8::Local<Boolean> b = args[0]->ToBoolean();
		*v = b->Value();
		return v8pp::class_<Json::Value>::import_external(isolate, v);
	}
	else if (args[0]->IsInt32())
	{
		Json::Value* v = new Json::Value(Json::ValueType::intValue);
		v8::Local<Int32> n = args[0]->ToInt32();
		*v = n->Value();
		return v8pp::class_<Json::Value>::import_external(isolate, v);
	}
	else if (args[0]->IsObject())
	{
		return v8pp::class_<Json::Value>::import_external(isolate, new Json::Value(Json::ValueType::objectValue));
	}
	else if (args[0]->IsNumber())
	{
		Json::Value* v = new Json::Value(Json::ValueType::realValue);
		v8::Local<Number> num = args[0]->ToNumber();
		*v = num->Value();
		return v8pp::class_<Json::Value>::import_external(isolate, v);
	}
	else if (args[0]->IsArray())
	{
		return v8pp::class_<Json::Value>::import_external(isolate, new Json::Value(Json::ValueType::arrayValue));
	}
	else
	{
		throw std::runtime_error("unsupported arguments");
	}
}

v8::Handle<v8::Value> init(v8::Isolate* isolate)
{
	v8::EscapableHandleScope scope(isolate);

	////////////
	//jsonValue
	v8pp::class_<Json::Value> JsonValue_class(isolate);
	JsonValue_class
		.ctor<Json::ValueType>()
		/*.ctor<Json::Int>()
		.ctor<Json::UInt>()
		.ctor<double>()
		.ctor<const char*>()
		.ctor<const char*, const char*>()
		.ctor<const std::string &>()
		.ctor<const Json::StaticString &>()
		.ctor<bool>()
		.ctor<const Json::Value&>()*/
		.set("ctor", &JsonValueCtor)
		.set("isObject", &Json::Value::isObject)
		.set("isString", &Json::Value::isString)
		.set("isBool", &Json::Value::isBool)
		.set("isNull", &Json::Value::isNull)
		.set("isInt", &Json::Value::isInt)
		.set("isInt64", &Json::Value::isInt64)
		.set("isUInt", &Json::Value::isUInt)
		.set("isUInt64", &Json::Value::isUInt64)
		.set("isIntegral", &Json::Value::isIntegral)
		.set("isDouble", &Json::Value::isDouble)
		.set("isNumeric", &Json::Value::isNumeric)
		.set("isString", &Json::Value::isString)
		.set("isArray", &Json::Value::isArray)
		.set("isObject", &Json::Value::isObject)
		.set("empty", &Json::Value::empty)
		.set("asString", (std::string(Json::Value::*)() const)&Json::Value::asString)
		.set("asCString", (const char* (Json::Value::*)() const)&Json::Value::asCString)
		.set("asUInt", &Json::Value::asUInt)
		.set("asInt", &Json::Value::asInt)
		.set("asInt64", &Json::Value::asInt64)
		.set("asUInt64", &Json::Value::asUInt64)
		.set("asBool", &Json::Value::asBool)
		.set("asDouble", &Json::Value::asDouble)
		.set("asFloat", &Json::Value::asFloat)
		//.set("[]", (Json::Value& (Json::Value::*)(Json::UInt))&Json::Value::operator [])
		.set("=", (Json::Value& (Json::Value::*)(const Json::Value&))&Json::Value::operator =)
		.set("[]", (Json::Value& (Json::Value::*)(const char *))&Json::Value::operator[])
		.set("isMember", (bool (Json::Value::*)(const char*) const)&Json::Value::isMember)
		.set("toStyledString", (std::string(Json::Value::*)() const)&Json::Value::toStyledString)
		.set("type", (Json::ValueType(Json::Value::*)() const)&Json::Value::type)
		.set("isConvertibleTo", (bool (Json::Value::*)(Json::ValueType) const)&Json::Value::isConvertibleTo)
		.set("get", (Json::Value(Json::Value::*)(const char*, const Json::Value&) const)&Json::Value::get)
		.set("get2", (Json::Value(Json::Value::*)(const std::string&, const Json::Value&) const)&Json::Value::get)
		.set("append", (Json::Value& (Json::Value::*)(const Json::Value&))&Json::Value::append)
		.set("size", (Json::ArrayIndex(Json::Value::*)() const)&Json::Value::size)
		;

	///////////
	v8pp::class_<json_parse> json_parse_class(isolate);
	json_parse_class
		.ctor()
		.set("IsJson", &json_parse::IsJson)
		.set("Get", &json_parse::Get)
		;

	////////////
	v8pp::module m(isolate);
	m.set("json_parse", json_parse_class);
	m.set("JsonValue", JsonValue_class);

	
	
	return scope.Escape(m.new_instance());
}

} // namespace console

V8PP_PLUGIN_INIT(v8::Isolate* isolate)
{
	return json::init(isolate);
}
