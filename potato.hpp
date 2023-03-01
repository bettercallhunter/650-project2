#include <cstdlib>
#include <vector>
class Potato {
   public:
    int hops;
    int path[512];
    int index;
    // std::vector<int> trace;

   public:
    Potato() {}
    Potato(int num) : hops(num), index(0) {}
    void printPath() {
        std::cout << "Trace of potato:" << std::endl;
        for (int i = 0; i < index - 1; i++) {
            std::cout << path[i] << ",";
        }
        std::cout << path[index - 1] << std::endl;
    }
};