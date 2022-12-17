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

PyObject *_AoC_y2022_d15_L1_distance(PyObject *lhs, PyObject *rhs) {
  PyObject *x0 = PyTuple_GetItem(lhs, 0);
  PyObject *y0 = PyTuple_GetItem(lhs, 1);
  PyObject *x1 = PyTuple_GetItem(rhs, 0);
  PyObject *y1 = PyTuple_GetItem(rhs, 1);
  PyObject *dx = PyNumber_Subtract(x0, x1);
  PyObject *dy = PyNumber_Subtract(y0, y1);
  Py_SETREF(dx, PyNumber_Absolute(dx));
  Py_SETREF(dy, PyNumber_Absolute(dy));
  PyObject *distance = PyNumber_Add(dx, dy);
  Py_DECREF(dx);
  Py_DECREF(dy);
  return distance;
}

PyObject *_AoC_y2022_d15_x_minmax(PyObject *sensors, const long long y) {
  long long x_min = LLONG_MAX;
  long long x_max = LLONG_MIN;
  for (Py_ssize_t i = 0; i < PyList_Size(sensors); ++i) {
    PyObject *sensor_data = PyList_GetItem(sensors, i);
    PyObject *sensor = PyTuple_GetItem(sensor_data, 0);
    const long long x_sensor = PyLong_AsLongLong(PyTuple_GetItem(sensor, 0));
    const long long y_sensor = PyLong_AsLongLong(PyTuple_GetItem(sensor, 1));
    const long long srange = PyLong_AsLongLong(PyTuple_GetItem(sensor_data, 2));
    if (y < y_sensor - srange || y_sensor + srange < y) {
      continue;
    }
    x_min = Py_MIN(x_min, x_sensor - srange);
    x_max = Py_MAX(x_max, x_sensor + srange);
  }
  return _AoC_y2022_d15_make_point(x_min, x_max);
}

int AoC_y2022_d15_is_inside_sensor_range(PyObject *sensor_data,
                                         PyObject *point) {
  PyObject *sensor = PyTuple_GetItem(sensor_data, 0);
  PyObject *point2sensor = _AoC_y2022_d15_L1_distance(point, sensor);
  PyObject *srange = PyTuple_GetItem(sensor_data, 2);
  int yes = 1 == PyObject_RichCompareBool(point2sensor, srange, Py_LE);
  Py_DECREF(point2sensor);
  return yes;
}

PyObject *_AoC_y2022_d15_find_part1(PyObject *sensors) {
  const long long y = 2000000;
  PyObject *x_minmax = _AoC_y2022_d15_x_minmax(sensors, y);
  const long long x_min = PyLong_AsLongLong(PyTuple_GetItem(x_minmax, 0));
  const long long x_max = PyLong_AsLongLong(PyTuple_GetItem(x_minmax, 1));
  Py_DECREF(x_minmax);

  long long part1 = 0;
  for (long long x = x_min; x < x_max; ++x) {
    PyObject *point = _AoC_y2022_d15_make_point(x, y);
    for (Py_ssize_t i = 0; i < PyList_Size(sensors); ++i) {
      PyObject *sensor_data = PyList_GetItem(sensors, i);
      PyObject *beacon = PyTuple_GetItem(sensor_data, 1);
      if (1 == PyObject_RichCompareBool(point, beacon, Py_EQ)) {
        continue;
      }
      if (AoC_y2022_d15_is_inside_sensor_range(sensor_data, point)) {
        ++part1;
        break;
      }
    }
    Py_DECREF(point);
  }

  return PyLong_FromLongLong(part1);
}

PyObject *_AoC_y2022_d15_find_part2(PyObject *sensors) {
  PyObject *const1 = PyLong_FromLong(1);
  const long long limit = 4000000;
  PyObject *hidden_beacon = 0;
  for (long long y = 0; !hidden_beacon && y <= limit; ++y) {
    long long x_jump = 0;
    for (long long x = 0; !hidden_beacon && x <= limit; x += x_jump) {
      PyObject *point = _AoC_y2022_d15_make_point(x, y);
      x_jump = 0;
      for (Py_ssize_t i = 0; x_jump == 0 && i < PyList_Size(sensors); ++i) {
        PyObject *sensor_data = PyList_GetItem(sensors, i);
        if (AoC_y2022_d15_is_inside_sensor_range(sensor_data, point)) {
          PyObject *sensor = PyTuple_GetItem(sensor_data, 0);
          PyObject *srange = PyTuple_GetItem(sensor_data, 2);
          PyObject *x_delta = PyNumber_Subtract(PyTuple_GetItem(sensor, 0),
                                                PyTuple_GetItem(point, 0));
          PyObject *y_delta = PyNumber_Subtract(PyTuple_GetItem(sensor, 1),
                                                PyTuple_GetItem(point, 1));
          Py_SETREF(y_delta, PyNumber_Absolute(y_delta));
          PyObject *x_jump_py = PyNumber_Subtract(x_delta, y_delta);
          Py_SETREF(x_jump_py, PyNumber_Add(x_jump_py, srange));
          Py_SETREF(x_jump_py, PyNumber_Add(x_jump_py, const1));
          x_jump = PyLong_AsLongLong(x_jump_py);
          Py_DECREF(x_jump_py);
          Py_DECREF(x_delta);
          Py_DECREF(y_delta);
        }
      }
      if (x_jump == 0) {
        hidden_beacon = Py_NewRef(point);
      }
      Py_DECREF(point);
    }
  }
  Py_DECREF(const1);
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

PyObject *AoC_y2022_d15(PyObject *unicode_input) {
  PyObject *solution = 0;
  PyObject *sensors = PyList_New(0);
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  if (!lines || !sensors) {
    goto done;
  }

  for (Py_ssize_t i = 0; i < PyList_Size(lines); ++i) {
    PyObject *line = PyList_GetItem(lines, i);
    const char *line_bytes = PyUnicode_AsUTF8(line);
    long long x_sensor = 0;
    long long y_sensor = 0;
    long long x_beacon = 0;
    long long y_beacon = 0;
    sscanf(line_bytes,
           "Sensor at x=%lld, y=%lld: closest beacon is at x=%lld, y=%lld",
           &x_sensor,
           &y_sensor,
           &x_beacon,
           &y_beacon);
    PyObject *sensor = _AoC_y2022_d15_make_point(x_sensor, y_sensor);
    PyObject *beacon = _AoC_y2022_d15_make_point(x_beacon, y_beacon);
    PyObject *srange = _AoC_y2022_d15_L1_distance(sensor, beacon);
    PyObject *sensor_data = PyTuple_New(3);
    PyTuple_SET_ITEM(sensor_data, 0, sensor);
    PyTuple_SET_ITEM(sensor_data, 1, beacon);
    PyTuple_SET_ITEM(sensor_data, 2, srange);
    PyList_Append(sensors, sensor_data);
  }

  PyObject *part1_py = _AoC_y2022_d15_find_part1(sensors);
  PyObject *part2_py = _AoC_y2022_d15_find_part2(sensors);
  solution = PyUnicode_FromFormat("%S %S", part1_py, part2_py);
  Py_DECREF(part1_py);
  Py_DECREF(part2_py);

done:
  Py_XDECREF(lines);
  Py_XDECREF(sensors);
  return solution;
}

#endif  // _AOC_Y2022_D15_H_INCLUDED
