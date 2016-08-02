#include <iostream>
#include <DescriptorManager.hpp>
#include "jmsr/JDistance.h"
#include "ExperimentEvaluator.hpp"
#include <algorithm>

using namespace std;

void calculateDescriptors(DescriptorManager& manager, std::string imageIdFiles, std::string finalBinaryFile){
    manager.calculateDescriptors(imageIdFiles);
    manager.saveDescriptors(finalBinaryFile);
}

void runExperiments(ExperimentEvaluator<JL2>& experiment, const std::string &output,
                    const std::string &firstRetrievedFile){
    experiment.runExperiments(output, firstRetrievedFile);
}

int main(int argc, char* argv[]) {
    std::vector<std::string> args;
    for(int i=1; i<argc; i++)
        args.push_back(argv[i]);

    std::string descFile = "descriptors/googlenetloss1fc.bin";

    if(std::find(args.begin(), args.end(),"-calculate_descriptors" ) != args.end() &&
            std::find(args.begin(), args.end(), "-run_experiment") != args.end()){
        DescriptorManager manager("configFiles/googleNetLoss1Fc.config");
        calculateDescriptors(manager,"neededFiles/images_ids.txt", descFile);
        std::map<std::string, float*> descriptors = manager.getDescriptorMap();
        ExperimentEvaluator<JL2> experiment(descriptors, "neededFiles/retrieval.txt", "neededFiles/testing.txt",
                                            "neededFiles/classes.txt");
        runExperiments(experiment, "experiment", "results/firstRetrieved.txt");
    }
    else if(std::find(args.begin(), args.end(),"-calculate_descriptors" ) != args.end()){
        DescriptorManager manager("configFiles/googleNetLoss1Fc.config");
        calculateDescriptors(manager, "neededFiles/images_ids.txt", descFile);
	manager.destroyDescriptors();
    }
    if(std::find(args.begin(), args.end(), "-run_experiment") != args.end()){
        ExperimentEvaluator<JL2> experiment(descFile, "neededFiles/retrieval.txt", "neededFiles/testing.txt",
                                            "neededFiles/classes.txt");
        runExperiments(experiment, "experiment", "results/firstRetrieved.txt");
    }

    return 0;
}
