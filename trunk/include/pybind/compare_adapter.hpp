#	pragma once

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
		virtual bool compare( kernel_interface * _kernel, PyObject * _obj, void * _self, const class_type_scope_interface_ptr & _scope, PyObject * _compare, PybindOperatorCompare _op, bool & _result ) = 0;
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
        }

    public:
		bool compare( kernel_interface * _kernel, PyObject * _obj, void * _self, const class_type_scope_interface_ptr & _scope, PyObject * _compare, PybindOperatorCompare _op, bool & _result ) override
        {
			uint32_t scope_name = _kernel->class_info<C>();
			uint32_t class_name = _kernel->class_info<C*>();			

			C * inst = detail::meta_cast_scope_t<C *>( _self, scope_name, class_name, _scope );

            if( (*m_compare)( _kernel, _obj, inst, _compare, _op, _result ) == false )
            {
                return false;
            }

            return true;
        }

    protected:
        F m_compare;
    };
}

