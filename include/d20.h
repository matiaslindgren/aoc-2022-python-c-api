#ifndef _AOC_Y2022_D20_H_INCLUDED
#define _AOC_Y2022_D20_H_INCLUDED
#include "common.h"

PyObject *_AoC_y2022_d20_get_nth(PyObject *node, Py_ssize_t nth) {
  for (Py_ssize_t i = 0; i < nth; ++i) {
    node = PyDict_GetItemString(node, "next");
  }
  return node;
}

PyObject *_AoC_y2022_d20_find_prev(PyObject *head) {
  PyObject *node = 0;
  PyObject *next = head;
  do {
    node = next;
    next = PyDict_GetItemString(node, "next");
  } while (!Py_Is(next, head));
  return node;
}

PyObject *_AoC_y2022_d20_mix(PyObject *init_values,
                             Py_ssize_t key,
                             Py_ssize_t iterations) {
  PyObject *result = 0;
  Py_ssize_t num_items = PyList_Size(init_values);
  PyObject *items = PyList_New(num_items);
  PyObject *values = PyList_New(num_items);
  PyObject *wrap = PyLong_FromSsize_t(num_items - 1);
  PyObject *key_factor = PyLong_FromSsize_t(key);
  if (!items || !values || !wrap || !key_factor) {
    goto done;
  }

  for (Py_ssize_t i = 0; i < num_items; ++i) {
    PyObject *init_value = PyList_GetItem(init_values, i);
    PyObject *value = PyNumber_Multiply(init_value, key_factor);
    Py_INCREF(value);
    PyList_SET_ITEM(values, i, value);
    PyObject *item = PyDict_New();
    PyDict_SetItemString(item, "value", value);
    PyList_SET_ITEM(items, i, item);
  }
  for (Py_ssize_t i = 0; i < num_items; ++i) {
    PyObject *item0 = PyList_GetItem(items, i);
    PyObject *item1 = PyList_GetItem(items, (i + 1) % num_items);
    PyDict_SetItemString(item0, "next", item1);
  }

  for (Py_ssize_t iter = 0; iter < iterations; ++iter) {
    for (Py_ssize_t i = 0; i < num_items; ++i) {
      PyObject *src = Py_NewRef(PyList_GetItem(items, i));
      PyObject *delta = Py_NewRef(PyDict_GetItemString(src, "value"));
      Py_SETREF(delta, PyNumber_Remainder(delta, wrap));
      PyObject *dst = _AoC_y2022_d20_get_nth(src, PyLong_AsSsize_t(delta));
      if (Py_Is(dst, src)) {
        Py_DECREF(src);
        Py_DECREF(delta);
        continue;
      }
      PyObject *src_next = Py_NewRef(PyDict_GetItemString(src, "next"));
      PyObject *src_prev = Py_NewRef(_AoC_y2022_d20_find_prev(src));
      PyDict_SetItemString(src_prev, "next", src_next);
      PyObject *dst_next = Py_NewRef(PyDict_GetItemString(dst, "next"));
      PyDict_SetItemString(dst, "next", src);
      PyDict_SetItemString(src, "next", dst_next);
      Py_DECREF(src_next);
      Py_DECREF(src_prev);
      Py_DECREF(dst_next);
      Py_DECREF(src);
      Py_DECREF(delta);
    }
  }

  PyObject *zero = PyList_GetItem(items, 0);
  while (PyLong_AsLong(PyDict_GetItemString(zero, "value"))) {
    zero = PyDict_GetItemString(zero, "next");
  }
  result = PyLong_FromLong(0);
  for (Py_ssize_t idx = 1; idx <= 3; ++idx) {
    PyObject *item = _AoC_y2022_d20_get_nth(zero, (1000 * idx) % num_items);
    PyObject *value = PyDict_GetItemString(item, "value");
    Py_SETREF(result, PyNumber_Add(result, value));
  }

done:
  Py_XDECREF(items);
  Py_XDECREF(values);
  Py_XDECREF(key_factor);
  Py_XDECREF(wrap);
  return result;
}

PyObject *AoC_y2022_d20(PyObject *unicode_input) {
  PyObject *solution = 0;
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  PyObject *values = 0;
  if (!lines) {
    goto done;
  }

  Py_ssize_t num_values = PyList_Size(lines);
  values = PyList_New(num_values);
  if (!values) {
    goto done;
  }
  for (Py_ssize_t i = 0; i < num_values; ++i) {
    PyObject *line = PyList_GetItem(lines, i);
    PyList_SET_ITEM(values, i, PyLong_FromUnicodeObject(line, 10));
  }

  PyObject *part1 = 0;
  {
    Py_ssize_t key = 1;
    Py_ssize_t iterations = 1;
    part1 = _AoC_y2022_d20_mix(values, key, iterations);
  }
  PyObject *part2 = 0;
  {
    Py_ssize_t key = 811589153;
    Py_ssize_t iterations = 10;
    part2 = _AoC_y2022_d20_mix(values, key, iterations);
  }
  solution = PyUnicode_FromFormat("%S %S", part1, part2);
  Py_DECREF(part1);
  Py_DECREF(part2);

done:
  Py_XDECREF(values);
  Py_XDECREF(lines);
  return solution;
}

#endif  // _AOC_Y2022_D20_H_INCLUDED
