#ifndef _AOC_Y2022_D18_H_INCLUDED
#define _AOC_Y2022_D18_H_INCLUDED
#include <limits.h>

#include "common.h"

PyObject *_AoC_y2022_d18_copy_point(PyObject *p1) {
  Py_ssize_t n = PyTuple_Size(p1);
  PyObject *p2 = PyTuple_New(n);
  for (Py_ssize_t i = 0; i < n; ++i) {
    PyTuple_SET_ITEM(p2, i, Py_NewRef(PyTuple_GetItem(p1, i)));
  }
  return p2;
}

PyObject *_AoC_y2022_d18_make_adjacent_points(PyObject *point) {
  PyObject *adjacent = PyList_New(0);
  for (Py_ssize_t step = -1; step <= 1; step += 2) {
    for (Py_ssize_t dim = 0; dim < PyTuple_Size(point); ++dim) {
      PyObject *adj = _AoC_y2022_d18_copy_point(point);
      long value = PyLong_AsLong(PyTuple_GetItem(point, dim));
      PyTuple_SetItem(adj, dim, PyLong_FromLong(value + step));
      PyList_Append(adjacent, adj);
    }
  }
  return adjacent;
}

Py_ssize_t _AoC_y2022_d18_count_3d_surface(PyObject *point,
                                           PyObject *points,
                                           PyObject *visited,
                                           PyObject *lo_bound,
                                           PyObject *hi_bound) {
  if (PySet_Contains(visited, point)) {
    return 0;
  }
  PySet_Add(visited, point);
  for (Py_ssize_t i = 0; i < PyTuple_Size(point); ++i) {
    PyObject *comp = PyTuple_GetItem(point, i);
    PyObject *lo = PyTuple_GetItem(lo_bound, i);
    PyObject *hi = PyTuple_GetItem(hi_bound, i);
    if (1 == PyObject_RichCompareBool(comp, lo, Py_LT)) {
      return 0;
    }
    if (1 == PyObject_RichCompareBool(comp, hi, Py_GT)) {
      return 0;
    }
  }
  Py_ssize_t area = 0;
  PyObject *adjacent = _AoC_y2022_d18_make_adjacent_points(point);
  for (Py_ssize_t i_adj = 0; i_adj < PyList_Size(adjacent); ++i_adj) {
    PyObject *adj_point = PyList_GetItem(adjacent, i_adj);
    if (PySet_Contains(points, adj_point) == 1) {
      ++area;
    } else {
      area += _AoC_y2022_d18_count_3d_surface(adj_point,
                                              points,
                                              visited,
                                              lo_bound,
                                              hi_bound);
    }
  }
  Py_DECREF(adjacent);
  return area;
}

PyObject *_AoC_y2022_d18_make_point(const long point[static 3]) {
  PyObject *p = PyTuple_New(3);
  for (Py_ssize_t dim = 0; dim < 3; ++dim) {
    PyTuple_SET_ITEM(p, dim, PyLong_FromLong(point[dim]));
  }
  return p;
}

PyObject *AoC_y2022_d18(PyObject *unicode_input) {
  PyObject *solution = 0;
  PyObject *points = PySet_New(0);
  PyObject *visited = PySet_New(0);
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  if (!points || !visited || !lines) {
    goto done;
  }

  long lo_bound[3] = {LONG_MAX};
  long hi_bound[3] = {LONG_MIN};
  Py_ssize_t num_points = PyList_Size(lines);
  Py_ssize_t num_sides = 0;
  for (Py_ssize_t i = 0; i < num_points; ++i) {
    PyObject *line_parts = AoC_unicode_split(PyList_GetItem(lines, i), ",", 2);
    if (!line_parts) {
      goto done;
    }
    PyObject *point = PyTuple_New(3);
    for (Py_ssize_t dim = 0; dim < PyTuple_Size(point); ++dim) {
      PyObject *value_str = PyList_GetItem(line_parts, dim);
      PyObject *value = PyLong_FromUnicodeObject(value_str, 10);
      PyTuple_SET_ITEM(point, dim, value);
      lo_bound[dim] = Py_MIN(lo_bound[dim], PyLong_AsLong(value) - 1);
      hi_bound[dim] = Py_MAX(hi_bound[dim], PyLong_AsLong(value) + 1);
    }
    num_sides += 6;
    PyObject *adjacent = _AoC_y2022_d18_make_adjacent_points(point);
    for (Py_ssize_t i_adj = 0; i_adj < PyList_Size(adjacent); ++i_adj) {
      PyObject *adj_point = PyList_GetItem(adjacent, i_adj);
      num_sides -= 2 * (PySet_Contains(points, adj_point) == 1);
    }
    Py_DECREF(adjacent);
    Py_DECREF(line_parts);
    if (PySet_Add(points, point) < 0) {
      goto done;
    }
  }
  PyObject *lo_bound_py = _AoC_y2022_d18_make_point(lo_bound);
  PyObject *hi_bound_py = _AoC_y2022_d18_make_point(hi_bound);
  PyObject *begin = Py_NewRef(lo_bound_py);
  Py_ssize_t surface_area = _AoC_y2022_d18_count_3d_surface(begin,
                                                            points,
                                                            visited,
                                                            lo_bound_py,
                                                            hi_bound_py);
  Py_DECREF(begin);
  Py_DECREF(lo_bound_py);
  Py_DECREF(hi_bound_py);
  solution = PyUnicode_FromFormat("%zd %zd", num_sides, surface_area);

done:
  Py_XDECREF(points);
  Py_XDECREF(visited);
  Py_XDECREF(lines);
  return solution;
}

#endif  // _AOC_Y2022_D18_H_INCLUDED
