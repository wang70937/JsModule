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

	Json::Value& JsonGetItemValue(v8::Isolate* isolate, Json::Value& value, const char* str)
	{
		Json::Value* x = new Json::Value(Json::ValueType::nullValue);
		v8pp::class_<Json::Value>::import_external(isolate, x);
		*x = value[str];

		// 	v8::Local<v8::Object> x = v8pp::class_<Json::Value>::create_object(isolate, Json::ValueType::nullValue);
		// 	// return a reference to the object
		// 	Json::Value v = v8pp::from_v8<Json::Value>(isolate, x);
		// 	v = value[str];

		return *x;
	}

	void JsonSetItemValue(v8::Isolate* isolate, const char* szKey, Json::Value& vValue, Json::Value& v)
	{
		// 	Json::Value* x = new Json::Value(vValue);
		// 	v8pp::class_<Json::Value>::import_external(isolate, x);
		// 	v[szKey] = *x;
		v[szKey] = vValue;
	}

	void JsonSetItemArr(v8::Isolate* isolate, UINT nIndex, Json::Value& vValue, Json::Value& v)
	{
		// 	Json::Value* x = new Json::Value(vValue);
		// 	v8pp::class_<Json::Value>::import_external(isolate, x);
		// 	v[nIndex] = *x;
		v[nIndex] = vValue;
	}

	void JsonGetArrayItem(v8::Isolate* isolate, UINT nIndex, Json::Value& vValue, Json::Value& v)
	{
		vValue = v[nIndex];
	}

	string toStyledString(v8::Isolate* isolate, Json::Value& v)
	{
		string str = v.toStyledString();
		return str;
	}

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

	static Json::Value* create_JsonValue(v8::FunctionCallbackInfo<v8::Value> const& args)
	{
		/*if (args[0]->IsNumber())
		{
			Json::Value* v = new Json::Value(Json::ValueType::realValue);
			v8::Local<Number> num = args[0]->ToNumber();
			*v = num->Value();
			return v;
		}*/
		return new Json::Value;
	}

v8::Handle<v8::Value> init(v8::Isolate* isolate)
{
	v8::EscapableHandleScope scope(isolate);

	////////////
	//jsonValue
	v8pp::class_<Json::Value> JsonValue_class(isolate);
	JsonValue_class
		.ctor<Json::ValueType>()
		.ctor(&create_JsonValue)
		//.set("ctor", &JsonValueCtor)
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

	//jsonReader
	v8pp::class_<Json::Reader> JsonReader_class(isolate);
	JsonReader_class
		.ctor()
		.set("parse", (bool (Json::Reader::*)(const std::string&, Json::Value&, bool))&Json::Reader::parse)
		.set("parse2", (bool (Json::Reader::*)(const char*, const char*, Json::Value&, bool))&Json::Reader::parse)
		.set("parse3", (bool (Json::Reader::*)(std::istream &, Json::Value &, bool))&Json::Reader::parse)
		;

	////////////
	v8pp::module m(isolate);
	m.set("JsonValue", JsonValue_class);
	m.set("JsonReader", JsonReader_class);
 	m.set("JsonGetItemValue", &JsonGetItemValue);
 	m.set("JsonSetItemValue", &JsonSetItemValue);
 	m.set("JsonSetItemArr", &JsonSetItemArr);
 	m.set("JsonGetArrayItem", &JsonGetArrayItem);

	
	return scope.Escape(m.new_instance());
}

} // namespace console

V8PP_PLUGIN_INIT(v8::Isolate* isolate)
{
	return json::init(isolate);
}
