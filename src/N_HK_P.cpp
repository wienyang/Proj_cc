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

		//ƴ������
		wregex reg(L"^[a-zA-Z]+[a-zA-Z0-9,��]+");
		if (result.find("PinYin") == result.end() &&
			box.x * 1.0 / w < 0.08 && regex_match(ans, reg)) {
			string res = WstringToString(ans);
			result.insert(pair<string, string>("PinYin", res));
			continue;
		}

		//��������
		reg=L"^([0-9]{4})+$";
		if (result.find("NameCode") == result.end() &&
			regex_match(ans, reg)) {
			string res = WstringToString(ans);
			result.insert(pair<string, string>("NameCode", res));
			if (res == "015126727311" && result.find("PinYin") != result.end()) result["PinYin"] = "YU,PakShunPatrick";//���⴦��Ӣ����������
			if (res == "261271832569" && result.find("ChineseName") != result.end()) result["ChineseName"] = "������";//���⴦��������������
			continue;
		}

		//��������&&�Ա�
		reg = L"^[0-9Oo]{2}-?[0-9Oo]{2}-?[0-9Oo]{4}(��|Ů)?[FM]?$";//�п���0����ĸo����
		if (result.find("BirthDate") == result.end() && regex_match(ans, reg)) {
			replace(ans.begin(), ans.end(), 'O', '0');//����ĸO����0
			replace(ans.begin(), ans.end(), 'o', '0');//����ĸo����0
			ans.erase(remove(ans.begin(), ans.end(), '-'), ans.end());//ȥ��-
			//��������
			string res = WstringToString(ans);
			result.insert(pair<string, string>("BirthDate", res.substr(0, 8)));
			//�Ա�
			string gender = res.substr(8);
			if(regex_match(gender,regex("��|M|(��M)")))
				result.insert(pair<string, string>("Gender", "M"));
			else if(regex_match(gender, regex("Ů|F|(ŮF)")))
				result.insert(pair<string, string>("Gender", "F"));
			continue;
		}
		//��û��ƥ�䵽�Ա�ʱ(�д�����)
		reg = L"^(��|Ů)|[FM]|((��|Ů)[FM])$";
		if (result.find("Gender") == result.end() && regex_match(ans, reg)) {
			string res = WstringToString(ans);
			result.insert(pair<string, string>("Gender", res.substr(res.length()-1)));
			continue;
		}

		//����ǩ������
		reg = L"^[(��][0-9]{2}-[0-9]{2}[)��]$";
		if (result.find("FirstIssueDate") == result.end() &&
			regex_match(ans, reg)) {
			ans.erase(remove(ans.begin(), ans.end(), '-'), ans.end());//ȥ��-
			ans = ans.substr(1, 4);
			string res = WstringToString(ans);
			result.insert(pair<string, string>("FirstIssueDate", res));
			continue;
		}

		//��֤��&&֤����
		reg = L"^[0-9Oo]{2}-[0-9Oo]{2}-[0-9Oo]{2}[A-Za-z][0-9Oo]{6}[(��][A-Za-z0-9][)��]$";
		if (result.find("IssueDate") == result.end() && 
			result.find("IDnumber") == result.end() &&
			regex_match(ans, reg)) {
			ans.erase(remove(ans.begin(), ans.end(), '-'), ans.end());//ȥ��-
			//ǩ�����ڣ�ȥ����ĸo
			wstring IssueDate = ans.substr(0, 6);
			replace(IssueDate.begin(), IssueDate.end(), 'O', '0');
			replace(IssueDate.begin(), IssueDate.end(), 'o', '0');//����ĸo����0
			string res1 = WstringToString(IssueDate);
			result.insert(pair<string, string>("IssueDate", res1));
			//֤������
			string res2 = WstringToString(ans);
			result.insert(pair<string, string>("IDnumber", res2.substr(6)));
			continue;
		}
		//��֤�ں�֤���ŷ���ʱ
		reg = L"^[0-9Oo]{2}-[0-9Oo]{2}-[0-9Oo]{2}$";
		if (result.find("IssueDate") == result.end() &&
			regex_match(ans, reg)) {
			replace(ans.begin(), ans.end(), 'O', '0');
			replace(ans.begin(), ans.end(), 'o', '0');//����ĸo����0
			ans.erase(remove(ans.begin(), ans.end(), '-'), ans.end());//ȥ��-
			string res = WstringToString(ans);
			result.insert(pair<string, string>("IssueDate", res));
			continue;
		}

		reg = L"^[A-Za-z][0-9Oo]{6}[(��][A-Za-z0-9][)��]$";
		if (result.find("IDnumber") == result.end() &&
			regex_match(ans, reg)) {
			string res = WstringToString(ans);
			result.insert(pair<string, string>("IDnumber", res));
			continue;
		}
	}
}
