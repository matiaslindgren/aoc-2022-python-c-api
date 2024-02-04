#ifndef _AOC_Y2022_D10_H_INCLUDED
#define _AOC_Y2022_D10_H_INCLUDED
#include <math.h>

#include "common.h"

const size_t num_rows = 6;
const size_t num_cols = 40;

PyObject *AoC_y2022_d10(PyObject *unicode_input) {
  PyObject *solution = NULL;
  PyObject *const0 = PyLong_FromLong(0);
  PyObject *const1 = PyLong_FromLong(1);
  PyObject *program = PyList_New(0);
  PyObject *reg_X = PyLong_FromLong(1);
  PyObject *part1 = PyLong_FromLong(0);
  PyObject *pixels = PyList_New(0);
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  if (!const0 || !const1 || !program || !reg_X || !part1 || !pixels || !lines) {
    PyErr_Format(PyExc_RuntimeError, "initialization failure");
    goto done;
  }

  Py_ssize_t lineno = 0;
  PyObject *op_code = NULL;
  PyObject *op_value = NULL;
  PyObject *op_cycles = NULL;
  for (size_t cycle = 1; lineno < PyList_Size(lines); ++cycle) {
    if (!op_code) {
      PyObject *line = PyList_GetItem(lines, lineno);
      PyObject *line_parts = PyUnicode_Split(line, NULL, 1);
      op_code = PyList_GetItem(line_parts, 0);
      Py_INCREF(op_code);
      op_value = PyLong_FromLong(0);
      op_cycles = PyLong_FromLong(1);
      if (AoC_PyUnicode_Equals_ASCII(op_code, "addx")) {
        PyObject *op_value_str = PyList_GetItem(line_parts, 1);
        Py_SETREF(op_value, PyLong_FromUnicodeObject(op_value_str, 10));
        Py_SETREF(op_cycles, PyLong_FromLong(2));
      }
      Py_DECREF(line_parts);
    }

    if (cycle % num_cols == 20) {
      PyObject *cycle_py = PyLong_FromSize_t(cycle);
      PyObject *sig = PyNumber_Multiply(cycle_py, reg_X);
      Py_SETREF(part1, PyNumber_Add(part1, sig));
      Py_DECREF(sig);
      Py_DECREF(cycle_py);
    }

    const long draw_pos = (cycle - 1) % num_cols;
    const long sprite_pos = PyLong_AsLong(reg_X);
    const int pixel_is_lit = labs(draw_pos - sprite_pos) < 2;
    if (0 >
        PyList_Append(pixels, PyUnicode_FromFormat(pixel_is_lit ? "#" : " "))) {
      goto done;
    }
    if (draw_pos + 1 == num_cols) {
      if (0 > PyList_Append(pixels, PyUnicode_FromFormat("\n"))) {
        goto done;
      }
    }

    Py_SETREF(op_cycles, PyNumber_Subtract(op_cycles, const1));
    if (PyObject_RichCompareBool(op_cycles, const0, Py_EQ) == 1) {
      if (AoC_PyUnicode_Equals_ASCII(op_code, "addx")) {
        Py_SETREF(reg_X, PyNumber_Add(reg_X, op_value));
      }
      Py_DECREF(op_code);
      Py_DECREF(op_value);
      Py_DECREF(op_cycles);
      op_code = NULL;
      op_value = NULL;
      op_cycles = NULL;
      ++lineno;
    }
  }

  PyObject *empty_str = PyUnicode_FromFormat("");
  PyObject *image = PyUnicode_Join(empty_str, pixels);
  Py_DECREF(empty_str);
  if (!image) {
    goto done;
  }
  solution = PyUnicode_FromFormat("%S\n%S", part1, image);
  Py_DECREF(image);

done:
  Py_XDECREF(const0);
  Py_XDECREF(const1);
  Py_XDECREF(program);
  Py_XDECREF(reg_X);
  Py_XDECREF(part1);
  Py_XDECREF(pixels);
  Py_XDECREF(lines);
  return solution;
}

#endif  // _AOC_Y2022_D10_H_INCLUDED
