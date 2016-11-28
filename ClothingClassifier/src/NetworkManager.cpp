//
// Created by Camila Alvarez on 18-11-16.
//

#include "NetworkManager.hpp"
#include <cstring>
#include <fstream>
#include <jmsr/preprocessing.h>

NetworkManager::NetworkManager(const std::string &config_file):network_config_file(ConfigFile(config_file, '\t')), predictor() {
    JUtil::jmsr_assert(network_config_file.isDefined("CAFFE_DIR"), "Missing CAFFE_DIR parameter");
    JUtil::jmsr_assert(network_config_file.isDefined("PROTOTXT"), "Missing PROTOTXT parameter");
    JUtil::jmsr_assert(network_config_file.isDefined("CAFFE_MODEL"), "Missing CAFFE_MODEL parameter");
    JUtil::jmsr_assert(network_config_file.isDefined("IMAGE_WIDTH"), "Missing IMAGE_WIDTH parameter");
    JUtil::jmsr_assert(network_config_file.isDefined("IMAGE_HEIGHT"), "Missing IMAGE_HEIGHT parameter");
    JUtil::jmsr_assert(network_config_file.isDefined("CAFFE_MODE"), "Missing CAFFE_MODE parameter");

    std::string dir = network_config_file.getValue("CAFFE_DIR");
    std::string prototxt = dir+network_config_file.getValue("PROTOTXT");
    std::string caffe_model = dir+network_config_file.getValue("CAFFE_MODEL");
    int image_width= std::stoi(network_config_file.getValue("IMAGE_WIDTH"));
    int image_height= std::stoi(network_config_file.getValue("IMAGE_HEIGHT"));
    int mode = network_config_file.getValue("CAFFE_MODE").compare("GPU") == 0 ? CAFFE_GPU_MODE : CAFFE_CPU_MODE;

    this->predictor.load(prototxt, caffe_model, image_width, image_height, mode);
    std::cout<<"LOADING OK!"<<std::endl;
}

float* NetworkManager::getLayer(const std::string &layer_name, int *descriptor_size,const std::string &image_file) {
    try {
	std::cout<<layer_name<<std::endl;
        float *desc = this->predictor.getCaffeDescriptor(image_file, descriptor_size, layer_name);
	return desc;
    }
    catch(const std::runtime_error &e){
        std::cout<<"INVALID IMAGE: "<<image_file<<std::endl;
	return NULL;
    }
}

NetworkManager::~NetworkManager() { }
