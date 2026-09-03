// This file is part of Mini-Neo-CSpaceEngine.
// It is subject to the license terms in the LICENSE file found in the top-level directory

#pragma once

#ifndef __ISCSTREAM__
#define __ISCSTREAM__

// 由于SE的原生类型都是已经确定的，因此可以使用Variant模式
#include <variant>
#include <flat_map>
#include <concepts>
#include <type_traits>
#include <filesystem>

#include <fmt/format.h>
#include <Eigen/Core>

#include "./ANTLR/SELexer.h"
#include "./ANTLR/SEParser.h"
#include "./ANTLR/SEBaseListener.h"

using SEInteger = int64_t;
using SEUInt    = uint64_t;
using SEReal    = double_t;
using SEBoolean = bool;
using SEString  = std::string;

using SEVec2    = Eigen::Array<SEReal, 2, 1>;
using SEVec3    = Eigen::Array<SEReal, 2, 1>;
using SEVec4    = Eigen::Array<SEReal, 2, 1>;
using SEIVec2   = Eigen::Array<SEInteger, 2, 1>;
using SEIVec3   = Eigen::Array<SEInteger, 3, 1>;
using SEIVec4   = Eigen::Array<SEInteger, 4, 1>;
using SEUVec2   = Eigen::Array<SEUInt, 2, 1>;
using SEUVec3   = Eigen::Array<SEUInt, 3, 1>;
using SEUVec4   = Eigen::Array<SEUInt, 4, 1>;
using SEBVec2   = Eigen::Array<SEBoolean, 2, 1>;
using SEBVec3   = Eigen::Array<SEBoolean, 3, 1>;
using SEBVec4   = Eigen::Array<SEBoolean, 4, 1>;

using SEArray   = Eigen::Array<SEReal, Eigen::Dynamic, 1>;
using SEIArray  = Eigen::Array<SEInteger, Eigen::Dynamic, 1>;
using SEUArray  = Eigen::Array<SEUInt, Eigen::Dynamic, 1>;
using SEBArray  = Eigen::Array<SEBoolean, Eigen::Dynamic, 1>;

using SEMat2    = Eigen::Matrix<SEReal, 2, 2>;
using SEMat3    = Eigen::Matrix<SEReal, 3, 3>;
using SEMat4    = Eigen::Matrix<SEReal, 4, 4>;

using SEMat2x2  = SEMat2;
using SEMat2x3  = Eigen::Matrix<SEReal, 3, 2>;
using SEMat2x4  = Eigen::Matrix<SEReal, 4, 2>;
using SEMat3x2  = Eigen::Matrix<SEReal, 2, 3>;
using SEMat3x3  = SEMat3;
using SEMat3x4  = Eigen::Matrix<SEReal, 4, 3>;
using SEMat4x2  = Eigen::Matrix<SEReal, 2, 4>;
using SEMat4x3  = Eigen::Matrix<SEReal, 3, 4>;
using SEMat4x4  = SEMat4;

using SEMatrix  = Eigen::Matrix<SEReal, Eigen::Dynamic, Eigen::Dynamic>;

using SETuple   = std::vector<class SEObject>;

using SEKey     = SEString;
using SEValues  = std::vector<class SEObject>;
using SETable   = std::multimap<SEKey, SEValues>; // 主要用于读取物体而非顺序执行，因此采用哈希字典

struct SECondition
{
    std::shared_ptr<class SEObject> Left; // 直接声明会导致循环依赖
    enum OperatorType
    {
        Less   = 0b001,
        More   = 0b010,
        Equal  = 0b100,
        NotEq  = 0b000,
        LessEq = 0b101,
        MoreEq = 0b110,
    }Operator;
    std::shared_ptr<class SEObject> Right;
};

struct SEVarOperation
{
    SEString Identifier;
    std::shared_ptr<class SEObject> Value;
};

class SEObject
{
public:
    using ValueType = std::variant<
        SEInteger, SEUInt, SEReal, SEBoolean, SEString,
        SEArray, SEIArray, SEUArray, SEBArray, std::vector<std::string>,
        SETuple, SETable, SECondition, SEVarOperation>;

    ValueType Elem;

    SEObject() {}

