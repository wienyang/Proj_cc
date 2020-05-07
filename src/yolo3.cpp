#include "yolo3.h";
using namespace std;

int GAPx = 150;//200
int GAPy = 10;


vector<string> classes;
vector<cv::String> getOutputsNames(Net& net)
{
	static vector<cv::String> names;
	if (names.empty())
	{
		//返回加载模型中所有层的输入和输出形状(shape)
		vector<int> outLayers = net.getUnconnectedOutLayers();

		//get the names of all the layers in the network
		vector<cv::String> layersNames = net.getLayerNames();

		// Get the names of the output layers in names
		names.resize(outLayers.size());
		for (size_t i = 0; i < outLayers.size(); ++i)
			names[i] = layersNames[outLayers[i] - 1];
	}
	return names;
}

void drawPred(int classId, float conf, int left, int top, int right, int bottom, cv::Mat& frame)
{
	//Draw a rectangle displaying the bounding box
	cv::rectangle(frame, cv::Point(left, top), cv::Point(right, bottom), cv::Scalar(0, 0, 0), 1);

	//Get the label for the class name and its confidence
	//string label = format("%.5f", conf);
	//if (!classes.empty())
	//{
	//	label = "text:" + label;
	//}

	//Display the label at the top of the bounding box
	//int baseLine;
	//Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
	//top = max(top, labelSize.height);
	//rectangle(frame, Point(left, top - round(1.5 * labelSize.height)), Point(left + round(1.5 * labelSize.width), top + baseLine), Scalar(255, 255, 255), FILLED);
	//putText(frame, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 0), 1);
}

bool isnear(const cv::Rect& box1, const cv::Rect& box2)
{
	int centx1 = box1.x + box1.width / 2;
	int centy1 = box1.y + box1.height / 2;
	int centx2 = box2.x + box2.width / 2;
	int centy2 = box2.y + box2.height / 2;

	//cout << "abs(centx1 - centx2)" << abs(centx1 - centx2) <<"	";

	//cout << "abs(centy1 - centy2)" << abs(centy1 - centy2) << endl;
	if ((abs(centx1 - centx2) < GAPx) && (abs(centy1 - centy2) < GAPy))
	{
		return true;
	}
	else
	{
		return false;
	}

}

bool isfinished(const int* ar, const int len, int* p)
{
	for (int i = 0; i < len; i++)
	{
		if (*(ar + i) == 0)
		{
			*p = i;
			return false;
		}

	}
	return true;
}

bool LessSort(cv::Rect a, cv::Rect b)
{
	return (a.x < b.x);
}

vector<cv::Rect> fuseBox(vector<cv::Rect>& boxes)
{
	//2019.09.28@wenyang
	vector<vector<int>> lines_up_down;
	vector<int> line_up_down;
	int up = 0;
	int down = 0;

	sort(boxes.begin(), boxes.end(), LessSort);
	vector<cv::Rect> singlebox;
	vector<	vector<cv::Rect>> finalboxes;
	int* num = new int[boxes.size()]();

	int* p = new int[1]();
	while (!isfinished(num, boxes.size(), p))
	{
		*(num + (*p)) = 1;
		singlebox.push_back(boxes[*p]);
		//2019.09.29@wenyang
		up = boxes[*p].y;
		down = boxes[*p].y + boxes[*p].height;

		for (int i = 0; i < boxes.size(); i++)
		{
			if ((*p) == i)continue;

			if (*(num + i))continue;
			if (isnear(singlebox[singlebox.size() - 1], boxes[i]))
			{
				//2019.09.29@wenyang
				up = min(up, boxes[i].y);
				down = max(down, boxes[i].y + boxes[i].height);

				singlebox.push_back(boxes[i]);
				sort(singlebox.begin(), singlebox.end(), LessSort);
				*(num + i) = 1;
			}

		}
		//2019.09.29@wenyang
		line_up_down.push_back(up);
		line_up_down.push_back(down);
		lines_up_down.push_back(line_up_down);
		line_up_down.clear();
		singlebox.shrink_to_fit();

		finalboxes.push_back(singlebox);
		singlebox.clear();
		singlebox.shrink_to_fit();
	}
	for (int i = 0; i < finalboxes.size(); i++)
	{
		int x = finalboxes[i][0].x;
		int y = finalboxes[i][0].y;
		int width = finalboxes[i][finalboxes[i].size() - 1].x - x + finalboxes[i][finalboxes[i].size() - 1].width + 1;
		//2019.09.29@wenyang
		int height = lines_up_down[i][1] - lines_up_down[i][0];
		singlebox.push_back(cv::Rect(x, lines_up_down[i][0], width, height));
		//int height = finalboxes[i][0].height;
		//int height = finalboxes[i][(finalboxes[i]).size()-1].y + finalboxes[i][(finalboxes[i]).size() - 1].height - finalboxes[i][0].y;

		//singlebox.push_back(Rect(x, y, width, height));
	}
	delete[]num;
	delete[]p;

	return singlebox;
}

