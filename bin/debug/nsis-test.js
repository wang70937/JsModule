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


//****************************************
//            nsis module
//****************************************
//nsis.exe path
var path_nsis = "D:\\NSIS\\makensis.exe";
//.nsi file path
var path_nsi = "D:\\test.nsi";

//common.cmdpip("ping www.baidu.com -t", "");
var pips = common.pips();
pips.callback(callback_pips);
pips.start(path_nsis, path_nsi);
console.cin();
path_nsi = "D:\\test2.nsi";
pips.start(path_nsis, path_nsi);
//pips.start("ping www.baidu.com -t", "");

function callback_pips(str)
{
	file.trace("[pips callback]: ", str);
}

//console.cin();
	
