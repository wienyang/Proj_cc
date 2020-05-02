#include"segMrzChar.h"


const int DefaultCharNum = 31;
//const int NumWidth = 900;
const int MaxCharWidth = 30;
const int MaxTwoCharWidth = 60;
const int MinCharWidth = 6;
const int AvgCharWidth = 19;

const int MaxCharHeight = 40;
const int MinCharHeight = 20;

const int MaxSpaceWidth = 16;
const int MinSpaceWidth = 2;

//�ָ�
void segement(cv::Mat numImg, CardChar& cardChar)
{
	cv::Mat BWNumImg;
	int NumWidth = numImg.cols;
	int NumHeight = numImg.rows;
	//cv::imshow("numImg",numImg);
	//cv::waitKey();

	//ת��Ϊ�Ҷ�ͼ
	cv::cvtColor(numImg, BWNumImg, cv::COLOR_BGR2GRAY);//�任Ϊ�Ҷ�
	//cv::imshow("�Ҷ�ͼ", BWNumImg);

	//�Ҷ�����
	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
	cv::dilate(BWNumImg, BWNumImg, element);
	//cv::imshow("�Ҷ�����", BWNumImg);

	//��ֵ��
	cv::threshold(BWNumImg, BWNumImg, 80, 255, cv::THRESH_BINARY);//��ֵ��
	//cv::imshow("��ֵ��", BWNumImg);

	//������
	cv::Mat element1 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 1));//3 1
	cv::Mat element2 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 1));//3 1
	cv::dilate(BWNumImg, BWNumImg, element2);
	cv::erode(BWNumImg, BWNumImg, element1);
	//cv::imshow("������", BWNumImg);

	//ˮƽ�и�г���
	cv::Mat lineImg = findLine(BWNumImg);
	//cv::imshow("��ͼ", lineImg);
	//cv::waitKey();
	//cv::destroyAllWindows();

	//��ֱͶӰ,ˮƽ�з�
	int startX, endX;					//�ַ���߽�X������ұ߽�X����
	int width, totalNum, num, spaceWidth;	//�ַ���� �ַ��ܸ��� �ַ����� ������
	int i = NumWidth - 1; int k = 0; int j = 0;
	if (NumWidth > 2000)
		return;
	int sum[2000] = { 0 };

	calcProjection(BWNumImg, cv::Point(0, 0), cv::Point(NumWidth - 1, cardChar.positionY[1] - cardChar.positionY[0]), sum, 'v');
	num = 0;
	while ((i > 0) && (num < DefaultCharNum))
	{
		//�ַ���߽�
		while (i > 0 && !(sum[i] && sum[i - 1] > 2))
		{
			--i;
		}
		startX = i;
		//�ַ��ұ߽�
		while (i > 0 && (sum[i]))
		{
			--i;
		}
		endX = i;
		//�ַ����
		width = startX - endX;
		if (width < MinCharWidth)//���С����С�ַ����
		{
			//ͳ�Ƹò����ּ����
			k = 0;
			for (j = startX; j > endX; j--)
			{
				k += sum[j];
			}
			if (k >= 40)//�ּ�����㹻����Ϊ���ַ�
			{
				//���߽��������positionX�� num������1
				++num;
				cardChar.positionX.emplace_back(startX);
				cardChar.positionX.emplace_back(endX);
			}
		}
		else
		{
			if (width < MaxCharWidth)//���С��һ���ַ��������
			{
				//������һ���ַ��洢�߽�
				++num;
				cardChar.positionX.emplace_back(startX);
				cardChar.positionX.emplace_back(endX);
			}
			else if (width < MaxTwoCharWidth)//���С�������ַ��������
			{
				//�����������ַ��ӿո�ı߽� �ָ��洢
				width = (width - MinSpaceWidth) >> 1;
				num += 2;
				cardChar.positionX.emplace_back(startX);
				cardChar.positionX.emplace_back(startX - width);
				cardChar.positionX.emplace_back(endX + width);
				cardChar.positionX.emplace_back(endX);
			}
		}
	}
}




//����ͶӰ�ķ���
void calcProjection(cv::Mat BWImg, cv::Point start, cv::Point end, int* pjt, char dir) {
	if (end.x > BWImg.cols || end.y > BWImg.rows || start.x < 0 || start.y < 0) {
		printf("ͶӰָ�����򳬳�ȫ����ֵ��ͼ��Χ");
		return;
	}
	int i, j;

	switch (dir) {
	case 'v':
		for (i = start.y; i <= end.y; i++) {
			uchar* data = BWImg.ptr<uchar>(i);
			for (j = start.x; j <= end.x; j++) {
				//cout << data[j];
				pjt[j] += (data[j] == 0 ? 1 : 0);
			}
		}
		break;
	case 'h':
		for (i = start.y; i <= end.y; i++) {
			uchar* data = BWImg.ptr<uchar>(i);
			for (j = start.x; j <= end.x; j++) {
				pjt[i] += (data[j] == 0 ? 1 : 0);
			}
		}
		break;
	default:
		break;
	}
}

cv::Mat findLine(cv::Mat BWNumImg) {
	//�����ֵ��ͼ��
	int NumWidth = BWNumImg.cols;
	int NumHeight = BWNumImg.rows;

	//ˮƽ�и�г���
	if (NumHeight > 200) {
		cardChar.positionY.emplace_back(0);
		cardChar.positionY.emplace_back(NumHeight - 1);
		return BWNumImg;
	}

	int rowSum[200] = { 0 };
	calcProjection(BWNumImg, cv::Point(0, 0), cv::Point(NumWidth - 1, NumHeight - 1), rowSum, 'h');
	//�ҵ����������±߽�y����͸߶�
	int height = 0;
	int i = 0;
	int numStartY = 0, numEndY = 0;
	while (height < MinCharHeight && i < NumHeight)
	{
		//�ϱ߽�
		while (i < NumHeight && !(rowSum[i] > 20 && rowSum[i + 2] > 40))
		{
			i += 2;
		}
		numStartY = (i > 2) ? (i - 2) : 0;
		//�±߽�
		while (i < NumHeight && (rowSum[i] || rowSum[i + 2]))
		{
			i += 2;
		}
		numStartY = (numStartY - 2 > 0) ? numStartY - 2 : 0;
		numEndY = (i + 2 < NumHeight - 1) ? (i + 2) : NumHeight - 1;
		height = numEndY - numStartY;
	}
	//cout << " " << numEndY << " " << numStartY;
	cv::Rect rect = { 0,numStartY,NumWidth,height };
	cv::Mat lineImg = BWNumImg(rect);
	return lineImg;
}