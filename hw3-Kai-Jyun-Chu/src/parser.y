%{
#include "AST/BinaryOperator.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/FunctionInvocation.hpp"
#include "AST/UnaryOperator.hpp"
#include "AST/VariableReference.hpp"
#include "AST/assignment.hpp"
#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/expression.hpp"
#include "AST/for.hpp"
#include "AST/function.hpp"
#include "AST/if.hpp"
#include "AST/print.hpp"
#include "AST/program.hpp"
#include "AST/read.hpp"
#include "AST/return.hpp"
#include "AST/variable.hpp"
#include "AST/while.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "AST/AstDumper.hpp"

#include <vector>

#define YYLTYPE yyltype

typedef struct YYLTYPE {
    uint32_t first_line;
    uint32_t first_column;
    uint32_t last_line;
    uint32_t last_column;
} yyltype;

extern uint32_t line_num;   /* declared in scanner.l */
extern char current_line[]; /* declared in scanner.l */
extern FILE *yyin;          /* declared by lex */
extern char *yytext;        /* declared by lex */

static AstNode *root;

extern "C" int yylex(void);
static void yyerror(const char *msg);
extern int yylex_destroy(void);
%}

// This guarantees that headers do not conflict when included together.
%define api.token.prefix {TOK_}

%code requires {
    class AstNode;
    class ProgramNode;
    class DeclNode;
    class FunctionNode;
    class VariableNode;
    class ConstantValueNode;
    class CompoundStatementNode;
    class PrintNode;
    class BinaryOperatorNode;
    class UnaryOperatorNode;
    class FunctionInvocationNode;
    class VariableReferenceNode;
    class AssignmentNode;
    class ReadNode;
    class IfNode;
    class WhileNode;
    class ForNode;
    class ReturnNode;
    class ExpressionNode;
    struct arrst{
        const char* type;
        std::vector<int>* dim;
    };
}

    /* For yylval */
%union {
    /* basic semantic value */
    char *identifier;
    struct arrst arrstruct;
    AstNode *node;
    CompoundStatementNode *compound_stmt_ptr;
    DeclNode* del_ptr;
    ConstantValueNode *const_node;
    FunctionNode *func_ptr;
    AstNode* ast_ptr;
    ExpressionNode* exp_ptr;
    FunctionInvocationNode* funcIn_ptr;
    VariableReferenceNode* varref;
    IfNode* ifnode;
    WhileNode* whilenode;
    ForNode* fornode;
    ReturnNode* renode;
    int int_val;       
    double real_val;   
    char *str_val;     
    bool bool_val;     

    char *type;
    std::vector<int> *dims;
    std::vector<DeclNode*>* declarations;
    std::vector<VariableNode*>* varsNode;
    std::vector<FunctionNode*>* funcNode;
    std::vector<AstNode*>* astNode;
    std::vector<ExpressionNode*>* Exprs;
    std::vector<VariableReferenceNode*>* varrefs;
};

%type <identifier> ProgramName ID FunctionName
%type <compound_stmt_ptr> CompoundStatement ElseOrNot
%type <del_ptr> Declaration FormalArg
//%type <func_ptr> FunctionList
%type <varsNode> IdList 
%type <declarations> DeclarationList Declarations FormalArgList FormalArgs
%type <type> ScalarType  
%type <const_node> LiteralConstant StringAndBoolean IntegerAndReal
%type <int_val> NegOrNot 
%type <dims> ArrDecl
%type <arrstruct> Type ArrType ReturnType
%type <funcNode> Functions FunctionList
%type <func_ptr> Function FunctionDeclaration FunctionDefinition 
%type <ast_ptr> Statement
%type <astNode> StatementList Statements
%type <exp_ptr> Expression
%type <Exprs> ExpressionList Expressions ArrRefList ArrRefs
%type <node> Simple
%type <funcIn_ptr> FunctionInvocation FunctionCall
%type <varref> VariableReference
%type <ifnode> Condition
%type <whilenode> While
%type <fornode> For
%type <renode> Return
    /* Follow the order in scanner.l */

    /* Delimiter */
%token COMMA SEMICOLON COLON
%token L_PARENTHESIS R_PARENTHESIS
%token L_BRACKET R_BRACKET

    /* Operator */
