//
// Created by Camila Alvarez on 02-08-16.
//

#include "ExperimentConfigurator.hpp"
#include "DescriptorManager.hpp"
#include "ExperimentEvaluator.hpp"
#include "MeasureCalculatorRetrieval.hpp"
#include <string>
#include "utils.hpp"
#include <jmsr/preprocessing.h>

ExperimentConfigurator::ExperimentConfigurator(const std::string &configFile):
        configuration(ConfigFile(configFile, '\t')){
    JUtil::jmsr_assert(configuration.isDefined("DESCRIPTORS_FILE"), "Missing DESCRIPTORS_FILE parameter");
    JUtil::jmsr_assert(configuration.isDefined("NETWORK_CONFIG_FILE"), "Missing NETWORK_CONFIG_FILE parameter");
    JUtil::jmsr_assert(configuration.isDefined("IMAGES_IDS_FILE"), "Missing IMAGES_ID_FILE parameter");
    JUtil::jmsr_assert(configuration.isDefined("TESTING_FILE"), "Missing TESTING_FILE parameter");
    JUtil::jmsr_assert(configuration.isDefined("IMAGES_CLASSES"), "Missing IMAGES_CLASSES parameter");
    JUtil::jmsr_assert(configuration.isDefined("OUTPUT_DIR"), "Missing OUTPUT_DIR parameter");

    JUtil::jmsr_assert(configuration.isDefined("CALCULATE_DESCRIPTORS"), "Missing CALCULATE_DESCRIPTORS parameter");
}

void ExperimentConfigurator::executeExperiment() {

    std::string descFile = configuration.getValue("DESCRIPTORS_FILE");
    std::string calculateDescriptors = configuration.getValue("CALCULATE_DESCRIPTORS");
    std::string experimentType = configuration.getValue("EXPERIMENT_TYPE");
    std::string testingImages = configuration.getValue("TESTING_FILE");
    std::string classesFile = configuration.getValue("IMAGES_CLASSES");
    std::string outputDir = configuration.getValue("OUTPUT_DIR");
    std::map<std::string, float*> descriptors;

    if(outputDir[outputDir.length()-1]!='/')
        outputDir += "/";

    //Calculate descriptors
    if(!calculateDescriptors.compare("YES")) {
        DescriptorManager manager(configuration.getValue("NETWORK_CONFIG_FILE"));
        manager.calculateDescriptors(configuration.getValue("IMAGES_IDS_FILE"));
        manager.saveDescriptors(descFile);
        descriptors = manager.getDescriptorMap();
    }
    else if(!calculateDescriptors.compare("LOAD")){
        descriptors = DescriptorManager::loadDescriptors(descFile);
    }

    //Run experiments
    if(configuration.isDefined("EXPERIMENT_TYPE") && (!calculateDescriptors.compare("YES") || !calculateDescriptors.compare("LOAD"))) {
        if (!experimentType.compare("RETRIEVAL")) {
            JUtil::jmsr_assert(configuration.isDefined("RETRIEVAL_IMAGES"), "Missing RETRIEVAL_IMAGES parameter");
            std::string retrievalImages = configuration.getValue("RETRIEVAL_IMAGES");
            ExperimentEvaluator<JL2> evaluator(descriptors, retrievalImages, testingImages, classesFile);
            evaluator.runRetrievalExperiments(outputDir);
        }
        else if (!experimentType.compare("CLASSIFICATION")) {
            DescriptorManager manager("configFiles/googleNetLoss1Fc.config");
           // calculateDescriptors(manager, "neededFiles/images_ids.txt", descFile);
           // manager.destroyDescriptors(configuration);
        }
    }

    //Calculate measurements
    if(configuration.isDefined("CALCULATE_MEASUREMENTS")){
        //query file must exist
        JUtil::jmsr_assert(configuration.isDefined("QUERIES_FILE"), "Missing QUERIES_FILE parameter");
        std::string queriesFile = configuration.getValue("QUERIES_FILE");
        if(!configuration.getValue("CALCULATE_MEASUREMENTS").compare("RETRIEVAL")){
            JUtil::jmsr_assert(configuration.isDefined("STEP"), "Missing STEP parameter");
            int step = std::stoi(configuration.getValue("STEP"));
            JUtil::jmsr_assert(configuration.isDefined("RETRIEVED_ITEMS"), "Missing RETRIEVED_ITEMS parameter");
            int retrievedNumber = std::stoi(configuration.getValue("RETRIEVED_ITEMS"));
            MeasureCalculatorRetrieval calc(queriesFile, classesFile);

            //float MAP = calc.calculateMAP();
            //std::vector<float> averageAccuracyVsRetrieved = calc.calculateAverageAccuracyVsRetrieved(step, retrievedNumber);
            std::map<std::string, std::vector<float>> accuracyVsRetrievedByClass = calc.calculateAccuracyVsRetrieved(step, retrievedNumber);
            //std::map<std::string, std::vector<float>> precisionVsRecall = calc.calculatePrecisionVsRecall();

            std::stringstream accuracyVsRetrievedStream;
            /*accuracyVsRetrievedStream<<"STEP"<<'\t'<<step<<std::endl;
            accuracyVsRetrievedStream<<"average"<<'\t'<<averageAccuracyVsRetrieved<<std::endl;*/
            for(std::map<std::string, std::vector<float>>::iterator it = accuracyVsRetrievedByClass.begin();
                    it != accuracyVsRetrievedByClass.end(); ++it){
                accuracyVsRetrievedStream<<it->first<<'\t'<<it->second<<std::endl;
            }

            /*std::string finalString = accuracyVsRetrievedStream.str();
	        std::string output = outputDir+"accuracyVsRetrieved.txt";
            writeToFile(finalString.c_str(), output.c_str(), (int)finalString.length()+1);

            std::stringstream precisionRecallStream;
            precisionRecallStream<<"MAP"<<'\t'<<MAP<<std::endl;
            for(std::map<std::string, std::vector<float>>::iterator it = precisionVsRecall.begin();
                    it!=precisionVsRecall.end(); ++it){
                precisionRecallStream<<it->first<<'\t'<<it->second<<std::endl;
            }
            finalString = precisionRecallStream.str();
	    output = outputDir+"precisionVsRecall.txt";
            writeToFile(finalString.c_str(), output.c_str(), (int)finalString.length()+1);*/

        }
    }

    DescriptorManager::destroyDescriptors(descriptors);
}
