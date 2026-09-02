/*
    适用于ANTLR4的SpaceEngine文件语法
    本语法基于IBM和IEEE754标准制定，不代表SE原版语法

    词类：
    {
        标识符类：（Identifier, Variable）
        {
            标识符：[a-zA-Z_][a-zA-Z0-9_]*
        }

        数字类：（Numeric）
        {
            整数：[+-]?([二进制整数]|[八进制整数]|[十进制整数]|[十六进制整数])(([Ll]{1,2}[Uu]?)|([Uu]([Ll]{1,2})?)|([Uu]?[Ii]128)|([Uu]?[Ii][Nn][Ff][Tt][Yy]))?
             - 二进制整数：0[Bb][0-1]+
             - 八进制整数：0[Oo]?[0-7]+
             - 十进制整数：[:digit:]+
             - 十六进制整数：0[Xx][:xdigit:]+
            二进制/十进制浮点数/科学计数：[+-]?(([:digit:]*\.[:digit:]+[指数e]?)|([:digit:]+\.[指数e]?)|([:digit:]+[指数e]))(([Dd]?[Ff])|[Dd]|[Ll]|([Dd]?[Qq])|[Oo]|([Dd]?[Ii][Nn][Ff][Tt][Yy]))?
             - 指数e：[Ee][+-]?[:digit:]+
            十六进制科学计数：[+-]?0[Xx](([:xdigit:]*\.[:xdigit:]+[指数p])|([:xdigit:]+\.[指数p])|([:xdigit:]+[指数p]))([Ff]|[Ll]|[Qq]|[Oo]|([Ii][Nn][Ff][Tt][Yy]))?
             - 指数p：[Pp][+-]?[:digit:]+
        }
        --注：整数后缀中(U)i128为128位整数，(U)infty为无限长度整数；
            浮点数后缀中的那个d表示十进制浮点数，即df，dl和dq分别为32，64和128位十进制浮点，o后缀为256位浮点（无对应十进制后缀），(d)infty为无限精度浮点
            十六进制科学计数的指数表示为2的p次方，p为十进制数，指数为必填项

        字符串类：（String）
        {
            字符串：\".*\"
        }

        布尔类：（Boolean）
        {
            布尔值：true，false
        }

        操作符类：（Operator）
        {
            比较操作符：==，!=，<，>，<=，>=
        }

        保留关键字：（Modifier）
        {
            变量操作：Set，SetU，SetForce，Reset，Interpolate，Get，WaitVar，SaveVars，RestoreVars
        }
        --注：布尔值与保留关键字区分大小写
    }

    语法：
    {
        Table => Key ValueGroup Table | Key ValueGroup SubTable Key | Key Table | Key SubTable Table | Key '{' BoolOp '}' Table | VariableOp Table | [Empty]
        Key => <Identifier>
        ValueGroup => Value ValueGroup | Value
        BoolOp => ComparableTypes <Operator> ComparableTypes
        VariableOp => <Modifier> <Identifier> SimpleTypes
        Value => SimpleTypes | '(' Array ')' | '{' Tuple '}'
        SubTable => '{' Table '}'
        SimpleTypes => <Numeric> | <String> | <Boolean>
        Array => SimpleTypes | Array ',' SimpleTypes | Array SimpleTypes
        Tuple => Value | Value ',' Tuple | Value Tuple | Value ','
        ComparableTypes => <Identifier> | SimpleTypes
    }
    --注：本语法原是基于Bison的，为LR1语法。
        [Empty]表示空推导，带<>的为终结语词（名称见词类后面的英文表示）
*/

// ----------本文件以下内容由DeepSeek-V4-Flash 0731生成（腾讯云混元3被干烧了）----------

grammar SE;

// ============ 语法规则（Parser Rules） ============

// 这里为了方便区分，每个分支都起了名字

// Table => Key ValueGroup Table | Key ValueGroup SubTable Table | Key Table | Key SubTable Table | Key '{' BoolOp '}' Table | VariableOp Table | [Empty]
table
    : key valueGroup table               # TableWithKeyValue
    | key valueGroup subTable table      # TableWithKeyValueSubtable
    | key table                          # TableWithKeyOnly
    | key subTable table                 # TableWithKeySubTable
    | key '{' boolOp '}' table           # TableWithBoolOperation
    | variableOp table                   # TableWithVarOperation
    | /* [Empty] */                      # TableIsEmpty
    ;

// Key => <Identifier>
key 
    : Identifier                         # KeyLabel
    ;

// ValueGroup => Value ValueGroup | Value
valueGroup
    : value valueGroup                   # ValueGroupIterating
    | value                              # ValueGroupTerminating
    ;

// BoolOp => ComparableTypes <Operator> ComparableTypes
boolOp 
    : comparableTypes op comparableTypes # BoolOperation
    ;

