//
// Created by Camila Alvarez on 21-07-16.
//

#include "utils.hpp"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <cstring>

std::string getStringFromColumn(const std::string& columnString, int column){
    std::string line = columnString;
    int localColumn = column;
    unsigned long tab_location;
    for(;;){
        tab_location = line.find('\t');
        if(localColumn == 1){
            if(tab_location == std::string::npos)
                tab_location = line.length()+1;
            line = line.substr(0, tab_location);
            break;
        }
        line = line.substr(tab_location + 1);
        localColumn--;
    }
    return line;
}

void handle_error(const char* msg){
    perror(msg);
    exit(255);
}

template <typename T>
T map_file(const char* filename, size_t& size){
    int fd = open(filename, O_RDONLY);
    if(fd == -1)
        handle_error("open");

    posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL);

    struct stat sb;
    if(fstat(fd, &sb) == -1)
        handle_error("fstat");

    size = sb.st_size;
    T addr = static_cast<T>(mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0u));
    if(addr == MAP_FAILED)
        handle_error("mmap");
    close(fd);
    return addr;
}

//Takes first two columns
std::map<std::string, std::string> loadFileToMap(const char *image_filename, int key_column, int value_column){
    size_t file_size;
    const char *file = map_file<const char*>(image_filename, file_size);
    const char* aux = file;
    const char *end_file = file+file_size;
    char line[1000];
    int count = 0;

    std::map<std::string, std::string> image_map;
    while(file && file!=end_file){
        if(*file == '\n' || file+1==end_file){
            line[count] = '\0';
            std::string file_line = line;
            if(file_line.compare("")==0){
            	file++;
		count=0;
		continue;
	    }
            std::string id = getStringFromColumn(file_line, key_column);
            std::string image = getStringFromColumn(file_line, value_column);
            image_map[id] = image;
            count=0;
        }
        else{
            line[count++] = *file;
        }
        file++;
    }

    if (munmap(const_cast<char *>(aux), file_size) == -1) {
        perror("Error un-mmapping the file");
        exit(-1);
    }

    return image_map;
}

std::map<std::string,float *> loadFileToFloatMap(const char *filename, int* descSize){
    size_t file_size;
    const float *file = map_file<const float*>(filename, file_size);
    const float* aux = file;
    const float *end_file = file+(file_size/sizeof(float));
    *descSize = (int)*file;
    file++;    
    std::map<std::string, float*> floatMap;
    while(file && file!=end_file){
	float id = *file;
	file++;
	float *desc = new float[*descSize];
	std::memcpy(desc, file, *descSize);
        file+=*descSize;
	floatMap[std::to_string((int)id)] = desc;
    }

    if (munmap(const_cast<float *>(aux), file_size) == -1) {
        perror("Error un-mmapping the file");
        exit(-2);
    }

    return floatMap;
}

std::vector<std::string> loadQueryFileToVector(const char* filename, int column){
    size_t file_size;
    const char *file = map_file<const char*>(filename, file_size);
    const char* aux = file;
    const char *end_file = file+file_size;
    char line[1000];
    int count = 0;

    std::vector<std::string> classVector;
    while(file && file!=end_file){
        if(*file == '\n' || file+1==end_file){
            line[count] = '\0';
            std::string file_line = line;
            if(file_line.compare("")==0){
                file++;
                count=0;
                continue;
            }

            std::string columnString = getStringFromColumn(file_line, column);
            classVector.push_back(columnString);
            count=0;
        }
        else{
            line[count++] = *file;
        }
        file++;
    }

    if (munmap(const_cast<char *>(aux), file_size) == -1) {
        perror("Error un-mmapping the file");
        exit(-1);
    }

    return classVector;
}

void writeToFile(const char *string, const char* filename, int filesize){
    int i;
    int fd;
    int result;
    char *map;  /* mmapped array of char's */

    /* Open a file for writing.
     *  - Creating the file if it doesn't exist.
     *  - Truncating it to 0 size if it already exists. (not really needed)
     *
     * Note: "O_WRONLY" mode is not sufficient when mmaping.
     */
    fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
    if (fd == -1) {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }

    /* Stretch the file size to the size of the (mmapped) array of ints
     */
    result = lseek(fd, filesize-1, SEEK_SET);
    if (result == -1) {
        close(fd);
        perror("Error calling lseek() to 'stretch' the file");
        exit(EXIT_FAILURE);
    }

    /* Something needs to be written at the end of the file to
     * have the file actually have the new size.
     * Just writing an empty string at the current file position will do.
     *
     * Note:
     *  - The current position in the file is at the end of the stretched
     *    file due to the call to lseek().
     *  - An empty string is actually a single '\0' character, so a zero-byte
     *    will be written at the last byte of the file.
     */
    result = write(fd, "", 1);
    if (result != 1) {
        close(fd);
        perror("Error writing last byte of the file");
        exit(EXIT_FAILURE);
    }

    /* Now the file is ready to be mmapped.
     */
    map = (char *)mmap(0, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        close(fd);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }

    /* Now write int's to the file as if it were memory (an array of ints).
     */
    for (i = 0; i <filesize; ++i) {
        map[i] = string[i];
    }

    /* Don't forget to free the mmapped memory
     */
    if (munmap(map, filesize) == -1) {
        perror("Error un-mmapping the file");
        close(fd);
        /* Decide here whether to close(fd) and exit() or not. Depends... */
    }

    /* Un-mmaping doesn't close the file, so we still need to do that.
     */
    close(fd);
}

std::ostream& operator<<(std::ostream& os, const std::vector<float>& array){
    std::string s = "";
    for(std::vector<float>::const_iterator it = array.begin(); it!=array.end(); ++it){
        std::string aux = "\t"+std::to_string(*it);
        s += aux;
    }
    os<<s<<std::endl;
    return os;
}

