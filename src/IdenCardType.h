#pragma once
#include<opencv.hpp>


int IdenCardType(cv::Mat& srcImg, cv::Mat frontImg, cv::Mat backImg,cv::Mat colorFront, cv::Mat colorBack);