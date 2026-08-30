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
using SETable   = std::flat_map<SEKey, SEValues>;

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
    SEString Operation;
    SEString Identifier;
    std::shared_ptr<class SEObject> Value;
};

class SEObject
{
public:
    using ValueType = std::variant<
        SEInteger, SEUInt, SEReal, SEBoolean, SEString,
        SEArray, SEIArray, SEUArray, SEBArray, 
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
    SEObject(SETuple M) : Elem(M) {}
    SEObject(SETable T) : Elem(T) {}
    SEObject(SECondition C) : Elem(C) {}
    SEObject(SEVarOperation V) : Elem(V) {}

    template<typename Tp> 
    requires (std::is_integral_v<Tp> || std::is_floating_point_v<Tp>)
    Tp AsNumeric()const
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

    SEBoolean AsBoolean()const
    {
        if (std::holds_alternative<SEBoolean>(Elem))
        {
            return std::get<SEBoolean>(Elem);
        }
        else {throw std::bad_variant_access();}
    }

    SEString AsString()const
    {
        if (std::holds_alternative<SEString>(Elem))
        {
            return std::get<SEString>(Elem);
        }
        else {throw std::bad_variant_access();}
    }

    template<typename Tp>
    Eigen::Array<Tp, Eigen::Dynamic, 1> AsArray()const
    {
        return std::visit([](auto&& v) -> Tp 
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

    SETuple AsTuple()const
    {
        if (std::holds_alternative<SETuple>(Elem))
        {
            return std::get<SETuple>(Elem);
        }
        else {throw std::bad_variant_access();}
    }

    SETable AsTable()const
    {
        if (std::holds_alternative<SETable>(Elem))
        {
            return std::get<SETable>(Elem);
        }
        else {throw std::bad_variant_access();}
    }

    SECondition AsCondition()const
    {
        if (std::holds_alternative<SECondition>(Elem))
        {
            return std::get<SECondition>(Elem);
        }
        else {throw std::bad_variant_access();}
    }

    SEVarOperation AsVariableOperation()const
    {
        if (std::holds_alternative<SEVarOperation>(Elem))
        {
            return std::get<SEVarOperation>(Elem);
        }
        else {throw std::bad_variant_access();}
    }
};


class ISCStream : public SEBaseListener
{
public:
    using Mybase        = SEBaseListener;
    using ValueType     = SEObject;
    using Reference     = ValueType&;
    using ConstRef      = const ValueType&;

    using TableType     = SETable;
    using KeyValueType  = TableType::value_type;
    using TempTableType = std::stack<KeyValueType>;
    using TempValueArr  = std::stack<SEObject>;

protected:
    SEObject Data; ///< 最终数据保存在这里

    // 以下所有变量均为中间状态
    std::stack<TempTableType> CurrentTable;
    TempValueArr CurrentValues;

public:
    void enterTable(SEParser::TableContext* Ctx) override;
    void exitTable(SEParser::TableContext* Ctx) override;

    void enterKey(SEParser::KeyContext* Ctx) override;
    void exitKey(SEParser::KeyContext* Ctx) override;

    void enterValueGroup(SEParser::ValueGroupContext* Ctx) override;
    void exitValueGroup(SEParser::ValueGroupContext* Ctx) override;

    void enterBoolOp(SEParser::BoolOpContext* Ctx) override;
    void exitBoolOp(SEParser::BoolOpContext* Ctx) override;

    void enterVariableOp(SEParser::VariableOpContext* Ctx) override;
    void exitVariableOp(SEParser::VariableOpContext* Ctx) override;

    void enterValue(SEParser::ValueContext* Ctx) override;
    void exitValue(SEParser::ValueContext* Ctx) override;

    void enterSubTable(SEParser::SubTableContext* Ctx) override;
    void exitSubTable(SEParser::SubTableContext* Ctx) override;

    void enterNumeric(SEParser::NumericContext* Ctx) override;
    void exitNumeric(SEParser::NumericContext* Ctx) override;

    void enterString(SEParser::StringContext* Ctx) override;
    void exitString(SEParser::StringContext* Ctx) override;

    void enterBoolean(SEParser::BooleanContext* Ctx) override;
    void exitBoolean(SEParser::BooleanContext* Ctx) override;

    void enterArray(SEParser::ArrayContext* Ctx) override;
    void exitArray(SEParser::ArrayContext* Ctx) override;

    void enterTuple(SEParser::TupleContext* Ctx) override;
    void exitTuple(SEParser::TupleContext* Ctx) override;

    void enterComparableTypes(SEParser::ComparableTypesContext* Ctx) override;
    void exitComparableTypes(SEParser::ComparableTypesContext* Ctx) override;

    void enterModifier(SEParser::ModifierContext* Ctx) override;
    void exitModifier(SEParser::ModifierContext* Ctx) override;

    void enterOp(SEParser::OpContext* Ctx) override;
    void exitOp(SEParser::OpContext* Ctx) override;

    void enterEveryRule(antlr4::ParserRuleContext* Ctx) override;
    void exitEveryRule(antlr4::ParserRuleContext* Ctx) override;
    void visitTerminal(antlr4::tree::TerminalNode* Node) override;
    void visitErrorNode(antlr4::tree::ErrorNode* Node) override;

    ConstRef GetObject()const{return Data;}
};

#endif