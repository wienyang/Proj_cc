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


//1：香港永久新
//2：香港永久旧
//3：香港非永久旧
//4：未能细分
int distinguishHKtype(Mat uv_A,Mat ir_B) {
	Mat uvFront = uv_A.clone();
	Mat irBack = ir_B.clone();

//////////////////////////////////////////////////////////////////////////////
	//判断是否为香港永久新
	cvtColor(uvFront, uvFront, COLOR_BGR2GRAY);
	threshold(uvFront, uvFront, 230, 255, THRESH_BINARY);
	//threshold(front, front, 0, 255, THRESH_BINARY | THRESH_OTSU);
#ifdef SHOW
	imshow("binFront", uvFront);
	waitKey(0);
	destroyAllWindows();
#endif
		//截取正面亮斑
	Mat idenPos1 = uvFront(Rect(410, 180, 300, 300));
#ifdef SHOW
	imshow("idenPos1", idenPos1);
	waitKey(0);
	destroyAllWindows();
#endif
	//统计像素点
	int w = idenPos1.cols;
	int h = idenPos1.rows;
	int sum = 0;
	for (int i = 0; i <= w - 1; i++) {
		uchar* data = idenPos1.ptr<uchar>(i);
		for (int j = 0; j <= h - 1; j++) {
			//cout << int(data[j])<<" ";
			sum += (data[j] == 255 ? 1 : 0);
		}
	}
	float res = sum * 1.0 / w / h;
	if (res > 0.35 && res < 0.8) //香港永久新
		return 1;

//////////////////////////////////////////////////////////////////////////////////////
	//判断是否为香港永久旧和香港非永久旧
	const int ROIHeight = 190;
	const int ROIWidth = 890;

	Mat idenPos2 = ir_B(Rect(50,270, ROIWidth, ROIHeight)).clone();//特征区域
	cvtColor(idenPos2, idenPos2, COLOR_BGR2GRAY);
	threshold(idenPos2, idenPos2, 130, 255, THRESH_BINARY);
#ifdef SHOW
	imshow("", idenPos2);
	waitKey(0);
#endif
	int j = 0;
	int rowSum[ROIHeight] = { 0 };
	int colSum[ROIWidth] = { 0 };
	int throughNum1 = 0, throughNum2 = 0; // 穿越特征符合要求的行数
	for (j = 0; j < ROIHeight; ++j)
	{
		uchar* data = idenPos2.ptr<uchar>(j);
		int throughFeatureNum = throughFeature(data, Size(ROIWidth, ROIHeight), 'h');// 计算水平方向穿越特征
		//std::cout << throughFeatureNum << ", ";
		if (throughFeatureNum > 30)//统计穿越特征大于30的行，即文字区域所在行
			++throughNum1;
		else if (throughFeatureNum < 6)////统计穿越特征小于6的行，即空白行
			++throughNum2;
	}
	//std::cout << std::endl;
	//std::cout << throughNum1 << "," << throughNum2 << std::endl;
	if (throughNum1> 40) 
		return 2;//穿越特征与香港永久旧相符合
	if (throughNum2 > 170)
		return 3;//与香港非永久旧相符合
	return 4;


}

