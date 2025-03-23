# hw3 report

|      |                 |
| ---: | :-------------- |
| Name | 曲楷鈞        |
|   ID | 111550122 |

## How much time did you spend on this project

30 or 40 hours plus 

## Project Overview

> After being confused for so many hours, I guess I now have the full glimpse of what I'm doing?

This project involves building an **Abstract Syntax Tree** and using the **visitor pattern** .

---
## Report

### **Goal**
The main goal is to construct an AST and utilize the visitor pattern to extract and display the required data. 

parser.y:

- 先在code require加入需要的class類別
- 並且宣告在%union裡面
- 在%type specify會用到的parsing token

接著我們會需要實作在parsing後所需要做出的action,要做到這點就要先去實作出各節點，中間的節點就會需要有相應的資訊=>要傳遞或者使用。
舉例從declaration開始做，我會到decl.hpp, decl.cpp完成需要的member attribute跟member function, 
- 當定義完constructor, getfunction..
- 我還需要去 *overide* 在ast.hpp裡的virtual func(如下)
- 利用visitor function完成我們要輸出各節點資訊的目標
```cpp
virtual void accept(AstNodeVisitor &p_visitor) = 0;
virtual void visitChildNodes(AstNodeVisitor &p_visitor){};
```
接著先回到parser.y
把action加入parsing，從最底部做到能接上program，也就是中間會用到的所有Node都會需要輪流去.hpp定義，follow助教提供的AstGuideLine.hd基本上沒什麼問題。

> [!note]
> 這邊一開始做會需要去scanner.i去更新一些yyval傳資料的方法，確保int real sci string這些都能成功傳給parser。


這個基本上就會是做出所有node跟parsing所有的流程，當然還有不少要注意的東西，但最困難的地方通常還是想節點需要甚麼資料型態，c++ class繼承後中間casting資料是怎麼傳之類的問題。

## visitor pattern
直接從整個執行flow來講，一開始在parser.y裡面會從root->accept開始呼叫
```cpp
//in parser.y
AstDumper ast_dumper;
root->accept(ast_dumper);
```
accept()是在ast.hpp裡面宣告的虛擬函式，對應root呼叫的accept=>
```cpp
//in program.hpp
void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
```
也就是等於我們現在是以 program這個class 去呼叫visit
visit是visitor/AstNodeVisitor宣告的virtual，在AstDumper overide，接著對各種節點的visit()傳入值在AstDumper.cpp定義
print到目前節點需要的資訊 => 拜訪子節點 visitChildNode() => 在program.cpp裡面會看到
```cpp
//program.cpp
void ProgramNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    program_child_Node ->accept(p_visitor);
}
```
到這邊一個run就完成，而child_node->accept就會完成下一個流程，藉著這樣，我們創造了另一個virtual function，統一解決了各種節點需要print出的東西，而不是分散在各個Node底下去print,讓整個project架構更完整，也容易擴充。
## What is the hardest you think in this project

- Understand what I'm doing and where to start
- What datatype to give to each Node
    像是一開始為了debug方便沒做array declaration
    結果到時候要加上dimension時就發現有些前面的型別要打掉
- Why child class assign to base class, the attribute is still there (this is pretty confusing at first)
- some pointer and reference and dereference stuff

## Feedback to T.A.s


