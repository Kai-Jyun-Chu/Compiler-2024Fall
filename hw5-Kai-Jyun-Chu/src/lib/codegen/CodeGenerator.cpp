#include "AST/CompoundStatement.hpp"
#include "AST/for.hpp"
#include "AST/function.hpp"
#include "AST/program.hpp"
#include "codegen/CodeGenerator.hpp"
#include "sema/SemanticAnalyzer.hpp"
#include "sema/SymbolTable.hpp"
#include "visitor/AstNodeInclude.hpp"

#include <algorithm>
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

CodeGenerator::CodeGenerator(const std::string &source_file_name,
                             const std::string &save_path,
                             std::unordered_map<SemanticAnalyzer::AstNodeAddr,
                                                      SymbolManager::Table>
                                 &&p_symbol_table_of_scoping_nodes)
    : m_symbol_manager(false /* no dump */),
      m_source_file_path(source_file_name),
      m_symbol_table_of_scoping_nodes(std::move(p_symbol_table_of_scoping_nodes)) {
    // FIXME: assume that the source file is always xxxx.p
    const auto &real_path =
        save_path.empty() ? std::string{"."} : save_path;
    auto slash_pos = source_file_name.rfind('/');
    auto dot_pos = source_file_name.rfind('.');

    if (slash_pos != std::string::npos) {
        ++slash_pos;
    } else {
        slash_pos = 0;
    }
    auto output_file_path{
        real_path + "/" +
        source_file_name.substr(slash_pos, dot_pos - slash_pos) + ".S"};
    m_output_file.reset(fopen(output_file_path.c_str(), "w"));
    assert(m_output_file.get() && "Failed to open output file");
}

static void dumpInstructions(FILE *p_out_file, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(p_out_file, format, args);
    va_end(args);
}

void CodeGenerator::visit(ProgramNode &p_program) {
    // Generate RISC-V instructions for program header
    // clang-format off
    constexpr const char *const riscv_assembly_file_prologue =
        "    .file \"%s\"\n"
        "    .option nopic\n";
    // clang-format on
    dumpInstructions(m_output_file.get(), riscv_assembly_file_prologue,
                     m_source_file_path.c_str());

    // Reconstruct the scope for looking up the symbol entry.
    // Hint: Use m_symbol_manager->lookup(symbol_name) to get the symbol entry.
    m_symbol_manager.pushScope(
        std::move(m_symbol_table_of_scoping_nodes.at(&p_program)));

    
    auto visit_ast_node = [&](auto &ast_node) { ast_node->accept(*this); };
    for_each(p_program.getDeclNodes().begin(), p_program.getDeclNodes().end(),
             visit_ast_node);
    for_each(p_program.getFuncNodes().begin(), p_program.getFuncNodes().end(),
             visit_ast_node);
    constexpr const char *const main_prologue =
            ".section .text\n"
            "    .align  2\n"
            "    .globl  main\n"
            "    .type   main, @function\n"
            "main:\n"
            "    addi sp, sp, -128\n"
            "    sw ra, 124(sp)\n"
            "    sw s0, 120(sp)\n"
            "    addi s0, sp, 128\n";

        dumpInstructions(m_output_file.get(), main_prologue,
                        m_source_file_path.c_str());

    const_cast<CompoundStatementNode &>(p_program.getBody()).accept(*this);
    constexpr const char *const main_pop =
            "    lw ra, 124(sp)\n"
            "    lw s0, 120(sp)\n"
            "    addi sp, sp, 128\n"
            "    jr ra\n"
            "   .size main, .-main\n";
    dumpInstructions(m_output_file.get(), main_pop);

    m_symbol_manager.popScope();
    

}

void CodeGenerator::visit(DeclNode &p_decl) {
    
    
    p_decl.visitChildNodes(*this);
    
}

void CodeGenerator::visit(VariableNode &p_variable) {
    bool is_constant = !(p_variable.getConstantPtr()==nullptr);
    constexpr const char *const vartype_out = ".comm %s, 4, 4\n";
    constexpr const char *const varconstant = ".section .rodata\n"
                                               ".align  2\n"
                                               ".globl  %s\n"
                                               ".type   %s, @object\n"
                                               "%s:\n"
                                               "    .word   %s\n";
    //p_variable.visitChildNodes(*this);
    if(m_symbol_manager.lookup(p_variable.getNameCString())->getLevel()==0){
        if(is_constant){
            dumpInstructions(m_output_file.get(),varconstant, 
            p_variable.getNameCString(),
            p_variable.getNameCString(),
            p_variable.getNameCString(),
            p_variable.getConstantPtr()->getConstantValueCString()
            );
        }else{
            dumpInstructions(m_output_file.get(),vartype_out,p_variable.getNameCString());
        }
    }else{
        //if(is_constant){
            
        //}
        //dumpInstructions(m_output_file.get(),vartype_out,p_variable.getNameCString());
    }
}

