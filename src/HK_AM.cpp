# pragma once
# include"HK_AM.h"

using namespace std;
void HK_AMInfo(OCR* ocrModel, vector<cv::Rect> boxes, cv::Mat img, map<string, string>& result) {
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
		cout << ans << endl;

		//���ݿ�Ĵ�С��λ����Ϣƥ������
		if (result.find("��������") == result.end() &&
			box.x * 1.0 / w > 0.25) {
			result.insert(pair<string, string>("��������", ans));
			//cout << "������";
			//cout << ans << endl;
			continue;
		}

		//ƴ������
		regex reg("^[a-zA-Z]+[a-zA-Z0-9,��]+$");
		if (result.find("Ӣ������") == result.end() &&
			regex_match(ans, reg)) {
			result.insert(pair<string, string>("Ӣ������", ans));
			//std::cout << "ƴ����" << ans << endl;
			continue;
		}

		//��������&&�Ա�
		//�������ڿ��ܺ��Ա�ֿ�
		reg = ("^[0-9]{2,4}[.]?[0-9]{2}[.]?[0-9]{2}(��|Ů)$");//1993.03.03�С���|ŮҪ������
		regex reg1("^[0-9]{2,4}[.]?[0-9]{2}[.]?[0-9]{2}$");//1993.03.03
		if (result.find("��������") == result.end()){//��������һ��
			if (regex_match(ans, reg)){
				ans.erase(remove(ans.begin(), ans.end(), '.'), ans.end());//ȥ��.
				result.insert(pair<string, string>("��������", ans.substr(0, ans.length()-2)));//����ռ�����ֽ�
				result.insert(pair<string, string>("�Ա�", ans.substr(ans.length() - 2)));
				//std::cout << "�������ڣ�" << ans << endl;
				continue;
			}else if(regex_match(ans, reg1)){//�����Ƿֿ���
				ans.erase(remove(ans.begin(), ans.end(), '.'), ans.end());//ȥ��.
				result.insert(pair<string, string>("��������", ans));
				//std::cout << "�������ڣ�" << ans << endl;
				continue;
			}

		}
		
		reg = ("^��|Ů$");
		if (result.find("�Ա�") == result.end() &&
			regex_match(ans, reg)) {
			result.insert(pair<string, string>("�Ա�", ans));
			//std::cout << "�������ڣ�" << ans << endl;
			continue;
		}


		//��֤��&&��Ч��
		reg = ("^[0-9]{4}[.]?[0-9]{2}[.]?[0-9]{2}.[0-9]{4}[.]?[0-9]{2}[.]?[0-9]{2}$");
		if (result.find("֤����Ч��") == result.end() &&
			regex_match(ans, reg)) {
			ans.erase(remove(ans.begin(), ans.end(), '.'), ans.end());//ȥ��.
			ans.erase(remove(ans.begin(), ans.end(), '-'), ans.end());//ȥ��-
			result.insert(pair<string, string>("ǩ������", ans.substr(0,8)));
			result.insert(pair<string, string>("֤����Ч��", ans.substr(8)));
			//std::cout << "��֤�ڣ�" << ans.substr(10) << endl;
			//std::cout << "��Ч������" << ans.substr(0, 10) << endl;
			continue;
		}

	}
}