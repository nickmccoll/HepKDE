#ifndef ANALYSISSUPPORT_UTILITIES_INTERFACE_PARPARSER_H_
#define ANALYSISSUPPORT_UTILITIES_INTERFACE_PARPARSER_H_
#include <vector>
#include <string>
#include "ParCont.h"


class ParParser {
public:
    ParParser() {
        verbose = addBool("v","verbose");
    };
    std::shared_ptr<bool>                addBool(const std::string label, const std::string& desc);
    std::shared_ptr<double>              addFloat(const std::string label, const std::string& desc, bool isRequired = false, double defaultValue = 0);
    std::shared_ptr<int>                 addInt(const std::string label, const std::string& desc, bool isRequired =false, int defaultValue = 0);
    std::shared_ptr<std::string>         addString(const std::string label, const std::string& desc, bool isRequired =false, std::string defaultValue = "");
    std::shared_ptr<std::vector<double>> addVFloat(const std::string label, const std::string& desc, bool isRequired = false, const std::vector<double>& defaultValue = {0.0});
    std::shared_ptr<std::vector<int>>    addVInt(const std::string label, const std::string& desc, bool isRequired = false, const std::vector<int>& defaultValue  = {0});
    void parse(const std::string& arguments);

private:
    std::vector<std::unique_ptr<BaseParCont> > parameters;
    std::shared_ptr<bool> verbose;
};
#endif /* FRAMEWORK_PROCESSORS_INTERFACE_EVENTWEIGHTS_H_ */
