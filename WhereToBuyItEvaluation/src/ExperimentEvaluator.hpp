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

};



template <class Distance>
class ExperimentEvaluator {
private:
    std::map<std::string, float*> testDesc;
    std::map<std::string, float*> retrievalMap;
    std::map<std::string, std::string> imageClassMap;
    std::map<std::string, int> classMap;
    int descSize;
    std::vector<search2::ResultPair> search(const float* query, int K = -1);
    std::map<std::string, std::vector<PairIdVector>> calculateMeasument(const std::map<std::string,
            std::vector<search2::ResultPair>>& results, std::function<float (int relevantItems, int retrieved,
                                                                             int totalRelevant)> measurement);
    std::map<std::string, std::vector<float>> calculateAverageMeasurements(const std::map<std::string,
            std::vector<PairIdVector>> &measurements);
    void writeResultsToFile(const std::map<std::string, std::vector<PairIdVector>>& individualResults,
                            const std::map<std::string, std::vector<float>>& averages, const std::string &output, const std::string &type);
public:
    ExperimentEvaluator(const std::string &descriptorFile, const std::string &retrievalCodes,
                        const std::string &testingCodes, const std::string &classesFile);
    void runExperiments(const std::string &outputFile);
};

std::ostream& operator<<(std::ostream& os, const std::vector<float>& array){
    std::string s = "";
    for(std::vector<float>::const_iterator it = array.begin(); it!=array.end(); ++it){
        std::string aux = "\t"+std::to_string(*it);
        s += aux;
    }
    os<<s<<std::endl;
    return os;
}
std::ostream& operator<<(std::ostream& os, const std::vector<PairIdVector>& array){
    std::string s = "";
    for(std::vector<PairIdVector>::const_iterator it = array.begin(); it!=array.end(); ++it){
        os<<*it;
    }
    return os;
}
#endif //WHERETOBUYITEVALUATION_EXPERIMENTEVALUATOR_HPP
