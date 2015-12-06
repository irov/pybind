#	include "pybind/class_type_scope.hpp"

#	include "pybind/new_adapter.hpp"
#	include "pybind/destroy_adapter.hpp"

#	include "pybind/repr_adapter.hpp"
#	include "pybind/hash_adapter.hpp"
#	include "pybind/convert_adapter.hpp"
#	include "pybind/compare_adapter.hpp"
#	include "pybind/constructor_adapter.hpp"

#	include "pybind/getattro_adapter.hpp"
#	include "pybind/mapping_adapter.hpp"
#	include "pybind/sequence_get_adapter.hpp"
#	include "pybind/sequence_set_adapter.hpp"
#	include "pybind/number_binary_adapter.hpp"
#	include "pybind/smart_pointer_adapter.hpp"

#	include "pod.hpp"
#	include "pybind/detail.hpp"

#	include "pybind/exception.hpp"

#	include "pybind/system.hpp"
#	include "config/python.hpp"

#	include "method_type.hpp"
#	include "member_type.hpp"

#	include "pybind/debug.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	namespace detail
	{
		//////////////////////////////////////////////////////////////////////////
		static void wrap( PyObject * _obj, void * _impl, bool _holder )
		{
			py_ptr_object * py_ptr = (py_ptr_object *)_obj;

			py_ptr->impl = _impl;

			py_ptr->flag |= PY_OBJECT_PTR;

			if( _holder == true )
			{
				py_ptr->flag |= PY_OBJECT_HOLDER;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		static void wrap_pod64( PyObject * _obj, void ** _impl )
		{
			py_pod64_object * py_pod = (py_pod64_object *)_obj;

			py_pod->flag |= PY_OBJECT_POD64;

			*_impl = (void *)py_pod->buff;
		}
		//////////////////////////////////////////////////////////////////////////
		static void wrap_pod32( PyObject * _obj, void ** _impl )
		{
			py_pod32_object * py_pod = (py_pod32_object *)_obj;

			py_pod->flag |= PY_OBJECT_POD32;

			*_impl = (void *)py_pod->buff;
		}
		//////////////////////////////////////////////////////////////////////////
		static void wrap_pod16( PyObject * _obj, void ** _impl )
		{
			py_pod16_object * py_pod = (py_pod16_object *)_obj;

			py_pod->flag |= PY_OBJECT_POD16;

			*_impl = (void *)py_pod->buff;
		}
		//////////////////////////////////////////////////////////////////////////
		static void wrap_pod8( PyObject * _obj, void ** _impl )
		{
			py_pod8_object * py_pod = (py_pod8_object *)_obj;

			py_pod->flag |= PY_OBJECT_POD8;

			*_impl = (void *)py_pod->buff;
		}
		//////////////////////////////////////////////////////////////////////////
		static void wrap_pod4( PyObject * _obj, void ** _impl )
		{
			py_pod4_object * py_pod = (py_pod4_object *)_obj;

			py_pod->flag |= PY_OBJECT_POD4;

			*_impl = (void *)py_pod->buff;
		}
		//////////////////////////////////////////////////////////////////////////
		static void wrap_pod( PyObject * _obj, void ** _impl, size_t _size )
		{
			if( _size > PYBIND_OBJECT_POD_SIZE )
			{
				pybind::throw_exception( "wrap_pod obj %s size %d > max pod size %d"
					, pybind::object_str( _obj )
					, _size
					, PYBIND_OBJECT_POD_SIZE
					);

				return;
			}

			if( _size > 32 )
			{
				wrap_pod64( _obj, _impl );
			}
			else if( _size > 16 )
			{
				wrap_pod32( _obj, _impl );
			}
			else if( _size > 8 )
			{
				wrap_pod16( _obj, _impl );
			}
			else if( _size > 4 )
			{
				wrap_pod8( _obj, _impl );
			}
			else
			{
				wrap_pod4( _obj, _impl );
			}
		}
		//////////////////////////////////////////////////////////////////////////
		static bool is_holder( PyObject * _obj )
		{
			py_base_object * py_base = (py_base_object *)_obj;

			if( py_base->flag & PY_OBJECT_HOLDER )
			{
				return true;
			}

			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		static PyObject * alloc_class( PyTypeObject * _type, PyObject * _args, PyObject * _kwds )
		{
			(void)_args;
			(void)_kwds;

			PyObject * py_self = PyType_GenericAlloc( _type, 0 );

			if( py_self == nullptr )
			{
				if( PyErr_Occurred() )
				{
					PyErr_Print();
				}

				return nullptr;
			}

			return py_self;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_callfunc( PyObject * _obj, PyObject * _args, PyObject * _kwds )
	{
		void * impl = pybind::detail::get_class_impl( _obj );

		if( impl == nullptr )
		{
			pybind::error_message( "pybind: callfunc unbind object" );

			return nullptr;
		}

		PyTypeObject * objtype = Py_TYPE( _obj );

		const class_type_scope_ptr & scope = detail::get_class_scope( objtype );
		
		const method_adapter_interface_ptr & adapter = scope->get_call_adapter();

		try
		{
			DEBUG_PYBIND_NOTIFY_BEGIN_BIND_CALL( scope->get_name(), adapter->getName(), _args, _kwds );
			PyObject * ret = adapter->call( impl, scope, _args, _kwds );
			DEBUG_PYBIND_NOTIFY_END_BIND_CALL( scope->get_name(), adapter->getName(), _args, _kwds );

			return ret;
		}
		catch( const pybind_exception & _ex )
		{
			pybind::error_message( "obj %s py_callfunc invalid call '%s' error '%s'\n"
				, pybind::object_str( _obj )
				, adapter->getName()
				, _ex.what()
				);
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_reprfunc( PyObject * _obj )
	{
		void * impl = pybind::detail::get_class_impl( _obj );

		if( impl == nullptr )
		{
			pybind::error_message( "pybind: reprfunc unbind object" );

			return nullptr;
		}

		PyTypeObject * objtype = Py_TYPE( _obj );

		const class_type_scope_ptr & scope = detail::get_class_scope( objtype );

		const repr_adapter_interface_ptr & adapter = scope->get_repr_adapter();

		try
		{
			PyObject * ret = adapter->repr( impl, scope );

			return ret;
		}
		catch( const pybind_exception & _ex )
		{
			pybind::error_message( "obj %s py_reprfunc invalid call error '%s'\n"
				, pybind::object_str( _obj )
				, _ex.what()
				);
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	static long py_hash( PyObject * _obj )
	{
		void * impl = pybind::detail::get_class_impl( _obj );

		if( impl == nullptr )
		{
			pybind::error_message( "pybind: reprfunc unbind object" );

			return 0;
		}

		PyTypeObject * objtype = Py_TYPE( _obj );

		const class_type_scope_ptr & scope = detail::get_class_scope( objtype );

		const hash_adapter_interface_ptr & adapter = scope->get_hash_adapter();

		try
		{
			long hash = adapter->hash( impl, scope );

			return hash;
		}
		catch( const pybind_exception & _ex )
		{
			pybind::error_message( "obj %s py_hash invalid call '%s' error '%s'\n"
				, pybind::object_str( _obj )
				, _ex.what()
				);
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_richcompare( PyObject * _obj, PyObject * _compare, int _op )
	{
		void * impl = pybind::detail::get_class_impl( _obj );

		if( impl == nullptr )
		{
			pybind::error_message( "py_richcompare %s compare unbind object"
				, pybind::object_str( _obj )
				);

			Py_INCREF( Py_NotImplemented );

			return Py_NotImplemented;
		}

		PyTypeObject * objtype = Py_TYPE( _obj );
				
		const class_type_scope_ptr & scope = detail::get_class_scope( objtype );

		PybindOperatorCompare pybind_op = POC_Less;

		switch( _op )
		{
		case Py_LT:
			{
				pybind_op = POC_Less;
			}break;
		case Py_LE:
			{
				pybind_op = POC_Lessequal;
			}break;
		case Py_EQ:
			{
				pybind_op = POC_Equal;
			}break;
		case Py_NE:
			{
				pybind_op = POC_Notequal;
			}break;
		case Py_GT:
			{
				pybind_op = POC_Great;
			}break;
		case Py_GE:
			{
				pybind_op = POC_Greatequal;
			}break;
		default:
			{
				pybind::error_message( "invalid compare op '%d'\n"
					, _op
					);
			}break;
		}

		try
		{
			bool test_result;

			const compare_adapter_interface_ptr & adapter = scope->get_compare_adapter();

			if( adapter->compare( _obj, impl, scope, _compare, pybind_op, test_result ) == false )
			{
				Py_INCREF( Py_NotImplemented );

				return Py_NotImplemented;
			}

			PyObject * py_result = pybind::ret_bool( test_result );

			return py_result;
		}
		catch( const pybind_exception & _ex )
		{
			pybind::error_message( "obj %s py_richcompare %s invalid compare %s\n"
				, pybind::object_str( _obj )
				, pybind::object_str( _obj )
				, _ex.what()
				);
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_getattro( PyObject * _obj, PyObject * _key )
	{
		void * impl = pybind::detail::get_class_impl( _obj );

		if( impl == nullptr )
		{
			pybind::error_message( "getattro %s unbind object"
				, pybind::object_str( _obj )
				);

			return nullptr;
		}

		PyTypeObject * objtype = Py_TYPE( _obj );

		const class_type_scope_ptr & scope = detail::get_class_scope( objtype );

		const getattro_adapter_interface_ptr & adapter = scope->get_getattro_adapter();

		try
		{
			DEBUG_PYBIND_NOTIFY_BEGIN_BIND_CALL( scope->get_name(), adapter->getName(), _key, nullptr );
			PyObject * res = adapter->call( impl, scope, _key );
			DEBUG_PYBIND_NOTIFY_END_BIND_CALL( scope->get_name(), adapter->getName(), _key, nullptr );

			return res;
		}
		catch( const pybind_exception & _ex )
		{
			pybind::error_message( "obj %s py_getattro invalid call '%s' error '%s'\n"
				, pybind::object_str( _obj )
				, adapter->getName()
				, _ex.what()
				);
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_subscript( PyObject * _obj, PyObject * _key )
	{
		void * impl = pybind::detail::get_class_impl( _obj );

		if( impl == nullptr )
		{
			pybind::error_message( "pybind: subscript unbind object" );

			return nullptr;
		}

		PyTypeObject * objtype = Py_TYPE( _obj );

		const class_type_scope_ptr & scope = detail::get_class_scope( objtype );

		const mapping_adapter_interface_ptr & adapter = scope->get_mapping_adapter();

		try
		{
			DEBUG_PYBIND_NOTIFY_BEGIN_BIND_CALL( scope->get_name(), adapter->getName(), _key, nullptr );
			PyObject * res = adapter->call( impl, scope, _key );
			DEBUG_PYBIND_NOTIFY_END_BIND_CALL( scope->get_name(), adapter->getName(), _key, nullptr );

			return res;
		}
		catch( const pybind_exception & _ex )
		{
			pybind::error_message( "obj %s py_subscript invalid call '%s' error '%s'\n"
				, pybind::object_str( _obj )
				, adapter->getName()
				, _ex.what()
				);
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	static PyMappingMethods py_as_mapping = {
		(lenfunc)0,		/* mp_length */
		(binaryfunc)&py_subscript,		/* mp_subscript */
		(objobjargproc)0,	/* mp_ass_subscript */
	};
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_item_get( PyObject * _obj, Py_ssize_t _index )
	{
		void * impl = pybind::detail::get_class_impl( _obj );

		if( impl == nullptr )
		{
			pybind::error_message( "pybind: subscript unbind object" );

			return nullptr;
		}

		PyTypeObject * objtype = Py_TYPE( _obj );

		const class_type_scope_ptr & scope = detail::get_class_scope( objtype );

		const sequence_get_adapter_interface_ptr & adapter = scope->get_sequence_get_adapter();

		try
		{
			DEBUG_PYBIND_NOTIFY_BEGIN_BIND_CALL( scope->get_name(), adapter->getName(), nullptr, nullptr );
			PyObject * res = adapter->call( impl, scope, (size_t)_index );
			DEBUG_PYBIND_NOTIFY_END_BIND_CALL( scope->get_name(), adapter->getName(), nullptr, nullptr );

			return res;
		}
		catch( const pybind_exception & _ex )
		{
			pybind::error_message( "obj %s py_item_get invalid call '%s' error '%s'\n"
				, pybind::object_str( _obj )
				, adapter->getName()
				, _ex.what()
				);
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	static int py_item_set( PyObject * _obj, Py_ssize_t _index, PyObject * _value )
	{
		void * impl = pybind::detail::get_class_impl( _obj );

		if( impl == nullptr )
		{
			pybind::error_message( "pybind: subscript unbind object" );

			return -1;
		}

		PyTypeObject * objtype = Py_TYPE( _obj );

		const class_type_scope_ptr & scope = detail::get_class_scope( objtype );

		const sequence_set_adapter_interface_ptr & adapter = scope->get_sequence_set_adapter();

		try
		{
			DEBUG_PYBIND_NOTIFY_BEGIN_BIND_CALL( scope->get_name(), adapter->getName(), nullptr, nullptr );
			adapter->call( impl, scope, _index, _value );
			DEBUG_PYBIND_NOTIFY_END_BIND_CALL( scope->get_name(), adapter->getName(), nullptr, nullptr );

			return 0;
		}
		catch( const pybind_exception & _ex )
		{
			pybind::error_message( "obj %s py_item_set invalid call '%s' error '%s'\n"
				, pybind::object_str( _obj )
				, adapter->getName()
				, _ex.what()
				);
		}

		return -1;
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_new_class( PyTypeObject * _type, PyObject * _args, PyObject * _kwds )
	{		
		const class_type_scope_ptr & scope = detail::get_class_scope( _type );

		try
		{
			PyObject * py_self = detail::alloc_class( _type, _args, _kwds );

			void * impl;

			const new_adapter_interface_ptr & adapter = scope->get_new_adapter();

			if( adapter != nullptr )
			{
				impl = adapter->call( scope, py_self, _args, _kwds );
			}
			else
			{
				impl = scope->construct( py_self, _args );
			}

			if( impl == nullptr )
			{
				Py_DECREF( py_self );

				return nullptr;
			}

			pybind::detail::wrap( py_self, impl, false );

			const smart_pointer_adapter_interface_ptr & smart_pointer_adapter = scope->get_smart_pointer();

			if( smart_pointer_adapter != nullptr )
			{
				smart_pointer_adapter->incref( impl, scope );
			}

			scope->addObject( py_self );

			return py_self;
		}
		catch( const pybind_exception & _ex )
		{
			pybind::error_message( "obj %s py_new_class scope '%s' error '%s'\n"
				, pybind::object_str( (PyObject *)_type )
				, scope->get_name()
				, _ex.what()
				);
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	static void py_del_class( PyObject * _obj )
	{
		PyTypeObject * objtype = Py_TYPE( _obj );
		
		const class_type_scope_ptr & scope = detail::get_class_scope( objtype );

		try
		{
			scope->removeObject( _obj );

			void * impl = pybind::detail::get_class_impl( _obj );

			const smart_pointer_adapter_interface_ptr & smart_pointer_adapter = scope->get_smart_pointer();

			if( smart_pointer_adapter != nullptr )
			{
				smart_pointer_adapter->decref( impl, scope );
			}

			bool holder = pybind::detail::is_holder( _obj );

			if( holder == false )
			{
				const destroy_adapter_interface_ptr & adapter = scope->get_destroy_adapter();
								
				if( adapter != nullptr )
				{
					adapter->call( scope, impl );
				}
			}
		}
		catch( const pybind_exception & _ex )
		{
			pybind::error_message( "obj %s py_del_class scope %s error '%s'\n"
				, pybind::object_str( _obj )
				, scope->get_name()
				, _ex.what()
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_new_pod( PyTypeObject * _type, PyObject * _args, PyObject * _kwds )
	{		
		const class_type_scope_ptr & scope = detail::get_class_scope( _type );

		try
		{
			PyObject * py_self = detail::alloc_class( _type, _args, _kwds );

			void * buff = nullptr;
			uint32_t pod_size = scope->is_pod();

			pybind::detail::wrap_pod( py_self, &buff, pod_size );

			void * impl;

			const new_adapter_interface_ptr & adapter = scope->get_new_adapter();

			if( adapter != nullptr )
			{
				impl = adapter->call( scope, py_self, _args, _kwds );
			}
			else
			{
				impl = scope->construct( py_self, _args );
			}

			const smart_pointer_adapter_interface_ptr & smart_pointer_adapter = scope->get_smart_pointer();

			if( smart_pointer_adapter != nullptr )
			{
				smart_pointer_adapter->incref( impl, scope );
			}

			scope->addObject( py_self );

			return py_self;
		}
		catch( const pybind_exception & _ex )
		{
			pybind::error_message( "obj %s py_new_class scope '%s' error '%s'\n"
				, pybind::object_str( (PyObject *)_type )
				, scope->get_name()
				, _ex.what()
				);
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	static void py_del_pod( PyObject * _obj )
	{
		PyTypeObject * objtype = Py_TYPE( _obj );

		const class_type_scope_ptr & scope = detail::get_class_scope( objtype );

		try
		{
			scope->removeObject( _obj );

			void * impl = pybind::detail::get_class_impl( _obj );

			const smart_pointer_adapter_interface_ptr & smart_pointer_adapter = scope->get_smart_pointer();

			if( smart_pointer_adapter != nullptr )
			{
				smart_pointer_adapter->decref( impl, scope );
			}

			const destroy_adapter_interface_ptr & adapter = scope->get_destroy_adapter();

			if( adapter != nullptr )
			{
				adapter->call( scope, impl );
			}
		}
		catch( const pybind_exception & _ex )
		{
			pybind::error_message( "obj %s py_del_pod scope '%s' error '%s'\n"
				, pybind::object_str( _obj )
				, scope->get_name()
				, _ex.what()
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	class_type_scope::class_type_scope( const char * _name, uint32_t _typeId, void * _user, const new_adapter_interface_ptr & _pynew, const destroy_adapter_interface_ptr & _pydestructor, uint32_t _pod )
		: m_name( _name )
		, m_typeId( _typeId )
		, m_user( _user )
		, m_objectCount( 0 )
		, m_new( _pynew )
		, m_destructor( _pydestructor )
		, m_pod( _pod )
		, m_pytypeobject( nullptr )
		, m_basesCount( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	class_type_scope::~class_type_scope()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::initialize( PyObject * _module )
	{
		PyObject * py_module = _module;

		if( py_module == nullptr )
		{
			py_module = pybind::get_currentmodule();

			if( py_module == nullptr )
			{
				pybind::throw_exception( "scope %s initialize not setup python module"
					, m_name
					);

				return;
			}
		}

#   if PYBIND_PYTHON_VERSION < 300
		PyObject * py_name = PyString_InternFromString( m_name );
#	else
		PyObject * py_name = PyUnicode_InternFromString( m_name );
#	endif

		PyObject * py_bases;

		if( m_basesCount > 0 )
		{
			Py_ssize_t py_basesCount = (Py_ssize_t)m_basesCount;
			py_bases = PyTuple_New( py_basesCount );
			Py_ssize_t index = 0;

			for( uint32_t i = 0; i != m_basesCount; ++i )
			{
				Metacast & mc = m_bases[i];
				
				PyTypeObject * py_base = mc.scope->get_typemodule();

				Py_INCREF( (PyObject *)py_base );
				PyTuple_SetItem( py_bases, index++, (PyObject*)py_base );
			}
		}
		else
		{
			py_bases = PyTuple_New( 1 );
						
			PyTypeObject * py_pybind_type = detail::get_pod_type( m_pod );

			if( py_pybind_type == nullptr )
			{
				pybind::throw_exception( "scope %s initialize not pod %d"
					, m_name
					, m_pod
					);
			}

			PyTuple_SetItem( py_bases, 0, (PyObject *)py_pybind_type );
		}

		PyObject * py_dict = PyDict_New();

		PyObject * py_pybind_scope_id = pybind::ptr_throw( m_typeId );

		PyObject * py_str_class_type_scope = detail::get_str_class_type_scope_();

		PyDict_SetItem( py_dict, py_str_class_type_scope, py_pybind_scope_id );
		Py_DECREF( py_pybind_scope_id );

		PyObject * py_args = PyTuple_Pack( 3, py_name, py_bases, py_dict );
		Py_DECREF( py_name );
		Py_DECREF( py_bases );
		Py_DECREF( py_dict );

		m_pytypeobject = (PyTypeObject *)PyType_Type.tp_call( (PyObject *)&PyType_Type, py_args, 0 );
		Py_DECREF( py_args );

		if( m_pytypeobject == nullptr )
		{
			pybind::throw_exception( "scope %s initialize invalide not create type"
				, m_name
				);

			return;
		}

        if( m_pod == 0 )
        {
			m_pytypeobject->tp_new = py_new_class;
			m_pytypeobject->tp_del = py_del_class;
		}
		else
		{
			m_pytypeobject->tp_new = py_new_pod;
			m_pytypeobject->tp_del = py_del_pod;
		}

		PyType_Modified( m_pytypeobject );

		Py_INCREF( (PyObject*)m_pytypeobject );
		PyModule_AddObject( py_module, m_pytypeobject->tp_name, (PyObject*)m_pytypeobject );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::finalize()
	{
		PyDict_Clear( m_pytypeobject->tp_dict );

		Py_DECREF( (PyObject*)m_pytypeobject );

		m_constructor = nullptr;

		m_convert = nullptr;
		m_call = nullptr;
		m_repr = nullptr;
		m_hash = nullptr;
		m_getattro = nullptr;
		m_mapping = nullptr;
		m_sequence_get = nullptr;
		m_sequence_set = nullptr;
		m_compare = nullptr;

		for( uint32_t i = 0; i != m_basesCount; ++i )
		{
			Metacast & m = m_bases[i];

			m.info = 0;
			m.cast = nullptr;
			m.scope = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::type_initialize( PyTypeObject * _type )
	{
		_type->tp_del = py_del_class;
	}
	//////////////////////////////////////////////////////////////////////////
	bool class_type_scope::is_instance( PyTypeObject * _type ) const
	{
		if( PyType_IsSubtype( _type, m_pytypeobject ) == 0 )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const char * class_type_scope::get_name() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t class_type_scope::get_type_id() const
	{
		return m_typeId;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t class_type_scope::is_pod() const
	{
		return m_pod;
	}
	//////////////////////////////////////////////////////////////////////////
	void * class_type_scope::get_user() const
	{
		return m_user;
	}
	//////////////////////////////////////////////////////////////////////////
	PyTypeObject * class_type_scope::get_typemodule() const
	{
		return m_pytypeobject;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace detail
	{
		class FCharCmp
		{
		public:
			FCharCmp( const char * _name )
				: m_name( _name )
			{
			}

		public:
			bool operator () ( const char * _name ) const
			{
				return strcmp( m_name, _name ) == 0;
			}

		protected:
			const char * m_name;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_method( const method_adapter_interface_ptr & _ifunc )
	{
		PyObject * py_type_method = method_type_scope::instance( _ifunc, m_pytypeobject );

		const char * name = _ifunc->getName();

		if( PyDict_SetItemString( m_pytypeobject->tp_dict, name, py_type_method ) == -1 )
		{
			pybind::throw_exception( "scope %s add_method %s python error"
				, this->m_name
				, name
				);

			return;
		}

		Py_DECREF( py_type_method );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_member( const member_adapter_interface_ptr & _imember )
	{
		PyObject * py_member = member_type_scope::instance( _imember );

		const char * name = _imember->getName();

		if( PyDict_SetItemString( m_pytypeobject->tp_dict, name, py_member ) == -1 )
		{
			pybind::throw_exception( "scope %s add_member %s python error"
				, this->m_name
				, name
				);

			return;
		}

		Py_DECREF( py_member );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_convert( const convert_adapter_interface_ptr & _iconvert )
	{
		m_convert = _iconvert;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_call( const method_adapter_interface_ptr & _icall )
	{
		m_call = _icall;

		m_pytypeobject->tp_call = &py_callfunc;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_repr( const repr_adapter_interface_ptr & _irepr )
	{
		m_repr = _irepr;

		m_pytypeobject->tp_repr = &py_reprfunc;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_hash( const hash_adapter_interface_ptr & _ihash )
	{
		m_hash = _ihash;

		m_pytypeobject->tp_hash = (hashfunc)&py_hash;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_compare( const compare_adapter_interface_ptr & _icompare )
	{
		m_compare = _icompare;

		m_pytypeobject->tp_richcompare = &py_richcompare;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_getattro( const getattro_adapter_interface_ptr & _igetattro )
	{
		m_getattro = _igetattro;

		m_pytypeobject->tp_getattro = &py_getattro;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_mapping( const mapping_adapter_interface_ptr & _imapping )
	{
		m_mapping = _imapping;

		m_pytypeobject->tp_as_mapping = &py_as_mapping;
	}
	//////////////////////////////////////////////////////////////////////////
	static PySequenceMethods py_as_sequence = {
		(lenfunc)0,                       /* sq_length */
		(binaryfunc)0,                    /* sq_concat */
		(ssizeargfunc)0,                  /* sq_repeat */
		(ssizeargfunc)&py_item_get,                    /* sq_item */
		(ssizessizeargfunc)0,              /* sq_slice */
		(ssizeobjargproc)&py_item_set,             /* sq_ass_item */
		(ssizessizeobjargproc)0,       /* sq_ass_slice */
		(objobjproc)0,                  /* sq_contains */
		(binaryfunc)0,            /* sq_inplace_concat */
		(ssizeargfunc)0,          /* sq_inplace_repeat */
	};
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_sequence_get( const sequence_get_adapter_interface_ptr & _isequence )
	{
		m_sequence_get = _isequence;

		m_pytypeobject->tp_as_sequence = &py_as_sequence;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_sequence_set( const sequence_set_adapter_interface_ptr & _isequence )
	{
		m_sequence_set = _isequence;

		m_pytypeobject->tp_as_sequence = &py_as_sequence;
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_nb_method( PyObject * _obj, PyObject * _value, const class_type_scope_ptr & _scope, const number_binary_adapter_interface_ptr & _adapter )
	{
		void * impl = pybind::detail::get_class_impl( _obj );

		if( impl == nullptr )
		{
			pybind::error_message( "pybind: subscript unbind object" );

			return nullptr;
		}
		
		DEBUG_PYBIND_NOTIFY_BEGIN_BIND_CALL( _scope->get_name(), _adapter->getName(), nullptr, nullptr );
		PyObject * res = _adapter->call( impl, _scope, _value );
		DEBUG_PYBIND_NOTIFY_END_BIND_CALL( _scope->get_name(), _adapter->getName(), nullptr, nullptr );

		return res;
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_nb_add_2( PyObject * _obj, PyObject * _value )
	{
		PyTypeObject * objtype = Py_TYPE( _obj );

		const class_type_scope_ptr & scope = detail::get_class_scope( objtype );

		const number_binary_adapter_interface_ptr & adapter = scope->get_number_add_adapter();

		if( adapter != nullptr )
		{
			try
			{
				PyObject * result = py_nb_method( _obj, _value, scope, adapter );

				return result;
			}
			catch( const pybind_exception & )
			{
			}
		}

		uint32_t type_id = detail::get_object_type_id( _value );

		if( type_id == 0 )
		{
			return nullptr;
		}

		const number_binary_adapter_interface_ptr & adapters = scope->get_number_add_adapters( type_id );

		if( adapters != nullptr )
		{
			try
			{
				PyObject * result = py_nb_method( _obj, _value, scope, adapters );

				return result;
			}
			catch( const pybind_exception & )
			{
			}
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_nb_add( PyObject * _obj, PyObject * _value )
	{
		if( detail::is_class( _obj ) == true )
		{
			return py_nb_add_2( _obj, _value );
		}
		
		return py_nb_add_2( _value, _obj );
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_nb_subtract_2( PyObject * _obj, PyObject * _value )
	{
		PyTypeObject * objtype = Py_TYPE( _obj );

		const class_type_scope_ptr & scope = detail::get_class_scope( objtype );

		const number_binary_adapter_interface_ptr & adapter = scope->get_number_sub_adapter();

		if( adapter != nullptr )
		{
			try
			{
				PyObject * result = py_nb_method( _obj, _value, scope, adapter );

				return result;
			}
			catch( const pybind_exception & )
			{
			}
		}

		uint32_t type_id = detail::get_object_type_id( _value );

		if( type_id == 0 )
		{
			return nullptr;
		}

		const number_binary_adapter_interface_ptr & adapters = scope->get_number_sub_adapters( type_id );

		if( adapters != nullptr )
		{
			try
			{
				PyObject * result = py_nb_method( _obj, _value, scope, adapters );

				return result;
			}
			catch( const pybind_exception & )
			{
			}
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_nb_subtract( PyObject * _obj, PyObject * _value )
	{
		if( detail::is_class( _obj ) == true )
		{
			return py_nb_subtract_2( _obj, _value );
		}

		return py_nb_subtract_2( _value, _obj );
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_nb_multiply_2( PyObject * _obj, PyObject * _value )
	{
		PyTypeObject * objtype = Py_TYPE( _obj );

		const class_type_scope_ptr & scope = detail::get_class_scope( objtype );

		const number_binary_adapter_interface_ptr & adapter = scope->get_number_mul_adapter();

		if( adapter != nullptr )
		{
			try
			{
				PyObject * result = py_nb_method( _obj, _value, scope, adapter );

				if( result != nullptr )
				{
					return result;
				}
			}
			catch( const pybind_exception & )
			{
			}
		}

		uint32_t type_id = detail::get_object_type_id( _value );

		if( type_id == 0 )
		{
			return nullptr;
		}

		const number_binary_adapter_interface_ptr & adapters = scope->get_number_mul_adapters( type_id );

		if( adapters != nullptr )
		{
			try
			{
				PyObject * result = py_nb_method( _obj, _value, scope, adapters );

				return result;
			}
			catch( const pybind_exception & )
			{
			}
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_nb_multiply( PyObject * _obj, PyObject * _value )
	{
		if( detail::is_class( _obj ) == true )
		{
			return py_nb_multiply_2( _obj, _value );
		}

		return py_nb_multiply_2( _value, _obj );
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_nb_divide_2( PyObject * _obj, PyObject * _value )
	{
		PyTypeObject * objtype = Py_TYPE( _obj );

		const class_type_scope_ptr & scope = detail::get_class_scope( objtype );

		const number_binary_adapter_interface_ptr & adapter = scope->get_number_div_adapter();

		if( adapter != nullptr )
		{
			try
			{
				PyObject * result = py_nb_method( _obj, _value, scope, adapter );

				return result;
			}
			catch( const pybind_exception & )
			{
			}
		}

		uint32_t type_id = detail::get_object_type_id( _value );

		if( type_id == 0 )
		{
			return nullptr;
		}

		const number_binary_adapter_interface_ptr & adapters = scope->get_number_div_adapters( type_id );

		if( adapters != nullptr )
		{
			try
			{
				PyObject * result = py_nb_method( _obj, _value, scope, adapters );

				return result;
			}
			catch( const pybind_exception & )
			{
			}
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_nb_divide( PyObject * _obj, PyObject * _value )
	{
		if( detail::is_class( _obj ) == true )
		{
			return py_nb_divide_2( _obj, _value );
		}

		return py_nb_divide_2( _value, _obj );
	}
	//////////////////////////////////////////////////////////////////////////
#	if PYBIND_PYTHON_VERSION < 300
	static PyNumberMethods py_as_number = {
		(binaryfunc)py_nb_add,
		(binaryfunc)py_nb_subtract,
		(binaryfunc)py_nb_multiply,
		(binaryfunc)py_nb_divide,
		(binaryfunc)0, /*nb_remainder*/
		(binaryfunc)0, /*nb_divmod*/
		(ternaryfunc)0, /*nb_power*/
		(unaryfunc)0, /*nb_negative*/
		(unaryfunc)0, /*nb_positive*/
		(unaryfunc)0, /*nb_absolute*/
		(inquiry) 0, /*nb_nonzero*/
		(unaryfunc)0, /*nb_invert*/
		(binaryfunc)0, /*nb_lshift*/
		(binaryfunc)0, /*nb_rshift*/
		(binaryfunc)0, /*nb_and*/
		(binaryfunc)0, /*nb_xor*/
		(binaryfunc)0, /*nb_or*/
		(coercion)0, /*nb_coerce*/
		(unaryfunc)0, /*nb_int*/
		(unaryfunc)0, /*nb_long*/
		(unaryfunc)0, /*nb_float*/
		(unaryfunc)0, /*nb_oct*/
		(unaryfunc)0, /*nb_hex*/
		/* Added in release 2.0 */
		(binaryfunc)py_nb_add, /*nb_inplace_add*/
		(binaryfunc)py_nb_subtract, /*nb_inplace_subtract*/
		(binaryfunc)py_nb_multiply, /*nb_inplace_multiply*/
		(binaryfunc)py_nb_divide, /*nb_inplace_divide*/
		(binaryfunc)0, /*nb_inplace_remainder*/
		(ternaryfunc)0, /*nb_inplace_power*/
		(binaryfunc)0, /*nb_inplace_lshift*/
		(binaryfunc)0, /*nb_inplace_rshift*/
		(binaryfunc)0, /*nb_inplace_and*/
		(binaryfunc)0, /*nb_inplace_xor*/
		(binaryfunc)0, /*nb_inplace_or*/

		/* Added in release 2.2 */
		/* The following require the Py_TPFLAGS_HAVE_CLASS flag */
		(binaryfunc)0, /*nb_floor_divide*/
		(binaryfunc)0, /*nb_true_divide*/
		(binaryfunc)0, /*nb_inplace_floor_divide*/
		(binaryfunc) 0, /*nb_inplace_true_divide*/

		/* Added in release 2.5 */
		(unaryfunc) 0, /*nb_index*/
	};
#	else
	static PyNumberMethods py_as_number = {
		/* Number implementations must check *both*
		arguments for proper type and implement the necessary conversions
		in the slot functions themselves. */

		(binaryfunc)py_nb_add,
		(binaryfunc)py_nb_subtract,
		(binaryfunc)py_nb_multiply,
		(binaryfunc)py_nb_divide,
		(binaryfunc)0, /*nb_divmod*/
		(ternaryfunc)0, /*nb_power*/
		(unaryfunc)0, /*nb_negative*/
		(unaryfunc)0, /*nb_positive*/
		(unaryfunc)0, /*nb_absolute*/
		(inquiry)0, /*nb_bool*/
		(unaryfunc)0, /*nb_invert*/
		(binaryfunc)0, /*nb_lshift*/
		(binaryfunc)0, /*nb_rshift*/
		(binaryfunc)0, /*nb_and*/
		(binaryfunc)0, /*nb_xor*/
		(binaryfunc)0, /*nb_or*/
		(unaryfunc)0, /*nb_int*/
		0,
		(unaryfunc)0, /*nb_float*/

		(binaryfunc)py_nb_add, /*nb_inplace_add*/
		(binaryfunc)py_nb_subtract, /*nb_inplace_subtract*/
		(binaryfunc)py_nb_multiply, /*nb_inplace_multiply*/
		(binaryfunc)py_nb_divide, /*nb_inplace_remainder*/
		(ternaryfunc)0, /*nb_inplace_power*/
		(binaryfunc)0, /*nb_inplace_lshift*/
		(binaryfunc)0, /*nb_inplace_rshift*/
		(binaryfunc)0, /*nb_inplace_and*/
		(binaryfunc)0, /*nb_inplace_xor*/
		(binaryfunc)0, /*nb_inplace_or*/

		(binaryfunc)0, /*nb_floor_divide*/
		(binaryfunc)0, /*nb_true_divide*/
		(binaryfunc)0, /*nb_inplace_floor_divide*/
		(binaryfunc)0, /*nb_inplace_true_divide*/

		(unaryfunc)0, /*nb_index*/

		(binaryfunc)0, /*nb_matrix_multiply*/
		(binaryfunc)0, /*nb_inplace_matrix_multiply*/
	};
#	endif
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_number_add( const number_binary_adapter_interface_ptr & _iadapter )
	{
		m_number_add = _iadapter;

		m_pytypeobject->tp_as_number = &py_as_number;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_number_sub( const number_binary_adapter_interface_ptr & _iadapter )
	{
		m_number_sub = _iadapter;

		m_pytypeobject->tp_as_number = &py_as_number;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_number_mul( const number_binary_adapter_interface_ptr & _iadapter )
	{
		m_number_mul = _iadapter;

		m_pytypeobject->tp_as_number = &py_as_number;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_number_div( const number_binary_adapter_interface_ptr & _iadapter )
	{
		m_number_div = _iadapter;

		m_pytypeobject->tp_as_number = &py_as_number;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_number_add( uint32_t _typeId, const number_binary_adapter_interface_ptr & _iadapter )
	{ 
		m_number_adds[_typeId] = _iadapter;

		m_pytypeobject->tp_as_number = &py_as_number;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_number_sub( uint32_t _typeId, const number_binary_adapter_interface_ptr & _iadapter )
	{ 
		m_number_subs[_typeId] = _iadapter;

		m_pytypeobject->tp_as_number = &py_as_number;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_number_mul( uint32_t _typeId, const number_binary_adapter_interface_ptr & _iadapter )
	{ 
		m_number_muls[_typeId] = _iadapter;

		m_pytypeobject->tp_as_number = &py_as_number;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_number_div( uint32_t _typeId, const number_binary_adapter_interface_ptr & _iadapter )
	{ 
		m_number_divs[_typeId] = _iadapter;

		m_pytypeobject->tp_as_number = &py_as_number;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_smart_pointer( const smart_pointer_adapter_interface_ptr & _iadapter )
	{
		m_smart_pointer = _iadapter;
	}
	//////////////////////////////////////////////////////////////////////////
	void * class_type_scope::construct( PyObject * _obj, PyObject * _args )
	{
		if( m_constructor == nullptr )
		{
			return nullptr;
		}

		void * obj = m_constructor->call( _obj, _args );

		return obj;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_construct( const constructor_adapter_interface_ptr & _ctr )
	{
		m_constructor = _ctr;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_base( uint32_t _info, const class_type_scope_ptr & _scope, pybind_metacast _cast )
	{
		if( m_basesCount == PYBIND_BASES_COUNT )
		{
			pybind::throw_exception( "pybind scope %s maximize bases count PYBIND_BASES_COUNT is %d"
				, this->m_name
				, PYBIND_BASES_COUNT
				);

			return;
		}

		Metacast mc;
		mc.info = _info;
		mc.scope = _scope;
		mc.cast = _cast;

		uint32_t newId = m_basesCount++;
		m_bases[newId] = mc;
	}
	//////////////////////////////////////////////////////////////////////////
	void * class_type_scope::metacast( uint32_t _info, void * _impl )
	{
		for( uint32_t i = 0; i != m_basesCount; ++i )
		{
			Metacast & mc = m_bases[i];

			if( mc.info != _info )
			{
				continue;
			}

			void * obj = mc.cast( _impl );

			return obj;
		}

		for( uint32_t i = 0; i != m_basesCount; ++i )
		{
			Metacast & mc = m_bases[i];

			void * down_impl = mc.cast( _impl );

			void * obj = mc.scope->metacast( _info, down_impl );

			if( obj == nullptr )
			{
				continue;
			}

			return obj;
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * class_type_scope::create_class( void * _impl )
	{
		if( m_pod != 0 )
		{
			return nullptr;
		}

		PyObject * py_self = pybind::detail::alloc_class( m_pytypeobject, nullptr, nullptr );

		pybind::detail::wrap( py_self, _impl, false );

		this->addObject( py_self );

		return py_self;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * class_type_scope::create_holder( void * _impl )
	{
		if( m_pod != 0 )
		{
			return nullptr;
		}

		PyObject * py_self = pybind::detail::alloc_class( m_pytypeobject, nullptr, nullptr );

		pybind::detail::wrap( py_self, _impl, true );

		this->addObject( py_self );

		return py_self;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * class_type_scope::create_pod( void ** _impl, size_t _size )
	{
		PyObject * py_self = pybind::detail::alloc_class( m_pytypeobject, nullptr, nullptr );

		pybind::detail::wrap_pod( py_self, _impl, _size );

		this->addObject( py_self );

		return py_self;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::addObject( PyObject * _obj )
	{
		(void)_obj;

		++m_objectCount;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::removeObject( PyObject * _obj )
	{
		(void)_obj;

		--m_objectCount;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t class_type_scope::getObjectCount() const
	{
		return m_objectCount;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::intrusive_ptr_destroy( class_type_scope * _ptr )
	{
		delete _ptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void * class_type_scope::operator new (size_t _size)
	{
		return PYBIND_MALLOC( _size );
	}
		//////////////////////////////////////////////////////////////////////////
		void class_type_scope::operator delete (void * _ptr, size_t _size)
	{
		(void)_size;

		PYBIND_FREE( _ptr, _size );
	}
	//////////////////////////////////////////////////////////////////////////
#	ifdef PYBIND_VISIT_OBJECTS
	void class_type_scope::visit_objects( pybind_visit_objects * _visitor )
	{
		for( TVectorObjects::iterator
			it = m_objects.begin(),
			it_end = m_objects.end();
		it != it_end;
		++it )
		{
			PyObject * obj = *it;

			_visitor->visit_object( obj );
		}
	}
#	endif
}
