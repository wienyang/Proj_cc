#pragma once
#include "utils.h"

std::vector<int> tensorShape(tensorflow::Tensor& tensor)
{
	std::vector<int> shape;
	int num_dimensions = tensor.shape().dims();
	for (int ii_dim = 0; ii_dim < num_dimensions; ii_dim++) {
		shape.push_back(tensor.shape().dim_size(ii_dim));
	}
	return shape;
}


tensorflow::Tensor Mat2Tensor(cv::Mat& img) {
	int new_height = 32;
	float rate = static_cast<float>(new_height) / static_cast<float>(img.rows);
	int new_width = static_cast<int>(rate * img.cols);
	cv::Mat reshaped_img;
	resize(img, reshaped_img, cv::Size(new_width, new_height));
	reshaped_img.convertTo(reshaped_img, CV_32FC1);
	reshaped_img /= 127.5f;
	reshaped_img -= 1.0f;
	if (reshaped_img.channels() == 1)
		cvtColor(reshaped_img, reshaped_img, cv::COLOR_GRAY2BGR);
	//reshaped_img.convertTo(reshaped_img, CV_32FC3);
	//std::cout << reshaped_img;
	tensorflow::Tensor input_tensor(tensorflow::DT_FLOAT, tensorflow::TensorShape({ 1,new_height,new_width,3 }));
	for (int row = 0; row < new_height; ++row) {
		for (int col = 0; col < new_width; ++col) {
			for (int depth = 0; depth < 3; ++depth) {
				input_tensor.tensor<float, 4>()(0, row, col, depth) = reshaped_img.at<cv::Vec3f>(row, col)[depth];
			}
		}
	}
	return input_tensor;
}