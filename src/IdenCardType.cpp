#include "IdenCardType.h"

using namespace cv;

const int AM = 0;
const int HK = 1;

/*
*  int throughFeature(Byte* data, Size DataSize, char dir)
*  @Author: LiChengchen
*  @Date: 2017-03-14
*  @Pramameter: Byte* data: Source image address;
*  				Size DataSize: Size of input image
*               char dir: the direction of projection, 'h' for horizontal, 'v' for vertical;
*  @Return: the throughFeature of one row or column of input BWImg
*  @Description: calculate the ThroughFeature of source image
*  @Notation: input Img must be BWImg, if the first or end of the row or column is black throughnumber++
*/
int throughFeature(uchar* data, Size DataSize, char dir) {
	int ThroughNum = 0;
	int i;
	uchar* p = data;

	switch (dir)
	{
	case 'h':
		for (i = 0; i < DataSize.width - 1; ++i)
		{
			if ((*(p + i) + *(p + i + 1)) == 255)
			{
				++ThroughNum;
			}
		}
		ThroughNum = ThroughNum + (*p == 0) + (*(p + DataSize.width) == 0);
		break;
	case 'v':
		for (i = 0; i < DataSize.height - 1; ++i)
		{
			if ((*(p + DataSize.width * i) + *(p + DataSize.width * (i + 1))) == 255)
			{
				++ThroughNum;
			}
		}
		ThroughNum = ThroughNum + (*p == 0) + (*(p + DataSize.width * (DataSize.height - 1)) == 0);
		break;
	default:
		break;
	}
	return ThroughNum;
}

//����ֵ˵��
//0    ���ţ���������
//1    ���ţ����淴��
//2	   ���ţ���������
//3    ���ţ����淴��
//10   ��ۣ���������
//11   ��ۣ����淴��
//12   ��ۣ���������
//13   ��ۣ����淴��

//-1��δ֪֤�����ͣ�
//-2��ͼƬ��С������Ҫ��
int IdenCardType(Mat& srcImg, Mat frontImg, Mat backImg,Mat colorFront,Mat colorBack)
{
	//std::cout << frontImg.rows << " " << frontImg.cols << std::endl;
	if (frontImg.rows < 600 || frontImg.cols < 900) {
		return -2;
	}
	const Size imgSize = { 996, 640 };
	resize(frontImg, frontImg, imgSize);
	//imshow("", frontImg);
	//waitKey(0);

	const Point ROIPos[4] = { {20, 410}, {20, 45} };
	const int ROIWidth = 900;
	const int ROIHeight = 200;
	int rowSum[ROIHeight] = { 0 };
	int colSum[ROIWidth] = { 0 };
	int rowNum = 0, colNum = 0, maxNum = 0;
	int throughNum1[4] = { 0 }, throughNum2[4] = { 0 }; // ��Խ��������Ҫ�������
	int i = 0, j = 0, label = 0;
	int cardType = -1;//֤������
	int cardDirection = -1;//֤������
	int cardFlag = 0;
	Mat roiImg;

	for (i = 0; i < 4; ++i) // �۰�������½��̨��������½�������л����봩Խ����ʶ����
	{
		if (i / 2 == 0)
		{
			roiImg = backImg(Rect(ROIPos[i].x, ROIPos[i].y, ROIWidth, ROIHeight)).clone();
		}
		else
		{
			roiImg = frontImg(Rect(ROIPos[i % 2].x, ROIPos[i % 2].y, ROIWidth, ROIHeight)).clone();
		}
		cvtColor(roiImg, roiImg, COLOR_BGR2GRAY);
		//imshow(i+"", roiImg);
		//waitKey(0);
		threshold(roiImg, roiImg, 75, 255, THRESH_BINARY);
		for (j = 0; j < ROIHeight; ++j)
		{
			uchar* data = roiImg.ptr<uchar>(j);
			int throughFeatureNum = throughFeature(data, Size(ROIWidth, ROIHeight), 'h');// ����ˮƽ����Խ����
			//std::cout << throughFeatureNum << ", ";
			if (throughFeatureNum > 60)
				++throughNum1[i];
			else if (throughFeatureNum < 4)//4
				++throughNum2[i];
		}
	
		if (throughNum1[i] > 60 && throughNum2[i] > 80) break;
	}

	if (i != 4) // �ҵ�����
	{
		cardType = AM;//AM 0
	}
	else
	{
		Point ROIPos[4] = { {410, 20}, {405, 420} };
		int ROIWidth = 180;
		int ROIHeight = 200;


		cardType = HK;//HK 1
		for (i = 0; i < 4; ++i)
		{
			if (i / 2 == 0)
			{
				roiImg = colorBack(Rect(ROIPos[i].x, ROIPos[i].y, ROIWidth, ROIHeight)).clone();
			}
			else
			{
				roiImg = colorFront(Rect(ROIPos[i % 2].x, ROIPos[i % 2].y, ROIWidth, ROIHeight)).clone();
			}
			//imshow(i + "", roiImg);
			//waitKey(0);
			cv::Mat channel[3];
			split(roiImg, channel);

			int cnt = 0;
			for (int i = 0; i < ROIHeight; i++)
			{
				for (int j = 0; j < ROIWidth; j++)
				{
					if (channel[0].at<uchar>(i, j) < 20 && channel[1].at<uchar>(i, j) < 20 && channel[2].at<uchar>(i, j) > 90)
						cnt++;
				}
			}
			//std::cout<<"��ɫ"<< cnt<< std::endl;
			if (cnt > 4000)break;
		}	
	}

	cardDirection = i;

	switch (cardDirection)
	{
	case 0: //��������
		srcImg = frontImg; // srcImg��������
		break;
	case 1: //���淴�� ��ת180��
		flip(frontImg, srcImg, 0); // ��X�ᷭת
		flip(srcImg, srcImg, 1); // ��Y�ᷭת
		break;
	case 2: // ��������
		srcImg = backImg;
		break;
	case 3: // ���淴�� ��ת180��
		flip(backImg, srcImg, 0); // ��X�ᷭת
		flip(srcImg, srcImg, 1); // ��Y�ᷭת
		break;
	default:
		return -1;//δ���ҵ�֤�����ͣ�
		break;
	}
	cardFlag = cardType * 10 + cardDirection;
	return cardFlag;
}

