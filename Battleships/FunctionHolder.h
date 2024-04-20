#pragma once

struct BaseFunction 
{
	~BaseFunction() {};
	virtual void run() = 0;
};

template<typename T>
struct Functor : BaseFunction
{
	Functor(T functor) : m_functor(functor) {}
	virtual void run() { m_functor(); };
	T m_functor;
};

template<typename F, typename A>
struct FunctionWithArg : BaseFunction
{
	FunctionWithArg(F func, A arg) : m_function(func), m_argument(arg){}
	virtual void run() { m_function(m_argument); }
	F m_function;
	A m_argument;
};

template<typename C>
struct ClassFunction : BaseFunction
{
	ClassFunction(void(C::*function)(), C* object) : m_function(function), m_object(object) {}
	virtual void run() { (m_object->*m_function)(); }
	void(C::*m_function)();
	C* m_object;
};

template<typename C, typename A>
struct ClassFunctionWithArg : BaseFunction
{
	ClassFunctionWithArg(void(C::*function)(A), C* object, A argument) : m_function(function), m_object(object), m_argument(argument) {}
    virtual void run() {(m_object->*m_function)(m_argument);}
    void(C::*m_function)(A);
    C* m_object;
	A m_argument;
};
