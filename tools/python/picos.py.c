#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "picos/utils.h"
#include "picos/piclang.h"

static PyObject * picos_piclang(PyObject *self, PyObject *args)
{
  char *program_name, *data;
  size_t len;
  FILE *file;
  PyObject *retval = NULL;
  
  if(!PyArg_Parse(args,"(s)",&program_name))
    {
      return PyErr_Format(PyExc_TypeError,"Invalid Argument. Need 1 filename.");
    }
  
  file = fopen(program_name,"r");
  if(file == NULL)
    return PyErr_SetFromErrno(PyExc_IOError);
  
  fseek(file,0,SEEK_END);
  len = ftell(file);
  rewind(file);
  
  data = (char*)malloc(len*sizeof(char));
  if(data == NULL)
    return PyErr_SetFromErrno(PyExc_IOError);
  fread(data,len,sizeof(char),file);
  
  retval = PyByteArray_FromStringAndSize(data,len);

  free(data);
  fclose(file);   
  return retval;
}

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
    {"piclang", picos_piclang, METH_VARARGS,"Load piclang program"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef picosmodule = {
   PyModuleDef_HEAD_INIT,
   "picos",   /* name of module */
   "Utility functions for PICOS", /* module documentation, may be NULL */
   -1,       /* size of per-interpreter state of the module,
                or -1 if the module keeps state in global variables. */
   picos_methods
};
#endif

PyMODINIT_FUNC
PyInit_core(void)
{
#if PY_MAJOR_VERSION >= 3
  PyObject* module = PyModule_Create(&picosmodule);
#else
  PyObject* module = Py_InitModule("core",picos_methods);
#endif
  if(module == NULL)
    return module;
  PyModule_AddIntConstant(module,"stack_size", PICLANG_STACK_SIZE);
  PyModule_AddIntConstant(module,"call_stack_size",PICLANG_CALL_STACK_SIZE);
  PyModule_AddIntConstant(module,"pcb_size",(long)PCB_SIZE);
  PyModule_AddIntConstant(module,"word_size",(long)sizeof(picos_size_t));
  return module;
}


