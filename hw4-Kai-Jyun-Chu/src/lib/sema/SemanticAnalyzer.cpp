#include "sema/Error.hpp"
#include "sema/SemanticAnalyzer.hpp"
#include "visitor/AstNodeInclude.hpp"
#include <sstream>

void SemanticAnalyzer::visit(ProgramNode &p_program) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    SymbolEntry programentry((p_program.getNameCString()),"program", 0, "void", "", false);
    
    SymbolTable* sym = new SymbolTable;
    sym->addSymbol(programentry);
    sym->setLevel(programentry.getLevel());
    manager.pushScope(sym);
    p_program.visitChildNodes(*this);
    if(dump){//add last
        sym->dumpSymbol();
    }
    
    manager.popScope();
}

void SemanticAnalyzer::visit(DeclNode &p_decl) {
    p_decl.visitChildNodes(*this);
}

void SemanticAnalyzer::visit(VariableNode &p_variable) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    auto table = manager.getTable();
    if (table->hasSym(p_variable.getNameCString())) {
        
        SymbolRedeclarationError error(
            p_variable.getLocation(), 
            p_variable.getNameCString() 
        );

        ErrorPrinter printer(stderr); // Print r
        printer.print(error);
        isError=true;
        return;
    }else if(manager.loopvarcheck(p_variable.getNameCString())){
        SymbolRedeclarationError error(
            p_variable.getLocation(), 
            p_variable.getNameCString() 
        );

        ErrorPrinter printer(stderr); // Print r
        printer.print(error);
        isError=true;
        return;
    } 
     
    table->addSymbol(SymbolEntry(p_variable.getNameCString(),(fromFunc>0)?"parameter":"variable",table->getLevel(),p_variable.getTypeCString(), 
                        "", false));
    if(!p_variable.Valid()){
        NonPositiveArrayDimensionError error(
            p_variable.getLocation(),
            p_variable.getNameCString()
        );
        ErrorPrinter printer(stderr); // Print r
        printer.print(error);
        isError=true;
        table->getEntry()->erris(true);
        return;
    }                   
    
    setconst=true;//add this while variableRef
    
    
    p_variable.visitChildNodes(*this);
    setconst=false;
}

void SemanticAnalyzer::visit(ConstantValueNode &p_constant_value) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    
    auto table = manager.getTable();
    auto entry = table->getEntry();
    
    if(setconst&&!entry->getErr()){
        entry->Attris(p_constant_value.getConstantValueCString());

        entry->kindis("constant");
        
    }
    
    
    p_constant_value.setType(p_constant_value.getTypeSharedPtr());
    
}

void SemanticAnalyzer::visit(FunctionNode &p_function) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    auto table = manager.getTable();
    if(table->hasSym(p_function.getNameCString())){
        SymbolRedeclarationError error(
            p_function.getLocation(), 
            p_function.getNameCString() 
        );
 
        ErrorPrinter printer(stderr); // Print
        printer.print(error);
        isError=true;
    }else{
        table->addSymbol(SymbolEntry(p_function.getNameCString(),"function",table->getLevel(),p_function.getRet(), 
                        p_function.getParametersCString(), false));
    }
    SymbolTable* functable = new SymbolTable();
    functable->setLevel(table->getLevel()+1);
    manager.pushScope(functable);
    fromFunc+=1;
    FuncNameforRet=p_function.getNameCString();
    p_function.visitChildNodes(*this);
    if(dump){
        functable->dumpSymbol();
    }
    
    manager.popScope();
    FuncNameforRet="";
    
    
}

void SemanticAnalyzer::visit(CompoundStatementNode &p_compound_statement) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol  current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    if(fromFunc){
        fromFunc-=1;
        //auto table=manager.getTable();
        p_compound_statement.visitChildNodes(*this);
    }else{
        SymbolTable* table = new SymbolTable();
        table->setLevel(manager.getTable()->getLevel()+1);
        manager.pushScope(table);
        p_compound_statement.visitChildNodes(*this);
        if(dump){
            table->dumpSymbol();
        }
        
        manager.popScope();
    }
    
}

void SemanticAnalyzer::visit(PrintNode &p_print) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_print.visitChildNodes(*this);
    auto &exprNode = p_print.getTarget();
    if(exprNode.getType()){
        std::string checktype = exprNode.getType()->getPTypeCString();
        if(checktype != "integer" && checktype != "real" && checktype != "boolean" && checktype != "string"){
            PrintOutNonScalarTypeError error(
                exprNode.getLocation()
            );
            ErrorPrinter printer(stderr); // Print
            printer.print(error);
            isError=true;
            
        }

    }
    

}

