#pragma once

#include <functional>
#include <utility>
#include <cmath>

#include "VMParam.h"
#include "tinyxml2.h"

using Parameters = std::map<std::string, std::shared_ptr<BaseParam>>;

const std::array<std::string, 4> NAME_PARAM = {"vCPU", "vRAM", "vHDD", "vIO"};

class Work {
public:
    Parameters M;
    int64_t num;
    double tBase;

    Work(tinyxml2::XMLElement* config);

    int64_t operator()(const Parameters& m) const { 
        double coef = 1 / (0.3 + 0.7 / (m.at(NAME_PARAM[0])->getValue() + 1 - M.at(NAME_PARAM[0])->getValue()));
        coef *= 1 / (0.6 + 0.4 / (m.at(NAME_PARAM[1])->getValue() + 1 - M.at(NAME_PARAM[1])->getValue()));
        coef *= 1 / (0.9 + 0.1 / (m.at(NAME_PARAM[3])->getValue() + 1 - M.at(NAME_PARAM[3])->getValue()));
        return static_cast<int64_t>(std::round(tBase / coef)); 
    };

    int64_t operator()() const { return (*this)(M); };

    double getBaseCost() const;
};

BaseParam* createParam(const std::string& name, const int64_t& value = 0);