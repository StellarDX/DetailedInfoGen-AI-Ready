#include "Mini-NeoCSE/ISCStream.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

void Require(bool condition, const char* message)
{
    if (!condition) throw std::runtime_error(message);
}

int main(int argc, char** argv)
{
    auto path=std::filesystem::temp_directory_path()/
        ("infogen-parser-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count())+".sc");
    struct Cleanup { std::filesystem::path path; ~Cleanup() {std::filesystem::remove(path);} } cleanup{path};
    try
    {
        Require(argc>=2,"missing test case");
        std::string test=argv[1];
        if(test=="real")
        {
            Require(argc==3||argc==4,"missing real fixture");
            auto expected=argc==4?std::stoull(argv[3]):1;
            Require(ParseFile(argv[2]).GetObject().As<SETable>().size()==expected,"real fixture object count changed");
        }
        else
        {
            std::string body="Planet \"body\" { Value 1 }";
            if(test=="invalid-token") body="Planet \"body\" { Radius -1.#IND }";
            if(test=="trailing") body+=" )";
            if(test=="real-overflow") body="Planet \"body\" { Value 1e999 }";
            if(test=="int-overflow") body="Planet \"body\" { Value 18446744073709551616ULL }";
            if(test=="signed-min") body="Planet \"body\" { Value -9223372036854775808 }";
            if(test=="valid-values") body="Planet \"body\" { Value -17 Real 1.25 }";
            if(test=="repeated-blocks") body="Planet \"body\" { Life { Class \"Organic\" } Life { Class \"Exotic\" } }";
            if(test!="missing") {std::ofstream file(path);file<<body;}
            bool invalid=test=="missing"||test=="invalid-token"||test=="trailing"||test=="real-overflow"||test=="int-overflow";
            if(invalid)
            {
                bool rejected=false;
                try {ParseFile(path);} catch(const std::exception&) {rejected=true;}
                Require(rejected,"invalid or partial source must be rejected");
            }
            else
            {
                auto raw=ParseFile(path).GetObject().As<SETable>().begin()->second[1].As<SETable>();
                if(test=="signed-min") Require(raw.find("Value")->second[0].As<SEInteger>()==std::numeric_limits<SEInteger>::min(),"signed minimum was silently changed");
                if(test=="valid-values") Require(raw.find("Value")->second[0].As<SEInteger>()==-17&&raw.find("Real")->second[0].As<SEReal>()==1.25,"valid scalar changed");
                if(test=="repeated-blocks") Require(raw.count("Life")==2,"repeated source block lost");
            }
        }
        std::cout<<"PASS "<<test<<'\n';
        return 0;
    }
    catch(const std::exception& error)
    {
        std::cerr<<"FAIL "<<error.what()<<'\n';
        return 1;
    }
}
