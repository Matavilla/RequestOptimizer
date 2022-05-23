#include "Task.h"

#include <algorithm>
#include <utility>
#include <cstring>

void dfs(size_t v, std::vector<bool>& visited, std::vector<std::pair<Work, std::vector<size_t>>>& graph, std::vector<size_t>& ans) { 
    visited[v] = true;
    for (auto& k : graph[v].second) {
        if (!visited[k]) {
            dfs(k, visited, graph, ans);
        }
    }
    ans.emplace_back(v);
}

std::vector<size_t> topological_sort(std::vector<std::pair<Work, std::vector<size_t>>>& graph) {
    std::vector<bool> visited(graph.size(), false);
    std::vector<size_t> ans;
    ans.reserve(graph.size());
    for (size_t i = 0; i < graph.size(); i++) {
        if (!visited[i]) {
            dfs(i, visited, graph, ans);
        }
    }
    std::reverse(ans.begin(), ans.end());
    return ans;
}

Task::Task(tinyxml2::XMLElement* config) {
    int tmp;
    config->FirstChildElement("Tmax")->QueryIntText(&tmp);
    Tmax = tmp;
    auto iter = config->FirstChildElement("Work");
    for (size_t i = 0; iter != nullptr; i++, iter = iter->NextSiblingElement("Work")) {
        graph.emplace_back(std::make_pair(Work(iter), std::vector<size_t>()));
    }

    iter = config->FirstChildElement("Link");
    for (size_t i = 0; iter != nullptr; i++, iter = iter->NextSiblingElement("Link")) {
        int i1, j1;
        iter->FirstChildElement("from")->QueryIntText(&i1);
        iter->FirstChildElement("to")->QueryIntText(&j1);
        graph[i1].second.emplace_back(j1);
    }

    topSort = topological_sort(graph);
    criticalTime.resize(graph.size());
}

void Task::updateCriticalTime(Solution& solution) {
    std::memset(criticalTime.data(), 0, criticalTime.size() * sizeof(criticalTime[0]));
    for (auto& v : topSort) {
      for (auto& j : graph[v].second) {
            int64_t time;
            if (solution.isAssigned(&graph[v].first)) {
                auto it = solution.getSchElem(&graph[v].first);
                time = it->endT;
            } else {
                time = criticalTime[v] + graph[v].first();
            }
            criticalTime[j] = std::max(criticalTime[j], time);
        }  
    }
}

std::vector<Work*> Task::getWorksGC1() {
    std::vector<Work*> tmp;
    for (auto& i : graph) {
        tmp.emplace_back(&i.first);
    }
    std::sort(tmp.begin(), tmp.end(), [](const Work* a, const Work* b) {return a->getBaseCost() > b->getBaseCost();});
    return tmp;
}

void Task::checkSolution(Solution& solution) {
    updateCriticalTime(solution);
    for (auto& vm : solution.Vm) {
        int64_t prevTime = 0;
        for (auto& el : vm.sch) {
            if (el.startT < criticalTime[el.work->num]) {
                //throw "wrong startTime";
            }
            if (el.startT < prevTime) {
                //throw "wrong schedule";
            }
            prevTime = std::max(prevTime, el.endT);
        }
    }
}

int64_t Task::getTime(Solution& solution) {
    checkSolution(solution);
    int64_t tmp = 0;
    for (auto& vm : solution.Vm) {
        tmp = std::max(tmp, vm.sch.back().endT);
    }
    return tmp;
 }