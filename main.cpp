#include <iostream>
#include "PointNodeStructure.cpp"

using  namespace std;

int main() {
    std::cout << "Hello, World!" << std::endl;

    PointNodeStructure *pointNodeStructure = new PointNodeStructure;

    pointNodeStructure->pointLabel = 1;
    pointNodeStructure->pointX = 1;

    cout<<pointNodeStructure->pointX<<endl;


    return 0;
}