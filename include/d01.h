#ifndef _AOC_Y2022_D01_H_INCLUDED
#define _AOC_Y2022_D01_H_INCLUDED
#include "common.h"

PyObject *AoC_y2022_d01(PyObject *unicode_input) {
  PyObject *calorie_groups = AoC_unicode_split_sections(unicode_input);
  Py_ssize_t num_groups = PyList_Size(calorie_groups);
  PyObject *calorie_sums = PyList_New(num_groups);
  for (Py_ssize_t group_i = 0; group_i < num_groups; ++group_i) {
    PyObject *calories_str = PyList_GetItem(calorie_groups, group_i);
    PyObject *calories = AoC_unicode_split(calories_str, "\n", -1);
    Py_ssize_t num_calories = PyList_Size(calories);
    PyObject *sum = PyLong_FromLong(0L);
    if (!sum) {
      Py_DECREF(calories);
      goto error;
    }
    for (Py_ssize_t i = 0; i < num_calories; ++i) {
      PyObject *cal_str = PyList_GetItem(calories, i);
      if (PyUnicode_GetLength(cal_str) > 0) {
        PyObject *cal = PyLong_FromUnicodeObject(cal_str, 10);
        PyObject *res = PyNumber_Add(sum, cal);
        Py_DECREF(cal);
        Py_SETREF(sum, res);
      }
    }
    Py_DECREF(calories);
    if (PyList_SetItem(calorie_sums, group_i, sum) == -1) {
      goto error;
    }
  }
  Py_DECREF(calorie_groups);

  if (PyList_Sort(calorie_sums) == -1) {
    goto error;
  }
  if (PyList_Reverse(calorie_sums) == -1) {
    goto error;
  }

  PyObject *top1 = PyList_GetItem(calorie_sums, 0);
  PyObject *top3 = PyLong_FromLong(0L);
  for (Py_ssize_t i = 0; i < 3; ++i) {
    PyObject *res = PyNumber_Add(top3, PyList_GetItem(calorie_sums, i));
    Py_SETREF(top3, res);
  }
  Py_DECREF(calorie_sums);
  PyObject *solution = PyUnicode_FromFormat("%S %S", top1, top3);
  Py_DECREF(top1);
  Py_DECREF(top3);
  return solution;

error:
  Py_DECREF(calorie_sums);
  return NULL;
}
#endif  // _AOC_Y2022_D01_H_INCLUDED
