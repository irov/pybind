#	include "pybind/system.hpp"

#	include "config/python.hpp"

namespace pybind
{
	struct PyLinkedNode
	{
		PyObject * value;
		struct PyLinkedNode * next;
	};

	static void node_link( PyLinkedNode * self, PyLinkedNode * dist )
	{
		dist->next = self->next;
		self->next = dist;
	}

	struct PyLinkedObject
	{
		PyObject_VAR_HEAD;

		PyLinkedNode * root;
		PyLinkedNode * last;

		Py_ssize_t size;
	};

	struct PyLinkedIterator
	{
		PyObject_HEAD;
		PyLinkedObject * linked;
		PyLinkedNode * node; /* Set to NULL when iterator is exhausted */
	};

	extern PyTypeObject PyLinked_Type;
	extern PyTypeObject PyLinkedIter_Type;

#define PyLinked_CheckExact(op) (Py_TYPE(op) == &PyLinked_Type)

	static int
		linked_clear( PyLinkedObject *a )
	{
		PyLinkedNode * node = a->root;

		while( node != nullptr )
		{
			PyObject * value = node->value;

			Py_DECREF( value );

			PyLinkedNode * next = node->next;

			PyMem_FREE( node );

			node = next;
		}

		a->root = nullptr;
		a->last = nullptr;
		a->size = 0;

		return 0;
	}

	static void
		linked_dealloc( PyLinkedObject * self )
	{
		PyObject_GC_UnTrack( self );

		linked_clear( self );

		Py_TYPE( self )->tp_free( (PyObject *)self );
	}

	static Py_ssize_t
		linked_length( PyLinkedObject *a )
	{
		return a->size;
	}

	static int
		linked_contains( PyLinkedObject *a, PyObject *el )
	{
		PyLinkedNode * node = a->root;

		while( node != nullptr )
		{
			PyObject * value = node->value;

			if( PyObject_RichCompareBool( el, value, Py_EQ ) == 1 )
			{
				return 1;
			}

			node = node->next;
		}

		return 0;
	}

	static PyObject *
		linked_item( PyLinkedObject *a, Py_ssize_t i )
	{
		Py_ssize_t index = 0;

		PyLinkedNode * node = a->root;

		while( node != nullptr )
		{
			if( index++ == i )
			{
				PyObject * value = node->value;

				Py_INCREF( value );

				return value;
			}

			node = node->next;
		}

		PyErr_SetString( PyExc_IndexError, "linked index out of range" );

		return NULL;
	}

	static int
		linked_ass_item( PyLinkedObject *a, Py_ssize_t i, PyObject *v )
	{
		Py_ssize_t index = 0;

		PyLinkedNode * node = a->root;

		while( node != nullptr )
		{
			if( index++ == i )
			{
				Py_INCREF( v );
				Py_DECREF( node->value );

				node->value = v;

				return 0;
			}

			node = node->next;
		}

		PyErr_SetString( PyExc_IndexError, "linked assignment index out of range" );

		return -1;
	}

	static PyObject *
		linkedappend( PyLinkedObject *self, PyObject *v )
	{
		PyLinkedNode * node = PyMem_New( PyLinkedNode, 1 );

		node->value = v;
		Py_INCREF( node->value );

		node->next = nullptr;

		if( self->last == nullptr )
		{
			self->root = node;
			self->last = node;
		}
		else
		{
			node_link( self->last, node );
			self->last = node;
		}

		self->size++;

		Py_RETURN_NONE;
	}

	static PyObject *
		linkedpushfront( PyLinkedObject *self, PyObject *v )
	{
		PyLinkedNode * node = PyMem_New( PyLinkedNode, 1 );

		node->value = v;
		Py_INCREF( node->value );

		node->next = nullptr;

		if( self->root == nullptr )
		{
			self->root = node;
			self->last = node;
		}
		else
		{
			PyLinkedNode * root = self->root;

			self->root = node;

			node_link( node, root );
		}

		self->size++;

		Py_RETURN_NONE;
	}

	static PyObject *
		linkedpopfront( PyLinkedObject *self )
	{
		if( self->root == nullptr )
		{
			PyErr_SetString( PyExc_IndexError, "linked empty" );

			return NULL;
		}

		PyLinkedNode * pop_node = self->root;

		self->root = self->root->next;

		if( self->root == nullptr )
		{
			self->last = nullptr;			
		}

		PyObject * pop_value = pop_node->value;

		PyMem_FREE( pop_node );

		return pop_value;
	}
	
