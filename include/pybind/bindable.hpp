#	pragma once

#	include "pybind/types.hpp"

#	include "pybind/system.hpp"

namespace pybind
{
	class bindable
	{
	public:
		bindable()
			:m_embed( nullptr )
		{
		}

		~bindable()
		{
			this->unwrap();
		}

	public:
		void setEmbed( PyObject * _embed )
		{
			pybind::decref( m_embed );
			m_embed = _embed;
			pybind::incref( m_embed );

			this->_embedding( m_embed );
		}

		PyObject * getEmbed()
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

		void removeEmbed()
		{
			pybind::decref( m_embed );
			m_embed = nullptr;
		}

		bool isEmbed() const
		{
			return m_embed != nullptr;
		}

	public:
		void unwrap()
		{
			pybind::unwrap( m_embed );

			pybind::decref( m_embed );
			m_embed = nullptr;
		}

	protected:
		virtual PyObject * _embedded() = 0;

	protected:
		virtual void _embedding( PyObject * _embed )
		{
			(void)_embed;
			//Empty
		}

	protected:
		PyObject * m_embed;
	};
}