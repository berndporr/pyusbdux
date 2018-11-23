%module pyusbdux
%include exception.i
%{
	#define SWIG_FILE_WITH_INIT
	#include <stdio.h>
	#include "pyusbdux.h"
%}

%exception {
    try {
        $action
    } catch (const char* e) {
	PyErr_SetString(PyExc_RuntimeError, e);
	return NULL;
    }
}

%typemap(out) sample_p {
  int i;
  $result = PyList_New(16);
  for (i = 0; i < 16; i++) {
    PyObject *o = PyFloat_FromDouble((double) $1[i]);
    PyList_SetItem($result,i,o);
  }
}

%include "pyusbdux.h"
