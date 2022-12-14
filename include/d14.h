#ifndef _AOC_Y2022_D14_H_INCLUDED
#define _AOC_Y2022_D14_H_INCLUDED
#include "common.h"

PyObject *_AoC_y2022_d14_make_point(long x, long y) {
  PyObject *p = PyTuple_New(2);
  PyTuple_SET_ITEM(p, 0, PyLong_FromLong(x));
  PyTuple_SET_ITEM(p, 1, PyLong_FromLong(y));
  return p;
}

PyObject *AoC_y2022_d14(PyObject *unicode_input) {
  PyObject *solution = 0;
  PyObject *obstacles = PySet_New(0);
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  if (!lines || !obstacles) {
    goto done;
  }

  for (Py_ssize_t i_line = 0; i_line < PyList_Size(lines); ++i_line) {
    PyObject *line = PyList_GetItem(lines, i_line);
    PyObject *str_points = AoC_unicode_split(line, " -> ", -1);
    PyObject *points = PyList_New(PyList_Size(str_points));

    for (Py_ssize_t i_point = 0; i_point < PyList_Size(str_points); ++i_point) {
      PyObject *point = PyList_GetItem(str_points, i_point);
      PyObject *xy = AoC_unicode_split(point, ",", 1);
      PyObject *x = PyLong_FromUnicodeObject(PyList_GetItem(xy, 0), 10);
      PyObject *y = PyLong_FromUnicodeObject(PyList_GetItem(xy, 1), 10);
      PyObject *pos = PyTuple_New(2);
      PyTuple_SET_ITEM(pos, 0, x);
      PyTuple_SET_ITEM(pos, 1, y);
      PyList_SET_ITEM(points, i_point, pos);
      Py_DECREF(xy);
    }

    Py_DECREF(str_points);

    PyObject *rock_begin = PyList_GetItem(points, 0);
    for (Py_ssize_t i_point = 1; i_point < PyList_Size(str_points); ++i_point) {
      PyObject *rock_end = PyList_GetItem(points, i_point);
      long x0 = PyLong_AsLong(PyTuple_GetItem(rock_begin, 0));
      long y0 = PyLong_AsLong(PyTuple_GetItem(rock_begin, 1));
      long x1 = PyLong_AsLong(PyTuple_GetItem(rock_end, 0));
      long y1 = PyLong_AsLong(PyTuple_GetItem(rock_end, 1));
      for (long x = Py_MIN(x0, x1); x <= Py_MAX(x0, x1); ++x) {
        for (long y = Py_MIN(y0, y1); y <= Py_MAX(y0, y1); ++y) {
          PySet_Add(obstacles, _AoC_y2022_d14_make_point(x, y));
        }
      }
      rock_begin = rock_end;
    }

    Py_DECREF(points);
  }

  long y_max_rock = 0;
  {
    PyObject *obstacles_iter = PyObject_GetIter(obstacles);
    PyObject *rock;
    while ((rock = PyIter_Next(obstacles_iter))) {
      y_max_rock = Py_MAX(y_max_rock, PyLong_AsLong(PyTuple_GetItem(rock, 1)));
      Py_DECREF(rock);
    }
  }

  PyObject *sand = _AoC_y2022_d14_make_point(500, 0);
  long y_max_sand = 0;
  long part1 = 0;
  long part2 = 0;
  while (!PySet_Contains(obstacles, sand)) {
    long x_sand = PyLong_AsLong(PyTuple_GetItem(sand, 0));
    long y_sand = PyLong_AsLong(PyTuple_GetItem(sand, 1));
    y_max_sand = Py_MAX(y_max_sand, y_sand);
    if (y_sand < y_max_rock + 1) {
      PyObject *next = _AoC_y2022_d14_make_point(x_sand, y_sand + 1);
      if (!PySet_Contains(obstacles, next)) {
        Py_SETREF(sand, next);
        continue;
      }
      Py_SETREF(next, _AoC_y2022_d14_make_point(x_sand - 1, y_sand + 1));
      if (!PySet_Contains(obstacles, next)) {
        Py_SETREF(sand, next);
        continue;
      }
      Py_SETREF(next, _AoC_y2022_d14_make_point(x_sand + 1, y_sand + 1));
      if (!PySet_Contains(obstacles, next)) {
        Py_SETREF(sand, next);
        continue;
      }
      Py_DECREF(next);
    }
    PySet_Add(obstacles, sand);
    part1 += y_max_sand < y_max_rock;
    ++part2;
    Py_SETREF(sand, _AoC_y2022_d14_make_point(500, 0));
  }

  PyObject *part1_py = PyLong_FromLong(part1);
  PyObject *part2_py = PyLong_FromLong(part2);
  solution = PyUnicode_FromFormat("%S %S", part1_py, part2_py);
  Py_DECREF(part1_py);
  Py_DECREF(part2_py);

done:
  Py_XDECREF(obstacles);
  Py_XDECREF(lines);
  return solution;
}

#endif  // _AOC_Y2022_D14_H_INCLUDED
