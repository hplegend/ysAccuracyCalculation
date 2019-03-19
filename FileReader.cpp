//
// Created by hp on 2019/3/18.
//

#include "FileReader.h"

vector<PointNodeStructure> FileReader::readAndConstructFromTxtFile(string file) {

    ifstream input;
    input.open(file.data(),ios::in);

    int lineNum ;
    input>>lineNum;

    vector<PointNodeStructure> pointNodes;

    for (int i = 0; i < lineNum; ++i) {

        PointNodeStructure *innerNode = new PointNodeStructure;

        input>>innerNode->pointX;
        input>>innerNode->pointY;
        input>>innerNode->pointZ;
        input>>innerNode->pointLabel;

        pointNodes.push_back(*innerNode);
    }

    return pointNodes;
}
