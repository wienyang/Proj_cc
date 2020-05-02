#pragma once
#include<vector>
#include<regex>
#include<map>
#include "ocr.h"
#include "tools.h"

void AMpInfo(OCR* ocrModel, std::vector<cv::Rect> boxes, cv::Mat img, std::map<std::string, std::string>& result);