%token ASSIGN
    /* TODO: specify the precedence of the following operators */
  

%token OR AND NOT
%token LESS LESS_OR_EQUAL EQUAL GREATER GREATER_OR_EQUAL NOT_EQUAL


%left PLUS
%left DIVIDE MOD
%left MULTIPLY
%left MINUS

    /* Keyword */
%token ARRAY BOOLEAN INTEGER REAL STRING
%token END BEGIN
%token DO ELSE FOR IF THEN WHILE
%token DEF OF TO RETURN VAR
%token FALSE TRUE
%token PRINT READ

    /* Identifier */
%token ID

    /* Literal */
%token <int_val>INT_LITERAL
%token <real_val>REAL_LITERAL
%token <str_val>STRING_LITERAL
%left "+" "-"
%left "*" "/"
%%

ProgramUnit:
    Program
    |
    Function
;

Program:
    ProgramName SEMICOLON
    /* ProgramBody */
    DeclarationList FunctionList CompoundStatement
    /* End of ProgramBody */
    END {
        root = new ProgramNode(@1.first_line, @1.first_column,
                            $1, 
                            "void",
                            $3,
                            $4,
                            $5
                            );

        free($1);
    }
;

ProgramName:
    ID
;

DeclarationList:
    Epsilon
    { $$ = new std::vector<DeclNode*>(); }
    |
    Declarations {
        
        $$=$1;
    }
;

Declarations:
    Declaration {
        $$ = new std::vector<DeclNode*>();
        $$->push_back($1);
    }
    |
    Declarations Declaration {
        $$ = $1; 
        $$->push_back($2);
    }
;

FunctionList:
    Epsilon{
        $$ = new std::vector<FunctionNode*>();
    }
    |
    Functions{
        $$=$1;
    }
;

Functions:
    Function{
        $$ = new std::vector<FunctionNode*>();
        $$->push_back($1);
    }
    |
    Functions Function{
        $$=$1;
        $$->push_back($2);
    }
;

Function:
    FunctionDeclaration{
        $$=$1;
    }
    |
    FunctionDefinition{
        $$=$1;
    }
;

FunctionDeclaration:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType SEMICOLON{
        $$ = new FunctionNode(
            @1.first_line,
            @1.first_column,
            $1,
            $3,
            $5.type,
            nullptr
        );

    }
;

FunctionDefinition:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType
    CompoundStatement
    END{
        $$ = new FunctionNode(
            @1.first_line,
            @1.first_column,
            $1,
            $3,
            $5.type,
            $6
        );
    }
;

FunctionName:
    ID
;

FormalArgList:
    Epsilon{
        $$ = new std::vector<DeclNode*>(); 
    }
    |
    FormalArgs{
        $$ = $1;
    }
;

FormalArgs:
    FormalArg{
        $$ = new std::vector<DeclNode*>();
        $$->push_back($1);
    }
    |
    FormalArgs SEMICOLON FormalArg{
        $$ = $1;
        $$->push_back($3);
    }
;

FormalArg:
    IdList COLON Type{
        //old one is const
        std::vector<VariableNode*>* newvars = new std::vector<VariableNode*>();
        for(auto &eachvar:*$1){//derefer
            newvars->push_back(new VariableNode(
                //@1.first_line,
                //@1.first_column,
                eachvar->getLocation().line,
                eachvar->getLocation().col,
                eachvar->getNameCString(),
                $3.type,
                nullptr,
                $3.dim
                
            ));

        }
        $$ = new DeclNode(
            @1.first_line,
            @1.first_column,
            newvars,
            $3.type,
            $3.dim
        );
    }
;

IdList:
    ID {
        $$= new std::vector<VariableNode*>();
        $$->push_back(new VariableNode(
            @1.first_line, @1.first_column,
            $1,            
            nullptr, nullptr, nullptr
        ));
    }
    |
    IdList COMMA ID {
        $$=$1;
        $$->push_back(new VariableNode(@3.first_line,@3.first_column,
                $3,
                nullptr,
                nullptr,
                nullptr
            )
        );
    }
;

ReturnType:
    COLON ScalarType {
        $$.type=$2;
        
    }
    |
    Epsilon {
        $$.type = "void";
    }
;

    /*
       Data Types and Declarations
                                   */

