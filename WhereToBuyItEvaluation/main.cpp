#include <iostream>
#include "ExperimentConfigurator.hpp"
#include <exception>

class missingArgument : public std::exception{
    virtual const char * what() const throw(){
        return "Missing argument";
    }
}missingArgumentException;

int main(int argc, char* argv[]) {
    std::vector<std::string> args;
    for(int i=1; i<argc; i++)
        args.push_back(argv[i]);

    std::vector<std::string>::iterator it;
    if((it = std::find(args.begin(), args.end(), "-config_file")) == args.end()){
        throw missingArgumentException;
    }
    if((it+1) == args.end())
        throw missingArgumentException;

    std::string configFile = *(it+1);
    ExperimentConfigurator configurator(configFile);
    configurator.executeExperiment();

    return 0;
}
