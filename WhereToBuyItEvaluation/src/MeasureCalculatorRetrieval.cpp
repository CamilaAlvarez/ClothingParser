//
// Created by Camila Alvarez on 05-08-16.
//

#include "MeasureCalculatorRetrieval.hpp"
#include "utils.hpp"
#include <math.h>
#include <string>
#ifdef _OPENMP
#include <omp.h>
#endif

MeasureCalculatorRetrieval::MeasureCalculatorRetrieval(const std::string &queriesFile, const std::string& classesFile ) {
    queryList = loadFileToMap(queriesFile.c_str());
    std::map<std::string, std::string> classes = loadFileToMap(classesFile.c_str());

    for(std::map<std::string, std::string>::iterator it = queryList.begin(); it!=queryList.end(); ++it ){
	    keyListQueryList.push_back(it->first);
    }

    for(std::map<std::string, std::string>::iterator it = classes.begin(); it!=classes.end(); ++it )
        classesSize[it->first] = std::stoi(it->second);
}

float MeasureCalculatorRetrieval::calculateMAP() {
    double accumulator = 0;
    #ifdef _OPENMP
    omp_lock_t lock;
	omp_init_lock(&lock);
    #endif
    #pragma omp parallel for
    for(int i = 0; i < keyListQueryList.size(); i++){
        std::string query = keyListQueryList[i];
        std::string queryClass = queryList[query];
        std::vector<std::string> retrievedClasses = loadQueryFileToVector(query.c_str(), 3);
        double aux = calculateAveragePrecision(queryClass, retrievedClasses);
        #ifdef _OPENMP
        omp_set_lock(&lock);
        #endif
        accumulator += aux;
        #ifdef _OPENMP
        omp_unset_lock(&lock);
        #endif
    }
    #ifdef _OPENMP
    omp_destroy_lock(&lock);
    #endif
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


std::map<std::string, std::vector<float>> MeasureCalculatorRetrieval::calculateAccuracyVsRetrieved(int step, int retrievedNumber){
    std::function<RelevantConditionCalculator* (std::string, std::string)> condCalculator = [](std::string query,
                                                                                               std::string queryClass)->RelevantConditionCalculator*{
        RelevantClassCalculator* conditionCalculator = new RelevantClassCalculator(queryClass);
        return conditionCalculator;
    };

    return genericAccuracyVsRetrieved(step, retrievedNumber, condCalculator);
}

void MeasureCalculatorRetrieval::correctlyRetrievedItemsByStep(int step, std::vector<float>& recallVector, RelevantConditionCalculator* condition,
                                                      const std::map<std::string, std::string>& retrievedElements, const std::vector<std::string>& retrievedCodes) {
    int index = 0;
    int resultNumber = 0;
    int accumulator = 0;
    int old_index = -1;
    for (std::vector<std::string>::const_iterator it = retrievedCodes.begin(); it!=retrievedCodes.end(); ++it) {
        std::string retrievedCode = *it;
	std::string retrievedClass = retrievedElements.at(retrievedCode);
        std::string expectedClass = condition->getExcpectedClass();
        if(condition->isRelevant(retrievedClass, retrievedCode))
            accumulator++;
        if(!retrievedClass.compare(expectedClass))
            index++;
        if(index%step==0 && index!=old_index){
	    old_index = index;
            recallVector[resultNumber] += accumulator>0;
            resultNumber++;
        }
    }
    if(index%step!=0)
        recallVector[resultNumber] += accumulator>0;
}

std::map<std::string, std::vector<float>> MeasureCalculatorRetrieval::calculatePrecisionVsRecall(){
    std::map<std::string, std::vector<float>> precisionRecallMap;
    #ifdef _OPENMP
    omp_lock_t lock;
	omp_init_lock(&lock);
    #endif
    #pragma omp parallel for
    for(int i = 0; i < keyListQueryList.size(); i++){
        std::string query = keyListQueryList[i];
        std::string queryClass = queryList[query];
        std::vector<float> precisionRecallQuery = calculatePrecisionVsRecallForQuery(query, queryClass);
    #ifdef _OPENMP
        omp_set_lock(&lock);
    #endif
        precisionRecallMap[query] = precisionRecallQuery;
    #ifdef _OPENMP
        omp_unset_lock(&lock);
    #endif

    }
    #ifdef _OPENMP
    omp_destroy_lock(&lock);
    #endif
    return precisionRecallMap;
}

std::vector<float> MeasureCalculatorRetrieval::calculatePrecisionVsRecallForQuery(const std::string &query, const std::string &queryClass) {
    std::map<int, float> retrievedVsPrecision;
    std::vector<std::string> retrievedClasses = loadQueryFileToVector(query.c_str(), 3);
    int relevant = 0;
    for(int i = 0; i < retrievedClasses.size(); i++){
        if(!queryClass.compare(retrievedClasses[i])){
            relevant++;
            retrievedVsPrecision[relevant] = (float)relevant/(float)i;
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

std::map<std::string, std::vector<float>> MeasureCalculatorRetrieval::calculateExactAccuracyVsRetrieved(int step,
                                                                                                        int retrievedNumber,
                                                                                                        const std::string &queriesProductsFile,
                                                                                                        const std::string &retrievalProductsFile){
    std::map<std::string, std::string> queryVsProducts = loadFileToMap(queriesProductsFile.c_str(), 1, 3);
    std::map<std::string, std::string> retrievalVsProducts = loadFileToMap(retrievalProductsFile.c_str(), 1 , 3);
    std::function<RelevantConditionCalculator* (std::string, std::string)> condBuilder = [&](std::string query,
                                                                                             std::string queryClass)-> RelevantConditionCalculator* {
        std::size_t start = query.find_last_of("/");
        std::size_t end = query.find_last_of(".");
        std::string code = query.substr(start+1, end-start-1);
        ExactRelevantCalculator* conditionCalculator = new ExactRelevantCalculator(queryVsProducts[code], queryClass,
                                                                                  &retrievalVsProducts);
        return conditionCalculator;
    };
    std::cout<<"CALCULATING EXACT ACCURACY VS RETRIEVAL"<<std::endl;
    return genericAccuracyVsRetrieved(step, retrievedNumber, condBuilder);
};

std::map<std::string, std::vector<float>> MeasureCalculatorRetrieval::genericAccuracyVsRetrieved(int step, int retrievedNumber,
                                                                     std::function<RelevantConditionCalculator* (std::string, std::string)> conditionBuilder){
    std::map<std::string, std::vector<float>> accuracyVsRetrievedByClass;
    int stepNumber = ceil((double)retrievedNumber/(double)step);
    std::map<std::string, int> queriesClasses;
    int count = 0;
    accuracyVsRetrievedByClass["average"] = std::vector<float>(stepNumber,0);
    queriesClasses["average"] = 0;
#ifdef _OPENMP
    omp_lock_t lock;
	omp_init_lock(&lock);
#endif
#pragma omp parallel for
    for(int i = 0; i < keyListQueryList.size(); i++ ){
        std::string query = keyListQueryList[i];
        std::string queryClass = queryList[query];
        std::map<std::string, std::string> retrievedCodeClasses = loadFileToMap(query.c_str(), 2, 3);
        std::vector<std::string> retrievedCodes = loadQueryFileToVector(query.c_str());
        std::vector<float> auxVector(stepNumber, 0);
        RelevantConditionCalculator* conditionCalculator = conditionBuilder(query, queryClass);
        correctlyRetrievedItemsByStep(step, auxVector, conditionCalculator, retrievedCodeClasses, retrievedCodes);
        delete conditionCalculator;
#ifdef _OPENMP
        omp_set_lock(&lock);
#endif
        if(count%100==0){
            std::cout<<"PROCESSED: "<<count<<std::endl;
        }
        if(accuracyVsRetrievedByClass.find(queryClass) == accuracyVsRetrievedByClass.end())
            accuracyVsRetrievedByClass[queryClass] = std::vector<float>(stepNumber, 0);
        if(queriesClasses.find(queryClass) == queriesClasses.end())
            queriesClasses[queryClass] = 0;
        queriesClasses[queryClass]++;
        queriesClasses["average"]++;
        count++;
        for(int j = 0; j < auxVector.size(); j++) {
            accuracyVsRetrievedByClass[queryClass][j] += auxVector[j];
            accuracyVsRetrievedByClass["average"][j] += auxVector[j];
        }
#ifdef _OPENMP
        omp_unset_lock(&lock);
#endif
    }
#ifdef _OPENMP
    omp_destroy_lock(&lock);
#endif
    for(std::map<std::string, int>::iterator it = queriesClasses.begin(); it!=queriesClasses.end(); ++it){
        for(int i = 0; i < accuracyVsRetrievedByClass[it->first].size(); i++){
            accuracyVsRetrievedByClass[it->first][i] /= it->second;
        }
    }

    return accuracyVsRetrievedByClass;
}
