//
// Created by hp on 2019/3/18.
//

#ifndef ACURACYCALCULATION_FILEREADER_H
#define ACURACYCALCULATION_FILEREADER_H

#include "PointNodeStructure.cpp"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

class FileReader {

private:


public:
    vector<PointNodeStructure> readAndConstructFromTxtFile(string file);

};


#endif //ACURACYCALCULATION_FILEREADER_H
