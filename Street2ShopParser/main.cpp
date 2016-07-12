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
	retrievalJson->erase(retrievalJson->begin());
	m->unlock();
	std::string photoNumber = std::to_string(photo);
	while(photoNumber.length()<9){
		photoNumber = "0"+photoNumber;
	}
        //std::string nameCmd = "ls "+photoDirectory+" | grep "+photoNumber+"$ | tr -d \"\n\" ";
        std::string name = photoMap[photoNumber];
        std::string cmd = "cp '" +name+"' '"+ finalDirectory+"retrieval/"+category+"/"+photoNumber+".jpg?"+std::to_string(product)+"'";
       	exec(cmd.c_str());
	    std::cout<<"name: "<<name<<std::endl;
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
	while(photo.length()<9){
		photo = "0"+photo;
	}
        
        //std::string nameCmd = "ls "+photoDirectory+" | grep "+photo+"$  | tr -d \"\n\"";
        std::string name = photoMap[photo];
        //std::string imgname = photoDirectory+name;
        cv::Mat img = cv::imread(name);
	if(!img.data){
		std::cout<<"corrupted image: "<<name<<std::endl;
		m->lock();
		continue;
	}
        for(json& box : value){
            json bbox = box["bbox"];
            std::string category = box["category"];
            unsigned long product = box["product"];
            int top = bbox["top"];
            int left = bbox["left"];
            int width = bbox["width"];
            int height = bbox["height"];
	    if(top+height > img.rows)
		height = img.rows - top;
	    if(left+width > img.cols)
		width = img.cols -left;
            cv::Rect rect(left, top, width, height);
	    std::cout<<"name: "<<name<<std::endl;
            cv::Mat productImg = img(rect);

            std::string boxString = "["+std::to_string(left)+","+std::to_string(top)+
                    ","+std::to_string(width)+","+std::to_string(height)+"]";
            std::string filename = finalDirectory+type+"/"+category+"/"+boxString+"-"+photo+".jpg?"+std::to_string(product);
            cv::imwrite(filename, productImg);    	
	    //    std::cout<<"name: "<<filename<<std::endl;
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
    std::vector<std::thread> threads;
    std::mutex retrievalMutex;
    std::mutex testingMutex;
    std::mutex trainingMutex;
    std::thread thread;
    for (int i = 0; i < THREAD_NUMBER; i++) {
        if(i<2){
            threads.push_back(std::thread(retrievalImgsFunction, &retrieval, &retrievalMutex));
        }
        else if(i<5){
            threads.push_back(std::thread(cropPhotoFunction, &testing,"testing", &testingMutex));
        }
        else{
            threads.push_back( std::thread(cropPhotoFunction, &training,"training", &trainingMutex));
        }

    }

    for(std::thread& t : threads){
        t.join();
    }
    return 0;
}
