#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "NetworkManager.hpp"

int main(int argc, char** argv) {
    if(argc<2){
        std::cout<<"MISSING ARGUMENT -configFile=<config_file>"<<std::endl;
        exit(1);
    }
    std::string argument_name = "-configFile=";
    std::string argument = argv[1];
    if(!argument.compare(0, argument_name.length(), argument_name)!=0){
        std::cout<<"MISSING ARGUMENT -configFile=<config_file>"<<std::endl;
        exit(1);
    }
    auto pos = argument.find("=");
    if(pos==std::string::npos){
        std::cout<<"MISSING ARGUMENT -configFile=<config_file>"<<std::endl;
        exit(1);
    }
    std::string config_file = argument.substr(pos+1);
    ConfigFile evaluation_file(config_file, '\t');
    JUtil::jmsr_assert(evaluation_file.isDefined("IMAGE_FILE"), "Missing IMAGE_FILE parameter");
    JUtil::jmsr_assert(evaluation_file.isDefined("NETWORK_CONFIG"), "Missing NETWORK_CONFIG parameter");
    JUtil::jmsr_assert(evaluation_file.isDefined("OUTPUT_FILE"), "Missing OUTPUT_FILE parameter");

    std::string image_file = evaluation_file.getValue("IMAGE_FILE");
    std::string network_config = evaluation_file.getValue("NETWORK_CONFIG");
    std::string results_file = evaluation_file.getValue("OUTPUT_FILE");
    std::ifstream infile(image_file);

    NetworkManager manager(network_config);
    std::string line;
    std::vector<std::string> results;
    while (std::getline(infile, line))
    {
        std::istringstream stream_line(line);
        std::string image;
        int image_class;
        float descriptor_size;
        if (!(stream_line >> image >> image_class)) { break; } // error
        float *probs = manager.getLayer("prob", descriptor_size, image);
        float class_prob = probs[image_class];
        std::cout<<"Descriptor size: "<<descriptor_size<<std::endl;
        std::cout<<"Expected class "<<image_class<<" probability: "<<class_prob<<std::endl;
        std::ostringstream ss;
        ss << image_class << "\t" << class_prob << std::endl;
        results.push_back(ss.str());
    }

    std::ofstream outfile;
    outfile.open(results_file);
    for(std::string result : results){
        outfile << result;
    }
    outfile.close();
    return 0;
}