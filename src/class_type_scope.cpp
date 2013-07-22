#	include "pybind/class_type_scope.hpp"

#	include "pybind/repr_adapter.hpp"
#	include "pybind/hash_adapter.hpp"
#	include "pybind/convert_adapter.hpp"
#	include "pybind/compare_adapter.hpp"
#	include "pybind/constructor.hpp"
#	include "pybind/exception.hpp"

#	include "pybind/system.hpp"
#	include "config/python.hpp"

#	include "method_type.hpp"
#	include "member_type.hpp"

#	include <algorithm>

#   include "static_var.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
    STATIC_DECLARE(PyObject *, s_pybind_object_impl);
    STATIC_DECLARE(PyObject *, s_pybind_class_type_scope);
    STATIC_DECLARE(PyObject *, s_pybind_object_holder);
    //////////////////////////////////////////////////////////////////////////
    STATIC_DECLARE(PyTypeObject, s_pod64_type);
    STATIC_DECLARE(PyTypeObject, s_base_type);
	//////////////////////////////////////////////////////////////////////////
	struct py_pod64_object
	{
		PyObject_HEAD
		char buff[64];
	};
	//////////////////////////////////////////////////////////////////////////
	struct py_base_object
	{
		PyObject_HEAD
	};
	//////////////////////////////////////////////////////////////////////////
	namespace detail
	{
        //////////////////////////////////////////////////////////////////////////
        class_type_scope_ptr g_class_type_scope[PYBIND_TYPE_COUNT];
        //////////////////////////////////////////////////////////////////////////
        void reg_class_type_scope( size_t _info, const class_type_scope_ptr & _scope )
        {
            g_class_type_scope[ _info ] = _scope;
        }
        //////////////////////////////////////////////////////////////////////////
        const class_type_scope_ptr & get_class_type_scope( size_t _info )
        {
            const class_type_scope_ptr & scope = g_class_type_scope[_info];

            return scope;	
        }
        //////////////////////////////////////////////////////////////////////////
        void visit_types_scope( visitor_class_type_scope * _getter )
        {
            for( size_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
            {
                const class_type_scope_ptr & scope_ptr = g_class_type_scope[index];

                if( scope_ptr == nullptr )
                {
                    continue;
                }

                class_type_scope * scope = scope_ptr.get();

                _getter->visit_scope( scope );
            }
        }
		//////////////////////////////////////////////////////////////////////////
		bool is_class( PyObject * _obj )
		{
			if( PyType_IsSubtype( _obj->ob_type, &STATIC_VAR(s_base_type) ) == 1 )
			{
				return true;
			}

			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		void * get_class_impl( PyObject * _obj )
		{
			PyObject * py_self = PyObject_GetAttr( _obj, STATIC_VAR(s_pybind_object_impl) );

			if( py_self == nullptr )
			{				
				pybind::throw_exception();

				return nullptr;
			}

			Py_DecRef( py_self );
			
			if( py_self->ob_type == &STATIC_VAR(s_pod64_type) )
			{
				py_pod64_object * py_pod64 = (py_pod64_object *)py_self;

				void * buff = py_pod64->buff;

				return buff;
			}
            
            if( void_ptr_check( py_self ) == false )
            {
                pybind::throw_exception();

                return nullptr;
            }

            void * impl = void_ptr_get( py_self );				

			return impl;
		}
		//////////////////////////////////////////////////////////////////////////
		const class_type_scope_ptr & get_class_scope( PyTypeObject * _type )
		{
			PyObject * py_scope = PyObject_GetAttr( (PyObject*)_type, STATIC_VAR(s_pybind_class_type_scope) );

			if( py_scope == nullptr )
			{
				pybind::throw_exception();
				
                const class_type_scope_ptr & scope_null = g_class_type_scope[0];

                return scope_null;
			}

            size_t id;
            if( pybind::extract_value( py_scope, id ) == false )
            {
                pybind::throw_exception();

                const class_type_scope_ptr & scope_null = g_class_type_scope[0];

                return scope_null;
            }
            
            Py_DecRef( py_scope );

            class_type_scope_ptr * scopes = g_class_type_scope;

            const class_type_scope_ptr & scope = scopes[id];

            if( scope == nullptr )
            {
                pybind::throw_exception();

                const class_type_scope_ptr & scope_null = g_class_type_scope[0];

                return scope_null;
            }
            
			return scope;
		}
		//////////////////////////////////////////////////////////////////////////
		void * unwrap( PyObject * _obj )
		{
			void * impl = pybind::detail::get_class_impl( _obj );

            PyObject * py_impl = void_ptr_new( NULL );

            if( PyObject_SetAttr( _obj, STATIC_VAR(s_pybind_object_impl), py_impl ) == -1 )
            {
                pybind::throw_exception();

                return NULL;
            }

            Py_DecRef( STATIC_VAR(s_pybind_object_impl) );
            Py_DecRef( py_impl );

			return impl;
		}
		//////////////////////////////////////////////////////////////////////////
		bool is_wrap( PyObject * _obj )
		{
			void * impl = pybind::detail::get_class_impl( _obj );
			
			return impl != NULL;
		}
		//////////////////////////////////////////////////////////////////////////
		void wrap( PyObject * _obj, void * _impl, bool _holder )
		{
			PyObject * py_impl = void_ptr_new( _impl );
            
            Py_IncRef( STATIC_VAR(s_pybind_object_impl) );
			if( PyObject_SetAttr( _obj, STATIC_VAR(s_pybind_object_impl), py_impl ) == -1 )
            {
                pybind::throw_exception();

                return;
            }

			Py_DecRef( py_impl );

            PyObject * py_holder = get_bool( _holder );

            Py_IncRef( STATIC_VAR(s_pybind_object_holder) );			
			if( PyObject_SetAttr( _obj, STATIC_VAR(s_pybind_object_holder), py_holder ) == -1 )
            {
                pybind::throw_exception();

                return;
            }
            //Py_DecRef( py_holder );
		}
		//////////////////////////////////////////////////////////////////////////
		void wrap_pod64( PyObject * _obj, void ** _impl )
		{
			PyObject * py_impl = PyType_GenericAlloc( &STATIC_VAR(s_pod64_type), 0 );

            Py_IncRef( STATIC_VAR(s_pybind_object_impl) );
			if( PyObject_SetAttr( _obj, STATIC_VAR(s_pybind_object_impl), py_impl ) == -1 )
            {
                pybind::throw_exception();

                return;
            }

			Py_DecRef( py_impl );

			py_pod64_object * py_pod64 = (py_pod64_object *)py_impl;

			*_impl = (void *)py_pod64->buff;

            Py_IncRef( STATIC_VAR(s_pybind_object_holder) );
			if( PyObject_SetAttr( _obj, STATIC_VAR(s_pybind_object_holder), Py_False ) == -1 )
            {
                pybind::throw_exception();

                return;
            }
//            Py_DecRef( Py_False );
		}
		//////////////////////////////////////////////////////////////////////////
		bool is_holder( PyObject * _obj )
		{
			PyObject * py_holder = PyObject_GetAttr( _obj, STATIC_VAR(s_pybind_object_holder) );

			if( py_holder == 0 )
			{
				pybind::throw_exception();
				return false;
			}
			
			if( bool_check( py_holder ) == false )
			{
				Py_DecRef( py_holder );
				pybind::throw_exception();
				return false;
			}

			bool result = py_holder == Py_True;

			Py_DecRef( py_holder );

			return result;
		}
		//////////////////////////////////////////////////////////////////////////
		void * check_registred_class( PyObject * _obj, size_t _info )
		{
			PyObject * py_type = PyObject_Type( _obj );

			const class_type_scope_ptr & type_scope = detail::get_class_type_scope( _info );

			if( PyType_IsSubtype( (PyTypeObject *)py_type, type_scope->m_pytypeobject ) )
			{
				return detail::get_class_impl( _obj );
			}

			return 0;
		}
        //////////////////////////////////////////////////////////////////////////
        void * meta_cast_scope( void * _self, size_t _scope_name, size_t _name, const class_type_scope_ptr & scope )
        {
            size_t class_name = scope->get_type();

            if( class_name == _scope_name )
            {
                return _self;
            }	

            void * impl_cast = scope->metacast( _name, _self );

            return impl_cast;
        }
		//////////////////////////////////////////////////////////////////////////
		PyObject * alloc_class( PyTypeObject * _type, PyObject * _args, PyObject * _kwds )
		{
            (void)_args;
            (void)_kwds;

			PyObject * py_self = PyBaseObject_Type.tp_alloc(_type, 0);

			if( py_self == 0 )
			{
				if (PyErr_Occurred())
				{
					PyErr_Print();
				}
				
				return 0;
			}			

			return py_self;
		}
		//////////////////////////////////////////////////////////////////////////
		PyObject * alloc_pod( PyTypeObject * _type, PyObject * _args, PyObject * _kwds, void ** _buff )
		{
            (void)_args;
            (void)_kwds;

			PyObject * py_self = PyBaseObject_Type.tp_alloc(_type, 0);

			if( py_self == 0 )
			{
				if (PyErr_Occurred())
				{
					PyErr_Print();
				}

				return 0;
			}

			pybind::detail::wrap_pod64( py_self, _buff );

			return py_self;
		}
    }
    //////////////////////////////////////////////////////////////////////////
    static PyObject * py_callfunc( PyObject * _obj, PyObject * _args, PyObject * _kwds )
    {
        void * impl = pybind::detail::get_class_impl( _obj );

        if( impl == 0 )
        {
            pybind::error_message( "pybind: callfunc unbind object" );
            return 0;
        }

        const class_type_scope_ptr & scope = pybind::detail::get_class_scope( _obj->ob_type );

        PyObject * repr = scope->m_call->call( impl, scope, _args, _kwds );

        return repr;
    }
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_reprfunc( PyObject * _obj )
	{
		void * impl = pybind::detail::get_class_impl( _obj );

		if( impl == 0 )
		{
			pybind::error_message( "pybind: reprfunc unbind object" );
			return 0;
		}

		const class_type_scope_ptr & scope = pybind::detail::get_class_scope( _obj->ob_type );

        PyObject * repr = scope->m_repr->repr( _obj, impl, scope );

		return repr;
	}
    //////////////////////////////////////////////////////////////////////////
    static int py_hash( PyObject * _obj )
    {
        void * impl = pybind::detail::get_class_impl( _obj );

        if( impl == 0 )
        {
            pybind::error_message( "pybind: reprfunc unbind object" );
            return 0;
        }

        const class_type_scope_ptr & scope = pybind::detail::get_class_scope( _obj->ob_type );

        size_t hash = scope->m_hash->hash( _obj, impl, scope );

        return hash;
    }
    //////////////////////////////////////////////////////////////////////////
    static PyObject * py_richcompare( PyObject * _obj, PyObject * _compare, int _op )
    {
        void * impl = pybind::detail::get_class_impl( _obj );

        if( impl == 0 )
        {
            pybind::error_message( "pybind: compare unbind object" );

            Py_IncRef(Py_NotImplemented);

            return Py_NotImplemented;
        }

        const class_type_scope_ptr & scope = pybind::detail::get_class_scope( _obj->ob_type );

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
                error_message("invalid compare op '%d'\n"
                    , _op
                    );
            }break;
        }

        bool test_result;

        try
        {
            if( scope->m_compare->compare( _obj, impl, scope, _compare, pybind_op, test_result ) == false )
            {
                Py_IncRef(Py_NotImplemented);

                return Py_NotImplemented;
            }
        }
        catch( const pybind_exception & )
        {
            error_message("invalid compare\n"
                );

            return NULL;
        }

        PyObject * py_result;

        py_result = (test_result)? Py_True : Py_False;

        Py_IncRef( py_result );

        return py_result;
    }
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_getattro( PyObject * _obj, PyObject * _key )
	{
		void * impl = pybind::detail::get_class_impl( _obj );

		if( impl == 0 )
		{
			error_message( "pybind: getattro unbind object" );
			return 0;
		}

		const class_type_scope_ptr & scope = pybind::detail::get_class_scope( _obj->ob_type );

		PyObject * attr = PyTuple_Pack( 1, _key );
		PyObject * res = scope->m_getattro->call( impl, scope, attr, 0 );
		Py_DecRef( attr );

		return res;
	}
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_subscript( PyObject * _obj, PyObject * _key )
	{
		void * impl = pybind::detail::get_class_impl( _obj );

		if( impl == 0 )
		{
			error_message( "pybind: subscript unbind object" );
			return 0;
		}

		const class_type_scope_ptr & scope = pybind::detail::get_class_scope( _obj->ob_type );

		PyObject * attr = PyTuple_Pack( 1, _key );
		PyObject * res = scope->m_mapping->call( impl, scope, attr, 0 );
		Py_DecRef( attr );

		return res;
	}
	//////////////////////////////////////////////////////////////////////////
	static PyMappingMethods py_as_mapping = {
		(lenfunc)0,		/* mp_length */
		(binaryfunc)py_subscript,		/* mp_subscript */
		(objobjargproc)0,	/* mp_ass_subscript */
	};
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_item( PyObject * _obj, Py_ssize_t _index )
	{
		void * impl = pybind::detail::get_class_impl( _obj );

		if( impl == 0 )
		{
			error_message( "pybind: subscript unbind object" );
			return 0;
		}

		const class_type_scope_ptr & scope = pybind::detail::get_class_scope( _obj->ob_type );

		PyObject * attr = Py_BuildValue( "(i)", _index );
		PyObject * res = scope->m_sequence->call( impl, scope, attr, 0 );
		Py_DecRef( attr );

		return res;
	}

	//////////////////////////////////////////////////////////////////////////
	static PySequenceMethods py_as_sequence = {
		(lenfunc)0,                       /* sq_length */
		(binaryfunc)0,                    /* sq_concat */
		(ssizeargfunc)0,                  /* sq_repeat */
		(ssizeargfunc)py_item,                    /* sq_item */
		(ssizessizeargfunc)0,              /* sq_slice */
		(ssizeobjargproc)0,             /* sq_ass_item */
		(ssizessizeobjargproc)0,       /* sq_ass_slice */
		(objobjproc)0,                  /* sq_contains */
		(binaryfunc)0,            /* sq_inplace_concat */
		(ssizeargfunc)0,          /* sq_inplace_repeat */
	};
	//////////////////////////////////////////////////////////////////////////
	static PyObject * py_new_class( PyTypeObject * _type, PyObject * _args, PyObject * _kwds )
	{
		const class_type_scope_ptr & scope = pybind::detail::get_class_scope(_type);

		PyObject * py_self = detail::alloc_class( _type, _args, _kwds );

        void * impl = (*scope->m_pynew)( scope, py_self, _args, _kwds );

        if( impl == NULL )
        {
            pybind::decref( py_self );

            return NULL;
        }

        pybind::detail::wrap( py_self, impl, false );

#	ifdef PYBIND_VISIT_OBJECTS
        scope->addObject( py_self );
#	endif

		return py_self;
	}
    //////////////////////////////////////////////////////////////////////////
    static PyObject * py_new_pod( PyTypeObject * _type, PyObject * _args, PyObject * _kwds )
    {
        const class_type_scope_ptr & scope = pybind::detail::get_class_scope(_type);

        void * buff;
        PyObject * py_self = detail::alloc_pod( _type, _args, _kwds, &buff );

        void * impl = (*scope->m_pynew)( scope, py_self, _args, _kwds );

        (void)impl;

#	ifdef PYBIND_VISIT_OBJECTS
        {
            scope->addObject( py_self );
        }
#	endif
        
        return py_self;
    }
	//////////////////////////////////////////////////////////////////////////
	static void py_del( PyObject * _obj )
	{
        const class_type_scope_ptr & scope = pybind::detail::get_class_scope(_obj->ob_type);

        scope->removeObject( _obj );

        bool holder = pybind::detail::is_holder(_obj);

		if( holder == false )
		{
			void * impl = pybind::detail::get_class_impl(_obj);
			
			(scope->m_pydestructor)( scope, impl );            
		}
	}
	//////////////////////////////////////////////////////////////////////////
	class_type_scope::class_type_scope( const char * _name, size_t _type_name, PyObject * _module, void * _user, pybind_new _pynew, pybind_destructor _pydestructor, bool _pod )
		: m_name(_name)
		, m_type(_type_name)
		, m_module(_module)
        , m_user(_user)
        , m_objectCount(0)
		, m_pynew(_pynew)
		, m_pydestructor(_pydestructor)
        , m_pod(_pod)
		, m_pytypeobject(nullptr)        
	{
        Py_IncRef( STATIC_VAR(s_pybind_class_type_scope) );
        Py_IncRef( STATIC_VAR(s_pybind_class_type_scope) ); //Double - memory leak! yeaaa
	}
	//////////////////////////////////////////////////////////////////////////
	class_type_scope::~class_type_scope()
	{
        this->finalize();
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::initialize()
	{
		if( m_module == 0 )
		{
			m_module = get_currentmodule();

            if( m_module == 0 )
            {
                pybind::throw_exception();
            }
		}

#   if PYBIND_PYTHON_VERSION < 300
		PyObject * py_name = PyString_InternFromString( m_name );
#	else
		PyObject * py_name = PyUnicode_InternFromString( m_name );
#	endif

		PyObject * py_bases;

		if( m_bases.empty() == false )
		{
            size_t bases_size = 0;
            for( TBases::iterator
                it = m_bases.begin(),
                it_end = m_bases.end();
            it != it_end;
            ++it)
            {
                if( it->setup == false )
                {
                    continue;
                }

                ++bases_size;
            }

			py_bases = PyTuple_New( bases_size );
			int index = 0;

			for( TBases::iterator
				it = m_bases.begin(),
				it_end = m_bases.end();
			it != it_end;
			++it)
			{
                Metacast & mc = *it; 

                if( mc.setup == false )
                {
                    continue;
                }
                
				PyTypeObject * py_base = mc.scope->m_pytypeobject;

				Py_IncRef( (PyObject *)py_base );
				PyTuple_SetItem( py_bases, index++, (PyObject*)py_base );
			}
		}
		else
		{
			py_bases = PyTuple_New( 1 );

            PyTypeObject * base_type = &STATIC_VAR(s_base_type);
			Py_IncRef( (PyObject *)base_type );
			PyTuple_SetItem( py_bases, 0, (PyObject *)base_type );
		}

		PyObject * py_dict = PyDict_New();
		
        PyObject * py_pybind_class_type = pybind::ptr( m_type );
                
		PyDict_SetItem( py_dict, STATIC_VAR(s_pybind_class_type_scope), py_pybind_class_type );
		Py_DecRef( py_pybind_class_type );


		PyObject * py_args = PyTuple_Pack( 3, py_name, py_bases, py_dict );
		Py_DecRef( py_name );
		Py_DecRef( py_bases );
		Py_DecRef( py_dict );
		
		m_pytypeobject = (PyTypeObject *)PyType_Type.tp_call( (PyObject*)&PyType_Type, py_args, 0 );
		Py_DecRef( py_args );

        if( m_pod == true )
        {
            m_pytypeobject->tp_new = py_new_pod;
            m_pytypeobject->tp_del = py_del;
        }
        else
        {
		    m_pytypeobject->tp_new = py_new_class;
		    m_pytypeobject->tp_del = py_del;
        }

		PyType_Modified( m_pytypeobject );

		//Py_IncRef( m_pytypeobject );	
        
        Py_IncRef( (PyObject*)m_pytypeobject );
		PyModule_AddObject( m_module, m_pytypeobject->tp_name, (PyObject*)m_pytypeobject );
	}
    //////////////////////////////////////////////////////////////////////////
    void class_type_scope::finalize()
    {
        PyObject * dummy = m_pytypeobject->tp_dict;
        m_pytypeobject->tp_dict = PyDict_New();
        Py_DecRef( dummy );

        Py_DECREF( (PyObject*)m_pytypeobject );

        m_pyconstructor = nullptr;

        m_convert = nullptr;
        m_call = nullptr;
        m_repr = nullptr;
        m_hash = nullptr;
        m_getattro = nullptr;
        m_mapping = nullptr;
        m_sequence = nullptr;
        m_compare = nullptr;

        Py_DecRef( STATIC_VAR(s_pybind_class_type_scope) );

        m_bases.clear();
    }
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::type_initialize( PyTypeObject * _type )
	{
		_type->tp_del = py_del;
	}
	//////////////////////////////////////////////////////////////////////////
	const char * class_type_scope::get_name() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t class_type_scope::get_type() const
	{
		return m_type;
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
	void class_type_scope::set_module( PyObject * _module )
	{
		m_module = _module;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace detail
	{
		class FCharCmp
		{
		public:
			FCharCmp( const char * _name )
				: m_name(_name)
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
	void class_type_scope::add_method( const char * _name, const method_adapter_interface_ptr & _ifunc )
	{
		PyObject * py_type_method = method_type_scope::instance( _name, _ifunc, m_pytypeobject );

		if( PyDict_SetItemString( m_pytypeobject->tp_dict, _name, py_type_method ) == -1 )
		{
			pybind::throw_exception();

            return;
		}

		Py_DecRef( py_type_method );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_member( const char * _name, const member_adapter_interface_ptr & _imember )
	{
		PyObject * py_member = member_type_scope::instance( _name, _imember );

		if( PyDict_SetItemString( m_pytypeobject->tp_dict, _name, py_member ) == -1 )
		{
			pybind::throw_exception();

            return;
		}

		Py_DecRef( py_member );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_convert( const convert_adapter_interface_ptr & _iconvert )
	{
		m_convert = _iconvert;
	}
	//////////////////////////////////////////////////////////////////////////
	const convert_adapter_interface_ptr & class_type_scope::get_convert()
	{
		return m_convert;
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

		//PyType_Modified( m_pytypeobject );
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
	void class_type_scope::set_getattro( const method_adapter_interface_ptr & _igetattro )
	{
		m_getattro = _igetattro;

		m_pytypeobject->tp_getattro = &py_getattro;

		//PyType_Modified( m_pytypeobject );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_mapping( const method_adapter_interface_ptr & _imapping )
	{
		m_mapping = _imapping;

		m_pytypeobject->tp_as_mapping = &py_as_mapping;

		//PyType_Modified( m_pytypeobject );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::set_sequence( const method_adapter_interface_ptr & _isequence )
	{
		m_sequence = _isequence;
		
		m_pytypeobject->tp_as_sequence = &py_as_sequence;
	}
	//////////////////////////////////////////////////////////////////////////
	void * class_type_scope::construct( PyObject * _obj, PyObject * _args )
	{
        if( m_pyconstructor == NULL )
        {
            return NULL;
        }

		return m_pyconstructor->call( _obj, _args, m_name );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::def_init( const constructor_adapter_interface_ptr & _ctr )
	{
		m_pyconstructor = _ctr;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::add_base( size_t _name, const class_type_scope_ptr & _base, pybind_metacast _cast )
	{
        size_t last_size = m_bases.size();
        if( last_size <= _name )
        {            
            m_bases.resize( _name + 1 );
            
            for( size_t i = last_size; i != _name; ++i )
            {
                m_bases[i].setup = false;
            }
        }

        Metacast mc;
        mc.scope = _base;
        mc.cast = _cast;
        mc.setup = true;

		m_bases[_name] = mc;
	}
	//////////////////////////////////////////////////////////////////////////
	void * class_type_scope::metacast( size_t _name, void * _impl )
	{
        if( _name < m_bases.size() )
        {
		    Metacast & mc = m_bases[_name];

            if( mc.setup == true )
            {
                return mc.cast( _impl );
            }
        }

		for( TBases::iterator 
			it = m_bases.begin(),
			it_end = m_bases.end();
		it != it_end;
		++it)
		{
            Metacast & mc = *it;

            if( mc.setup == false )
            {
                continue;
            }            

			void * down_impl = mc.cast(_impl);
			if( void * result = mc.scope->metacast( _name, down_impl ) )
			{
				return result;
			}
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * class_type_scope::create_holder( void * _impl )
	{
		PyObject * py_args = PyTuple_New(0);
		PyObject * py_self = pybind::detail::alloc_class( m_pytypeobject, py_args, 0 );
        Py_DecRef( py_args );

        pybind::detail::wrap( py_self, _impl, true );

	    this->addObject( py_self );

		return py_self;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * class_type_scope::create_impl( void * _impl )
	{
		PyObject * py_args = PyTuple_New(0);
		PyObject * py_self = pybind::detail::alloc_class( m_pytypeobject, py_args, 0 );
        Py_DecRef( py_args );

        pybind::detail::wrap( py_self, _impl, false );
        
        this->addObject( py_self );

		return py_self;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * class_type_scope::create_pod( void ** _impl )
	{
		PyObject * py_args = PyTuple_New(0);

		PyObject * py_self = pybind::detail::alloc_pod( m_pytypeobject, py_args, 0, _impl );
		Py_DecRef( py_args );

	    this->addObject( py_self );

		return py_self;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::addObject( PyObject * _obj )
	{
        (void)_obj;

        ++m_objectCount;

#	ifdef PYBIND_VISIT_OBJECTS
		m_objects.push_back( _obj );
#   endif
	}
	//////////////////////////////////////////////////////////////////////////
	void class_type_scope::removeObject( PyObject * _obj )
	{
        (void)_obj;

        --m_objectCount;

#	ifdef PYBIND_VISIT_OBJECTS
		m_objects.remove( _obj );
#   endif
	}
    //////////////////////////////////////////////////////////////////////////
    size_t class_type_scope::getObjectCount() const
    {
        return m_objectCount;
    }
    //////////////////////////////////////////////////////////////////////////
    void class_type_scope::destroy()
    {
        delete this;
    }
    //////////////////////////////////////////////////////////////////////////
#	ifdef PYBIND_VISIT_OBJECTS
    void class_type_scope::visit_objects( pybind_visit_objects * _visitor )
	{
		for( TListObjects::iterator
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
	//////////////////////////////////////////////////////////////////////////
	static void py_dealloc( PyObject * _obj )
	{
		_obj->ob_type->tp_free( _obj );
	}
	//////////////////////////////////////////////////////////////////////////
	bool initialize_classes()
	{
        for( size_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
        {
            detail::g_class_type_scope[index] = nullptr;
        }

#   if PYBIND_PYTHON_VERSION > 300
        STATIC_VAR(s_pybind_object_impl) = pybind::unicode_from_utf8( "__pybind_object_impl" );
        STATIC_VAR(s_pybind_class_type_scope) = pybind::unicode_from_utf8( "__pybind_class_type_scope" );
        STATIC_VAR(s_pybind_object_holder) = pybind::unicode_from_utf8( "__pybind_object_holder" );
#   else
        STATIC_VAR(s_pybind_object_impl) = pybind::string_from_char( "__pybind_object_impl" );
        STATIC_VAR(s_pybind_class_type_scope) = pybind::string_from_char( "__pybind_class_type_scope" );
        STATIC_VAR(s_pybind_object_holder) = pybind::string_from_char( "__pybind_object_holder" );
#   endif

		STATIC_VAR(s_pod64_type).tp_name = "pod64_type";
		STATIC_VAR(s_pod64_type).tp_basicsize = sizeof(py_pod64_object);
		STATIC_VAR(s_pod64_type).tp_dealloc = &py_dealloc;
		STATIC_VAR(s_pod64_type).tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;

		if( PyType_Ready( &STATIC_VAR(s_pod64_type) ) < 0 )
		{
			printf("invalid embedding class '%s' \n", STATIC_VAR(s_pod64_type).tp_name );

            return false;
		}		

		STATIC_VAR(s_base_type).tp_name = "pybind_base_type";
		STATIC_VAR(s_base_type).tp_basicsize = sizeof(py_base_object);
		STATIC_VAR(s_base_type).tp_dealloc = &py_dealloc;
		STATIC_VAR(s_base_type).tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;

		if( PyType_Ready( &STATIC_VAR(s_base_type) ) < 0 )
		{
			printf("invalid embedding class '%s' \n", STATIC_VAR(s_base_type).tp_name );

            return false;
		}	

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void finalize_classes()
	{
		Py_DecRef(STATIC_VAR(s_pybind_object_impl));
		Py_DecRef(STATIC_VAR(s_pybind_class_type_scope));
		Py_DecRef(STATIC_VAR(s_pybind_object_holder));

        for( size_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
        {
            detail::g_class_type_scope[index] = nullptr;
        }
	}
}
