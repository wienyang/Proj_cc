#pragma once
#include<algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include<vector>
//#include <ctime>

using namespace std;
using namespace cv::dnn;

class YOLO {
public:
	YOLO() {};
	~YOLO() {};
	int init_yolo(string yoloConfigPath, string yoloWeightsPath);
	int text_detect(cv::Mat img, vector<cv::Rect>& boxes);

private:
	Net net;
};