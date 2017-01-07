#	pragma once

#   include "pybind/base.hpp"

#	include "pybind/helper.hpp"

namespace pybind
{
	class PYBIND_API object
        : public base
	{
	public:
		static const pybind::object & get_invalid();

	public:
		object();
        object( const base & _base );
		object( const object & _obj );

		explicit object( PyObject * _obj, borrowed );
		explicit object( PyObject * _obj );

		explicit object( invalid );

		object & operator = (const object & _obj);

		~object();

	public:
		bool has_attr( const detail::import_operator_t & _name ) const;
		pybind::object get_attr( const detail::import_operator_t & _name ) const;

	public:
		detail::extract_operator_t extract() const;

	public:
		detail::extract_operator_t call() const;
		detail::extract_operator_t call( const detail::import_operator_t & _t0 ) const;
		detail::extract_operator_t call( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1 ) const;
		detail::extract_operator_t call( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2 ) const;
		detail::extract_operator_t call( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3 ) const;
		detail::extract_operator_t call( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4 ) const;
		detail::extract_operator_t call( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5 ) const;
		detail::extract_operator_t call( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6 ) const;
		detail::extract_operator_t call( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::import_operator_t & _t7 ) const;

	public:
		detail::extract_operator_t call_args( const detail::args_operator_t & _args ) const;
		detail::extract_operator_t call_args( const detail::import_operator_t & _t0, const detail::args_operator_t & _args ) const;
		detail::extract_operator_t call_args( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::args_operator_t & _args ) const;
		detail::extract_operator_t call_args( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::args_operator_t & _args ) const;
		detail::extract_operator_t call_args( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::args_operator_t & _args ) const;
		detail::extract_operator_t call_args( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::args_operator_t & _args ) const;
		detail::extract_operator_t call_args( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::args_operator_t & _args ) const;
		detail::extract_operator_t call_args( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::args_operator_t & _args ) const;
		detail::extract_operator_t call_args( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::import_operator_t & _t7, const detail::args_operator_t & _args ) const;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class It>
	inline void foreach_t( const pybind::object & _cb, It _begin, It _end )
	{
		for( It
			it = _begin,
			it_end = _end;
		it != it_end;
		++it )
		{
			_cb.call( *it );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	PYBIND_API pybind::object make_none_t();
	PYBIND_API pybind::object make_true_t();
	PYBIND_API pybind::object make_false_t();
	PYBIND_API pybind::object make_object_t( const detail::import_operator_t & _t0 );
	PYBIND_API pybind::object make_invalid_object_t();
}