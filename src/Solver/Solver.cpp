#include "Solver.h"

#include <algorithm>
#include <array>
#include <random>
#include <map>
#include <set>
#include <ctime>
#include <iostream>


Solver::Solver(const std::string& dataPath) {
    tinyxml2::XMLDocument doc;
    doc.LoadFile(dataPath.c_str());
    A = Task(doc.FirstChildElement("Task"));
}

bool next_combination(std::vector<size_t>& a, int n) {
	int k = a.size();
	for (int i = k - 1; i >= 0; --i)
		if (a[i] < (n - k + i + 1)) {
			++a[i];
			for (int j = i + 1; j < k; ++j)
				a[j] = a[j - 1] + 1;
			return true;
		}
	return false;
}

void Solver::run() {
    firstStep();
    correctTime(firstSol);
    bestSol = firstSol;
    for (size_t k = 1; k < firstSol.Vm.size() - 1; k++) {
        std::vector<std::vector<size_t>> delVM;
        std::vector<size_t> tmp;
        for (size_t i = 0; i < k; i++) {
            tmp.emplace_back(i + 1);
        }
        delVM.push_back(tmp);
        delVM.push_back(std::move(tmp));
        while (next_combination(delVM.back(), firstSol.Vm.size() - 1)) {
            delVM.push_back(delVM.back());
        }
        delVM.pop_back();

        std::mt19937 engine;
        engine.seed(unsigned(std::time(nullptr)));
        int deep1 = 100;
        do {
            if (delVM.empty()) {
                break;
            }
            std::uniform_int_distribution<size_t> distribution(0, delVM.size() - 1);
            size_t ind = distribution(engine);
            curSol = firstSol;
            for (auto& vm : curSol.Vm) {
                vm.updateParameters();
            }
            secondStep(delVM[ind]); 
            if (curSol.getCost() < bestSol.getCost() || ((curSol.getCost() == bestSol.getCost()) && A.getTime(curSol) < A.getTime(bestSol))) {
                bestSol = std::move(curSol);
            }
            delVM.erase(delVM.begin() + ind);
        } while (deep1--);
    }
    printAns();   
}

void Solver::correctTime(Solution& sol) {
    if (A.getTime(sol) > A.Tmax) {
        tryReduceTime(sol);
        while (A.getTime(sol) > A.Tmax) {
            if (!riseCost(sol, A.getTime(sol))) {
                throw("WTF with time !?");
            }
        } 
    }
}

bool Solver::riseCost(Solution& sol, int64_t time) {
    bool flag = true;
    size_t nVm = 0;
    std::string nameParam;
    int diffTime = 0;

    size_t k = 0;
    for (auto& vm : sol.Vm) {
        for (auto& [i, j] : vm.X) {
            if (j->riseCost()) {
                Solution tmpSol(sol);
                std::queue<int> q;
                q.push(-1);

                A.updateCriticalTime(tmpSol);
                int64_t lastEnd = 0;
                for (auto i1 = vm.sch.begin(); i1 != vm.sch.end(); i1++) {
                    i1->startT = std::max(lastEnd, A.criticalTime[i1->work->num]);
                    i1->endT = i1->startT + (*(i1->work))(vm.X);
                    lastEnd = std::max(time, i1->endT);
                    q.push(i1->work->num);
                }                
                rebuildSchedule(q, tmpSol);
                int curDiffTime = time - A.getTime(tmpSol);
                if (flag || diffTime < curDiffTime) {
                    flag = false;
                    nVm = k;
                    nameParam = i;
                    diffTime = curDiffTime;
                }
                j->downCost();
            }
        }
        k++;
    }
    if (flag) {
        return false;
    }
    sol.Vm[nVm].X[nameParam]->riseCost();
    return true;
}

