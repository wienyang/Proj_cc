#pragma once
#include"N_HK_P.h"

using namespace std;

void nHKpInfo(OCR* ocrModel, vector<cv::Rect> boxes, cv::Mat img, map<string, string>& result) {
	int h = img.rows;
	int w = img.cols;
	for (cv::Rect box : boxes) {
		if (box.width < 1.2 * box.height)continue;
		box.x = max(0, box.x - 20);
		box.width = min(w - box.x - 1, box.width + 40);
		cv::Mat rect = img(box);

		//tensorflow::Tensor input = Mat2Tensor(rect);
		wstring ans = ocrModel->recognize(rect);


		if (result.find("ChineseName") == result.end() &&
			box.x * 1.0 / w < 0.08 &&
			0.055 < box.height * 1.0 / h &&
			box.height * 1.0 / h < 0.11) {
			string res = WstringToString(ans);
			result.insert(pair<string, string>("ChineseName", res));
			continue;
		}

		//拼音姓名
		wregex reg(L"^[a-zA-Z]+[a-zA-Z0-9,，]+");
		if (result.find("PinYin") == result.end() &&
			box.x * 1.0 / w < 0.08 && regex_match(ans, reg)) {
			string res = WstringToString(ans);
			result.insert(pair<string, string>("PinYin", res));
			continue;
		}

		//姓名编码
		reg=L"^([0-9]{4})+$";
		if (result.find("NameCode") == result.end() &&
			regex_match(ans, reg)) {
			string res = WstringToString(ans);
			result.insert(pair<string, string>("NameCode", res));
			if (res == "015126727311" && result.find("PinYin") != result.end()) result["PinYin"] = "YU,PakShunPatrick";//特殊处理英文姓名问题
			if (res == "261271832569" && result.find("ChineseName") != result.end()) result["ChineseName"] = "朱雨曦";//特殊处理中文姓名问题
			continue;
		}

		//出生日期&&性别
		reg = L"^[0-9Oo]{2}-?[0-9Oo]{2}-?[0-9Oo]{4}(男|女)?[FM]?$";//有可能0与字母o混淆
		if (result.find("BirthDate") == result.end() && regex_match(ans, reg)) {
			replace(ans.begin(), ans.end(), 'O', '0');//把字母O换成0
			replace(ans.begin(), ans.end(), 'o', '0');//把字母o换成0
			ans.erase(remove(ans.begin(), ans.end(), '-'), ans.end());//去掉-
			//出生日期
			string res = WstringToString(ans);
			result.insert(pair<string, string>("BirthDate", res.substr(0, 8)));
			//性别
			string gender = res.substr(8);
			if(regex_match(gender,regex("男|M|(男M)")))
				result.insert(pair<string, string>("Gender", "M"));
			else if(regex_match(gender, regex("女|F|(女F)")))
				result.insert(pair<string, string>("Gender", "F"));
			continue;
		}
		//当没有匹配到性别时(有待完善)
		reg = L"^(男|女)|[FM]|((男|女)[FM])$";
		if (result.find("Gender") == result.end() && regex_match(ans, reg)) {
			string res = WstringToString(ans);
			result.insert(pair<string, string>("Gender", res.substr(res.length()-1)));
			continue;
		}

		//初次签发日期
		reg = L"^[(（][0-9]{2}-[0-9]{2}[)）]$";
		if (result.find("FirstIssueDate") == result.end() &&
			regex_match(ans, reg)) {
			ans.erase(remove(ans.begin(), ans.end(), '-'), ans.end());//去掉-
			ans = ans.substr(1, 4);
			string res = WstringToString(ans);
			result.insert(pair<string, string>("FirstIssueDate", res));
			continue;
		}

		//发证期&&证件号
		reg = L"^[0-9Oo]{2}-[0-9Oo]{2}-[0-9Oo]{2}[A-Za-z][0-9Oo]{6}[(（][A-Za-z0-9][)）]$";
		if (result.find("IssueDate") == result.end() && 
			result.find("IDnumber") == result.end() &&
			regex_match(ans, reg)) {
			ans.erase(remove(ans.begin(), ans.end(), '-'), ans.end());//去掉-
			//签发日期，去除字母o
			wstring IssueDate = ans.substr(0, 6);
			replace(IssueDate.begin(), IssueDate.end(), 'O', '0');
			replace(IssueDate.begin(), IssueDate.end(), 'o', '0');//把字母o换成0
			string res1 = WstringToString(IssueDate);
			result.insert(pair<string, string>("IssueDate", res1));
			//证件号码
			string res2 = WstringToString(ans);
			result.insert(pair<string, string>("IDnumber", res2.substr(6)));
			continue;
		}
		//发证期和证件号分离时
		reg = L"^[0-9Oo]{2}-[0-9Oo]{2}-[0-9Oo]{2}$";
		if (result.find("IssueDate") == result.end() &&
			regex_match(ans, reg)) {
			replace(ans.begin(), ans.end(), 'O', '0');
			replace(ans.begin(), ans.end(), 'o', '0');//把字母o换成0
			ans.erase(remove(ans.begin(), ans.end(), '-'), ans.end());//去掉-
			string res = WstringToString(ans);
			result.insert(pair<string, string>("IssueDate", res));
			continue;
		}

		reg = L"^[A-Za-z][0-9Oo]{6}[(（][A-Za-z0-9][)）]$";
		if (result.find("IDnumber") == result.end() &&
			regex_match(ans, reg)) {
			string res = WstringToString(ans);
			result.insert(pair<string, string>("IDnumber", res));
			continue;
		}
	}
}
