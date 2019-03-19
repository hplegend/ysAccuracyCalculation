#include <iostream>
#include "FileReader.h"

using  namespace std;

int main() {
    std::cout << "Hello, World!" << std::endl;
    FileReader *fileReader = new FileReader;
    vector<PointNodeStructure> pointNodes = fileReader->readAndConstructFromTxtFile("./text.txt");

    for (int i=0 ;i< pointNodes.size() ; ++i) {
        cout<<pointNodes[i].pointX<<endl;
    }
    return 0;
}