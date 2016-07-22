#include <iostream>
#include <DescriptorManager.hpp>
#include "ExperimentEvaluator.hpp"
#include <algorithm>

using namespace std;

void calculateDescriptors(std::string configFile, std::string imageIdFiles, std::string finalBinaryFile){
    DescriptorManager d(configFile);
    d.calculateDescriptors(imageIdFiles);
    d.saveDescriptors(finalBinaryFile);
}

void runExperiments(const std::string &descriptorFile, const std::string &retrievalCodes,
                    const std::string &testingCodes, const std::string &classesFile, const std::string &output){
    ExperimentEvaluator exp(descriptorFile, retrievalCodes, testingCodes, classesFile);
    exp.runExperiments(output);
}

int main(int argc, char* argv[]) {
    std::vector<std::string> args;
    for(int i=1; i<argc; i++)
        args.push_back(argv[i]);

    std::string descFile = "wheretobuydescfc7.bin";
    if(std::find(args.begin(), args.end(),"-calculate_descriptors" ) != args.end())
        calculateDescriptors("Experimentfc7.config", "images_ids.txt", descFile);

    if(std::find(args.begin(), args.end(), "-run_experiment") != args.end())
        runExperiments(descFile, "retrieval.txt", "testing.txt", "classes.txt", "experiment");
    return 0;
}
