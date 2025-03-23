#ifndef SEMA_SEMANTIC_ANALYZER_H
#define SEMA_SEMANTIC_ANALYZER_H

#include "sema/ErrorPrinter.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <stack>
#include <string>
#include <vector>
class SymbolEntry {
private:
    std::string name;// Variable names
    std::string kind;// Kind
    int level;// Level
    std::string type;
    std::string attribute;// ...
    bool arrErr;
public:
    SymbolEntry(std::string name, std::string kind, int level, std::string type, std::string attribute, bool arrErr)
      :name(name), kind(kind), level(level), type(type), attribute(attribute), arrErr(arrErr){}
    const char* getName(){return name.c_str();}
    std::string getKind(){return kind;}
    const char* getType(){return type.c_str();}
    const char* getAttribute(){return attribute.c_str();}
    bool getErr(){return arrErr;}
    int getLevel(){return level;}
    void Nameis(std::string newname){name=newname;}
    void kindis(std::string newkind){kind=newkind;}
    void Attris(std::string newattri){attribute=newattri;}
    void Typeis(std::string newtype){type=newtype;}
    void erris(bool bit){arrErr=bit;}
    int getArrDim(){
      int count=0;
      for(auto thing:type){
        if(thing=='['){
          count+=1;
        }
      }
      return count;
    }
};

class SymbolTable {
public:
    ///////////////////////////////////////////////
    void dumpDemarcation(const char chr) {
      for (size_t i = 0; i < 110; ++i) {printf("%c", chr);}puts("");}
    void dumpSymbol() {
      dumpDemarcation('=');
      printf("%-33s%-11s%-11s%-17s%-11s\n", "Name", "Kind", "Level", "Type","Attribute");
      dumpDemarcation('-');
      for(auto thing:entries) {
        printf("%-33s%-11s%d%-10s%-17s%-11s\n", thing.getName(), thing.getKind().c_str(), thing.getLevel(),thing.getLevel()?"(local)":"(global)",
                                                  thing.getType(), thing.getAttribute());}
        //puts("");
      dumpDemarcation('-');
    }
    ////////////////////////////////////////////
    SymbolTable() { entries = std::vector<SymbolEntry>(); }

    void addSymbol(SymbolEntry entry){
      entries.push_back(entry);
    };

    int getSize(){return entries.size();}
    // other methods
    bool hasSym(std::string name){
      for(auto thing:entries){
        if(name==thing.getName()){
          return 1;
        }
      }
      return 0;
    }
    
    SymbolEntry* getExact(std::string name){
      for(auto &thing:entries){
        if(thing.getName()==name){
          return &thing;
        }
      }
      return nullptr;
    }
    int getLevel(){return level;}
    void setLevel(int p_level){level=p_level;}
    SymbolEntry* getEntry(){
      return &entries.back(); 
    }
    
private:
    // keep the order by simply using vector
    std::vector<SymbolEntry> entries;
    int level;
};

class SymbolManager {
public:
    SymbolManager() { tables = std::vector<SymbolTable *>();}
    void pushScope(SymbolTable *new_scope){tables.push_back(new_scope);};
    void popScope(){
      tables.pop_back();
    };
    SymbolTable* getTable(){
      return tables.back();
    }
    bool loopvarcheck(std::string name){
      for(auto thing:tables){
        if(thing->hasSym(name)){
          if(thing->getExact(name)->getKind()=="loop_var"){
            return 1;
          }
          
        }
      }
      return 0;
    }

    SymbolEntry* getExact(std::string name) {
      for (auto it = tables.rbegin(); it != tables.rend(); ++it) {
          if ((*it)->hasSym(name)) {
              return (*it)->getExact(name);
          }
      }
      return nullptr; // Not found
    }
private:
    std::vector<SymbolTable *> tables;
};
class SemanticAnalyzer final : public AstNodeVisitor {
  private:
    ErrorPrinter m_error_printer{stderr};
    SymbolManager manager;
    // TODO: something like symbol manager (manage symbol tables)
    //       context manager, return type manager
    
  public:
    ~SemanticAnalyzer() = default;
    SemanticAnalyzer() = default;
    SemanticAnalyzer(int m_dump):dump(m_dump){}
    void visit(ProgramNode &p_program) override;
    void visit(DeclNode &p_decl) override;
    void visit(VariableNode &p_variable) override;
    void visit(ConstantValueNode &p_constant_value) override;
    void visit(FunctionNode &p_function) override;
    void visit(CompoundStatementNode &p_compound_statement) override;
    void visit(PrintNode &p_print) override;
    void visit(BinaryOperatorNode &p_bin_op) override;
    void visit(UnaryOperatorNode &p_un_op) override;
    void visit(FunctionInvocationNode &p_func_invocation) override;
    void visit(VariableReferenceNode &p_variable_ref) override;
    void visit(AssignmentNode &p_assignment) override;
    void visit(ReadNode &p_read) override;
    void visit(IfNode &p_if) override;
    void visit(WhileNode &p_while) override;
    void visit(ForNode &p_for) override;
    void visit(ReturnNode &p_return) override;
    int fromFunc=0;
    bool isError=false;
    bool setconst=false;
    std::string FuncNameforRet="";
    bool dump=true;
    
};

#endif
