#include "Task.h"

#include <algorithm>
#include <utility>

Task::Task(tinyxml2::XMLElement* config) {
    config->FirstChildElement("Tmax")->QueryIntText(&Tmax);
    auto iter = config->FirstChildElement("Work");
    for (size_t i = 0; iter != nullptr; i++, iter = iter->NextSiblingElement("Work")) {
        graph.emplace_back(std::make_pair(Work(iter), std::list<Work*>()));
    }

    iter = config->FirstChildElement("Link");
    for (size_t i = 0; iter != nullptr; i++, iter = iter->NextSiblingElement("Link")) {
        int i1, j1;
        iter->FirstChildElement("from")->QueryIntText(&i1);
        iter->FirstChildElement("to")->QueryIntText(&j1);
        graph[i1].second.emplace_back(j1));
    }

    iter = config->FirstChildElement("Source");
    for (size_t i = 0; iter != nullptr; i++, iter = iter->NextSiblingElement("Source")) {
        int i;
        iter->QueryIntText(&i);
        indexSource.emplace_back(i);
    }
}

void Task::updateCriticalTime(const Solution& solution) {
    criticalTime = std::move(std::vector<int64_t>(graph.size(), -1));
    for (auto& source : indexSource) {
        std::vector<bool> visited(graph.size(), false);
        std::vector<int64_t> time(graph.size(), -1);
        // std::vector<int> prev(graph.size(), -1);

        time[source] = 0;
        for (size_t i = 0; i < graph.size(); i++) {
            int v = -1;
            for (size_t j = 0; j < graph.size(); j++) {
                if (!visited[j] && (v == -1 || time[j] > time[v])) {
                    v = j;
                }
            }

            visited[v] = true;

            for (auto& j : graph[v].second) {
                if (solution.isAssigned(&graph[v].first)) {
                    auto it = solution.getSchElem(&graph[v].first);
                    if (it->endT > time[j]) {
                        time [j] = it->endT;
                    }
                } else {
                    if (time[v] + graph[v].first() > time[j]) {
                        time[j] = time[v] + graph[v].first();
                        // prev[j] = v;
                    }
                }
            }
        }

        for (size_t j = 0; j < time.size(); j++) {
            if (criticalTime[j] < time[j]) {
                criticalTime[j] = time[j];
                // prevWork[j] = prev[j];
            }
        }
    }
}

std::vector<Work*> Task::getWorksGC1() {
    std::vector<Work*> tmp(graph.size());
    for (auto& i : graph) {
        tmp.emplace_back(i.first);
    }
    std::sort(tmp.begin(), tmp.end(), [](const Work* a, const Work* b) {return a->getBaseCost() > b->getBaseCost()});
    return std::move(tmp);
}