void SemanticAnalyzer::visit(BinaryOperatorNode &p_bin_op) {
    
    p_bin_op.visitChildNodes(*this);
    std::string op = p_bin_op.getOpCString();
    if(p_bin_op.getLeftOperand()->getType()==nullptr||p_bin_op.getRightOperand()->getType()==nullptr){
        return;
    }
    std::string lefttype = p_bin_op.getLeftOperand()->getType()->getPTypeCString();
    std::string righttype = p_bin_op.getRightOperand()->getType()->getPTypeCString();
    //printf(lefttype.c_str());
    //printf("\n");
    //string concat
    if(op=="+"){
        if(lefttype=="string"&&righttype=="string"){
            p_bin_op.setType(PTypeSharedPtr(new PType(PType::PrimitiveTypeEnum::kStringType)));
            return;
        }
    }
    if(op=="+"||op=="-"||op=="*"||op=="/"){
        //printf("%s", lefttype.c_str());
        //printf("\n");
        if((lefttype!="integer"&&lefttype!="real")||(righttype!="integer"&&righttype!="real")){
            
            InvalidBinaryOperandError error(
                p_bin_op.getLocation(),
                p_bin_op.getOp(),
                p_bin_op.getLeftOperand()->getType().get(),
                p_bin_op.getRightOperand()->getType().get()
            );
            ErrorPrinter printer(stderr); // Print
            printer.print(error);
            isError=true;
            return;
        }else {
            if(lefttype == "integer" && righttype == "integer"){
                p_bin_op.setType(PTypeSharedPtr(new PType(PType::PrimitiveTypeEnum::kIntegerType)));
            } else {
                p_bin_op.setType(PTypeSharedPtr(new PType(PType::PrimitiveTypeEnum::kRealType)));
            }
        }
    }else if(op=="mod"){
        if(!((lefttype=="integer")&&(righttype=="integer"))){
            InvalidBinaryOperandError error(
                p_bin_op.getLocation(),
                p_bin_op.getOp(),
                p_bin_op.getLeftOperand()->getType().get(),
                p_bin_op.getRightOperand()->getType().get()
            );
            ErrorPrinter printer(stderr); // Print
            printer.print(error);
            isError=true;
            return;
        }else{
            p_bin_op.setType(PTypeSharedPtr(new PType(PType::PrimitiveTypeEnum::kIntegerType)));
        }
    }else if(op=="and"||op=="or"){
        //printf("%s", lefttype.c_str());
        //printf("\n");
        if((lefttype!="boolean")||(righttype!="boolean")){
         InvalidBinaryOperandError error(
                p_bin_op.getLocation(),
                p_bin_op.getOp(),
                p_bin_op.getLeftOperand()->getType().get(),
                p_bin_op.getRightOperand()->getType().get()
            );
            ErrorPrinter printer(stderr); // Print
            printer.print(error);
            isError=true;
            return;
        }else{
            p_bin_op.setType(PTypeSharedPtr(new PType(PType::PrimitiveTypeEnum::kBoolType)));
        }
    }else if(op == "<" || op == "<=" || op == ">" || op == ">=" || op == "=" || op == "<>"){
        if(!((lefttype == "real" || lefttype == "integer") && (righttype == "real" || righttype == "integer"))){
            InvalidBinaryOperandError error(
                p_bin_op.getLocation(),
                p_bin_op.getOp(),
                p_bin_op.getLeftOperand()->getType().get(),
                p_bin_op.getRightOperand()->getType().get()
            );
            ErrorPrinter printer(stderr); // Print
            printer.print(error);
            isError=true;
            return;
        }else{
            p_bin_op.setType(PTypeSharedPtr(new PType(PType::PrimitiveTypeEnum::kBoolType)));
        }
    }
        
    
    
}

void SemanticAnalyzer::visit(UnaryOperatorNode &p_un_op) {

    p_un_op.visitChildNodes(*this);
    //std::string operand = p_un_op.getoperand();
    //check
    PTypeSharedPtr check = p_un_op.checktype();
    std::string op = p_un_op.getOpCString();
    if(check==nullptr){
        return;
    }else{
        std::string operand = check->getPTypeCString();
        if(op=="neg"){
            if((operand!="integer"&&operand!="real")){
                InvalidUnaryOperandError error(
                    p_un_op.getLocation(),
                    p_un_op.getOpspecial(),
                    check.get()
                );
                ErrorPrinter printer(stderr); // Print
                printer.print(error);
                isError=true;
                return;
            }else{
                p_un_op.setType(p_un_op.checktype());
            }
        }else if(op=="not"){
            if(operand!="boolean"){
                InvalidUnaryOperandError error(
                    p_un_op.getLocation(),
                    p_un_op.getOpspecial(),
                    check.get()
                );
                ErrorPrinter printer(stderr); // Print
                printer.print(error);
                isError=true;
                return;
            }else{
                p_un_op.setType(p_un_op.checktype());
            }
        }
        

    }

}

