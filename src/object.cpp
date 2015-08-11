#	include "pybind/object.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	const pybind::object & object::get_invalid()
	{
		static pybind::object s_obj;

		return s_obj;
	}
	//////////////////////////////////////////////////////////////////////////
	object::object()
		: m_obj( nullptr )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	object & object::operator = (const object & _obj)
	{
		pybind::decref( m_obj );
		m_obj = _obj.ptr();
		pybind::incref( m_obj );

		return *this;
	}
	//////////////////////////////////////////////////////////////////////////
	object::object( const object & _obj )
		: m_obj( _obj.ptr() )
	{
		pybind::incref( m_obj );
	}
	//////////////////////////////////////////////////////////////////////////
	object::object( PyObject * _obj, borrowed _br )
		: m_obj(_obj)
	{
		(void)_br;
	}
	//////////////////////////////////////////////////////////////////////////
	object::object( PyObject * _obj )
		: m_obj( _obj )
	{
		pybind::incref( m_obj );
	}
	//////////////////////////////////////////////////////////////////////////
	object::~object()
	{
		pybind::decref( m_obj );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * object::ptr() const
	{
		return m_obj;
	}
	//////////////////////////////////////////////////////////////////////////
	void object::reset()
	{
		pybind::decref( m_obj );
		m_obj = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t object::get_ref() const
	{
		uint32_t ref = pybind::refcount( m_obj );

		return ref;
	}
	//////////////////////////////////////////////////////////////////////////
	bool object::is_invalid() const
	{
		return m_obj == nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool object::is_valid() const
	{
		return m_obj != nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool object::is_wrap() const
	{
		return pybind::detail::is_wrap( m_obj );
	}
	//////////////////////////////////////////////////////////////////////////
	bool object::is_class() const
	{
		return pybind::is_class( m_obj );
	}
	//////////////////////////////////////////////////////////////////////////
	bool object::is_type_class() const
	{
		return pybind::is_type_class( m_obj );
	}
	//////////////////////////////////////////////////////////////////////////
	bool object::is_none() const
	{
		return pybind::is_none( m_obj );
	}
	//////////////////////////////////////////////////////////////////////////
	bool object::is_bool() const
	{
		return pybind::bool_check( m_obj );
	}
	//////////////////////////////////////////////////////////////////////////
	bool object::is_callable() const
	{
		return pybind::is_callable( m_obj );
	}
	//////////////////////////////////////////////////////////////////////////
	bool object::has_attr( const char * _name ) const
	{
		return pybind::has_attr( m_obj, _name );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::object object::get_attr( const char * _name ) const
	{
		PyObject * py_attr = pybind::get_attr( m_obj, _name );

		return pybind::object( py_attr );
	}
	//////////////////////////////////////////////////////////////////////////
	void object::unwrap() const
	{
		pybind::unwrap( m_obj );
	}
	//////////////////////////////////////////////////////////////////////////
	const char * object::repr() const
	{
		return pybind::object_repr( m_obj );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t object::extract()
	{
		return detail::extract_operator_t( m_obj );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t object::operator () () const
	{
		return pybind::call_t( m_obj );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t object::operator () ( const detail::import_operator_t & _t0 ) const
	{
		return pybind::call_t( m_obj, _t0 );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t object::operator () ( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1 ) const
	{
		return pybind::call_t( m_obj, _t0, _t1 );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t object::operator () ( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2 ) const
	{
		return pybind::call_t( m_obj, _t0, _t1, _t2 );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t object::operator () ( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3 ) const
	{
		return pybind::call_t( m_obj, _t0, _t1, _t2, _t3 );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t object::operator () ( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4 ) const
	{
		return pybind::call_t( m_obj, _t0, _t1, _t2, _t3, _t4 );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t object::operator () ( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5 ) const
	{
		return pybind::call_t( m_obj, _t0, _t1, _t2, _t3, _t4, _t5 );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t object::operator () ( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6 ) const
	{
		return pybind::call_t( m_obj, _t0, _t1, _t2, _t3, _t4, _t5, _t6 );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t object::operator () ( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::import_operator_t & _t7 ) const
	{
		return pybind::call_t( m_obj, _t0, _t1, _t2, _t3, _t4, _t5, _t6, _t7 );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::object make_none_t()
	{
		PyObject * py_none = pybind::ret_none();

		return pybind::object( py_none );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::object make_true_t()
	{
		PyObject * py_true = pybind::ret_true();

		return pybind::object( py_true );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::object make_false_t()
	{
		PyObject * py_false = pybind::ret_false();

		return pybind::object( py_false );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::object make_invalid_object_t()
	{
		return pybind::object( nullptr );
	}
}