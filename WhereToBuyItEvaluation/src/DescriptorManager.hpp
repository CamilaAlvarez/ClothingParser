//
// Created by Camila Alvarez on 24-06-16.
//

#ifndef WHERETOBUYITEVALUATION_DESCRIPTORMANAGER_HPP
#define WHERETOBUYITEVALUATION_DESCRIPTORMANAGER_HPP

#include "cvision/caffe_predictor.h"

class DescriptorManager {
public:
    DescriptorManager(std::string _prototxt, std::string _caffemodel, int im_w, int im_h, int mode=CAFFE_CPU_MODE):
            predictor(CaffePredictor(_prototxt, _caffemodel, im_w, im_h, mode)){}
private:
    CaffePredictor predictor;
};


#endif //WHERETOBUYITEVALUATION_DESCRIPTORMANAGER_HPP
