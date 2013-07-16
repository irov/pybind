#	pragma once

#	include "pybind/class_core.hpp"

namespace pybind
{
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
    class compare_adapter_interface
        : public adapter_interface
    {
    public:
        virtual bool compare( PyObject * _obj, void * _self, const class_type_scope_ptr & _scope, PyObject * _compare, PybindOperatorCompare _op, bool & _result ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<compare_adapter_interface> compare_adapter_interface_ptr;
    //////////////////////////////////////////////////////////////////////////
    template<class C, class F>
    class compare_adapter
        : public compare_adapter_interface
    {
    public:
        compare_adapter( F _compare )
            : m_compare(_compare)
        {
            m_class_name = class_info<C*>();
            m_scope_name = class_info<C>();
        }

    public:
        bool compare( PyObject * _obj, void * _self, const class_type_scope_ptr & _scope, PyObject * _compare, PybindOperatorCompare _op, bool & _result ) override
        {
            C * inst = static_cast<C*>( detail::meta_cast_scope( _self, m_scope_name, m_class_name, _scope ) );

            if( (*m_compare)( _obj, inst, _compare, _op, _result ) == false )
            {
                return false;
            }

            return true;
        }

    protected:
        F m_compare;

        size_t m_class_name;
        size_t m_scope_name;
    };
}

