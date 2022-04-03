#pragma once

#include "Work.h"
#include "VM.h"

struct Solution {
    std::vector<VM> Vm;
    std::map<Work*, VM*> assignedVM;

    bool isAssigned(Work* work) {
        return solution.assignedVM.contains(work);
    }

    std::list<SchElem>::iterator  getSchElem(Work* work) {
        VM* vm = assignedVM[work];
        return std::find_if(vm->sch.begin(), vm->sch.end(), [work] (const SchElem& a) {return a.work == work;})
    }
    
    double getCost() {
        std::for_each(Vm.begin(), Vm.end(), [] (VM& vm) {vm.updateCost();});
        return std::accumulate(Vm.begin(), Vm.end(), 0.0, [] (const double& sum, const VM& vm) {return sum + vm.curCost;});
    }
};