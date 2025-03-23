# hw4 report

|||
|-:|:-|
|Name|曲楷鈞|
|ID|111550122|

## How much time did you spend on this project

3.5 days of workload.

## Project Overview
這次的assignment主要是做個sematic analyzer，同樣利用之前visitor 去遍歷Abstract Syntax Tree，驗證個個節點的語意正確性，像是decalration, function call，然後還有傳遞不同資訊，儲存設定之後可能需要的狀態，有些部分有點像在做一個fsm，利用一些flag去決定現在的狀態跟行為，然後根據得到的錯誤，去輸出相關的error，例如redeclaration, incompatible types, and invalid operations.。



幾個重點:

- **Symbol Table**: 在manager之下正確利用P language語言的scope rule去push or pop Symbol table，然後這個table還要包括一些取得資訊的方法，比如說幫助我們遍歷一個scope內的redeclaration的function。

- **Semantic Validation**:
  - Redeclaration of variables or functions.
  - Invalid array indexing or usage of non-integer indices.
  - Assignment to constants, loop variables, or incompatible types.
  - Function call validation for argument types and count.
  - Correct usage of operators (binary and unary) with compatible operand types.

- **Error Reporting**: 用提供的error printer class去呼叫各種錯誤要的printer
```c
SymbolRedeclarationError error(p_function.getLocation(), p_function.getNameCString());//需要輸出的資訊
        ErrorPrinter printer(stderr); // Print
        printer.print(error);
        isError=true;//給semantic analyzer存是否有發現錯誤
```

- **Node Traversal and Type Checking**: 遍歷 Abstract Syntax Tree (AST) 去驗證各個節點及其子節點(在不同的Node可能會有不同要求，像是遇到錯誤還是會去進行後面的語意分析)，像是 `If`, `For`, `While`, `Print`和 `Read`.

express這個Token是很多class的base class，所以在處理type上真的頻頻卡關，對於物件導向還有reference dereference要更加了解跟注意，大概就是這樣，每次做一個analyzer的步驟大概就是
- 先開xxx.hpp和parser確定其子節點跟class醫訊
- 讀readme相關的規則
- 對sample resolution看需要輸出的錯誤資訊
- 去藉著子節點或者analyzer內的flag去獲取需要驗證的資訊，或做字串處理
- 解決Segmentation fault

## What is the hardest you think in this project

同上oop，看得懂助教提供的code，class之間的繼承，reference pointer之類的
還有以下這個=>
> [!important]
> DO NOT use string for everything, unless you're obsessed with parsing strings for extracting information from them.
還是做了不少parsing string ==

## Feedback to T.A.s

> Not required, but bonus point may be given.
