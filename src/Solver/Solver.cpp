#include "Solver.h"

#include <queue>
#include <algorithm>

Solver::Solver(const std::string& dataPath) {
    tinyxml2::XMLDocument doc;
    doc.LoadFile(dataPath.c_str());
    A = Task(doc->FirstChildElement("Task"));
}

void Solver::run() {
    firstStep();
    correctTime(firstSol);
    bestSol = firstSol;
    int deep1 = 100;
    do {
        curSol = firstSol;
        secondStep(); 
        if (curSol.getCost() < bestSol.getCost() || ((curSol.getCost() == bestSol.getCost()) && A.getTime(curSol) < A.getTime(bestSol))) {
            bestSol = std::move(curSol);
        }
    } while (deep1--);
    //printAns();   
}

void Solver::correctTime(Solution& sol) {
    if (A.getTime(sol) > A.Tmax) {
        tryReduceTime(sol);
        if (A.getTime(sol) > A.Tmax) {
            riseCost(sol);
        } 
    }
}

void Solver::rebuildSchedule(const Work& w) {
    std::queue<int> q;
    q.push(w.num);
    while (!q.empty()) {
        int node = q.pop();
        if (node == -1) {
            A.updateCriticalTime(firstSol);
            continue;
        }

        Work& work = A.graph[node].first;
        int64_t time = A.criticalTime[node];
        if (firstSol.isAssigned(&work) && firstSol.getSchElem(&work)->startT > A.criticalTime[node]) {
            VM* vm = firstSol.assignedVM(&work);
            vm->sch.erase(firstSol.getSchElem(&work));
            bool flag = true;
            int64_t lastEnd = 0;
            for (auto i = vm->sch.begin(); i != vm->sch.end(); i++) {
                if (time < i->startT) {
                    if (lastEnd + work(vm->X) <= i->startT) {
                        sch.insert(i, SchElem{&work, lastEnd, lastEnd + work(vm->X)});
                        flag = false;
                        break;
                    }
                }
                lastEnd = std::max(time, i->endT);
            }
            if (flag) {
                sch.emplace_back(SchElem{&work, lastEnd, lastEnd + work(vm->X)});
            }
        }

        for (auto& i : A.graph[node].second) {
            q.push(i->num);
        }
        if (!A.graph[node].second.empty()) {
            q.push(-1); // update Critical Time
        }
    }
}

void Solver::firstStep() {
    auto workQueue = A.getWorksGC1();
    A.updateCriticalTime(firstSol);
    for(auto& work : workQueue) {
        bool notFindVM = true;
        for (auto& vm : firstSol.Vm) {
            if (vm.canAssignWork(*work, A.criticalTime[work->num])) {
                firstSol.assignedVM[work] = &vm;
                notFindVM = false;
                break;
            }
        }

        if (notFindVM) {
            Vm.emplace_back(*work);
            firstSol.assignedVM[work] = &Vm.last();
        }
        firstSol.assignedVM[work]->insertWork(*work, A.criticalTime[work->num]);
        A.updateCriticalTime(firstSol);
        rebuildSchedule(*work);
    }
}