void Solver::rebuildSchedule(std::queue<int>& q, Solution& sol) {
    while (!q.empty()) {
        int node = q.front();
        q.pop();
        if (node == -1) {
            A.updateCriticalTime(sol);
            continue;
        }

        for (auto& vm : sol.Vm) {
            for (auto i = vm.sch.begin(); i != vm.sch.end(); i++) {
                if (std::find(A.graph[node].second.begin(), A.graph[node].second.end(), i->work->num) != A.graph[node].second.end()) {
                    int64_t time = A.criticalTime[i->work->num];
                    Work& work = *(i->work);
                    vm.sch.erase(i);
                    bool flag = true;
                    int64_t lastEnd = time;
                    for (auto j = vm.sch.begin(); j != vm.sch.end(); j++) {
                        if (time < j->startT) {
                            if ((lastEnd + work(vm.X)) <= j->startT) {
                                i = vm.sch.insert(j, SchElem{&work, lastEnd, lastEnd + work(vm.X)});
                                flag = false;
                                break;
                            }
                        }
                        lastEnd = std::max(time, j->endT);
                    }
                    if (flag) {
                        vm.sch.emplace_back(SchElem{&work, lastEnd, lastEnd + work(vm.X)});
                        i = vm.sch.end();
                    }
                }
            }
        }

        if (!A.graph[node].second.empty()) {
            q.push(-1); // update Critical Time
        }
        for (auto& i : A.graph[node].second) {
            q.push(i);
        }
    }
}

void Solver::firstStep() {
    auto workQueue = A.getWorksGC1();
    A.updateCriticalTime(firstSol);
    std::queue<int> q;
    for(auto& work : workQueue) {
        q.push(-1);
        q.push(work->num);

        bool notFindVM = true;
        for (auto& vm : firstSol.Vm) {
            if (vm.canAssignWork(*work, A.criticalTime[work->num])) {
                for (auto& [i, j] : work->M) {
                    if (!j->overcommitFlag()) {
                        vm.X[i]->setValue(std::max(j->getValue(), vm.X[i]->getValue()));
                    } else {
                        vm.X[i]->setValue(j->getValue() + vm.X[i]->getValue());
                    }
                }
                int64_t lastEnd = 0;
                for (auto i = vm.sch.begin(); i != vm.sch.end(); i++) {
                    i->startT = std::max(lastEnd, A.criticalTime[i->work->num]);
                    i->endT = i->startT + (*(i->work))(vm.X);
                    lastEnd = i->endT;
                    q.push(i->work->num);
                }
                firstSol.assignedVM[work->num] = &vm;
                notFindVM = false;
                break;
            }
        }

        if (notFindVM) {
            firstSol.Vm.emplace_back(*work);
            firstSol.assignedVM[work->num] = &firstSol.Vm.back();
            q.pop(); q.pop();
        }
        firstSol.assignedVM[work->num]->insertWork(*work, A.criticalTime[work->num]);
        rebuildSchedule(q, firstSol);
    }
}

