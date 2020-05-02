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

		//ƴ������
		regex reg("^[a-zA-Z]+[a-zA-Z0-9,��]+");
		if (result.find("PinYin") == result.end() &&
			box.x * 1.0 / w < 0.08 && regex_match(ans, reg)) {

			result.insert(pair<string, string>("PinYin", ans));
			continue;
		}

		//��������&&�Ա�
		reg = ("^[0-9Oo]{2}-?[0-9Oo]{2}-?[0-9Oo]{4}(��|Ů)?[FM]?$");//�п���0����ĸo����
		if (result.find("BirthDate") == result.end() && regex_match(ans, reg)) {
			replace(ans.begin(), ans.end(), 'O', '0');//����ĸO����0
			replace(ans.begin(), ans.end(), 'o', '0');//����ĸo����0
			ans.erase(remove(ans.begin(), ans.end(), '-'), ans.end());//ȥ��-
			//��������
			result.insert(pair<string, string>("BirthDate", ans.substr(0, 8)));
			//�Ա�
			string gender = ans.substr(8);
			if(regex_match(gender,regex("��|M|(��M)")))
				result.insert(pair<string, string>("Gender", "M"));
			else if(regex_match(gender, regex("Ů|F|(ŮF)")))
				result.insert(pair<string, string>("Gender", "F"));
			continue;
		}
		//��û��ƥ�䵽�Ա�ʱ(�д�����)
		reg = ("^(��|Ů)|[FM]|((��|Ů)[FM])$");
		if (result.find("Gender") == result.end() && regex_match(ans, reg)) {
			result.insert(pair<string, string>("Gender", ans.substr(ans.length()-1)));
			continue;
		}

		//��֤��&&֤����
		reg = ("^[0-9Oo]{2}-[0-9Oo]{2}-[0-9Oo]{2}[A-Za-z][0-9Oo]{6}[(��][A-Za-z0-9][)��]$");
		if (result.find("IssueDate") == result.end() && 
			result.find("IDnumber") == result.end() &&
			regex_match(ans, reg)) {
			ans.erase(remove(ans.begin(), ans.end(), '-'), ans.end());//ȥ��-
			//ǩ�����ڣ�ȥ����ĸo
			string IssueDate = ans.substr(0, 6);
			replace(IssueDate.begin(), IssueDate.end(), 'O', '0');
			replace(IssueDate.begin(), IssueDate.end(), 'o', '0');//����ĸo����0
			result.insert(pair<string, string>("IssueDate", IssueDate));
			//֤������
			result.insert(pair<string, string>("IDnumber", ans.substr(6)));
			continue;
		}
		//��֤�ں�֤���ŷ���ʱ
		reg = ("^[0-9Oo]{2}-[0-9Oo]{2}-[0-9Oo]{2}$");
		if (result.find("IssueDate") == result.end() &&
			regex_match(ans, reg)) {
			replace(ans.begin(), ans.end(), 'O', '0');
			replace(ans.begin(), ans.end(), 'o', '0');//����ĸo����0
			ans.erase(remove(ans.begin(), ans.end(), '-'), ans.end());//ȥ��-
			result.insert(pair<string, string>("IssueDate", ans));
			continue;
		}

		reg = ("^[A-Za-z][0-9Oo]{6}[(��][A-Za-z0-9][)��]$");
		if (result.find("IDnumber") == result.end() &&
			regex_match(ans, reg)) {
			result.insert(pair<string, string>("IDnumber", ans));
			continue;
		}
	}
}
