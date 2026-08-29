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
            - 十进制整数：[1-9][:digit:]*
            - 十六进制整数：0[Xx][:xdigit:]+
           二进制/十进制浮点数/科学计数：[+-]?(([:digit:]*\.[:digit:]+[指数e]?)|([:digit:]+\.[指数e]?)|([:digit:]+[指数e]))(([Dd]?[Ff])|([Dd]?[Ll])|([Dd]?[Qq])|[Oo]|([Dd]?[Ii][Nn][Ff][Tt][Yy]))?
            - 指数e：[Ee][+-]?[:digit:]+
           十六进制科学计数：[+-]?0[Xx](([:xdigit:]*\.[:xdigit:]+[指数p]?)|([:xdigit:]+\.[指数p]?)|([:xdigit:]+[指数p]))([Ff]|[Ll]|[Qq]|[Oo]|([Ii][Nn][Ff][Tt][Yy]))?
            - 指数p：[Pp][+-]?[:digit:]+
       }
       --注：整数后缀中(U)i128为128位整数，(U)infty为无限长度整数；
           浮点数后缀中的那个d表示十进制浮点数，即df，dl和dq分别为32，64和128位十进制浮点，o后缀为256位浮点（无对应十进制后缀），(d)infty为无限精度浮点
           十六进制科学计数的指数表示为2的p次方，p为十进制数

       字符串类：（String）
       {
           字符串：\".*\"
       }

       布尔类：（Boolean）
       {
           布尔值：(true)|(false)
       }

       操作符类：（Operator）
       {
           比较操作符：((<=?)|(>=?)|([!=]=))
       }
   }

   语法：
   {
       Table => Key ValueGroup Table | Key '{' BoolOp '}' Table | Key Table | [Empty]
       Key => <Identifier>
       ValueGroup => Value ValueGroup | Value | SubTable
       BoolOp => SimpleTypes <Operator> SimpleTypes
       Value => SimpleTypes | '(' Array ')' | '{' Tuple '}'
       SubTable => '{' Table '}'
       SimpleTypes => <Numeric> | <String> | <Boolean> | <Variable>
       Array = SimpleTypes | Array ',' SimpleTypes | Array SimpleTypes
       Tuple = Value | Value ',' Tuple | Value Tuple | Value ','
   }
   --注：本语法原是基于Bison的，为LR1语法。
         [Empty]表示空推导，带<>的表示终结语词（名称见词类后面的英文表示）

    |----------本文件以下内容由DeepSeek-V4-Flash 0731生成（腾讯云混元3被干烧了）----------|

   以下是本文件实际的ANTLR4实现：
   ----------------------------------------------------------------------------
   实现说明：
   1. 词法层
      - 词类中（Identifier, Variable）在词法上均为 IDENT（上下文相关语义）
      - 数字类拆分为 INTEGER / DEC_FLOAT / HEX_FLOAT 三个终结符（子规则用 fragment 表达）
      - 字符串类为 STRING，布尔类为 BOOL，比较操作符为 OP
      - 词法规则按“最长匹配优先、先定义者优先”排序，因此 BOOL / OP 必须排在 IDENT 之前

   2. 语法层（原 LR1 语法改写为 ANTLR4 可接受的 LL/闭包形式）
      - Table 的三种推导（带值组 / 带布尔块 / 裸键）合并为 table 规则中的可选分支
      - ValueGroup 的右递归改为闭包 value+，子表单独作为备选
      - Array / Tuple 的左递归改为闭包形式
      - 子表 SubTable（'{' Table '}'）与值中元组 Tuple（'{' Tuple '}'）均以 '{' 开头，
        存在固有歧义；ANTLR4 的 ALL(*) 自适应预测会按规则顺序与前瞻作出选择。
        如需调整优先级，可调整 value 与 valueGroup 中备选项的先后顺序。
   ----------------------------------------------------------------------------
*/

grammar SE;

/*
 * ============================ 语法规则（Parser） ============================
 */

// 顶层入口：一个完整的表后必须到达输入末尾
start
    : table EOF
    ;

// Table => Key ValueGroup Table | Key '{' BoolOp '}' Table | Key Table | [Empty]
// 注：原 [Empty] 空推导在 ANTLR 中由闭包 `*` 自然表达；
//     table 本身不含 EOF，以便嵌套在子表（subTable）中复用
table
    : entry*
    ;

// 表中每一项：带值组 / 带布尔块 / 裸键（对应原 Table 的三种 Key 推导）
entry
    : key valueGroup
    | key boolBlock
    | key
    ;

// Key => <Identifier>
key
    : IDENT
    ;

// ValueGroup => Value ValueGroup | Value | SubTable
// 改写为：一个或多个 Value 的闭包；或一个子表
valueGroup
    : value+
    | subTable
    ;

// Key '{' BoolOp '}' Table 中的布尔块
boolBlock
    : '{' boolOp '}'
    ;

// BoolOp => SimpleTypes <Operator> SimpleTypes
boolOp
    : simpleTypes OP simpleTypes
    ;

// Value => SimpleTypes | '(' Array ')' | '{' Tuple '}'
value
    : simpleTypes
    | '(' array ')'
    | '{' tuple '}'
    ;

// SubTable => '{' Table '}'
subTable
    : '{' table '}'
    ;

// SimpleTypes => <Numeric> | <String> | <Boolean> | <Variable>
simpleTypes
    : numeric
    | STRING
    | BOOL
    | variable
    ;

