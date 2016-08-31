#include <iostream>
#include <fstream>
#include "json.hpp"
#include <thread>
#include <mutex>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

//for mmap (memory map)
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#define THREAD_NUMBER 8


using json = nlohmann::json;

std::string photoDirectory;
std::string finalDirectory;
std::map<std::string, std::string> photoMap;

void handle_error(const char* msg){
    perror(msg);
    exit(255);
}

const char* map_file(const char* filename, size_t& size){
    int fd = open(filename, O_RDONLY);
    if(fd == -1)
        handle_error("open");

    posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL);

    struct stat sb;
    if(fstat(fd, &sb) == -1)
        handle_error("fstat");

    size = sb.st_size;
    const char* addr = static_cast<const char *>(mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0u));
    if(addr == MAP_FAILED)
        handle_error("mmap");
    close(fd); 
    return addr;
}

void parseJson(std::string filename, json& resultContainer, std::string category,
               std::map<std::string, std::string> &outdoorImageMap){
    std::ifstream fileReader;
    fileReader.open(filename);
    json testJson = json::parse(fileReader);
    for(json& element : testJson){
        unsigned long photoId = element["photo"];
        std::string photoIdStr = std::to_string(photoId);
        while(photoIdStr.length() < 9)
            photoIdStr = "0"+photoIdStr;
        if(resultContainer.find(photoIdStr) == resultContainer.end())
            resultContainer[photoIdStr] = {};
        resultContainer[photoIdStr].push_back({{"bbox", element["bbox"]},
                                       {"category", category}});
        outdoorImageMap[photoIdStr] = photoMap[photoIdStr];


    }
    fileReader.close();
}

int main(int argc, char *argv[]) {

    std::string jsonDirectory = std::string(argv[1]);
    photoDirectory = std::string(argv[2]);
    finalDirectory = std::string(argv[3]);
    size_t size;
    char line[1000];
    const char* f = map_file("/home/calvarez/Datasets/DatasetStreet2Shop/photos/images.txt", size);
    const char* l = f+size;
    int count = 0;
    while(f && f!=l) {
        //if((f = static_cast<const char*>(memchr(f, '\n', l-f)))){
        if (*f == '\n') {
            line[count] = '\0';
            std::string stringLine= line;
            unsigned long tabLocation = stringLine.find('\t');
            std::string key = stringLine.substr(0, tabLocation);
            std::string value = stringLine.substr(tabLocation+1);
            photoMap[key] = value;
            count = 0;
        }
        else{
            line[count]=*f;
            count++;
        }
        f++;

        //}
    }


    std::vector<std::string> categories = {"bags", "belts", "dresses", "eyewear",
                                           "footwear", "hats", "leggings", "outerwear",
                                           "pants", "skirts", "tops"};
    std::string testingFiles = "test_pairs_";
    std::string trainingFiles = "train_pairs_";

    json imagesJson;
    std::map<std::string, std::string> outdoorImages;

    typedef std::vector<std::string>::iterator iterator;
    for (iterator it = categories.begin();  it!=categories.end() ; ++it){
        std::string category = *it;

        std::string testFile = jsonDirectory+testingFiles+category+".json";
        parseJson(testFile, imagesJson, category, outdoorImages);

        std::string trainFile = jsonDirectory+trainingFiles+category+".json";
        parseJson(trainFile, imagesJson, category, outdoorImages);
    }

    std::ofstream ss(finalDirectory+"outdoorImages.json");
    ss<<imagesJson.dump(4)<<std::endl;
    ss.close();
    return 0;
}
