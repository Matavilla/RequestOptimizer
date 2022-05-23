#include "Work.h"

#include <cmath>

Work::Work(tinyxml2::XMLElement* config) {
    int tmp;
    config->FirstChildElement("N")->QueryIntText(&tmp);
    num = tmp;
    config->FirstChildElement("TBase")->QueryIntText(&tmp);
    tBase = (double) tmp;
    
    for (const auto& i : NAME_PARAM) {
        config->FirstChildElement(i.c_str())->QueryIntText(&tmp);
        M[i].reset(createParam(i, tmp));
    }
}

double Work::getBaseCost() const {
    double curCost = 0;
    for (auto& [k, j] : M) {
        curCost += j->getCost();
    }
    return curCost;
}

BaseParam* createParam(const std::string& name, const int64_t& value) {
    if (name == NAME_PARAM[0]) {
        return new Param<1, 128, 1, 400, 0>(value);
    } else if (name == NAME_PARAM[1]) {
        return new Param<1, 2048, 1, 300, 0>(value);
    } else if (name == NAME_PARAM[2]) {
        return new Param<1, 2048, 1, 100, 0>(value);
    } else if (name == NAME_PARAM[3]) {
        return new Param<1, 62, 1, 300, 0>(value);
    } else {
        throw "wrong name param";
    }
};