#pragma once
#include<string>


extern "C" {

	//模型初始化API
	//输入crnn模型文件夹所在路径，初始化失败返回NULL,初始化成功返回模型指针
	 __declspec(dllexport) void* init_crnn_net(const char* model_path);
	 //输入yolo模型文件夹所在路径，初始化失败返回NULL,初始化成功返回模型指针
	 __declspec(dllexport) void* init_yolo_model(const char* model_path);
	 //输入mrz模型文件所在路径，初始化失败返回NULL,初始化成功返回模型指针
	 __declspec(dllexport) void* initMrzNet(const char* modelPath);
	 

	 /*证件件表面信息识别api
		 输入：
			tmp_crnn:crnn模型指针
			tmp_yolo:yolo模型指针
			irFront,irBack,viFront,viBack分别为ir图像ab面和vi图像的ab面
		 返回值：
				返回cardFlag:识别成功
				0    澳门，正面正向
				1    澳门，正面反向
				2	   澳门，反面正向
				3    澳门，反面反向
				10   香港，正面正向
				11   香港，正面反向
				12   香港，反面正向
				13   香港，反面反向
			返回-1:图片读取错误
			返回-2:未知证件类型
			返回-3：证件大小不符合要求
			返回-4：获取的信息不全，认为错误类型证件*/
	 __declspec(dllexport) int getCardInfo(const char* saveDir,void* tmp_crnn, void* tmp_yolo, const char* irFront, const char* irBack, const char* viFront, const char* viBack);


	 /*mrz识别api
		 输入：
			mrz:mrz模型指针
			irFront,irBack分别为ir图像ab面
		 返回值:
			//0，1，2，3:成功
				//0,csc,往来港澳通行证
				//1，cdl,往来台湾通行证
				//2，crh,港澳居民往来内地通行证
				//3，ct,台湾居民往来大陆通行证
			//-1：未能找到机读码*/
	 __declspec(dllexport) int recognizeMrz(const char* saveDir,void* mrz, const char* irFront, const char* irBack);
}

