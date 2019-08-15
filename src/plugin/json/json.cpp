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
#include <v8pp/object.hpp>

#include <string>
using namespace std;

//#include "json/Json.h"
#include <v8pp/json.hpp>

using namespace v8;


namespace json {

	class parse
	{
	public:
		parse(v8::FunctionCallbackInfo<v8::Value> const& args)
		{
			m_isolate = args.GetIsolate();
			if (args.Length() == 1)
			{
				v8::String::Utf8Value str(args[0]);
				m_str = *str;
			}
		}
		~parse(){};

		//
		bool IsValid()
		{
			v8::Handle<v8::Value> v = v8pp::json_parse(m_isolate, m_str);
			if (v.IsEmpty())
				return false;
			bool bObj = v->IsObject();
// 			if (bObj)
// 			{
// 				m_root = v->ToObject();
// 			}
			return bObj;
		}

		bool FindItem(const char* szItem)
		{
			v8::Handle<v8::Value> v = v8pp::json_parse(m_isolate, m_str);
			v8::Handle<v8::Object> root = v->ToObject();

			v8::Local<v8::Object> vItem;
			v8pp::get_option(m_isolate, root, szItem, vItem);
			
			return !(vItem->IsUndefined() || vItem->IsNull());
		}

		//
// 		v8::Handle<v8::Object> GetRoot()
// 		{
// 			return m_root;
// 		}

		//
		v8::Handle<v8::Object> GetArrayItem(v8::Handle<v8::Object> obj, uint16_t nIndex)
		{
			v8::Local<v8::Value> vItem = obj->Get(nIndex);
			if (vItem->IsUndefined() || vItem->IsNull())
			{
				return vItem->ToObject();
			}
			return vItem->ToObject();
		}

		int GetArrayCount(v8::Handle<v8::Object> obj)
		{
			int nCount = 0;
			while (1)
			{
				v8::Local<v8::Value> vItem = obj->Get(nCount);
				if (vItem->IsUndefined() || vItem->IsNull())
				{
					break;
				}
				nCount++;
			}
			
			return nCount;
		}

		//
		v8::Handle<v8::Object> GetItem(const char* szItem)
		{
			v8::Handle<v8::Value> v = v8pp::json_parse(m_isolate, m_str);
			v8::Handle<v8::Object> root = v->ToObject();

			v8::Local<v8::Object> vItem;
			v8pp::get_option(m_isolate, root, szItem, vItem);

			if (vItem->IsNull() || vItem->IsUndefined())
			{
				return vItem;
			}

			return vItem;
		}

		//
		v8::Handle<v8::Object> GetObjItem(v8::Handle<v8::Object> obj, const char* szItem)
		{
			//v8::Handle<v8::Value> v = v8pp::json_parse(m_isolate, m_str);
			//v8::Handle<v8::Object> root = v->ToObject();

			v8::Local<v8::Object> vItem;
			v8pp::get_option(m_isolate, obj, szItem, vItem);

			if (vItem->IsNull() || vItem->IsUndefined())
			{
				return vItem;
			}

			return vItem;
		}

		//
		bool IsObject(v8::Handle<v8::Object> obj)
		{
			return obj->IsObject();
		}

		//
		bool IsArray(v8::Handle<v8::Object> obj)
		{
			return obj->IsArray();
		}

		//
		bool IsBool(v8::Handle<v8::Object> obj)
		{
			return obj->IsBoolean();
		}

		//
		bool IsNumber(v8::Handle<v8::Object> obj)
		{
			return obj->IsNumber();
		}

		//
		bool IsString(v8::Handle<v8::Object> obj)
		{
			return obj->IsString();
		}

		bool IsDate(v8::Handle<v8::Object> obj)
		{
			return obj->IsDate();
		}

		v8::Handle<v8::Value> ToString(v8::Handle<v8::Object> obj)
		{
			v8::String::Utf8Value str(obj);
			string s = *str;
			return v8pp::to_v8(m_isolate, s);
		}

		bool ToBool(v8::Handle<v8::Object> obj)
		{
			bool b = obj->IsBoolean();
			return b;
		}

		//
		bool IsInt32(v8::Handle<v8::Object> obj)
		{
			return obj->IsInt32();
		}

		int32_t ToInt32(v8::Handle<v8::Object> obj)
		{
			int32_t  n = obj->Int32Value();
			return  n;
		}

	private:
		string			m_str;
		v8::Isolate*	m_isolate;
		//v8::Handle<v8::Object>	m_root;
		v8::Handle<v8::Value>	m_value;
	};


v8::Handle<v8::Value> init(v8::Isolate* isolate)
{
	v8::EscapableHandleScope scope(isolate);

	////////////
	v8pp::class_<parse> parse_class(isolate);
	parse_class
		.ctor<v8::FunctionCallbackInfo<v8::Value> const&>()
		.set("IsValid", &parse::IsValid)
		.set("GetItem", &parse::GetItem)
		//.set("GetRoot", &parse::GetRoot)
		.set("FindItem", &parse::FindItem)
		.set("IsString", &parse::IsString)
		.set("ToString", &parse::ToString)
		.set("ToInt32", &parse::ToInt32)
		.set("IsInt32", &parse::IsInt32)
		.set("IsObj", &parse::IsObject)
		.set("IsArray", &parse::IsArray)
		.set("IsBool", &parse::IsBool)
		.set("ToBool", &parse::ToBool)
		.set("IsDate", &parse::IsDate)
		.set("IsNumber", &parse::IsNumber)
		.set("GetArrayItem", &parse::GetArrayItem)
		.set("GetArrayCount", &parse::GetArrayCount)
		.set("GetObjItem", &parse::GetObjItem)
		;

	////////////
	v8pp::module m(isolate);
	m.set("parse", parse_class);

	return scope.Escape(m.new_instance());
}

} // namespace json

V8PP_PLUGIN_INIT(v8::Isolate* isolate)
{
	return json::init(isolate);
}
