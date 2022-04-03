#pragma once

#include "VMParam.h"
#include "Work.h"

#include <algorithm>
#include <numeric>
#include <utility>
#include <list>

struct SchElem {
    Work* work;
    int64_t startT;
    int64_t endT;
};

struct VM {
    std::list<SchElem> sch;
    Parameters X;
    
    double curCost = 0.0;

    void updateCost() {
        auto sum = [](const double& sum, const std::unique_ptr<BaseParam>& tmp) {return sum + tmp->getCost();};
        curCost = std::accumulate(X.begin(), X.end(), 0.0, sum);
    }

    VM() = delete;

    VM(const Work& a) {
        for (auto& [i, j] : a.M) {
            X[i] = createParam(i, j->getValue());
        }
    }

    VM(const VM& vm) {
        sch = vm.sch;
        for (auto& [i, j] : vm.X) {
            X[i] = createParam(i, j->getValue());
        }
    }

    bool canAssignWork(const Work& w, const int64_t& time) {
        for (auto& i : sch) {
            if ((i->startT <= time && i->endT >= time) || (i->startT <= (time + w(X)) && i->endT >= (time + w(X)))) {
                return false;
            }
        }
        return true;
    }

    void insertWork(const Work& w, const int64_t& time) {
        SchElem tmp {&w, time, time + w(X)};
        for (auto i = sch.begin(); i < sch.end(); i++) {
            if ((*i)->startT > tmp.endT)) {
                sch.insert(i, std::move(tmp));
                return;
            }
        }
        sch.emplace_back(std::move(tmp));
    }
};