void SemanticAnalyzer::visit(FunctionInvocationNode &p_func_invocation) {
    //get
    
    auto func = manager.getExact(p_func_invocation.getNameCString());
    p_func_invocation.visitChildNodes(*this);
    
    if (!func) {
        // seg
        UndeclaredSymbolError error(p_func_invocation.getLocation(), p_func_invocation.getNameCString());
        ErrorPrinter printer(stderr);
        printer.print(error);
        isError = true;
        return;
    }
    //check is func
    if (func->getKind() != "function") {
        NonFunctionSymbolError error(p_func_invocation.getLocation(), p_func_invocation.getNameCString());
        ErrorPrinter printer(stderr);
        printer.print(error);
        isError = true;
        return;
    }
    


    std::string attributeStr = func->getAttribute();
    int InvocNum = p_func_invocation.getArgsNum();
    int aqruireNum=0;
    //std::vector<std::string> InvocAttr = p_func_invocation.getArgString();
    std::vector<std::string> AquireAttr ;
    if(attributeStr==""){
        aqruireNum=0;
    }else{
        aqruireNum+=1;
        for(auto thing:attributeStr){
            if(thing==','){aqruireNum+=1;}}
    }
    std::string token;
    std::istringstream stream(attributeStr);

    while (std::getline(stream, token, ',')) {
        token.erase(0, token.find_first_not_of(" \t")); // Remove leading spaces
        token.erase(token.find_last_not_of(" \t") + 1); // Remove trailing spaces
        AquireAttr.push_back(token);
    }
    ///////////////////////////////////////////////
    if(aqruireNum!=InvocNum){
        ArgumentNumberMismatchError error(
            p_func_invocation.getLocation(),
            p_func_invocation.getNameCString()
        );
        ErrorPrinter printer(stderr); // Print
        printer.print(error);
        isError=true;
        return;
    }else {

        for (size_t i = 0; i < AquireAttr.size(); i++) {
            auto &exprNode = p_func_invocation.getExpressNode()[i];
            if(exprNode&&exprNode->getType()){
                if (exprNode->getType()->getPTypeCString() != AquireAttr[i]&&!(AquireAttr[i]=="real"&&std::string(exprNode->getType()->getPTypeCString())=="integer")) {
                IncompatibleArgumentTypeError error(
                    exprNode->getLocation(),
                    AquireAttr[i].c_str(),
                    exprNode->getType().get()
                );
                ErrorPrinter printer(stderr); // Print
                printer.print(error);
                isError = true;
                return;
            }
            }
            
        }
        
    }


    std::string typeString = func->getType();
    if (typeString == "integer") {
        p_func_invocation.setType(PTypeSharedPtr(new PType(PType::PrimitiveTypeEnum::kIntegerType)));
    } else if (typeString == "real") {
        p_func_invocation.setType(PTypeSharedPtr(new PType(PType::PrimitiveTypeEnum::kRealType)));
    } else if (typeString == "boolean") {
        p_func_invocation.setType(PTypeSharedPtr(new PType(PType::PrimitiveTypeEnum::kBoolType)));
    } else if (typeString == "string") {
        p_func_invocation.setType(PTypeSharedPtr(new PType(PType::PrimitiveTypeEnum::kStringType)));
    } else {
        // check
        p_func_invocation.setType(PTypeSharedPtr(new PType(PType::PrimitiveTypeEnum::kVoidType)));
    }



}

