#include <iostream>
#include <DescriptorManager.hpp>

using namespace std;

int main() {
    DescriptorManager d("Expriment.config");
    d.calculateDescriptors("all_images.txt");
    d.saveDescriptors("wheretobuydesc.bin");
    return 0;
}