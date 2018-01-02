#ifndef ANALYSISSUPPORT_UTILITIES_INTERFACE_PARCONT_H_
#define ANALYSISSUPPORT_UTILITIES_INTERFACE_PARCONT_H_
#include <vector>
#include <iostream>
#include <memory>


class BaseParCont {
public:
    BaseParCont(const std::string label, const std::string& desc, bool useValue, bool isRequired);
    virtual ~BaseParCont() {};
    virtual bool fill( const std::string& val) {return val != "";}
    virtual void fill() {}
    virtual void print(std::ostream& where) const;

    friend  std::ostream& operator<< (std::ostream& out, const BaseParCont& mc) {
        mc.BaseParCont::print(out);
        mc.print(out);
        return out;
    }


    //label used to select the parameter
    const std::string label;
    //Description
    const std::string desc;

    const bool useValue; //if false it is just a flag
    const bool isRequired;

    //How it was handled
    bool wasFilled = false;
};


class BoolCont : public BaseParCont{
public:
    BoolCont(const std::string& label, const std::string& desc);
    void print(std::ostream& where) const override;
    void fill() override;
    std::shared_ptr<bool> value;
};

class FloatCont : public BaseParCont{
public:
    FloatCont(const std::string& label, const std::string& desc, bool isRequired, double defaultValue);
    void print(std::ostream& where) const override;
    bool fill( const std::string& val) override;
    std::shared_ptr<double> value;
};
class IntCont : public BaseParCont{
public:
    IntCont(const std::string& label, const std::string& desc, bool isRequired, int defaultValue);
    void print(std::ostream& where) const override;
    bool fill( const std::string& val) override;
    std::shared_ptr<int> value;
};
class StringCont : public BaseParCont{
public:
    StringCont(const std::string& label, const std::string& desc, bool isRequired, const std::string& defaultValue);
    void print(std::ostream& where) const override;
    bool fill( const std::string& val) override;
    std::shared_ptr<std::string> value;
};

class VFloatCont : public BaseParCont{
public:
    VFloatCont(const std::string& label, const std::string& desc, bool isRequired, const std::vector<double>& defaultValue);
    void print(std::ostream& where) const override;
    bool fill( const std::string& val) override;
    std::shared_ptr<std::vector<double> > value;
};

class VIntCont : public BaseParCont{
public:
    VIntCont(const std::string& label, const std::string& desc, bool isRequired, const std::vector<int>& defaultValue);
    void print(std::ostream& where) const override;
    bool fill( const std::string& val) override;
    std::shared_ptr<std::vector<int> > value;
};

#endif /* FRAMEWORK_PROCESSORS_INTERFACE_EVENTWEIGHTS_H_ */
