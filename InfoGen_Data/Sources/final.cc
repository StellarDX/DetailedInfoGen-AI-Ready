// 测试编译

#include <fmt/format.h>
#include <iostream>
#include <pybind11/pybind11.h>
#include <antlr4-runtime.h>
#include <sstream>
#include <ANTLRFileStream.h>
#include <ANTLRInputStream.h>
#include <CharStream.h>
#include <Lexer.h>
#include <Parser.h>

int InfoGen_Main()
{
    std::string str = "Test";
    std::istringstream is(str);
    antlr4::ANTLRInputStream Test(is);
    std::cout << fmt::format("Hello World\n");
    return 0;
}