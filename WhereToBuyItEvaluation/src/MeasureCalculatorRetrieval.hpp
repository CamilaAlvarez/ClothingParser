//
// Created by Camila Alvarez on 05-08-16.
//

#ifndef WHERETOBUYITEVALUATION_MEASURECALCULATORRETRIEVAL_HPP
#define WHERETOBUYITEVALUATION_MEASURECALCULATORRETRIEVAL_HPP

#include "MeasureCalculator.hpp"
#include <iostream>
#include <vector>
#include <map>
 
class MeasureCalculatorRetrieval: public MeasureCalculator {
public:
    MeasureCalculatorRetrieval(const std::string &queriesFile, const std::string& classesFile); //Map query -> clase correcta
    float calculateMAP();
    std::vector<float> calculateAverageAccuracyVsRetrieved(int step, int retrievedNumber);
    std::map<std::string, std::vector<float>> calculateAccuracyVsRetrieved(int step, int retrievedNumber);
    std::map<std::string, std::vector<float>> calculatePrecisionVsRecall();

private:
    std::map<std::string, std::string> queryList;
    std::map<std::string, std::string> classesSize;
    std::vector<std::string> keyListQueryList;
    double calculateAveragePrecision(std::string expectedClass, const std::vector<std::string>& retrievedClasses);
    std::vector<double> calculatePrecision(std::string expectedClass, const std::vector<std::string>& retrievedClasses);
    void correctlyRetrievedItemsByStep(int step, std::vector<float>& recallVector, std::string expectedClass,
                                        const std::vector<std::string>& retrievedClasses);
    std::vector<float> calculatePrecisionVsRecallForQuery(const std::string &query, const std::string &queryClass);
};


#endif //WHERETOBUYITEVALUATION_MEASURECALCULATOR_HPP
