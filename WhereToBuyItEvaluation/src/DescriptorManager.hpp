//
// Created by Camila Alvarez on 24-06-16.
//

#ifndef WHERETOBUYITEVALUATION_DESCRIPTORMANAGER_HPP
#define WHERETOBUYITEVALUATION_DESCRIPTORMANAGER_HPP

#include <cvision/caffe_predictor.h>
#include <jmsr/JUtil.h>
#include <iostream>
#include <map>


class DescriptorManager {
public:
    DescriptorManager(const std::string &config_file);
    void calculateDescriptors(const std::string &image_filename);
    void saveDescriptors(const std::string &output);
    ~DescriptorManager();

private:
    ConfigFile network_config_file;
    std::string desc_layer_name;
    std::map<std::string, float *> descriptor_map; //memory has to be freed when descriptors are no longer used
    CaffePredictor predictor;
    int desc_size;
    std::map<std::string, std::string> invalid_image_map;

};


#endif //WHERETOBUYITEVALUATION_DESCRIPTORMANAGER_HPP