void CodeGenerator::visit(ConstantValueNode &p_constant_value) {
    //dumpInstructions(m_output_file.get(),"reach constant\n");
    constexpr const char *const constant_assign =
    "\tli t0, %s\n"        
    "\taddi sp, sp, -4\n"
    "\tsw t0, 0(sp)\n";
    dumpInstructions(m_output_file.get(), constant_assign, p_constant_value.getConstantValueCString());
}

void CodeGenerator::visit(FunctionNode &p_function) {
    // Reconstruct the scope for looking up the symbol entry.
    m_symbol_manager.pushScope(
        std::move(m_symbol_table_of_scoping_nodes.at(&p_function)));
    constexpr const char *const function_out =
        ".section .text\n"
        "    .align  2\n"
        "    .globl  %s\n"
        "    .type   %s, @function\n"
        "%s:\n"
        "    addi sp, sp, -128\n"
        "    sw ra, 124(sp)\n"
        "    sw s0, 120(sp)\n"
        "    addi s0, sp, 128\n";
    
    dumpInstructions(m_output_file.get(), function_out,p_function.getNameCString(), p_function.getNameCString(),
                     p_function.getNameCString());
    
    constexpr const char *const store_para = "\tsw a%d, %d(s0)\n";
    int index=0;
    /*
    for(auto& thing : p_function.getParameters()){
        dumpInstructions(m_output_file.get(), store_para, index, where);
        index++;
        where-=4;
    }
    */
    int temp = where;
    where = -12;
    for(auto i=0U; i<p_function.getParameters().size();i++){
        for(auto j=0U; j<p_function.getParameters()[i]->getVariables().size();j++){
            if(index>=8){
                dumpInstructions(m_output_file.get(), "\tsw t%d, %d(s0)\n", index-8, where);
            }else{
                dumpInstructions(m_output_file.get(), store_para, index, where);
            }
            
            m_symbol_manager.lookup(p_function.getParameters()[i]->getVariables()[j].get()->getNameCString())->setSet(where);
            index++;
            where-=4;
        }
        
    }
    return_flag=flag_num;
    flag_num+=1;
    p_function.visitBodyChildNodes(*this);
    
    dumpInstructions(m_output_file.get(), "L%d:\n", return_flag);
    constexpr const char *const function_endout =
        "    lw ra, 124(sp)\n"
        "    lw s0, 120(sp)\n"
        "    addi sp, sp, 128\n"
        "    jr ra\n"
        "    .size   %s, .-%s\n";
    dumpInstructions(m_output_file.get(), function_endout,p_function.getNameCString(), p_function.getNameCString());
    where=temp;

    // Remove the entries in the hash table
    m_symbol_manager.popScope();
}

void CodeGenerator::visit(CompoundStatementNode &p_compound_statement) {
    // Reconstruct the scope for looking up the symbol entry.
    m_symbol_manager.pushScope(
        std::move(m_symbol_table_of_scoping_nodes.at(&p_compound_statement)));

    p_compound_statement.visitChildNodes(*this);

    m_symbol_manager.popScope();
}

void CodeGenerator::visit(PrintNode &p_print) {
    setvalue=true;
    p_print.visitChildNodes(*this);
    //
    std::string function_name;
    switch(p_print.getTarget().getInferredType()->getPrimitiveType()){
        case PType::PrimitiveTypeEnum::kIntegerType:
            function_name = "printInt";
            break;
        case PType::PrimitiveTypeEnum::kRealType:
            function_name = "printReal";
            break;
        case PType::PrimitiveTypeEnum::kStringType:
            function_name = "printString";
            break;
        default:
            assert(false && "Invalid type");//
    }

    if(function_name == "printReal"){
        constexpr const char *const print_instr = "\tflw fa0, 0(sp)\n"
                                  "\taddi sp, sp, 4\n"
                                  "\tjal ra, %s\n";
        dumpInstructions(m_output_file.get(), print_instr, function_name.c_str());
    } else {
        constexpr const char *const print_instr = "\tlw a0, 0(sp)\n"
                              "\taddi sp, sp, 4\n"
                              "\tjal ra, %s\n";
        dumpInstructions(m_output_file.get(), print_instr, function_name.c_str()); 
    }
}

