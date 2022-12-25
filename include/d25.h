#ifndef _AOC_Y2022_D25_H_INCLUDED
#define _AOC_Y2022_D25_H_INCLUDED
#include "common.h"

PyObject *_AoC_y2022_d25_parse_snafu(PyObject *str) {
  PyObject *two = PyLong_FromLong(2);
  PyObject *five = PyLong_FromLong(5);

  const Py_ssize_t n = PyUnicode_GET_LENGTH(str);
  PyObject *res = PyLong_FromLong(0);

  for (Py_ssize_t i = 0; i < n; ++i) {
    PyObject *x = PyLong_FromLong(n - (i + 1));
    Py_SETREF(x, PyNumber_Power(five, x, Py_None));
    switch (PyUnicode_READ_CHAR(str, i)) {
      case '0': {
        Py_SETREF(x, PyLong_FromLong(0));
      } break;
      case '-': {
        Py_SETREF(x, PyNumber_Negative(x));
      } break;
      case '=': {
        Py_SETREF(x, PyNumber_Negative(x));
      }
      case '2': {
        Py_SETREF(x, PyNumber_Multiply(x, two));
      } break;
    }
    Py_SETREF(res, PyNumber_Add(res, x));
    Py_DECREF(x);
  }

  Py_DECREF(two);
  Py_DECREF(five);

  return res;
}

int _AoC_y2022_d25_is_zero(PyObject *x) {
  PyObject *zero = PyLong_FromLong(0);
  int yes = 1 == PyObject_RichCompareBool(x, zero, Py_EQ);
  Py_DECREF(zero);
  return yes;
}

PyObject *_AoC_y2022_d25_format_snafu(PyObject *snafu_num) {
  PyObject *five = PyLong_FromLong(5);
  PyObject *base5_digits = PyList_New(0);
  PyObject *x = Py_NewRef(snafu_num);
  while (!_AoC_y2022_d25_is_zero(x)) {
    PyObject *base5_digit = PyNumber_Remainder(x, five);
    Py_SETREF(x, PyNumber_FloorDivide(x, five));
    PyList_Append(base5_digits, base5_digit);
  }
  Py_DECREF(x);

  PyObject *one = PyLong_FromLong(1);
  PyObject *three = PyLong_FromLong(3);
  PyObject *carry = PyLong_FromLong(0);
  PyObject *snafu_digits = PyList_New(0);
  for (Py_ssize_t i = 0; i < PyList_Size(base5_digits); ++i) {
    PyObject *base5_digit = Py_NewRef(PyList_GetItem(base5_digits, i));
    Py_SETREF(base5_digit, PyNumber_Add(base5_digit, carry));
    Py_SETREF(carry, PyNumber_FloorDivide(base5_digit, three));
    PyObject *snafu_digit = PyNumber_Multiply(carry, five);
    Py_SETREF(snafu_digit, PyNumber_Subtract(base5_digit, snafu_digit));
    PyList_Append(snafu_digits, snafu_digit);
  }
  if (!_AoC_y2022_d25_is_zero(carry)) {
    PyList_Append(snafu_digits, carry);
  }
  Py_DECREF(one);
  Py_DECREF(three);
  Py_DECREF(five);
  Py_DECREF(carry);
  Py_DECREF(base5_digits);

  const Py_ssize_t snafu_len = PyList_Size(snafu_digits);
  PyObject *snafu_str = PyUnicode_New(snafu_len, (Py_UCS4)(255));
  for (Py_ssize_t i = 0; i < snafu_len; ++i) {
    const long snafu_digit = PyLong_AsLong(PyList_GetItem(snafu_digits, i));
    char ch = 0;
    switch (snafu_digit) {
      case 0:
      case 1:
      case 2:
        ch = '0' + snafu_digit;
        break;
      case -1:
        ch = '-';
        break;
      case -2:
        ch = '=';
        break;
    }
    PyUnicode_WriteChar(snafu_str, snafu_len - (i + 1), ch);
  }
  Py_DECREF(snafu_digits);

  return snafu_str;
}

PyObject *AoC_y2022_d25(PyObject *unicode_input) {
  PyObject *solution = 0;
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  if (!lines) {
    goto done;
  }

  PyObject *sum = PyLong_FromLong(0);
  Py_ssize_t num_lines = PyList_Size(lines);
  for (Py_ssize_t i = 0; i < num_lines; ++i) {
    PyObject *line = PyList_GetItem(lines, i);
    PyObject *num = _AoC_y2022_d25_parse_snafu(line);
    PyObject *x = _AoC_y2022_d25_format_snafu(num);
    PySys_FormatStdout("%zd %S %S %S\n", i, line, num, x);
    Py_SETREF(sum, PyNumber_Add(sum, num));
    Py_DECREF(num);
  }

  PyObject *part1 = _AoC_y2022_d25_format_snafu(sum);
  PyObject *part2 = PyLong_FromLong(0);
  solution = PyUnicode_FromFormat("%S %S", part1, part2);
  Py_DECREF(part1);
  Py_DECREF(part2);
  Py_DECREF(sum);

done:
  Py_XDECREF(lines);
  return solution;
}

#endif  // _AOC_Y2022_D25_H_INCLUDED
