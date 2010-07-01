#	pragma once

#	include <list>

namespace pybind
{
	namespace helper
	{
		template<class T>
		struct delete_holder
		{
 			typedef std::list<T *> TList;
			~delete_holder()
			{
				for( typename TList::iterator
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

			TList m_list;
		};
	}
}