void SemanticAnalyzer::visit(VariableReferenceNode &p_variable_ref) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    auto var = manager.getExact(p_variable_ref.getNameCString());
    if(var==nullptr){
        UndeclaredSymbolError error(
            p_variable_ref.getLocation(),
            p_variable_ref.getNameCString()
        );
        ErrorPrinter printer(stderr); // Print
        printer.print(error);
        isError=true;
        return;
    }else if(var->getKind()=="program"||var->getKind()=="function"){
        NonVariableSymbolError error(
            p_variable_ref.getLocation(),
            p_variable_ref.getNameCString()
        );
        ErrorPrinter printer(stderr); // Print
        printer.print(error);
        isError=true;
    }else{
        if(!var->getErr()){
            
            p_variable_ref.visitChildNodes(*this);
            for(auto &thing:p_variable_ref.getIndice()){
                if(thing->getType()==nullptr){
                    return;
                }
                std::string str = thing->getType()->getPTypeCString();
                if(str!="integer"){
                    NonIntegerArrayIndexError error(
                        thing->getLocation()
                    );
                    ErrorPrinter printer(stderr); // Print
                    printer.print(error);
                    isError=true;
                    
                    return;
                }
            }

            if(p_variable_ref.getDim()>var->getArrDim()){
                OverArraySubscriptError error(
                    p_variable_ref.getLocation(),
                    p_variable_ref.getNameCString()
                );
                ErrorPrinter printer(stderr); // Print
                printer.print(error);
                isError=true;
            }else{
                if(var->getType()==nullptr){
                    return;
                }
                std::string typeString = var->getType();
                
                if (typeString[0] == 'i') {  // integer
                    p_variable_ref.setType(PTypeSharedPtr(new PType(PType::PrimitiveTypeEnum::kIntegerType)));
                } else if (typeString[0] == 'r') {  // real
                    p_variable_ref.setType(PTypeSharedPtr(new PType(PType::PrimitiveTypeEnum::kRealType)));
                } else if (typeString[0] == 'b') {  // boolean
                    p_variable_ref.setType(PTypeSharedPtr(new PType(PType::PrimitiveTypeEnum::kBoolType)));
                } else if (typeString[0] == 's') {  // string
                    p_variable_ref.setType(PTypeSharedPtr(new PType(PType::PrimitiveTypeEnum::kStringType)));
                }

                //if (!p_variable_ref.getIndice().empty()) {
                    size_t refDims = p_variable_ref.getDim();   
                         
                    
                    


                    std::vector<uint64_t> dimensions;
                    size_t start = 0;
                    while ((start = typeString.find('[', start)) != std::string::npos) {
                        size_t end = typeString.find(']', start);
                        
                        // Extract the number inside '[]' and convert it to an integer
                        uint64_t dim = std::stoul(typeString.substr(start + 1, end - start - 1));
                        dimensions.push_back(dim);
                        // Move to the next position
                        start = end + 1;
                    }

                    if (refDims < dimensions.size()) {
                        std::vector<uint64_t> adjustedDims(dimensions.begin() + refDims, dimensions.end());
                        p_variable_ref.getType()->setDimensions(adjustedDims);
                    }
                //}
            }
        }
    }   
}

void SemanticAnalyzer::visit(AssignmentNode &p_assignment) {
    p_assignment.visitChildNodes(*this);
    auto &ref = p_assignment.getRef();
    if(ref.getType()){
        auto thing = ref.getType()->getmdimension();
        if(thing.size()>0){
            AssignToArrayTypeError error(
                ref.getLocation()
            );
            ErrorPrinter printer(stderr); // Print
            printer.print(error);
            isError=true;
            return;
        }

        auto symbol = manager.getExact(ref.getNameCString());
        if(symbol->getKind()=="constant"){
            AssignToConstantError error(
                ref.getLocation(),
                ref.getNameCString()
            );
            ErrorPrinter printer(stderr); // Print
            printer.print(error);
            isError=true;
            return;
        }else if(symbol->getKind()=="loop_var"){
            AssignToLoopVarError error(
                ref.getLocation()
            );
            ErrorPrinter printer(stderr); // Print
            printer.print(error);
            isError=true;
            return;
        }
        if(p_assignment.getExpress().getType()){
            if(p_assignment.getExpress().getType()->getmdimension().size()>0){
                AssignToArrayTypeError error(
                    p_assignment.getExpress().getLocation()
                );
                ErrorPrinter printer(stderr); // Print
                printer.print(error);
                isError=true;
                return;
            }
            std::string reftype = ref.getType()->getPTypeCString();
            std::string assigntype = p_assignment.getExpress().getType()->getPTypeCString();
            if(reftype!=assigntype&&!(reftype=="real"&&assigntype=="integer")){
                IncompatibleAssignmentError error(
                    p_assignment.getLocation(),
                    ref.getType().get(),
                    p_assignment.getExpress().getType().get()
                );
                ErrorPrinter printer(stderr); // Print
                printer.print(error);
                isError=true;
                return;
            }
        }
    }
}

