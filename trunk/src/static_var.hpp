#   pragma once

#   define STATIC_DECLARE(Type, Name)\
    static Type & Name()\
    {\
        static Type var;\
        return var;\
    }

#   define STATIC_DECLARE_VALUE(Type, Name, Value)\
    static Type & Name()\
    {\
    static Type var = Value;\
    return var;\
    }


#   define STATIC_DECLARE_VALUE_BEGIN(Type, Name)\
    static Type & Name()\
    {\
    static Type var =

#   define STATIC_DECLARE_ARRAY_BEGIN(Type, Name)\
    static Type * Name()\
    {\
    static Type var[] =

#   define STATIC_DECLARE_VALUE_END()\
    ;\
    return var;\
    }


#   define STATIC_VAR(Name)\
    Name()

