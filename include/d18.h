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
    for (Py_ssize_t dim = 0; dim < 3; ++dim) {
      PyObject *adj = _AoC_y2022_d18_copy_point(point);
      long value = PyLong_AsLong(PyTuple_GetItem(point, dim));
      PyTuple_SetItem(adj, dim, PyLong_FromLong(value + step));
      PyList_Append(adjacent, adj);
    }
  }
  return adjacent;
}

Py_ssize_t _AoC_y2022_d18_count_surface_3d_flood_fill(PyObject *point,
                                                      PyObject *points,
                                                      PyObject *visited,
                                                      PyObject *lo_bound,
                                                      PyObject *hi_bound) {
  if (PySet_Contains(visited, point)) {
    return 0;
  }
  PySet_Add(visited, point);
  for (Py_ssize_t i = 0; i < 3; ++i) {
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
      area += _AoC_y2022_d18_count_surface_3d_flood_fill(adj_point,
                                                         points,
                                                         visited,
                                                         lo_bound,
                                                         hi_bound);
    }
  }
  Py_DECREF(adjacent);
  return area;
}

PyObject *_AoC_y2022_d18_make_point(const long x, const long y, const long z) {
  return PyTuple_Pack(3,
                      PyLong_FromLong(x),
                      PyLong_FromLong(y),
                      PyLong_FromLong(z));
}

Py_ssize_t _AoC_y2022_d18_count_surface(PyObject *points) {
  long xmin = LONG_MAX;
  long ymin = LONG_MAX;
  long zmin = LONG_MAX;
  long xmax = LONG_MIN;
  long ymax = LONG_MIN;
  long zmax = LONG_MIN;
  {
    PyObject *points_iter = PyObject_GetIter(points);
    PyObject *point;
    while ((point = PyIter_Next(points_iter))) {
      xmin = Py_MIN(xmin, PyLong_AsLong(PyTuple_GetItem(point, 0)) - 1);
      ymin = Py_MIN(ymin, PyLong_AsLong(PyTuple_GetItem(point, 1)) - 1);
      zmin = Py_MIN(zmin, PyLong_AsLong(PyTuple_GetItem(point, 2)) - 1);
      xmax = Py_MAX(xmax, PyLong_AsLong(PyTuple_GetItem(point, 0)) + 1);
      ymax = Py_MAX(ymax, PyLong_AsLong(PyTuple_GetItem(point, 1)) + 1);
      zmax = Py_MAX(zmax, PyLong_AsLong(PyTuple_GetItem(point, 2)) + 1);
      Py_DECREF(point);
    }
  }
  PyObject *visited = PySet_New(0);
  PyObject *lo_bound = _AoC_y2022_d18_make_point(xmin, ymin, zmin);
  PyObject *hi_bound = _AoC_y2022_d18_make_point(xmax, ymax, zmax);
  PyObject *point = Py_NewRef(lo_bound);
  Py_ssize_t surface_area =
      _AoC_y2022_d18_count_surface_3d_flood_fill(point,
                                                 points,
                                                 visited,
                                                 lo_bound,
                                                 hi_bound);
  Py_DECREF(visited);
  Py_DECREF(point);
  Py_DECREF(lo_bound);
  Py_DECREF(hi_bound);
  return surface_area;
}

PyObject *AoC_y2022_d18(PyObject *unicode_input) {
  PyObject *solution = 0;
  PyObject *const1 = PyLong_FromLong(1);
  PyObject *points = PySet_New(0);
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  if (!lines || !const1 || !points) {
    goto done;
  }

  Py_ssize_t num_points = PyList_Size(lines);
  Py_ssize_t num_sides = 0;
  for (Py_ssize_t i = 0; i < num_points; ++i) {
    PyObject *line_parts = AoC_unicode_split(PyList_GetItem(lines, i), ",", 2);
    if (!line_parts) {
      goto done;
    }
    PyObject *x = PyLong_FromUnicodeObject(PyList_GetItem(line_parts, 0), 10);
    PyObject *y = PyLong_FromUnicodeObject(PyList_GetItem(line_parts, 1), 10);
    PyObject *z = PyLong_FromUnicodeObject(PyList_GetItem(line_parts, 2), 10);
    PyObject *point = PyTuple_Pack(3, x, y, z);
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
  Py_ssize_t surface_area = _AoC_y2022_d18_count_surface(points);
  solution = PyUnicode_FromFormat("%zd %zd", num_sides, surface_area);

done:
  Py_XDECREF(const1);
  Py_XDECREF(points);
  Py_XDECREF(lines);
  return solution;
}

#endif  // _AOC_Y2022_D18_H_INCLUDED
