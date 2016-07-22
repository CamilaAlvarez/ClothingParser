//
// Created by Camila Alvarez on 21-07-16.
//

#include "ExperimentEvaluator.hpp"
#include "utils.hpp"


typedef std::map<std::string, float *>::iterator map_iter;

PairIdVector::PairIdVector(std::string id, std::vector<float> values): id(id), values(values) {
}

std::ostream& operator<<(std::ostream& os, const PairIdVector& pair){
    std::string result = pair.id;
    for(std::vector<float>::iterator it = pair.values.begin(); it!=pair.values.end(); ++it){
        result += ('\t'+*it);
    }
    os<<result<<std::endl;
    return os;
}

template <class Distance>
ExperimentEvaluator::ExperimentEvaluator(const std::string &descriptorFile, const std::string &retrievalCodes,
                                         const std::string &testingCodes, const std::string &classesFile) {

    std::map<std::string, std::string> classes = loadFileToMap(classesFile.c_str());
    for(std::map<std::string, std::string>::iterator it=classes.begin(); it!=classes.end(); ++it){
        classMap[it->first] = std::stoi(it->second);
    }

    std::map<std::string, std::string> retrievalClasses = loadFileToMap(retrievalCodes.c_str());
    std::map<std::string, std::string> testingClasses = loadFileToMap(testingCodes.c_str());
    std::ifstream descriptors(descriptorFile);
    if(descriptors){
        descriptors.seekg (0, descriptors.end);
        long long length = descriptors.tellg();
        descriptors.seekg (0, descriptors.beg);
        std::vector<float> descriptorsVector;
        descriptorsVector.resize(length/ sizeof(float));
        descriptors.read(reinterpret_cast<char *>(&descriptorsVector[0]),length);
        descriptors.close();

        descSize = (int)descriptorsVector[0];
        int i = 1;
        while(i < length){
            float *desc = new float[descSize];
            float imageCode = descriptorsVector[i];
            std::copy(descriptorsVector.begin()+i, descriptorsVector.begin()+i+descSize, desc);
            i += descSize;
            std::string imageCodeString = std::to_string(imageCode);
            if(retrievalClasses.find(imageCodeString)!= retrievalClasses.end())
                retrievalMap[imageCodeString] = desc;
            else if (testingClasses.find(imageCodeString) != testingClasses.end())
                testDesc[imageCodeString] = desc;
        }

        retrievalClasses.insert(testingClasses.begin(), testingClasses.end());
        imageClassMap = retrievalClasses;
    }
    else{
        throw std::runtime_error("Couldn't open file");
    }

}

template <class Distance>
void ExperimentEvaluator::runExperiments(const std::string &outputFile) {

    const float* query;
    std::map<std::string, std::vector<search2::ResultPair>> mapResults;
    for (map_iter it = testDesc.begin(); it!=testDesc.end(); ++it){
        query = it->second;
        std::vector<search2::ResultPair> results = search(query);
        mapResults[it->first] = results;
    }
    std::map<std::string, std::vector<PairIdVector>> presicionPerClass = calculateMeasument(mapResults,
                                                                                            [](int relevantItems, int retrieved, int totalRelevant){
                                                                                                return ((float)relevantItems)/((float) retrieved);
                                                                                            });
    std::map<std::string, std::vector<PairIdVector>> recallPerClass = calculateMeasument(mapResults,
                                                                                            [](int relevantItems, int retrieved, int totalRelevant){
                                                                                                return ((float)relevantItems)/((float) totalRelevant);
                                                                                            });
    std::map<std::string, std::vector<float>> averagePrecisionClass = calculateAverageMeasurements(presicionPerClass);
    std::map<std::string, std::vector<float>> averageRecallClass = calculateAverageMeasurements(recallPerClass);
    writeResultsToFile(presicionPerClass, averagePrecisionClass, outputFile, "precision");
    writeResultsToFile(recallPerClass, averageRecallClass, outputFile, "recall");
}

template<class Distance>
std::vector<search2::ResultPair> ExperimentEvaluator::search(const float *query, int K) {
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
std::map<std::string, std::vector<PairIdVector>> ExperimentEvaluator::calculateMeasument(const std::map<std::string,
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

std::map<std::string, std::vector<float>> ExperimentEvaluator::calculateAverageMeasurements(const std::map<std::string,
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

void ExperimentEvaluator::writeResultsToFile(const std::map<std::string, std::vector<PairIdVector>> &individualResults,
                        const std::map<std::string, std::vector<float>> &averages, const std::string &output, const std::string &type){
    //Obtain class
    for(std::map<std::string, int>::iterator it = classMap.begin(); it!=classMap.end(); ++it){
        //obtain individual results
        std::string itemClass = it->first;
        std::vector<PairIdVector> classResults = individualResults.at(itemClass);
        std::stringstream sstream;
        //write results to stringstream
        sstream<<classResults;
        sstream<<"-1"<<averages.at(itemClass);
        std::string finalString = sstream.str();
        const char * classResultsString = finalString.c_str();
        std::string filename = output+"_"+itemClass+"_"+type+".txt";
        writeToFile(classResultsString, filename.c_str(), (int)finalString.size());
    }

}