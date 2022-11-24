#ifndef _AOC_Y2022_D04_H_INCLUDED
#define _AOC_Y2022_D04_H_INCLUDED
#define PY_SSIZE_T_CLEAN
#include "Python.h"

PyObject *AoC_y2022_d04(PyObject *unicode_input) {
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  if (lines == NULL) {
    PyErr_SetString(PyExc_RuntimeError, "Failed splitting day 4 input into lines");
    return NULL;
  }
  if (_AoC_list_contains_null(lines)) {
    PyErr_SetString(PyExc_RuntimeError, "input lines contains NULLs");
    Py_DECREF(lines);
    return NULL;
  }

  Py_ssize_t num_lines = PyList_Size(lines);
  for (Py_ssize_t i = 0; i < num_lines; ++i) {
    PyObject *line = PyList_GetItem(lines, i);
    PyObject *sep = PyUnicode_FromString(" ");
    PyObject *parts = PyUnicode_Split(line, sep, 2);
    Py_DECREF(sep);
    PySys_FormatStdout("%zd:\n", i);
    _AoC_debug_dump_lines(parts);
    printf("\n");
    Py_DECREF(parts);
  }

  Py_DECREF(lines);
  return Py_BuildValue("U", "TODO answer");
}
#endif  // _AOC_Y2022_D04_H_INCLUDED
