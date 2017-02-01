//
// Created by Camila Alvarez on 01-02-17.
//

#ifndef SIAMESEDATACROPPER_PAIR_HPP
#define SIAMESEDATACROPPER_PAIR_HPP

#include "Image.hpp"
#include <iostream>

class Pair {
private:
    int pairId;
    Image catalogImage;
    Image outdoorImage;
public:
    Pair(int pairId, const Image &catalogImage, const Image &outdoorImage):pairId(pairId), catalogImage(catalogImage),
                                                                           outdoorImage(outdoorImage){}
    void updateImages(const std::string &newImageDirectory);
};


#endif //SIAMESEDATACROPPER_PAIR_HPP
