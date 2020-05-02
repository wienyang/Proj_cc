#include "findMRZ.h"
using namespace std;
using namespace cv;
//#define SHOW

//自定义排序函数  2020.03.25@wenyang
inline bool sortFun1(const cv::Rect& p1, const cv::Rect& p2)
{
	return p1.y < p2.y;//升序排列  
}

//自定义排序函数  2020.03.25@wenyang
inline bool sortFun2(const cv::Rect& p1, const cv::Rect& p2)
{
	return p1.y > p2.y;//降序排列  
}

int removeOutlierContours(Mat& img)
{
	//-----------------------------------------------------------------------
	//C++接口的findContours在边界处似乎存在bug，所以首先应该对边界进行预处理
	//四个边界全部置为0
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

//返回-1：无结果
//返回0：一行机读码证件
//返回1：三行机读码证件
int findMRZ(Mat image,vector<Mat>& result) {
	//将图片转化为灰度图
	if (image.channels() != 1)
		cvtColor(image, image, COLOR_BGR2GRAY);

	const Size imgSize = { 960, 600 };
	resize(image, image, imgSize);

	int h = image.rows;
	int w = image.cols;

	int kernel_x = 35;//两列文字中间的宽度
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
	//水平方向开运算，垂直方向闭运算
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
	//找出行的轮廓
	int height = 0;//机读码高度

	for (Mat contour : contours) {
		Rect rect = boundingRect(contour);
		//cout << rect.width << endl;
		if (rect.width < 770)continue;//大于770的为机读码

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
	if (height /rects.size() < h / 2) {//机读码在上方
		sort(rects.begin(), rects.end(), sortFun2);//按高度降序排序
		for (Rect rect : rects) {
			cv::Mat rectImg;
			flip(image(rect), rectImg,-1);//水平翻转
			result.push_back(rectImg);
		}
	}else {//机读码在下方
		sort(rects.begin(), rects.end(), sortFun1);//按高度升序排序
		for (Rect rect : rects) {
			result.push_back(image(rect));
		}
	}



	return rects.size() == 1 ? 0 : 1;



}



