#pragma once

#include "Work.h"
#include "VM.h"

struct Solution {
    std::vector<VM> Vm;
    std::map<int64_t, VM*> assignedVM;

    bool isAssigned(Work* work) const {
        return assignedVM.contains(work->num);
    }

    std::list<SchElem>::iterator  getSchElem(Work* work) {
        VM* vm = assignedVM[work->num];
        return std::find_if(vm->sch.begin(), vm->sch.end(), [work] (const SchElem& a) {return a.work == work;});
    }
    
    double getCost() {
        std::for_each(Vm.begin(), Vm.end(), [] (VM& vm) {vm.updateCost();});
        return std::accumulate(Vm.begin(), Vm.end(), 0.0, [] (const double& sum, const VM& vm) {return sum + vm.curCost;});
    }
};