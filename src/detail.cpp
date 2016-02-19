#	include "pybind/detail.hpp"

#	include "pybind/class_type_scope.hpp"

#	include "pybind/system.hpp"

#	include "pybind/exception.hpp"
#	include "pybind/extract.hpp"

#	include "pod.hpp"

#	include "config/python.hpp"

namespace pybind
{
	namespace detail
	{
		//////////////////////////////////////////////////////////////////////////
		struct detail_scope
		{
			class_info_desc_t class_info_desc[PYBIND_TYPE_COUNT];
			type_cast_ptr type_cast[PYBIND_TYPE_COUNT];
			class_type_scope_ptr class_type_scopes[PYBIND_TYPE_COUNT];

			uint32_t enumerator;

			PyObject * str_pybind_class_type_scope;
		};
		//////////////////////////////////////////////////////////////////////////
		static detail_scope & get_scope()
		{
			static detail_scope k;

			return k;
		}
		//////////////////////////////////////////////////////////////////////////
		void * unwrap( PyObject * _obj )
		{
			if( _obj == nullptr )
			{
				return nullptr;
			}

			void * impl = get_class_impl( _obj );

			py_base_object * py_base = (py_base_object *)_obj;

			py_base->flag |= PY_OBJECT_UNWRAP;

			return impl;
		}
		//////////////////////////////////////////////////////////////////////////
		bool is_wrap( PyObject * _obj )
		{
			py_base_object * py_base = (py_base_object *)_obj;

			if( py_base->flag & PY_OBJECT_UNWRAP )
			{
				return false;
			}

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		void * meta_cast_scope( void * _self, uint32_t _typeId, uint32_t _name, const class_type_scope_ptr & _scope )
		{
			uint32_t class_name = _scope->get_type_id();

			if( class_name == _typeId )
			{
				return _self;
			}

			void * impl_cast = _scope->metacast( _name, _self );

			return impl_cast;
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t get_next_id()
		{
			detail_scope & k = get_scope();

			uint32_t id = k.enumerator;

			k.enumerator++;

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t find_class_info_desc_name( const char * _name )
		{
			detail_scope & k = get_scope();

			for( uint32_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
			{
				const class_info_desc_t & desc = k.class_info_desc[index];

				if( desc.name == nullptr )
				{
					continue;
				}

				if( strcmp( desc.name, _name ) != 0 )
				{
					continue;
				}

				return index;
			}

			return 0;
		}
		//////////////////////////////////////////////////////////////////////////
		const class_info_desc_t * get_class_info_desc_id( uint32_t _typeId )
		{
			if( _typeId >= PYBIND_TYPE_COUNT )
			{ 
				return nullptr;
			}

			detail_scope & k = get_scope();

			const class_info_desc_t & desc = k.class_info_desc[_typeId];

			if( desc.name == nullptr )
			{
				return nullptr;
			}

			return &desc;
		}
		//////////////////////////////////////////////////////////////////////////
		bool set_class_info_desc( uint32_t _typeId, const char * _info )
		{
			if( _typeId >= PYBIND_TYPE_COUNT )
			{
				return false;
			}

			detail_scope & k = get_scope();

			class_info_desc_t & desc = k.class_info_desc[_typeId];

			if( desc.name != nullptr )
			{
				return false;
			}

			desc.name = _info;

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t get_class_type_id( const std::type_info & _info )
		{
			const char * info_name = _info.name();

			uint32_t id = pybind::detail::find_class_info_desc_name( info_name );

			if( id != 0 )
			{
				return id;
			}

			uint32_t new_id = detail::get_next_id();
			
			if( pybind::detail::set_class_info_desc( new_id, info_name ) == false )
			{
				pybind::throw_exception( "invalid create new type info" );

				return 0;
			}

			return new_id;
		}
		//////////////////////////////////////////////////////////////////////////
		const char * get_class_type_info( uint32_t _id )
		{
			const class_info_desc_t * desc = detail::get_class_info_desc_id( _id );

			if( desc == nullptr )
			{
				pybind::throw_exception( "invalid get class info %d"
					, _id
					);

				return nullptr;
			}

			const char * name = desc->name;

			return name;
		}
		//////////////////////////////////////////////////////////////////////////
		void register_type_info_extract( uint32_t _info, const type_cast_ptr & _cast )
		{
			detail_scope & k = get_scope();

			k.type_cast[_info] = _cast;
		}
		//////////////////////////////////////////////////////////////////////////
		void unregister_type_info_extract( uint32_t _info )
		{
			detail_scope & k = get_scope();

			k.type_cast[_info] = nullptr;
		}
		//////////////////////////////////////////////////////////////////////////
		type_cast * find_type_info_extract( uint32_t _info )
		{
			detail_scope & k = get_scope();

			const type_cast_ptr & cast = k.type_cast[_info];

			type_cast * t = cast.get();

			return t;
		}
		//////////////////////////////////////////////////////////////////////////
		class_type_scope_ptr create_new_type_scope( uint32_t _info, const char * _name, void * _user, const new_adapter_interface_ptr & _pynew, const destroy_adapter_interface_ptr & _pydestructor, uint32_t _pod )
		{
			class_type_scope_ptr scope = new class_type_scope( _name, _info, _user, _pynew, _pydestructor, _pod );

			detail_scope & k = get_scope();

			k.class_type_scopes[_info] = scope;

			return scope;
		}
		//////////////////////////////////////////////////////////////////////////
		bool has_class_type_scope( uint32_t _info )
		{
			detail_scope & k = get_scope();

			const class_type_scope_ptr & scope = k.class_type_scopes[_info];

			return scope != nullptr;
		}
		//////////////////////////////////////////////////////////////////////////
		const class_type_scope_ptr & get_class_type_scope( uint32_t _info )
		{
			detail_scope & k = get_scope();

			const class_type_scope_ptr & scope = k.class_type_scopes[_info];

			return scope;
		}
		//////////////////////////////////////////////////////////////////////////
		void visit_types_scope( visitor_class_type_scope * _getter )
		{
			detail_scope & k = get_scope();

			for( uint32_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
			{
				const class_type_scope_ptr & scope_ptr = k.class_type_scopes[index];

				if( scope_ptr == nullptr )
				{
					continue;
				}

				class_type_scope * scope = scope_ptr.get();

				_getter->visit_scope( scope );
			}
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t get_object_type_id( PyObject * _obj )
		{
			detail_scope & k = get_scope();

			PyTypeObject * objtype = Py_TYPE( _obj );

			PyObject * py_scope = PyObject_GetAttr( (PyObject*)objtype, k.str_pybind_class_type_scope );
			PyErr_Clear();

			if( py_scope == nullptr )
			{
				if( _obj == Py_None )
				{					
					return 0;
				}
#   if PYBIND_PYTHON_VERSION < 300
				else if( PyInt_Check( _obj ) == true )
				{					
					return detail::class_info<int>();
				}
#	endif
				else if( PyLong_Check( _obj ) == true )
				{
					return detail::class_info<int>();
				}
				else if( PyFloat_Check( _obj ) == true )
				{
					return detail::class_info<float>();
				}
#   if PYBIND_PYTHON_VERSION < 300
				else if( PyString_Check( _obj ) == true )
				{
					return detail::class_info<const char *>();
				}
#	endif
				else if( PyUnicode_Check( _obj ) == true )
				{
					return detail::class_info<const wchar_t *>();
				}

				pybind::throw_exception( "obj %s not wrap pybind (scope)"
						, pybind::object_str( (PyObject *)objtype )
						);

				return 0;
			}

			uint32_t id;
			if( pybind::extract_value( py_scope, id, false ) == false )
			{
				Py_DECREF( py_scope );

				pybind::throw_exception( "obj %s incorrect wrap pybind (scope)"
					, pybind::object_str( (PyObject *)objtype )
					);

				return 0;
			}

			Py_DECREF( py_scope );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		const class_type_scope_ptr & get_class_scope( PyTypeObject * _type )
		{
			detail_scope & k = get_scope();

			PyObject * py_scope = PyObject_GetAttr( (PyObject*)_type, k.str_pybind_class_type_scope );
			PyErr_Clear();

			if( py_scope == nullptr )
			{
				pybind::throw_exception( "obj %s not wrap pybind (scope)"
					, pybind::object_str( (PyObject *)_type )
					);

				return class_type_scope_ptr::none();
			}

			uint32_t id;
			if( pybind::extract_value( py_scope, id, false ) == false )
			{
				pybind::throw_exception( "obj %s incorrect wrap pybind (scope)"
					, pybind::object_str( (PyObject *)_type )
					);

				return class_type_scope_ptr::none();
			}

			Py_DECREF( py_scope );

			const class_type_scope_ptr & scope = detail::get_class_type_scope( id );

			if( scope == nullptr )
			{
				pybind::throw_exception( "obj %s incorrect wrap pybind (id %d)"
					, pybind::object_str( (PyObject *)_type )
					, id
					);

				return class_type_scope_ptr::none();
			}

			return scope;
		}
		//////////////////////////////////////////////////////////////////////////
		PyObject * scope_create_holder( const class_type_scope_ptr & _scope, void * _ptr )
		{
			PyObject * py_obj = _scope->create_holder( _ptr );

			return py_obj;
		}
		//////////////////////////////////////////////////////////////////////////
		bool is_class( PyObject * _obj )
		{
			PyTypeObject * objtype = Py_TYPE( _obj );

			bool successful = pybind::is_type_class( (PyObject *)objtype );

			return successful;
		}
		//////////////////////////////////////////////////////////////////////////
		bool is_type_class( PyObject * _type )
		{
			detail_scope & k = get_scope();

			if( PyObject_HasAttr( _type, k.str_pybind_class_type_scope ) == 0 )
			{
				return false;
			}

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		void * check_registred_class( PyObject * _obj, uint32_t _info )
		{
			detail_scope & k = get_scope();

			PyTypeObject * py_type = Py_TYPE( _obj );

			const class_type_scope_ptr & type_scope = k.class_type_scopes[_info];

			if( type_scope->is_instance( py_type ) == false )
			{
				return nullptr;
			}

			void * impl = detail::get_class_impl( _obj );

			return impl;
		}
		//////////////////////////////////////////////////////////////////////////
		void error_invalid_extract( PyObject * _obj, uint32_t _tinfo )
		{
			pybind::check_error();

			const char * typeinfo_name = detail::get_class_type_info( _tinfo );

			const char * obj_repr = pybind::object_repr( _obj );
			const char * obj_repr_type = pybind::object_repr_type( _obj );

			if( obj_repr != nullptr )
			{
				pybind::error_message( "invalid extract from %.256s type %.256s to %.256s"
					, obj_repr
					, obj_repr_type
					, typeinfo_name
					);
			}
			else
			{
				pybind::error_message( "invalid extract from unknown object type %s to %s"
					, _obj->ob_type->tp_name
					, typeinfo_name
					);
			}
		}
		//////////////////////////////////////////////////////////////////////////
		bool instance_of_type( PyObject * _obj, uint32_t _tinfo )
		{
			if( pybind::is_class( _obj ) == false )
			{
				return false;
			}

			if( detail::is_wrap( _obj ) == false )
			{
				pybind::error_message( "instance_of_type: unbind object" );

				return false;
			}

			const class_type_scope_ptr & scope = detail::get_class_scope( _obj->ob_type );
			const class_type_scope_ptr & cur_scope = detail::get_class_type_scope( _tinfo );

			if( scope != cur_scope )
			{
				return false;
			}

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		void * get_class_impl( PyObject * _obj )
		{
			py_base_object * py_base = (py_base_object *)_obj;

			uint32_t flag = py_base->flag;

			if( flag & PY_OBJECT_UNWRAP )
			{
				return nullptr;
			}
			else if( flag & PY_OBJECT_PTR )
			{
				py_ptr_object * py_ptr = (py_ptr_object *)_obj;

				void * impl = py_ptr->impl;

				return impl;
			}
			else if( flag & PY_OBJECT_POD4 )
			{
				py_pod4_object * py_pod = (py_pod4_object *)_obj;

				void * impl = (void *)py_pod->buff;

				return impl;
			}
			else if( flag & PY_OBJECT_POD8 )
			{
				py_pod8_object * py_pod = (py_pod8_object *)_obj;

				void * impl = (void *)py_pod->buff;

				return impl;
			}
			else if( flag & PY_OBJECT_POD16 )
			{
				py_pod16_object * py_pod = (py_pod16_object *)_obj;

				void * impl = (void *)py_pod->buff;

				return impl;
			}
			else if( flag & PY_OBJECT_POD32 )
			{
				py_pod32_object * py_pod = (py_pod32_object *)_obj;

				void * impl = (void *)py_pod->buff;

				return impl;
			}
			else if( flag & PY_OBJECT_POD64 )
			{
				py_pod64_object * py_pod = (py_pod64_object *)_obj;

				void * impl = (void *)py_pod->buff;

				return impl;
			}

			pybind::throw_exception( "obj %s not wrap pybind (impl)"
				, pybind::object_str( _obj )
				);

			return nullptr;
		}
		//////////////////////////////////////////////////////////////////////////
		PyObject * get_str_class_type_scope_()
		{
			detail_scope & k = get_scope();

			return k.str_pybind_class_type_scope;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool initialize_detail()
	{
		detail::detail_scope & k = detail::get_scope();

		k.enumerator = 4;
		k.str_pybind_class_type_scope = nullptr;

		for( uint32_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
		{
			detail::class_info_desc_t & desc = k.class_info_desc[index];

			desc.name = nullptr;
		}

#   if PYBIND_PYTHON_VERSION > 300
		k.str_pybind_class_type_scope = pybind::unicode_from_utf8( "__pybind_class_type_scope" );
#   else
		k.str_pybind_class_type_scope = pybind::string_from_char( "__pybind_class_type_scope" );
#   endif
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void finalize_detail()
	{
		detail::detail_scope & k = detail::get_scope();

		for( uint32_t index = 0; index != PYBIND_TYPE_COUNT; ++index )
		{
			class_type_scope_ptr & scope = k.class_type_scopes[index];

			if( scope != nullptr )
			{
				scope->finalize();
				scope = nullptr;
			}

			k.class_type_scopes[index] = nullptr;

			k.type_cast[index] = nullptr;

			k.class_info_desc[index].name = nullptr;
		}

		if( k.str_pybind_class_type_scope != nullptr )
		{
			pybind::decref( k.str_pybind_class_type_scope );
			k.str_pybind_class_type_scope = nullptr;
		}
	}
}