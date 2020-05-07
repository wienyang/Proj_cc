#pragma once
#include<ctime>
#include<windows.h>

#include <direct.h>
#include <filesystem>


#include"cardInfoRecog.h"
#include"mrzRecog.h"



#pragma comment(lib,"tensorflow.lib")
#pragma warning(suppress : 4996)

//int findType(cv::Mat imgInput) {
//	if (imgInput.cols < 350 || imgInput.rows < 200)return -1;
//	//cv::Mat img = imgInput(cv::Rect(265, 120, 270, 270));
//	//ʹ�����λ����Ϣ
//	cv::Mat img = imgInput(cv::Rect((int)(0.266*imgInput.cols),(int)(0.188*imgInput.rows),(int)(0.27*imgInput.cols), (int)(0.42*imgInput.cols)));
//
//	int type;
//	cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);//�任Ϊ�Ҷ�
//	
//	int w = img.cols;
//	int h = img.rows;
//	int sum = 0;
//	for (int i = 0; i <= w-1; i++) {
//		uchar* data = img.ptr<uchar>(i);
//		for (int j = 0; j <= h-1; j++) {
//			//cout << data[j];
//			sum += (data[j] > 240 ? 1 : 0);
//		}
//	}
//	if (sum * 1.0 > h * w / 5)type = 0;
//	else type = 1;
//	//std::cout << "sum:" << sum*1.0/(h*w)  << endl;
//	return type;
//}

////д����
//int writeResult(int type,map<string, string> result) {
//	map <string, string>::iterator res_iter;
//	Json::Value root;
//	Json::Value info;
//
//	for (res_iter = result.begin(); res_iter != result.end(); res_iter++) {
//		//cout << res_iter->first << ":" << res_iter->second << endl;
//		info[res_iter->first] = Json::Value(res_iter->second);
//	}
//	if (type == 0)root["�������֤"] = Json::Value(info);
//	else if (type == 1)root["������֤"] = Json::Value(info);
//	//�������  
//	Json::StyledWriter sw;
//	cout << sw.write(root) << endl << endl;
//	//������ļ�  
//	ofstream os;
//	remove("recogResult.json");
//	os.open("recogResult.json", std::ios::out | std::ios::app);
//	if (!os.is_open())
//		cout << "error��can not find or create the file which named \" result.json\"." << endl;
//	//os.clear();
//	os << sw.write(root);
//	os.close();
//
//}


//void* init_crnn_net(std::string model_path) {
//	OCR* ocr = new OCR;
//	std::string pb_path = model_path + "\\out.pb";
//	std::string char_table_path = model_path + "\\char_table.txt";
//	ocr->setNumClass(7822);//5991
//	ocr->readCharTable(char_table_path);
//	//ocr.LoadGraph(pb_path);
//
//	if(ocr->LoadGraph(pb_path) != Status::OK())return NULL;
//	
//	tensorflow::Tensor input_tensor(tensorflow::DT_FLOAT, tensorflow::TensorShape({ 1,32,320,3 }));
//	for (int row = 0; row < 32; ++row) {
//		for (int col = 0; col < 320; ++col) {
//			for (int depth = 0; depth < 3; ++depth) {
//				input_tensor.tensor<float, 4>()(0, row, col, depth) = 0.0f;
//			}
//		}
//	}
//	ocr->net_forward(input_tensor);
//	std::cout << "crnnģ�ͳ�ʼ���ɹ�" << std::endl;
//	return (void*)ocr;
//}
//
//
//void* init_yolo_model(string model_path) {
//	YOLO* yolo = new YOLO;
//	std::string yoloConfigPath = model_path + "\\text_tiny2_anchor.cfg";
//	std::string yoloWeightsPath = model_path + "\\text_tiny2_last.weights";
//	int flag = yolo->init_yolo(yoloConfigPath, yoloWeightsPath);
//	if (flag == 0)return (void*)yolo;
//	else return NULL;
//
//}
//
////����1:ͼƬ��ȡ����
////����2:δ֪֤������
////����3��֤����С������Ҫ��
//int cardRecog(void* tmp_crnn, void* tmp_yolo,std::string irFront, std::string irBack, std::string viFront, std::string viBack) {
//	OCR* ocr = (OCR*)tmp_crnn;
//	YOLO* yolo = (YOLO*)tmp_yolo;
//	//��ȡͼƬ
//	cv::Mat viFrontImg;
//	cv::Mat viBackImg;
//	cv::Mat irFrontImg;
//	cv::Mat irBackImg;
//	viFrontImg = cv::imread(viFront);
//	viBackImg = cv::imread(viBack);
//	irFrontImg = cv::imread(irFront);
//	irBackImg = cv::imread(irBack);
//	if(viFrontImg.cols==0||viBackImg.cols==0|| irFrontImg.cols==0|| irBackImg.cols==0)return 1;//ͼƬ·������
//
//	cv::Mat srcImg;
//	int type = IdenCardType(srcImg,irFrontImg, irBackImg, viFrontImg, viBackImg);
//	//cv::imshow("", srcImg);
//	//cv::waitKey(0);
//	if (type == -1) {
//		//std::cout << "δ֪֤������" << endl;
//		return 2;
//	}
//	if (type == -2) {
//		//std::cout << "֤��ͼ���С������Ҫ��" << endl;
//		return 3;
//	}
//	std::vector<cv::Rect> boxes;
//	yolo->text_detect(srcImg.clone(), boxes);//yoloģ��·��Ҫע��
//
//	map<string, string> result;
//	if (type == 0) {
//		AMpInfo(ocr, boxes, srcImg, result);
//	}
//	else if (type == 1) {
//		//nhkpinfo(&ocr, boxes, whiteimg,result);
//		nHKpInfo(ocr, boxes, srcImg, result);
//	}
//	writeResult(type,result);
//	return 0;
//	/*map <string, string>::iterator res_iter;
//	for (res_iter = result.begin(); res_iter != result.end(); res_iter++) {
//		cout << res_iter->first << ":" << res_iter->second << endl;
//	}*/
//
//}

