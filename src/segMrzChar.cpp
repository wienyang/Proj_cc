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

//分割
void segement(cv::Mat numImg, CardChar& cardChar)
{
	cv::Mat BWNumImg;
	int NumWidth = numImg.cols;
	int NumHeight = numImg.rows;
	//cv::imshow("numImg",numImg);
	//cv::waitKey();

	//转化为灰度图
	cv::cvtColor(numImg, BWNumImg, cv::COLOR_BGR2GRAY);//变换为灰度
	//cv::imshow("灰度图", BWNumImg);

	//灰度膨胀
	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
	cv::dilate(BWNumImg, BWNumImg, element);
	//cv::imshow("灰度膨胀", BWNumImg);

	//二值化
	cv::threshold(BWNumImg, BWNumImg, 80, 255, cv::THRESH_BINARY);//二值化
	//cv::imshow("二值化", BWNumImg);

	//开运算
	cv::Mat element1 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 1));//3 1
	cv::Mat element2 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 1));//3 1
	cv::dilate(BWNumImg, BWNumImg, element2);
	cv::erode(BWNumImg, BWNumImg, element1);
	//cv::imshow("开运算", BWNumImg);

	//水平切割，切出行
	cv::Mat lineImg = findLine(BWNumImg);
	//cv::imshow("行图", lineImg);
	//cv::waitKey();
	//cv::destroyAllWindows();

	//垂直投影,水平切分
	int startX, endX;					//字符左边界X坐标和右边界X坐标
	int width, totalNum, num, spaceWidth;	//字符宽度 字符总个数 字符个数 间隔宽度
	int i = NumWidth - 1; int k = 0; int j = 0;
	if (NumWidth > 2000)
		return;
	int sum[2000] = { 0 };

	calcProjection(BWNumImg, cv::Point(0, 0), cv::Point(NumWidth - 1, cardChar.positionY[1] - cardChar.positionY[0]), sum, 'v');
	num = 0;
	while ((i > 0) && (num < DefaultCharNum))
	{
		//字符左边界
		while (i > 0 && !(sum[i] && sum[i - 1] > 2))
		{
			--i;
		}
		startX = i;
		//字符右边界
		while (i > 0 && (sum[i]))
		{
			--i;
		}
		endX = i;
		//字符宽度
		width = startX - endX;
		if (width < MinCharWidth)//宽度小于最小字符宽度
		{
			//统计该部分字迹面积
			k = 0;
			for (j = startX; j > endX; j--)
			{
				k += sum[j];
			}
			if (k >= 40)//字迹面积足够大认为是字符
			{
				//将边界坐标存入positionX中 num计数加1
				++num;
				cardChar.positionX.emplace_back(startX);
				cardChar.positionX.emplace_back(endX);
			}
		}
		else
		{
			if (width < MaxCharWidth)//宽度小于一个字符的最大宽度
			{
				//正好是一个字符存储边界
				++num;
				cardChar.positionX.emplace_back(startX);
				cardChar.positionX.emplace_back(endX);
			}
			else if (width < MaxTwoCharWidth)//宽度小于两个字符的最大宽度
			{
				//正好是两个字符加空格的边界 分割后存储
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




//计算投影的方法
void calcProjection(cv::Mat BWImg, cv::Point start, cv::Point end, int* pjt, char dir) {
	if (end.x > BWImg.cols || end.y > BWImg.rows || start.x < 0 || start.y < 0) {
		printf("投影指定区域超出全部二值化图像范围");
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
	//输入二值化图像
	int NumWidth = BWNumImg.cols;
	int NumHeight = BWNumImg.rows;

	//水平切割，切出行
	if (NumHeight > 200) {
		cardChar.positionY.emplace_back(0);
		cardChar.positionY.emplace_back(NumHeight - 1);
		return BWNumImg;
	}

	int rowSum[200] = { 0 };
	calcProjection(BWNumImg, cv::Point(0, 0), cv::Point(NumWidth - 1, NumHeight - 1), rowSum, 'h');
	//找到机读码上下边界y坐标和高度
	int height = 0;
	int i = 0;
	int numStartY = 0, numEndY = 0;
	while (height < MinCharHeight && i < NumHeight)
	{
		//上边界
		while (i < NumHeight && !(rowSum[i] > 20 && rowSum[i + 2] > 40))
		{
			i += 2;
		}
		numStartY = (i > 2) ? (i - 2) : 0;
		//下边界
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