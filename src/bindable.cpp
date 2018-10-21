#include "pybind/bindable.hpp"

#include "pybind/kernel.hpp"

#ifndef NDEBUG
#include "python/python_class_type_scope_helper.hpp"
#include "pybind/exception.hpp"
#endif

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////		
    bindable::bindable()
        : m_kernel( nullptr )
        , m_embed( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bindable::~bindable()
    {
        this->unwrap();
    }
    //////////////////////////////////////////////////////////////////////////
    void bindable::setKernel( kernel_interface * _kernel )
    {
        m_kernel = _kernel;
    }
    //////////////////////////////////////////////////////////////////////////
    kernel_interface * bindable::getKernel() const
    {
        return m_kernel;
    }
    //////////////////////////////////////////////////////////////////////////
    void bindable::setEmbed( PyObject * _embed )
    {
#ifndef NDEBUG
        if( m_kernel == nullptr )
        {
            pybind::throw_exception( "bindable::setEmbed but kernel is not settup"
            );

            return;
        }

        if( m_kernel != nullptr && pybind::helper::is_object_bindable( _embed ) == false )
        {
            pybind::throw_exception( "bindable::setEmbed '%s' but scope not settup bindable"
                , m_kernel->object_repr_type( _embed )
            );

            return;
        }
#endif

        m_embed = _embed;

        this->_embedding( m_embed );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * bindable::getEmbed()
    {
        if( m_embed == nullptr )
        {
            PyObject * embed = this->_embedded();

            this->setEmbed( embed );
        }
        else
        {
            m_kernel->incref( m_embed );
        }

        return m_embed;
    }
    //////////////////////////////////////////////////////////////////////////
    bool bindable::isEmbed() const
    {
        return m_embed != nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void bindable::clear()
    {
        m_embed = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void bindable::unwrap()
    {
        if( m_embed != nullptr )
        {
            PyObject * embed = m_embed;
            m_embed = nullptr;
            m_kernel->unwrap( embed );
            m_kernel = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void bindable::_embedding( PyObject * _embed )
    {
        (void)_embed;
        //Empty
    }
}