#ifndef _AOC_Y2022_D01_H_INCLUDED
#define _AOC_Y2022_D01_H_INCLUDED
#include "common.h"

PyObject *AoC_y2022_d01(PyObject *unicode_input) {
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  if (!lines) {
    return NULL;
  }

  PyObject *tail = PyUnicode_FromString("");
  if (PyList_Append(lines, tail) < 0) {
    Py_DECREF(tail);
    Py_DECREF(lines);
    return NULL;
  }
  Py_DECREF(tail);

  PyObject *solution = NULL;
  PyObject *top3 = PyList_New(0);
  PyObject *current_sum = PyLong_FromLong(0L);

  Py_ssize_t num_lines = PyList_Size(lines);
  for (Py_ssize_t i = 0; i < num_lines; ++i) {
    PyObject *line = PyList_GetItem(lines, i);
    if (PyUnicode_GetLength(line) == 0) {
      if (PyList_Append(top3, current_sum) < 0) {
        goto done;
      }
      Py_DECREF(current_sum);
      current_sum = PyLong_FromLong(0L);
      if (PyList_Sort(top3) == -1) {
        goto done;
      }
      if (PyList_Size(top3) == 4 && PySequence_DelItem(top3, 0) < 0) {
        goto done;
      }
    } else {
      PyObject *calories = PyLong_FromUnicodeObject(line, 10);
      PyObject *tmp = PyNumber_Add(current_sum, calories);
      Py_DECREF(calories);
      Py_SETREF(current_sum, tmp);
    }
  }

  PyObject *part1 = PyList_GetItem(top3, 2);
  PyObject *part2 = PyLong_FromLong(0L);
  for (Py_ssize_t i = 0; i < 3; ++i) {
    PyObject *cal = PyList_GetItem(top3, i);
    PyObject *tmp = PyNumber_Add(part2, cal);
    Py_SETREF(part2, tmp);
  }
  solution = PyUnicode_FromFormat("%S %S", part1, part2);
  Py_DECREF(part1);
  Py_DECREF(part2);

done:
  Py_DECREF(current_sum);
  Py_DECREF(top3);
  Py_DECREF(lines);
  return solution;
}

#endif  // _AOC_Y2022_D01_H_INCLUDED
