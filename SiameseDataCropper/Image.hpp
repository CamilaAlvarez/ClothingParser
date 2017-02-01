//
// Created by Camila Alvarez on 01-02-17.
//

#ifndef SIAMESEDATACROPPER_IMAGE_HPP
#define SIAMESEDATACROPPER_IMAGE_HPP

#include "BoundingBox.hpp"
#include <opencv2/core/core.hpp>
#include <iostream>
#include <string>

//We assume that image names have already been fixed
class Image {
private:
    int imageId;
    std::string imageLocation;
    std::string imageName;
    BoundingBox* boundingBox;
    bool writeImage(const std::string &newDirectory, const cv::Mat &image);
public:
    Image(int imageId, BoundingBox* bbox, const std::string &location):imageId(imageId), boundingBox(bbox),
                                                                imageLocation(location){
        size_t lastPosition = location.rfind("/");
        imageName = location.substr(lastPosition+1);
    }
    inline int getImageId(){ return imageId; }
    void cutImage(const std::string &newDirectory);
    ~Image(){
        delete boundingBox;
    }
};


#endif //SIAMESEDATACROPPER_IMAGE_HPP