//自定义排序函数  2020.03.25@wenyang
bool sortFun(const cv::Rect& p1, const cv::Rect& p2)
{
	return p1.y < p2.y;//升序排列  
}

vector<cv::Rect> postprocess(cv::Mat& frame, const vector<cv::Mat>& outs, float confThreshold, float nmsThreshold)
{
	vector<int> classIds;
	vector<float> confidences;
	vector<cv::Rect> boxes;

	for (size_t i = 0; i < outs.size(); ++i)
	{
		// Scan through all the bounding boxes output from the network and keep only the
		// ones with high confidence scores. Assign the box's class label as the class
		// with the highest score for the box.
		float* data = (float*)outs[i].data;
		for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
		{
			cv::Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
			cv::Point classIdPoint;
			double confidence;
			// Get the value and location of the maximum score
			minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
			if (confidence > confThreshold)
			{

				int centerX = (int)(data[0] * frame.cols);
				int centerY = (int)(data[1] * frame.rows);
				int width = (int)(data[2] * frame.cols);
				int height = (int)(data[3] * frame.rows);
				int left = centerX - width / 2;
				int top = centerY - height / 2;

				classIds.push_back(classIdPoint.x);
				confidences.push_back((float)confidence);
				boxes.push_back(cv::Rect(left, top, width, height));//boxes中存储所有小box的相关信息，即左上角坐标以及宽高
			}
		}
	}

	// Perform non maximum suppression to eliminate redundant overlapping boxes with
	// lower confidences
	vector<int> indices;
	NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);
	vector<cv::Rect> final = fuseBox(boxes);
	/*for (int i = 0; i < final.size(); i++)
	{
		int left = final[i].x;
		int top = final[i].y;
		int right = left + final[i].width;
		int bottom = final[i].y + final[i].height;
		rectangle(frame, cv::Point(left - 20, top), cv::Point(right + 20, bottom), cv::Scalar(0, 0, 255), 3);
	}*/
	sort(final.begin(), final.end(), sortFun);
	return final;
	/*for (size_t i = 0; i < indices.size(); ++i)
	{
		int idx = indices[i];
		Rect box = boxes[idx];
		drawPred(classIds[idx], confidences[idx], box.x, box.y,
			box.x + box.width, box.y + box.height, frame);
	}*/
}


int YOLO::init_yolo(string yoloConfigPath,string yoloWeightsPath) {
	cv::String modelConfiguration = yoloConfigPath;
	cv::String modelBinary = yoloWeightsPath;
	try {
		//clock_t start = clock();
		net = readNetFromDarknet(modelConfiguration, modelBinary);
		cout << "yolo模型加载成功" << endl;
		//cout << "yolo加载用时：" << clock() - start << endl;
	}
	catch (...) {
		return -1;//模型加载失败
	}

	if (net.empty())
	{
		printf("Could not load net...\n");
		return -1;
	}

	/*要求网络在支持的地方使用特定的计算后端
	*如果opencv是用intel的推理引擎库编译的，那么dnn_backend_default意味着dnn_backend_interrusion_引擎
	*否则等于dnn_backend_opencv。
	*/
	net.setPreferableBackend(DNN_BACKEND_OPENCV);
	//要求网络对特定目标设备进行计算
	net.setPreferableTarget(DNN_TARGET_CPU);
	return 0;
}

int YOLO::text_detect(cv::Mat img, vector<cv::Rect>& boxes){
	//加载网络模
	if (img.channels() == 1)
		cvtColor(img, img, cv::COLOR_GRAY2BGR);

	vector<cv::Mat> frames;
	frames.push_back(img);
	cv::Mat inputBlob = blobFromImages(frames, 1 / 255.F, cv::Size(608, 608), cv::Scalar(), true, false);
	
	net.setInput(inputBlob, "data");

	//检测和显示
	//获得“dectection_out"的输出
	vector<cv::Mat> outs;
	net.forward(outs, getOutputsNames(net));
	double thresh = 0.25;
	double nms_thresh = 0.25;
	boxes = postprocess(img, outs, thresh, nms_thresh);

	//cv::namedWindow("OCT", 0);
	//cv::imshow("OCT", img);
	//cv::waitKey();
	return 0;
}

