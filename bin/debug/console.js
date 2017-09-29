//
// Copyright (c) 2013-2016 Pavel Medvedev. All rights reserved.
//
// This file is part of v8pp (https://github.com/pmed/v8pp) project.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
var console = require('console');
console.log("load console.dll ok!");
/*console.log('yo', 'baby', 4.2, null);

var c2 = require('console');
c2.log('conker 2');

function pando() {
    this.friend = 1;
    this.masto = require('console');
    this.whack = function() {
        this.masto.log('truckman')
    };
}

var c = new pando();
c.whack();
*/

function strlen(str){
    var len = 0;
    for (var i=0; i<str.length; i++) { 
     var c = str.charCodeAt(i); 
    //单字节加1 
     if ((c >= 0x0001 && c <= 0x007e) || (0xff60<=c && c<=0xff9f)) { 
       len++; 
     } 
     else { 
      len+=2; 
     } 
    } 
    return len;
}


//console.log(str);


//****************************************
//socket
//var socket = require('socket');
//socket.start();
//socket.send("hello hpsocket, ok");
//var clent_listener = new socket.client_listener();
//var clinet = socket.ssl_client(clent_listener);

//client.CleanupSSLContext();
/*client.SetupSSLContext(3, L"ssl-cert\\server.cer", L"ssl-cert\\server.key", L"123456", L"ssl-cert\\ca.crt");
client.Start(L"127.0.0.1", 5555);*/

//****************************************


//****************************************
var curl = require('curl');
var str = "";
ret = curl.post("http://192.168.35.115:3000/queryMsg/query", "_trunc=-1&_limit=10&_skip=0&_sort={\"createtime\":-1}&_doc=", 1, 0, str);

var json = require('json');
//解析json
var parse =  json.parse(ret);
if(parse.IsValid())
{
	//code -->string
	if(parse.IsString("code"))
	{
		var vCode = parse.GetItem("code");
		var sCode = parse.ToString(vCode);
		console.log(sCode);
	}
	
	//data -->array
	var vData = parse.GetItem("data");
	if(parse.IsArray(vData))
	{
		var nArrCount = parse.GetArrayCount(vData);
		for( i=0; i<nArrCount; i++)
		{
			//item
			var vObjItem = parse.GetArrayItem(vData, i);
			
			//_id -->string
			var vId = parse.GetObjItem(vObjItem, "_id");
			var sValueId = parse.ToString(vId);
			
			//isvalid -->bool
			var vIsValid = parse.GetObjItem(vObjItem, "isvalid");
			var bValid = parse.ToBool(vIsValid);
			
			//msgType -->string
			
			//title -->string
			var vTitle = parse.GetObjItem(vObjItem, "title");
			var sValueTitle = parse.ToString(vTitle);
			
			//content -->string
			var vContent = parse.GetObjItem(vObjItem, "content");
			var sValueContent = parse.ToString(vContent);
			
			//dataid -->int32
			var vDataId = parse.GetObjItem(vObjItem,"dataid");
			var nDataId = parse.ToInt32(vDataId);			
			
			//validfrom -->date
			var vDateFrom = parse.GetObjItem(vObjItem, "validfrom");
			if(parse.IsDate(vDateFrom))
			{
				var sDateFrom = parse.ToString(vDateFrom);
				console.log(sDateFrom);
			}
			
			console.log("array['data']: ", i+1, "isvalid=", bValid, ",dataid=",nDataId, ",_id=", sValueId, ", title=", sValueTitle,  "\r\n");
		}
	}
}
else
{
	console.log("invalid");
}
///*******************************************