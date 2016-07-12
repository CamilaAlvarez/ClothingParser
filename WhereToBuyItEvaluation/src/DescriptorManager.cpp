//
// Created by Camila Alvarez on 24-06-16.
//

#include "DescriptorManager.hpp"
#include <string>

DescriptorManager::DescriptorManager(const std::string &config_file):network_config_file(ConfigFile(config_file, '\t'))
{
    JUtil::jmsr_assert(network_config_file.isDefined("CAFFE_DIR"), "Missing CAFFE_DIR parameter");
    JUtil::jmsr_assert(network_config_file.isDefined("PROTOTXT"), "Missing PROTOTXT parameter");
    JUtil::jmsr_assert(network_config_file.isDefined("CAFFE_MODEL"), "Missing CAFFE_MODEL parameter");
    JUtil::jmsr_assert(network_config_file.isDefined("LAYER_NAME"), "Missing LAYER_NAME parameter");
    JUtil::jmsr_assert(network_config_file.isDefined("IMAGE_WIDTH"), "Missing IMAGE_WIDTH parameter");
    JUtil::jmsr_assert(network_config_file.isDefined("IMAGE_HEIGHT"), "Missing IMAGE_HEIGHT parameter");
    JUtil::jmsr_assert(network_config_file.isDefined("CAFFE_MODE"), "Missing CAFFE_MODE parameter");

    desc_layer_name = network_config_file.getValue("LAYER_NAME");
    std::string dir = network_config_file.getValue("CAFFE_DIR");
    std::string prototxt = dir+network_config_file.getValue("PROTOTXT");
    std::string caffe_model = dir+network_config_file.getValue("CAFFE_MODEL");
    int image_width= std::stoi(network_config_file.getValue("IMAGE_WIDTH"));
    int image_height= std::stoi(network_config_file.getValue("IMAGE_HEIGHT"));
    int mode = network_config_file.getValue("CAFFE_MODE").compare("GPU") == 0 ? CAFFE_GPU_MODE : CAFFE_CPU_MODE;

    predictor = CaffePredictor(prototxt, caffe_model, image_width, image_height, mode);
}