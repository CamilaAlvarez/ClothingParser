#include <iostream>
#include <vector>
#include <string>
#include "Pair.hpp"
#include <fstream>
#ifdef _OPENMP
#include <omp.h>
#endif


static std::vector<std::string> splitLine(const std::string &line, const std::string &separator){
    std::vector<std::string> parts;
    std::string auxLine = line;
    size_t found = auxLine.find(separator);
    while(found != std::string::npos){
        std::string part = auxLine.substr(0, found);
        parts.push_back(part);
        auxLine = auxLine.substr(found+1);
        found = auxLine.find(separator);
    }
    return parts;
}

static std::vector<Pair> readPairsFile(const std::string &filename){
    std::ifstream file(filename);
    std::string line;
    std::vector<Pair> pairs;

    while(std::getline(file, line)){
        std::vector<std::string> lineParts = splitLine(line, ",");
        int idPair = std::stoi(lineParts[0]);
        BoundingBox* catalogBbox = NULL;
        int idCatalog = std::stoi(lineParts[1]);
        if(lineParts[3].size() !=0 )
            catalogBbox = new BoundingBox(std::stoi(lineParts[3]),std::stoi(lineParts[4]),std::stoi(lineParts[5]),
                                          std::stoi(lineParts[6]),std::stoi(lineParts[7]));
        Image catalogImage(idCatalog, catalogBbox, lineParts[2]);

        int idOutdoor = std::stoi(lineParts[8]);
        BoundingBox* outdoorBBox = NULL;
        if(lineParts[3].size() !=0 )
            outdoorBBox = new BoundingBox(std::stoi(lineParts[10]),std::stoi(lineParts[11]),std::stoi(lineParts[12]),
                                          std::stoi(lineParts[13]),std::stoi(lineParts[14]));
        Image outdoorImage(idOutdoor, outdoorBBox, lineParts[9]);
        Pair pair(idPair, catalogImage, outdoorImage);
        pairs.push_back(pair);
    }
    return pairs;
}

int main() {
    std::vector<Pair> pairs = readPairsFile("");
    std::string newImageDirectory = "";
    unsigned long pairNumber = pairs.size();
    #pragma omp parallel for
    for(unsigned long i = 0; i < pairNumber; i++ ){
        Pair p = pairs[i];
        p.updateImages(newImageDirectory);
    }
    return 0;
}