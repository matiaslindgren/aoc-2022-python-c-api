#ifndef _AOC_Y2022_D06_H_INCLUDED
#define _AOC_Y2022_D06_H_INCLUDED
#include "common.h"

Py_ssize_t _AoC_y2022_d06_find_distinct(PyObject *input, Py_ssize_t msg_len) {
  if (PyUnicode_READY(input) < 0) {
    return -1;
  }
  Py_ssize_t input_len = PyUnicode_GetLength(input);
  Py_ssize_t rhs = msg_len;
  for (; rhs < input_len; ++rhs) {
    PyObject *window = PyUnicode_Substring(input, rhs - msg_len, rhs);
    PyObject *window_uniq = PySet_New(window);
    Py_DECREF(window);
    Py_ssize_t uniq_len = PySet_Size(window_uniq);
    Py_DECREF(window_uniq);
    if (uniq_len == msg_len) {
      break;
    }
  }
  return rhs;
}

PyObject *AoC_y2022_d06(PyObject *unicode_input) {
  Py_ssize_t part1 = _AoC_y2022_d06_find_distinct(unicode_input, 4);
  Py_ssize_t part2 = _AoC_y2022_d06_find_distinct(unicode_input, 14);
  return PyUnicode_FromFormat("%zd %zd", part1, part2);
}

#endif  // _AOC_Y2022_D06_H_INCLUDED