Declaration:
    VAR IdList COLON ScalarType SEMICOLON {
        std::vector<VariableNode*>* p_vars = new std::vector<VariableNode*> ();
        //add type
        for(auto &i_var:*$2){
            p_vars->push_back( new VariableNode(i_var->getLocation().line,
            i_var->getLocation().col,
            i_var->getNameCString(),
            $4,
            nullptr,
            nullptr
            ));
        }
        $$ = new DeclNode(
            @1.first_line,
            @1.first_column,
            p_vars,
            $4,
            (std::vector<int>*)nullptr
        );
    }
    |
    VAR IdList COLON ArrType SEMICOLON{
        std::vector<VariableNode*>* p_vars = new std::vector<VariableNode*> ();
        //add type
        for(auto &i_var:*$2){
            p_vars->push_back( new VariableNode(i_var->getLocation().line,
            i_var->getLocation().col,
            i_var->getNameCString(),
            $4.type,
            nullptr,
            $4.dim
            ));
        }
        $$ = new DeclNode(
            @1.first_line,
            @1.first_column,
            p_vars,
            $4.type,
            $4.dim
        );
    }
    |
    VAR IdList COLON LiteralConstant SEMICOLON {
        std::vector<VariableNode*>* p_vars = new std::vector<VariableNode*> ();
        for(auto &i_var:*$2){
            p_vars->push_back( new VariableNode(i_var->getLocation().line, i_var->getLocation().col,
            i_var->getNameCString(),
            $4->getType(),
            $4,
            nullptr
            ));
        }
        //printf("%s", $4->getString());
    
        $$ = new DeclNode(
            @1.first_line,
            @1.first_column,
            p_vars,
            $4->getType(),
            (std::vector<int>*)nullptr
            
        );

    }
;

Type:
    ScalarType { 
        $$.type = $1; 
        $$.dim = new std::vector<int>();
    }
    |
    ArrType { $$ = $1; }
;

ScalarType:
    INTEGER { $$ = strdup("integer"); };
    |
    REAL { $$ = strdup("real"); }
    |
    STRING { $$ = strdup("string"); }
    |
    BOOLEAN { $$ = strdup("boolean"); }
;


ArrType:
    ArrDecl ScalarType{
        $$.type = $2;
        $$.dim = $1;
    }
;

ArrDecl:
    ARRAY INT_LITERAL OF { 
        
        $$ = new std::vector<int>();
        $$->push_back($2); 
    }
    |
    ArrDecl ARRAY INT_LITERAL OF {
        $$ = $1; 
        $$->push_back($3); 
    }
;

LiteralConstant:
    NegOrNot INT_LITERAL {
        
        if ($1) { 
            $$ = new ConstantValueNode(
                @1.first_line,
                @1.first_column,
                "integer",
                -$2 
            );
        } else { 
            $$ = new ConstantValueNode(
                @2.first_line,
                @2.first_column,
                "integer",
                $2
            );
        }
    }
    |
    NegOrNot REAL_LITERAL {
        if ($1) {
            $$ = new ConstantValueNode(
                @1.first_line,
                @1.first_column,
                "real",
                -$2
            );
        } else {
            $$ = new ConstantValueNode(
                @2.first_line,
                @2.first_column,
                "real",
                $2
            );
        }
    }
    |
    StringAndBoolean {
        $$ = $1;
    }
;

NegOrNot:
    Epsilon { $$ = 0; } 
    |
    MINUS { $$ = 1; }   
;

StringAndBoolean:
    STRING_LITERAL {
        //printf("Parsed string======: %s\n", $1);
        $$ = new ConstantValueNode(
            @1.first_line,
            @1.first_column,
            "string",               
            $1       
        );
    }
    |
    TRUE {
        $$ = new ConstantValueNode(
            @1.first_line,
            @1.first_column,
            "boolean",              
            true                    
        );
    }
    |
    FALSE {
        $$ = new ConstantValueNode(
            @1.first_line,
            @1.first_column,
            "boolean",              
            false                   
        );
    }
;

IntegerAndReal:
    INT_LITERAL {
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, "integer",$1);
        
    }
    |
    REAL_LITERAL {
        $$ = new ConstantValueNode(@1.first_line, @1.first_column, "real",$1);
        
    }
;

    /*
       Statements
                  */

