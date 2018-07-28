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
        void setEmbed( PyObject * _embed );
        PyObject * getEmbed();
        bool isEmbed() const;

    public:
        void clear();
        void unwrap();

    protected:
        virtual PyObject * _embedded() = 0;

    protected:
        virtual void _embedding( PyObject * _embed );

    protected:
        PyObject * m_embed;
    };
}