#pragma once

#include "Work.h"
#include "Solution.h"
#include "tinyxml2.h"

class Task {
public:
    std::vector<std::pair<Work, std::vector<size_t>>> graph;
    std::vector<size_t> topSort;
    std::vector<int64_t> criticalTime;
 
    int64_t Tmax;

    Task() {};
    
    Task(tinyxml2::XMLElement* config);

    int64_t getTime(Solution& solution);

    void checkSolution(Solution& solution);

    void updateCriticalTime(Solution& solution);

    std::vector<Work*> getWorksGC1();
};