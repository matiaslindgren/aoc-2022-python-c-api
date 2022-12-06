#ifndef _AOC_Y2022_D01_H_INCLUDED
#define _AOC_Y2022_D01_H_INCLUDED
#include "common.h"

PyObject *AoC_y2022_d01(PyObject *unicode_input) {
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  if (!lines) {
    return NULL;
  }

  PyObject *solution = NULL;
  PyObject *sums = PyList_New(0);
  PyObject *sum = PyLong_FromLong(0L);

  for (Py_ssize_t i = 0; i < PyList_Size(lines); ++i) {
    PyObject *line = PyList_GetItem(lines, i);
    if (PyUnicode_GET_LENGTH(line) == 0) {
      if (PyList_Append(sums, sum) < 0) {
        goto done;
      }
      Py_DECREF(sum);
      sum = PyLong_FromLong(0L);
    } else {
      PyObject *calories = PyLong_FromUnicodeObject(line, 10);
      PyObject *tmp = PyNumber_Add(sum, calories);
      Py_DECREF(calories);
      Py_SETREF(sum, tmp);
    }
  }
  if (PyList_Append(sums, sum) < 0) {
    goto done;
  }

  if (PyList_Sort(sums) == -1) {
    goto done;
  }
  if (PyList_Reverse(sums) == -1) {
    goto done;
  }

  PyObject *top1 = PyList_GetItem(sums, 0);
  Py_INCREF(top1);
  PyObject *top3 = PyLong_FromLong(0L);
  for (Py_ssize_t i = 0; i < 3; ++i) {
    PyObject *cal = PyList_GetItem(sums, i);
    PyObject *res = PyNumber_Add(top3, cal);
    Py_SETREF(top3, res);
  }
  solution = PyUnicode_FromFormat("%S %S", top1, top3);
  Py_DECREF(top1);
  Py_DECREF(top3);

done:
  Py_DECREF(sum);
  Py_DECREF(sums);
  Py_DECREF(lines);
  return solution;
}
#endif  // _AOC_Y2022_D01_H_INCLUDED
