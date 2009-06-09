#	include "pybind/class_core.hpp"

#	include "pybind/class_scope.hpp"
#	include "pybind/class_type.hpp"

#	include "config/python.hpp"

#	include <list>

namespace pybind
{
	namespace detail
	{
		//////////////////////////////////////////////////////////////////////////
		void * get_class( PyObject * _obj )
		{
			py_class_type * self = (py_class_type *)_obj;
			return self->impl;
		}
		//////////////////////////////////////////////////////////////////////////
		class_type_scope * get_class_scope( PyObject * _obj )
		{
			py_class_type * self = (py_class_type *)_obj;
			return self->scope;
		}
		//////////////////////////////////////////////////////////////////////////
		void * check_registred_class( PyObject * _obj, const std::type_info & _info )
		{
			PyObject * py_type = PyObject_Type( _obj );

			class_type_scope * type_scope = class_scope::get_class_scope( _info );

			if( PyType_IsSubtype( (PyTypeObject *)py_type, type_scope->m_type ) )
			{
				return get_class( _obj );
			}

			if( PyType_IsSubtype( (PyTypeObject *)py_type, type_scope->m_type_holder ) )
			{
				return get_class( _obj );
			}

			return 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	typedef std::list<class_type_scope *> TListClassType;
	static TListClassType s_listClassType;
	//////////////////////////////////////////////////////////////////////////
	void class_core::finialize()
	{
		for( TListClassType::iterator
			it = s_listClassType.begin(),
			it_end = s_listClassType.end();
		it != it_end;
		++it )
		{
			delete *it;
		}

		s_listClassType.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	class_type_scope * class_core::create_new_type_scope( 
		const std::type_info & _info,
		const char * _name, 
		PyObject * _module,
		pybind_newfunc _pynew,
		pybind_destructor _pydestructor)
	{
		class_type_scope * t_scope = new class_type_scope();

		const char * type_name = _info.name();
		t_scope->setup( _name, type_name, _module, _pynew, _pydestructor );

		class_scope::reg_class_scope( _info, t_scope );

		s_listClassType.push_back( t_scope );

		return t_scope;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * class_core::create_holder( const std::type_info & _info, void * _impl )
	{
		class_type_scope * t_scope = class_scope::get_class_scope( _info );

		return t_scope->create_holder( _impl );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * class_core::create_impl( const std::type_info & _info, void * _impl )
	{
		class_type_scope * t_scope = class_scope::get_class_scope( _info );

		return t_scope->create_impl( _impl );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_core::wrap_holder( PyObject * _obj, void * _impl )
	{
		py_class_type * self = (py_class_type*)_obj;
		self->scope->update_method_self( self, _impl );
		self->impl = _impl;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_core::def_init( class_type_scope * _scope, constructor * _ctr )
	{
		_scope->def_init( _ctr );
	}
	//////////////////////////////////////////////////////////////////////////
	void * class_core::construct( class_type_scope * _scope, PyObject * _args )
	{
		return _scope->construct( _args );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_core::set_module( class_type_scope * _scope, PyObject * _module )
	{
		_scope->set_module( _module );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_core::def_method( const char * _name, method_proxy_interface * _ifunc, int _arity, const std::type_info & _info )
	{
		class_type_scope * scope = class_scope::get_class_scope( _info );

		scope->add_method( _name, _ifunc, _arity );
	}
	//////////////////////////////////////////////////////////////////////////
	void class_core::add_method_from_scope( class_type_scope * _scope, class_type_scope * _basescope )
	{
		if( _scope )
		{
			_scope->add_method_from_scope( _basescope );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void class_core::add_base_to_scope( class_type_scope * _scope, const char * _name, class_type_scope * _base, pybind_metacast cast )
	{
		_scope->add_base( _name, _base, cast );
	}
	//////////////////////////////////////////////////////////////////////////
	void * class_core::meta_cast( void * _impl, class_type_scope * _scope, const char * _name )
	{
		return _scope->metacast( _name, _impl );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * class_core::new_impl( PyTypeObject * _type, PyObject * _args, void * _impl, const std::type_info & _tinfo )
	{
		py_class_type *self = 
			(py_class_type *)_type->tp_alloc( _type, 0 );

		class_type_scope * scope = class_scope::get_class_scope( _tinfo );

		if( self != NULL )
		{
			self->impl = _impl;
			self->scope = scope;
			scope->setup_method( self );
		}

		return (PyObject *)self;
	}
	//////////////////////////////////////////////////////////////////////////
	void * class_core::dealloc_impl( PyObject * _obj )
	{
		py_class_type * self = (py_class_type *)(_obj);
		void * impl = self->impl;

		Py_DECREF( self->dict );

		_obj->ob_type->tp_free( (PyObject*)self );

		return impl;		
	}
}
