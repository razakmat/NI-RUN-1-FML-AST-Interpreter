#ifndef MEMORY_H
#define MEMORY_H


#include <unordered_map>
#include <string>
#include <vector>

#include "AST.h"

using namespace std;

class Heap
{
    public:
        int GetCurrent();
        void AssignToHeap(ASTValue * val);
        void AssignToHeapFun(ASTFunction * val);
        ASTValue * GetFromHeap(int ref);
        ASTFunction * GetFromHeapFun(int ref);
    private:
        vector<unsigned char> m_data;
        int m_current = 0;

};

class Stack
{
    public:
        unordered_map<string, int> m_ref;
        Stack * m_prev = nullptr;
};

class Env
{
    public:
                Env();
                ~Env();
        void    CreateVar(const string & name, ASTValue * val);
        void    AssignValue(const string & name, ASTValue * val);
        ASTValue * GetValue(const string & name);
        ASTFunction * GetFun(const string & name);
        void    CreateFun(const string name, ASTFunction * fun);
        void    PushStack();
        void    PopStack();
    private:
        unordered_map<string, int> m_refFun;
        Heap * m_heap;
        Stack * m_stack;

};



#endif