Statement:
    CompoundStatement{$$ = $1;}
    |
    Simple{$$ = $1;}
    |
    Condition{$$ = $1;}
    |
    While{$$ = $1;}
    |
    For{$$ = $1;}
    |
    Return{$$ = $1;}
    |
    FunctionCall{$$ = $1;}
;

CompoundStatement:
    BEGIN
    DeclarationList
    StatementList
    END {
        $$ = new CompoundStatementNode(@1.first_line,
        @1.first_column,
        $2,
        $3
        );
        
    }
;

Simple:
    VariableReference ASSIGN Expression SEMICOLON{
        $$ = new AssignmentNode(
            @2.first_line,
            @2.first_column,
            $1,
            $3
        );
    }
    |
    PRINT Expression SEMICOLON{
        $$ = new PrintNode{
            @1.first_line,
            @1.first_column,
            $2
        };
    }
    |
    READ VariableReference SEMICOLON{
        $$ = new ReadNode{
            @1.first_line,
            @1.first_column,
            $2
        };
    }
;

VariableReference:
    ID ArrRefList{
        $$ = new VariableReferenceNode(
            @1.first_line,
            @1.first_column,
            $1,
            $2
        );
    }
;

ArrRefList:
    Epsilon{
        $$ = new std::vector<ExpressionNode*>();
    }
    |
    ArrRefs{
        $$=$1;
    }
;

ArrRefs:
    L_BRACKET Expression R_BRACKET{
        $$ = new std::vector<ExpressionNode*>();
        $$->push_back($2);
    }
    |
    ArrRefs L_BRACKET Expression R_BRACKET{
        $$ = $1;
        $$->push_back($3);
    }
;

Condition:
    IF Expression THEN
    CompoundStatement
    ElseOrNot
    END IF{
        $$ = new IfNode(
            @1.first_line,
            @1.first_column,
            $2,
            $4,
            $5
        );
    }
;

ElseOrNot:
    ELSE
    CompoundStatement{
        $$ = $2;
    }
    |
    Epsilon{
        $$ = nullptr;
    }
;

While:
    WHILE Expression DO
    CompoundStatement
    END DO{
        $$ = new WhileNode(
            @1.first_line,
            @1.first_column,
            $2,
            $4
        );
    }
;

For:
    FOR ID ASSIGN INT_LITERAL TO INT_LITERAL DO
    CompoundStatement
    END DO{
        //decl
        VariableNode* var = new VariableNode(
        @2.first_line,
        @2.first_column,
        $2,
        "integer",nullptr,nullptr
    );
    std::vector<VariableNode*> *p_vars = new std::vector<VariableNode *>();
    p_vars->push_back(var);
    DeclNode *Decl = new DeclNode(
        @2.first_line,
        @2.first_column,
        p_vars,
        "integer",
        (std::vector<int>*)nullptr
    );
    //assign
    AssignmentNode *assign = new AssignmentNode(
        @3.first_line,
        @3.first_column,
        new VariableReferenceNode(
            @2.first_line,
            @2.first_column,
            $2,
            nullptr
        ),
        new ConstantValueNode(
            @4.first_line,
            @4.first_column,
            "integer",
            $4
        )
    );

    //body
    ConstantValueNode *body = new ConstantValueNode(
        @6.first_line,
        @6.first_column,
        "integer",
        $6
    );
    $$ = new ForNode(
        @1.first_line,
        @1.first_column,
        Decl,
        assign,
        body,
        $8
    );
    }
;

Return:
    RETURN Expression SEMICOLON{
        $$ = new ReturnNode(
            @1.first_line,
            @1.first_column,
            $2
        );
    }
;

FunctionCall:
    FunctionInvocation SEMICOLON{
        $$=$1;
    }
;

FunctionInvocation:
    ID L_PARENTHESIS ExpressionList R_PARENTHESIS{
        $$ = new FunctionInvocationNode(
            @1.first_line,
            @1.first_column,
            $1,
            $3
        );
    }
;

ExpressionList:
    Epsilon{
        $$ = new std::vector<ExpressionNode*>();
    }
    |
    Expressions{
        $$ = $1;
    }
;

Expressions:
    Expression {
        $$ = new std::vector<ExpressionNode*>();
        $$->push_back($1);
    }
    |
    Expressions COMMA Expression{
        $$=$1;
        $$->push_back($3);
    }
