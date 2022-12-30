#ifndef _AOC_Y2022_D15_H_INCLUDED
#define _AOC_Y2022_D15_H_INCLUDED
#include <limits.h>
#include <math.h>
#include <stdio.h>

#include "common.h"

PyObject *_AoC_y2022_d15_make_point(const long long x, const long long y) {
  PyObject *p = PyTuple_New(2);
  PyTuple_SET_ITEM(p, 0, PyLong_FromLongLong(x));
  PyTuple_SET_ITEM(p, 1, PyLong_FromLongLong(y));
  return p;
}

PyObject *_AoC_y2022_d15_L1_distance(PyObject *const lhs, PyObject *const rhs) {
  PyObject *x0 = PyTuple_GetItem(lhs, 0);
  PyObject *y0 = PyTuple_GetItem(lhs, 1);
  PyObject *x1 = PyTuple_GetItem(rhs, 0);
  PyObject *y1 = PyTuple_GetItem(rhs, 1);
  PyObject *dx = PyNumber_Subtract(x0, x1);
  PyObject *dy = PyNumber_Subtract(y0, y1);
  Py_SETREF(dx, PyNumber_Absolute(dx));
  Py_SETREF(dy, PyNumber_Absolute(dy));
  PyObject *const distance = PyNumber_Add(dx, dy);
  Py_DECREF(dx);
  Py_DECREF(dy);
  return distance;
}

int AoC_y2022_d15_is_inside_sensor_range(PyObject *const sensor,
                                         PyObject *const pos) {
  PyObject *xy_sensor = PyTuple_GetItem(sensor, 0);
  PyObject *distance = _AoC_y2022_d15_L1_distance(pos, xy_sensor);
  PyObject *range = PyTuple_GetItem(sensor, 2);
  int yes = 1 == PyObject_RichCompareBool(distance, range, Py_LE);
  Py_DECREF(distance);
  return yes;
}

long long _AoC_y2022_d15_find_next_empty_x(PyObject *const sensors,
                                           const long long y,
                                           const long long x_begin,
                                           const long long x_end) {
  long long x = x_begin;
  long long x_jump = 1;
  for (; x_jump && x <= x_end; x += x_jump) {
    PyObject *pos = _AoC_y2022_d15_make_point(x, y);
    x_jump = 0;
    for (Py_ssize_t i = 0; x_jump == 0 && i < PyList_Size(sensors); ++i) {
      PyObject *sensor = PyList_GetItem(sensors, i);
      if (!AoC_y2022_d15_is_inside_sensor_range(sensor, pos)) {
        continue;
      }
      PyObject *one = PyLong_FromLong(1);
      PyObject *xy_sensor = PyTuple_GetItem(sensor, 0);
      PyObject *range = PyTuple_GetItem(sensor, 2);
      PyObject *x_delta = PyNumber_Subtract(PyTuple_GetItem(xy_sensor, 0),
                                            PyTuple_GetItem(pos, 0));
      PyObject *y_delta = PyNumber_Subtract(PyTuple_GetItem(xy_sensor, 1),
                                            PyTuple_GetItem(pos, 1));
      Py_SETREF(y_delta, PyNumber_Absolute(y_delta));
      PyObject *x_jump_py = PyNumber_Subtract(x_delta, y_delta);
      Py_SETREF(x_jump_py, PyNumber_Add(x_jump_py, range));
      Py_SETREF(x_jump_py, PyNumber_Add(x_jump_py, one));
      x_jump = PyLong_AsLongLong(x_jump_py);
      Py_DECREF(x_jump_py);
      Py_DECREF(x_delta);
      Py_DECREF(y_delta);
      Py_DECREF(one);
    }
    Py_DECREF(pos);
  }
  return x;
}

PyObject *_AoC_y2022_d15_abs_diff(PyObject *const x, PyObject *const y) {
  PyObject *res = PyNumber_Subtract(x, y);
  Py_SETREF(res, PyNumber_Absolute(res));
  return res;
}

