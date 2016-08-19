//
// Created by Camila Alvarez on 21-07-16.
//

#ifndef WHERETOBUYITEVALUATION_UTILS_HPP
#define WHERETOBUYITEVALUATION_UTILS_HPP

#include <iostream>
#include <map>
#include <vector>

void handle_error(const char* msg);
template <typename T>
T map_file(const char* filename, size_t& size);
std::map<std::string, std::string> loadFileToMap(const char *image_filename, int key_column = 1, int value_column = 2);
void writeToFile(const char *string, const char* filename, int filesize);
std::map<std::string,float*> loadFileToFloatMap(const char *filename, int *descSize);
std::vector<std::string> loadQueryFileToVector(const char* filename, int column = 2);
std::ostream& operator<<(std::ostream& os, const std::vector<float>& array);
#endif //WHERETOBUYITEVALUATION_UTILS_HPP
