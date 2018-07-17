#include "pybind/tuple.hpp"

namespace pybind
{
	//////////////////////////////////////////////////////////////////////////
	tuple::tuple( kernel_interface * _kernel )
		: pybind::object( _kernel, pybind::tuple_new( 0 ) )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	tuple::tuple( borrowed )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	tuple::tuple( kernel_interface * _kernel, PyObject * _obj, borrowed _br )
		: pybind::object( _kernel, _obj, _br )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	tuple::tuple( kernel_interface * _kernel, PyObject * _obj )
		: pybind::object( _kernel, _obj )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	detail::extract_operator_t tuple::operator[]( tuple::size_type _index ) const
	{
		return pybind::tuple_getitem_t( m_kernel, m_obj, _index );
	}
	//////////////////////////////////////////////////////////////////////////
	tuple::size_type tuple::size() const
	{
		return pybind::tuple_size( m_obj );
	}
	//////////////////////////////////////////////////////////////////////////
	bool tuple::empty() const
	{
		return this->size() == 0;
	}
	//////////////////////////////////////////////////////////////////////////	
	pybind::tuple make_tuple_t( kernel_interface * _kernel )
	{
		PyObject * py_tuple = pybind::tuple_new( 0 );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 0" );
		}

		return pybind::tuple( _kernel, py_tuple, pybind::borrowed() );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::tuple make_tuple_i( kernel_interface * _kernel, const detail::import_operator_t & _t0 )
	{
		PyObject * py_tuple = pybind::tuple_new( 1 );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 1" );
		}

		pybind::tuple_setitem_i( _kernel, py_tuple, 0, _t0 );

