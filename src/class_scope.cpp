#	include "pybind/class_scope.hpp"
#	include "pybind/class_type.hpp"

#	include "pybind/config.hpp"

#	include <list>
#	include <map>
#	include <string>

#	include <typeinfo>

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	typedef std::list< PyTypeObject * > TListTypeDef;
	typedef std::map< std::string, class_type_scope * > TMapTypeScope;
	//////////////////////////////////////////////////////////////////////////
	static TListTypeDef s_listTypeDef;
	static TMapTypeScope s_mapTypeScope;
	//////////////////////////////////////////////////////////////////////////
	void class_scope::reg_class_type( PyTypeObject * _type )
	{
		s_listTypeDef.push_back( _type );
	}
	//////////////////////////////////////////////////////////////////////////
	PyTypeObject * class_scope::find_sub_type( PyTypeObject * _subtype )
	{
		for each( PyTypeObject * type in s_listTypeDef )
		{
			if( PyType_IsSubtype( type, _subtype ) )
			{
				return type;
			}
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void class_scope::reg_class_scope( const type_info & _info, class_type_scope * _scope )
	{
		s_mapTypeScope[ _info.name() ] = _scope;
	}
	//////////////////////////////////////////////////////////////////////////
	class_type_scope * class_scope::get_class_scope( const type_info & _info )
	{
		TMapTypeScope::iterator it_find = s_mapTypeScope.find( _info.name() );

		if( it_find == s_mapTypeScope.end() )
		{
			return 0;
		}

		return it_find->second;		
	}
}