// VariableOp => <Modifier> <Identifier> SimpleTypes
variableOp 
    : modifier Identifier simpleTypes    # VariableOperation
    ;

// Value => SimpleTypes | '(' Array ')' | '{' Tuple '}'
value
    : simpleTypes                        # ValueAsRawTypes
    | '(' array ')'                      # ValueAsArray
    | '{' tuple '}'                      # ValueAsTuple
    ;

// SubTable => '{' Table '}'
subTable 
    : '{' table '}'                      # SubTableExpand
    ;

// SimpleTypes => <Numeric> | <String> | <Boolean>
simpleTypes
    : Numeric                            # SimpleTypeNumeric
    | String                             # SimpleTypeString
    | Boolean                            # SimpleTypeBoolean
    ;

// Array => SimpleTypes | Array ',' SimpleTypes | Array SimpleTypes
array
    : simpleTypes                        # ArrayStart
    | array ',' simpleTypes              # ArrayIterating1
    | array simpleTypes                  # ArrayIterating2
    ;

// Tuple => Value | Value ',' Tuple | Value Tuple | Value ','
tuple
    : value                              # TupleTerminating1
    | value ',' tuple                    # TupleIterating1
    | value tuple                        # TupleIterating2
    | value ','                          # TupleTerminating2
    ;

// ComparableTypes => <Identifier> | SimpleTypes
comparableTypes 
    : Identifier                         # CompTypeAsIdent
    | simpleTypes                        # CompTypeAsRawTypes
    ;

// ============ 词法规则（Lexer Rules） ============

// 保留关键字（区分大小写）
Set        : 'Set';
SetU       : 'SetU';
SetForce   : 'SetForce';
Reset      : 'Reset';
Interpolate: 'Interpolate';
Get        : 'Get';
WaitVar    : 'WaitVar';
SaveVars   : 'SaveVars';
RestoreVars: 'RestoreVars';

modifier
    : Set | SetU | SetForce | Reset | Interpolate | Get | WaitVar | SaveVars | RestoreVars
    ;

// 布尔值（区分大小写）
Boolean : 'true' | 'false';

// 操作符类：比较操作符 ==，!=，<，>，<=，>=
op : Op;
Op : '==' | '!=' | '<=' | '>=' | '<' | '>';

// 数字类
Numeric
    : INT
    | FLOAT
    | HEX_FLOAT
    ;

fragment INT
    : Sign? ( BIN_INT | OCT_INT | DEC_INT | HEX_INT ) INT_SUFFIX?
    ;

fragment BIN_INT : '0' [bB] [0-1]+;
fragment OCT_INT : '0' [oO]? [0-7]+;
fragment DEC_INT : [0-9]+;
fragment HEX_INT : '0' [xX] [0-9a-fA-F]+;

fragment INT_SUFFIX
    : [Ll] [Ll]? [Uu]?      // L, LL, LU, LLU
    | [Uu] [Ll] [Ll]?       // U, UL, ULL
    | [Uu]? [Ii] '128'      // i128, Ui128
    | [Uu]? [Ii] [Nn] [Ff] [Tt] [Yy]  // infty, Uinfty
    ;

fragment FLOAT
    : Sign? (
          DIGITS? '.' DIGITS+ EXPONENT?
        | DIGITS+ '.' EXPONENT?
        | DIGITS+ EXPONENT
      ) FLOAT_SUFFIX?
    ;

fragment DIGITS : [0-9]+;

fragment EXPONENT : [eE] Sign? DIGITS;

fragment FLOAT_SUFFIX
    : [Dd]? [Ff]            // f, df
    | [Dd]                  // d
    | [Ll]                  // l
    | [Dd]? [Qq]            // q, dq
    | [Oo]                  // o
    | [Dd]? [Ii] [Nn] [Ff] [Tt] [Yy]  // infty, dinfty
    ;

fragment HEX_FLOAT
    : Sign? '0' [xX] (
          XDIGITS? '.' XDIGITS+ HEX_EXPONENT
        | XDIGITS+ '.' HEX_EXPONENT
        | XDIGITS+ HEX_EXPONENT
      ) HEX_FLOAT_SUFFIX?
    ;

fragment XDIGITS : [0-9a-fA-F]+;

fragment HEX_EXPONENT : [pP] Sign? DIGITS;

fragment HEX_FLOAT_SUFFIX
    : [Ff] | [Ll] | [Qq] | [Oo] | [Ii] [Nn] [Ff] [Tt] [Yy]
    ;

fragment Sign : '+' | '-';

// 字符串类
String : '"' .*? '"';

// 标识符类
Identifier : [a-zA-Z_] [a-zA-Z0-9_]*;

// 空白与注释
WS           : [ \t\r\n]+ -> skip;
LINE_COMMENT : '//' ~[\r\n]* -> skip;
BLOCK_COMMENT : '/*' .*? '*/' -> skip;
