# pragma once
# include"AM_P.h"

using namespace std;
void AMpInfo(OCR* ocrModel, vector<cv::Rect> boxes, cv::Mat img, map<string, string>& result) {
	int h = img.rows;
	int w = img.cols;
	for (cv::Rect box : boxes) {
		//ȥ�������ʵĿ򣬸߶ȴ��ڳ��ȵ�1.2����
		if (box.width < 1.2 * box.height)continue;

		//yolo3����Ŀ�����Ҳ���������������
		box.x = max(0, box.x - 20);
		box.width = min(w - box.x - 1, box.width + 40);

		//ʶ��
		cv::Mat rect = img(box);
		//tensorflow::Tensor input = Mat2Tensor(rect);
		wstring ans = ocrModel->recognize(rect);

		//wstringתstring
		//string ans = WstringToString(res);
		//std::cout << ans << std::endl;

		//���ݿ�Ĵ�С��λ����Ϣƥ�����������Ķ���δ����
		if (result.find("ChineseName") == result.end() &&
			box.x * 1.0 / w < 0.08 &&
			box.y * 1.0 / h < 0.07 &&
			0.055 < box.height * 1.0 / h &&
			box.height * 1.0 / h < 0.11) {
			ans.erase(remove(ans.begin(), ans.end(), ','), ans.end());//ȥ��,
			//ans.erase(remove(ans.begin(), ans.end(), '��'), ans.end());//����ȥ�������ַ�
			string res = WstringToString(ans);
			result.insert(pair<string, string>("ChineseName", res));
			continue;
		}
		//��������
		wregex reg(L"^[0-9].[0-9]{2}([0-9]{4})+$");//���⴦��������ĵ���
		if (result.find("NameCode") == result.end() &&
			regex_match(ans, reg)) {
			replace(ans.begin(), ans.end(), ']', '7');
			string res = WstringToString(ans);
			result.insert(pair<string, string>("NameCode", res));
			//���⴦��������֤��������
			if (res == "677200136978" && result.find("ChineseName") != result.end())result["ChineseName"] = "�����O";
			continue;
		}

		//ƴ������
		reg=L"^[a-zA-Z]+[a-zA-Z0-9,��]+";
		if (result.find("PinYin") == result.end() &&
			box.x * 1.0 / w < 0.08 &&
			regex_match(ans, reg)) {
			replace(ans.begin(), ans.end(), '5', 'S');//���⴦��
			string res = WstringToString(ans);
			result.insert(pair<string, string>("PinYin", res));
			//std::cout << "ƴ����" << ans << endl;
			continue;
		}

		//��������
		reg = L"^[0-9]{2}-[0-9]{2}-[0-9]{4}$";
		if (result.find("BirthDate") == result.end() &&
			box.x * 1.0 / w < 0.08 &&
			regex_match(ans, reg)) {
			ans.erase(remove(ans.begin(), ans.end(), '-'), ans.end());//ȥ��-
			string res = WstringToString(ans);
			result.insert(pair<string, string>("BirthDate", res));
			continue;
		}

		//�Ա�
		reg = L"^[A-Za-z]{1,3}[MFmf]$";
		if (result.find("Gender") == result.end() && regex_match(ans, reg)) {
			string res = WstringToString(ans);
			result.insert(pair<string, string>("Gender", res.substr(res.length() - 1)));
			continue;
		}

		//�״�ǩ֤����&&֤����Ч��
		reg = L"^[0-9]{2}-[0-9]{2}-[0-9]{4}[0-9]{2}-[0-9]{2}-[0-9]{4}$";
		if (result.find("ExpiringDate") == result.end() &&
			box.x * 1.0 / w < 0.08 &&
			regex_match(ans, reg)) {
			ans.erase(remove(ans.begin(), ans.end(), '-'), ans.end());//ȥ��-
			string res = WstringToString(ans);
			result.insert(pair<string, string>("ExpiringDate", res.substr(0,8)));
			result.insert(pair<string, string>("FirstIssueDate", res.substr(8)));//�״�ǩ֤����
			continue;
		}

		//ǩ������&&֤���ţ���������δ����
		reg = L"^[0-9]{2}-[0-9]{2}-[0-9]{4}[0-9]{7}[(��][0-9][)��]$";
		if (result.find("IssueDate") == result.end() &&
			result.find("IDnumber") == result.end() &&
			regex_match(ans, reg)) {
			ans.erase(remove(ans.begin(), ans.end(), '-'), ans.end());//ȥ��-
			string res = WstringToString(ans);
			result.insert(pair<string, string>("IssueDate", res.substr(0,8)));
			result.insert(pair<string, string>("IDnumber", res.substr(8)));
			continue;
		}
	}
}