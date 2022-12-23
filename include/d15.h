#ifndef _AOC_Y2022_D15_H_INCLUDED
#define _AOC_Y2022_D15_H_INCLUDED
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

int AoC_y2022_d15_is_inside_sensor_range(PyObject *sensor_data,
                                         PyObject *point) {
  PyObject *sensor = PyTuple_GetItem(sensor_data, 0);
  PyObject *point2sensor = _AoC_y2022_d15_L1_distance(point, sensor);
  PyObject *srange = PyTuple_GetItem(sensor_data, 2);
  int yes = 1 == PyObject_RichCompareBool(point2sensor, srange, Py_LE);
  Py_DECREF(point2sensor);
  return yes;
}

PyObject *_AoC_y2022_d15_find_part1(PyObject *sensors, const long long y) {
  long long part1 = 0;
  for (Py_ssize_t i = 0; i < PyList_Size(sensors); ++i) {
    PyObject *sensor_data = PyList_GetItem(sensors, i);
    PyObject *sensor = PyTuple_GetItem(sensor_data, 0);
    PyObject *beacon = PyTuple_GetItem(sensor_data, 1);
    PyObject *srange = PyTuple_GetItem(sensor_data, 2);
    const long long y_sensor = PyLong_AsLongLong(PyTuple_GetItem(sensor, 1));
    const long long range = PyLong_AsLongLong(srange);
    if (y_sensor - range <= y && y <= y_sensor + range) {
      part1 += 2 * (range - llabs(y - y_sensor)) + 1;
      const long long y_beacon = PyLong_AsLongLong(PyTuple_GetItem(beacon, 1));
      part1 -= y_beacon == y;
      part1 -= y_sensor == y;
    }
    /* const long long xs = PyLong_AsLongLong(PyTuple_GetItem(sensor, 0)); */
    PySys_FormatStdout("%zd %S %lld %lld %lld\n",
                       i,
                       sensor,
                       range,
                       range - llabs(y - y_sensor),
                       part1);
  }
  return PyLong_FromLongLong(part1);
}

PyObject *_AoC_y2022_d15_find_part2(PyObject *sensors, const long long limit) {
  PyObject *const1 = PyLong_FromLong(1);
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
  PyObject *sensors = 0;
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  if (!lines) {
    goto done;
  }

  Py_ssize_t num_sensors = PyList_Size(lines);
  sensors = PyList_New(num_sensors);
  if (!sensors) {
    goto done;
  }

  for (Py_ssize_t i = 0; i < num_sensors; ++i) {
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
    PyObject *sensor_data = PyTuple_Pack(3, sensor, beacon, srange);
    PyList_SET_ITEM(sensors, i, sensor_data);
  }

  PyObject *part1 = _AoC_y2022_d15_find_part1(sensors, 10);
  /* PyObject *part1 = _AoC_y2022_d15_find_part1(sensors, 2000000); */
  PyObject *part2 = PyLong_FromLong(0);
  /* PyObject *part2 = _AoC_y2022_d15_find_part2(sensors, 4000000); */
  solution = PyUnicode_FromFormat("%S %S", part1, part2);
  Py_DECREF(part1);
  Py_DECREF(part2);

done:
  Py_XDECREF(lines);
  Py_XDECREF(sensors);
  return solution;
}

#endif  // _AOC_Y2022_D15_H_INCLUDED
