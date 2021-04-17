#include "Compiler_Cfg.h" 
 /***************************************************************************************/
 /*2种 Memory class 的定义 */
 /* 内存类AUTOMATIC应作为空定义提供，用于声明本地指针。*/
#define AUTOMATIC 
/*
内存类TYPEDEF应作为空定义提供。此内存类应在类型定义中使用，其中不能指定内存限定符。
这对于定义指针类型(例如P2VAR)是必要的，其中宏需要两个参数。
可以在类型定义中指定第一个参数(到指针引用的内存位置的距离)，但此时不能定义第二个参数(指针本身的内存分配)。
因此，应用内存类TYPEDEF。
*/
#define TYPEDEF 
/***************************************************************************************/
/*1种 空指针 的定义*/
#define NULL_PTR                      ((void *)0) 
/***************************************************************************************/
/*内联定义 inline */
#define INLINE                        inline
/*静态定义 static */
#define LOCAL_INLINE                  static
/***************************************************************************************/
/*3种 Function 的定义*/

/*返回值不是指针的函数定义*/
#define FUNC(rettype, memclass)                                 rettype memclass
/*用于声明和定义返回常量指针的函数的宏*/
#define FUNC_P2CONST(rettype, ptrclass, memclass)               const rettype* ptrclass memclass
/*用于声明和定义返回变量指针的函数的宏*/
#define FUNC_P2VAR(rettype, ptrclass, memclass)                 rettype* ptrclass memclass
/***************************************************************************************/
/* 6种 Pointer 的定义*/

/*定义P2VAR宏，用于声明和定义RAM中的指针，指向变量。
指针本身是可修改的(例如ExamplePtr++)。指针的目标是可修改的(例如*ExamplePtr=5)。*/
#define P2VAR(ptrtype, memclass, ptrclass)                      ptrtype * ptrclass memclass

/*定义P2CONST宏，用于声明和定义RAM中指向指针本身可修改的常量的指针(例如ExamplePtr++)。
    指针的目标不可修改(只读)。*/
#define P2CONST(ptrtype, memclass, ptrclass)                    const ptrtype ptrclass * memclass

/* */
#define CONSTP2VAR(ptrtype, memclass, ptrclass)  ptrtype* const memclass ptrclass

/* 声明和定义访问常量的常量指针定义CONSTP2CONST宏。
指针本身不可修改(固定地址)。指针的目标不可修改(只读)。 */
#define CONSTP2CONST(ptrtype, memclass, ptrclass) const ptrtype * const memclass ptrclass

/* 指向函数的指针的类型定义定义P2FUNC宏。 */
#define P2FUNC(rettype, ptrclass, fctname) rettype (*ptrclass fctname)

/*指向函数的常量指针的类型定义定义CONSTP2FUNC宏*/
#define CONSTP2FUNC(rettype, ptrclass, fctname) rettype (* const ptrclass fctname) 

/***************************************************************************************/

/* 1种 常量 的定义*/

/* 定义常量声明和定义的常量宏。 */
#define CONST(type, memclass) memclass const type 

/***************************************************************************************/
/* 1种 变量 的定义*/

/* 定义变量声明和定义的变量宏。 */
#define VAR(type, memclass) memclass type 

/***************************************************************************************/