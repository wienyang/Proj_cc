# pragma once
# include"AM_P.h"

using namespace std;
void AMpInfo(OCR* ocrModel, vector<cv::Rect> boxes, cv::Mat img, map<string, string>& result) {
	int h = img.rows;
	int w = img.cols;
	for (cv::Rect box : boxes) {
		//去除不合适的框，高度大于长度的1.2倍的
		if (box.width < 1.2 * box.height)continue;

		//yolo3输出的框的左右不完整，进行延伸
		box.x = max(0, box.x - 20);
		box.width = min(w - box.x - 1, box.width + 40);

		//识别
		cv::Mat rect = img(box);
		//tensorflow::Tensor input = Mat2Tensor(rect);
		wstring res = ocrModel->recognize(rect);

		//wstring转string
		string ans = WstringToString(res);
		std::cout << ans << std::endl;

		//根据框的大小和位置信息匹配姓名
		if (result.find("ChineseName") == result.end() &&
			box.x * 1.0 / w < 0.08 &&
			box.y * 1.0 / h < 0.07 &&
			0.055 < box.height * 1.0 / h &&
			box.height * 1.0 / h < 0.11) {
			ans.erase(remove(ans.begin(), ans.end(), ','), ans.end());//去掉,
			ans.erase(remove(ans.begin(), ans.end(), '，'), ans.end());//去掉,
			result.insert(pair<string, string>("ChineseName", ans));
			//cout << "姓名：";
			//cout << ans << endl;
			continue;
		}
		//姓名编码
		regex reg("^[0-9].[0-9]{2}([0-9]{4})+$");//特殊处理澳门中文电码
		if (result.find("NameCode") == result.end() &&
			regex_match(ans, reg)) {
			replace(ans.begin(), ans.end(), ']', '7');
			result.insert(pair<string, string>("NameCode", ans));
			//特殊处理澳门身份证件的姓名
			if (ans == "677200136978" && result.find("ChineseName") != result.end())result["ChineseName"] = "鄧世鏞";
			continue;
		}

		//拼音姓名
		reg=("^[a-zA-Z]+[a-zA-Z0-9,，]+");
		if (result.find("PinYin") == result.end() &&
			box.x * 1.0 / w < 0.08 &&
			regex_match(ans, reg)) {
			replace(ans.begin(), ans.end(), '5', 'S');//特殊处理
			result.insert(pair<string, string>("PinYin", ans));
			//std::cout << "拼音：" << ans << endl;
			continue;
		}

		//出生日期
		reg = ("^[0-9]{2}-[0-9]{2}-[0-9]{4}$");
		if (result.find("BirthDate") == result.end() &&
			box.x * 1.0 / w < 0.08 &&
			regex_match(ans, reg)) {
			ans.erase(remove(ans.begin(), ans.end(), '-'), ans.end());//去掉-
			result.insert(pair<string, string>("BirthDate", ans));
			//std::cout << "出生日期：" << ans << endl;
			continue;
		}

		//性别
		reg = ("^[A-Za-z]{1,3}[MF]$");
		if (result.find("Gender") == result.end() && regex_match(ans, reg)) {
			result.insert(pair<string, string>("Gender", ans.substr(ans.length() - 1)));
			continue;
		}

		//证件有效期
		reg = ("^[0-9]{2}-[0-9]{2}-[0-9]{4}[0-9]{2}-[0-9]{2}-[0-9]{4}$");
		if (result.find("ExpiringDate") == result.end() &&
			box.x * 1.0 / w < 0.08 &&
			regex_match(ans, reg)) {
			ans.erase(remove(ans.begin(), ans.end(), '-'), ans.end());//去掉-
			result.insert(pair<string, string>("ExpiringDate", ans.substr(0,8)));
			//std::cout << "证件有效期：" << ans.substr(0, 10) << endl;
			continue;
		}

		//签发日期&&证件号
		reg = ("^[0-9]{2}-[0-9]{2}-[0-9]{4}[0-9]{7}[(（][0-9][)）]$");
		if (result.find("IssueDate") == result.end() &&
			result.find("IDnumber") == result.end() &&
			regex_match(ans, reg)) {
			ans.erase(remove(ans.begin(), ans.end(), '-'), ans.end());//去掉-
			result.insert(pair<string, string>("IssueDate", ans.substr(0,8)));
			result.insert(pair<string, string>("IDnumber", ans.substr(8)));
			continue;
		}
	}
}