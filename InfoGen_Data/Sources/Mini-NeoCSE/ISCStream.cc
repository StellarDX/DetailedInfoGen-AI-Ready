// This file is part of Mini-Neo-CSpaceEngine.
// It is subject to the license terms in the LICENSE file found in the top-level directory

#include <fast_float/fast_float.h>
#include <double-conversion/double-conversion.h>
#include <fstream>
#include <variant>

#include "ISCStream.h"

// 采用Eisel-Lemire+谷歌V8双引擎的字符串到数字转换器（部分为AI生成，腾讯云CodeBuddy提供技术支持）
SEObject ISCStream::NumericToObject(std::string Context)
{
    size_t StartPoint = 0;
    int Base = 10;
    bool Negative = 0;
    bool IsFloat = 0;

    if (!Context.empty() && (Context[0] == '+' || Context[0] == '-'))
    {
        Negative = Context[0] == '-';
        StartPoint = 1;
    }

    if (Context.size() > StartPoint && Context[StartPoint] == '0')
    {
        if (Context.size() > StartPoint + 1)
        {
            switch ((unsigned char)Context[StartPoint + 1] | 0x20)  // 转小写比较
            {
            case 'x':
                Base = 16;
                IsFloat = Context.find_first_of("pP", StartPoint) != std::string::npos;
                if (!IsFloat) {StartPoint += 2;}
                break;
            case 'b':
                StartPoint += 2;
                Base = 2;
                break;
            case 'o':
                StartPoint += 2;
                Base = 8;
                break;
            default:
                // 前导 0 但不是 0x/0b/0o：出现 . e E 才视为浮点
                IsFloat = Context.find_first_of(".eE", StartPoint) != std::string::npos;
                if (!IsFloat)
                {
                    StartPoint += 1;
                    Base = 8;
                }
                break;
            }
        }
    }
    else
    {
        IsFloat = Context.find_first_of(".eE", StartPoint) != std::string::npos;
        Base = 10;
    }

    std::string_view Full(Context.data() + StartPoint, Context.size() - StartPoint);

    auto MatchSuffix = [](std::string_view s, std::string_view Suffix)->bool
    {
        if (s.size() < Suffix.size()) return false;
        auto p = s.data() + s.size() - Suffix.size();
        for (size_t i = 0; i < Suffix.size(); ++i)
        {
            if (((unsigned char)(p[i]) | 0x20) != ((unsigned char)(Suffix[i]) | 0x20)) {return 0;}
        }
        return true;
    };

    if (IsFloat)
    {
        size_t Len = Full.size();
        if (Base == 16)
        {
            enum Suffix { Bin32, Bin64, IEEEExt, Bin128, Bin256, BInfty };
            Suffix Kind = Suffix::Bin64;  // 默认 double
            static const double_conversion::StringToDoubleConverter Converter
            (
                double_conversion::StringToDoubleConverter::ALLOW_TRAILING_JUNK | 
                double_conversion::StringToDoubleConverter::ALLOW_HEX_FLOATS,
                0.0, std::numeric_limits<double>::quiet_NaN(), "inf", "nan"
            );

            if      (MatchSuffix(Full, "infty")) { Kind = Suffix::BInfty; Len -= 5; }
            else if (MatchSuffix(Full, "o")) { Kind = Suffix::Bin256; Len -= 1; }
            else if (MatchSuffix(Full, "q")) { Kind = Suffix::Bin128; Len -= 1; }
            else if (MatchSuffix(Full, "l")) { Kind = Suffix::IEEEExt; Len -= 1; }
            else if (MatchSuffix(Full, "f")) { Kind = Suffix::Bin32; Len -= 1; }

            const char* Begin = Full.data();
            const char* End   = Full.data() + Len;

            int Processed = 0;
            switch (Kind)
            {
            case Suffix::Bin32:
                {
                    return SEObject((Negative ? -1 : 1) *
                        Converter.StringToFloat(Begin, int(Len), &Processed));
                }
            // ......
            case Suffix::Bin64:
            default:[[likely]]
                {
                    return SEObject((Negative ? -1 : 1) *
                        Converter.StringToDouble(Begin, int(Len), &Processed));
                }
            }
        }
        else
        {
            enum Suffix
            { 
                Bin32, Bin64, IEEEExt, Bin128, Bin256, BInfty,
                Dec32, Dec64, Dec128, DInfty
            };
            Suffix Kind = Suffix::Bin64;  // 默认 double

            if      (MatchSuffix(Full, "infty")) { Kind = Suffix::BInfty; Len -= 5; }
            else if (MatchSuffix(Full, "o")) { Kind = Suffix::Bin256; Len -= 1; }
            else if (MatchSuffix(Full, "q")) { Kind = Suffix::Bin128; Len -= 1; }
            else if (MatchSuffix(Full, "l")) { Kind = Suffix::IEEEExt; Len -= 1; }
            else if (MatchSuffix(Full, "f")) { Kind = Suffix::Bin32; Len -= 1; }
            else if (MatchSuffix(Full, "dinfty")) { Kind = Suffix::DInfty; Len -= 1; }
            else if (MatchSuffix(Full, "dq")) { Kind = Suffix::Dec128; Len -= 1; }
            else if (MatchSuffix(Full, "d")) { Kind = Suffix::Dec64; Len -= 1; }
            else if (MatchSuffix(Full, "df")) { Kind = Suffix::Dec32; Len -= 1; }

            const char* Begin = Full.data();
            const char* End   = Full.data() + Len;

            switch (Kind)
            {
            case Suffix::Bin32:
                {
                    float Result = 0;
                    fast_float::from_chars(Begin, End, Result);
                    return SEObject((Negative ? -1 : 1) * Result);
                }
            // ......
            case Suffix::Bin64:
            default:[[likely]]
                {
                    double Result = 0;
                    fast_float::from_chars(Begin, End, Result);
                    return SEObject((Negative ? -1 : 1) * Result);
                }
            }
        }
    }
    else
    {
        size_t Len = Full.size();
        enum Suffix { I64, I32, U32, U64, I128, U128, Infty, UInfty };
        Suffix Kind = Suffix::I64;  // 默认 int64_t
        
        if      (MatchSuffix(Full, "Uinfty")) { Kind = Suffix::UInfty; Len -= 6; }
        else if (MatchSuffix(Full, "infty")) { Kind = Suffix::UInfty; Len -= 5; }
        else if (MatchSuffix(Full, "Ui128")) { Kind = Suffix::U128; Len -= 5; }
        else if (MatchSuffix(Full, "i128"))  { Kind = Suffix::I128; Len -= 4; }
        else if (MatchSuffix(Full, "ULL") || MatchSuffix(Full, "LLU")) { Kind = Suffix::U64; Len -= 3; }
        else if (MatchSuffix(Full, "UL")  || MatchSuffix(Full, "LU"))  { Kind = Suffix::U32; Len -= 2; }
        else if (MatchSuffix(Full, "LL"))  { Kind = Suffix::I64;  Len -= 2; }
        else if (MatchSuffix(Full, "U"))   { Kind = Suffix::U32;  Len -= 1; }
        else if (MatchSuffix(Full, "L"))   { Kind = Suffix::I32;  Len -= 1; }

        const char* Begin = Full.data();
        const char* End   = Full.data() + Len;

        switch (Kind)
        {
        case Suffix::I32:
            { 
                int32_t Result = 0; 
                fast_float::from_chars(Begin, End, Result, Base);
                return SEObject((Negative ? -1 : 1) * Result); 
            }
        case Suffix::U32:
            { 
                uint32_t Result = 0; 
                fast_float::from_chars(Begin, End, Result, Base);
                return SEObject((Negative ? -1 : 1) * Result);
            }
        case Suffix::U64:
            { 
                uint64_t Result = 0; 
                fast_float::from_chars(Begin, End, Result, Base);
                return SEObject((Negative ? -1 : 1) * Result); 
            }
        case Suffix::I128:
            { 
                __int128 Result = 0; 
                fast_float::from_chars(Begin, End, Result, Base);
                return SEObject((Negative ? -1 : 1) * Result);
            }
        case Suffix::U128:
            {
                unsigned __int128 Result = 0; 
                fast_float::from_chars(Begin, End, Result, Base);
                return SEObject((Negative ? -1 : 1) * Result); 
            }
        case Suffix::I64:
        default:[[likely]]
            {
                int64_t Result = 0; 
                fast_float::from_chars(Begin, End, Result, Base);
                return SEObject((Negative ? -1 : 1) * Result); 
            }
        }
    }
    return SEObject();  // 不可达
}

