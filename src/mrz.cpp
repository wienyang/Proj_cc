#include "mrz.h"

using namespace std;
using namespace cv;
using namespace dnn;
//#define SHOW

bool CompareRectByX(cv::Rect rect1, cv::Rect rect2)
{
	return rect1.x < rect2.x;
}


MRZ::~MRZ()
{
}


bool MRZ::initNet(std::string modelPath)
{
	m_net = readNet(modelPath);
	return !m_net.empty();
}

int MRZ::segment(vector<Mat>lineImgs) {
	mrzNum = lineImgs.size();//机读码行数
	vector<Mat> lineBinImgs;//二值化的机读码图像
	m_charImgMat.clear();//清空之前的字符记录
	for (Mat lineImg : lineImgs)
	{
		m_charImgMat.push_back(vector<Mat>());
//		Mat blackhat;
//		morphologyEx(lineImg, blackhat, MORPH_BLACKHAT, 35);
//#ifdef SHOW
//		namedWindow("blackhat", 0);
//		imshow("blackhat", blackhat);
//		waitKey(0);
//		destroyAllWindows();
//#endif
		Mat imgBin = lineImg.clone();
		threshold(imgBin, imgBin, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);

		auto tmpKernel1 = getStructuringElement(MORPH_RECT, Size(1, 5));
		auto tmpKernel2 = getStructuringElement(MORPH_RECT, Size(2, 1));

		morphologyEx(imgBin, imgBin, MORPH_OPEN, tmpKernel2);
		morphologyEx(imgBin, imgBin, MORPH_CLOSE, tmpKernel1);

		lineBinImgs.push_back(imgBin);
#ifdef SHOW
		namedWindow("line", 1);
		namedWindow("lineBin", 1);
		imshow("line", lineImg);
		imshow("lineBin", imgBin);
		waitKey(0);
		destroyAllWindows();
#endif // SHOW
	}

	vector<Mat> contours;
	for (int i = 0; i < lineImgs.size(); ++i)
	{
		vector<Rect>rects;//储存分割出的文字框
		findContours(lineBinImgs[i], contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		for (Mat contour : contours)
		{
			Rect rect = boundingRect(contour);
			if (rect.height > lineImgs[i].rows * 0.35)
			{
				int toBePad = rect.height - rect.width;
				if (toBePad > 0)
				{
					int left = toBePad / 2;
					rect.x -= left;
					rect.width += toBePad;
					rect.x = max(0, rect.x);
					rect.width = min(rect.width, lineBinImgs[i].cols - 1 - rect.x);
				}
				rects.push_back(rect);
			}
		}
		sort(rects.begin(), rects.end(), CompareRectByX);
#ifdef SHOW
		Mat tmp = lineImgs[i].clone();
		for (Rect rect : rects)
			rectangle(tmp, rect, 255);
		namedWindow("tmp", 0);
		imshow("tmp", tmp);
		waitKey(0);
		destroyAllWindows();
#endif
		for (Rect rect : rects)
		{
			m_charImgMat[i].push_back(lineImgs[i](rect));
#ifdef SHOW
			namedWindow("char", 0);
			imshow("char", lineImgs[i](rect));
			waitKey(0);
			destroyAllWindows();
#endif // SHOW
		}

	}
}

void MRZ::recognize()
{
	m_mrzStr = "";//清空之前的结果
	for (vector<Mat> vec : m_charImgMat)
	{
		for (Mat charImg : vec)
		{
			int index = 0;
			Mat input;
			resize(charImg, input, Size(32, 32));
			Mat blob;
			blobFromImage(input, blob, 1.0, Size(32, 32));
			m_net.setInput(blob);
			Mat logit = m_net.forward();
			float max = logit.at<float>(0, 0);
			for (int i = 1; i < 37; ++i)
			{
				if (logit.at<float>(0, i) > max)
				{
					max = logit.at<float>(0, i);
					index = i;
				}
			}
			m_mrzStr.push_back(charTable[index]);
		}
		m_mrzStr.push_back('\n');
	}
}

std::string MRZ::mrzStr()
{
	return m_mrzStr;
}

int MRZ::getCardType(){
	return cardType;
}

void MRZ::alpha2num(char& c)
{
	if (c == 'O')
		c = '0';
	if (c == 'A')
		c = '4';
	if (c == 'Z')
		c = '2';
}

void MRZ::num2alpha(char& c)
{
	if (c == '0')
		c = 'O';
	if (c == '4')
		c = 'A';
	if (c == '2')
		c = 'Z';
}

void MRZ::checkMrzStr(){
	cardType = -1;
	if (mrzNum == 1)//一行机读码
	{
		if (m_mrzStr.size() != 31)
		{
			//LOG(WARNING) << "type1 mrz size not equal to 31";
			return;
		}
		for (int i = 4; i < m_mrzStr.size(); ++i)
		{
			alpha2num(m_mrzStr[i]);
		}

		//找出证件类型
		if (m_mrzStr.substr(0, 3) == "CSC")cardType = 0;
		else if(m_mrzStr.substr(0, 3) == "CDL")cardType = 1;

	}
	else if (mrzNum == 3)
	{
		if (m_mrzStr.size() != 93)//三行机读码
		{
			//LOG(WARNING) << "type2 mrz size not equal to 93";
			return;
		}
		string line1Str = m_mrzStr.substr(0, 31);
		string line2Str = m_mrzStr.substr(31, 31);
		string line3Str = m_mrzStr.substr(62, 31);
		//*********第一行********
		for (int i = 0; i < 2; ++i)
			num2alpha(line1Str[i]);
		for (int i = 3; i < 22; ++i)
			alpha2num(line1Str[i]);
		num2alpha(line1Str[22]);
		for (int i = 23; i < 30; ++i)
			alpha2num(line1Str[i]);
		//*********第二行********
		for (int i = 0; i < 20; ++i)
			num2alpha(line2Str[i]);
		for (int i = 20; i < 30; ++i)
			alpha2num(line2Str[i]);
		//*********第三行********
		for (char c : line3Str)
			num2alpha(c);
		//*********合并**********
		m_mrzStr.clear();
		m_mrzStr += line1Str;
		m_mrzStr += line2Str;
		m_mrzStr += line3Str;

		//找出证件类型
		if (m_mrzStr.substr(0, 3) == "CRH")cardType = 2;
		else if (m_mrzStr.substr(0, 2) == "CT")cardType = 3;
	}
}
