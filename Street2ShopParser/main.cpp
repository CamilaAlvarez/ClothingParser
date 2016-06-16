
#include <iostream>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;

int main(int argc, char *argv[]) {

    std::string jsonDirectory = std::string(argv[1]);
    std::string photoFileDirectory = std::string(argv[2]);
    std::vector<std::string> categories = {"bags", "belts", "dresses", "eyewear",
                                           "footwear", "hats", "leggings", "outerwear",
                                           "pants", "skirts", "tops"};
    std::string retrievalFiles = "retrieval_";
    std::string testingFiles = "test_pairs_";
    std::string trainingFiles = "train_pairs_";

    json retrieval;
    json testing;
    json training;

    typedef std::vector<std::string>::iterator iterator;
    for (iterator it = categories.begin();  it!=categories.end() ; ++it){
        std::ifstream fileReader;
        std::string retrievalFile = jsonDirectory+retrievalFiles+(*it)+".json";
        std::cout<<retrievalFile<<std::endl;
        fileReader.open(retrievalFile);
        json retrievalJson = json::parse(fileReader);
        std::cout<<retrievalJson[0]<<std::endl;
        std::string testFile = jsonDirectory+testingFiles+(*it)+".json";
        std::string trainFile = jsonDirectory+trainingFiles+(*it)+".json";

    }


    std::cout << "hola" << std::endl;
    return 0;
}