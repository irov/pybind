#pragma once

#include "pybind/types.hpp"

namespace pybind
{
    class bindable
    {
    public:
        bindable();
        ~bindable();

    public:
        void setEmbed( pybind::kernel_interface * _kernel, PyObject * _embed );
        PyObject * getEmbed( pybind::kernel_interface * _kernel );
        bool isEmbed() const;

    public:
        void clear();
        void unwrap();

    protected:
        virtual PyObject * _embedded( pybind::kernel_interface * _kernel ) = 0;

    protected:
        virtual void _embedding( pybind::kernel_interface * _kernel, PyObject * _embed );

    protected:
        kernel_interface * m_kernel;
        PyObject * m_embed;
    };
}