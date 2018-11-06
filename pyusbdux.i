%module pyusbdux
%{
	#define SWIG_FILE_WITH_INIT
	#include <stdio.h>
	#include "pyusbdux.h"
%}

%typemap(out) sample_p {
  int i;
  $result = PyList_New(16);
  for (i = 0; i < 8; i++) {
    PyObject *o = PyFloat_FromDouble((double) $1[i]);
    PyList_SetItem($result,i,o);
  }
}

%include "pyusbdux.h"
