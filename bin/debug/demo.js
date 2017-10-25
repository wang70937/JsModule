//****************************************
var console = require('console');
var curl = require('curl');
var file = require('file');
var json = require('json');
var jstring = require('string');
var socket = require('socket');
var compress = require('compress');




//****************************************
//            file module
//****************************************
//trace
file.trace("***  start  ***");
file.info("***  info  ***");
file.debug("*** debug   ***");
file.error("*** error   ***");
file.warning("*** warning  ***");
//GetCurDir
var vCurPath = file.GetCurDir();
//vCurPath = jstring.AnsiToUtf8(vCurPath);
file.trace("file.GetCurDir() : " + vCurPath);
//writer
var log = new file.writer();
var logname = "2017-09-29" + ".log";
log.open(logname);
log.print("写日誌類first write log file.", logname);
log.close();


//****************************************
//            console module
//****************************************
//log
console.log("console log info. pls input a string...");
//cin
//var vInput = console.cin();



//****************************************
//            curl module
//****************************************
//download
var download =  curl.download();
download.callback(download_callback);
download.start("https://ss0.bdstatic.com/5aV1bjqh_Q23odCf/static/superman/img/logo/bd_logo1_31bdc765.png", "./baidu.png");

function download_callback(dlnow, dltotal)
{
	var progress = dlnow / dltotal * 100;
	console.log("download percent is " + progress.toFixed(2) + "%");
}


//****************************************
//            json module
//****************************************
var vRet = curl.post("192.168.35.99", "http://192.168.35.99:3000/queryMsg/query", "_trunc=-1&_limit=10", 0, 1);
//parse
var vRoot = json.parse(vRet);
//IsValid
if(vRoot.IsValid())
{
	//IsString
	if(vRoot.IsString("code"))
	{
		//GetItem
		var vCode = vRoot.GetItem("code");
		//ToString
		var sCode = vRoot.ToString(vCode);
	}
	
	
	var vData = vRoot.GetItem("data");
	//IsArray
	if(vRoot.IsArray(vData))
	{
		//GetArrayCount
		var nArrCount = vRoot.GetArrayCount(vData);
		for( i=0; i<nArrCount; i++)
		{
			//GetArrayItem
			var vObjItem = vRoot.GetArrayItem(vData, i);
			
			//ToString
			var vId = vRoot.GetObjItem(vObjItem, "_id");
			var sValueId = vRoot.ToString(vId);
			
			//ToBool
			var vIsValid = vRoot.GetObjItem(vObjItem, "isvalid");
			var bValid = vRoot.ToBool(vIsValid);
			
			//msgType -->string
			
			//ToString
			var vTitle = vRoot.GetObjItem(vObjItem, "title");
			var sValueTitle = vRoot.ToString(vTitle);
			
			//ToString
			var vContent = vRoot.GetObjItem(vObjItem, "content");
			var sValueContent = vRoot.ToString(vContent);
			
			//ToInt32
			var vDataId = vRoot.GetObjItem(vObjItem,"dataid");
			var nDataId = vRoot.ToInt32(vDataId);			
			
			//IsData
			var vDateFrom = vRoot.GetObjItem(vObjItem, "validfrom");
			if(vRoot.IsDate(vDateFrom))
			{
				var sDateFrom = vRoot.ToString(vDateFrom);
				console.log(sDateFrom);
			}
			
			console.log("array['data']: ", i+1, "isvalid=", bValid, ",dataid=",nDataId, ",_id=", sValueId, ", title=", sValueTitle,  "\r\n");
		}
	}
}


//****************************************
//            compress module
//****************************************
//CreateZipFile
compress.CreateZipFile("压缩多个文件.zip", 0, "baidu.png", "file.js", "libcurl.dll",  "demod.exe", "demo - 副本.js");
//compress.AddFileToZip("new.zip", "", "libssh2-a.dll", "libssh2.dll");
//CreateZipFileFromDir
compress.CreateZipFileFromDir("dir.zip", 0, "log");
//UncompressZip
compress.UncompressZip("dir.zip", 0, "解压缩目录");


//****************************************
//            socket module
//****************************************
//TcpClient
//function tcp_listener_on_send(len, len2)
//{
//	file.trace("tcp_listener_on_send callback: len=" + len);
//}

//var tcplistener =  socket.client_listener();

//var tcpclient = new socket.jstcpclient(tcplistener);
//tcpclient.SetCallbackOnSend(tcp_listener_on_send);
//tcpclient.Start("192.168.35.99", 4463);

//var clent_listener = new socket.client_listener();
//var clinet = socket.ssl_client(clent_listener);

//client.CleanupSSLContext();
//client.SetupSSLContext(3, "ssl-cert\\server.cer", "ssl-cert\\server.key", "123456", "ssl-cert\\ca.crt");
//client.Start("127.0.0.1", 5555);



//****************************************
//            other module
//****************************************
//console.cin();