    template<std::integral IntTp>
    SEObject(IntTp IntType)
    {
        if constexpr (std::is_unsigned_v<IntTp>)
        {
            Elem = SEUInt(IntType);
        }
        else
        {
            Elem = SEInteger(IntType);
        }
    }
    template<std::floating_point FltTp>
    SEObject(FltTp FltType) : Elem(SEReal(FltType)) {}
    SEObject(const std::string& Str) : Elem(Str) {}
    SEObject(bool B) : Elem(B) {}
    template<typename Tp>
    SEObject(Eigen::Array<Tp, Eigen::Dynamic, 1> Arr) : Elem(Arr) {}
    SEObject(std::vector<std::string> Arr) : Elem(Arr) {}
    SEObject(SETuple M) : Elem(M) {}
    SEObject(SETable T) : Elem(T) {}
    SEObject(SECondition C) : Elem(C) {}
    SEObject(SEVarOperation V) : Elem(V) {}

    template<typename Tp> 
    requires (std::is_integral_v<Tp> || std::is_floating_point_v<Tp>)
    Tp As()const
    {
        return std::visit([](auto&& v) -> Tp 
        {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, SEInteger> ||
                std::is_same_v<T, SEUInt> ||
                std::is_same_v<T, SEReal>)
            {
                return Tp(v);
            }
            else {throw std::bad_variant_access();}
        }, Elem);
    }

    template<typename Tp>
    Eigen::Array<Tp, Eigen::Dynamic, 1> As(std::type_identity<Eigen::Array<Tp, Eigen::Dynamic, 1>>)const
    {
        return std::visit([](auto&& v) -> Eigen::Array<Tp, Eigen::Dynamic, 1>
        {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, SEArray> ||
                std::is_same_v<T, SEIArray> ||
                std::is_same_v<T, SEUArray> ||
                std::is_same_v<T, SEBArray>)
            {
                return Eigen::Array<Tp, Eigen::Dynamic, 1>(v);
            }
            else {throw std::bad_variant_access();}
        }, Elem);
    }

    template<typename Tp>
    Tp As()const
    {
        if (std::holds_alternative<Tp>(Elem))
        {
            return std::get<Tp>(Elem);
        }
        else {throw std::bad_variant_access();}
    }
};

class ISCStream : public SEBaseListener
{
public:
    using Mybase    = SEBaseListener;
    using ValueType = SEObject;
    using Reference = ValueType&;
    using ConstRef  = const ValueType&;
    using TableType = SETable;

protected:
    SEObject Data; ///< 最终数据保存在这里

    // 以下所有变量均为中间状态，访问它们得到的结果是未定义的
    std::stack<std::pair<SEString, std::deque<SEObject>>> KeyValueBuffer1; ///< 这个缓冲区存储已有的键值对
    std::deque<TableType::value_type> KeyValueBuffer2; ///< 这个缓冲区存储即将构造为表的键值对

    std::stack<TableType::mapped_type::value_type> CurrentValueBuffer; ///< 这个缓冲区中存储值
    TableType::mapped_type::value_type CurrentValue; ///< 这个缓冲区用来存放分析好的单值
    std::vector<TableType::mapped_type::value_type> ArrayValueBuffer; ///< 这个缓冲区中存储数组里的值
    std::deque<TableType::mapped_type::value_type> TupleValueBuffer; ///< 这个缓冲区中存储即将被构造为tuple的值

    SECondition CondBuffer; ///< 这个缓冲区存最近一次的布尔变量操作（即if语句）

    static SEObject NumericToObject(std::string Context);
    static SEObject ArrayBufferToObject(std::vector<TableType::mapped_type::value_type> Context);

    void PostParsing();

public:
    ISCStream() {}

    // 以下是语义分析时用的中间函数，此处依然采用和以前版本一样的自下而上归约的策略
    void exitKeyLabel(SEParser::KeyLabelContext* Ctx) override final; // <Identifier> <= Key

    void exitSimpleTypeNumeric(SEParser::SimpleTypeNumericContext* Ctx) override final; // <Numeric> <= SimpleTypes
    void exitSimpleTypeString(SEParser::SimpleTypeStringContext* Ctx) override final; // <String> <= SimpleTypes
    void exitSimpleTypeBoolean(SEParser::SimpleTypeBooleanContext* Ctx) override final; // <Boolean> <= SimpleTypes

