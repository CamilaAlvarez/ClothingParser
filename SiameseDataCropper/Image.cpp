//
// Created by Camila Alvarez on 01-02-17.
//

#include "Image.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <sstream>

void Image::cutImage(const std::string &newDirectory) {
    cv::Mat image = cv::imread(this->imageLocation, -1);
    if(this->boundingBox==NULL || this->boundingBox->getIsActive()==0){
        this->writeImage(newDirectory, image);
        return;
    }
    int width = image.cols;
    int height = image.rows;
    float factor = 1;
    int chosenHeight = 435;
    if(height > chosenHeight){
        factor = height/chosenHeight;
    }
    int rectHeight = boundingBox->getHeight()*factor;
    int rectWidth = boundingBox->getWidth()*factor;
    if(rectHeight > height)
        rectHeight = height;
    if(rectWidth > width)
        rectWidth = width;
    cv::Rect ROI(boundingBox->getX()*factor, boundingBox->getY()*factor,
                 rectWidth,rectHeight);
    cv::Mat ROIImage = image(ROI);
    this->writeImage(newDirectory, ROIImage);

}

bool Image::writeImage(const std::string &newDirectory,const cv::Mat &image) {
    std::osstringstream newImage;
    newImage << newDirectory << "/" << this->imageName;
    cv::imwrite(newImage.str(), image);
}
