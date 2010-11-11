#	pragma once

#	include "config/python.hpp"

namespace pybind
{
	class adapter_interface
	{
	public:
		virtual ~adapter_interface()
		{
		};
	};

	template<class C>
	class class_adapter_interface
		: public adapter_interface
	{
	protected:
		class_adapter_interface()
		{
			const std::type_info & class_info = typeid(C*);
			m_class_name = class_info.name();

			const std::type_info & scope_info = typeid(C);
			m_scope_name = scope_info.name();
		}

	protected:
		const char * m_class_name;
		const char * m_scope_name;
	};
}