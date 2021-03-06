#include "Python.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined _MSC_VER && _MSC_VER >= 1200
    int _PyVerify_fd( int fd )
    {
        (void)fd;

        return 1;
    }
#endif

    //extern void PyMarshal_Init(void);
    extern void initimp( void );
    extern void initgc( void );
    //extern void init_ast(void);
    extern void initunicodedata( void );
    extern void initmath( void );
    extern void init_sre( void );
    extern void init_struct( void );
    //extern void initnt(void);
    //extern void initcPickle(void);
    //extern void initcStringIO(void);
    extern void init_weakref( void );
    extern void initbinascii( void );
    //extern void initzlib( void );

    struct _inittab _PyImport_Inittab[] = {

        /* -- ADDMODULE MARKER 2 -- */
        //{"nt", initnt}, /* Use the NT os functions, not posix */

        /* This module lives in marshal.c */
        //{"marshal", PyMarshal_Init},

        /* This lives in import.c */
        {"imp", initimp},
    {"math", initmath},
    {"_sre", init_sre},
    {"_struct", init_struct},

    /* This lives in Python/Python-ast.c */
    //{"_ast", init_ast},
    /* These entries are here for sys.builtin_module_names */
    {"__main__", NULL},
    {"__builtin__", NULL},
    {"sys", NULL},
    {"exceptions", NULL},
    {"binascii", initbinascii},
    //{"zlib", initzlib},

    /* This lives in gcmodule.c */
    {"gc", initgc},

    //{"cPickle", initcPickle},
    //{"cStringIO", initcStringIO},
    {"_weakref", init_weakref},

    /* Sentinel */
    {0, 0}
    };

#ifdef __cplusplus
}
#endif

