////////////////////////
////////////////////////
var g_rooturl = "https://kyfw.12306.cn";

var file     = require('file');
var console  = require('console');
var curl     = require('curl');

//
var download = curl.download();

//初始化页面
function init()
{
	var ret = curl.post(g_rooturl + "/login/init", "", 0, 1);
}

//登陆前检查
function uamtk()
{
	var ret = curl.post(g_rooturl + "/passport/web/auth/uamtk", "appid=otn", 1, 0);
	//console.log(ret);
	file.trace("uamtk:" + ret);
}

//下载js
function dl_login_js()
{
	var ret = curl.post(g_rooturl + "/otn/resources/js/newpasscode/captcha_js.js", "_=1507899235150", 0, 0);
	//console.log(ret);
	file.trace("dl_login_code_js:");
}

//下载登陆验证码图片
function dl_login_code_pic()
{
	download.start(g_rooturl + "/passport/captcha/captcha-image?login_site=E&module=login&rand=sjrand&0.005265091371501107=", 
	"./login_code.png");
	//console.log("dl_login_code_pic");
	file.trace("dl_login_code_pic:");
}

//检查选择的验证码
function captcha_check(point)
{
	file.trace("captcha_check: param=" + point);
	var strParam = "answer=109,112&login_site=E&rand=sjrand";
	file.trace(g_rooturl + "/passport/captcha/captcha-check?" + strParam);
	var ret = curl.post(g_rooturl + "/passport/captcha/captcha-check", strParam, 1, 0);
	//console.log(ret);
	file.trace("captcha_check:" + ret);
}

//login
function login(account , pwd)
{
	//file.trace("login: param=" + account + ", "+ pwd);
	var strParam = "username=" + account + "&password=" + pwd + "&appid=otn";
	var ret = curl.post(g_rooturl + "/passport/web/login", strParam, 1, 0);
	//console.log(ret);
	file.trace("login:" + ret);
}

//init();
//uamtk();
//dl_login_js();
dl_login_code_pic();

var input = console.cin();

file.trace("cin>>" + input);

captcha_check(input);
login("wang70", "xxde");

//run("login2.js");
