//
// Created by Camila Alvarez on 02-08-16.
//

#ifndef WHERETOBUYITEVALUATION_EXPERIMENTCONFIGURATOR_HPP
#define WHERETOBUYITEVALUATION_EXPERIMENTCONFIGURATOR_HPP

#include <jmsr/JUtil.h>
#include <vector>
#include <string>

class ExperimentConfigurator {
public:
    ExperimentConfigurator(const std::string& configFile);
    void executeExperiment();

private:
    ConfigFile configuration;
};


#endif //WHERETOBUYITEVALUATION_EXPERIMENTCONFIGURATOR_HPP
