#pragma once

#include <functional>
#include <utility>

#include "VMParam.h"
#include "tinyxml2.h"

struct Work {
    Parameters M;
    std::function<int64_t(const Parameters&)> t;
    int64_t num;

    Work(tinyxml2::XMLElement* config);

    int64_t operator()(const Parameters& m = M) { return t(m); }

    double getBaseCost();
};
