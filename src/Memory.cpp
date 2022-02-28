#include "Memory.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>


using namespace std;


int Heap::GetCurrent()
{
    return m_current;
}

void Heap::AssignToHeap(ASTValue * val)
{
    AST::Type type = val->Type();
    m_data.push_back(static_cast<int>(type));

    switch(type){
        case AST::Type::Null:
            m_current++;
            break;
        case AST::Type::Integer:
        {
            ASTInteger * v = reinterpret_cast<ASTInteger*>(val);
            m_data.push_back((v->m_value >> 24) & 0xFF);
            m_data.push_back((v->m_value >> 16) & 0xFF);
            m_data.push_back((v->m_value >> 8) & 0xFF);
            m_data.push_back(v->m_value & 0xFF);
            m_current += 5;
            break;
        }
        case AST::Type::BoolTrue:
        {
            m_current ++;
            break;
        }
        case AST::Type::BoolFalse:
        {
            m_current ++;
            break;
        }
        default:
            break;
    }
}


void Heap::AssignToHeapFun(ASTFunction * val)
{

    unsigned char bytes[sizeof(val)];

    copy(static_cast<const char*>(static_cast<const void*>(&val)),
          static_cast<const char*>(static_cast<const void*>(&val)) 
          + sizeof val, bytes);


    m_data.push_back(6);
    m_data.push_back(sizeof(val));
    for (uint i = 0; i < sizeof(val); i++)
        m_data.push_back(bytes[i]);

    m_current += 2 + sizeof(val);

}

ASTValue * Heap::GetFromHeap(int ref)
{
    int type = m_data[ref];
    switch (type){
        case 0:
        {
            return new ASTNull();
        }
        case 1:
        {
            return new ASTInteger((m_data[ref+1]<<24)|(m_data[ref+2]<<16)
                                    |(m_data[ref+3]<<8)|(m_data[ref+4]));
        }
        case 2:
        {
            return new ASTBoolean(true);
        }
        case 3:
        {
            return new ASTBoolean(false);
        }
    }
    throw "Error";
}

ASTFunction * Heap::GetFromHeapFun(int ref)
{
    int size = m_data[ref+1];

    uint64_t pointer = 0;

    for(int i = size - 1; i >= 0; i--)
    {
        pointer <<= 8;
        pointer |= m_data[i + ref + 2];
    }

    return reinterpret_cast<ASTFunction*>(pointer);
}


Env::Env()
: m_heap(new Heap())
{

}

Env::~Env()
{
    delete m_heap;
}


void Env::CreateVar(const string & name, ASTValue * val)
{
    auto it = m_stack->m_ref.find(name);

    if ( it != m_stack->m_ref.end() )
        throw runtime_error("Error : Variable was already declared.");

    m_stack->m_ref.insert({name,m_heap->GetCurrent()});
    m_heap->AssignToHeap(val);
}

ASTValue * Env::GetValue(const string & name)
{
    Stack * now = m_stack;
    do{
        auto ref = now->m_ref.find(name);
        if (ref == now->m_ref.end())
            now = now->m_prev;
        else
            return m_heap->GetFromHeap(ref->second);
    }while(now != nullptr);

    throw runtime_error("Error : Variable was not declared.");
}

ASTFunction * Env::GetFun(const string & name)
{
    auto ref = m_refFun.find(name);
    if ( ref == m_refFun.end() )
        throw runtime_error("Error : Function does not exist.");

    return m_heap->GetFromHeapFun(ref->second);

}


void Env::AssignValue(const string & name, ASTValue * val)
{
    Stack * now = m_stack;
    do{
        auto ref = now->m_ref.find(name);
        if (ref == now->m_ref.end())
            now = now->m_prev;
        else{
            ref->second = m_heap->GetCurrent();
            return m_heap->AssignToHeap(val);
        }
    }while(now != nullptr);
    
    throw runtime_error("Error : Variable was not defined.");
}


void Env::CreateFun(const string name, ASTFunction * fun)
{
    auto it = m_refFun.find(name);

    if ( it != m_refFun.end() )
        throw runtime_error("Error : Function was already defined.");

    m_refFun.insert({name,m_heap->GetCurrent()});
    m_heap->AssignToHeapFun(fun);
}


void Env::PushStack()
{
    Stack * now = new Stack();
    now->m_prev = m_stack;
    m_stack = now;
}
void Env::PopStack()
{
    Stack * now = m_stack;
    m_stack = m_stack->m_prev;
    delete now;
}