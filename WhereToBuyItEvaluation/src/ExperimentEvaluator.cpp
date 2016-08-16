//
// Created by Camila Alvarez on 21-07-16.
//
#ifdef _OPENMP
    #include <omp.h>
#endif
#include "utils.hpp"
#include "ExperimentEvaluator.hpp"
#include <fstream>
#include <sstream>
#include "jmsr/JDistance.h"

typedef std::map<std::string, float *>::iterator map_iter;

std::ostream& operator<<(std::ostream& os, const PairIdVector& pair){
    std::string result = pair.id;
    os<<result;
    os<<pair.values<<std::endl;
    return os;
}

PairIdVector::PairIdVector(std::string id, std::vector<float> values): id(id), values(values) {
}

template <class Distance>
void ExperimentEvaluator<Distance>::load(const std::map<std::string, float*> &descriptorsMap, const std::string &retrievalCodes,
                                         const std::string &testingCodes, const std::string &classesFile){
    std::cout<<"READING CLASSES"<<std::endl;
    std::map<std::string, std::string> classes = loadFileToMap(classesFile.c_str());
    std::cout<<"READ CLASSES"<<std::endl;
    for(std::map<std::string, std::string>::iterator it=classes.begin(); it!=classes.end(); ++it){
        classMap[it->first] = std::stoi(it->second);
    }

    std::cout<<"READING RETRIEVAL ITEMS"<<std::endl;
    std::map<std::string, std::string> retrievalClasses = loadFileToMap(retrievalCodes.c_str());
    std::cout<<"READING TESTING ITEMS"<<std::endl;
    std::map<std::string, std::string> testingClasses = loadFileToMap(testingCodes.c_str());

    int count = 0;
    for(std::map<std::string, float*>::const_iterator it = descriptorsMap.begin(); it!=descriptorsMap.end(); ++it){
        if(count%100==0)
            std::cout<<"Processed: "<<count<<std::endl;
        std::string imageCodeString = it->first;
        if(retrievalClasses.find(imageCodeString)!= retrievalClasses.end())
            retrievalMap[imageCodeString] = it->second;
        else if (testingClasses.find(imageCodeString) != testingClasses.end()){
            testDesc[imageCodeString] = it->second;
            testKeys.push_back(imageCodeString);
        }
        count++;
    }
    std::cout<<"LOADED DESCRIPTORS"<<std::endl;
    //Mover imageClassMap
    retrievalClasses.insert(testingClasses.begin(), testingClasses.end());
    imageClassMap = retrievalClasses;
    std::cout<<"FINISHED CREATING EXPERIMENT EVALUATOR"<<std::endl;
}


template <class Distance>
ExperimentEvaluator<Distance>::ExperimentEvaluator(const std::map<std::string, float*> &descriptorsMap, const std::string &retrievalCodes,
                                                   const std::string &testingCodes, const std::string &classesFile) {
    load(descriptorsMap, retrievalCodes, testingCodes, classesFile);
}


template <class Distance>
void ExperimentEvaluator<Distance>::runRetrievalExperiments(const std::string& outputDir) {
    std::cout<<"STARTING EXPERIMENT"<<std::endl;
    std::string dir = outputDir;
    if(outputDir[outputDir.length()-1] != '/')
        dir += "/";
    #ifdef _OPENMP
	omp_lock_t lock;
	omp_init_lock(&lock);
     #endif   
    int count = 0; 
    #pragma omp parallel for
    for(int i = 0; i < testKeys.size(); i++){
        std::vector<search2::ResultPair> results;
	    std::string key = testKeys[i];
	    float *query = testDesc[key];
        std::string filename = dir+imageClassMap[key]+"/"+key+".txt";
	    #ifdef _OPENMP
	        omp_set_lock(&lock);
	    #endif
	    std::cout<<"BEGAN SEARCH NUMBER "<<++count<<" FOR: "<<key<<std::endl;
        perfomedExperiments[filename] = imageClassMap[key];
        #ifdef _OPENMP
            omp_unset_lock(&lock);
	    #endif
	    results = search(query);
        std::stringstream resultString;
        for(int j = 0; j<results.size(); j++){
            std::string id = results[j].getId();
            resultString<<j+1<<"\t"<<id<<"\t"<<imageClassMap[id]<<std::endl;
        }
        std::string finalString = resultString.str();

        writeToFile(finalString.c_str(), filename.c_str(), (int)finalString.size()+1);
    }
    std::stringstream queriesFile;
    for(std::map<std::string, std::string>::iterator it = perfomedExperiments.begin(); it != perfomedExperiments.end();
            ++it){
        queriesFile<<it->first<<'\t'<<it->second<<std::endl;
    }
    std::string queries = queriesFile.str();
    std::string outputFile = dir+"queries.txt";
    writeToFile(queries.c_str(), outputFile.c_str(), (int)queries.length()+1);

    #ifdef _OPENMP
	omp_destroy_lock(&lock);
    #endif
    
    
}

template<class Distance>
std::vector<search2::ResultPair> ExperimentEvaluator<Distance>::search(const float *query, int K) {
    Distance d;
    float dist = 0;
    std::vector<search2::ResultPair> results;
    for(map_iter it=retrievalMap.begin(); it!=retrievalMap.end(); ++it){
        dist = d.getDistance(query, it->second, descSize);
        results.push_back(search2::ResultPair(it->first, dist));
    }
    std::sort(results.begin(), results.end(), search2::ResultPair::comp_pair_asc);
    if(K != -1 && K < results.size())
        results.resize(K);

    return results;
}

template <class Distance>
ExperimentEvaluator<Distance>::~ExperimentEvaluator(){
}

template class ExperimentEvaluator<JL2>;
