#	pragma once

#	include "pybind/helper.hpp"
#	include "pybind/detail.hpp"

namespace pybind
{
	class object
	{
	public:
		static const pybind::object & get_invalid()
		{
			static pybind::object s_obj;

			return s_obj;
		}

	public:
		object()
			: m_obj( nullptr )
		{
		}

		object & operator = (const object & _obj)
		{
			pybind::decref( m_obj );
			m_obj = _obj.ptr();
			pybind::incref( m_obj );

			return *this;
		}

		object( const object & _obj )
			: m_obj( _obj.ptr() )
		{
			pybind::incref( m_obj );
		}

		explicit object( PyObject * _obj )
			: m_obj( _obj )
		{
			pybind::incref( _obj );
		}

		~object()
		{
			pybind::decref( m_obj );
		}

	public:
		PyObject * ptr() const
		{
			return m_obj;
		}

		void reset()
		{
			pybind::decref( m_obj );
			m_obj = nullptr;
		}

		uint32_t get_ref() const
		{
			uint32_t ref = pybind::refcount( m_obj );

			return ref;
		}

	public:
		bool is_invalid() const
		{
			return m_obj == nullptr;
		}

		bool is_valid() const
		{
			return m_obj != nullptr;
		}

		bool is_wrap() const
		{
			return pybind::detail::is_wrap( m_obj );
		}

		bool is_class() const
		{
			return pybind::is_class( m_obj );
		}

		bool is_type_class() const
		{
			return pybind::is_type_class( m_obj );
		}

		bool is_none() const
		{
			return pybind::is_none( m_obj );
		}

		bool is_bool() const
		{
			return pybind::bool_check( m_obj );
		}

		bool is_callable() const
		{
			return pybind::is_callable( m_obj );
		}

	public:
		bool has_attr( const char * _name ) const
		{
			return pybind::has_attr( m_obj, _name );
		}

		pybind::object get_attr( const char * _name ) const
		{
			PyObject * py_attr = pybind::get_attr( m_obj, _name );

			return pybind::object( py_attr );
		}

	public:
		void unwrap() const
		{
			pybind::unwrap( m_obj );
		}

	public:
		const char * repr() const
		{
			return pybind::object_repr( m_obj );
		}

	public:
		detail::extract_operator_t extract()
		{
			return detail::extract_operator_t( m_obj );
		}

	public:
		detail::extract_operator_t operator () () const
		{
			return pybind::call_t( m_obj );
		}

		template<class T0>
		detail::extract_operator_t operator () ( const T0 & _t0 ) const
		{
			return pybind::call_t( m_obj, _t0 );
		}

		template<class T0, class T1>
		detail::extract_operator_t operator () ( const T0 & _t0, const T1 & _t1 ) const
		{
			return pybind::call_t( m_obj, _t0, _t1 );
		}

		template<class T0, class T1, class T2>
		detail::extract_operator_t operator () ( const T0 & _t0, const T1 & _t1, const T2 & _t2 ) const
		{
			return pybind::call_t( m_obj, _t0, _t1, _t2 );
		}

		template<class T0, class T1, class T2, class T3>
		detail::extract_operator_t operator () ( const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3 ) const
		{
			return pybind::call_t( m_obj, _t0, _t1, _t2, _t3 );
		}

		template<class T0, class T1, class T2, class T3, class T4>
		detail::extract_operator_t operator () ( const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4 ) const
		{
			return pybind::call_t( m_obj, _t0, _t1, _t2, _t3, _t4 );
		}

		template<class T0, class T1, class T2, class T3, class T4, class T5>
		detail::extract_operator_t operator () ( const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4, const T5 & _t5 ) const
		{
			return pybind::call_t( m_obj, _t0, _t1, _t2, _t3, _t4, _t5 );
		}

		template<class T0, class T1, class T2, class T3, class T4, class T5, class T6>
		detail::extract_operator_t operator () ( const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4, const T5 & _t5, const T6 & _t6 ) const
		{
			return pybind::call_t( m_obj, _t0, _t1, _t2, _t3, _t4, _t5, _t6 );
		}

		template<class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
		detail::extract_operator_t operator () ( const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4, const T5 & _t5, const T6 & _t6, const T7 & _t7 ) const
		{
			return pybind::call_t( m_obj, _t0, _t1, _t2, _t3, _t4, _t5, _t6, _t7 );
		}

	protected:
		PyObject * m_obj;
	};
	//////////////////////////////////////////////////////////////////////////
	inline pybind::object make_none_t()
	{
		PyObject * py_none = pybind::ret_none();

		return pybind::object( py_none );
	}
	//////////////////////////////////////////////////////////////////////////
	inline pybind::object make_true_t()
	{
		PyObject * py_true = pybind::ret_true();

		return pybind::object( py_true );
	}
	//////////////////////////////////////////////////////////////////////////
	inline pybind::object make_false_t()
	{
		PyObject * py_false = pybind::ret_false();

		return pybind::object( py_false );
	}
	//////////////////////////////////////////////////////////////////////////
	inline pybind::object make_invalid_object_t()
	{
		return pybind::object( nullptr );
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool extract_value( PyObject * _obj, pybind::object & _value )
	{
		if( _obj == nullptr )
		{
			return false;
		}

		_value = pybind::object( _obj );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline PyObject * ptr_throw( const pybind::object & _value )
	{
		PyObject * obj = _value.ptr();

		pybind::incref( obj );

		return obj;
	}
}