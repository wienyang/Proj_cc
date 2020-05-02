#pragma once
#include"ocr.h"
#include"utils.h"

using tensorflow::Flag;
using tensorflow::Tensor;
using tensorflow::TensorShape;
using tensorflow::Status;
using tensorflow::string;
using tensorflow::int32;
using tensorflow::DT_FLOAT;

Status OCR::LoadGraph(const string& graph_file_name) {
	tensorflow::GraphDef graph_def;
	Status load_graph_status =
		ReadBinaryProto(tensorflow::Env::Default(), graph_file_name, &graph_def);
	if (!load_graph_status.ok()) {
		return tensorflow::errors::NotFound("Failed to load compute graph at '",
			graph_file_name, "'");
	}
	session.reset(tensorflow::NewSession(tensorflow::SessionOptions()));
	Status session_create_status = session->Create(graph_def);
	if (!session_create_status.ok()) {
		return session_create_status;
	}
	return Status::OK();
}

std::vector<int> OCR::net_forward(Tensor& input) {
	std::vector<Tensor> outputs;
	Status run_status = session->Run({ {"input", input} }, { "shadow_net/sequence_rnn_module/transpose_time_major" }, {}, &outputs);
	if (run_status != Status::OK())
		return {};
	std::vector<int> shape = tensorShape(outputs[0]);
	auto out_tensor = outputs[0].tensor<float, 3>();

	std::vector<int> ret(shape[0]);
	for (int i = 0; i < shape[0]; i++) {
		float max = out_tensor(i, 0, 0);
		int max_index = 0;
		for (int k = 1; k < shape[2]; k++) {
			if (out_tensor(i, 0, k) > max) {
				max = out_tensor(i, 0, k);
				max_index = k;
			}
		}
		ret[i] = max_index;
	}
	return ret;
}

int OCR::readCharTable(std::string path)
{
	const std::locale utf8(std::locale(), new std::codecvt_utf8<wchar_t>);
	std::wifstream f_in(path, std::ios::in);
	f_in.imbue(utf8);
	f_in >> char_table;
	return 0;
}

std::vector<int> OCR::ctc_decode(std::vector<int>& arr) {
	std::vector<int> output;
	int index = 0;
	while (index < arr.size()) {
		while (arr[index] == num_class - 1) {
			index++;
			if (index >= arr.size())
				return output;
		}
		int cur = arr[index];
		while (arr[index] == cur) {
			index++;
			if (index >= arr.size())
				return output;
		}
		output.push_back(cur);
		index++;
	}
	return output;
}

std::wstring OCR::recognize(cv::Mat img) {
	std::wstring output;
	Tensor input = Mat2Tensor(img);
	std::vector<int> ret = net_forward(input);
	ret = ctc_decode(ret);

	for (auto i : ret) {
		//std::wcout << char_table[i] << std::endl;
		output.push_back(char_table[i]);
	}
	return output;
}

