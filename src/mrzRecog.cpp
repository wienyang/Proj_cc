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

//0，1，2，3:成功
	//0, csc,往来港澳通行证
	//1，cdl,往来台湾通行证
	//2，crh,港澳居民往来内地通行证
	//3，ct,台湾居民往来大陆通行证
//-1：未能找到机读码
int recognizeMrz(const char* saveDir,void* mrz, const char* irFront, const char* irBack) {
	std::string resultOut = saveDir;
	resultOut = resultOut + "\\wy";
	CreateDirectory(resultOut.c_str(), NULL);
	std::string resultPath = resultOut + "\\mrz.txt";
	std::vector<cv::Mat> lineImgs;
	MRZ* mrzModel = (MRZ*)mrz;
	int type = -1;

	//检测并识别正面
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
			return mrzModel->getCardType();//成功识别
		}
	}

	//检测并识别反面
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
			return mrzModel->getCardType();//成功识别
		}
	}
	return -1;//未能找到机读码


}