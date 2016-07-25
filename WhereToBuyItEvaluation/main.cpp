#include <iostream>
#include <DescriptorManager.hpp>
#include "jmsr/JDistance.h"
#include "ExperimentEvaluator.hpp"
#include <algorithm>

using namespace std;

void calculateDescriptors(std::string configFile, std::string imageIdFiles, std::string finalBinaryFile){
    DescriptorManager d(configFile);
    d.calculateDescriptors(imageIdFiles);
    d.saveDescriptors(finalBinaryFile);
}

void runExperiments(const std::string &descriptorFile, const std::string &retrievalCodes,
                    const std::string &testingCodes, const std::string &classesFile, const std::string &output,
                    const std::string &firstRetrievedFile){
    ExperimentEvaluator<JL2> experiment(descriptorFile, retrievalCodes, testingCodes, classesFile);
    experiment.runExperiments(output, firstRetrievedFile);
}

int main(int argc, char* argv[]) {
    std::vector<std::string> args;
    for(int i=1; i<argc; i++)
        args.push_back(argv[i]);

    std::string descFile = "descriptors/wheretobuydescfc7.bin";
    if(std::find(args.begin(), args.end(),"-calculate_descriptors" ) != args.end())
        calculateDescriptors("configFiles/Experimentfc7.config", "neededFiles/images_ids.txt", descFile);

    if(std::find(args.begin(), args.end(), "-run_experiment") != args.end())
        runExperiments(descFile, "neededFiles/retrieval.txt", "neededFiles/testing.txt", "neededFiles/classes.txt", "experiment", "results/firstRetrieved.txt");
    return 0;
}
