#pragma once

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////
    typedef intrusive_ptr<class class_type_scope_interface> class_type_scope_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
    enum PybindOperatorCompare
    {
        POC_Less,
        POC_Lessequal,
        POC_Equal,
        POC_Notequal,
        POC_Great,
        POC_Greatequal
    };
    //////////////////////////////////////////////////////////////////////////
    template<class C>
    static bool compare_equal( pybind::kernel_interface * _kernel, PyObject * _obj, C * _self, PyObject * _compare, pybind::PybindOperatorCompare _op, bool * const _result )
    {
        (void)(_obj);

        C cs_compare;
        if( pybind::extract_value( _kernel, _compare, cs_compare, true ) == false )
        {
            return false;
        }

        switch( _op )
        {
        case pybind::POC_Less:
            {
                return false;
            }break;
        case pybind::POC_Lessequal:
            {
                return false;
            }break;
        case pybind::POC_Equal:
            {
                *_result = *_self == cs_compare;
            }break;
        case pybind::POC_Notequal:
            {
                *_result = *_self != cs_compare;
            }break;
        case pybind::POC_Great:
            {
                return false;
            }break;
        case pybind::POC_Greatequal:
            {
                return false;
            }break;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    template<class C>
    static bool compare_default( pybind::kernel_interface * _kernel, PyObject * _obj, C * _self, PyObject * _compare, pybind::PybindOperatorCompare _op, bool * const _result )
    {
        (void)(_obj);

        C cs_compare;
        if( pybind::extract_value( _kernel, _compare, cs_compare, true ) == false )
        {
            return false;
        }

        switch( _op )
        {
        case pybind::POC_Less:
            {
                *_result = *_self < cs_compare;
            }break;
        case pybind::POC_Lessequal:
            {
                *_result = *_self <= cs_compare;
            }break;
        case pybind::POC_Equal:
            {
                *_result = *_self == cs_compare;
            }break;
        case pybind::POC_Notequal:
            {
                *_result = *_self != cs_compare;
            }break;
        case pybind::POC_Great:
            {
                *_result = *_self > cs_compare;
            }break;
        case pybind::POC_Greatequal:
            {
                *_result = *_self >= cs_compare;
            }break;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    class compare_adapter_interface
        : public adapter_interface
    {
    public:
        virtual bool compare( kernel_interface * _kernel, PyObject * _obj, void * _self, const class_type_scope_interface_ptr & _scope, PyObject * _compare, PybindOperatorCompare _op, bool * const _result ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef intrusive_ptr<compare_adapter_interface> compare_adapter_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
    template<class C, class F>
    class compare_adapter
        : public compare_adapter_interface
    {
    public:
        compare_adapter( F _compare )
            : m_compare( _compare )
        {
        }

    public:
        bool compare( kernel_interface * _kernel, PyObject * _obj, void * _self, const class_type_scope_interface_ptr & _scope, PyObject * _compare, PybindOperatorCompare _op, bool * const _result ) override
        {
            C * inst = _kernel->meta_cast_class_t<C>( _self, _scope );

            bool successful = (*m_compare)(_kernel, _obj, inst, _compare, _op, _result);

            return successful;
        }

    protected:
        F m_compare;
    };
}

