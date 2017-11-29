////////////////////////
////////////////////////
var g_rooturl = "https://kyfw.12306.cn/otn";

var file     = require('file');
var console  = require('console');
var curl     = require('curl');
var damatu   = require('damatu');
var string	 = require('string');

//
var download = curl.download();


//下载登陆验证码图片
function getPassCodeNew(picfilename)
{
	var vurl = g_rooturl + "/passcodeNew/getPassCodeNew?module=login&rand=sjrand&0.5674695470370352";
	download.start( vurl, picfilename);
	file.trace("dl_login_code_pic:");
}

//初始化页面
function init()
{
	var vurl = g_rooturl + "/login/init";
	var ret = curl.post(g_rooturl, vurl, "", 0, 1);
}


//检查选择的验证码
function checkRandCodeAnsyn(point)
{
	file.trace("checkRandCodeAnsyn: param=" + point);
	var strParam = "randCode=" + point + "&rand=sjrand";
	
	var vurl = g_rooturl + "/passcodeNew/checkRandCodeAnsyn";
	var ret = curl.post(g_rooturl, vurl, strParam, 1, 0);
	//console.log(ret);
	file.trace("checkRandCodeAnsyn:",ret);
}

//login
function login(account , pwd, code)
{
	var vurl = g_rooturl + "/login/loginAysnSuggest";
	var strParam = "loginUserDTO.user_name=" + account + "&userDTO.password=" + pwd + "&appid=otn&randCode=" + code;
	var ret = curl.post(g_rooturl, vurl, strParam, 1, 0);
	//console.log(ret);
	file.trace("login:", ret);
}

//
function queryticket()
{
	///leftTicket/queryT?leftTicketDTO.train_date=2016-02-06&leftTicketDTO.from_station=SZQ&leftTicketDTO.to_station=WHN&purpose_codes=ADULT
	var vurl = g_rooturl + "/leftTicket/queryT";
	var strParam = "leftTicketDTO.train_date=2017-11-11&leftTicketDTO.from_station=SZQ&leftTicketDTO.to_station=WHN&purpose_codes=ADULT";
	var ret = curl.post(g_rooturl, vurl, strParam, 0, 0);
	file.trace("query ticket:", ret);
}

//1
getPassCodeNew("login_code.jpg");
//2
init();

queryticket();

//****************************************
//            damatu module
//****************************************
//init
var nInitCode = damatu.init("xhhzdl", "45772fd1a7f06da0f699c67bdb283c75");
file.trace("damatu : init ret=", nInitCode);

//login
var nLoginCode = damatu.login("wang70937", "xxxxxx");
file.trace("damatu : login ret=", nLoginCode);

if(nLoginCode == 0)
{
	
//querybalance
var nQueryBalanceCode = damatu.querybalance();
file.trace("damatu : querybalance ret=", nQueryBalanceCode);


//decodefile
var nDecodeFile = damatu.decodefile("login_code.jpg", 10, 287);
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

}

//var nId = damatu.d2file("45772fd1a7f06da0f699c67bdb283c75", "wang70937", "xxxxxx", "Login_code.jpg", 60, 287);


//****************************************
var vRandCode = string.replace(nResult, "|", ",");


//3
checkRandCodeAnsyn(vRandCode);
//4
login("wang", "xxxxxx", vRandCode);

