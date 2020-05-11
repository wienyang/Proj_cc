#pragma once
#include<json/json.h>
#include<fstream>
#include "yolo3.h"
#include "ocr.h"
#include"N_HK_P.h"
#include"AM_P.h"
#include"HK_AM.h"
#include"IdenCardType.h"
#include"cardRecog.h"


//#define GLOG_NO_ABBREVIATED_SEVERITIES 
//#include "glog/logging.h"


void* init_crnn_net(const char* model_path);
void* init_yolo_model(const char* model_path);
int getCardInfo(const char* saveDir, void* tmp_crnn, void* tmp_yolo,
	const char* viFront, const char* viBack,const char* irFront, const char* irBack, const char* uvFront, const char* uvBack);
