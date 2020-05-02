# pragma once
# include"HK_AM.h"

using namespace std;
void HK_AMInfo(OCR* ocrModel, vector<cv::Rect> boxes, cv::Mat img, map<string, string>& result) {
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
		cout << ans << endl;

		//根据框的大小和位置信息匹配姓名
		if (result.find("中文姓名") == result.end() &&
			box.x * 1.0 / w > 0.25) {
			result.insert(pair<string, string>("中文姓名", ans));
			//cout << "姓名：";
			//cout << ans << endl;
			continue;
		}

		//拼音姓名
		regex reg("^[a-zA-Z]+[a-zA-Z0-9,，]+$");
		if (result.find("英文姓名") == result.end() &&
			regex_match(ans, reg)) {
			result.insert(pair<string, string>("英文姓名", ans));
			//std::cout << "拼音：" << ans << endl;
			continue;
		}

		//出生日期&&性别
		//出生日期可能和性别分开
		reg = ("^[0-9]{2,4}[.]?[0-9]{2}[.]?[0-9]{2}(男|女)$");//1993.03.03男。男|女要加括号
		regex reg1("^[0-9]{2,4}[.]?[0-9]{2}[.]?[0-9]{2}$");//1993.03.03
		if (result.find("出生日期") == result.end()){//假如连在一起
			if (regex_match(ans, reg)){
				ans.erase(remove(ans.begin(), ans.end(), '.'), ans.end());//去掉.
				result.insert(pair<string, string>("出生日期", ans.substr(0, ans.length()-2)));//汉字占两个字节
				result.insert(pair<string, string>("性别", ans.substr(ans.length() - 2)));
				//std::cout << "出生日期：" << ans << endl;
				continue;
			}else if(regex_match(ans, reg1)){//假如是分开的
				ans.erase(remove(ans.begin(), ans.end(), '.'), ans.end());//去掉.
				result.insert(pair<string, string>("出生日期", ans));
				//std::cout << "出生日期：" << ans << endl;
				continue;
			}

		}
		
		reg = ("^男|女$");
		if (result.find("性别") == result.end() &&
			regex_match(ans, reg)) {
			result.insert(pair<string, string>("性别", ans));
			//std::cout << "出生日期：" << ans << endl;
			continue;
		}


		//发证期&&有效期
		reg = ("^[0-9]{4}[.]?[0-9]{2}[.]?[0-9]{2}.[0-9]{4}[.]?[0-9]{2}[.]?[0-9]{2}$");
		if (result.find("证件有效期") == result.end() &&
			regex_match(ans, reg)) {
			ans.erase(remove(ans.begin(), ans.end(), '.'), ans.end());//去掉.
			ans.erase(remove(ans.begin(), ans.end(), '-'), ans.end());//去掉-
			result.insert(pair<string, string>("签发日期", ans.substr(0,8)));
			result.insert(pair<string, string>("证件有效期", ans.substr(8)));
			//std::cout << "发证期：" << ans.substr(10) << endl;
			//std::cout << "有效期至：" << ans.substr(0, 10) << endl;
			continue;
		}

	}
}