void SemanticAnalyzer::visit(ReadNode &p_read) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_read.visitChildNodes(*this);
    auto &ref = p_read.getref();
    PTypeSharedPtr ptypeptr = ref.getType();
    if(ptypeptr==nullptr){
        return;
    }
    std::string RefType = ptypeptr->getPTypeCString();
    if(RefType != "integer" && RefType != "real" && RefType != "boolean" && RefType != "string"){
        ReadToNonScalarTypeError error(
            ref.getLocation()
        );
        ErrorPrinter printer(stderr); // Print
        printer.print(error);
        isError=true;
    }else{
        //這之前就處理過undeclared
        auto symbol = manager.getExact(ref.getNameCString());
        if(symbol->getKind()=="loop_var"||symbol->getKind()=="constant"){
            ReadToConstantOrLoopVarError error(
                ref.getLocation()
            );
            ErrorPrinter printer(stderr); // Print
            printer.print(error);
            isError=true;
        }
    }
}

void SemanticAnalyzer::visit(IfNode &p_if) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_if.visitChildNodes(*this);
    if(p_if.getCon().getType()){
        std::string conType = p_if.getCon().getType()->getPTypeCString();
        if(conType!="boolean"){
            NonBooleanConditionError error(
                p_if.getCon().getLocation()
            );
            ErrorPrinter printer(stderr); // Print
            printer.print(error);
            isError=true;
        }
    }else{
        return;
    }
}

void SemanticAnalyzer::visit(WhileNode &p_while) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_while.visitChildNodes(*this);
    if(p_while.getCon().getType()){
        std::string conType = p_while.getCon().getType()->getPTypeCString();
        if(conType!="boolean"){
            NonBooleanConditionError error(
                p_while.getCon().getLocation()
            );
            ErrorPrinter printer(stderr); // Print
            printer.print(error);
            isError=true;
        }
    }else{
        return;
    }
}

void SemanticAnalyzer::visit(ForNode &p_for) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Traverse child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    
    SymbolTable* fortable = new SymbolTable();
    fortable->setLevel(manager.getTable()->getLevel()+1);
    manager.pushScope(fortable);
    auto &loop = p_for.m_loop_var_decl->getVariables()[0];
    if (manager.loopvarcheck(loop->getNameCString())) {
        
        SymbolRedeclarationError error(
            loop->getLocation(), 
            loop->getNameCString()
        );
 
        ErrorPrinter printer(stderr); // Print
        printer.print(error);
        isError=true;
    }else{
        
        fortable->addSymbol(
            SymbolEntry(
                loop->getNameCString(),
                "loop_var",
                manager.getTable()->getLevel(),
                loop->getTypeCString(),
                "",
                false
            )
        );
    
    }
    ConstantValueNode *begin = dynamic_cast<ConstantValueNode*>(&p_for.m_init_stmt->getExpress());
    ConstantValueNode *end = dynamic_cast<ConstantValueNode*>(&(*p_for.m_end_condition));
    int init = std::stoi(begin->getConstantValueCString());
    int ending = std::stoi(end->getConstantValueCString());
    if(init>ending){
        NonIncrementalLoopVariableError error(
            p_for.getLocation()
        );
        ErrorPrinter printer(stderr); // Print
        printer.print(error);
        isError=true;
        
    }


    //p_for.m_init_stmt->accept(*this);
    p_for.m_body->accept(*this);
    if(dump){
        fortable->dumpSymbol();
    }
    
    manager.popScope();
    
}

void SemanticAnalyzer::visit(ReturnNode &p_return) {
    p_return.visitChildNodes(*this);
    auto func = manager.getExact(FuncNameforRet);
    if(func==nullptr){
        ReturnFromVoidError error(
            p_return.getLocation()
        );
        ErrorPrinter printer(stderr); // Print
        printer.print(error);
        isError=true;
        return;
    }
    std::string Aquiretype = func->getType();
    std::string Rettype = "";
    if(Aquiretype==""){
        ReturnFromVoidError error(
            p_return.getLocation()
        );
        ErrorPrinter printer(stderr); // Print
        printer.print(error);
        isError=true;
        return;
    }

    if(p_return.getRet().getType()){
        Rettype = p_return.getRet().getType()->getPTypeCString();
        if(Aquiretype!=Rettype&&!(Aquiretype=="real"&&Rettype=="integer")){
            IncompatibleReturnTypeError error(
                p_return.getRet().getLocation(),
                Aquiretype.c_str(),
                p_return.getRet().getType().get()
            );
            ErrorPrinter printer(stderr); // Print
            printer.print(error);
            isError=true;
            return;
        }
    }else{
        return;
    }
    
    
    


}
