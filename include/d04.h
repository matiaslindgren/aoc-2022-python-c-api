#ifndef _AOC_Y2022_D04_H_INCLUDED
#define _AOC_Y2022_D04_H_INCLUDED
#define PY_SSIZE_T_CLEAN
#include "Python.h"

PyObject *aoc_y2022_d04(PyObject *unicode_input) {
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  if (lines == NULL) {
    PyErr_SetString(PyExc_RuntimeError, "Failed splitting day 4 input into lines");
    return NULL;
  }

  _aoc_common_debug_dump_lines(lines);

  Py_DECREF(lines);
  return Py_BuildValue("U", "TODO answer");
}
#endif  // _AOC_Y2022_D04_H_INCLUDED
