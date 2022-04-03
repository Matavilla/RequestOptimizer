#include "Work.h"

Work::Work(tinyxml2::XMLElement* config) {
    int tmp;
    config->FirstChildElement("N")->QueryIntText(&num);
    config->FirstChildElement("t")->QueryIntText(&tmp);
    t = [](const Parameters& tmp) {return 10;};
    
    for (const auto& i : NAME_PARAM) {
        config->FirstChildElement(i)->QueryIntText(&tmp);
        M[i] = CreateParam(i, tmp);
    }
}

double Work::getBaseCost() {
    auto sum = [](const double& sum, const std::unique_ptr<BaseParam>& tmp) {return sum + tmp->getCost();}
    return std::accumulate(M.begin(), M.end(), 0.0, sum);
}