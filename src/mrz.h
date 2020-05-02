#pragma once
#include <opencv2\opencv.hpp>
#include <string>
#include <vector>


static const char charTable[37] = {
	'0', '1', '2', '3', '4', '5', '6',
	'7', '8', '9', 'A', 'B', 'C', 'D',
	'E', 'F', 'G', 'H', 'I', 'J', 'K',
	'L', 'M', 'N', 'O', 'P', 'Q', 'R',
	'S', 'T', 'U', 'V', 'W', 'X', 'Y',
	'Z', '<' };



class MRZ {
public:
	 MRZ(){};
	//MRZ(int type) :m_type(type) {}
	//MRZ(std::vector<cv::Mat> lineImgs);
	~MRZ();

public:

	bool initNet(std::string modelPath);//��ʼ��ģ��
	int segment(std::vector<cv::Mat>lineImgs);//�ָ�������ַ�
	void recognize();//ʶ��
	void checkMrzStr();//��������

	std::string mrzStr();//����ʶ��Ļ�������
	int getCardType();
	


#ifdef _DEBUG
public:
#else
private:
#endif // _DEBUG
	void alpha2num(char& c);
	void num2alpha(char& c);
	std::vector<std::vector<cv::Mat>> m_charImgMat;
	std::string m_mrzStr;
	cv::dnn::Net m_net;
	int mrzNum=0;//����������
	//0��CSC
	//1:CDL
	//2��CRH
	//3:CT
	//-1:δ֪
	int cardType=-1;//֤������

}; 