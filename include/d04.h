#ifndef _AOC_Y2022_D04_H_INCLUDED
#define _AOC_Y2022_D04_H_INCLUDED
#include "common.h"

PyObject *AoC_y2022_d04(PyObject *unicode_input) {
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  if (lines == NULL) {
    PyErr_SetString(PyExc_RuntimeError, "Failed splitting day 4 input into lines");
    return NULL;
  }
  if (!_AoC_list_is_ok(lines)) {
    PyErr_SetString(PyExc_RuntimeError, "list of input lines is not valid");
    Py_DECREF(lines);
    return NULL;
  }

  Py_ssize_t num_lines = PyList_Size(lines);
  for (Py_ssize_t i = 0; i < num_lines; ++i) {
    PyObject *line = PyList_GetItem(lines, i);
    PyObject *parts = AoC_unicode_split(line, " ", 2);

    PyObject *date_str = PyList_GetItem(parts, 0);

    PyObject *year_str = PyUnicode_Substring(date_str, (Py_ssize_t)1, (Py_ssize_t)5);
    PyObject *year = PyLong_FromUnicodeObject(year_str, 10);
    Py_DECREF(year_str);

    PyObject *month_str = PyUnicode_Substring(date_str, (Py_ssize_t)6, (Py_ssize_t)8);
    PyObject *month = PyLong_FromUnicodeObject(month_str, 10);
    Py_DECREF(month_str);

    PyObject *day_str = PyUnicode_Substring(date_str, (Py_ssize_t)9, (Py_ssize_t)11);
    PyObject *day = PyLong_FromUnicodeObject(day_str, 10);
    Py_DECREF(day_str);

    PyObject *time_str = PyList_GetItem(parts, 1);

    PyObject *hour_str = PyUnicode_Substring(time_str, (Py_ssize_t)0, (Py_ssize_t)2);
    PyObject *hour = PyLong_FromUnicodeObject(hour_str, 10);
    Py_DECREF(hour_str);

    PyObject *minute_str = PyUnicode_Substring(time_str, (Py_ssize_t)3, (Py_ssize_t)5);
    PyObject *minute = PyLong_FromUnicodeObject(minute_str, 10);
    Py_DECREF(minute_str);

    Py_DECREF(parts);

    PySys_FormatStdout("%zd: %S-%S-%S %S:%S\n", i, year, month, day, hour, minute);

    Py_DECREF(year);
    Py_DECREF(month);
    Py_DECREF(day);
    Py_DECREF(hour);
    Py_DECREF(minute);
  }

  Py_DECREF(lines);
  return Py_BuildValue("U", "TODO answer");
}
#endif  // _AOC_Y2022_D04_H_INCLUDED
