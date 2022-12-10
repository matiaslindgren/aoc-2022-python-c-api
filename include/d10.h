#ifndef _AOC_Y2022_D10_H_INCLUDED
#define _AOC_Y2022_D10_H_INCLUDED
#include "common.h"

int _AoC_y2022_d10_pixel_is_lit(long cycle, long reg) {
  long draw_pos = (cycle - 1) % 40;
  return reg - 1 == draw_pos || reg == draw_pos || reg + 1 == draw_pos;
}

PyObject *AoC_y2022_d10(PyObject *unicode_input) {
  PyObject *program = PyList_New(0);
  PyObject *reg_X = PyLong_FromLong(1);
  PyObject *part1 = PyLong_FromLong(0);
  PyObject *solution = NULL;
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  if (!lines) {
    PyErr_Format(PyExc_RuntimeError, "could not split input into lines");
    goto error;
  }

  for (Py_ssize_t line_num = 0; line_num < PyList_Size(lines); ++line_num) {
    PyObject *line = PyList_GetItem(lines, line_num);
    PyObject *line_parts = PyUnicode_Split(line, NULL, 1);
    PyObject *op_code = PyList_GetItem(line_parts, 0);
    PyObject *op_value = PyLong_FromLong(0);
    PyObject *op_cycles = PyLong_FromLong(1);
    if (AoC_PyUnicode_Equals_ASCII(op_code, "addx")) {
      PyObject *line_value = PyList_GetItem(line_parts, 1);
      Py_SETREF(op_value, PyLong_FromUnicodeObject(line_value, 10));
      Py_SETREF(op_cycles, PyLong_FromLong(2));
    }
    PyObject *op = PyTuple_New(3);
    Py_INCREF(op_code);
    PyTuple_SET_ITEM(op, 0, op_code);
    PyTuple_SET_ITEM(op, 1, op_value);
    PyTuple_SET_ITEM(op, 2, op_cycles);
    PyList_Append(program, op);
    Py_DECREF(op);
    Py_DECREF(line_parts);
  }

  int pixel_is_lit[240] = {0};
  PyObject *executing_ins = NULL;
  for (size_t cycle = 1; executing_ins || PyList_Size(program) > 0; ++cycle) {
    if (!executing_ins) {
      executing_ins = PyList_GetItem(program, 0);
      if (!executing_ins) {
        goto error;
      }
      Py_INCREF(executing_ins);
      if (PySequence_DelItem(program, 0) < 0) {
        Py_DECREF(executing_ins);
        goto error;
      }
    }
    if (cycle % 40 == 20) {
      PyObject *cycle_py = PyLong_FromSize_t(cycle);
      PyObject *sig = PyNumber_Multiply(cycle_py, reg_X);
      Py_SETREF(part1, PyNumber_Add(part1, sig));
      Py_DECREF(sig);
      Py_DECREF(cycle_py);
    }
    pixel_is_lit[cycle - 1] = _AoC_y2022_d10_pixel_is_lit(cycle, PyLong_AsLong(reg_X));
    PyObject *op_code = PyTuple_GET_ITEM(executing_ins, 0);
    PyObject *op_value = PyTuple_GET_ITEM(executing_ins, 1);
    PyObject *op_cycles = PyTuple_GET_ITEM(executing_ins, 2);
    PyObject *const1 = PyLong_FromLong(1);
    Py_SETREF(op_cycles, PyNumber_Subtract(op_cycles, const1));
    PyTuple_SetItem(executing_ins, 2, op_cycles);
    PyObject *const0 = PyLong_FromLong(0);
    if (PyObject_RichCompareBool(op_cycles, const0, Py_EQ) == 1) {
      if (AoC_PyUnicode_Equals_ASCII(op_code, "addx")) {
        Py_SETREF(reg_X, PyNumber_Add(reg_X, op_value));
      }
      Py_DECREF(executing_ins);
      executing_ins = NULL;
    }
    Py_DECREF(const1);
    Py_DECREF(const0);
  }

  PyObject *pixels = PyList_New(0);
  for (size_t row = 0; row < 6; ++row) {
    for (size_t col = 0; col < 40; ++col) {
      PyList_Append(pixels, PyUnicode_FromFormat(pixel_is_lit[row * 40 + col] ? "#" : " "));
    }
    PyList_Append(pixels, PyUnicode_FromFormat("\n"));
  }

  PyObject *empty_sep = PyUnicode_FromFormat("");
  PyObject *image = PyUnicode_Join(empty_sep, pixels);
  solution = PyUnicode_FromFormat("%S\n%S", part1, image);
  Py_DECREF(image);
  Py_DECREF(empty_sep);
  Py_DECREF(pixels);

error:
  Py_XDECREF(program);
  Py_XDECREF(reg_X);
  Py_XDECREF(part1);
  Py_XDECREF(lines);
  return solution;
}

#endif  // _AOC_Y2022_D10_H_INCLUDED
