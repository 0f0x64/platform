// Macro for adding quotes
#define STRINGIFY(X) STRINGIFY2(X)    
#define STRINGIFY2(X) #X

// Macros for concatenating tokens
#define CAT(X,Y) CAT2(X,Y)
#define CAT2(X,Y) X##Y
#define CAT_2 CAT
#define CAT_3(X,Y,Z) CAT(X,CAT(Y,Z))
#define CAT_4(A,X,Y,Z) CAT(A,CAT_3(X,Y,Z))

#define SEMI(x) x;
#define EXPAND(arg)      EXPAND1(EXPAND1(EXPAND1(EXPAND1(arg))))
#define EXPAND1(arg)     EXPAND2(EXPAND2(EXPAND2(EXPAND2(arg))))
#define EXPAND2(arg)     EXPAND3(EXPAND3(EXPAND3(EXPAND3(arg))))
#define EXPAND3(arg)     EXPAND4(EXPAND4(EXPAND4(EXPAND4(arg))))
#define EXPAND4(arg)     arg
#define PARENS ()
#define FOR_EACH_HELPER(action, x, ...) \
    action(x) \
    __VA_OPT__(FOR_EACH_AGAIN PARENS (action, __VA_ARGS__))

#define FOR_EACH_AGAIN() FOR_EACH_HELPER

#define FOR_EACH(action, ...) \
    __VA_OPT__(EXPAND(FOR_EACH_HELPER(action, __VA_ARGS__)))