// <Numeric>：整数或两类浮点数
numeric
    : INTEGER
    | DEC_FLOAT
    | HEX_FLOAT
    ;

// <Variable>：词法上即标识符
variable
    : IDENT
    ;

// Array = SimpleTypes | Array ',' SimpleTypes | Array SimpleTypes
// 改写为闭包：SimpleTypes (',' SimpleTypes | SimpleTypes)*
array
    : simpleTypes (COMMA? simpleTypes)*
    ;

// Tuple = Value | Value ',' Tuple | Value Tuple | Value ','
// 改写为闭包：Value (COMMA? Value)* COMMA?
tuple
    : value (COMMA? value)* COMMA?
    ;

/*
 * ============================ 词法规则（Lexer） ============================
 */

// 空白与注释（跳过）
WS      : [ \t\r\n]+ -> skip;
LINE_COMMENT : '//' ~[\r\n]* -> skip;
BLOCK_COMMENT : '/*' .*? '*/' -> skip;

// 标点
LPAREN  : '(';
RPAREN  : ')';
LBRACE  : '{';
RBRACE  : '}';
COMMA   : ',';

// 布尔类：（Boolean）(true)|(false)
// 注意：必须排在 IDENT 之前，否则 true/false 会被识别为标识符
BOOL    : 'true' | 'false';

// 操作符类：（Operator）((<=?)|(>=?)|([!=]=)) 即 < <= > >= != ==
OP      : '<=' | '>=' | '!=' | '==' | '<' | '>';

// 标识符类：（Identifier, Variable）[a-zA-Z_][a-zA-Z0-9_]*
IDENT   : [a-zA-Z_][a-zA-Z0-9_]*;

// 字符串类：（String）".*"（非贪婪，支持内含转义）
STRING  : '"' (ESC | ~["\\])* '"';
fragment ESC : '\\' .;

// 数字类（Numeric）
// 整数：[+-]?([二进制整数]|[八进制整数]|[十进制整数]|[十六进制整数]) + 可选后缀
INTEGER
    : SIGN? (BIN_INT | OCT_INT | DEC_INT | HEX_INT) INTEGER_SUFFIX?
    ;
fragment SIGN      : [+-];
fragment DIGIT     : [0-9];
fragment XDIGIT    : [0-9a-fA-F];

// 二进制整数：0[Bb][0-1]+
fragment BIN_INT   : '0'[Bb][01]+;
// 八进制整数：0[Oo]?[0-7]*（允许单独的数字 0）
fragment OCT_INT   : '0'[Oo]?[0-7]*;
// 十进制整数：[1-9][:digit:]*
fragment DEC_INT   : [1-9]DIGIT*;
// 十六进制整数：0[Xx][:xdigit:]+
fragment HEX_INT   : '0'[Xx]XDIGIT+;

// 整数后缀：((Ll){1,2}[Uu]?) | ([Uu](Ll){1,2}?) | ([Uu]?[Ii]128) | ([Uu]?[Ii][Nn][Ff][Tt][Yy])
fragment INTEGER_SUFFIX
    : ([Ll]|[Ll][Ll]|[Ll][Uu]|[Ll][Ll][Uu])
    | ([Uu]|[Uu][Ll]|[Uu][Ll][Ll])
    | ([Ii]'128'|[Uu][Ii]'128')
    | ([Ii][Nn][Ff][Tt][Yy]|[Uu][Ii][Nn][Ff][Tt][Yy])
    ;

// 二进制/十进制浮点数/科学计数：
// [+-]?(([:digit:]*\.[:digit:]+[指数e]?)|([:digit:]+\.[指数e]?)|([:digit:]+[指数e])) + 可选后缀
DEC_FLOAT
    : SIGN? ((DIGIT* '.' DIGIT+ DEC_EXP?) | (DIGIT+ '.' DEC_EXP?) | (DIGIT+ DEC_EXP)) DEC_FLOAT_SUFFIX?
    ;
// 指数e：[Ee][+-]?[:digit:]+
fragment DEC_EXP  : [Ee] SIGN? DIGIT+;
// 浮点后缀：(([Dd]?[Ff])|([Dd]?[Ll])|([Dd]?[Qq])|[Oo]|([Dd]?[Ii][Nn][Ff][Tt][Yy]))
fragment DEC_FLOAT_SUFFIX
    : ([Dd]?[Ff]) | ([Dd]?[Ll]) | ([Dd]?[Qq]) | [Oo] | ([Dd]?[Ii][Nn][Ff][Tt][Yy])
    ;

// 十六进制科学计数：
// [+-]?0[Xx](([:xdigit:]*\.[:xdigit:]+[指数p]?)|([:xdigit:]+\.[指数p]?)|([:xdigit:]+[指数p])) + 可选后缀
HEX_FLOAT
    : SIGN? '0'[Xx] ((XDIGIT* '.' XDIGIT+ HEX_EXP?) | (XDIGIT+ '.' HEX_EXP?) | (XDIGIT+ HEX_EXP)) HEX_FLOAT_SUFFIX?
    ;
// 指数p：[Pp][+-]?[:digit:]+
fragment HEX_EXP  : [Pp] SIGN? DIGIT+;
// 十六进制后缀：([Ff]|[Ll]|[Qq]|[Oo]|([Ii][Nn][Ff][Tt][Yy]))
fragment HEX_FLOAT_SUFFIX
    : [Ff] | [Ll] | [Qq] | [Oo] | [Ii][Nn][Ff][Tt][Yy]
    ;
