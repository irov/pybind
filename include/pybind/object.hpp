#	pragma once

#	include "pybind/helper.hpp"
#	include "pybind/detail.hpp"

namespace pybind
{
	class borrowed
	{
	};

	class invalid
	{
	};

	class PYBIND_API object
	{
	public:
		static const pybind::object & get_invalid();

	public:
		object();
		object( const object & _obj );

		explicit object( PyObject * _obj, borrowed );
		explicit object( PyObject * _obj );

		explicit object( invalid );

		object & operator = (const object & _obj);

		~object();

	public:
		PyObject * ptr() const;
		PyObject * ret() const;

		void reset();
		uint32_t get_ref() const;

	public:
		bool is_invalid() const;
		bool is_valid() const;
		bool is_wrap() const;
		bool is_class() const;
		bool is_type_class() const;
		bool is_none() const;
		bool is_bool() const;
		bool is_callable() const;

	public:
		bool has_attr( const detail::import_operator_t & _name ) const;
		pybind::object get_attr( const detail::import_operator_t & _name ) const;

	public:
		void unwrap() const;

	public:
		const char * repr() const;

	public:
		detail::extract_operator_t extract() const;

	public:
		detail::extract_operator_t operator () () const;
		detail::extract_operator_t operator () ( const detail::import_operator_t & _t0 ) const;
		detail::extract_operator_t operator () ( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1 ) const;
		detail::extract_operator_t operator () ( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2 ) const;
		detail::extract_operator_t operator () ( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3 ) const;
		detail::extract_operator_t operator () ( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4 ) const;
		detail::extract_operator_t operator () ( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5 ) const;
		detail::extract_operator_t operator () ( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6 ) const;
		detail::extract_operator_t operator () ( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::import_operator_t & _t7 ) const;

	public:
		detail::extract_operator_t operator () ( const detail::args_operator_t & _args ) const;
		detail::extract_operator_t operator () ( const detail::import_operator_t & _t0, const detail::args_operator_t & _args ) const;
		detail::extract_operator_t operator () ( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::args_operator_t & _args ) const;
		detail::extract_operator_t operator () ( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::args_operator_t & _args ) const;
		detail::extract_operator_t operator () ( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::args_operator_t & _args ) const;
		detail::extract_operator_t operator () ( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::args_operator_t & _args ) const;
		detail::extract_operator_t operator () ( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::args_operator_t & _args ) const;
		detail::extract_operator_t operator () ( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::args_operator_t & _args ) const;
		detail::extract_operator_t operator () ( const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::import_operator_t & _t7, const detail::args_operator_t & _args ) const;

	protected:
		PyObject * m_obj;
	};
	//////////////////////////////////////////////////////////////////////////
	PYBIND_API pybind::object make_none_t();
	PYBIND_API pybind::object make_true_t();
	PYBIND_API pybind::object make_false_t();
	PYBIND_API pybind::object make_object_t( const detail::import_operator_t & _t0 );
	PYBIND_API pybind::object make_invalid_object_t();
}