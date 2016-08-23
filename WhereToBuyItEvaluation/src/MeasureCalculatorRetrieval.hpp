//
// Created by Camila Alvarez on 05-08-16.
//

#ifndef WHERETOBUYITEVALUATION_MEASURECALCULATORRETRIEVAL_HPP
#define WHERETOBUYITEVALUATION_MEASURECALCULATORRETRIEVAL_HPP

#include "MeasureCalculator.hpp"
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>

class RelevantConditionCalculator{
public:
    virtual bool isRelevant(std::string retrievedClass, std::string retrievedId) = 0;
    RelevantConditionCalculator(std::string expectedClass):expectedClass(expectedClass){}
    RelevantConditionCalculator(std::string expectedProduct, std::string expectedClass, std::map<std::string,
            std::string>* retrievalMap): expectedProd(expectedProduct), retrievalVsProd(retrievalMap), expectedClass(expectedClass){}
protected:
    std::string expectedProd;
    std::string expectedClass;
    std::map<std::string, std::string>* retrievalVsProd;
};

class RelevantClassCalculator : public RelevantConditionCalculator{
public:
    bool isRelevant(std::string retrievedClass, std::string retrievedId){ return !retrievedClass.compare(expectedClass); }
    RelevantClassCalculator(std::string expectedClass): RelevantConditionCalculator(expectedClass){}
};

class ExactRelevantCalculator : public RelevantConditionCalculator{
public:
    bool isRelevant(std::string retrievedClass, std::string retrievedId){
        return !(retrievedClass.compare(expectedClass) || expectedProd.compare((*retrievalVsProd)[retrievedId]));
    }
    ExactRelevantCalculator(std::string expectedProduct, std::string expectedClass, std::map<std::string,
            std::string>* retrievalMap): RelevantConditionCalculator(expectedProduct, expectedClass, retrievalMap){};
};


class MeasureCalculatorRetrieval: public MeasureCalculator {
public:
    MeasureCalculatorRetrieval(const std::string &queriesFile, const std::string& classesFile); //Map query -> clase correcta
    float calculateMAP();
    std::vector<float> calculateAverageAccuracyVsRetrieved(int step, int retrievedNumber);
    std::map<std::string, std::vector<float>> calculateAccuracyVsRetrieved(int step, int retrievedNumber);
    std::map<std::string, std::vector<float>> calculatePrecisionVsRecall();
    std::map<std::string, std::vector<float>> calculateExactAccuracyVsRetrieved(int step, int retrievedNumber,
                                                                                const std::string &queriesProductsFile,
                                                                                const std::string &retrievalProductsFile);

private:
    std::map<std::string, std::string> queryList;
    std::map<std::string, int> classesSize;
    std::vector<std::string> keyListQueryList;
    double calculateAveragePrecision(std::string expectedClass, const std::vector<std::string>& retrievedClasses);
    std::vector<double> calculatePrecision(std::string expectedClass, const std::vector<std::string>& retrievedClasses);
    void correctlyRetrievedItemsByStep(int step, std::vector<float>& recallVector, RelevantConditionCalculator* calculator,
                                       const std::map<std::string, std::string>& retrievedElements, const std::vector<std::string>& retrievedCodes);
    std::vector<float> calculatePrecisionVsRecallForQuery(const std::string &query, const std::string &queryClass);
    std::map<std::string, std::vector<float>> genericAccuracyVsRetrieved(int step, int retrievedNumber,
                                                                         std::function<RelevantConditionCalculator* (std::string, std::string)> conditionBuilder);
};


#endif //WHERETOBUYITEVALUATION_MEASURECALCULATOR_HPP
