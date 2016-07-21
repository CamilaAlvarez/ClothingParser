#include <iostream>
#include <DescriptorManager.hpp>
#include <algorithm>

using namespace std;

void calculateDescriptors(std::string configFile, std::string imageIdFiles, std::string finalBinaryFile){
    DescriptorManager d(configFile);
    d.calculateDescriptors(imageIdFiles);
    d.saveDescriptors(finalBinaryFile);
}


int main(int argc, char* argv[]) {
    std::vector<std::string> args;
    for(int i=1; i<argc; i++)
        args.push_back(argv[i]);

    if(std::find(args.begin(), args.end(),"-calculate_descriptors" ) != args.end())
        calculateDescriptors("Experimentfc7.config", "images_ids.txt","wheretobuydescfc7.bin");


    return 0;
}
