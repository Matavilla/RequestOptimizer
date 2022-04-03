#pragma once

#include <vector>
#include <string>

#include "Task.h"
#include "Solution.h"
#include "tinyxml2.h"

class Solver {
    Task A;
    Solution bestSol;
    Solution firstSol;
    Solution curSol;

    void rebuildSchedule(const Work& work);
public:
    Task(const std::string& dataPath);

    void run();

    void firstStep();

    //void secondStep();

    //void printAns();
};