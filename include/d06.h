#ifndef _AOC_Y2022_D06_H_INCLUDED
#define _AOC_Y2022_D06_H_INCLUDED
#include "common.h"

PyObject *AoC_y2022_d06(PyObject *unicode_input) {
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  if (!lines) {
    goto error;
  }

  Py_ssize_t num_lines = PyList_Size(lines);
  long part1 = 0;
  long part2 = 0;
  for (Py_ssize_t i_line = 0; i_line < num_lines; ++i_line) {
    PyObject *line = PyList_GetItem(lines, i_line);
    for (Py_ssize_t line_pos = 0; line_pos < PyUnicode_GET_LENGTH(line); ++line_pos) {
      if (part1 == 0) {
        PyObject *chunk = PyUnicode_Substring(line, line_pos, line_pos + 4);
        PyObject *unique = PySet_New(chunk);
        Py_DECREF(chunk);
        if (PySet_Size(unique) == 4) {
          part1 += line_pos + 4;
          Py_DECREF(unique);
        }
        Py_DECREF(unique);
      }
      if (part2 == 0) {
        PyObject *chunk = PyUnicode_Substring(line, line_pos, line_pos + 14);
        PyObject *unique = PySet_New(chunk);
        Py_DECREF(chunk);
        if (PySet_Size(unique) == 14) {
          part2 += line_pos + 14;
          Py_DECREF(unique);
        }
        Py_DECREF(unique);
      }
    }
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

#endif  // _AOC_Y2022_D06_H_INCLUDED