void Solver::assigneDeletedWork(Solution& sol, std::vector<Work*>& delWork) {
    A.updateCriticalTime(sol);
    for (auto& vm : sol.Vm) {
        int64_t lastEnd = 0;
        for (auto i = vm.sch.begin(); i != vm.sch.end(); i++) {
            i->startT = std::max(lastEnd, A.criticalTime[i->work->num]);
            i->endT = i->startT + (*i->work)(vm.X);
        }
    }
    std::queue<int> q;
    for (auto& w : delWork) {
        A.updateCriticalTime(sol);
        VM* vmForAssigned = nullptr;
        size_t delta = -1;
        for (auto& vm : sol.Vm) {
            size_t locDelta = 0;
            for (auto& j : vm.sch) {
                if (j.startT <= A.criticalTime[w->num] && j.endT >= A.criticalTime[w->num]) {
                    locDelta = j.endT - A.criticalTime[w->num];
                    break;
                }
            }

            if (delta > locDelta) {
                vmForAssigned = &vm;
                delta = locDelta;
            }
        }

        for (auto& [k, j] : w->M) {
            if (!j->overcommitFlag()) {
                vmForAssigned->X[k]->setValue(std::max(j->getValue(), vmForAssigned->X[k]->getValue()));
            } else {
                vmForAssigned->X[k]->setValue(j->getValue() + vmForAssigned->X[k]->getValue());
            }
        }
        bool flag = true;
        int64_t lastEnd = 0;
        SchElem tmp {w, A.criticalTime[w->num] + delta, A.criticalTime[w->num] + delta + (*w)(vmForAssigned->X)};
        vmForAssigned->sch.emplace_back(tmp);
        A.updateCriticalTime(sol);
        vmForAssigned->sch.pop_back();
        for (auto i = vmForAssigned->sch.begin(); i != vmForAssigned->sch.end(); i++) {
            i->startT = std::max(lastEnd, A.criticalTime[i->work->num]);
            i->endT = i->startT + (*i->work)(vmForAssigned->X);
            if (flag && (i->startT >= tmp.startT)) {
                i = vmForAssigned->sch.insert(i, std::move(tmp));
                flag = false;
            }
            lastEnd = i->endT;
        }
        if (flag) {
            vmForAssigned->sch.emplace_back(std::move(tmp));
        }
        q.push(w->num);
    }
    /* перестроить расписание */
    rebuildSchedule(q, sol);
}

void Solver::tryReduceTime(Solution& sol) {
    int deep = 1000;
    do {
        A.updateCriticalTime(sol);
        std::vector<Work*> delWork;
        Work* delW = nullptr;
        size_t delta = 0;
        for (auto& vm : sol.Vm) {
            for (auto& j : vm.sch) {
                if (delta < (j.startT - A.criticalTime[j.work->num])) {
                    delW = j.work;
                    delta = j.startT - A.criticalTime[j.work->num];
                }
            }
        }
        delWork.emplace_back(delW);
        sol.assignedVM.erase(delW->num);
        std::remove_if(sol.assignedVM[delW->num]->sch.begin(),
                       sol.assignedVM[delW->num]->sch.end(),
                       [&delW] (const SchElem& el) { return el.work == delW; });
        assigneDeletedWork(sol, delWork);
    } while (deep--);
}

void Solver::secondStep(std::vector<size_t>& delVM) {
    std::vector<Work*> delWork;
    std::set<VM*> delVMPtr;
    for (auto& ind : delVM) {
        delVMPtr.insert(&curSol.Vm[ind]);
        for (auto& j : curSol.Vm[ind].sch) {
            delWork.emplace_back(j.work);
            curSol.assignedVM.erase(j.work->num);
        }
    }
    std::remove_if(curSol.Vm.begin(),
                   curSol.Vm.end(),
                   [&delVMPtr] (VM& el)  { return delVMPtr.find(&el) != delVMPtr.end(); });
    std::sort(delWork.begin(),
             delWork.end(),
             [this] (Work* a, Work* b) {
                 auto aIt = std::find(A.topSort.begin(), A.topSort.end(), a->num);
                 auto bIt = std::find(A.topSort.begin(), A.topSort.end(), b->num);
                 return  aIt < bIt;
             });
    assigneDeletedWork(curSol, delWork);
    correctTime(curSol);
}

void Solver::printAns() {
    size_t i = 1;
    for (auto& vm : bestSol.Vm) {
        std:: cout << "VM_" << i++ << " : ";
        for (auto& i : NAME_PARAM) {
            std::cout << "<" << i << ": " << vm.X[i]->getValue() << ">, ";
        }
        std::cout << "## Cost: " << vm.curCost << std::endl;

        for (auto& i : vm.sch) {
            std::cout << "(*" << i.work->num << "* " << i.startT << ", " << i.endT << ")  ->  ";
        }
        std::cout << std::endl;
    }
    std::cout << "TotalCost: " << bestSol.getCost() << std::endl; 
}