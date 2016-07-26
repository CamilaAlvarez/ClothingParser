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
    std::map<std::string, float*> testDesc;
    std::map<std::string, float*> retrievalMap;
    std::map<std::string, std::string> imageClassMap;
    std::map<std::string, int> classMap;
    int descSize;
    std::vector<search2::ResultPair> search(const float* query, int K = -1);
    std::vector<float> calculatePrecisionRecall(const std::string &idQuery,
            const std::vector<search2::ResultPair>& results);
    std::map<std::string, std::vector<float>> calculateAverageMeasurements(const std::map<std::string,
            std::vector<PairIdVector>> &measurements);
    void writeResultsToFile(const std::map<std::string, std::vector<PairIdVector>>& individualResults,
                            const std::map<std::string, std::vector<float>>& averages, const std::string &output, const std::string &type);
    void load(const std::map<std::string, float*> &descriptors, const std::string &retrievalCodes,
              const std::string &testingCodes, const std::string &classesFile);
public:
    ExperimentEvaluator(const std::string &descriptorFile, const std::string &retrievalCodes,
                        const std::string &testingCodes, const std::string &classesFile);
    ExperimentEvaluator(const std::map<std::string, float*> &descriptors, const std::string &retrievalCodes,
                        const std::string &testingCodes, const std::string &classesFile);
    void runExperiments(const std::string &outputFile, const std::string &firstRetrievedFile);
    ~ExperimentEvaluator();
};
#endif //WHERETOBUYITEVALUATION_EXPERIMENTEVALUATOR_HPP
