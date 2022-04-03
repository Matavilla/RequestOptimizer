#pragma once

#include "Work.h"
#include "Solution.h"
#include "tinyxml2.h"

struct Task {
    std::vector<std::pair<Work, std::vector<size_t>> graph;
    std::vector<size_t> indexSource;
    std::vector<int64_t> criticalTime;
    // std::vector<int64_t> prevWork;
    int64_t Tmax;

    Task(tinyxml2::XMLElement* config);

    int64_t getTime() const;

    void updateCriticalTime(const Solution& solution);

    std::vector<Work*> getWorksGC1();
};