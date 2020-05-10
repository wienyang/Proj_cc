#pragma once
#include<ctime>
#include<windows.h>

#include <direct.h>
#include <filesystem>

#include"cardInfoRecog.h"
#include"mrzRecog.h"

#include"IdenCardType.h"



#pragma comment(lib,"tensorflow.lib")
#pragma warning(suppress : 4996)



int main() {
	//// yoloģ��·��
	//const char* yolo_model_path = ".\\models";
	//void* yolo = init_yolo_model(yolo_model_path);
	//if (yolo == NULL)return -1;
	////crnnģ��
	//const char* crnn_model_path = ".\\·������";
	//void* ocr=init_crnn_net(crnn_model_path);
	//if (ocr == NULL)return -2;

	//const char* irfront = "C:\\Users\\admin\\Desktop\\GWIIC60A11\\������þ�\\image_ir_B.bmp";
	//const char* irback = "C:\\Users\\admin\\Desktop\\GWIIC60A11\\������þ�\\image_ir_A.bmp";
	//const char* uvfront = "C:\\Users\\admin\\Desktop\\GWIIC60A11\\������þ�\\image_uv_B.bmp";
	//const char* uvback = "C:\\Users\\admin\\Desktop\\GWIIC60A11\\������þ�\\image_uv_A.bmp";
	//const char* vifront = "C:\\Users\\admin\\Desktop\\GWIIC60A11\\������þ�\\image_vi_B.bmp";
	//const char* viback = "C:\\Users\\admin\\Desktop\\GWIIC60A11\\������þ�\\image_vi_A.bmp";

	////ʶ��֤��������Ϣ
	//getCardInfo("./", ocr, yolo, irfront, irback, vifront, viback) << std::endl;

	////Ѱ��mrz
	//void* mrz = initMrzNet("F:/hahaha/backup/2020-01/ocr/charRecognition/models/mrz32_61.pb");
	//const char* path1 = "C:\\Users\\admin\\Desktop\\mrz\\ch1A.bmp";
	//const char* path2 = "C:\\Users\\admin\\Desktop\\mrz\\ch1B.bmp";
	//return recognizeMrz("./",mrz, path1, path2);

	while (true) {
		int i = 0;
		std::string imgs[6];
		for (int i = 0; i < 6; i++) {
			cin >> imgs[i];
		}

		cv::Mat irFrontImg = cv::imread(imgs[0]);
		cv::Mat irBackImg = cv::imread(imgs[1]);
		cv::Mat uvFrontImg = cv::imread(imgs[2]);
		cv::Mat uvBackImg = cv::imread(imgs[3]);
		cv::Mat viFrontImg = cv::imread(imgs[4]);
		cv::Mat viBackImg = cv::imread(imgs[5]);

		std::cout<<IdenCardType(irFrontImg, irBackImg, uvFrontImg, uvBackImg, viFrontImg, viBackImg)<<std::endl;

		cv::imshow("",irFrontImg);
		cv::waitKey(0);
		cv::imshow("", irBackImg);
		cv::waitKey(0);
		cv::imshow("", uvFrontImg);
		cv::waitKey(0);
		cv::imshow("", uvBackImg);
		cv::waitKey(0);
		cv::imshow("", viFrontImg);
		cv::waitKey(0);
		cv::imshow("", viBackImg);
		cv::waitKey(0);
		cv::destroyAllWindows();
	}
	return 0;

}