SEObject ISCStream::ArrayBufferToObject(std::vector<TableType::mapped_type::value_type> Context)
{
    enum {Num, Bool, Str} Kind;
    bool Correct = 0;
    if (!Correct)
    {
        Correct = 1;
        Kind = Num;
        try {Context.front().As<SEReal>();}
        catch (...) {Correct = 0;}
    }
    if (!Correct) [[unlikely]]
    {
        Correct = 1;
        Kind = Bool;
        try {Context.front().As<SEBoolean>();}
        catch (...) {Correct = 0;}
    }
    if (!Correct) [[unlikely]]
    {
        Correct = 1;
        Kind = Str;
        try {Context.front().As<SEString>();}
        catch (...) {Correct = 0;}
    }

    switch (Kind)
    {
    case Num:
    default:
        {
            SEArray Arr;
            Arr.resize(Context.size());
            for (size_t i = 0; i < Context.size(); ++i)
            {
                try {Arr[i] = Context[i].As<SEReal>();}
                catch(const std::bad_variant_access& e)
                {
                    Arr[i] = 0.0;
                }
            }
            return SEObject(Arr);
        }
    case Bool:
        {
            SEBArray Arr;
            Arr.resize(Context.size());
            for (size_t i = 0; i < Context.size(); ++i)
            {
                try {Arr[i] = Context[i].As<SEBoolean>();}
                catch(const std::bad_variant_access& e)
                {
                    Arr[i] = false;
                }
            }
            return SEObject(Arr);
        }
    case Str:
        {
            std::vector<std::string> Arr(Context.size());
            for (size_t i = 0; i < Context.size(); ++i)
            {
                try {Arr[i] = Context[i].As<SEString>();}
                catch(const std::bad_variant_access& e)
                {
                    Arr[i] = "";
                }
            }
            return SEObject(Arr);
        }
    }
}

