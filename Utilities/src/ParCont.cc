#include "../interface/ParCont.h"
#include <sstream>

BaseParCont::BaseParCont(const std::string label, const std::string& desc, bool useValue, bool isRequired)
: label(label), desc(desc), useValue(useValue),isRequired(isRequired) {}
void BaseParCont::print(std::ostream& where) const { where << label << " Desc:'"<<desc<<"' ";}

BoolCont::BoolCont(const std::string& label, const std::string& desc) : BaseParCont(label,desc,false,false), value(new bool(false))
{}

void BoolCont::print(std::ostream& where) const {
    where <<"Val:'" << (*value ? "True" : "False") <<"'";
}
void BoolCont::fill() {
    *value = true;
    wasFilled = true;
}

FloatCont::FloatCont(const std::string& label, const std::string& desc, bool isRequired, double defaultValue) :
        BaseParCont(label,desc,true,isRequired), value(new double(defaultValue))
{}

void FloatCont::print(std::ostream& where) const {
    where << (isRequired ? "R " : "NR ") << "Val:'" << *value <<"'";
}
bool FloatCont::fill( const std::string& val) {
    std::stringstream ss(val);
    if(ss >> (*value)){
        wasFilled = true;
        return true;
    }
    return false;
}

IntCont::IntCont(const std::string& label, const std::string& desc, bool isRequired, int defaultValue) :
        BaseParCont(label,desc,true,isRequired), value(new int(defaultValue))
{}

void IntCont::print(std::ostream& where) const {
    where << (isRequired ? "R " : "NR ") << "Val:'" << *value <<"'";
}
bool IntCont::fill( const std::string& val) {
    std::stringstream ss(val);
    if(ss >> (*value)){
        wasFilled = true;
        return true;
    }
    return false;

}

StringCont::StringCont(const std::string& label, const std::string& desc, bool isRequired, const std::string& defaultValue) :
        BaseParCont(label,desc,true,isRequired), value(new std::string(defaultValue))
{}

void StringCont::print(std::ostream& where) const {
    where << (isRequired ? "R " : "NR ") << "Val:'" << *value <<"'";
}
bool StringCont::fill( const std::string& val) {
    *value = val;
    wasFilled = true;
    return true;
}

VFloatCont::VFloatCont(const std::string& label, const std::string& desc, bool isRequired, const std::vector<double>& defaultValue) :
        BaseParCont(label,desc,true,isRequired), value(new std::vector<double>(defaultValue))
{}

void VFloatCont::print(std::ostream& where) const {
    where << (isRequired ? "R " : "NR ") << "Val:'";
    for(const auto v : *value) where << v <<",";
    where <<"'";
}
bool VFloatCont::fill( const std::string& val) {
    std::stringstream ss(val);
    value->clear();
    double i;
    while (ss >> i) {
       value->push_back(i);
        if (ss.peek() == ',') ss.ignore();
    }
    wasFilled = true;
    return true;
}

VIntCont::VIntCont(const std::string& label, const std::string& desc, bool isRequired, const std::vector<int>& defaultValue) :
        BaseParCont(label,desc,true,isRequired), value(new std::vector<int>(defaultValue))
{}

void VIntCont::print(std::ostream& where) const {
    where << (isRequired ? "R " : "NR ") << "Val:'";
    for(const auto v : *value) where << v <<",";
    where <<"'";
}
bool VIntCont::fill( const std::string& val) {
    std::stringstream ss(val);
    value->clear();
    int i;
    while (ss >> i) {
       value->push_back(i);
        if (ss.peek() == ',') ss.ignore();
    }
    wasFilled = true;
    return true;
}



