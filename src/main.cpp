#pragma once
#include<ctime>
#include<windows.h>

#include <direct.h>
#include <filesystem>


#include"cardInfoRecog.h"
#include"mrzRecog.h"



#pragma comment(lib,"tensorflow.lib")
#pragma warning(suppress : 4996)


int main() {
	// yolo模型路径
	const char* yolo_model_path = ".\\models";
	void* yolo = init_yolo_model(yolo_model_path);
	if (yolo == NULL)return -1;
	//crnn模型
	const char* crnn_model_path = ".\\crnn_model";
	
	fstream _file;
	_file.open(".\\路径测试\\out.pb", ios::in);
	if (!_file)
	{
		cout << crnn_model_path << "没有被创建!" << endl;
	}

	void* ocr=init_crnn_net(crnn_model_path);
	if (ocr == NULL)return -2;

	const char* irFront = "C:\\Users\\admin\\Desktop\\GWIIC60A11\\香港永久新\\image_ir_A.bmp";
	const char* irBack = "C:\\Users\\admin\\Desktop\\GWIIC60A11\\香港永久新\\image_ir_B.bmp";
	const char* viFront = "C:\\Users\\admin\\Desktop\\GWIIC60A11\\香港永久新\\image_vi_A.bmp";
	const char* viBack = "C:\\Users\\admin\\Desktop\\GWIIC60A11\\香港永久新\\image_vi_B.bmp";

	//std::cout << getCardInfo("./", ocr, yolo, irFront, irBack, viFront, viBack) << std::endl;
	
	//return 0;


	////寻找mrz
	void* mrz = initMrzNet("F:/hahaha/backup/2020-01/ocr/charRecognition/models/mrz32_61.pb");
	const char* path1 = "C:\\Users\\admin\\Desktop\\mrz\\ch1A.bmp";
	const char* path2 = "C:\\Users\\admin\\Desktop\\mrz\\ch1B.bmp";
	//std::cout << "mrz识别。。。。" << std::endl;
	for (int i = 0; i < 100; i++) {
		std::cout << getCardInfo("./", ocr, yolo, irFront, irBack, viFront, viBack) << std::endl;
		recognizeMrz("./",mrz, path1, path2);
	}

	return 0;
}

