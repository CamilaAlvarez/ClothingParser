//
// Created by Camila Alvarez on 21-07-16.
//

#include "utils.hpp"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

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

std::map<std::string, std::string> loadFileToMap(const char *image_filename){
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
            if(file_line.compare("")==0){
            	file++;
		count=0;
		continue;
	    }
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
    for (i = 1; i <=filesize; ++i) {
        map[i] = string[i];
    }

    /* Don't forget to free the mmapped memory
     */
    if (munmap(map, filesize) == -1) {
        perror("Error un-mmapping the file");
        /* Decide here whether to close(fd) and exit() or not. Depends... */
    }

    /* Un-mmaping doesn't close the file, so we still need to do that.
     */
    close(fd);
}
