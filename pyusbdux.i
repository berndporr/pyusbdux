%module(directors="1") pyusbdux

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

%feature("director") Callback;

%include "pyusbdux.h"
