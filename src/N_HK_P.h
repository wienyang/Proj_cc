#pragma once
#include<vector>
#include<regex>
#include<map>
#include<algorithm>

#include "ocr.h"
#include "tools.h"


void nHKpInfo(OCR* ocrModel, std::vector<cv::Rect> boxes, cv::Mat img, std::map<std::string, std::string>& result);

