#	pragma once

#	include "pybind/system.hpp"

namespace pybind
{
	class bindable
	{
	public:
		bindable()
			: m_embed( nullptr )
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

			this->_embedding( m_embed );
		}

		PyObject * getEmbed()
		{
			if( m_embed == nullptr )
			{
				PyObject * embed = this->_embedded();

				if( embed == nullptr )
				{
					return nullptr;
				}

				this->setEmbed( embed );
			}

			return m_embed;
		}

		bool isEmbed() const
		{
			return m_embed != nullptr;
		}

	public:
		void unwrap()
		{
			if( m_embed != nullptr )
			{
				pybind::unwrap( m_embed );
				pybind::decref( m_embed );

				m_embed = nullptr;
			}
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