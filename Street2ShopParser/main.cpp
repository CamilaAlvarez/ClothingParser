#include <iostream>
#include <fstream>
#include "json.hpp"
#include <thread>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#define THREAD_NUMBER 10


using json = nlohmann::json;

std::string photoDirectory;
std::string finalDirectory;

std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    return result;
}

void retrievalImgsFunction(json* retrievalJson, std::mutex* m){
    m->lock();
    while(retrievalJson->size()>0){
        json element = (*retrievalJson)[0];
        unsigned long photo = element["photo"];
        unsigned long product = element["product"];
        std::string category = element["category"];
        retrievalJson->erase(0);
        m->unlock();

        std::string nameCmd = "ls "+photoDirectory+" | grep "+std::to_string(photo)+"$";
        std::string name = exec(nameCmd.c_str());
        std::string cmd = "cp" +photoDirectory+name+" "+ finalDirectory+"retrieval/"+category+"/"+std::to_string(product)
                      +"-"+name;
        exec(cmd.c_str());
        m->lock();
    }
    m->unlock();
}

void cropPhotoFunction(json* contentJson,  std::string type, std::mutex* m){
    m->lock();
    while(contentJson->size()>0){
        json::iterator keyValue = contentJson->begin();
        std::string photo = keyValue.key();
        json value = keyValue.value();
        contentJson->erase(photo);
        m->unlock();

        std::string nameCmd = "ls "+photoDirectory+" | grep "+photo+"$";
        std::string name = exec(nameCmd.c_str());
        std::string imgname = photoDirectory+name;
        cv::Mat img = cv::imread(imgname);

        for(json& box : value){
            json bbox = box["bbox"];
            std::string category = box["category"];
            unsigned long product = box["product"];
            int top = bbox["top"];
            int left = bbox["left"];
            int width = bbox["width"];
            int height = bbox["height"];
            cv::Rect rect(left, top, width, height);
            cv::Mat productImg = img(rect);

            std::string boxString = "["+std::to_string(left)+","+std::to_string(top)+
                    ","+std::to_string(width)+","+std::to_string(height)+"]";
            std::string filename = finalDirectory+type+"/"+category+"/"+std::to_string(product)
                                   +"-"+boxString+"-"+name;
            cv::imwrite(filename, productImg);
        }
        m->lock();
    }
    m->unlock();
}

void parseJson(std::string filename, json& resultContainer, std::string category){
    std::ifstream fileReader;
    fileReader.open(filename);
    json testJson = json::parse(fileReader);

    for(json& element : testJson){
        unsigned long photoId = element["photo"];
        std::string photoIdStr = std::to_string(photoId);
        if(resultContainer.find(photoIdStr) == resultContainer.end())
            resultContainer[photoIdStr] = {};
        resultContainer[photoIdStr].push_back({{"product", element["product"]},
                                       {"bbox", element["bbox"]},
                                       {"category", category}});

    }
    fileReader.close();
}

int main(int argc, char *argv[]) {

    std::string jsonDirectory = std::string(argv[1]);
    photoDirectory = std::string(argv[2]);
    finalDirectory = std::string(argv[3]);
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
        std::string category = *it;
        std::string retrievalFile = jsonDirectory+retrievalFiles+category+".json";

        std::cout<<retrievalFile<<std::endl;
        fileReader.open(retrievalFile);
        json retrievalJson = json::parse(fileReader);

        for(json& element : retrievalJson){
            element["category"] = category;
            retrieval.push_back(element);
        }
        fileReader.close();

        std::string testFile = jsonDirectory+testingFiles+category+".json";
        parseJson(testFile, testing, category);

        std::string trainFile = jsonDirectory+trainingFiles+category+".json";
        parseJson(trainFile, training, category);
    }
    //thread pointers, because threads are not copyable
    std::vector<std::thread*> threads;
    std::mutex retrievalMutex;
    std::mutex testingMutex;
    std::mutex trainingMutex;
    std::thread thread;
    for (int i = 0; i < THREAD_NUMBER; i++) {
        if(i<2){
            thread = std::thread(retrievalImgsFunction, &retrieval, &retrievalMutex);
        }
        else if(i<6){
            thread = std::thread(cropPhotoFunction, &testing,"testing", &testingMutex);
        }
        else{
            thread = std::thread(cropPhotoFunction, &training,"training", &trainingMutex);
        }
        threads.push_back(&thread);

    }

    for(std::thread*& t : threads){
        (*t).join();
    }
    return 0;
}