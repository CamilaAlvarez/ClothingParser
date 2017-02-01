//
// Created by Camila Alvarez on 01-02-17.
//

#include "Pair.hpp"
#include <sys/stat.h>

void Pair::updateImages(const std::string &newImageDirectory) {
    std::ostringstream imageDirectory;
    imageDirectory << newImageDirectory << "/" << this->pairId;
    std::string newDirectory = imageDirectory.str();
    mkdir(newDirectory, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
    this->catalogImage.cutImage(newDirectory);
    this->outdoorImage.cutImage(newDirectory);
}