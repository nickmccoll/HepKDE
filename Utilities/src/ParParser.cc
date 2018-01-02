#include "../interface/ParParser.h"

#include <sstream>
#include <iostream>
#include <iterator>

std::shared_ptr<bool>        ParParser::addBool(const std::string label, const std::string& desc) {
    auto ptr = new BoolCont(label,desc);
    parameters.emplace_back(ptr);
    return ptr->value;
}
std::shared_ptr<double>       ParParser::addFloat(const std::string label, const std::string& desc, bool isRequired, double defaultValue) {
    auto ptr = new FloatCont(label,desc,isRequired,defaultValue);
    parameters.emplace_back(ptr);
    return ptr->value;
}
std::shared_ptr<int>       ParParser::addInt(const std::string label, const std::string& desc, bool isRequired, int defaultValue) {
    auto ptr = new IntCont(label,desc,isRequired,defaultValue);
    parameters.emplace_back(ptr);
    return ptr->value;
}
std::shared_ptr<std::string>       ParParser::addString(const std::string label, const std::string& desc, bool isRequired, std::string defaultValue) {
    auto ptr = new StringCont(label,desc,isRequired,defaultValue);
    parameters.emplace_back(ptr);
    return ptr->value;
}
std::shared_ptr<std::vector<double>>  ParParser::addVFloat(const std::string label, const std::string& desc, bool isRequired, const std::vector<double>& defaultValue) {
    auto ptr = new VFloatCont(label,desc,isRequired,defaultValue);
    parameters.emplace_back(ptr);
    return ptr->value;
}
std::shared_ptr<std::vector<int>>  ParParser::addVInt(const std::string label, const std::string& desc, bool isRequired, const std::vector<int>& defaultValue) {
    auto ptr = new VIntCont(label,desc,isRequired,defaultValue);
    parameters.emplace_back(ptr);
    return ptr->value;
}
void ParParser::parse(const std::string& arguments ) {
    //Check to see if there are duplicates
    for(unsigned int iP1 = 0; iP1 < parameters.size(); ++iP1)
        for(unsigned int iP2 = iP1+1; iP2 < parameters.size(); ++iP2){
            if(parameters[iP1]->label != parameters[iP2]->label  ) continue;
            std::cout <<" ++ Repeated parameter:"<<std::endl ;
            std::cout << *parameters[iP1]<<std::endl;
            std::cout << *parameters[iP2]<<std::endl;
            throw std::invalid_argument("ParParser::ParParser() -> Bad parsing");

        }

    auto cannotParse = [](const std::string& label, const std::string& val ){
        std::cout <<" ++ Cannot parse parameter:  '" << label <<"'";
        if(val != "") std::cout <<" with value '"<<val<<"'";
        std::cout << std::endl;
    };

    auto procArg =[&](const std::string& label, const std::string& val){
        bool found = true;
        for(auto& p : parameters ){
            if(label != p->label) continue;
            if(p->wasFilled){
                std::cout <<" ++ Parameter:  '" << label <<"' was filled twice!"<<std::endl;
                throw std::invalid_argument("ParParser::ParParser() -> Bad parsing");
            }
            if(p->useValue){
                if(val == ""){
                    std::cout <<" ++ Parameter was not provided a value:"<<std::endl;
                    std::cout << *p<<std::endl;
                    throw std::invalid_argument("ParParser::ParParser() -> Bad parsing");
                }
                if(!p->fill(val)){
                      std::cout <<" ++ Supplied value ('" << val<<"') cannot be parsed for this parameter:"<<std::endl ;
                      std::cout << *p<<std::endl;
                      throw std::invalid_argument("ParParser::ParParser() -> Bad parsing");
                  }
            } else {
                if( val != "")std::cout <<" ++ Parameter:  '" << label <<"' does not use a value, will ignore '" << val<<"'"<<std::endl ;
                p->fill();
            }
        }
        if(!found) cannotParse(label,val);
    };

    std::istringstream iss(arguments);
    std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},
                          std::istream_iterator<std::string>{}};

    for(unsigned int iA = 0; iA < tokens.size(); ++iA){
        if(!tokens[iA].size()) continue;
        if(tokens[iA].size() == 1 || tokens[iA].compare(0,1,"-")  !=0   ){
            cannotParse(tokens[iA],"");
            continue;
        }
        std::string label = tokens[iA].substr(1);
        //Check if next parameter is value (not another label)
        std::string val = "";
        if(iA + 1 < tokens.size() && tokens[iA+1].size() && tokens[iA+1].compare(0,1,"-")  != 0  ){
            val =  tokens[iA+1];
            iA++;
        }
        procArg(label,val);
    }


    for(const auto& p : parameters){
        if(!p->wasFilled && p->isRequired){
            std::cout <<" ++ Required parameter was not filled:"<<std::endl ;
            std::cout << *p<<std::endl;
            throw std::invalid_argument("ParParser::ParParser() -> Bad parsing");
        }
    }
    if(*verbose){
        std::cout <<" ++ Parameters:"<<std::endl ;
        for(const auto& p : parameters){
            std::cout << *p<<std::endl;
        }
        std::cout <<" ++"<<std::endl ;
    }
}