	static PyObject * linkedclone( PyLinkedObject *self )
	{ 
		PyLinkedObject * new_linked = PyObject_GC_New( PyLinkedObject, &PyLinked_Type );

		new_linked->last = nullptr;
		new_linked->root = nullptr;

		new_linked->size = 0;

		PyLinkedNode * node = self->root;

		while( node != nullptr )
		{
			linkedappend( new_linked, node->value );

			node = node->next;
		}

		return (PyObject *)new_linked;
	}

	static PyObject * linkedempty( PyLinkedObject *self )
	{
		if( self->root == nullptr )
		{
			Py_RETURN_TRUE;
		}
		
		Py_RETURN_FALSE;
	}

	static PyObject *
		linkedremove( PyLinkedObject *self, PyObject *v )
	{
		if( self->root == nullptr )
		{
			PyErr_SetString( PyExc_ValueError, "linked.remove(x): x not in list" );

			return NULL;
		}

		--self->size;

		if( PyObject_RichCompareBool( self->root->value, v, Py_EQ ) > 0 )
		{
			Py_DECREF( self->root->value );

			PyLinkedNode * node_free = self->root;

			if( self->last == self->root )
			{
				self->last = nullptr;
				self->root = nullptr;
			}
			else
			{
				self->root = self->root->next;
			}

			PyMem_FREE( node_free );

			Py_RETURN_NONE;
		}

		PyLinkedNode * node = self->root;

		while( node->next != nullptr )
		{
			if( PyObject_RichCompareBool( node->next->value, v, Py_EQ ) == 0 )
			{
				node = node->next;

				continue;
			}

			if( node->next == self->last )
			{
				self->last = node;
			}

			node->next = node->next->next;

			break;
		}

		Py_RETURN_NONE;
	}

	static int
		linked_traverse( PyLinkedObject *self, visitproc visit, void *arg )
	{
		PyLinkedNode * node = self->root;

		while( node != nullptr )
		{
			PyObject * value = node->value;

			Py_VISIT( value );

			node = node->next;
		}

		return 0;
	}

	static PyObject *
		linked_iter( PyLinkedObject * seq )
	{
		PyLinkedIterator * it = PyObject_GC_New( PyLinkedIterator, &PyLinkedIter_Type );

		if( it == NULL )
			return NULL;

		Py_INCREF( seq );
		it->linked = seq;

		it->node = seq->root;

		_PyObject_GC_TRACK( it );

		return (PyObject *)it;
	}


	PyDoc_STRVAR( append_doc,
		"L.append(object) -- append object to end" );
	PyDoc_STRVAR( clone_doc,
		"L.clone() -- clone linked" );
	PyDoc_STRVAR( empty_doc,
		"L.empty() -- is empty linked" );
	PyDoc_STRVAR( remove_doc,
		"L.remove(value) -- remove first occurrence of value.\n"
		"Raises ValueError if the value is not present." );

	static PyMethodDef linked_methods[] = {
		{"append", (PyCFunction)linkedappend, METH_O, append_doc},
		{"push_front", (PyCFunction)linkedpushfront, METH_O, append_doc},
		{"pop_front", (PyCFunction)linkedpopfront, METH_O, append_doc},
		{"clone", (PyCFunction)linkedclone, METH_NOARGS, clone_doc},
		{"empty", (PyCFunction)linkedempty, METH_NOARGS, empty_doc},
		{"remove", (PyCFunction)linkedremove, METH_O, remove_doc},
		{NULL, NULL}           /* sentinel */
	};
		
	static PySequenceMethods linked_as_sequence = {
		(lenfunc)linked_length,                       /* sq_length */
		0,                    /* sq_concat */
		0,                  /* sq_repeat */
		(ssizeargfunc)linked_item,                    /* sq_item */
		0,					/*sq_slice*/
		(ssizeobjargproc)linked_ass_item,             /* sq_ass_item */
		0,       /* sq_ass_slice */
		(objobjproc)linked_contains,                  /* sq_contains */
		0,            /* sq_inplace_concat */
		0,          /* sq_inplace_repeat */
	};

	PyDoc_STRVAR( linked_doc, "linked() -> new empty linked" );

