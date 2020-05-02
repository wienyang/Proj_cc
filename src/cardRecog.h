#pragma once
#include<string>


extern "C" {

	//ģ�ͳ�ʼ��API
	//����crnnģ���ļ�������·������ʼ��ʧ�ܷ���NULL,��ʼ���ɹ�����ģ��ָ��
	 __declspec(dllexport) void* init_crnn_net(const char* model_path);
	 //����yoloģ���ļ�������·������ʼ��ʧ�ܷ���NULL,��ʼ���ɹ�����ģ��ָ��
	 __declspec(dllexport) void* init_yolo_model(const char* model_path);
	 //����mrzģ���ļ�����·������ʼ��ʧ�ܷ���NULL,��ʼ���ɹ�����ģ��ָ��
	 __declspec(dllexport) void* initMrzNet(const char* modelPath);
	 

	 /*֤����������Ϣʶ��api
		 ���룺
			tmp_crnn:crnnģ��ָ��
			tmp_yolo:yoloģ��ָ��
			irFront,irBack,viFront,viBack�ֱ�Ϊirͼ��ab���viͼ���ab��
		 ����ֵ��
				����cardFlag:ʶ��ɹ�
				0    ���ţ���������
				1    ���ţ����淴��
				2	   ���ţ���������
				3    ���ţ����淴��
				10   ��ۣ���������
				11   ��ۣ����淴��
				12   ��ۣ���������
				13   ��ۣ����淴��
			����-1:ͼƬ��ȡ����
			����-2:δ֪֤������
			����-3��֤����С������Ҫ��
			����-4����ȡ����Ϣ��ȫ����Ϊ��������֤��*/
	 __declspec(dllexport) int getCardInfo(const char* saveDir,void* tmp_crnn, void* tmp_yolo, const char* irFront, const char* irBack, const char* viFront, const char* viBack);


	 /*mrzʶ��api
		 ���룺
			mrz:mrzģ��ָ��
			irFront,irBack�ֱ�Ϊirͼ��ab��
		 ����ֵ:
			//0��1��2��3:�ɹ�
				//0,csc,�����۰�ͨ��֤
				//1��cdl,����̨��ͨ��֤
				//2��crh,�۰ľ��������ڵ�ͨ��֤
				//3��ct,̨�����������½ͨ��֤
			//-1��δ���ҵ�������*/
	 __declspec(dllexport) int recognizeMrz(const char* saveDir,void* mrz, const char* irFront, const char* irBack);
}

