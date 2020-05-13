#pragma once


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
			viFront,viBack,irFront,irBack,uvFront,uvBack分别为vi、ir和uv图像的ab面
		 返回值：
				返回cardFlag:识别成功
					cardFlag = cardType * 10 + cardDirection
					cardType   			    cardDirection
					0    澳门				0	正面正向
					1    香港永久新			1	正面反向
					2	 香港永久旧			2	反面正向
					3    香港非永久旧		3	反面反向
					4    香港身份证（未能细分）
				返回-1:图片读取错误
				返回-2:未知证件类型
				返回-3：证件大小不符合要求
				返回-4：获取的信息不全，认为错误类型证件
	*/
	 __declspec(dllexport) int getCardInfo(const char* saveDir, void* tmp_crnn, void* tmp_yolo,
		const char* viFront, const char* viBack, const char* irFront, const char* irBack,const char* uvFront, const char* uvBack);

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