int main() {
	// yoloģ��·��
	const char* yolo_model_path = ".\\models";
	void* yolo = init_yolo_model(yolo_model_path);
	if (yolo == NULL)return -1;
	//crnnģ��
	const char* crnn_model_path = ".\\·������";
	
	/*fstream _file;
	_file.open(".\\·������\\out.pb", ios::in);
	if (!_file)
	{
		cout << crnn_model_path << "û�б�����!" << endl;
	}*/

	void* ocr=init_crnn_net(crnn_model_path);
	if (ocr == NULL)return -2;

	const char* irfront = "c:\\users\\admin\\desktop\\gwiic60a11\\���������\\image_ir_a.bmp";
	const char* irback = "c:\\users\\admin\\desktop\\gwiic60a11\\���������\\image_ir_b.bmp";
	const char* vifront = "c:\\users\\admin\\desktop\\gwiic60a11\\���������\\image_vi_a.bmp";
	const char* viback = "c:\\users\\admin\\desktop\\gwiic60a11\\���������\\image_vi_b.bmp";

	std::cout << getCardInfo("./", ocr, yolo, irfront, irback, vifront, viback) << std::endl;
	
	return 0;


	////Ѱ��mrz
	void* mrz = initMrzNet("F:/hahaha/backup/2020-01/ocr/charRecognition/models/mrz32_61.pb");
	const char* path1 = "C:\\Users\\admin\\Desktop\\mrz\\ch1A.bmp";
	const char* path2 = "C:\\Users\\admin\\Desktop\\mrz\\ch1B.bmp";
	return recognizeMrz("./",mrz, path1, path2);


	//string imgDir = "F:\\hahaha\\backup\\2020-01\\��Ҫ����\\����img\\type22";
	//std::string find = imgDir + "\\*.jpg";//�������е�jpg�ļ�
	//string name;
	//_finddata_t file ;
	//long lf;
	////�����ļ���·��
	//lf = _findfirst(find.c_str(),&file);
	////cout << lf << endl;
	//while (_findnext(lf,&file) == 0) {
	//	//����ļ���
	//	//cout<<file.name<<endl;
	//	if (strcmp(file.name, ".") == 0 || strcmp(file.name, "..") == 0)
	//		continue;
	//	std::string path = imgDir +"\\"+ file.name;
	//	cout << path << endl;
	//	
	//	recognizeMrz(mrz, "",path);
	//	/*cv::Mat img = imread(path);
	//	cv::imshow("", img);
	//	cv::waitKey(0);
	//	cv::destroyAllWindows();
	//	std::vector<cv::Mat> result;
	//	findMRZ(img.clone(),result);
	//	for (cv::Mat r : result) {
	//		cv::imshow("", r);
	//		cv::waitKey(0);
	//		cv::destroyAllWindows();
	//	}*/
	//}
	//return 0;
}

