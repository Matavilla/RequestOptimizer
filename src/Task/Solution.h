#pragma once

#include "Work.h"
#include "VM.h"

#include <set>
struct Solution {
    std::vector<VM> Vm;
    std::set<int64_t> assignedWork;

    bool isAssigned(Work* work) const {
        return assignedWork.contains(work->num);
    }

    VM* getVM(Work* work) {
        for (auto& vm : Vm) {
            auto it = std::find_if(vm.sch.begin(), vm.sch.end(), [work] (const SchElem& a) {return a.work == work;});
            if (it != vm.sch.end()) {
                return &vm;
            }
        }
        return nullptr;
    }

    std::list<SchElem>::iterator getSchElem(Work* work) {
        for (auto& vm : Vm) {
            auto it = std::find_if(vm.sch.begin(), vm.sch.end(), [work] (const SchElem& a) {return a.work == work;});
            if (it != vm.sch.end()) {
                return it;
            }
        }
    }
    
    double getCost() {
        std::for_each(Vm.begin(), Vm.end(), [] (VM& vm) {vm.updateCost();});
        return std::accumulate(Vm.begin(), Vm.end(), 0.0, [] (const double& sum, const VM& vm) {return sum + vm.curCost;});
    }
};