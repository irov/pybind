#pragma once

#include "pybind/mutex.hpp"

namespace pybind
{
    class mutex_scope
    {
    public:
        mutex_scope( kernel_interface * _kernel )
            : m_kernel( _kernel )
        {
            m_kernel->lock_mutex();
        }

        ~mutex_scope()
        {
            m_kernel->unlock_mutex();
        }

    protected:
        kernel_interface * m_kernel;
    };
}