;

StatementList:
    Epsilon{$$ = new std::vector<AstNode*>();}
    |
    Statements{$$ = $1;}
;

Statements:
    Statement{
        $$ = new std::vector<AstNode*>();
        $$->push_back($1);
    }
    |
    Statements Statement{
        $$ = $1;
        $$->push_back($2);
    }
;

Expression:
    L_PARENTHESIS Expression R_PARENTHESIS{$$=$2;}
    |
    MINUS Expression{
        $$ = new UnaryOperatorNode(
            @1.first_line,
            @1.first_column,
            "neg",
            $2
        );
    }
    |
    Expression MULTIPLY Expression{
        $$ = new BinaryOperatorNode(
            @2.first_line,
            @2.first_column,
            "*",
            $1,
            $3
        );
    }
    |
    Expression DIVIDE Expression{
        $$ = new BinaryOperatorNode(
            @2.first_line,
            @2.first_column,
            "/",
            $1,
            $3
        );
    }
    |
    Expression MOD Expression{
        $$ = new BinaryOperatorNode(
            @2.first_line,
            @2.first_column,
            "mod",
            $1,
            $3
        );
    }
    |
    Expression PLUS Expression{
        $$ = new BinaryOperatorNode(
            @2.first_line,
            @2.first_column,
            "+",
            $1,
            $3
        );
    }
    |
    Expression MINUS Expression{
        $$ = new BinaryOperatorNode(
            @2.first_line,
            @2.first_column,
            "-",
            $1,
            $3
        );
    }
    |
    Expression LESS Expression{
        $$ = new BinaryOperatorNode(
            @2.first_line,
            @2.first_column,
            "<",
            $1,
            $3
        );
    }
    |
    Expression LESS_OR_EQUAL Expression{
        $$ = new BinaryOperatorNode(
            @2.first_line,
            @2.first_column,
            "<=",
            $1,
            $3
        );
    }
    |
    Expression GREATER Expression{
        $$ = new BinaryOperatorNode(
            @2.first_line,
            @2.first_column,
            ">",
            $1,
            $3
        );
    }
    |
    Expression GREATER_OR_EQUAL Expression{
        $$ = new BinaryOperatorNode(
            @2.first_line,
            @2.first_column,
            ">=",
            $1,
            $3
        );
    }
    |
    Expression EQUAL Expression{
        $$ = new BinaryOperatorNode(
            @2.first_line,
            @2.first_column,
            "=",
            $1,
            $3
        );
    }
    |
    Expression NOT_EQUAL Expression{
        $$ = new BinaryOperatorNode(
            @2.first_line,
            @2.first_column,
            "<>",
            $1,
            $3
        );
    }
    |
    NOT Expression{
        $$ = new UnaryOperatorNode(
            @1.first_line,
            @1.first_column,
            "not",
            $2
        );
    }
    |
    Expression AND Expression{
        $$ = new BinaryOperatorNode(
            @2.first_line,
            @2.first_column,
            "and",
            $1,
            $3
        );
    }
    |
    Expression OR Expression{
        $$ = new BinaryOperatorNode(
            @2.first_line,
            @2.first_column,
            "or",
            $1,
            $3
        );
    }
    |
    IntegerAndReal{
        $$=$1;
    }
    |
    StringAndBoolean{
        $$=$1;
    }
    |
    VariableReference{$$ = $1;}
    |
    FunctionInvocation{$$ = $1;}
;

    /*
       misc
            */
Epsilon:
;

%%

void yyerror(const char *msg) {
    fprintf(stderr,
            "\n"
            "|-----------------------------------------------------------------"
            "---------\n"
            "| Error found in Line #%d: %s\n"
            "|\n"
            "| Unmatched token: %s\n"
            "|-----------------------------------------------------------------"
            "---------\n",
            line_num, current_line, yytext);
    exit(-1);
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename> [--dump-ast]\n", argv[0]);
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("fopen() failed");
        exit(-1);
    }

    yyparse();

    if (argc >= 3 && strcmp(argv[2], "--dump-ast") == 0) {
        AstDumper ast_dumper;
        root->accept(ast_dumper);
    }

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");

    delete root;
    fclose(yyin);
    yylex_destroy();
    return 0;
}