void ISCStream::PostParsing()
{
    Data = TableType(KeyValueBuffer2.begin(), KeyValueBuffer2.end());
    KeyValueBuffer2.clear();
}

void ISCStream::exitKeyLabel(SEParser::KeyLabelContext* Ctx)
{
    KeyValueBuffer1.push({Ctx->Identifier()->getText(), {}});
}

void ISCStream::exitSimpleTypeNumeric(SEParser::SimpleTypeNumericContext* Ctx)
{
    CurrentValue = NumericToObject(Ctx->Numeric()->getText());
}

void ISCStream::exitSimpleTypeString(SEParser::SimpleTypeStringContext* Ctx)
{
    auto Value = Ctx->String()->getText();
    CurrentValue = Value.substr(1, Value.size() - 2); // 截取中间部分，去掉双引号
}

void ISCStream::exitSimpleTypeBoolean(SEParser::SimpleTypeBooleanContext* Ctx)
{
    auto Value = Ctx->Boolean()->getText();
    if ("true" == Value) {CurrentValue = true;}
    else {CurrentValue = false;}
}

void ISCStream::exitArrayStart(SEParser::ArrayStartContext* Ctx)
{
    ArrayValueBuffer.emplace_back(CurrentValue);
}

void ISCStream::exitArrayIterating1(SEParser::ArrayIterating1Context* Ctx)
{
    ArrayValueBuffer.emplace_back(CurrentValue);
}

void ISCStream::exitArrayIterating2(SEParser::ArrayIterating2Context* Ctx)
{
    ArrayValueBuffer.emplace_back(CurrentValue);
}

void ISCStream::exitTupleTerminating1(SEParser::TupleTerminating1Context* Ctx)
{
    TupleValueBuffer.emplace_front(CurrentValueBuffer.top());
    CurrentValueBuffer.pop();
}

void ISCStream::exitTupleTerminating2(SEParser::TupleTerminating2Context* Ctx)
{
    TupleValueBuffer.emplace_front(CurrentValueBuffer.top());
    CurrentValueBuffer.pop();
}

void ISCStream::exitTupleIterating1(SEParser::TupleIterating1Context* Ctx)
{
    TupleValueBuffer.emplace_front(CurrentValueBuffer.top());
    CurrentValueBuffer.pop();
}

void ISCStream::exitTupleIterating2(SEParser::TupleIterating2Context* Ctx)
{
    TupleValueBuffer.emplace_front(CurrentValueBuffer.top());
    CurrentValueBuffer.pop();
}

void ISCStream::exitValueAsRawTypes(SEParser::ValueAsRawTypesContext* Ctx)
{
    CurrentValueBuffer.push(CurrentValue);
}

void ISCStream::exitValueAsArray(SEParser::ValueAsArrayContext* Ctx)
{
    CurrentValueBuffer.push(ArrayBufferToObject(ArrayValueBuffer));
    ArrayValueBuffer.clear();
}

void ISCStream::exitValueAsTuple(SEParser::ValueAsTupleContext* Ctx)
{
    CurrentValueBuffer.push(SETuple(TupleValueBuffer.begin(), TupleValueBuffer.end()));
    TupleValueBuffer.clear();
}

void ISCStream::exitValueGroupTerminating(SEParser::ValueGroupTerminatingContext* Ctx)
{
    KeyValueBuffer1.top().second.emplace_front(CurrentValueBuffer.top());
    CurrentValueBuffer.pop();
}

