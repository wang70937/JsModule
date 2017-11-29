//****************************************

var console = require('console');
var curl = require('curl');
var file = require('file');
var json = require('json');
var jstring = require('string');
var socket = require('socket');
var compress = require('compress');
var damatu = require('damatu');
var common = require('common');



///////////////////////////////////////////
var pips = common.pips();
pips.callback(callback_pips);
pips.start("\"D:\\Program Files\\Microsoft Visual Studio 12.0\\Common7\\IDE\\devenv.com\" \"E:\\github\\soui\\soui.sln\" /Clean Debug");

//pips.start("ping.exe www.baidu.com");

function callback_pips(finished, str)
{
	if(finished)
	{
		file.trace("[pips] : finished!");
	}
	else
	{
		//console.log(str);
		file.trace("[pips]: ", str);
	}
}


//****************************************
//            ftp module
//****************************************
//download
var ftp = curl.ftp();
ftp.callbackDownload(callback_ftp_download);
ftp.callbackResult(callback_ftp_result);
ftp.startDownload("ftp://127.0.0.1:21//abc.zip", "c:\\abcde.zip", "user", "pwd");

function callback_ftp_download(now, total)
{
	var progress = now / total * 100;
	file.trace("[ftp_download]:" + progress.toFixed(2) + "%" +" now=" + now + ", total=" + total);
}

//upload
var ftp2 = curl.ftp();
ftp2.callbackUpload(callback_ftp_upload);
ftp2.callbackResult(callback_ftp_result);
ftp2.startUpload("ftp://127.0.0.1:21/test.exe", "C:\\Users\\Administrator\\Desktop\\test.exe", "user", "pwd");

function callback_ftp_upload(now, total)
{
	var progress = now / total * 100;
	file.trace("[ftp_upload]:" + progress.toFixed(2) + "%" +" now=" + now + ", total=" + total);
}

function callback_ftp_result(code, msg)
{
	file.trace("[ftp_result]: code=" + code + ", msg=" + msg);
}

//console.cin();
	
