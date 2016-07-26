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

std::ostream& operator<<(std::ostream& os, const std::vector<float>& array){
    std::string s = "";
    for(std::vector<float>::const_iterator it = array.begin(); it!=array.end(); ++it){
        std::string aux = "\t"+std::to_string(*it);
        s += aux;
    }
    os<<s<<std::endl;
    return os;
}

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
ExperimentEvaluator<Distance>::ExperimentEvaluator(const std::string &descriptorFile, const std::string &retrievalCodes,
                                         const std::string &testingCodes, const std::string &classesFile) {
    std::cout<<"OPEN DESCRIPTORS FILE"<<std::endl;
    std::map<std::string,float*> descriptorsMap = loadFileToFloatMap(descriptorFile.c_str(), &descSize);
    load(descriptorsMap, retrievalCodes, testingCodes, classesFile);
}

template <class Distance>
ExperimentEvaluator<Distance>::ExperimentEvaluator(const std::map<std::string, float*> &descriptorsMap, const std::string &retrievalCodes,
                                                   const std::string &testingCodes, const std::string &classesFile) {
    load(descriptorsMap, retrievalCodes, testingCodes, classesFile);
}


//Guardar menos cosas al principio. Me estoy comiendo la memoria!!
template <class Distance>
void ExperimentEvaluator<Distance>::runExperiments(const std::string &outputFile, const std::string& firstRerievedFile) {
    std::cout<<"STARTING EXPERIMENT"<<std::endl;
    std::map<std::string, std::string> firstRetrieved;
    std::map<std::string, std::vector<search2::ResultPair>> mapResults;
    #ifdef _OPENMP
	omp_lock_t lock;
	omp_init_lock(&lock);
     #endif   
    int count = 0; 
    #pragma omp parallel for
    for(int i = 0; i < testKeys.size(); i++){  
	std::string key = testKeys[i];
	float *query = testDesc[key];
	#ifdef _OPENMP
	    omp_set_lock(&lock);
	#endif
	std::cout<<"BEGAN SEARCH NUMBER "<<++count<<" FOR: "<<key<<std::endl;
        #ifdef _OPENMP
	    omp_unset_lock(&lock);
	#endif
	std::vector<search2::ResultPair> results = search(query);
        firstRetrieved[key] = results[0].getId();
        mapResults[key] = results;
    }

    #ifdef _OPENMP
	omp_destroy_lock(&lock);
    #endif
    std::map<std::string, std::vector<PairIdVector>> presicionPerClass = calculateMeasurement(mapResults,
                                                                                            [](int relevantItems, int retrieved, int totalRelevant){
                                                                                                return ((float)relevantItems)/((float) retrieved);
                                                                                            });
    std::map<std::string, std::vector<PairIdVector>> recallPerClass = calculateMeasurement(mapResults,
                                                                                            [](int relevantItems, int retrieved, int totalRelevant){
                                                                                                return ((float)relevantItems)/((float) totalRelevant);
                                                                                            });
    std::map<std::string, std::vector<float>> averagePrecisionClass = calculateAverageMeasurements(presicionPerClass);
    std::map<std::string, std::vector<float>> averageRecallClass = calculateAverageMeasurements(recallPerClass);
    writeResultsToFile(presicionPerClass, averagePrecisionClass, outputFile, "precision");
    writeResultsToFile(recallPerClass, averageRecallClass, outputFile, "recall");

    std::ofstream firstRetrievedItem(firstRerievedFile);
    for(std::map<std::string, std::string>::iterator it=firstRetrieved.begin(); it!=firstRetrieved.end(); ++it){
        firstRetrievedItem<<it->first<<"\t"<<it->second<<std::endl;
    }
    firstRetrievedItem.close();
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
    //No deberia guardar todos los resultados->Se cae por memoria
    return results;
}

template <class Distance>
std::map<std::string, std::vector<PairIdVector>> ExperimentEvaluator<Distance>::calculateMeasurement(const std::map<std::string,
        std::vector<search2::ResultPair>>& distances, std::function<float (int relevantItems, int retrieved,
                                                                           int totalRelevant)> measurement){
    typedef std::map<std::string, std::vector<search2::ResultPair>>::const_iterator iter;
    std::map<std::string, std::vector<PairIdVector>> results;

    for(std::map<std::string, int>::iterator it = classMap.begin(); it!=classMap.end(); ++it){
        results[it->first] = std::vector<PairIdVector>();
    }

    for(iter it = distances.begin(); it!=distances.end(); ++it){
        int retrieved = 0;
        int relevant = 0;
        std::vector<float> measures;
        std::string itemClass = imageClassMap[it->first];
        std::vector<search2::ResultPair> values = it->second;
        for(std::vector<search2::ResultPair>::const_iterator itert = values.begin(); itert!=values.end(); ++itert){
            std::string retrievedClass = imageClassMap[(*itert).getId()];
            if(itemClass.compare(retrievedClass)==0)
                relevant++;
            retrieved++;
            float measure = measurement(relevant, retrieved, classMap[itemClass]);
            measures.push_back(measure);
        }
        PairIdVector pair(it->first, measures);
        results[itemClass].push_back(pair);
    }

    return results;

}

template <class Distance>
std::map<std::string, std::vector<float>> ExperimentEvaluator<Distance>::calculateAverageMeasurements(const std::map<std::string,
        std::vector<PairIdVector>> &measurements){

    typedef std::map<std::string,std::vector<PairIdVector>>::const_iterator iter;
    std::map<std::string, std::vector<float>> averages;

    for(iter it = measurements.begin(); it!=measurements.end(); ++it){
        unsigned long  size = it->second[0].getValues().size();
        std::vector<PairIdVector> measurePerClass = it->second;
        unsigned long classSize = measurePerClass.size();
        std::vector<float> averagePerClass;

        for(unsigned long i = 0; i < size; i++) {
            float accumulator = 0;
            for (unsigned long j = 0; j < classSize; j++) {
                accumulator += measurePerClass[j].getValues()[i] / (float) classSize;
            }
            averagePerClass.push_back(accumulator);
        }

        averages[it->first] = averagePerClass;
    }
    return averages;
}

template <class Distance>
void ExperimentEvaluator<Distance>::writeResultsToFile(const std::map<std::string, std::vector<PairIdVector>> &individualResults,
                        const std::map<std::string, std::vector<float>> &averages, const std::string &output, const std::string &type){
    //Obtain class
    for(std::map<std::string, int>::iterator it = classMap.begin(); it!=classMap.end(); ++it){
        //obtain individual results
        std::string itemClass = it->first;
        std::vector<PairIdVector> classResults = individualResults.at(itemClass);
        std::stringstream sstream;
        //write results to stringstream
        for(std::vector<PairIdVector>::iterator itert=classResults.begin(); itert!=classResults.end(); ++itert)
		sstream<<(*itert);
        sstream<<"-1"<<averages.at(itemClass);
        std::string finalString = sstream.str();
        const char * classResultsString = finalString.c_str();
        std::string filename = "results/"+output+"_"+itemClass+"_"+type+".txt";
        writeToFile(classResultsString, filename.c_str(), (int)finalString.size());
    }

}

template <class Distance>
ExperimentEvaluator<Distance>::~ExperimentEvaluator(){
    for(map_iter it = testDesc.begin(); it!=testDesc.end(); ++it)
	delete it->second;
    
    for(map_iter it = testDesc.begin(); it!=testDesc.end(); ++it)
	delete it->second;	
}

template class ExperimentEvaluator<JL2>;
