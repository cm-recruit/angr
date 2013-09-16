#include <Python.h>
#include <structmember.h>
#include <libvex.h>

#include "pyvex_types.h"
#include "pyvex_macros.h"
#include "vex/angr_vexir.h"

PYVEX_NEW(IRSB)
PYVEX_DEALLOC(IRSB)
PYVEX_WRAP(IRSB)
PYVEX_METH_STANDARD(IRSB)

static int
pyIRSB_init(pyIRSB *self, PyObject *args, PyObject *kwargs)
{
	if (!kwargs) { self->wrapped = emptyIRSB(); return 0; }
	PYVEX_WRAP_CONSTRUCTOR(IRSB);

	PyObject *py_bytes = NULL;
	unsigned char *bytes = NULL;
	unsigned int mem_addr = 0;
	int num_inst = -1;
	int num_bytes = 0;

	static char *kwlist[] = {"wrap", "bytes", "mem_addr", "num_inst", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|OSii", kwlist, &wrap_object, &py_bytes, &mem_addr, &num_inst)) return -1;

	if (py_bytes)
	{
		if (PyString_Size(py_bytes) == 0)
		{
			PyErr_SetString(VexException, "No bytes provided");
			return -1;
		}

		num_bytes = PyString_Size(py_bytes);
		bytes = (unsigned char *)PyString_AsString(py_bytes);

		vex_init();
		if (num_inst > -1) self->wrapped = vex_block_inst(VexArchAMD64, bytes, mem_addr, num_inst);
		else self->wrapped = vex_block_bytes(VexArchAMD64, bytes, mem_addr, num_bytes);
	}
	else
	{
		PyErr_SetString(VexException, "Not enough arguments provided.");
		return -1;
	}

	return 0;
}

static PyMemberDef pyIRSB_members[] =
{
	{NULL}  /* Sentinel */
};

PYVEX_ACCESSOR_SET_WRAPPED(IRSB)
PYVEX_ACCESSOR_GET_WRAPPED(IRSB)

static PyGetSetDef pyIRSB_getseters[] =
{
	PYVEX_ACCESSOR_DEF_WRAPPED(IRSB),
	{NULL}  /* Sentinel */
};

static PyObject *
pyIRSB_statements(pyIRSB* self)
{
	PyObject *result = PyTuple_New(self->wrapped->stmts_used);

	for (int i = 0; i < self->wrapped->stmts_used; i++)
	{
		PyObject *wrapped = wrap_IRStmt(self->wrapped->stmts[i]);
		PyTuple_SetItem(result, i, wrapped);
	}

	return result;
}

static PyObject *pyIRSB_deepCopyExceptStmts(pyIRSB* self) { return (PyObject *)wrap_IRSB(deepCopyIRSBExceptStmts(self->wrapped)); }
static PyObject *pyIRSB_addStatement(pyIRSB* self, PyObject *stmt)
{
	PYVEX_CHECKTYPE(stmt, pyIRStmtType);
	addStmtToIRSB(self->wrapped, ((pyIRStmt *)stmt)->wrapped);
	Py_RETURN_NONE;
}

static PyMethodDef pyIRSB_methods[] =
{
	PYVEX_METHDEF_STANDARD(IRSB),
	{"addStatement", (PyCFunction)pyIRSB_addStatement, METH_O, "Adds a statement to the basic block."},
	{"deepCopyExceptStmts", (PyCFunction)pyIRSB_deepCopyExceptStmts, METH_NOARGS, "Copies the IRSB, without any statements."},
	{"statements", (PyCFunction)pyIRSB_statements, METH_NOARGS, "Returns a tuple of the IRStmts in the IRSB"},
	{NULL}  /* Sentinel */
};

PyTypeObject pyIRSBType =
{
	PyObject_HEAD_INIT(NULL)
	0,						 /*ob_size*/
	"pyvex.IRSB",			 /*tp_name*/
	sizeof(pyIRSB),			 /*tp_basicsize*/
	0,						 /*tp_itemsize*/
	(destructor)pyIRSB_dealloc, /*tp_dealloc*/
	0,						 /*tp_print*/
	0,						 /*tp_getattr*/
	0,						 /*tp_setattr*/
	0,						 /*tp_compare*/
	0,						 /*tp_repr*/
	0,						 /*tp_as_number*/
	0,						 /*tp_as_sequence*/
	0,						 /*tp_as_mapping*/
	0,						 /*tp_hash */
	0,						 /*tp_call*/
	0,						 /*tp_str*/
	0,						 /*tp_getattro*/
	0,						 /*tp_setattro*/
	0,						 /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
	"pyIRSB objects",		   /* tp_doc */
	0,					   /* tp_traverse */
	0,					   /* tp_clear */
	0,					   /* tp_richcompare */
	0,					   /* tp_weaklistoffset */
	0,					   /* tp_iter */
	0,					   /* tp_iternext */
	pyIRSB_methods,			 /* tp_methods */
	pyIRSB_members,			 /* tp_members */
	pyIRSB_getseters,		   /* tp_getset */
	0,						 /* tp_base */
	0,						 /* tp_dict */
	0,						 /* tp_descr_get */
	0,						 /* tp_descr_set */
	0,						 /* tp_dictoffset */
	(initproc)pyIRSB_init,	  /* tp_init */
	0,						 /* tp_alloc */
	pyIRSB_new,				 /* tp_new */
};