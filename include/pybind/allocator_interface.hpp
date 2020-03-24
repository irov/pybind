#pragma once

#include <stddef.h>

namespace pybind
{
    class allocator_interface
    {
    public:
        allocator_interface() = default;
        virtual ~allocator_interface() = default;

    public:
        virtual void * malloc( size_t _size ) = 0;
        virtual void * calloc( size_t _num, size_t _size ) = 0;
        virtual void * realloc( void * _ptr, size_t _size ) = 0;
        virtual void free( void * _ptr ) = 0;

    public:
        template<class T, class ... Args>
        T * newP( Args && ... _args )
        {
            void * p = this->malloc( sizeof( T ) );

            new (p) T( std::forward<Args &&>( _args )... );

            T * t = static_cast<T *>(p);

            return t;
        }

        template<class T, class ... Args>
        stdex::intrusive_ptr<T> newT( Args && ... _args )
        {
            void * p = this->malloc( sizeof( T ) );

            new (p) T( std::forward<Args &&>( _args )... );

            T * t = static_cast<T *>(p);

            t->set_allocator( this );

            return stdex::intrusive_ptr<T>::from( t );
        }

        template<class T>
        void deleteT( T * _ptr )
        {
            _ptr->~T();

            this->free( _ptr );
        }
    };
}
