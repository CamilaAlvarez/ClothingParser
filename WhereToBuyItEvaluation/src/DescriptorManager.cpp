//
// Created by Camila Alvarez on 24-06-16.
//

#include "DescriptorManager.hpp"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <iostream>

static void handle_error(const char* msg){
    perror(msg);
    exit(255);
}

static const char* map_file(const char* filename, size_t& size){   
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

static std::map<std::string, std::string> loadFileToMap(const char *image_filename){
    size_t file_size;
    const char *file = map_file(image_filename, file_size);
    const char *end_file = file+file_size;
    char line[1000];
    int count = 0;

    std::map<std::string, std::string> image_map;
    while(file && file!=end_file){
        if(*file == '\n' || file+1==end_file){
            line[count] = '\0';
            std::string file_line = line;
            unsigned long tab_location = file_line.find('\t');
            std::string id = file_line.substr(0,tab_location);
            std::string image = file_line.substr(tab_location+1);
            image_map[id] = image;
	    count=0;
        }
        else{
            line[count++] = *file;
        }
        file++;
    }

    return image_map;
}


DescriptorManager::DescriptorManager(const std::string &config_file):network_config_file(ConfigFile(config_file, '\t')), predictor()
{
    JUtil::jmsr_assert(network_config_file.isDefined("CAFFE_DIR"), "Missing CAFFE_DIR parameter");
    JUtil::jmsr_assert(network_config_file.isDefined("PROTOTXT"), "Missing PROTOTXT parameter");
    JUtil::jmsr_assert(network_config_file.isDefined("CAFFE_MODEL"), "Missing CAFFE_MODEL parameter");
    JUtil::jmsr_assert(network_config_file.isDefined("LAYER_NAME"), "Missing LAYER_NAME parameter");
    JUtil::jmsr_assert(network_config_file.isDefined("IMAGE_WIDTH"), "Missing IMAGE_WIDTH parameter");
    JUtil::jmsr_assert(network_config_file.isDefined("IMAGE_HEIGHT"), "Missing IMAGE_HEIGHT parameter");
    JUtil::jmsr_assert(network_config_file.isDefined("CAFFE_MODE"), "Missing CAFFE_MODE parameter");

    desc_layer_name = network_config_file.getValue("LAYER_NAME");
    std::string dir = network_config_file.getValue("CAFFE_DIR");
    std::string prototxt = dir+network_config_file.getValue("PROTOTXT");
    std::string caffe_model = dir+network_config_file.getValue("CAFFE_MODEL");
    int image_width= std::stoi(network_config_file.getValue("IMAGE_WIDTH"));
    int image_height= std::stoi(network_config_file.getValue("IMAGE_HEIGHT"));
    int mode = network_config_file.getValue("CAFFE_MODE").compare("GPU") == 0 ? CAFFE_GPU_MODE : CAFFE_CPU_MODE;

    this->predictor.load(prototxt, caffe_model, image_width, image_height, mode);
    std::cout<<"LOADING OK!"<<std::endl;
}

void DescriptorManager::calculateDescriptors(const std::string &image_filename) {

    std::map<std::string, std::string> image_map = loadFileToMap(image_filename.c_str());
    typedef std::map<std::string, std::string>::iterator map_iter;
    for(map_iter iter = image_map.begin(); iter != image_map.end(); ++iter){
	std::string image = iter->second;
        try{
		std::cout<<image<<std::endl;
		float *desc = this->predictor.getCaffeDescriptor(image, &desc_size, desc_layer_name);
        	//To avoid memory leaks
		std::cout<<"CALCULATED DESCRIPTOR FOR: "<<image<<std::endl;
        	if(descriptor_map.find(iter->first)!=descriptor_map.end())
            		delete descriptor_map[iter->first];

        	descriptor_map[iter->first] = desc;
	}
	catch(const std::runtime_error &e){
		std::cout<<"INVALID IMAGE: "<<image<<std::endl;
		invalid_image_map[iter->first] = image;
	}
    }

}

void DescriptorManager::saveDescriptors(const std::string &output) {
    typedef std::map<std::string, float *>::iterator map_iter;
    std::vector<float> values;
    values.push_back(desc_size);
    for (map_iter iter = descriptor_map.begin(); iter != descriptor_map.end(); ++iter) {
        values.push_back(std::stof(iter->first));
        for (int i = 0; i < desc_size; i++)
            values.push_back(iter->second[i]);
    }
    std::ofstream output_file(output);
    output_file.write(reinterpret_cast<char *>(values.size()), sizeof(float));
    output_file.write(reinterpret_cast<char *>(&values[0]), values.size()* sizeof(float));
    output_file.close();

   std::ofstream invalid_imgs("failed_images.txt");
   for(std::map<std::string, std::string>::iterator it = invalid_image_map.begin(); 
	it!=invalid_image_map.end(); ++it){
	invalid_imgs<<it->first<<'\t'<<it->second<<std::endl;
    }
    invalid_imgs.close();
}

DescriptorManager::~DescriptorManager() {
    typedef std::map<std::string, float *>::iterator map_iter;

    for(map_iter iter= descriptor_map.begin(); iter!=descriptor_map.end(); ++iter)
        delete iter->second;
}
