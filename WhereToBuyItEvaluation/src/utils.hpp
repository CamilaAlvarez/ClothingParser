//
// Created by Camila Alvarez on 21-07-16.
//

#ifndef WHERETOBUYITEVALUATION_UTILS_HPP
#define WHERETOBUYITEVALUATION_UTILS_HPP

#include <iostream>
#include <map>

void handle_error(const char* msg);
const char* map_file(const char* filename, size_t& size);
std::map<std::string, std::string> loadFileToMap(const char *image_filename);
void writeToFile(const char *string, const char* filename, int filesize);

#endif //WHERETOBUYITEVALUATION_UTILS_HPP
