#pragma once

#include <functional>
#include <utility>

#include "VMParam.h"
#include "tinyxml2.h"

class Work {
public:
    Parameters M;
    std::function<int64_t(const Parameters&)> t;
    int64_t num;

    Work(tinyxml2::XMLElement* config);

    int64_t operator()(const Parameters& m) const { return t(m); };

    int64_t operator()() const { return t(M); };

    double getBaseCost() const;
};
