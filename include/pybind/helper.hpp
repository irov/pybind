#	pragma once

#	include <list>

namespace pybind
{
	namespace helper
	{
		template<class T>
		struct delete_holder
		{
			~delete_holder()
			{
				for( TList::iterator
					it = m_list.begin(),
					it_end = m_list.end();
				it != it_end;
				++it)
				{
					delete *it;
				}
			}

			void add( T * _ptr )
			{
				m_list.push_back( _ptr );
			}

			typedef std::list<T *> TList;
			TList m_list;
		};
	}
}