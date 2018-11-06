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
    void bindable::setEmbed( pybind::kernel_interface * _kernel, PyObject * _embed )
    {
#ifndef NDEBUG
        if( pybind::helper::is_object_bindable( _kernel, _embed ) == false )
        {
            pybind::throw_exception( "bindable::setEmbed '%s' but scope not settup bindable"
                , _kernel->object_repr_type( _embed )
            );

            return;
        }
#endif
        m_kernel = _kernel;
        m_embed = _embed;

        this->_embedding( _kernel, m_embed );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * bindable::getEmbed( pybind::kernel_interface * _kernel )
    {
        if( m_embed == nullptr )
        {
            PyObject * embed = this->_embedded( _kernel );

            this->setEmbed( _kernel, embed );
        }
        else
        {
            _kernel->incref( m_embed );
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
        if( m_kernel != nullptr )
        {
            PyObject * embed = m_embed;
            m_embed = nullptr;
            m_kernel->unwrap( embed );
            m_kernel = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void bindable::_embedding( pybind::kernel_interface * _kernel, PyObject * _embed )
    {
        (void)_kernel;
        (void)_embed;

        //Empty
    }
}