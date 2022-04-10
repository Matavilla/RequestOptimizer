#pragma once

#include <vector>
#include <queue>
#include <string>

#include "Task.h"
#include "Solution.h"
#include "tinyxml2.h"

class Solver {
    Task A;
    Solution bestSol;
    Solution firstSol;
    Solution curSol;

    void rebuildSchedule(std::queue<int>& q, Solution& sol);

    void correctTime(Solution& sol);

    void tryReduceTime(Solution& sol);

    void assigneDeletedWork(Solution& sol, std::vector<Work*>& delWork);

    bool riseCost(Solution& sol, int64_t time);
public:
    Solver(const std::string& dataPath = "");

    void run();

    void firstStep();

    void secondStep(std::vector<size_t>& delVM);

    void printAns();
};