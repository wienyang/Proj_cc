#pragma once
#include<Windows.h>
#include<string>
#include<iostream>
//#include<json/json.h>
//#include<fstream>
//#include <filesystem>

std::string WstringToString(const std::wstring str);
std::string GBKToUTF8(const std::string& strGBK);
//int writeResult(int type, std::map<std::string, std::string> result);