	PyTypeObject PyLinked_Type = {
		PyVarObject_HEAD_INIT( &PyType_Type, 0 )
		"linked",
		sizeof( PyLinkedObject ),
		0,
		(destructor)linked_dealloc,                   /* tp_dealloc */
		0,                      /* tp_print */
		0,                                          /* tp_getattr */
		0,                                          /* tp_setattr */
		0,                                          /* tp_compare */
		0,                        /* tp_repr */
		0,                                          /* tp_as_number */
		&linked_as_sequence,                          /* tp_as_sequence */
		0,                           /* tp_as_mapping */
		(hashfunc)PyObject_HashNotImplemented,      /* tp_hash */
		0,                                          /* tp_call */
		0,                                          /* tp_str */
		PyObject_GenericGetAttr,                    /* tp_getattro */
		0,                                          /* tp_setattro */
		0,                                          /* tp_as_buffer */
		Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC |
		Py_TPFLAGS_BASETYPE,         /* tp_flags */
		linked_doc,                                   /* tp_doc */
		(traverseproc)linked_traverse,                /* tp_traverse */
		(inquiry)linked_clear,                        /* tp_clear */
		0,                           /* tp_richcompare */
		0,                                          /* tp_weaklistoffset */
		(getiterfunc)linked_iter,                                  /* tp_iter */
		0,                                          /* tp_iternext */
		linked_methods,                               /* tp_methods */
		0,                                          /* tp_members */
		0,                                          /* tp_getset */
		0,                                          /* tp_base */
		0,                                          /* tp_dict */
		0,                                          /* tp_descr_get */
		0,                                          /* tp_descr_set */
		0,                                          /* tp_dictoffset */
		0,                        /* tp_init */
		PyType_GenericAlloc,                        /* tp_alloc */
		PyType_GenericNew,                          /* tp_new */
		PyObject_GC_Del,                            /* tp_free */
	};

	/*********************** List Iterator **************************/

	static void
		linkediter_dealloc( PyLinkedIterator *it )
	{
		_PyObject_GC_UNTRACK( it );
		Py_XDECREF( it->linked );

		PyObject_GC_Del( it );
	}

	static int
		linkediter_traverse( PyLinkedIterator *it, visitproc visit, void *arg )
	{
		Py_VISIT( it->node->value );

		return 0;
	}

	static PyObject *
		linkediter_next( PyLinkedIterator *it )
	{
		if( it->node == nullptr )
		{
			Py_DECREF( it->linked );

			it->linked = nullptr;

			return NULL;
		}

		PyObject * item = it->node->value;

		it->node = it->node->next;

		Py_INCREF( item );

		return item;
	}

	PyTypeObject PyLinkedIter_Type = {
		PyVarObject_HEAD_INIT( &PyType_Type, 0 )
		"linkediterator",                             /* tp_name */
		sizeof( PyLinkedIterator ),                     /* tp_basicsize */
		0,                                          /* tp_itemsize */
		/* methods */
		(destructor)linkediter_dealloc,               /* tp_dealloc */
		0,                                          /* tp_print */
		0,                                          /* tp_getattr */
		0,                                          /* tp_setattr */
		0,                                          /* tp_compare */
		0,                                          /* tp_repr */
		0,                                          /* tp_as_number */
		0,                                          /* tp_as_sequence */
		0,                                          /* tp_as_mapping */
		0,                                          /* tp_hash */
		0,                                          /* tp_call */
		0,                                          /* tp_str */
		PyObject_GenericGetAttr,                    /* tp_getattro */
		0,                                          /* tp_setattro */
		0,                                          /* tp_as_buffer */
		Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC,/* tp_flags */
		0,                                          /* tp_doc */
		(traverseproc)linkediter_traverse,            /* tp_traverse */
		0,                                          /* tp_clear */
		0,                                          /* tp_richcompare */
		0,                                          /* tp_weaklistoffset */
		PyObject_SelfIter,                          /* tp_iter */
		(iternextfunc)linkediter_next,                /* tp_iternext */
		0,                           /* tp_methods */
		0,                                          /* tp_members */
	};

	bool initialize_linked()
	{
		if( PyType_Ready( &PyLinked_Type ) < 0 )
		{
			return false;
		}

		if( PyType_Ready( &PyLinkedIter_Type ) < 0 )
		{
			return false;
		}

		PyObject * builtins = pybind::get_builtins();

		PyObject * dir_bltin = pybind::module_dict( builtins );

		if( PyDict_SetItemString( dir_bltin, "linked", (PyObject *)&PyLinked_Type ) < 0 )
		{
			return false;
		}

		return true;
	}
}