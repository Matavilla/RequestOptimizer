#include "Work.h"

Work::Work(tinyxml2::XMLElement* config) {
    int tmp;
    config->FirstChildElement("N")->QueryIntText(&tmp);
    num = tmp;
    config->FirstChildElement("t")->QueryIntText(&tmp);
    t = [](const Parameters& tmp) {return 10;};
    
    for (const auto& i : NAME_PARAM) {
        config->FirstChildElement(i.c_str())->QueryIntText(&tmp);
        M[i].reset(createParam(i, tmp));
    }
}

double Work::getBaseCost() const{
    double curCost = 0;
    for (auto& [k, j] : M) {
        curCost += j->getCost();
    }
    return curCost;
}