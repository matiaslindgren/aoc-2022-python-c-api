#ifndef _AOC_Y2022_D04_H_INCLUDED
#define _AOC_Y2022_D04_H_INCLUDED
#include <stdio.h>

#include "common.h"

PyObject *AoC_y2022_d04(PyObject *unicode_input) {
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  if (!lines) {
    PyErr_Format(PyExc_RuntimeError, "could not split input into lines");
    goto error;
  }

  Py_ssize_t num_lines = PyList_Size(lines);
  long part1 = 0;
  long part2 = 0;
  for (Py_ssize_t i = 0; i < num_lines; ++i) {
    PyObject *line = PyUnicode_AsASCIIString(PyList_GetItem(lines, i));
    if (!line) {
      goto error;
    }
    size_t lhs_a, lhs_b, rhs_a, rhs_b;
    sscanf(PyBytes_AsString(line), "%zd-%zd,%zd-%zd\n", &lhs_a, &lhs_b, &rhs_a, &rhs_b);
    part1 += (lhs_a <= rhs_a && rhs_b <= lhs_b) || (rhs_a <= lhs_a && lhs_b <= rhs_b);
    part2 += (lhs_a <= rhs_a && rhs_a <= lhs_b) || (rhs_a <= lhs_a && lhs_a <= rhs_b);
    Py_DECREF(line);
  }
  Py_DECREF(lines);

  PyObject *part1_py = PyLong_FromLong(part1);
  PyObject *part2_py = PyLong_FromLong(part2);
  PyObject *solution = PyUnicode_FromFormat("%S %S", part1_py, part2_py);
  Py_DECREF(part1_py);
  Py_DECREF(part2_py);
  return solution;

error:
  return NULL;
}

#endif  // _AOC_Y2022_D04_H_INCLUDED
