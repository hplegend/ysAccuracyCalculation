//
// Created by hp.he on 2019/3/20.
//

#ifndef ACURACYCALCULATION_POINTNODESCOMPARISON_H
#define ACURACYCALCULATION_POINTNODESCOMPARISON_H

#include <vector>
#include "PointNodeStructure.cpp"

using namespace std;

class PointNodesComparison {

public:
    float  simpleCalculateAccuracy(vector<PointNodeStructure> groundTruth, vector<PointNodeStructure> others);

    float  ocalTreeCalculateAccuracy(vector<PointNodeStructure> groundTruth, vector<PointNodeStructure> others);


};


#endif //ACURACYCALCULATION_POINTNODESCOMPARISON_H
