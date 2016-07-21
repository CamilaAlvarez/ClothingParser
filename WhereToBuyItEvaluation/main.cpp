#include <iostream>
#include <DescriptorManager.hpp>

using namespace std;

int main() {
    DescriptorManager d("Experimentfc7.config");
    d.calculateDescriptors("images_ids.txt");
    d.saveDescriptors("wheretobuydescfc7.bin");
    return 0;
}
