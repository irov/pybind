#	pragma once

#	include "pybind/object.hpp"

namespace pybind
{
	class bindable
	{
	public:
		bindable()
		{
		}

		~bindable()
		{
			this->unwrap();
		}

	public:
		void setEmbed( const pybind::object & _embed )
		{
			m_embed = _embed;

			this->_embedding( m_embed );
		}

		pybind::object getEmbed()
		{
			if( m_embed.is_invalid() == true )
			{
				pybind::object embed = this->_embedded();

				if( embed.is_invalid() == true )
				{
					return pybind::make_invalid_object_t();
				}

				this->setEmbed( embed );
			}

			return m_embed;
		}

		bool isEmbed() const
		{
			return m_embed.is_valid();
		}

	public:
		void unwrap()
		{
			m_embed.unwrap();
		}

	protected:
		virtual pybind::object _embedded() = 0;

	protected:
		virtual void _embedding( const pybind::object & _embed )
		{
			(void)_embed;
			//Empty
		}

	protected:
		pybind::object m_embed;
	};
}