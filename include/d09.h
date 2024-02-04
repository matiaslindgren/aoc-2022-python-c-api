#ifndef _AOC_Y2022_D09_H_INCLUDED
#define _AOC_Y2022_D09_H_INCLUDED
#include <stdlib.h>

#include "common.h"

long _AoC_y2022_d09_parse_num_steps(PyObject *line) {
  PyObject *num_steps_str =
      PyUnicode_Substring(line, 2, PyUnicode_GetLength(line));
  PyObject *num_steps_py = PyLong_FromUnicodeObject(num_steps_str, 10);
  Py_DECREF(num_steps_str);
  long num_steps = PyLong_AsLong(num_steps_py);
  Py_DECREF(num_steps_py);
  return num_steps;
}

PyObject *_AoC_y2022_d09_make_pair(long y, long x) {
  PyObject *p = PyTuple_New(2);
  PyTuple_SET_ITEM(p, 0, PyLong_FromLong(y));
  PyTuple_SET_ITEM(p, 1, PyLong_FromLong(x));
  return p;
}

void _AoC_y2022_d09_step(long y_h, long x_h, long *y_t, long *x_t) {
  long dy = y_h - *y_t;
  long dx = x_h - *x_t;
  int move_y = labs(dy) > 1 && labs(dx) == 0;
  int move_x = labs(dy) == 0 && labs(dx) > 1;
  int move_yx = labs(dy) + labs(dx) > 2;
  if (move_y || move_yx) {
    *y_t += dy / labs(dy);
  }
  if (move_x || move_yx) {
    *x_t += dx / labs(dx);
  }
}

typedef struct knot knot;
struct knot {
  long y;
  long x;
};

PyObject *AoC_y2022_d09(PyObject *unicode_input) {
  PyObject *solution = NULL;
  PyObject *knot1_visits = PySet_New(NULL);
  PyObject *knot9_visits = PySet_New(NULL);
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  if (!lines) {
    PyErr_Format(PyExc_RuntimeError, "could not split input into lines");
    goto error;
  }

  knot knots[10] = {0};

  for (Py_ssize_t i = 0; i < PyList_Size(lines); ++i) {
    PyObject *line = PyList_GetItem(lines, i);
    long num_steps = _AoC_y2022_d09_parse_num_steps(line);
    for (long step = 0; step < num_steps; ++step) {
      switch (PyUnicode_READ_CHAR(line, 0)) {
        case 'U': {
          --(knots[0].y);
        } break;
        case 'R': {
          ++(knots[0].x);
        } break;
        case 'D': {
          ++(knots[0].y);
        } break;
        case 'L': {
          --(knots[0].x);
        } break;
        default:
          goto error;
      }
      for (size_t k = 1; k < sizeof(knots) / sizeof(knot); ++k) {
        _AoC_y2022_d09_step(knots[k - 1].y,
                            knots[k - 1].x,
                            &(knots[k].y),
                            &(knots[k].x));
      }
      PySet_Add(knot1_visits, _AoC_y2022_d09_make_pair(knots[1].y, knots[1].x));
      PySet_Add(knot9_visits, _AoC_y2022_d09_make_pair(knots[9].y, knots[9].x));
    }
  }

  Py_ssize_t part1 = PySet_Size(knot1_visits);
  Py_ssize_t part2 = PySet_Size(knot9_visits);
  solution = PyUnicode_FromFormat("%zd %zd", part1, part2);

error:
  Py_XDECREF(lines);
  Py_XDECREF(knot1_visits);
  Py_XDECREF(knot9_visits);
  return solution;
}

#endif  // _AOC_Y2022_D09_H_INCLUDED