/*
*  int IdenCardType(Mat& ir_A, Mat& ir_B, Mat& uv_A,Mat& uv_B,Mat& vi_A,Mat& vi_B)
*  @Author: WY
*  @Date: 2020-05-10
*  @Pramameter: Mat& ir_A, Mat& ir_B, Mat& uv_A,Mat& uv_B,Mat& vi_A,Mat& vi_B
*  @Return: int cardFlag (the card TYPE and DIRECTION information)
*			cardFlag = cardType*10+ cardDirection
*
*			cardType   			    cardDirection
*			0    澳门				0	正面正向
*			1    香港永久新			1	正面反向
*			2	 香港永久旧			2	反面正向
*			3    香港非永久旧		3	反面反向
*			4    香港身份证（未能细分）
*
*			-1   区分香港和澳门身份证错误
*			-2   图片大小不符合要求
*  @Description: 找出证件类型和正反，并进行纠正
*/
int IdenCardType(Mat& ir_A, Mat& ir_B, Mat& uv_A, Mat& uv_B, Mat& vi_A, Mat& vi_B)
{
	//图片大小太小，退出
	if (ir_A.rows < 600 || ir_A.cols < 900) {
		return -2;
	}
	const Size imgSize = { 996, 640 };
	//调整图片大小
	resize(ir_A, ir_A, imgSize);
	resize(ir_B, ir_B, imgSize);
	resize(uv_A, uv_A, imgSize);
	resize(uv_B, uv_B, imgSize);
	resize(vi_A, vi_A, imgSize);
	resize(vi_B, vi_B, imgSize);

	const Point ROIPos[4] = { {20, 410}, {20, 45} };//穿越特征开始点
	const int ROIWidth = 900;
	const int ROIHeight = 200;
	int rowSum[ROIHeight] = { 0 };
	int colSum[ROIWidth] = { 0 };
	int rowNum = 0, colNum = 0, maxNum = 0;
	int throughNum1[4] = { 0 }, throughNum2[4] = { 0 }; // 穿越特征符合要求的行数
	int i = 0, j = 0, label = 0;
	int cardType = -1;//证件类型
	int cardDirection = -1;//证件方向
	int cardFlag = 0;
	Mat roiImg;

	for (i = 0; i < 4; ++i) // 澳门身份证三行机读码穿越特征识别方向
	{
		//i=0，1，2，3分别取背面正向，背面反向，正面正向，正面反向进行特征检测（机读码特征在背面）
		//所以相当于分别假设是正面正向，正面反向，反面正向，反面反向
		if (i / 2 == 0)
		{
			roiImg = ir_B(Rect(ROIPos[i].x, ROIPos[i].y, ROIWidth, ROIHeight)).clone();
		}
		else
		{
			roiImg = ir_A(Rect(ROIPos[i % 2].x, ROIPos[i % 2].y, ROIWidth, ROIHeight)).clone();
		}
		cvtColor(roiImg, roiImg, COLOR_BGR2GRAY);

		threshold(roiImg, roiImg, 75, 255, THRESH_BINARY);
		for (j = 0; j < ROIHeight; ++j)
		{
			uchar* data = roiImg.ptr<uchar>(j);
			int throughFeatureNum = throughFeature(data, Size(ROIWidth, ROIHeight), 'h');// 计算水平方向穿越特征
			//std::cout << throughFeatureNum << ", ";
			if (throughFeatureNum > 60)//统计穿越特征大于60的行，即机读码所在行
				++throughNum1[i];
			else if (throughFeatureNum < 4)////统计穿越特征小于4的行，即空白行
				++throughNum2[i];
		}
	
		if (throughNum1[i] > 60 && throughNum2[i] > 80) break;//穿越特征与澳门身份证机读码特征相符
	}

	if (i != 4) //类型为澳门身份证，且方向为i
	{
		cardType = AM;//AM 0
	}
	else//类型为香港身份证，方向信息未知
	{
		Point ROIPos[4] = { {410, 20}, {405, 420} };//红色标志开始的位置
		int ROIWidth = 180;
		int ROIHeight = 200;


		cardType = HK;//HK 1
		for (i = 0; i < 4; ++i)//根据香港身份证可见光图背面红色圆形香港标志进行方向区分
		{
			if (i / 2 == 0)
			{
				roiImg = vi_B(Rect(ROIPos[i].x, ROIPos[i].y, ROIWidth, ROIHeight)).clone();
			}
			else
			{
				roiImg = vi_A(Rect(ROIPos[i % 2].x, ROIPos[i % 2].y, ROIWidth, ROIHeight)).clone();
			}
			//imshow(i + "", roiImg);
			//waitKey(0);
			cv::Mat channel[3];
			split(roiImg, channel);//将图片分割成bgr三个通道
			//计算红色的像素点数量
			int cnt = 0;
			for (int i = 0; i < ROIHeight; i++)
			{
				for (int j = 0; j < ROIWidth; j++)
				{
					if (channel[0].at<uchar>(i, j) < 20 && channel[1].at<uchar>(i, j) < 20 && channel[2].at<uchar>(i, j) > 90)
						cnt++;
				}
			}
			//std::cout<<"红色"<< cnt<< std::endl;
			if (cnt > 4000)break;
		}	
	}

	cardDirection = i;

	Mat tmp;
	switch (cardDirection)
	{
	case 0: //正面正
		break;
	case 1: //正面反向 
		//旋转180度
		flip(ir_A, ir_A, 0); 
		flip(ir_A, ir_A, 1); 

		flip(ir_B, ir_B, 0);
		flip(ir_B, ir_B, 1); 

		flip(uv_A, uv_A, 0); 
		flip(uv_A, uv_A, 1); 

		flip(uv_B, uv_B, 0); 
		flip(uv_B, uv_B, 1); 

		flip(vi_A, vi_A, 0); 
		flip(vi_A, vi_A, 1); 

		flip(vi_B, vi_B, 0);
		flip(vi_B, vi_B, 1);

		break;
	case 2: // 反面正向
		//交换ir图像正反面
		tmp = ir_A;
		ir_A = ir_B;
		ir_B = tmp;

		//交换uv图像正反面
		tmp = uv_A;
		uv_A = uv_B;
		uv_B = tmp;

		//交换vi图像正反面
		tmp = vi_A;
		vi_A = vi_B;
		vi_B = tmp;
		break;
	case 3: // 反面反向 
		//旋转180度
		flip(ir_A, ir_A, 0);
		flip(ir_A, ir_A, 1);

		flip(ir_B, ir_B, 0);
		flip(ir_B, ir_B, 1);

		flip(uv_A, uv_A, 0);
		flip(uv_A, uv_A, 1);

		flip(uv_B, uv_B, 0);
		flip(uv_B, uv_B, 1);

		flip(vi_A, vi_A, 0);
		flip(vi_A, vi_A, 1);

		flip(vi_B, vi_B, 0);
		flip(vi_B, vi_B, 1);

		//交换ir图像正反面
		tmp = ir_A;
		ir_A = ir_B;
		ir_B = tmp;

		//交换uv图像正反面
		tmp = uv_A;
		uv_A = uv_B;
		uv_B = tmp;

		//交换vi图像正反面
		tmp = vi_A;
		vi_A = vi_B;
		vi_B = tmp;

		break;
	default:
		return -1;//未能找到证件类型，
		break;
	}
	//如果是香港身份证，进行类型细分
	if (cardType == 1)
		cardType = distinguishHKtype(uv_A, ir_B);
	//cardflag十位表示类型信息，个位表示方向信息
	cardFlag = cardType * 10 + cardDirection;
	return cardFlag;
}

