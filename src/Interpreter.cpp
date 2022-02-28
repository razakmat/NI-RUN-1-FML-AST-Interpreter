#include "Interpreter.h"
#include <iostream>
#include <string>

using namespace std;



ASTValue * Interpreter::visit ( ASTInteger * integer)
{
    return integer;
}

ASTValue * Interpreter::visit ( ASTBoolean * boolean)
{
    return boolean;
}

ASTValue * Interpreter::visit ( ASTNull * null)
{
    return null;
}

ASTValue * Interpreter::visit ( ASTVariable * variable)
{
    ASTValue * val = variable->m_value->Accept(*this);

    m_stack.CreateVar(variable->m_name,val);

    return val;
}

ASTValue * Interpreter::visit ( ASTAccessVariable * accessVar)
{
    return m_stack.GetValue(accessVar->m_name);
}

ASTValue * Interpreter::visit ( ASTAssignVariable * assignVar)
{
    ASTValue * val = assignVar->m_value->Accept(*this);
    m_stack.AssignValue(assignVar->m_name,val);
    return val;
}

ASTValue * Interpreter::visit ( ASTFunction * fun)
{
    m_stack.CreateFun(fun->m_name,fun);
    return new ASTNull();
}

ASTValue * Interpreter::visit ( ASTCallFunction * callFun)
{

    ASTFunction * fun = m_stack.GetFun(callFun->m_name);

    if (fun->m_param.size() != callFun->m_arg.size())
        throw InterpreterException("Error : different number of arguments and parameters in function call.");


    m_stack.PushStack();


    for (uint i = 0; i < fun->m_param.size(); i++)
    {
        m_stack.CreateVar(fun->m_param[i],callFun->m_arg[i]->Accept(*this));
    }

    ASTValue * val = fun->m_body->Accept(*this);

    m_stack.PopStack();
    return val;
}

ASTValue * Interpreter::visit ( ASTPrint * print)
{
    uint cur_arg = 0;
    for (uint i = 0; i < print->m_format.size(); i++)
    {
        if (print->m_format[i] == '~')
        {
            if (cur_arg == print->m_arg.size())
                throw InterpreterException("Error : Wrong number of arguments in print.");
            string s =  print->m_arg[cur_arg++]->Accept(*this)->Print();
            cout << s;
        }
        else if (print->m_format[i] == '\\')
        {
            if (++i == print->m_format.size())
                throw InterpreterException("Error : Wrong format in print.");
            switch (print->m_format[i]){
                case '~':
                    cout << '~';
                    break;
                case 'n':
                    cout << endl;
                    break;
                case '"':
                    cout << "\"";
                    break;
                case 'r':
                    cout << "\r";
                    break;
                case 't':
                    cout << "\t";
                    break;
                case '\\':
                    cout << "\\";
                default:
                    throw InterpreterException("Error : Wrong symbol after backslash in print.");
            }
        }
        else
            cout << print->m_format[i];
    }


    return new ASTNull();
}

ASTValue * Interpreter::visit ( ASTBlock * block)
{
    if (!block->m_stmt.empty())
    {
        m_stack.PushStack();
        ASTValue * ret = nullptr;
        for (uint i = 0; i < block->m_stmt.size(); i++)
        {
            ret = block->m_stmt[i]->Accept(*this);
        }
        m_stack.PopStack();
        return ret;
    }
    return new ASTNull();
}

ASTValue * Interpreter::visit ( ASTTop * top)
{
    ASTValue * ret = nullptr;
    m_stack.PushStack();
    for (uint i = 0; i < top->m_stmt.size(); i++)
    {
        ret = top->m_stmt[i]->Accept(*this);
    }
    m_stack.PopStack();
    return ret;
}

ASTValue * Interpreter::visit ( ASTLoop * loop)
{
    AST::Type type = loop->m_cond->Accept(*this)->Type();

    while(type != AST::Type::Null && type != AST::Type::BoolFalse)
    {
        loop->m_body->Accept(*this);
        type = loop->m_cond->Accept(*this)->Type();
    }
    return new ASTNull();
}

ASTValue * Interpreter::visit ( ASTConditional * cond)
{

    AST::Type type = cond->m_cond->Accept(*this)->Type();


    if (type != AST::Type::Null && type != AST::Type::BoolFalse)
        return cond->m_if->Accept(*this);


    return cond->m_else->Accept(*this);
}














ASTValue * Interpreter::visit ( ASTObject * obj)
{
    return new ASTNull();
}

ASTValue * Interpreter::visit ( ASTArray * arr)
{
    return new ASTNull();
}

ASTValue * Interpreter::visit ( ASTAssignArray * assignArr)
{
    return new ASTNull();
}

ASTValue * Interpreter::visit ( ASTAccessArray * accessArr)
{
    return new ASTNull();
}

ASTValue * Interpreter::visit ( ASTAssignField * assignFie)
{
    return new ASTNull();
}

ASTValue * Interpreter::visit ( ASTAccessField * accessFie)
{
    return new ASTNull();
}

ASTValue * Interpreter::visit ( ASTCallMethod * call)
{
    return new ASTNull();
}

