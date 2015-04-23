#	pragma once

#	include "pybind/helper.hpp"

namespace pybind
{
	class object
	{
	public:
		object()
			: m_obj( nullptr )
		{
		}

		object( PyObject * _obj )
			: m_obj( _obj )
		{
			pybind::incref( m_obj );
		}

		object & operator = (const object & _obj)
		{
			pybind::decref( m_obj );
			m_obj = _obj.ptr();
			pybind::incref( m_obj );
			
			return *this;
		}

		object( const object & _obj )
		{
			m_obj = _obj.ptr();
			pybind::incref( m_obj );
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

		bool valid() const
		{
			return m_obj == nullptr;
		}

	public:
		detail::extract_operator_t operator () () const
		{
			return pybind::ask_t( m_obj );
		}

		template<class T0>
		detail::extract_operator_t operator () ( const T0 & _t0 ) const
		{
			return pybind::ask_t( m_obj, _t0 );
		}

		template<class T0, class T1>
		detail::extract_operator_t operator () ( const T0 & _t0, const T1 & _t1 ) const
		{
			return pybind::ask_t( m_obj, _t0, _t1 );
		}

		template<class T0, class T1, class T2>
		detail::extract_operator_t operator () ( const T0 & _t0, const T1 & _t1, const T2 & _t2 ) const
		{
			return pybind::ask_t( m_obj, _t0, _t1, _t2 );
		}

		template<class T0, class T1, class T2, class T3>
		detail::extract_operator_t operator () ( const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3 ) const
		{
			return pybind::ask_t( m_obj, _t0, _t1, _t2, _t3 );
		}
		
		template<class T0, class T1, class T2, class T3, class T4>
		detail::extract_operator_t operator () ( const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4 ) const
		{
			return pybind::ask_t( m_obj, _t0, _t1, _t2, _t3, _t4 );
		}


		template<class T0, class T1, class T2, class T3, class T4, class T5>
		detail::extract_operator_t operator () ( const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4, const T5 & _t5 ) const
		{
			return pybind::ask_t( m_obj, _t0, _t1, _t2, _t3, _t4, _t5 );
		}


		template<class T0, class T1, class T2, class T3, class T4, class T5, class T6>
		detail::extract_operator_t operator () ( const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4, const T5 & _t5, const T6 & _t6 ) const
		{
			return pybind::ask_t( m_obj, _t0, _t1, _t2, _t3, _t4, _t5, _t6 );
		}


		template<class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
		detail::extract_operator_t operator () ( const T0 & _t0, const T1 & _t1, const T2 & _t2, const T3 & _t3, const T4 & _t4, const T5 & _t5, const T6 & _t6, const T7 & _t7 ) const
		{
			return pybind::ask_t( m_obj, _t0, _t1, _t2, _t3, _t4, _t5, _t6, _t7 );
		}

	protected:
		PyObject * m_obj;
	};

	inline bool extract_value( PyObject * _obj, pybind::object & _value )
	{
		_value = pybind::object( _obj );

		return true;
	}
}