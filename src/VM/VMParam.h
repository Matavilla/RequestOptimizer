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

    virtual double getStepCost() const = 0;

    virtual ~BaseParam() = default;
};

template <int64_t x1_val, int64_t x2_val, int64_t step, int16_t cost>
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

    double getCost() const {
        return cost * ((double) value) / x2;
    }

    double getStepCost() const {
        return cost * step;
    }
};

using Parameters = std::map<std::string, std::unique_ptr<BaseParam>>;
const std::vector<std::string> NAME_PARAM = {"vCPU", "vRAM", "vHDD", "vIO"};

BaseParam* createParam(const std::string& name, const int64_t& value = 0) {
    if (name == NAME_PARAM[0]) {
        return new Param<1, 100, 1, 400>(value);
    } else if (name == NAME_PARAM[1]) {
        return new Param<1, 256, 1, 300>(value);
    } else if (name == NAME_PARAM[2]) {
        return new Param<1, 2048, 1, 100>(value);
    } else if (name == NAME_PARAM[3]) {
        return new Param<1, 128, 1, 300>(value);
    } else {
        throw "wrong name param";
    }
};