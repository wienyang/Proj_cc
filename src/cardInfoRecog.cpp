#include "cardInfoRecog.h"


void getFace(std::string resultOut,int cardType, cv::Mat vi_A, cv::Mat ir_A) {
	cv::Mat viFace, irFace;
	cv::Rect faceArea;
	switch (cardType)
	{
	case 0://�������֤
		faceArea = { 690,60,265,330 };
		break;
	case 1://���������
		faceArea = { 70,255,240,340 };
		break;
	case 2://������þ�
		faceArea = { 705,220,220,280 };
		break;
	case 3://��۷����þ�
		faceArea = { 705,210,210,290 };
		break;
	default:
		return;
	}
	viFace = vi_A(faceArea).clone();
	irFace = ir_A(faceArea).clone();
	//д��ͼƬ
	cv::imwrite(resultOut + "\\viFace.bmp", viFace);
	cv::imwrite(resultOut + "\\irFace.bmp", irFace);

}
//д����
int writeResult(std::string resultOut, std::map<std::string, std::string> result) {

	std::string resultPath = resultOut + "\\cardInfo.txt";//�ļ�����·��
	std::map <std::string, std::string>::iterator res_iter;

	Json::Value info;
	for (res_iter = result.begin(); res_iter != result.end(); res_iter++) {
		info[res_iter->first] = Json::Value(res_iter->second);
	}

	//���  
	Json::FastWriter  fw;
	//std::cout << fw.write(info) << std::endl;
	//������ļ�  
	std::ofstream out;
	out.open(resultPath, ofstream::out);
	//std::ofstream os;
	//os.open(resultPath, std::ios::out);
	if (!out.is_open())
		std::cout << "error��can not find or create the file which named \"cardInfo.txt\"." << std::endl;
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
		std::cout << "ģ�ͼ���ʧ��" << std::endl;
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
	std::cout << "crnnģ�ͳ�ʼ���ɹ�" << std::endl;
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
*����cardFlag:ʶ��ɹ�
*	cardFlag = cardType * 10 + cardDirection
*	cardType   			    cardDirection
*	0    ����				0	��������
*	1    ���������			1	���淴��
*	2	 ������þ�			2	��������
*	3    ��۷����þ�		3	���淴��
*	4    ������֤��δ��ϸ�֣�
*����-1:ͼƬ��ȡ����
*����-2:δ֪֤������
*����-3��֤����С������Ҫ��
*����-4����ȡ����Ϣ��ȫ����Ϊ��������֤��
*/

int getCardInfo(const char* saveDir,void* tmp_crnn, void* tmp_yolo, 
	const char* viFront, const char* viBack,const char* irFront, const char* irBack,const char* uvFront,const char*uvBack) {
	//�����ļ�����Ŀ¼
	std::string resultOut = saveDir;
	resultOut = resultOut + "\\wy";
	CreateDirectory(resultOut.c_str(), NULL);

	OCR* ocr = (OCR*)tmp_crnn;
	YOLO* yolo = (YOLO*)tmp_yolo;
	//��ȡͼƬ
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
	if (viFrontImg.empty()|| viBackImg.empty() || irFrontImg.empty() || irBackImg.empty() || uvFrontImg.empty()|| uvBackImg.empty())return -1;//ͼƬ·������
	
	int cardFlag = IdenCardType(viFrontImg, viBackImg,irFrontImg, irBackImg,uvFrontImg, uvBackImg);//�������Ͳ�У��ͼƬ
	getFace(resultOut, cardFlag/10, viFrontImg, irFrontImg);//��ȡ����

	if (cardFlag == -1) {//δ֪֤������
		return -2;
	}
	if (cardFlag == -2) {//֤����С������Ҫ��
		return -3;
	}
	std::vector<cv::Rect> boxes;
	yolo->text_detect(irFrontImg.clone(), boxes);//yoloģ��·��Ҫע��

	std::map<string, string> result;
	if (cardFlag /10 == 0) {//����
		AMpInfo(ocr, boxes, irFrontImg, result);
		//cout << "�����Ŀ" << result.size() << endl;
		if (result.size() < 7)return -4;//ʶ������ȫ����Ϊ�Ǵ����֤��
	}
	else if (cardFlag /10 >= 1) {//���
		nHKpInfo(ocr, boxes, irFrontImg, result);
		if (result.size() < 6)return -4;//ʶ������ȫ����Ϊ�Ǵ����֤��
	}
	writeResult(resultOut, result);
	return cardFlag;
}