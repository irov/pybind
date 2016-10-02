#	include "pybind/object.hpp"

#	include "pybind/system.hpp"

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
	object::object( invalid _iv )
		: m_obj(nullptr)
	{
		(void)_iv;
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
#	ifdef _DEBUG
		m_obj = nullptr;
#	endif
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
	PyObject * object::ptr() const
	{
		return m_obj;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * object::ret() const
	{
		pybind::incref( m_obj );

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
		bool successful = pybind::is_wrap( m_obj );

		return successful;
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
	bool object::has_attr( const detail::import_operator_t & _name ) const
	{
		return pybind::has_attr( m_obj, _name );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::object object::get_attr( const detail::import_operator_t & _name ) const
	{
		PyObject * py_attr = pybind::get_attr( m_obj, _name );

		return pybind::object( py_attr, pybind::borrowed() );
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
	detail::extract_operator_t object::extract() const
	{
		return detail::extract_operator_t( m_obj );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t object::call() const
	{
		return pybind::call_t( m_obj );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t object::call( const detail::import_operator_t & _t0 ) const
	{
		return pybind::call_t( m_obj, _t0 );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t object::call( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1 ) const
	{
		return pybind::call_t( m_obj, _t0, _t1 );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t object::call( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2 ) const
	{
		return pybind::call_t( m_obj, _t0, _t1, _t2 );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t object::call( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3 ) const
	{
		return pybind::call_t( m_obj, _t0, _t1, _t2, _t3 );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t object::call( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4 ) const
	{
		return pybind::call_t( m_obj, _t0, _t1, _t2, _t3, _t4 );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t object::call( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5 ) const
	{
		return pybind::call_t( m_obj, _t0, _t1, _t2, _t3, _t4, _t5 );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t object::call( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6 ) const
	{
		return pybind::call_t( m_obj, _t0, _t1, _t2, _t3, _t4, _t5, _t6 );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t object::call( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::import_operator_t & _t7 ) const
	{
		return pybind::call_t( m_obj, _t0, _t1, _t2, _t3, _t4, _t5, _t6, _t7 );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t object::call_args( const detail::args_operator_t & _args ) const
	{
		return pybind::call_args_t( m_obj, _args );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t object::call_args( const detail::import_operator_t & _t0, const detail::args_operator_t & _args ) const
	{
		return pybind::call_args_t( m_obj, _t0, _args );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t object::call_args( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::args_operator_t & _args ) const
	{
		return pybind::call_args_t( m_obj, _t0, _t1, _args );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t object::call_args( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::args_operator_t & _args ) const
	{
		return pybind::call_args_t( m_obj, _t0, _t1, _t2, _args );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t object::call_args( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::args_operator_t & _args ) const
	{
		return pybind::call_args_t( m_obj, _t0, _t1, _t2, _t3, _args );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t object::call_args( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::args_operator_t & _args ) const
	{
		return pybind::call_args_t( m_obj, _t0, _t1, _t2, _t3, _t4, _args );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t object::call_args( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::args_operator_t & _args ) const
	{
		return pybind::call_args_t( m_obj, _t0, _t1, _t2, _t3, _t4, _t5, _args );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t object::call_args( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::args_operator_t & _args ) const
	{
		return pybind::call_args_t( m_obj, _t0, _t1, _t2, _t3, _t4, _t5, _t6, _args );
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t object::call_args( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::import_operator_t & _t7, const detail::args_operator_t & _args ) const
	{
		return pybind::call_args_t( m_obj, _t0, _t1, _t2, _t3, _t4, _t5, _t6, _t7, _args );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::object make_none_t()
	{
		PyObject * py_none = pybind::ret_none();

		return pybind::object( py_none, pybind::borrowed() );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::object make_true_t()
	{
		PyObject * py_true = pybind::ret_true();

		return pybind::object( py_true, pybind::borrowed() );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::object make_false_t()
	{
		PyObject * py_false = pybind::ret_false();

		return pybind::object( py_false, pybind::borrowed() );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::object make_object_t( const detail::import_operator_t & _t0 )
	{
		return pybind::object( _t0 );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::object make_invalid_object_t()
	{
		return pybind::object( pybind::invalid() );
	}
}