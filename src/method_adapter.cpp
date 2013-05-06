#	include "pybind/method_adapter.hpp"

#	include "class_type.hpp"

#	include <string.h>

namespace pybind
{
	namespace detail
	{
		void * meta_cast_scope( void * _self, size_t _scope_name, size_t _name, class_type_scope * scope )
		{
			size_t class_name = scope->get_type();

			if( class_name == _scope_name )
			{
				return _self;
			}	
			
			return scope->metacast( _name, _self );			
		}
	}
}