    void exitArrayStart(SEParser::ArrayStartContext* Ctx) override final; // SimpleTypes <= Array
    void exitArrayIterating1(SEParser::ArrayIterating1Context* Ctx) override final; // Array ',' SimpleTypes <= Array
    void exitArrayIterating2(SEParser::ArrayIterating2Context* Ctx) override final; // Array SimpleTypes <= Array

    void exitTupleTerminating1(SEParser::TupleTerminating1Context* Ctx) override final; // Value <= Tuple
    void exitTupleTerminating2(SEParser::TupleTerminating2Context* Ctx) override final; // Value ',' <= Tuple
    void exitTupleIterating1(SEParser::TupleIterating1Context* Ctx) override final; // Value ',' Tuple <= Tuple
    void exitTupleIterating2(SEParser::TupleIterating2Context* Ctx) override final; // Value Tuple <= Tuple

    void exitValueAsRawTypes(SEParser::ValueAsRawTypesContext* Ctx) override final; // SimpleTypes <= Value
    void exitValueAsArray(SEParser::ValueAsArrayContext* Ctx) override final; // '(' Array ')' <= Value
    void exitValueAsTuple(SEParser::ValueAsTupleContext* Ctx) override final; // '{' Tuple '}' <= Value

    void exitValueGroupTerminating(SEParser::ValueGroupTerminatingContext* Ctx) override final; // Value <= ValueGroup
    void exitValueGroupIterating(SEParser::ValueGroupIteratingContext* Ctx) override final; // Value ValueGroup <= ValueGroup

    void exitTableWithKeyValue(SEParser::TableWithKeyValueContext* Ctx) override final; // Key ValueGroup Table <= Table
    void exitTableWithKeyValueSubtable(SEParser::TableWithKeyValueSubtableContext* Ctx) override final; // Key ValueGroup SubTable Table <= Table
    void exitTableWithKeyOnly(SEParser::TableWithKeyOnlyContext* Ctx) override final; // Key Table <= Table
    void exitTableWithKeySubTable(SEParser::TableWithKeySubTableContext* Ctx) override final; // Key SubTable Table <= Table

    void exitSubTableExpand(SEParser::SubTableExpandContext* Ctx) override final; // '{' Table '}' <= SubTable

    // 为了保证逻辑完整，以下过程仍会正常进行，但是最终得到的结果是未定义的，
    // 要保留顺序请使用ISEStream（命令行模式，最终数据结构为线性表，TODO）而不是ISCStream（数据模式，最终数据结构为哈希表）

    void exitCompTypeAsIdent(SEParser::CompTypeAsIdentContext* Ctx) override final; // <Identifier> <= ComparableTypes
    void exitOp(SEParser::OpContext* Ctx) override final; // 读取操作符
    void exitCompTypeAsRawTypes(SEParser::CompTypeAsRawTypesContext* Ctx) override final; // SimpleTypes <= ComparableTypes

    void exitModifier(SEParser::ModifierContext* Ctx) override final; // 读取关键字

    void exitBoolOperation(SEParser::BoolOperationContext* Ctx) override final; // ComparableTypes <Operator> ComparableTypes <= BoolOp

    void exitVariableOperation(SEParser::VariableOperationContext* Ctx) override final; // <Modifier> <Identifier> SimpleTypes <= VariableOp

    void exitTableWithBoolOperation(SEParser::TableWithBoolOperationContext* Ctx) override final; // Key '{' BoolOp '}' Table <= Table
    void exitTableWithVarOperation(SEParser::TableWithVarOperationContext* Ctx) override final; // VariableOp Table <= Table

    ConstRef GetObject()const{return Data;}

    friend ISCStream ParseFile(std::filesystem::path Path, std::string* Report);
};

using ANTLRReportRowType = std::tuple<std::string, int, double, int, int, int, int>;

std::string ANTLRReportTableToString(const std::vector<ANTLRReportRowType>& Data);
std::string GenerateParseReport(antlr4::Parser& Parser);

ISCStream ParseFile(std::filesystem::path Path, std::string* Report = nullptr);

#endif