void CodeGenerator::visit(BinaryOperatorNode &p_bin_op) {
    p_bin_op.visitChildNodes(*this);
    std::string binary_out;
    //bool condition=0;
    switch(p_bin_op.getOp()){
        case Operator::kPlusOp:            
                binary_out = "add t0, t0, t1\n";
            break;
        case Operator::kMinusOp:            
                binary_out = "sub t0, t0, t1\n";
            break;
        case Operator::kMultiplyOp:           
                binary_out = "mul t0, t0, t1\n";
            break;
        case Operator::kDivideOp:
                binary_out = "div t0, t0, t1\n";
            break;
        case Operator::kModOp:
            binary_out = "rem t0, t0, t1\n";
            break;
        case Operator::kAndOp:
            binary_out = "and t0, t0, t1\n";
            break;
        case Operator::kOrOp:
            binary_out = "or t0, t0, t1\n";
            break;
        case Operator::kLessOp:
            binary_out = "slt t0, t0, t1\n";
            break;
        case Operator::kLessOrEqualOp:
            binary_out = "slt t0, t1, t0\n"
                           "xor t0, t0, 1\n";
            break;
        case Operator::kGreaterOp:
            binary_out = "slt t0, t1, t0\n";
            break;
        case Operator::kGreaterOrEqualOp:
            binary_out = "slt t0, t0, t1\n"
                           "xor t0, t0, 1\n";
            break;
        case Operator::kEqualOp:
            binary_out = "slt t2, t0, t1\n"
                           "slt t3, t1, t0\n"
                           "or t0, t2, t3\n"
                           "xor t0, t0, 1\n";
            break;
        case Operator::kNotEqualOp:
            binary_out = "slt t2, t0, t1\n"
                           "slt t3, t1, t0\n"
                           "or t0, t2, t3\n";
            break;
        default:
            assert(false && "Invalid operator");
    }
    constexpr const char *const final_out = 
    "\tlw t1, 0(sp)\n"
    "\taddi sp, sp, 4\n"
    "\tlw t0, 0(sp)\n"
    "\taddi sp, sp, 4\n"
    "\t%s"
    "\taddi sp, sp, -4\n"
    "\tsw t0, 0(sp)\n";
    
    dumpInstructions(m_output_file.get(), final_out, binary_out.c_str());
}

void CodeGenerator::visit(UnaryOperatorNode &p_un_op) {
    p_un_op.visitChildNodes(*this);
    std::string op;
    std::string unary_out;
    switch(p_un_op.getOp()){
        case Operator::kNegOp:
            unary_out = "lw t0, 0(sp)\n"
                          "addi sp, sp, 4\n"
                          "sub t0, zero, t0\n"
                          "addi sp, sp, -4\n"
                          "sw t0, 0(sp)\n";
            break;
        case Operator::kNotOp:
            unary_out = "lw t0, 0(sp)\n"
                          "addi sp, sp, 4\n"
                          "xor t0, t0, 1\n"
                          "addi sp, sp, -4\n"
                          "sw t0, 0(sp)\n";
            break;
        default:
            assert(false && "Invalid operator");
    }
    dumpInstructions(m_output_file.get(), unary_out.c_str());
}

void CodeGenerator::visit(FunctionInvocationNode &p_func_invocation) {
    p_func_invocation.visitChildNodes(*this);
    int argument_num = p_func_invocation.getArguments().size();
    //dumpInstructions(m_output_file.get(), "//debug arg_num %d", argument_num);
    for(int i=argument_num-1;i>=0;i--){
        
        if(i >= 8){
            constexpr const char *const move_instr = 
            "\tlw t%d, 0(sp)\n"
            "\taddi sp, sp, 4\n";
            dumpInstructions(m_output_file.get(), move_instr, i-8);
        } else {
            constexpr const char *const move_instr = 
            "\tlw a%d, 0(sp)\n"
            "\taddi sp, sp, 4\n";
            dumpInstructions(m_output_file.get(), move_instr, i);
        }
    }
    
    constexpr const char *const call_out = "\tjal ra, %s\n";
    dumpInstructions(m_output_file.get(), call_out, p_func_invocation.getNameCString());
    SymbolEntry* symbol = m_symbol_manager.lookup(p_func_invocation.getNameCString());
    if(!symbol->getTypePtr()->isVoid()){
        constexpr const char *const void_out = 
        "\tmv t0, a0\n"
        "\taddi sp, sp, -4\n"
        "\tsw t0, 0(sp)\n";
        dumpInstructions(m_output_file.get(), void_out);
    }

}

