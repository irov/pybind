#	include "pybind/base.hpp"

#	include "pybind/system.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
    base::base()
		: m_obj( nullptr )
	{
	}
	//////////////////////////////////////////////////////////////////////////
    base::base( const base & _obj )
		: m_obj( _obj.ptr() )
	{
		pybind::incref( m_obj );
	}
	//////////////////////////////////////////////////////////////////////////
    base::base( PyObject * _obj, borrowed _br )
		: m_obj(_obj)
	{
		(void)_br;
	}
	//////////////////////////////////////////////////////////////////////////
    base::base( invalid _iv )
		: m_obj(nullptr)
	{
		(void)_iv;
	}
	//////////////////////////////////////////////////////////////////////////
    base::base( PyObject * _obj )
		: m_obj( _obj )
	{
		pybind::incref( m_obj );
	}
	//////////////////////////////////////////////////////////////////////////
    base::~base()
	{
		pybind::decref( m_obj );
#	ifdef _DEBUG
		m_obj = nullptr;
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
    base & base::operator = (const base & _obj)
	{
		pybind::decref( m_obj );
		m_obj = _obj.ptr();
		pybind::incref( m_obj );

		return *this;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * base::ptr() const
	{
		return m_obj;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * base::ret() const
	{
		pybind::incref( m_obj );

		return m_obj;
	}
	//////////////////////////////////////////////////////////////////////////
	void base::reset()
	{
		pybind::decref( m_obj );
		m_obj = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t base::get_ref() const
	{
		uint32_t ref = pybind::refcount( m_obj );

		return ref;
	}
	//////////////////////////////////////////////////////////////////////////
	bool base::is_invalid() const
	{
		return m_obj == nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool base::is_valid() const
	{
		return m_obj != nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool base::is_wrap() const
	{
		bool successful = pybind::is_wrap( m_obj );

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool base::is_class() const
	{
		return pybind::is_class( m_obj );
	}
	//////////////////////////////////////////////////////////////////////////
	bool base::is_type_class() const
	{
		return pybind::is_type_class( m_obj );
	}
	//////////////////////////////////////////////////////////////////////////
	bool base::is_none() const
	{
		return pybind::is_none( m_obj );
	}
	//////////////////////////////////////////////////////////////////////////
	bool base::is_bool() const
	{
		return pybind::bool_check( m_obj );
	}
	//////////////////////////////////////////////////////////////////////////
	bool base::is_callable() const
	{
		return pybind::is_callable( m_obj );
	}
	//////////////////////////////////////////////////////////////////////////
	void base::unwrap() const
	{
		pybind::unwrap( m_obj );
	}
	//////////////////////////////////////////////////////////////////////////
	const char * base::repr() const
	{
		return pybind::object_repr( m_obj );
	}
}