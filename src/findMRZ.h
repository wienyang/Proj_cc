#pragma once
#include <opencv2\opencv.hpp>
#include <string>
#include <vector>
using namespace std;
using namespace cv;

int removeOutlierContours(Mat& img);
int findMRZ(Mat img,vector<Mat>& result);

