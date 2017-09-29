// Demo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "JsModule.h"
#include <string>
#include <stdio.h>

std::string ReadJsFileContent(std::string strFile)
{
	FILE* file = NULL;
	errno_t err = fopen_s(&file, strFile.c_str(), "rb");
	if (err)
	{
		return "";
	}

	fseek(file, 0, SEEK_END);
	int nLen = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* p = new char[nLen + 1];
	memset(p, 0, nLen + 1);
	fread_s(p, nLen, 1, nLen, file);
	fclose(file);

	std::string strContent(p, nLen);
	delete[] p;
	p = NULL;

	return strContent;
}

int _tmain(int argc, _TCHAR* argv[])
{

	V8_init();
	CJsModule js;
	
//	std::string str = ReadJsFileContent("console.js");

//	js.V8_execute("var console = require('console');console.log('yo', 'baby', 4.2, null); ");

//	js.V8_execute(str.c_str());

	js.V8_executeFile("console.js");

//	js.V8_executeFile("file.js");

	V8_uninit();

	getchar();
	return 0;
}

