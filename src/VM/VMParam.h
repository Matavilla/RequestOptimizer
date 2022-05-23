#pragma once

#include <string>
#include <memory>
#include <map>

class BaseParam {
public:
    virtual bool riseCost() = 0;

    virtual bool downCost() = 0;

    virtual double getCost() const = 0;

    virtual int64_t getValue() const = 0;
    
    virtual bool setValue(int64_t val) = 0;

    virtual double getStepCost() const = 0;

    virtual bool overcommitFlag() const = 0;

    virtual ~BaseParam() = default;
};

template <int64_t x1_val, int64_t x2_val, int64_t step, int16_t cost, int64_t overcommit>
class Param : public BaseParam {
    int64_t value;
    static constexpr int64_t x1 = x1_val;
    static constexpr int64_t x2 = x2_val;
public:
    Param(const int64_t& init_val) : value(init_val) {
        if (init_val < x1 || init_val > x2)
            throw "wrong param init_val";
    }

    Param(Param& tmp) = default;

    Param(Param&& tmp) = default;
    
    Param& operator=(Param&& tmp) = default;

    bool riseCost() {
        if (value + step > x2) {
            return false;
        }
        value += step;
        return true;
    }

    bool overcommitFlag() const {
        return overcommit;
    }

    bool downCost() {
        if (value - step < x1) {
            return false;
        }
        value -= step;
        return true;
    }

    int64_t getValue() const {
        return value;
    }

    bool setValue(int64_t val) {
        if (val < x1 || val > x2) {
            return false;
        }
        value = val;
        return true;
    }

    double getCost() const {
        return cost * ((double) value) / x2;
    }

    double getStepCost() const {
        return cost * step;
    }
};