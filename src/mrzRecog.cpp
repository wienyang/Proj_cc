#pragma once
#include"mrzRecog.h"

bool glog_initialized1 = false;

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
//-2:写入mrz失败 2020 05 04
int recognizeMrz(const char* saveDir,void* mrz, const char* irFront, const char* irBack) {
	std::string resultOut = saveDir;
	resultOut = resultOut + "\\wy";//结果保存目录
	std::string log_dir = resultOut + "\\log";//日志目录
	CreateDirectory(resultOut.c_str(), NULL);
	CreateDirectory(log_dir.c_str(), NULL);

	FLAGS_log_dir = log_dir;
	if (!glog_initialized1)
	{
		google::InitGoogleLogging("mrzRecog");
		glog_initialized1 = true;
	}

	std::string resultPath = resultOut + "\\mrz.txt";//mrz保存路径
	std::vector<cv::Mat> lineImgs;
	MRZ* mrzModel = (MRZ*)mrz;
	LOG(INFO) << "recognize mrz start...";
	if (NULL == mrzModel)LOG(ERROR) << "mrzModel is NULL...";

	int type = -1;
	//检测并识别正面
	cv::Mat irFrontImg = cv::imread(irFront);
	if (!irFrontImg.empty()) {
		LOG(INFO) << "processing irFront img...";
		//cv::imshow("", irFrontImg);
		//cv::waitKey(0);
		type = findMRZ(irFrontImg, lineImgs);
		if (type != -1) {
			LOG(INFO) << "found mrz...";
			mrzModel->segment(lineImgs);
			LOG(INFO) << "segment mrz done...";
			mrzModel->recognize();
			LOG(INFO) << "recognize mrz done...";
			mrzModel->checkMrzStr();
			LOG(INFO) << "check mrz done...";
			std::ofstream out;
			out.open(resultPath, ofstream::out);
			if (out.is_open())
			{
				//out << mrzModel->getCardType() << std::endl;
				out << mrzModel->mrzStr();
				out.close();
				LOG(INFO) << "write mrz done...";
				//关闭glog
				if (glog_initialized1)
				{
					google::ShutdownGoogleLogging();
					glog_initialized1 = false;
				}
				return mrzModel->getCardType();//成功识别
			}else {
				std::cout << "cant open file ./mrz.txt/" << std::endl;
				LOG(INFO) << "write mrz failed...";
				//关闭glog
				if (glog_initialized1)
				{
					google::ShutdownGoogleLogging();
					glog_initialized1 = false;
				}
				return -2;
			}
		}
	}

	//检测并识别反面
	cv::Mat irBackImg = cv::imread(irBack);
	if (!irBackImg.empty()) {
		LOG(INFO) << "processing irBack img...";
		//cv::imshow("", irBackImg);
		//cv::waitKey(0);
		type = findMRZ(irBackImg, lineImgs);
		if (type != -1) {
			LOG(INFO) << "found mrz...";
			mrzModel->segment(lineImgs);
			LOG(INFO) << "segment mrz done...";
			mrzModel->recognize();
			LOG(INFO) << "recognize mrz done...";
			mrzModel->checkMrzStr();
			LOG(INFO) << "check mrz done...";
			std::ofstream out;
			out.open("./wy/mrz.txt", ofstream::out);
			if (out.is_open())
			{
				out << mrzModel->getCardType() << std::endl;
				out << mrzModel->mrzStr();
				out.close();
				LOG(INFO) << "write mrz done...";
				//关闭glog
				if (glog_initialized1)
				{
					google::ShutdownGoogleLogging();
					glog_initialized1 = false;
				}
				return mrzModel->getCardType();//成功识别
			}else {
				std::cout << "cant open file ./mrz.txt/" << std::endl;
				LOG(INFO) << "write mrz failed...";
				//关闭glog
				if (glog_initialized1)
				{
					google::ShutdownGoogleLogging();
					glog_initialized1 = false;
				}
				return -2;
			}
		}
	}
	LOG(INFO) << "detect mrz zone failed...";
	//关闭glog
	if (glog_initialized1)
	{
		google::ShutdownGoogleLogging();
		glog_initialized1 = false;
	}
	return -1;//未能找到机读码


}