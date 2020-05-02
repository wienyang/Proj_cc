#include "findMRZ.h"
using namespace std;
using namespace cv;
//#define SHOW

//�Զ���������  2020.03.25@wenyang
inline bool sortFun1(const cv::Rect& p1, const cv::Rect& p2)
{
	return p1.y < p2.y;//��������  
}

//�Զ���������  2020.03.25@wenyang
inline bool sortFun2(const cv::Rect& p1, const cv::Rect& p2)
{
	return p1.y > p2.y;//��������  
}

int removeOutlierContours(Mat& img)
{
	//-----------------------------------------------------------------------
	//C++�ӿڵ�findContours�ڱ߽紦�ƺ�����bug����������Ӧ�öԱ߽����Ԥ����
	//�ĸ��߽�ȫ����Ϊ0
	for (int i = 0; i < img.rows; ++i)
	{
		img.at<uchar>(i, 0) = 0;
		img.at<uchar>(i, img.cols - 1) = 0;
	}
	for (int j = 0; j < img.cols; ++j)
	{
		img.at<uchar>(0, j) = 0;
		img.at<uchar>(img.rows - 1, j) = 0;
	}

	Mat imgCopy = img.clone();
#ifdef SHOW
	namedWindow("rc0", 0);
	imshow("rc0", imgCopy);
	waitKey(0);
#endif // SHOW
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(imgCopy, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
	for (vector<Point> contour : contours)
	{
		cv::Rect contourRect = boundingRect(contour);
		imgCopy(contourRect) = 255;
	}

#ifdef SHOW
	namedWindow("rc1", 0);
	imshow("rc1", imgCopy);
	waitKey(0);
#endif // SHOW

	findContours(imgCopy, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
	vector<Rect> rects;
	vector<int> heights;
	for (vector<Point> contour : contours)
	{
		Rect contourRect = boundingRect(contour);
		rects.push_back(contourRect);
		heights.push_back(contourRect.height);
	}
	sort(heights.begin(), heights.end());
	double refHeight = (double)heights[heights.size() / 2];

	for (Rect rect : rects)
	{
		double ratio = (double)rect.height / refHeight;

		if (ratio > 1.3 || ratio < 0.7)
			img(rect) = 0;
	}
#ifdef SHOW
	namedWindow("rc2", 0);
	imshow("rc2", img);
	waitKey(0);
#endif // SHOW
	return 0;
}

//����-1���޽��
//����0��һ�л�����֤��
//����1�����л�����֤��
int findMRZ(Mat image,vector<Mat>& result) {
	//��ͼƬת��Ϊ�Ҷ�ͼ
	if (image.channels() != 1)
		cvtColor(image, image, COLOR_BGR2GRAY);

	const Size imgSize = { 960, 600 };
	resize(image, image, imgSize);

	int h = image.rows;
	int w = image.cols;

	int kernel_x = 35;//���������м�Ŀ��
	Mat horizontalShort = getStructuringElement(MORPH_RECT, Size(kernel_x, 1));
	Mat verticalKernel = getStructuringElement(MORPH_RECT, Size(1, 5));
	Mat blackhat;
	morphologyEx(image, blackhat, MORPH_BLACKHAT, horizontalShort);
#ifdef SHOW
	namedWindow("blackhat", 0);
	imshow("blackhat", blackhat);
	waitKey(0);
	destroyAllWindows();
#endif
	Mat imgBin = blackhat.clone();
	threshold(imgBin, imgBin, 0, 255, THRESH_BINARY | THRESH_OTSU);
	//imgBin = imgBin == 0;
	imgBin(Rect(0, 0, 30, image.rows)) = 0;
#ifdef SHOW
	namedWindow("imgBin", 0);
	imshow("imgBin", imgBin);
	waitKey(0);
	destroyAllWindows();
#endif
	Mat gradX;
	Sobel(blackhat, gradX, CV_32F, 1, 0, 3);
	convertScaleAbs(gradX, gradX);
	morphologyEx(gradX, gradX, MORPH_CLOSE, horizontalShort);
#ifdef SHOW
	namedWindow("gradX", 0);
	imshow("gradX", gradX);
	waitKey(0);
	destroyAllWindows();
#endif
	//ˮƽ�������㣬��ֱ���������
	threshold(gradX, gradX, 0, 255, THRESH_BINARY | THRESH_OTSU);
	morphologyEx(gradX, gradX, MORPH_OPEN, verticalKernel);
	for (int i = 0; i < gradX.rows; ++i)
	{
		gradX.at<uchar>(i, 0) = 0;
		gradX.at<uchar>(i, gradX.cols - 1) = 0;
	}
	gradX(Rect(0, 0, 30, image.rows)) = 0;
	auto tmpKernal = getStructuringElement(MORPH_RECT, Size(15, 1));
	morphologyEx(gradX, gradX, MORPH_OPEN, tmpKernal);
#ifdef SHOW
	namedWindow("gradX", 0);
	imshow("gradX", gradX);
	waitKey(0);
	destroyAllWindows();
#endif

	vector<Mat> contours;
	findContours(gradX, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	vector<Rect> rects;
	//vector<Mat> lineImgs;
	//vector<Mat>lineBinImgs;
	//�ҳ��е�����
	int height = 0;//������߶�

	for (Mat contour : contours) {
		Rect rect = boundingRect(contour);
		//cout << rect.width << endl;
		if (rect.width < 770)continue;//����770��Ϊ������

		height = height + rect.y;

		rect.y -= 5;
		rect.height += 10;
		rect.y = max(0, rect.y);
		rect.height = min(image.rows - 1 - rect.y, rect.height);

		rect.x -= 20;
		rect.width += 40;
		rect.x = max(0, rect.x);
		rect.width = min(image.cols - 1 - rect.x, rect.width);
		rects.push_back(rect);
		//imshow("", image(rect));
		//waitKey(0);
		//destroyAllWindows();
	}
	if ((rects.size()!=1)&&(rects.size()!= 3))return -1;
	if (height /rects.size() < h / 2) {//���������Ϸ�
		sort(rects.begin(), rects.end(), sortFun2);//���߶Ƚ�������
		for (Rect rect : rects) {
			cv::Mat rectImg;
			flip(image(rect), rectImg,-1);//ˮƽ��ת
			result.push_back(rectImg);
		}
	}else {//���������·�
		sort(rects.begin(), rects.end(), sortFun1);//���߶���������
		for (Rect rect : rects) {
			result.push_back(image(rect));
		}
	}



	return rects.size() == 1 ? 0 : 1;



}



