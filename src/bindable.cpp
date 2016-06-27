#	include "pybind/bindable.hpp"

#	include "pybind/system.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////		
	bindable::bindable()
		:m_embed( nullptr )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bindable::~bindable()
	{
		this->unwrap();
	}
	//////////////////////////////////////////////////////////////////////////
	void bindable::setEmbed( PyObject * _embed )
	{
		pybind::decref( m_embed );
		m_embed = _embed;
		pybind::incref( m_embed );

		this->_embedding( m_embed );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * bindable::getEmbed()
	{
		if( m_embed == nullptr )
		{
			PyObject * embed = this->_embedded();

			this->setEmbed( embed );

			pybind::decref( embed );
		}

		pybind::incref( m_embed );

		return m_embed;
	}
	//////////////////////////////////////////////////////////////////////////
	void bindable::removeEmbed()
	{
		pybind::decref( m_embed );
		m_embed = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool bindable::isEmbed() const
	{
		return m_embed != nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void bindable::unwrap()
	{
		pybind::unwrap( m_embed );

		pybind::decref( m_embed );
		m_embed = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void bindable::_embedding( PyObject * _embed )
	{
		(void)_embed;
		//Empty
	}
}