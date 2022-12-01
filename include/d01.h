#ifndef _AOC_Y2022_D01_H_INCLUDED
#define _AOC_Y2022_D01_H_INCLUDED
#include "common.h"

PyObject *AoC_y2022_d01(PyObject *unicode_input) {
  PyObject *calorie_groups = AoC_unicode_split(unicode_input, "\n\n", -1);
  Py_ssize_t num_groups = PyList_Size(calorie_groups);
  PyObject *calorie_sums = PyList_New(num_groups);
  for (Py_ssize_t group_i = 0; group_i < num_groups; ++group_i) {
    PyObject *calories_str = PyList_GetItem(calorie_groups, group_i);
    PyObject *calories = AoC_unicode_split(calories_str, "\n", -1);
    Py_ssize_t n = PyList_Size(calories);
    PyObject *sum = PyLong_FromLong(0L);
    if (!sum) {
      goto error;
    }
    for (Py_ssize_t i = 0; i < n; ++i) {
      PyObject *x_str = PyList_GetItem(calories, i);
      if (PyUnicode_GetLength(x_str) > 0) {
        PyObject *x = PyLong_FromUnicodeObject(x_str, 10);
        PyObject *y = PyNumber_Add(sum, x);
        Py_DECREF(sum);
        sum = y;
        Py_DECREF(x);
      }
    }
    Py_DECREF(calories);
    if (PyList_SetItem(calorie_sums, group_i, sum) == -1) {
      goto error;
    }
  }
  Py_DECREF(calorie_groups);
  _AoC_debug_dump_lines(calorie_sums);
  printf("\n");
  if (PyList_Sort(calorie_sums) == -1) {
    goto error;
  }
  _AoC_debug_dump_lines(calorie_sums);
  printf("\n");
  if (PyList_Reverse(calorie_sums) == -1) {
    goto error;
  }
  _AoC_debug_dump_lines(calorie_sums);
  PyObject *total = PyLong_FromLong(0L);
  for (Py_ssize_t i = 0; i < 3; ++i) {
    PyObject *x = PyNumber_Add(total, PyList_GetItem(calorie_sums, i));
    Py_DECREF(total);
    total = x;
  }
  Py_DECREF(calorie_sums);
  return total;

error:
  PyErr_SetString(PyExc_RuntimeError, "failure");
  return NULL;
}
#endif  // _AOC_Y2022_D01_H_INCLUDED
