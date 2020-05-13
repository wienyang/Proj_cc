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
	//// yolo模型路径
	//const char* yolo_model_path = ".\\models";
	//void* yolo = init_yolo_model(yolo_model_path);
	//if (yolo == NULL)return -1;
	////crnn模型
	//const char* crnn_model_path = ".\\路径测试";
	//void* ocr=init_crnn_net(crnn_model_path);
	//if (ocr == NULL)return -2;

	//const char* irfront = "C:\\Users\\admin\\Desktop\\GWIIC60A11\\香港永久旧\\image_ir_B.bmp";
	//const char* irback = "C:\\Users\\admin\\Desktop\\GWIIC60A11\\香港永久旧\\image_ir_A.bmp";
	//const char* uvfront = "C:\\Users\\admin\\Desktop\\GWIIC60A11\\香港永久旧\\image_uv_B.bmp";
	//const char* uvback = "C:\\Users\\admin\\Desktop\\GWIIC60A11\\香港永久旧\\image_uv_A.bmp";
	//const char* vifront = "C:\\Users\\admin\\Desktop\\GWIIC60A11\\香港永久旧\\image_vi_B.bmp";
	//const char* viback = "C:\\Users\\admin\\Desktop\\GWIIC60A11\\香港永久旧\\image_vi_A.bmp";

	////识别证件表面信息
	//getCardInfo("./", ocr, yolo, irfront, irback, vifront, viback) << std::endl;

	//寻找mrz
	void* mrz = initMrzNet("F:/hahaha/backup/2020-01/ocr/charRecognition/models/mrz32_61.pb");
	const char* path1 = "C:\\Users\\admin\\Desktop\\mrz\\2020-05-13_13-43-23irB.jpg";
	const char* path2 = "C:\\Users\\admin\\Desktop\\mrz\\2020-05-13_13-43-23irF.jpg";
	return recognizeMrz("./",mrz, path1, path2);

	/*while (true) {
		int i = 0;
		std::string imgs[6];
		for (int i = 0; i < 6; i++) {
			cin >> imgs[i];
		}
		const char* viFront = imgs[0].c_str();
		const char* viBack = imgs[1].c_str();
		const char* irFront = imgs[2].c_str();
		const char* irBack = imgs[3].c_str();
		const char* uvFront =imgs[4].c_str();
		const char* uvBack = imgs[5].c_str();

		std::cout<<getCardInfo(".\\", ocr,yolo,viFront, viBack, irFront, irBack, uvFront,uvBack)<<std::endl;


	}*/
	return 0;

}

