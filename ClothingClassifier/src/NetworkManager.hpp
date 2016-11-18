//
// Created by Camila Alvarez on 18-11-16.
//

#ifndef CLOTHINGCLASSIFIER_NETWORKMANAGER_HPP
#define CLOTHINGCLASSIFIER_NETWORKMANAGER_HPP

#include <cvision/caffe_predictor.h>
#include <jmsr/JUtil.h>
#include <iostream>

class NetworkManager {
public:
    NetworkManager(const std::string &config_file);
    float * getLayer(std::string layer_name, float &descriptor_size, const std::string &image_file);
    ~NetworkManager();

private:
    ConfigFile network_config_file;
    CaffePredictor predictor;

};


#endif //CLOTHINGCLASSIFIER_NETWORKMANAGER_HPP