void ISCStream::exitValueGroupIterating(SEParser::ValueGroupIteratingContext* Ctx)
{
    KeyValueBuffer1.top().second.emplace_front(CurrentValueBuffer.top());
    CurrentValueBuffer.pop();
}

void ISCStream::exitTableWithKeyValue(SEParser::TableWithKeyValueContext* Ctx)
{
    auto& Elem = KeyValueBuffer1.top();
    std::vector<SEObject> Values(Elem.second.begin(), Elem.second.end());
    KeyValueBuffer2.emplace_front(std::pair{Elem.first, Values});
    KeyValueBuffer1.pop();
}

void ISCStream::exitTableWithKeyValueSubtable(SEParser::TableWithKeyValueSubtableContext* Ctx)
{
    auto& Elem = KeyValueBuffer1.top();
    std::vector<SEObject> Values(Elem.second.begin(), Elem.second.end());
    KeyValueBuffer2.emplace_front(std::pair{Elem.first, Values});
    KeyValueBuffer1.pop();
}

void ISCStream::exitTableWithKeyOnly(SEParser::TableWithKeyOnlyContext* Ctx)
{
    auto& Elem = KeyValueBuffer1.top();
    std::vector<SEObject> Values(Elem.second.begin(), Elem.second.end());
    KeyValueBuffer2.emplace_front(std::pair{Elem.first, Values});
    KeyValueBuffer1.pop();
}

void ISCStream::exitTableWithKeySubTable(SEParser::TableWithKeySubTableContext* Ctx)
{
    auto& Elem = KeyValueBuffer1.top();
    std::vector<SEObject> Values(Elem.second.begin(), Elem.second.end());
    KeyValueBuffer2.emplace_front(std::pair{Elem.first, Values});
    KeyValueBuffer1.pop();
}

void ISCStream::exitSubTableExpand(SEParser::SubTableExpandContext* Ctx)
{
    TableType SubTbl(KeyValueBuffer2.begin(), KeyValueBuffer2.end());
    KeyValueBuffer1.top().second.emplace_back(SubTbl);
    KeyValueBuffer2.clear();
}

void ISCStream::exitCompTypeAsIdent(SEParser::CompTypeAsIdentContext* Ctx)
{   
    auto Ptr = std::make_shared<SEObject>(SEObject(Ctx->Identifier()->getText()));
    if (!CondBuffer.Left) {CondBuffer.Left = Ptr;}
    else {CondBuffer.Right = Ptr;}
}

void ISCStream::exitOp(SEParser::OpContext* Ctx)
{
    auto Opt = Ctx->Op()->getText();
    if (Opt == "==") {CondBuffer.Operator = CondBuffer.Equal;}
    else if (Opt == "!=") {CondBuffer.Operator = CondBuffer.NotEq;}
    else if (Opt == "<=") {CondBuffer.Operator = CondBuffer.LessEq;}
    else if (Opt == ">=") {CondBuffer.Operator = CondBuffer.MoreEq;}
    else if (Opt == "<") {CondBuffer.Operator = CondBuffer.Less;}
    else if (Opt == ">") {CondBuffer.Operator = CondBuffer.More;}
}

void ISCStream::exitCompTypeAsRawTypes(SEParser::CompTypeAsRawTypesContext* Ctx)
{
    auto Ptr = std::make_shared<SEObject>(CurrentValue);
    if (!CondBuffer.Left) {CondBuffer.Left = Ptr;}
    else {CondBuffer.Right = Ptr;}
}

void ISCStream::exitModifier(SEParser::ModifierContext* Ctx)
{
    KeyValueBuffer1.push({Ctx->getText(), {}});
}

void ISCStream::exitBoolOperation(SEParser::BoolOperationContext* Ctx)
{
    KeyValueBuffer1.top().second.emplace_front(SEObject(CondBuffer));
    CondBuffer.Left = nullptr;
    CondBuffer.Right = nullptr;
}

void ISCStream::exitVariableOperation(SEParser::VariableOperationContext* Ctx)
{
    SEVarOperation VarOpBuffer; ///< 这个缓冲区存最近一次变量操作（Set，SetForce等）
    VarOpBuffer.Identifier = Ctx->Identifier()->getText();
    VarOpBuffer.Value = std::make_shared<SEObject>(CurrentValue);
    KeyValueBuffer1.top().second.emplace_front(SEObject(VarOpBuffer));
}

