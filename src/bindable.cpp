#include "pybind/bindable.hpp"

#include "pybind/system.hpp"

#ifndef NDEBUG
#include "python_class_type_scope_helper.hpp"
#include "pybind/exception.hpp"
#endif

namespace pybind
{
    //////////////////////////////////////////////////////////////////////////		
    bindable::bindable()
        : m_embed( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bindable::~bindable()
    {
        this->unwrap();
    }
    //////////////////////////////////////////////////////////////////////////
    void bindable::setEmbed( PyObject * _embed )
    {
#ifndef NDEBUG
        if( pybind::helper::is_object_bindable( _embed ) == false )
        {
            pybind::throw_exception( "bindable::setEmbed '%s' but scope not settup bindable"
                , pybind::object_repr_type( _embed )
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
            pybind::incref( m_embed );
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
            pybind::unwrap( embed );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void bindable::_embedding( PyObject * _embed )
    {
        (void)_embed;
        //Empty
    }
}