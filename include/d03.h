#ifndef _AOC_Y2022_D03_H_INCLUDED
#define _AOC_Y2022_D03_H_INCLUDED
#include "common.h"

int _AoC_y2022_d03_item_priority(Py_UCS4 ch) {
  return Py_UNICODE_ISLOWER(ch) * (ch - 'a' + 1) +
         Py_UNICODE_ISUPPER(ch) * (ch - 'A' + 27);
}

PyObject *AoC_y2022_d03(PyObject *unicode_input) {
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  if (!lines) {
    PyErr_Format(PyExc_RuntimeError, "could not split input into lines");
    goto error;
  }

  Py_ssize_t num_lines = PyList_Size(lines);
  long part1 = 0;
  for (Py_ssize_t i = 0; i < num_lines; ++i) {
    PyObject *line = PyList_GetItem(lines, i);
    Py_INCREF(line);
    Py_ssize_t n = PyUnicode_GetLength(line);
    if (n == 0 || n % 2) {
      Py_DECREF(line);
      Py_DECREF(lines);
      PyErr_Format(PyExc_RuntimeError,
                   "line %zd length %zd is not divisible by 2",
                   i,
                   n);
      goto error;
    }
    PyObject *lhs_str = PyUnicode_Substring(line, 0, n / 2);
    PyObject *rhs_str = PyUnicode_Substring(line, n / 2, n);
    Py_DECREF(line);
    PyObject *lhs = PySet_New(lhs_str);
    PyObject *rhs = PySet_New(rhs_str);
    Py_DECREF(lhs_str);
    Py_DECREF(rhs_str);
    PyObject *intersection = PyNumber_And(lhs, rhs);
    Py_DECREF(lhs);
    Py_DECREF(rhs);
    PyObject *common = PySet_Pop(intersection);
    Py_DECREF(intersection);
    if (!common) {
      Py_DECREF(lines);
      goto error;
    }
    Py_UCS4 ch = PyUnicode_ReadChar(common, 0);
    Py_DECREF(common);
    part1 += _AoC_y2022_d03_item_priority(ch);
  }

  long part2 = 0;
  for (Py_ssize_t i = 0; i < num_lines; i += 3) {
    PyObject *line1 = PyList_GetItem(lines, i);
    Py_INCREF(line1);
    PyObject *line2 = PyList_GetItem(lines, i + 1);
    Py_INCREF(line2);
    PyObject *line3 = PyList_GetItem(lines, i + 2);
    Py_INCREF(line3);
    PyObject *bag1 = PySet_New(line1);
    PyObject *bag2 = PySet_New(line2);
    PyObject *bag3 = PySet_New(line3);
    Py_DECREF(line1);
    Py_DECREF(line2);
    Py_DECREF(line3);
    PyObject *intersection12_ = PyNumber_And(bag1, bag2);
    PyObject *intersection1_3 = PyNumber_And(bag1, bag3);
    Py_DECREF(bag1);
    Py_DECREF(bag2);
    Py_DECREF(bag3);
    PyObject *intersection123 = PyNumber_And(intersection12_, intersection1_3);
    PyObject *common = PySet_Pop(intersection123);
    Py_DECREF(intersection12_);
    Py_DECREF(intersection1_3);
    Py_DECREF(intersection123);
    if (!common) {
      Py_DECREF(lines);
      goto error;
    }
    Py_UCS4 ch = PyUnicode_ReadChar(common, 0);
    Py_DECREF(common);
    part2 += _AoC_y2022_d03_item_priority(ch);
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
#endif  // _AOC_Y2022_D03_H_INCLUDED
