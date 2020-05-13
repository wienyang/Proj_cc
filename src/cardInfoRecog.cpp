#include "cardInfoRecog.h"


void getFace(std::string resultOut,int cardType, cv::Mat vi_A, cv::Mat ir_A) {
	cv::Mat viFace, irFace;
	cv::Rect faceArea;
	switch (cardType)
	{
	case 0://澳门身份证
		faceArea = { 690,60,265,330 };
		break;
	case 1://香港永久新
		faceArea = { 70,255,240,340 };
		break;
	case 2://香港永久旧
		faceArea = { 705,220,220,280 };
		break;
	case 3://香港非永久旧
		faceArea = { 705,210,210,290 };
		break;
	default:
		return;
	}
	viFace = vi_A(faceArea).clone();
	irFace = ir_A(faceArea).clone();
	//写入图片
	cv::imwrite(resultOut + "\\viFace.bmp", viFace);
	cv::imwrite(resultOut + "\\irFace.bmp", irFace);

}
//写入结果
int writeResult(std::string resultOut, std::map<std::string, std::string> result) {

	std::string resultPath = resultOut + "\\cardInfo.txt";//文件保存路径
	std::map <std::string, std::string>::iterator res_iter;

	Json::Value info;
	for (res_iter = result.begin(); res_iter != result.end(); res_iter++) {
		info[res_iter->first] = Json::Value(res_iter->second);
	}

	//输出  
	Json::FastWriter  fw;
	//std::cout << fw.write(info) << std::endl;
	//输出到文件  
	std::ofstream out;
	out.open(resultPath, ofstream::out);
	//std::ofstream os;
	//os.open(resultPath, std::ios::out);
	if (!out.is_open())
		std::cout << "error：can not find or create the file which named \"cardInfo.txt\"." << std::endl;
	//os.clear();
	out << fw.write(info);
	out.close();

}

void* init_crnn_net(const char* model_path) {
	OCR* ocr = new OCR;
	std::string modelPath = model_path;
	std::string pb_path = modelPath + "\\out.pb";
	pb_path = GBKToUTF8(pb_path);
	std::string char_table_path = modelPath + "\\char_table.txt";
	ocr->setNumClass(7822);//5991
	ocr->readCharTable(char_table_path);
	//std::cout << ocr->char_table.length()<<std::endl;
	//ocr.LoadGraph(pb_path);

	if (ocr->LoadGraph(pb_path) != tensorflow::Status::OK()) {
		std::cout << "模型加载失败" << std::endl;
		return NULL;
	}
	tensorflow::Tensor input_tensor(tensorflow::DT_FLOAT, tensorflow::TensorShape({ 1,32,320,3 }));
	for (int row = 0; row < 32; ++row) {
		for (int col = 0; col < 320; ++col) {
			for (int depth = 0; depth < 3; ++depth) {
				input_tensor.tensor<float, 4>()(0, row, col, depth) = 0.0f;
			}
		}
	}
	ocr->net_forward(input_tensor);
	std::cout << "crnn模型初始化成功" << std::endl;
	return (void*)ocr;
}

void* init_yolo_model(const char* model_path) {
	YOLO* yolo = new YOLO;
	std::string modelPath = model_path;
	std::string yoloConfigPath = modelPath + "\\text_tiny2_anchor.cfg";
	std::string yoloWeightsPath = modelPath + "\\text_tiny2_last.weights";
	int flag = yolo->init_yolo(yoloConfigPath, yoloWeightsPath);
	if (flag == 0)return (void*)yolo;
	else return NULL;

}

/**
*返回cardFlag:识别成功
*	cardFlag = cardType * 10 + cardDirection
*	cardType   			    cardDirection
*	0    澳门				0	正面正向
*	1    香港永久新			1	正面反向
*	2	 香港永久旧			2	反面正向
*	3    香港非永久旧		3	反面反向
*	4    香港身份证（未能细分）
*返回-1:图片读取错误
*返回-2:未知证件类型
*返回-3：证件大小不符合要求
*返回-4：获取的信息不全，认为错误类型证件
*/

int getCardInfo(const char* saveDir,void* tmp_crnn, void* tmp_yolo, 
	const char* viFront, const char* viBack,const char* irFront, const char* irBack,const char* uvFront,const char*uvBack) {
	//创建文件保存目录
	std::string resultOut = saveDir;
	resultOut = resultOut + "\\wy";
	CreateDirectory(resultOut.c_str(), NULL);

	OCR* ocr = (OCR*)tmp_crnn;
	YOLO* yolo = (YOLO*)tmp_yolo;
	//读取图片
	cv::Mat viFrontImg;
	cv::Mat viBackImg;
	cv::Mat irFrontImg;
	cv::Mat irBackImg;
	cv::Mat uvFrontImg;
	cv::Mat uvBackImg;
	viFrontImg = cv::imread(viFront);
	viBackImg = cv::imread(viBack);
	irFrontImg = cv::imread(irFront);
	irBackImg = cv::imread(irBack);
	uvFrontImg = cv::imread(uvFront);
	uvBackImg = cv::imread(uvBack);
	if (viFrontImg.empty()|| viBackImg.empty() || irFrontImg.empty() || irBackImg.empty() || uvFrontImg.empty()|| uvBackImg.empty())return -1;//图片路径错误
	
	int cardFlag = IdenCardType(viFrontImg, viBackImg,irFrontImg, irBackImg,uvFrontImg, uvBackImg);//区分类型并校正图片
	getFace(resultOut, cardFlag/10, viFrontImg, irFrontImg);//获取人脸

	if (cardFlag == -1) {//未知证件类型
		return -2;
	}
	if (cardFlag == -2) {//证件大小不符合要求
		return -3;
	}
	std::vector<cv::Rect> boxes;
	yolo->text_detect(irFrontImg.clone(), boxes);//yolo模型路径要注意

	std::map<string, string> result;
	if (cardFlag /10 == 0) {//澳门
		AMpInfo(ocr, boxes, irFrontImg, result);
		//cout << "结果条目" << result.size() << endl;
		if (result.size() < 7)return -4;//识别结果不全，认为是错误的证件
	}
	else if (cardFlag /10 >= 1) {//香港
		nHKpInfo(ocr, boxes, irFrontImg, result);
		if (result.size() < 6)return -4;//识别结果不全，认为是错误的证件
	}
	writeResult(resultOut, result);
	return cardFlag;
}