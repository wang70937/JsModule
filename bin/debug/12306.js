////////////////////////
////////////////////////
var g_rooturl = "https://kyfw.12306.cn";

var file     = require('file');
var console  = require('console');
var curl     = require('curl');
var damatu   = require('damatu');
var string	 = require('string');

//
var download = curl.download();

//初始化页面
function init()
{
	var vurl = g_rooturl + "/login/init";
	var ret = curl.post(g_rooturl, vurl, "", 0, 1);
}

//登陆前检查
function uamtk()
{
	var vurl = g_rooturl + "/passport/web/auth/uamtk";
	var ret = curl.post(g_rooturl, vurl, "appid=otn", 1, 0);
	//console.log(ret);
	file.trace("uamtk:", ret);
}

//下载js
function dl_login_js()
{
	var vurl = g_rooturl + "/otn/resources/js/newpasscode/captcha_js.js";
	var ret = curl.post(g_rooturl, vurl, "_=1507899235150", 0, 0);
	//console.log(ret);
	file.trace("dl_login_code_js:");
}

//下载登陆验证码图片
function dl_login_code_pic(picfilename)
{
	var vurl = g_rooturl + "/passport/captcha/captcha-image?login_site=E&module=login&rand=sjrand&0.005265091371501107=";
	download.start( vurl, picfilename);
	//console.log("dl_login_code_pic");
	file.trace("dl_login_code_pic:");
}

//检查选择的验证码
function captcha_check(point)
{
	file.trace("captcha_check: param=" + point);
	var strParam = "answer=" + point + "&login_site=E&rand=sjrand";
	file.trace(g_rooturl, "/passport/captcha/captcha-check?", strParam);

	var vurl = g_rooturl + "/passport/captcha/captcha-check";
	var ret = curl.post(g_rooturl, vurl, strParam, 1, 0);
	//console.log(ret);
	file.trace("captcha_check:",ret);
}

//login
function login(account , pwd)
{
	var vurl = g_rooturl + "/passport/web/login";
	var strParam = "username=" + account + "&password=" + pwd + "&appid=otn";
	var ret = curl.post(g_rooturl, vurl, strParam, 1, 0);
	//console.log(ret);
	file.trace("login:", ret);
}

init();
uamtk();
//dl_login_js();
dl_login_code_pic("login_code.jpg");

//****************************************
//            damatu module
//****************************************
//init
var nInitCode = damatu.init("xhhzdl", "45772fd1a7f06da0f699c67bdb283c75");
file.trace("damatu : init ret=", nInitCode);

//login
var nLoginCode = damatu.login("test", "test");
file.trace("damatu : login ret=", nLoginCode);

//querybalance
var nQueryBalanceCode = damatu.querybalance();
file.trace("damatu : querybalance ret=", nQueryBalanceCode);


//decodefile
var nDecodeFile = damatu.decodefile("./login_code.jpg", 10, 287);
file.trace("damatu : decodefile ret=", nDecodeFile);


//d2file
//var nDecodeFile = damatu.d2file("45772fd1a7f06da0f699c67bdb283c75", "test", "test", "login_code.jpg", 60, 287);
//file.trace("damatu : d2file ret=", nDecodeFile);

//gerresult
var nResult = damatu.getresult(nDecodeFile, 10);
file.trace("damatu : getresult ret=", nResult);

//logoff
var nLogoffCode = damatu.logoff();
file.trace("damatu : logoff ret=", nLogoffCode);

//uninit
damatu.uninit();
file.trace("damatu : uninit");

//****************************************
var vRandCode = string.replace(nResult, "|", ",");
captcha_check(vRandCode);
login("aaaaa", "*****");

