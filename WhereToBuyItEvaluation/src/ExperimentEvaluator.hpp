//
// Created by Camila Alvarez on 21-07-16.
//

#ifndef WHERETOBUYITEVALUATION_EXPERIMENTEVALUATOR_HPP
#define WHERETOBUYITEVALUATION_EXPERIMENTEVALUATOR_HPP

#include "jmsr/JLinearSearch.h"
#include "jmsr/JDistance.h"
#include <map>

class PairIdVector{
private:
    std::string id;
    std::vector<float> values;
public:
    PairIdVector(std::string id, std::vector<float> values);
    std::string getId(){return id; };
    std::vector<float> getValues(){ return values; };
    friend std::ostream& operator<<(std::ostream& os, const PairIdVector& pair);
    std::vector<float> getValues() const { return values; };

};



template <class Distance>
class ExperimentEvaluator {
private:
    std::vector<std::string> testKeys;
    std::map<std::string, std::string> perfomedExperiments;
    std::map<std::string, float*> testDesc;
    std::map<std::string, float*> retrievalMap;
    std::map<std::string, std::string> imageClassMap;
    std::map<std::string, int> classMap;
    std::map<std::string, std::string> retrievalClasses;
    std::map<std::string, std::string> testingClasses;
    int descSize;
    std::vector<search2::ResultPair> search(const float* query, const std::string& expectedClass, int K = -1);
    void load(const std::map<std::string, float*> &descriptors, const std::string &retrievalCodes,
              const std::string &testingCodes, const std::string &classesFile);
public:
    ExperimentEvaluator(const std::map<std::string, float*> &descriptors, const std::string &retrievalCodes,
                        const std::string &testingCodes, const std::string &classesFile, int descSize);
    void runRetrievalExperiments(const std::string& outputDir);
    ~ExperimentEvaluator();
};
#endif //WHERETOBUYITEVALUATION_EXPERIMENTEVALUATOR_HPP
