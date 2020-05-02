#include "tools.h"

std::string WstringToString(const std::wstring str)
{// wstringתstring
	unsigned len = str.size() * 4;
	setlocale(LC_CTYPE, "");
	char* p = new char[len];
	wcstombs(p, str.c_str(), len);
	std::string str1(p);
	delete[] p;
	return str1;
}


// 工具：GBK转UTF8编码（本cpp文件默认编码为GBK，约定返回给客户的字符串编码为UTF8）
std::string GBKToUTF8(const std::string& strGBK)
{
	std::string strOutUTF8 = "";
	WCHAR* str1;
	int n = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);
	str1 = new WCHAR[n];
	MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, str1, n);
	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
	char* str2 = new char[n];
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
	strOutUTF8 = str2;
	delete[]str1;
	str1 = NULL;
	delete[]str2;
	str2 = NULL;
	return strOutUTF8;
}
////写入结果
//int writeResult(int type, std::map<std::string, std::string> result) {
//	std::map <std::string, std::string>::iterator res_iter;
//	Json::Value root;
//	Json::Value info;
//
//	for (res_iter = result.begin(); res_iter != result.end(); res_iter++) {
//		//cout << res_iter->first << ":" << res_iter->second << endl;
//		info[res_iter->first] = Json::Value(res_iter->second);
//	}
//	if (type == 0)root["澳门身份证"] = Json::Value(info);
//	else if (type == 1)root["香港身份证"] = Json::Value(info);
//	//缩进输出  
//	Json::StyledWriter sw;
//	std::cout << sw.write(root) << std::endl;
//	//输出到文件  
//	std::ofstream os;
//	remove("recogResult.json");
//	os.open("recogResult.json", std::ios::out | std::ios::app);
//	if (!os.is_open())
//		std::cout << "error：can not find or create the file which named \" result.json\"." << std::endl;
//	//os.clear();
//	os << sw.write(root);
//	os.close();
//	return 0;
//}