PyObject *_AoC_y2022_d15_find_part1(PyObject *const sensors) {
  PyObject *const y = PyLong_FromLong(2000000);

  long long x_begin = LONG_MAX;
  long long x_end = LONG_MIN;
  for (Py_ssize_t i = 0; i < PyList_Size(sensors); ++i) {
    PyObject *sensor = PyList_GetItem(sensors, i);
    PyObject *xy_sensor = PyTuple_GetItem(sensor, 0);
    PyObject *range = PyTuple_GetItem(sensor, 2);
    PyObject *x_s = PyTuple_GetItem(xy_sensor, 0);
    PyObject *y_s = PyTuple_GetItem(xy_sensor, 1);
    PyObject *dy = _AoC_y2022_d15_abs_diff(y_s, y);
    if (PyObject_RichCompareBool(dy, range, Py_GT) == 1) {
      continue;
    }
    PyObject *dx = PyNumber_Subtract(range, dy);
    PyObject *x_min = PyNumber_Subtract(x_s, dx);
    PyObject *x_max = PyNumber_Add(x_s, dx);
    x_begin = Py_MIN(x_begin, PyLong_AsLongLong(x_min));
    x_end = Py_MAX(x_end, PyLong_AsLongLong(x_max));
    Py_DECREF(dy);
    Py_DECREF(dx);
    Py_DECREF(x_min);
    Py_DECREF(x_max);
  }

  const long long x = _AoC_y2022_d15_find_next_empty_x(sensors,
                                                       PyLong_AsLongLong(y),
                                                       x_begin,
                                                       x_end);
  return PyLong_FromLongLong(llabs(x - x_begin - 1));
}

PyObject *_AoC_y2022_d15_find_part2(PyObject *const sensors) {
  const long long limit = 4000000;
  PyObject *hidden_beacon = 0;
  for (long long y = 0; !hidden_beacon && y <= limit; ++y) {
    const long long x = _AoC_y2022_d15_find_next_empty_x(sensors, y, 0, limit);
    if (x < limit) {
      hidden_beacon = _AoC_y2022_d15_make_point(x, y);
    }
  }
  if (!hidden_beacon) {
    PyErr_Format(PyExc_RuntimeError, "failed finding the hidden beacon");
    return 0;
  }
  PyObject *part2 = PyLong_FromLong(4000000);
  Py_SETREF(part2, PyNumber_Multiply(part2, PyTuple_GetItem(hidden_beacon, 0)));
  Py_SETREF(part2, PyNumber_Add(part2, PyTuple_GetItem(hidden_beacon, 1)));
  Py_DECREF(hidden_beacon);
  return part2;
}

PyObject *_AoC_y2022_d15_parse_xy(PyObject *const str) {
  PyObject *x = AoC_unicode_split(str, "x=", 1);
  Py_SETREF(x, Py_NewRef(PyList_GetItem(x, 1)));
  Py_SETREF(x, AoC_unicode_split(x, ", ", 1));
  Py_SETREF(x, Py_NewRef(PyList_GetItem(x, 0)));
  Py_SETREF(x, PyLong_FromUnicodeObject(x, 10));
  PyObject *y = AoC_unicode_split(str, "y=", 1);
  Py_SETREF(y, Py_NewRef(PyList_GetItem(y, 1)));
  Py_SETREF(y, PyLong_FromUnicodeObject(y, 10));
  PyObject *xy = PyTuple_New(2);
  PyTuple_SET_ITEM(xy, 0, x);
  PyTuple_SET_ITEM(xy, 1, y);
  return xy;
}

PyObject *_AoC_y2022_d15_parse_sensors(PyObject *const lines) {
  const Py_ssize_t num_sensors = PyList_Size(lines);
  PyObject *sensors = PyList_New(num_sensors);

  for (Py_ssize_t i = 0; i < num_sensors; ++i) {
    PyObject *line = PyList_GetItem(lines, i);

    PyObject *line_parts = AoC_unicode_split(line, ": ", 1);
    PyObject *lhs = PyList_GetItem(line_parts, 0);
    PyObject *xy_sensor = _AoC_y2022_d15_parse_xy(lhs);
    PyObject *rhs = PyList_GetItem(line_parts, 1);
    PyObject *xy_beacon = _AoC_y2022_d15_parse_xy(rhs);
    PyObject *range = _AoC_y2022_d15_L1_distance(xy_sensor, xy_beacon);

    PyObject *sensor = PyTuple_New(3);
    PyTuple_SET_ITEM(sensor, 0, xy_sensor);
    PyTuple_SET_ITEM(sensor, 1, xy_beacon);
    PyTuple_SET_ITEM(sensor, 2, range);
    PyList_SET_ITEM(sensors, i, sensor);

    Py_DECREF(line_parts);
  }

  return sensors;
}

PyObject *AoC_y2022_d15(PyObject *const unicode_input) {
  PyObject *solution = 0;
  PyObject *sensors = 0;
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  if (!lines) {
    goto done;
  }

  sensors = _AoC_y2022_d15_parse_sensors(lines);
  if (!sensors) {
    goto done;
  }

  PyObject *part1 = _AoC_y2022_d15_find_part1(sensors);
  PyObject *part2 = _AoC_y2022_d15_find_part2(sensors);
  solution = PyUnicode_FromFormat("%S %S", part1, part2);
  Py_DECREF(part1);
  Py_DECREF(part2);

done:
  Py_XDECREF(lines);
  Py_XDECREF(sensors);
  return solution;
}

#endif  // _AOC_Y2022_D15_H_INCLUDED
