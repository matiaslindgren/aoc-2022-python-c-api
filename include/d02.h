#ifndef _AOC_Y2022_D02_H_INCLUDED
#define _AOC_Y2022_D02_H_INCLUDED
#include "common.h"

PyObject *AoC_y2022_d02(PyObject *unicode_input) {
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  if (!lines) {
    PyErr_Format(PyExc_RuntimeError, "could not split input to lines");
    goto error;
  }
  Py_ssize_t num_lines = PyList_Size(lines);
  long ans_a = 0;
  long ans_b = 0;
  for (Py_ssize_t i = 0; i < num_lines; ++i) {
    PyObject *line = PyList_GetItem(lines, i);
    Py_INCREF(line);
    PyObject *parts = AoC_unicode_split(line, " ", 1);
    Py_DECREF(line);
    PyObject *lhs = PyList_GetItem(parts, 0);
    Py_INCREF(lhs);
    PyObject *rhs = PyList_GetItem(parts, 1);
    Py_INCREF(rhs);
    Py_DECREF(parts);
    int lhs_rock = PyUnicode_CompareWithASCIIString(lhs, "A") == 0;
    int lhs_paper = PyUnicode_CompareWithASCIIString(lhs, "B") == 0;
    int lhs_scissors = PyUnicode_CompareWithASCIIString(lhs, "C") == 0;
    int rhs_x = PyUnicode_CompareWithASCIIString(rhs, "X") == 0;
    int rhs_y = PyUnicode_CompareWithASCIIString(rhs, "Y") == 0;
    int rhs_z = PyUnicode_CompareWithASCIIString(rhs, "Z") == 0;
    Py_DECREF(lhs);
    Py_DECREF(rhs);
    ans_a += 3 * ((lhs_rock && rhs_x) || (lhs_paper && rhs_y) || (lhs_scissors && rhs_z));
    ans_a += 6 * ((lhs_rock && rhs_y) || (lhs_paper && rhs_z) || (lhs_scissors && rhs_x));
    ans_a += rhs_x + 2 * rhs_y + 3 * rhs_z;
    int lhs_choice = lhs_rock + 2 * lhs_paper + 3 * lhs_scissors;
    if (rhs_x) {
      ans_b += (lhs_choice - 1 + 2) % 3 + 1;
    }
    if (rhs_y) {
      ans_b += 3 + lhs_choice;
    }
    if (rhs_z) {
      ans_b += 6 + (lhs_choice - 1 + 1) % 3 + 1;
    }
  }
  Py_DECREF(lines);
  PyObject *a = PyLong_FromLong(ans_a);
  PyObject *b = PyLong_FromLong(ans_b);
  PyObject *solution = PyUnicode_FromFormat("%S %S", a, b);
  Py_DECREF(a);
  Py_DECREF(b);
  return solution;
error:
  return NULL;
}
#endif  // _AOC_Y2022_D02_H_INCLUDED
