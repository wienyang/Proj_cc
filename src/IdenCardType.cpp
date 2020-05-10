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


//1�����������
//2��������þ�
//3����۷����þ�
//4��δ��ϸ��
int distinguishHKtype(Mat uv_A,Mat ir_B) {
	Mat uvFront = uv_A.clone();
	Mat irBack = ir_B.clone();

//////////////////////////////////////////////////////////////////////////////
	//�ж��Ƿ�Ϊ���������
	cvtColor(uvFront, uvFront, COLOR_BGR2GRAY);
	threshold(uvFront, uvFront, 230, 255, THRESH_BINARY);
	//threshold(front, front, 0, 255, THRESH_BINARY | THRESH_OTSU);
#ifdef SHOW
	imshow("binFront", uvFront);
	waitKey(0);
	destroyAllWindows();
#endif
		//��ȡ��������
	Mat idenPos1 = uvFront(Rect(410, 180, 300, 300));
#ifdef SHOW
	imshow("idenPos1", idenPos1);
	waitKey(0);
	destroyAllWindows();
#endif
	//ͳ�����ص�
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
	if (res > 0.35 && res < 0.8) //���������
		return 1;

//////////////////////////////////////////////////////////////////////////////////////
	//�ж��Ƿ�Ϊ������þɺ���۷����þ�
	const int ROIHeight = 190;
	const int ROIWidth = 890;

	Mat idenPos2 = ir_B(Rect(50,270, ROIWidth, ROIHeight)).clone();//��������
	cvtColor(idenPos2, idenPos2, COLOR_BGR2GRAY);
	threshold(idenPos2, idenPos2, 130, 255, THRESH_BINARY);
#ifdef SHOW
	imshow("", idenPos2);
	waitKey(0);
#endif
	int j = 0;
	int rowSum[ROIHeight] = { 0 };
	int colSum[ROIWidth] = { 0 };
	int throughNum1 = 0, throughNum2 = 0; // ��Խ��������Ҫ�������
	for (j = 0; j < ROIHeight; ++j)
	{
		uchar* data = idenPos2.ptr<uchar>(j);
		int throughFeatureNum = throughFeature(data, Size(ROIWidth, ROIHeight), 'h');// ����ˮƽ����Խ����
		//std::cout << throughFeatureNum << ", ";
		if (throughFeatureNum > 30)//ͳ�ƴ�Խ��������30���У�����������������
			++throughNum1;
		else if (throughFeatureNum < 6)////ͳ�ƴ�Խ����С��6���У����հ���
			++throughNum2;
	}
	//std::cout << std::endl;
	//std::cout << throughNum1 << "," << throughNum2 << std::endl;
	if (throughNum1> 40) 
		return 2;//��Խ������������þ������
	if (throughNum2 > 170)
		return 3;//����۷����þ������
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
*			0    ����				0	��������
*			1    ���������			1	���淴��
*			2	 ������þ�			2	��������
*			3    ��۷����þ�		3	���淴��
*			4    ������֤��δ��ϸ�֣�
*
*			-1   ������ۺͰ������֤����
*			-2   ͼƬ��С������Ҫ��
*  @Description: �ҳ�֤�����ͺ������������о���
*/
int IdenCardType(Mat& ir_A, Mat& ir_B, Mat& uv_A, Mat& uv_B, Mat& vi_A, Mat& vi_B)
{
	//ͼƬ��С̫С���˳�
	if (ir_A.rows < 600 || ir_A.cols < 900) {
		return -2;
	}
	const Size imgSize = { 996, 640 };
	//����ͼƬ��С
	resize(ir_A, ir_A, imgSize);
	resize(ir_B, ir_B, imgSize);
	resize(uv_A, uv_A, imgSize);
	resize(uv_B, uv_B, imgSize);
	resize(vi_A, vi_A, imgSize);
	resize(vi_B, vi_B, imgSize);

	const Point ROIPos[4] = { {20, 410}, {20, 45} };//��Խ������ʼ��
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

	for (i = 0; i < 4; ++i) // �������֤���л����봩Խ����ʶ����
	{
		//i=0��1��2��3�ֱ�ȡ�������򣬱��淴�������������淴�����������⣨�����������ڱ��棩
		//�����൱�ڷֱ�����������������淴�򣬷������򣬷��淴��
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
			int throughFeatureNum = throughFeature(data, Size(ROIWidth, ROIHeight), 'h');// ����ˮƽ����Խ����
			//std::cout << throughFeatureNum << ", ";
			if (throughFeatureNum > 60)//ͳ�ƴ�Խ��������60���У���������������
				++throughNum1[i];
			else if (throughFeatureNum < 4)////ͳ�ƴ�Խ����С��4���У����հ���
				++throughNum2[i];
		}
	
		if (throughNum1[i] > 60 && throughNum2[i] > 80) break;//��Խ������������֤�������������
	}

	if (i != 4) //����Ϊ�������֤���ҷ���Ϊi
	{
		cardType = AM;//AM 0
	}
	else//����Ϊ������֤��������Ϣδ֪
	{
		Point ROIPos[4] = { {410, 20}, {405, 420} };//��ɫ��־��ʼ��λ��
		int ROIWidth = 180;
		int ROIHeight = 200;


		cardType = HK;//HK 1
		for (i = 0; i < 4; ++i)//����������֤�ɼ���ͼ�����ɫԲ����۱�־���з�������
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
			split(roiImg, channel);//��ͼƬ�ָ��bgr����ͨ��
			//�����ɫ�����ص�����
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

	Mat tmp;
	switch (cardDirection)
	{
	case 0: //������
		break;
	case 1: //���淴�� 
		//��ת180��
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
	case 2: // ��������
		//����irͼ��������
		tmp = ir_A;
		ir_A = ir_B;
		ir_B = tmp;

		//����uvͼ��������
		tmp = uv_A;
		uv_A = uv_B;
		uv_B = tmp;

		//����viͼ��������
		tmp = vi_A;
		vi_A = vi_B;
		vi_B = tmp;
		break;
	case 3: // ���淴�� 
		//��ת180��
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

		//����irͼ��������
		tmp = ir_A;
		ir_A = ir_B;
		ir_B = tmp;

		//����uvͼ��������
		tmp = uv_A;
		uv_A = uv_B;
		uv_B = tmp;

		//����viͼ��������
		tmp = vi_A;
		vi_A = vi_B;
		vi_B = tmp;

		break;
	default:
		return -1;//δ���ҵ�֤�����ͣ�
		break;
	}
	//�����������֤����������ϸ��
	if (cardType == 1)
		cardType = distinguishHKtype(uv_A, ir_B);
	//cardflagʮλ��ʾ������Ϣ����λ��ʾ������Ϣ
	cardFlag = cardType * 10 + cardDirection;
	return cardFlag;
}

