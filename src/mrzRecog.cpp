#pragma once
#include"mrzRecog.h"

void* initMrzNet(const char* modelPath)
{
	MRZ* mrz = new MRZ;
	try {
		mrz->initNet(modelPath);
	}
	catch (...) {
		return NULL;
	}
	return (void*)mrz;
}

//0��1��2��3:�ɹ�
	//0, csc,�����۰�ͨ��֤
	//1��cdl,����̨��ͨ��֤
	//2��crh,�۰ľ��������ڵ�ͨ��֤
	//3��ct,̨�����������½ͨ��֤
//-1��δ���ҵ�������
int recognizeMrz(const char* saveDir,void* mrz, const char* irFront, const char* irBack) {
	std::string resultOut = saveDir;
	resultOut = resultOut + "\\wy";
	CreateDirectory(resultOut.c_str(), NULL);
	std::string resultPath = resultOut + "\\mrz.txt";
	std::vector<cv::Mat> lineImgs;
	MRZ* mrzModel = (MRZ*)mrz;
	int type = -1;

	//��Ⲣʶ������
	cv::Mat irFrontImg = cv::imread(irFront);
	if (!irFrontImg.empty()) {
		//cv::imshow("", irFrontImg);
		//cv::waitKey(0);
		type = findMRZ(irFrontImg, lineImgs);
		if (type != -1) {
			mrzModel->segment(lineImgs);
			mrzModel->recognize();
			mrzModel->checkMrzStr();
			std::ofstream out;
			out.open(resultPath, ofstream::out);
			if (out.is_open())
			{
				//out << mrzModel->getCardType() << std::endl;
				out << mrzModel->mrzStr();
				out.close();
			}
			else {
				std::cout << "cant open file ./mrz.txt/" << std::endl;
			}
			return mrzModel->getCardType();//�ɹ�ʶ��
		}
	}

	//��Ⲣʶ����
	cv::Mat irBackImg = cv::imread(irBack);
	if (!irBackImg.empty()) {
		//cv::imshow("", irBackImg);
		//cv::waitKey(0);
		type = findMRZ(irBackImg, lineImgs);
		if (type != -1) {
			mrzModel->segment(lineImgs);
			mrzModel->recognize();
			mrzModel->checkMrzStr();
			std::ofstream out;
			out.open("./wy/mrz.txt", ofstream::out);
			if (out.is_open())
			{
				out << mrzModel->getCardType() << std::endl;
				out << mrzModel->mrzStr();
				out.close();
			}
			return mrzModel->getCardType();//�ɹ�ʶ��
		}
	}
	return -1;//δ���ҵ�������


}