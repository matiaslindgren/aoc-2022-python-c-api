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
  PyObject *part1 = PyLong_FromLong(0L);
  PyObject *part2 = PyLong_FromLong(0L);
  for (Py_ssize_t i = 0; i < num_lines; ++i) {
    PyObject *line = PyUnicode_AsASCIIString(PyList_GetItem(lines, i));
    if (!line) {
      goto error;
    }
    long lhs_a, lhs_b, rhs_a, rhs_b;
    sscanf(PyBytes_AsString(line),
           "%ld-%ld,%ld-%ld\n",
           &lhs_a,
           &lhs_b,
           &rhs_a,
           &rhs_b);
    int full_contained = (lhs_a <= rhs_a && rhs_b <= lhs_b) ||
                         (rhs_a <= lhs_a && lhs_b <= rhs_b);
    int part_contained = (lhs_a <= rhs_a && rhs_a <= lhs_b) ||
                         (rhs_a <= lhs_a && lhs_a <= rhs_b);
    part1 = AoC_PyLong_Add(part1, full_contained);
    part2 = AoC_PyLong_Add(part2, part_contained);
    Py_DECREF(line);
  }
  Py_DECREF(lines);

  PyObject *solution = PyUnicode_FromFormat("%S %S", part1, part2);
  Py_DECREF(part1);
  Py_DECREF(part2);
  return solution;

error:
  return NULL;
}

#endif  // _AOC_Y2022_D04_H_INCLUDED
