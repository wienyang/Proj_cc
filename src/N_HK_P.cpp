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
		wstring res = ocrModel->recognize(rect);
		string ans = WstringToString(res);
		//gbk to utf8
		//ans = GBKToUTF8(ans);


		if (result.find("ChineseName") == result.end() &&
			box.x * 1.0 / w < 0.08 &&
			0.055 < box.height * 1.0 / h &&
			box.height * 1.0 / h < 0.11) {
			result.insert(pair<string, string>("ChineseName", ans));
			continue;
		}

		//拼音姓名
		regex reg("^[a-zA-Z]+[a-zA-Z0-9,，]+");
		if (result.find("PinYin") == result.end() &&
			box.x * 1.0 / w < 0.08 && regex_match(ans, reg)) {

			result.insert(pair<string, string>("PinYin", ans));
			continue;
		}

		//出生日期&&性别
		reg = ("^[0-9Oo]{2}-?[0-9Oo]{2}-?[0-9Oo]{4}(男|女)?[FM]?$");//有可能0与字母o混淆
		if (result.find("BirthDate") == result.end() && regex_match(ans, reg)) {
			replace(ans.begin(), ans.end(), 'O', '0');//把字母O换成0
			replace(ans.begin(), ans.end(), 'o', '0');//把字母o换成0
			ans.erase(remove(ans.begin(), ans.end(), '-'), ans.end());//去掉-
			//出生日期
			result.insert(pair<string, string>("BirthDate", ans.substr(0, 8)));
			//性别
			string gender = ans.substr(8);
			if(regex_match(gender,regex("男|M|(男M)")))
				result.insert(pair<string, string>("Gender", "M"));
			else if(regex_match(gender, regex("女|F|(女F)")))
				result.insert(pair<string, string>("Gender", "F"));
			continue;
		}
		//当没有匹配到性别时(有待完善)
		reg = ("^(男|女)|[FM]|((男|女)[FM])$");
		if (result.find("Gender") == result.end() && regex_match(ans, reg)) {
			result.insert(pair<string, string>("Gender", ans.substr(ans.length()-1)));
			continue;
		}

		//发证期&&证件号
		reg = ("^[0-9Oo]{2}-[0-9Oo]{2}-[0-9Oo]{2}[A-Za-z][0-9Oo]{6}[(（][A-Za-z0-9][)）]$");
		if (result.find("IssueDate") == result.end() && 
			result.find("IDnumber") == result.end() &&
			regex_match(ans, reg)) {
			ans.erase(remove(ans.begin(), ans.end(), '-'), ans.end());//去掉-
			//签发日期，去除字母o
			string IssueDate = ans.substr(0, 6);
			replace(IssueDate.begin(), IssueDate.end(), 'O', '0');
			replace(IssueDate.begin(), IssueDate.end(), 'o', '0');//把字母o换成0
			result.insert(pair<string, string>("IssueDate", IssueDate));
			//证件号码
			result.insert(pair<string, string>("IDnumber", ans.substr(6)));
			continue;
		}
		//发证期和证件号分离时
		reg = ("^[0-9Oo]{2}-[0-9Oo]{2}-[0-9Oo]{2}$");
		if (result.find("IssueDate") == result.end() &&
			regex_match(ans, reg)) {
			replace(ans.begin(), ans.end(), 'O', '0');
			replace(ans.begin(), ans.end(), 'o', '0');//把字母o换成0
			ans.erase(remove(ans.begin(), ans.end(), '-'), ans.end());//去掉-
			result.insert(pair<string, string>("IssueDate", ans));
			continue;
		}

		reg = ("^[A-Za-z][0-9Oo]{6}[(（][A-Za-z0-9][)）]$");
		if (result.find("IDnumber") == result.end() &&
			regex_match(ans, reg)) {
			result.insert(pair<string, string>("IDnumber", ans));
			continue;
		}
	}
}
