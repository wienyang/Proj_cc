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
		wstring res = ocrModel->recognize(rect);

		//wstringתstring
		string ans = WstringToString(res);
		std::cout << ans << std::endl;

		//���ݿ�Ĵ�С��λ����Ϣƥ������
		if (result.find("ChineseName") == result.end() &&
			box.x * 1.0 / w < 0.08 &&
			box.y * 1.0 / h < 0.07 &&
			0.055 < box.height * 1.0 / h &&
			box.height * 1.0 / h < 0.11) {
			ans.erase(remove(ans.begin(), ans.end(), ','), ans.end());//ȥ��,
			ans.erase(remove(ans.begin(), ans.end(), '��'), ans.end());//ȥ��,
			result.insert(pair<string, string>("ChineseName", ans));
			//cout << "������";
			//cout << ans << endl;
			continue;
		}
		//��������
		regex reg("^[0-9].[0-9]{2}([0-9]{4})+$");//���⴦��������ĵ���
		if (result.find("NameCode") == result.end() &&
			regex_match(ans, reg)) {
			replace(ans.begin(), ans.end(), ']', '7');
			result.insert(pair<string, string>("NameCode", ans));
			//���⴦��������֤��������
			if (ans == "677200136978" && result.find("ChineseName") != result.end())result["ChineseName"] = "�����O";
			continue;
		}

		//ƴ������
		reg=("^[a-zA-Z]+[a-zA-Z0-9,��]+");
		if (result.find("PinYin") == result.end() &&
			box.x * 1.0 / w < 0.08 &&
			regex_match(ans, reg)) {
			replace(ans.begin(), ans.end(), '5', 'S');//���⴦��
			result.insert(pair<string, string>("PinYin", ans));
			//std::cout << "ƴ����" << ans << endl;
			continue;
		}

		//��������
		reg = ("^[0-9]{2}-[0-9]{2}-[0-9]{4}$");
		if (result.find("BirthDate") == result.end() &&
			box.x * 1.0 / w < 0.08 &&
			regex_match(ans, reg)) {
			ans.erase(remove(ans.begin(), ans.end(), '-'), ans.end());//ȥ��-
			result.insert(pair<string, string>("BirthDate", ans));
			//std::cout << "�������ڣ�" << ans << endl;
			continue;
		}

		//�Ա�
		reg = ("^[A-Za-z]{1,3}[MF]$");
		if (result.find("Gender") == result.end() && regex_match(ans, reg)) {
			result.insert(pair<string, string>("Gender", ans.substr(ans.length() - 1)));
			continue;
		}

		//֤����Ч��
		reg = ("^[0-9]{2}-[0-9]{2}-[0-9]{4}[0-9]{2}-[0-9]{2}-[0-9]{4}$");
		if (result.find("ExpiringDate") == result.end() &&
			box.x * 1.0 / w < 0.08 &&
			regex_match(ans, reg)) {
			ans.erase(remove(ans.begin(), ans.end(), '-'), ans.end());//ȥ��-
			result.insert(pair<string, string>("ExpiringDate", ans.substr(0,8)));
			//std::cout << "֤����Ч�ڣ�" << ans.substr(0, 10) << endl;
			continue;
		}

		//ǩ������&&֤����
		reg = ("^[0-9]{2}-[0-9]{2}-[0-9]{4}[0-9]{7}[(��][0-9][)��]$");
		if (result.find("IssueDate") == result.end() &&
			result.find("IDnumber") == result.end() &&
			regex_match(ans, reg)) {
			ans.erase(remove(ans.begin(), ans.end(), '-'), ans.end());//ȥ��-
			result.insert(pair<string, string>("IssueDate", ans.substr(0,8)));
			result.insert(pair<string, string>("IDnumber", ans.substr(8)));
			continue;
		}
	}
}