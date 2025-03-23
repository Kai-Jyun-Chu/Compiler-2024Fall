# hw5 report

|||
|-:|:-|
|Name|曲楷鈞|
|ID|111550122|

## How much time did you spend on this project
```plain
   _________              ____________
  /_________ \          /____________/\
 |________  \ \        /  _________   \ \
          \  \ \      /  ///        \  \ \
          |  | |      |  |/|         |  | |
    ______|  | |      |  |/|         |  | |
   /_________///      |  |/|         |  | |
  |_______   \|\      |  |/|         |  | |
          |  | |      \  \/\         /  / /
  ________|  | |       \  \/\       /  / /
 /________/  / /        \  \_\_____/  / /
 |__________/_/          \ __________/_/               h
```



## Project overview

簡單來說要做的事情就是藉由visit AST, 在走到特定的Node時**生成 RISC-V assembly code**.

因為上次的作業主要是做 **semantic analyzer**, 所以我們已經有:
- **symbol table** with crucial information about variables, constants, and functions.
- The required **semantic information** for code generation.


### **1. Constant/Assignment**
    這是最一開始很confused我的地方，我不知道一整串的riscv要怎麼去拆分在逛到不同的AST node去分配產生哪段assembly, 知道怎麼拆
    去分配code generation後，接下來是分開處理address跟value, 以及在symbol table裡面entry我們需要用到相對stack的位置的資
    訊，需要分配給定的數值，讓後面reference能去正確輸出。

    列點說的話:
   - 判斷要處理 **address** 或 **value assignment**.
   - 利用 **symbol table entries** 去決定 **variable 在 stack上相對的位置--> offset** .
   - 確認value有被assign, 以及後面global local的reference能正確處理.

   **Example RISC-V Code Snippet :**
   ```assembly
   li t0, <constant_value>
   addi sp, sp, -4
   sw t0, 0(sp)
   ```
### **2. Function**
    這地方也花了點時間，主要是處理參數，以及修改在constant/assignment部分inconsistent的code，還有return的問題==
    ，最後是在advanced argument處理過多參數要存在指定的register.

### **3. Loop**
    基本上好像就是照著tutorial的寫法，為了正確地把各個label輸出，在code gen加上一個全域變數去紀錄現在label記到哪裡
    ，以及codition判斷後的branch。

### **4. Others**
    其餘以及沒提到的細節我講一個大致的流程
  - 看Readme上的risv
  - 依照visit的Node去輸出
  - 出bug
  - 主要看assembler_output和實際dump出的riscv
  - 看testcase的
  - Debug
  
>大概是這樣
另外，還有**scope**與**symbol table**之間的關係，否則有時候在不同function或block裡面存取到錯誤的變數，就會造成莫名其妙的錯誤。

## What is the hardest you think in this project

剛開始都是最困惑的，像是load address, load word, load immediate, 位址怎麼決定等等的，
第一個測資應該也是花最久的

## Feedback to T.A.s


