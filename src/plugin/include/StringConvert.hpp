#pragma once
#include <v8pp/config.hpp>

#include <string>
using namespace std;



void Convert(const char* strIn, char* strOut, int sourceCodepage, int targetCodepage)
{
	int len = strlen(strIn);
	int unicodeLen = MultiByteToWideChar(sourceCodepage, 0, strIn, -1, NULL, 0);
	wchar_t* pUnicode;
	pUnicode = new wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1)*sizeof(wchar_t));
	MultiByteToWideChar(sourceCodepage, 0, strIn, -1, (LPWSTR)pUnicode, unicodeLen);
	BYTE * pTargetData = NULL;
	int targetLen = WideCharToMultiByte(targetCodepage, 0, (LPWSTR)pUnicode, -1, (char *)pTargetData, 0, NULL, NULL);
	pTargetData = new BYTE[targetLen + 1];
	memset(pTargetData, 0, targetLen + 1);
	WideCharToMultiByte(targetCodepage, 0, (LPWSTR)pUnicode, -1, (char *)pTargetData, targetLen, NULL, NULL);
	strcpy(strOut, (char*)pTargetData);
	delete pUnicode;
	delete pTargetData;
}

std::string MbToUtf8(string strIn)
{
	char chOut2[10240] = { 0 };
	Convert(strIn.c_str(), chOut2, CP_ACP, CP_UTF8);
	string strOut = chOut2;
	return strOut;
}

bool Utf8ToMb(char* strStcText, int nLen, std::string &strDstText)
{
	// convert an UTF8 string to widechar
	int nWLen = MultiByteToWideChar(CP_UTF8, 0, strStcText, nLen, NULL, 0);
	WCHAR *strTemp = (WCHAR*)malloc(sizeof(WCHAR)* nWLen);
	if (NULL == strTemp)
	{
		return false;
	}
	int nRtn = MultiByteToWideChar(CP_UTF8, 0, strStcText, nLen, strTemp, nWLen);
	if (nRtn == 0)
	{
		free(strTemp);
		return false;
	}
	// convert an widechar string to Multibyte  
	int MBLen = WideCharToMultiByte(CP_ACP, 0, strTemp, nWLen, NULL, 0, NULL, NULL);
	if (0 == MBLen)
	{
		free(strTemp);
		return false;
	}
	char *str = (char*)malloc(sizeof(char)*MBLen + 1);
	if (NULL == str)
	{
		return false;
	}
	nRtn = WideCharToMultiByte(CP_ACP, 0, strTemp, nWLen, str, MBLen, NULL, NULL);
	if (0 == nRtn)
	{
		free(strTemp);
		free(str);
		return false;
	}
	//最后释放所有的变量
	str[nRtn] = '\0';
	strDstText = str;
	free(strTemp);
	free(str);
	return true;
}

///////////////////////////////////
std::string AnsiToUtf8(v8::FunctionCallbackInfo<v8::Value> const& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());

	v8::String::Utf8Value str(args[0]);

	std::string sIn = *str;
	char chOut[1024] = { 0 };
	Convert(sIn.c_str(), chOut, CP_UTF8, CP_ACP);

	char chOut2[1024] = { 0 };
	Convert(chOut, chOut2, CP_ACP, CP_UTF8);

	std::string sOut = chOut2;
	//Utf8ToMb((char*)sIn.c_str(), sIn.length(), sOut);
	return sOut;
}

std::string Utf8ToAnsi(v8::FunctionCallbackInfo<v8::Value> const& args)
{
	v8::HandleScope handle_scope(args.GetIsolate());

	v8::String::Utf8Value str(args[0]);
	std::string s = *str;
	std::string sOut;
	Utf8ToMb((char*)s.c_str(), s.length(), sOut);

	return sOut;
}

wstring utf8ToUnicode(char* szSrc)
{
	int  len = 0;
	string str = szSrc;
	len = str.length();
	int  unicodeLen = ::MultiByteToWideChar(CP_UTF8,
		0,
		str.c_str(),
		-1,
		NULL,
		0);
	wchar_t *  pUnicode;
	pUnicode = new  wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1)*sizeof(wchar_t));
	::MultiByteToWideChar(CP_UTF8,
		0,
		str.c_str(),
		-1,
		(LPWSTR)pUnicode,
		unicodeLen);
	wstring  rt;
	rt = (wchar_t*)pUnicode;
	delete  pUnicode;

	return  rt;
}

///////////////////////////////////////
/// v8的utf8Value 转换为MultiBytes
///////////////////////////////////////
string utf8Value2Mb(v8::Local<v8::Value>& v)
{
	v8::String::Utf8Value str(v);
	string s = *str;
	string sOut;
	Utf8ToMb((char*)s.c_str(), s.length(), sOut);

	return sOut;
}
///////////////////////////////////////
/// v8的utf8Value 转换为uint32_t
///////////////////////////////////////
uint32_t utf8Value2uint32(v8::Local<v8::Value>& v)
{
	uint32_t uValue = v->Uint32Value();
	return uValue;
}

///////////////////////////////////////
/// v8的utf8Value 转换为bool
///////////////////////////////////////
bool utf8Value2bool(v8::Local<v8::Value>& v)
{
	bool uValue = v->BooleanValue();
	return uValue;
}

