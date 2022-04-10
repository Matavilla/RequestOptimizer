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

    void updateParameters() {
        for (auto& i : sch) {
            for (auto& [k, j] : i.work->M) {
                if (!j->overcommitFlag()) {
                    X[k]->setValue(std::max(j->getValue(), X[k]->getValue()));
                } else {
                    X[k]->setValue(j->getValue() + X[k]->getValue());
                }
            }
        }
    }

    void updateCost() {
        curCost = 0;
        for (auto& [k, j] : X) {
            curCost += j->getCost();
        }
    }

    VM() = delete;

    VM(const Work& a) {
        for (auto& [i, j] : a.M) {
            X[i].reset(createParam(i, j->getValue()));
        }
    }

    VM(const VM& vm) {
        sch = vm.sch;
        for (auto& [i, j] : vm.X) {
            X[i].reset(createParam(i, j->getValue()));
        }
    }

    bool canAssignWork(const Work& w, const int64_t& time) {
        std::map<std::string, int64_t> saveParam;
        bool flag = true;
        for (auto& [i, j] : w.M) {
            saveParam[i] = X[i]->getValue();
            bool retVal;
            if (!j->overcommitFlag()) {
                retVal = X[i]->setValue(std::max(j->getValue(), X[i]->getValue()));
            } else {
                retVal = X[i]->setValue(j->getValue() + X[i]->getValue());
            }
            if (!retVal) {
                flag = false;
                break;
            }
        }

        if (flag) {
            for (auto& i : sch) {
                int64_t endT = i.startT + i.work->t(X);
                if ((i.startT <= time && endT >= time) || (i.startT <= (time + w(X)) && endT >= (time + w(X)))) {
                    flag = false;
                    break;
                }
            }
        }

        if (!flag) {
            for (auto& [i, j] : saveParam) {
                X[i]->setValue(j);
            }
            return false;
        }
        return true;
    }

    void insertWork(Work& w, const int64_t& time) {
        SchElem tmp {&w, time, time + w(X)};
        for (auto i = sch.begin(); i != sch.end(); i++) {
            if (i->startT >= tmp.endT) {
                sch.insert(i, std::move(tmp));
                return;
            }
        }
        sch.emplace_back(std::move(tmp));
    }
};