void CodeGenerator::visit(VariableReferenceNode &p_variable_ref) {
    p_variable_ref.visitChildNodes(*this);
    constexpr const char *const variable_glob_assign =
    "\tla t0, %s\n"      
    "\taddi sp, sp, -4\n"
    "\tsw t0, 0(sp)\n";

    constexpr const char *const variable_glob_value =
    "\tla t0, %s\n"
    "\tlw t1, 0(t0)\n"
    "\taddi sp, sp, -4\n"
    "\tsw t1, 0(sp)\n";
    /*
    "li t0, %s\n"        
    "addi sp, sp, -4\n"
    "sw t0, 0(sp)\n";
    */
   //auto thing = m_symbol_manager.lookup(p_variable_ref.getNameCString());
   bool is_glob = (m_symbol_manager.lookup(p_variable_ref.getNameCString())->getLevel()==0);
   int depth;

   if(is_glob){
        if(setvalue){
            dumpInstructions(m_output_file.get(),variable_glob_value,p_variable_ref.getNameCString());
        }else{
            dumpInstructions(m_output_file.get(),variable_glob_assign,p_variable_ref.getNameCString());
        }
        
   }else{
        depth = m_symbol_manager.lookup(p_variable_ref.getNameCString())->getSet();
        //dumpInstructions(m_output_file.get(), "//Debug: %s, %d\n", p_variable_ref.getNameCString(), depth);
        constexpr const char *const variable_local_value = 
            "\taddi t0, s0, %d\n"
            "\tlw t1, 0(t0)\n"
            "\taddi sp, sp, -4\n"
            "\tsw t1, 0(sp)\n";

        constexpr const char *const variable_local_const = 
            "\tli t0, %s\n"
            "\taddi sp, sp, -4\n"
            "\tsw t0, 0(sp)\n";
        if(setvalue){
            if(depth==0){
                //printf("check\n");
                dumpInstructions(m_output_file.get(), variable_local_const, m_symbol_manager.lookup(p_variable_ref.getNameCString())->getAttribute().constant()->getConstantValueCString());
            }else{
                dumpInstructions(m_output_file.get(), variable_local_value, depth);
            }
                 

        }else{
            if(depth==0){
                m_symbol_manager.lookup(p_variable_ref.getNameCString())->setSet(where);
                constexpr const char *const variable_local_assign = 
                "\taddi t0, s0, %d\n"
                "\taddi sp, sp, -4\n"
                "\tsw t0, 0(sp)\n";


                dumpInstructions(m_output_file.get(), variable_local_assign, where); 
                where-=4;
            }else{
                constexpr const char *const variable_local_assign = 
                "\taddi t0, s0, %d\n"
                "\taddi sp, sp, -4\n"
                "\tsw t0, 0(sp)\n";
                //dumpInstructions(m_output_file.get(), variable_local_const, m_symbol_manager.lookup(p_variable_ref.getNameCString())->getAttribute().constant()->getConstantValueCString());
                dumpInstructions(m_output_file.get(), variable_local_assign, depth);     
                
            }
        }
   }    
    
    


}

void CodeGenerator::visit(AssignmentNode &p_assignment) {
    setvalue=false;
    p_assignment.getLvalue().accept(*this);
    setvalue=true;
    p_assignment.getExpr().accept(*this);

    constexpr const char *const assign_glob = 
    "\tlw t0, 0(sp)\n"
    "\taddi sp, sp, 4\n"
    "\tlw t1, 0(sp)\n"
    "\taddi sp, sp, 4\n"
    "\tsw t0, 0(t1)\n";

    dumpInstructions(m_output_file.get(), assign_glob);
}

void CodeGenerator::visit(ReadNode &p_read) {
    setvalue=false;
    p_read.visitChildNodes(*this);

    constexpr const char *const read_out = 
        "\tjal ra, readInt\n"
        "\tmv t0, a0\n"
        "\tlw t1, 0(sp)\n"
        "\taddi sp, sp, 4\n"
        "\tsw t0, 0(t1)\n";
    dumpInstructions(m_output_file.get(), read_out);
}

