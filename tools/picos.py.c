#include <Python.h>
#include <stdio.h>
#include <stdlib.h>

static PyObject *
picos_picdisk(PyObject *self, PyObject *args)
{
    char *filename;
    FILE *image_file;
    if(!PyArg_Parse(args,"(s)",&filename))
        return PyErr_Format(PyExc_TypeError,"Invalid Argument. Need 1 filename.");
    if(filename == NULL)
        return PyErr_SetFromErrno(PyExc_IOError);

    image_file = fopen(filename,"r");
    if(image_file == NULL)
        return NULL;

    scan_disk(image_file);
    fclose(image_file);

    return Py_None;
}

static PyMethodDef picos_methods[] = {
    {"picdisk",  picos_picdisk, METH_VARARGS,"Writes information about a disk image to stdout."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef picosmodule = {
   PyModuleDef_HEAD_INIT,
   "picos",   /* name of module */
   "Utility functions for PICOS", /* module documentation, may be NULL */
   -1,       /* size of per-interpreter state of the module,
                or -1 if the module keeps state in global variables. */
   picos_methods
};

PyMODINIT_FUNC
PyInit_picos(void)
{
    return PyModule_Create(&picosmodule);
}