void ISCStream::exitTableWithBoolOperation(SEParser::TableWithBoolOperationContext* Ctx)
{
    auto& Elem = KeyValueBuffer1.top();
    std::vector<SEObject> Values(Elem.second.begin(), Elem.second.end());
    KeyValueBuffer2.emplace_front(std::pair{Elem.first, Values});
    KeyValueBuffer1.pop();
}

void ISCStream::exitTableWithVarOperation(SEParser::TableWithVarOperationContext* Ctx)
{
    auto& Elem = KeyValueBuffer1.top();
    std::vector<SEObject> Values(Elem.second.begin(), Elem.second.end());
    KeyValueBuffer2.emplace_front(std::pair{Elem.first, Values});
    KeyValueBuffer1.pop();
}

std::string ANTLRReportTableToString(const std::vector<ANTLRReportRowType>& Data)
{
    if (Data.empty()) return "";
    
    std::vector<std::string> THead =
    {
        "语法规则", 
        "调用次数", 
        "总延迟(ms)", 
        "已查看总Token数", 
        "单次预测最远Token数", 
        "歧义事件数", 
        "DFA缓存未命中计数"
    };
    
    std::vector<size_t> ColWidths(7, 0);
    
    for (size_t i = 0; i < THead.size(); ++i) 
    {
        ColWidths[i] = THead[i].length();
    }

    for (const auto& row : Data)
    {
        ColWidths[0] = std::max(ColWidths[0], std::get<0>(row).length());
        ColWidths[1] = std::max(ColWidths[1], std::to_string(std::get<1>(row)).length());
        ColWidths[2] = std::max(ColWidths[2], fmt::format("{:.6f}", std::get<2>(row)).length());
        ColWidths[3] = std::max(ColWidths[3], std::to_string(std::get<3>(row)).length());
        ColWidths[4] = std::max(ColWidths[4], std::to_string(std::get<4>(row)).length());
        ColWidths[5] = std::max(ColWidths[5], std::to_string(std::get<5>(row)).length());
        ColWidths[6] = std::max(ColWidths[6], std::to_string(std::get<6>(row)).length());
    }
    
    std::string formatStr = "|";
    for (size_t i = 0; i < ColWidths.size(); ++i) 
    {
        formatStr += fmt::format(" {{:>{}}} |", ColWidths[i]);
    }
    formatStr += "\n";
    
    std::string Separator = "+";
    for (size_t w : ColWidths) 
    {
        Separator += std::string(w + 2, '-') + "+";
    }
    Separator += "\n";
    
    std::string Result;
    Result += Separator;
    
    Result += fmt::vformat(formatStr, fmt::make_format_args(
        THead[0], THead[1], THead[2], THead[3], 
        THead[4], THead[5], THead[6]
    ));
    Result += Separator;
    
    for (const auto& row : Data) 
    {
        std::string DelayStr = fmt::format("{:.6f}", std::get<2>(row));
        Result += fmt::vformat(formatStr, fmt::make_format_args(
            std::get<0>(row),
            std::get<1>(row),
            DelayStr,
            std::get<3>(row),
            std::get<4>(row),
            std::get<5>(row),
            std::get<6>(row)
        ));
    }
    Result += Separator;
    
    return Result;
}

std::string GenerateParseReport(antlr4::Parser& Parser)
{
    std::vector<ANTLRReportRowType> Table;
    auto Info = Parser.getParseInfo();
    for (const auto& i : Info.getDecisionInfo())
    {
        auto DecNumber = i.decision;
        auto DState = Parser.getATN().decisionToState.at(DecNumber);
        auto RuleIdx = DState->ruleIndex;
        std::string RuleName = fmt::format("{}:{}", Parser.getRuleNames().at(RuleIdx), DecNumber);

        Table.push_back({RuleName, i.invocations, i.timeInPrediction / 1000000.,
            i.SLL_TotalLook + i.LL_TotalLook, std::max(i.SLL_MaxLook, i.LL_MaxLook),
            i.ambiguities.size(), i.SLL_ATNTransitions});
    }
    return ANTLRReportTableToString(Table);
}

ISCStream ParseFile(std::filesystem::path Path, std::string* Report)
{
    std::ifstream fin(Path);
    antlr4::ANTLRInputStream Input(fin);
    SELexer Lex(&Input);
    antlr4::CommonTokenStream Tokens(&Lex);
    SEParser Parser(&Tokens);
    if (Report) {Parser.setProfile(true);}
    auto ParseTree = Parser.table();
    if (Report) {*Report = GenerateParseReport(Parser);}
    ISCStream ISC;
    antlr4::tree::ParseTreeWalker::DEFAULT.walk(&ISC, ParseTree);
    ISC.PostParsing();
    return ISC;
}