void CodeGenerator::visit(IfNode &p_if) {
    bool check_else = p_if.m_else_body != nullptr;
    p_if.m_condition->accept(*this);
    std::stack<int> flags;
    constexpr const char *const branch_out = 
    "\tlw t0, 0(sp)\n"
    "\taddi sp, sp, 4\n"
    "\tbeq t0, zero, L%d\n";
    
    //constexpr const char *const tag = "L:%d";
    flags.push(flag_num);
    flags.push(flag_num + 1);
    flag_num += 2+check_else;
    dumpInstructions(m_output_file.get(), branch_out,flags.top());
    flags.pop();
    dumpInstructions(m_output_file.get(), "L%d:\n",flags.top());
    flags.pop();
    p_if.m_body->accept(*this);
    //dumpInstructions(m_output_file.get(), "L%d:\n",flags.top());
    //flags.pop();

    if(check_else){
        constexpr const char *const jump_out = "\tj L%d\n";
        flags.push(flag_num -1);
        flags.push(flag_num -2);
        flags.push(flag_num -1);

        dumpInstructions(m_output_file.get(), jump_out, flags.top());
        flags.pop();
        dumpInstructions(m_output_file.get(), "L%d:\n",flags.top());
        flags.pop();                         
        
        p_if.m_else_body->accept(*this);
        dumpInstructions(m_output_file.get(), "L%d:\n", flags.top());
        flags.pop();
    } else {
        flags.push(flag_num -1);
        dumpInstructions(m_output_file.get(), "L%d:\n", flags.top());
        flags.pop();
    }
    
}

void CodeGenerator::visit(WhileNode &p_while) {
    std::stack<int> whilestack;
    whilestack.push(flag_num+1);
    whilestack.push(flag_num);
    flag_num+=2;
    dumpInstructions(m_output_file.get(), "L%d:\n", whilestack.top());
    whilestack.pop();
    p_while.m_condition->accept(*this);
    constexpr const char *const jump_out = 
    "\tlw t0, 0(sp)\n"
    "\taddi sp, sp, 4\n"
    "\tbeq t0, zero, L%d\n";

    dumpInstructions(m_output_file.get(), jump_out, whilestack.top());
    whilestack.pop();
    p_while.m_body->accept(*this);
    whilestack.push(flag_num-1);
    whilestack.push(flag_num-2);
    constexpr const char *const jump_out2 = "\tj L%d\n";
    dumpInstructions(m_output_file.get(), jump_out2, whilestack.top());
    whilestack.pop();
    constexpr const char *const jump_out3 = "L%d:\n";
    dumpInstructions(m_output_file.get(), jump_out3, whilestack.top());
    whilestack.pop();

}

void CodeGenerator::visit(ForNode &p_for) {
    // Reconstruct the scope for looking up the symbol entry.
    m_symbol_manager.pushScope(
        std::move(m_symbol_table_of_scoping_nodes.at(&p_for)));
    std::vector<int> forstack={flag_num,flag_num+2};
    
    p_for.m_loop_var_decl->accept(*this);
    p_for.m_init_stmt->accept(*this);
    dumpInstructions(m_output_file.get(), "L%d:\n", flag_num);

    p_for.m_end_condition->accept(*this);
    auto entry = m_symbol_manager.lookup(p_for.m_loop_var_decl->getVariables()[0]->getNameCString());

    constexpr const char *const get_loop_var_end_instr = "\taddi t0, s0, %d\n"
                                         "\tlw t0, 0(t0)\n"
                                         "\tlw t1, 0(sp)\n"
                                         "\taddi sp, sp, 4\n";
    dumpInstructions(m_output_file.get(), get_loop_var_end_instr, entry->getSet());

    constexpr const char *const exit_out = "\tbge t0, t1, L%d\n"
                                  "L%d:\n";
    dumpInstructions(m_output_file.get(), exit_out, flag_num+2, flag_num+1);
    flag_num+=3;
    p_for.m_body->accept(*this);
    constexpr const char *const increase_out = "\taddi t0, s0, %d\n"
                                    "\tlw t1, 0(t0)\n"
                                    "\taddi t1, t1, 1\n"
                                    "\tsw t1, 0(t0)\n";
    dumpInstructions(m_output_file.get(), increase_out, entry->getSet());
    constexpr const char *const jump_instr = 
    "\tj L%d\n"
    "L%d:\n";
    dumpInstructions(m_output_file.get(), jump_instr, forstack[0], forstack[1]);
    //p_for.visitChildNodes(*this);

    // Remove the entries in the hash table
    m_symbol_manager.popScope();
}

void CodeGenerator::visit(ReturnNode &p_return) {
    setvalue=true;
    p_return.visitChildNodes(*this);
    std::string dump_code = "    lw t0, 0(sp)\n"
                            "    addi sp, sp, 4\n"
                            "    mv a0, t0\n";
    dumpInstructions(m_output_file.get(), dump_code.c_str());
    //jump to function stack recover
    dumpInstructions(m_output_file.get(), "    j L%d\n",return_flag);
}