		return pybind::tuple( _kernel, py_tuple, pybind::borrowed() );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::tuple make_tuple_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1 )
	{
		PyObject * py_tuple = pybind::tuple_new( 2 );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 2" );
		}

		pybind::tuple_setitem_i( _kernel, py_tuple, 0, _t0 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 1, _t1 );

		return pybind::tuple( _kernel, py_tuple, pybind::borrowed() );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::tuple make_tuple_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2 )
	{
		PyObject * py_tuple = pybind::tuple_new( 3 );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 3" );
		}

		pybind::tuple_setitem_i( _kernel, py_tuple, 0, _t0 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 1, _t1 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 2, _t2 );

		return pybind::tuple( _kernel, py_tuple, pybind::borrowed() );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::tuple make_tuple_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3 )
	{
		PyObject * py_tuple = pybind::tuple_new( 4 );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 4" );
		}

		pybind::tuple_setitem_i( _kernel, py_tuple, 0, _t0 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 1, _t1 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 2, _t2 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 3, _t3 );

		return pybind::tuple( _kernel, py_tuple, pybind::borrowed() );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::tuple make_tuple_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4 )
	{
		PyObject * py_tuple = pybind::tuple_new( 5 );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 5" );
		}

		pybind::tuple_setitem_i( _kernel, py_tuple, 0, _t0 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 1, _t1 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 2, _t2 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 3, _t3 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 4, _t4 );

		return pybind::tuple( _kernel, py_tuple, pybind::borrowed() );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::tuple make_tuple_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5 )
	{ 
		PyObject * py_tuple = pybind::tuple_new( 6 );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 6" );
		}

		pybind::tuple_setitem_i( _kernel, py_tuple, 0, _t0 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 1, _t1 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 2, _t2 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 3, _t3 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 4, _t4 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 5, _t5 );

		return pybind::tuple( _kernel, py_tuple, pybind::borrowed() );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::tuple make_tuple_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6 )
	{
		PyObject * py_tuple = pybind::tuple_new( 7 );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 7" );
		}

		pybind::tuple_setitem_i( _kernel, py_tuple, 0, _t0 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 1, _t1 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 2, _t2 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 3, _t3 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 4, _t4 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 5, _t5 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 6, _t6 );

		return pybind::tuple( _kernel, py_tuple, pybind::borrowed() );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::tuple make_tuple_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::import_operator_t & _t7 )
	{ 
		PyObject * py_tuple = pybind::tuple_new( 8 );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 8" );
		}

		pybind::tuple_setitem_i( _kernel, py_tuple, 0, _t0 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 1, _t1 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 2, _t2 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 3, _t3 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 4, _t4 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 5, _t5 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 6, _t6 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 7, _t7 );

		return pybind::tuple( _kernel, py_tuple, pybind::borrowed() );
	}
	//////////////////////////////////////////////////////////////////////////	
	pybind::tuple make_tuple_args_t( kernel_interface * _kernel, const args & _args )
	{
        args::size_type args_size = _args.size();

		PyObject * py_tuple = pybind::tuple_new( 0 + args_size );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_args_t 0" );
		}

		for( args::size_type i = 0; i != args_size; ++i )
		{
			pybind::tuple_setitem_i( _kernel, py_tuple, 0 + i, _args[i] );
		}

		return pybind::tuple( _kernel, py_tuple, pybind::borrowed() );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::tuple make_tuple_args_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const args & _args )
	{
        args::size_type args_size = _args.size();

		PyObject * py_tuple = pybind::tuple_new( 1 + args_size );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 1" );
		}

		pybind::tuple_setitem_i( _kernel, py_tuple, 0, _t0 );

		for( args::size_type i = 0; i != args_size; ++i )
		{
			pybind::tuple_setitem_i( _kernel, py_tuple, 1 + i, _args[i] );
		}

		return pybind::tuple( _kernel, py_tuple, pybind::borrowed() );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::tuple make_tuple_args_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const args & _args )
	{
        args::size_type args_size = _args.size();

		PyObject * py_tuple = pybind::tuple_new( 2 + args_size );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 2" );
		}

		pybind::tuple_setitem_i( _kernel, py_tuple, 0, _t0 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 1, _t1 );

		for( args::size_type i = 0; i != args_size; ++i )
		{
			pybind::tuple_setitem_i( _kernel, py_tuple, 2 + i, _args[i] );
		}

		return pybind::tuple( _kernel, py_tuple, pybind::borrowed() );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::tuple make_tuple_args_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const args & _args )
	{
        args::size_type args_size = _args.size();

		PyObject * py_tuple = pybind::tuple_new( 3 + args_size );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 3" );
		}

		pybind::tuple_setitem_i( _kernel, py_tuple, 0, _t0 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 1, _t1 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 2, _t2 );

		for( args::size_type i = 0; i != args_size; ++i )
		{
			pybind::tuple_setitem_i( _kernel, py_tuple, 3 + i, _args[i] );
		}

		return pybind::tuple( _kernel, py_tuple, pybind::borrowed() );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::tuple make_tuple_args_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const args & _args )
	{
        args::size_type args_size = _args.size();

		PyObject * py_tuple = pybind::tuple_new( 4 + args_size );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 4" );
		}

		pybind::tuple_setitem_i( _kernel, py_tuple, 0, _t0 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 1, _t1 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 2, _t2 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 3, _t3 );

		for( args::size_type i = 0; i != args_size; ++i )
		{
			pybind::tuple_setitem_i( _kernel, py_tuple, 4 + i, _args[i] );
		}

		return pybind::tuple( _kernel, py_tuple, pybind::borrowed() );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::tuple make_tuple_args_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const args & _args )
	{
        args::size_type args_size = _args.size();

		PyObject * py_tuple = pybind::tuple_new( 5 + args_size );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 5" );
		}

		pybind::tuple_setitem_i( _kernel, py_tuple, 0, _t0 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 1, _t1 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 2, _t2 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 3, _t3 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 4, _t4 );

		for( args::size_type i = 0; i != args_size; ++i )
		{
			pybind::tuple_setitem_i( _kernel, py_tuple, 5 + i, _args[i] );
		}

		return pybind::tuple( _kernel, py_tuple, pybind::borrowed() );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::tuple make_tuple_args_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const args & _args )
	{
        args::size_type args_size = _args.size();

		PyObject * py_tuple = pybind::tuple_new( 6 + args_size );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 6" );
		}

		pybind::tuple_setitem_i( _kernel, py_tuple, 0, _t0 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 1, _t1 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 2, _t2 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 3, _t3 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 4, _t4 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 5, _t5 );

		for( args::size_type i = 0; i != args_size; ++i )
		{
			pybind::tuple_setitem_i( _kernel, py_tuple, 6 + i, _args[i] );
		}

		return pybind::tuple( _kernel, py_tuple, pybind::borrowed() );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::tuple make_tuple_args_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const args & _args )
	{
        args::size_type args_size = _args.size();

		PyObject * py_tuple = pybind::tuple_new( 7 + args_size );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 7" );
		}

		pybind::tuple_setitem_i( _kernel, py_tuple, 0, _t0 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 1, _t1 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 2, _t2 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 3, _t3 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 4, _t4 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 5, _t5 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 6, _t6 );

		for( args::size_type i = 0; i != args_size; ++i )
		{
			pybind::tuple_setitem_i( _kernel, py_tuple, 7 + i, _args[i] );
		}

		return pybind::tuple( _kernel, py_tuple, pybind::borrowed() );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::tuple make_tuple_args_i( kernel_interface * _kernel, const detail::import_operator_t & _t0, const detail::import_operator_t & _t1, const detail::import_operator_t & _t2, const detail::import_operator_t & _t3, const detail::import_operator_t & _t4, const detail::import_operator_t & _t5, const detail::import_operator_t & _t6, const detail::import_operator_t & _t7, const args & _args )
	{
        args::size_type args_size = _args.size();

		PyObject * py_tuple = pybind::tuple_new( 8 );

		if( py_tuple == nullptr )
		{
			pybind::throw_exception( "make_tuple_t 8" );
		}

		pybind::tuple_setitem_i( _kernel, py_tuple, 0, _t0 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 1, _t1 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 2, _t2 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 3, _t3 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 4, _t4 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 5, _t5 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 6, _t6 );
		pybind::tuple_setitem_i( _kernel, py_tuple, 7, _t7 );

		for( args::size_type i = 0; i != args_size; ++i )
		{
			pybind::tuple_setitem_i( _kernel, py_tuple, 8 + i, _args[i] );
		}

		return pybind::tuple( _kernel, py_tuple, pybind::borrowed() );
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::tuple make_invalid_tuple_t( kernel_interface * _kernel )
	{
		(void)_kernel;

		return pybind::tuple( pybind::borrowed() );
	}
}