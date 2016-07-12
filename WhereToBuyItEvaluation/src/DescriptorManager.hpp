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
    void calculateDescriptors(const std::string &image_filename, std::map<std::string, long*>& descriptor_map);

private:
    ConfigFile network_config_file;
    std::string desc_layer_name;
    CaffePredictor predictor;

};


#endif //WHERETOBUYITEVALUATION_DESCRIPTORMANAGER_HPP
