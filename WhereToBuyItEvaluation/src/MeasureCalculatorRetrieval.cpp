//
// Created by Camila Alvarez on 05-08-16.
//

#include "MeasureCalculatorRetrieval.hpp"
#include "utils.hpp"
#include <math.h>

MeasureCalculatorRetrieval::MeasureCalculatorRetrieval(const std::string &queriesFile) {
    queryList = loadFileToMap(queriesFile);
}

float MeasureCalculatorRetrieval::calculateMAP() {
    double accumulator = 0;
    for(std::map<std::string, std::string>::iterator it = queryList.begin(); it!=queryList.end(); ++it){
        std::string queryClass = it->second;
        std::vector<std::string> retrievedClasses = loadQueryFileToVector(it->first.c_str());
        accumulator += calculateAveragePrecision(queryClass, retrievedClasses);
    }

    double map = accumulator/queryList.size();
    return (float)map;
}

double MeasureCalculatorRetrieval::calculateAveragePrecision(std::string expectedClass,
                                                    const std::vector<std::string>& retrievedClasses) {
    double accumulator = 0;
    std::vector<double> precisionVector = calculatePrecision(expectedClass, retrievedClasses);
    for(std::vector<double>::iterator it=precisionVector.begin(); it!=precisionVector.end(); ++it){
        accumulator += *it;
    }
    return accumulator/precisionVector.size();
}

std::vector<double> MeasureCalculatorRetrieval::calculatePrecision(std::string expectedClass,
                                                          const std::vector<std::string>& retrievedClasses) {
    double relevantItems = 0;

    std::vector<double> precisionVector;

    for(int i = 0; i < retrievedClasses.size(); i++){
        if(!expectedClass.compare(retrievedClasses[i]))
            relevantItems++;
        double precision = relevantItems/(i+1);
        precisionVector.push_back(precision);
    }
    return precisionVector;
}

std::vector<float> MeasureCalculatorRetrieval::calculateAverageAccuracyVsRetrieved(int step, int retrievedNumber) {

    std::vector<float> recallVsRetrieved(ceil((double)retrievedNumber/(double)step), 0);
    for(std::map<std::string, std::string>::iterator it = queryList.begin(); it!=queryList.end(); ++it){
        std::string queryClass = it->second;
        std::vector<std::string> retrievedClasses = loadQueryFileToVector(it->first.c_str());
        correctlyRetrievedItemsByStep(step, recallVsRetrieved, queryClass, retrievedClasses);
    }

    for(int i = 0; i < recallVsRetrieved.size(); i++){
        recallVsRetrieved[i] = recallVsRetrieved/queryList.size();
    }

    return recallVsRetrieved;

}

std::map<std::string, std::vector<float>> MeasureCalculatorRetrieval::calculateAccuracyVsRetrieved(int step, int retrievedNumber){
    std::map<std::string, std::vector<float>> accuracyVsRetrievedByClass;
    std::map<std::string, int> classSize;
    int stepNumber = ceil((double)retrievedNumber/(double)step);
    for(std::map<std::string, std::string>::iterator it = queryList.begin(); it!=queryList.end(); ++it){
        std::string queryClass = it->second;
        if(accuracyVsRetrievedByClass[queryClass] == accuracyVsRetrievedByClass.end())
            accuracyVsRetrievedByClass[queryClass] = std::vector<float>(stepNumber, 0);
        if(classSize[queryClass] == classSize.end())
            classSize[queryClass] = 0;
        classSize[queryClass]++;
        std::vector<std::string> retrievedClasses = loadQueryFileToVector(it->first.c_str());
        correctlyRetrievedItemsByStep(step, accuaracyVsRetrievedByClass[queryClass], queryClass, retrievedClasses);
    }

    for(std::map<std::string, int>::iterator it = classSize.begin(); it!=classSize.end(); ++it){
        for(int i = 0; i < accuracyVsRetrievedByClass[it->first].size(); i++){
            accuracyVsRetrievedByClass[it->first][i] /= it->second;
        }
    }

    return accuracyVsRetrievedByClass;
}

void MeasureCalculatorRetrieval::correctlyRetrievedItemsByStep(int step, std::vector<float>& recallVector, std::string expectedClass,
                                                      const std::vector<std::string>& retrievedClasses) {
    int index = 0;
    int resultNumber = 0;
    int accumulator = 0;
    for (std::vector<std::string>::const_iterator it = retrievedClasses.begin(); it!=retrievedClasses.end(); ++it) {
        if(!expectedClass.compare(*it))
            accumulator++;
        index++;
        if(index%step==0){
            recallVector[resultNumber] += accumulator>0;
            accumulator = 0;
            resultNumber++;
        }
    }
    if(index%step!=0)
        recallVector[resultNumber] += accumulator>0;
}

std::map<std::string, std::vector<float>> MeasureCalculatorRetrieval::calculatePrecisionVsRecall(){
    std::map<std::string, std::vector<float>> precisionRecallMap;
    for(std::map<std::string, std::string>::iterator it = queryList.begin(); it != queryList.end(); ++it){
        std::string query = it->first;
        std::string queryClass = it->second;
        std::vector<float> precisionRecallQuery = calculatePrecisionVsRecallForQuery(query, queryClass);
        precisionRecallMap[query] = precisionRecallQuery;
    }

    return precisionRecallMap;
}

std::vector<float> MeasureCalculatorRetrieval::calculatePrecisionVsRecallForQuery(const std::string &query, const std::string &queryClass) {
    std::map<int, float> retrievedVsPrecision;
    std::vector<std::string> retrievedClasses = loadQueryFileToVector(query.c_str());
    int relevant = 0;
    for(int i = 0; i < retrievedClasses.size(); i++){
        if(!queryClass.compare(retrievedClasses[i])){
            relevant++;
            retrievedVsRelevant[relevant] = (float)relevant/(float)i;
        }
    }

    std::vector<float> precisionRecall = std::vector<float>(11, 0);
    float currentMax = 0;
    int index = 9;
    int currentRetrieval = 0.9;
    int relevantNumber = 1;
    bool first = true;

    for(std::map<int, float>::reverse_iterator it=retrievedVsPrecision.rbegin(); it != retrievedVsPrecision.rend(); ++it){
        if(first) {
            precisionRecall[index] = it->second;
            relevantNumber = it->first;
            first = false;
            currentMax = it->second;
            continue;
        }
        float retrieval = (float)it->first/relevantNumber;

        if(retrieval < currentRetrieval){
            precisionRecall[index] = currentMax;
            currentRetrieval -= 0.1;
            index--;
        }
        currentMax = std::max(currentMax, it->second);
    }
    //The first value shouldn't be zero. If it is it means that it wasn't filled with any value
    if(precisionRecall[0] == 0)
        precisionRecall[0] = currentMax;

    return precisionRecall;
}