
/* FUNC定义 对应的 配置代码*/
/*  
    在内存超过架构可寻址范围的平台上(例如，具有512k闪存的S12X)，
    编译器需要知道调用的函数是否可以通过正常寻址命令(‘NEAR’) 或 扩展/分页寻址命令(‘FAR’)到达。

    #define <PREFIX>_CODE @near
    #define <PREFIX>_PBCFG @far
    
    1 FUNC(rettype, memclass) 
        memclass 取值 -> #define <PREFIX>_CODE
    2 FUNC_P2CONST(rettype, ptrclass, memclass)
        memclass 取值 -> #define <PREFIX>_CODE    指针常量本身的分类
        ptrclass 取值 -> #define <PREFIX>_PBCFG   定义指针距离的分类
    3 FUNC_P2VAR(rettype, ptrclass, memclass)
        memclass 取值 -> #define <PREFIX>_CODE    指针变量本身的分类
        ptrclass 取值 -> #define <PREFIX>_PBCFG   定义指针距离的分类

*/
#define EEP_CODE 
#define TEST_CODE 
#define TEST_PBCFG

#define SECOC_CODE
#define SLAVE_CODE
#define MASTER_CODE

/* Pointer定义 对应的 配置代码*/
/*
    编译器需要知道 指针 引用的数据是否可通过正常寻址命令(‘NEAR’)或扩展/分页寻址命令(‘FAR’)访问。

    #define SPI_APPL_DATA  @far   SPI模块中的远指针……
    #define SPI_VAR_FAST   @near  SPI模块中的近指针……

    声明和定义RAM中的指针，指向变量。(e.g. ExamplePtr++;*ExamplePtr = 5)
    1 #define P2VAR(ptrtype, memclass, ptrclass)
        ptrclass 取值 -> #define <模块>_APPL_DATA  指针变量本身的分类
        memclass 取值 -> #define <模块>_VAR        定义指针距离的分类

    声明和定义RAM中指向指针本身可修改的常量的指针。指针的目标不可修改(只读) (例如ExamplePtr++)
    2 #define P2CONST(ptrtype, memclass, ptrclass) const ptrtype memclass * ptrclass 
        ptrclass 取值 -> #define <模块>_APPL_CONST  指针常量本身的分类
        memclass 取值 -> #define <模块>_VAR         定义指针距离的分类

    声明和定义访问变量的常量指针定义，指针本身不可修改(固定地址)。指针的目标是可修改的(例如*ExamplePtr=18)。
    3 #define CONSTP2VAR (ptrtype, memclass, ptrclass)  ptrtype* const memclass ptrclass
        ptrclass 取值 -> #define <模块>_APPL_DATA  指针变量本身的分类
        memclass 取值 -> #define <模块>_VAR        定义指针距离的分类

    声明和定义访问常量的常量指针定义CONSTP2CONST宏。指针本身不可修改(固定地址)。指针的目标不可修改(只读)。
    4 #define  CONSTP2CONST (ptrtype, memclass, ptrclass) const ptrtype * const memclass ptrclass
        ptrclass 取值 -> #define <模块>_PBCFG_CONST 
        memclass 取值 -> #define <模块>_CONST 

    指向函数的指针的类型定义定义P2FUNC宏。
    5 #define P2FUNC(rettype, ptrclass, fctname) rettype (*ptrclass fctname)

    指向函数的常量指针的类型定义定义CONSTP2FUNC宏
    6 #define CONSTP2FUNC(rettype, ptrclass, fctname) rettype (* const ptrclass fctname) 

*/
#define EEP_APPL_DATA
#define TEST_APPL_DATA

// ptrclass
#define SECOC_APPL_DATA
#define SECOC_APPL_CONST

#define SLAVE_APPL_DATA
#define SLAVE_APPL_CONST

#define MASTER_APPL_DATA
#define MASTER_APPL_CONST
// memclass

// AUTOMATIC


/*
    Constant定义 对应的 配置代码
*/
/*
    定义常量声明和定义的常量宏。
    #define CONST(type, memclass) memclass const type 
        memclass  #define <模块>_CONST   常量本身的分类
        type 
*/
#define CONST(type, memclass) memclass const type

/* var 定义 对应的 配置代码 */
/*
    定义变量声明和定义的变量宏
    #define VAR(type, memclass) memclass type 
        memclass #define <模块>_VAR    变量本身的分类
*/
#define VAR(type, memclass) memclass type

#define STD_TYPES_VAR
#define COMSTACK_TYPES_VAR
#define RTE_SECOC_TYPES_VAR
#define FRESH_VAR
