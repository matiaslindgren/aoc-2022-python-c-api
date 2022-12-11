#ifndef _AOC_Y2022_D11_H_INCLUDED
#define _AOC_Y2022_D11_H_INCLUDED
#include "common.h"

PyObject *_AoC_y2022_d10_parse_starting_items(PyObject *section) {
  PyObject *lines = PyUnicode_Splitlines(section, 0);
  PyObject *items_line = PyList_GetItem(lines, 1);
  PyObject *items_str = AoC_unicode_split(items_line, "Starting items: ", 1);
  PyObject *items = PyList_New(0);
  if (PyList_Size(items_str) > 1) {
    Py_SETREF(items_str, AoC_unicode_split(PyList_GetItem(items_str, 1), ", ", -1));
    for (Py_ssize_t i_item = 0; i_item < PyList_Size(items_str); ++i_item) {
      PyObject *item_str = PyList_GetItem(items_str, i_item);
      PyObject *item = PyLong_FromUnicodeObject(item_str, 10);
      PyList_Append(items, item);
    }
  }
  Py_DECREF(items_str);
  Py_DECREF(lines);
  return items;
}

PyObject *_AoC_y2022_d10_parse_binary_operation(PyObject *section) {
  PyObject *op = NULL;
  PyObject *str_old = PyUnicode_FromFormat("old");
  PyObject *lines = PyUnicode_Splitlines(section, 0);
  PyObject *op_line = PyList_GetItem(lines, 2);
  PyObject *op_str = AoC_unicode_split(op_line, "Operation: ", 1);
  Py_SETREF(op_str, AoC_unicode_split(PyList_GetItem(op_str, 1), " ", -1));

  PyObject *op_lhs = PyList_GetItem(op_str, 2);
  PyObject *op_rhs = PyList_GetItem(op_str, 4);
  PyObject *op_bin = PyList_GetItem(op_str, 3);

  if (PyUnicode_Compare(op_lhs, str_old) == 0) {
    op_lhs = Py_None;
    Py_INCREF(op_lhs);
  } else {
    op_lhs = PyLong_FromUnicodeObject(op_lhs, 10);
  }
  if (PyUnicode_Compare(op_rhs, str_old) == 0) {
    op_rhs = Py_None;
    Py_INCREF(op_rhs);
  } else {
    op_rhs = PyLong_FromUnicodeObject(op_rhs, 10);
  }
  switch (PyUnicode_READ_CHAR(op_bin, 0)) {
    case '*': {
    } break;
    case '+': {
    } break;
    default: {
      PyErr_Format(PyExc_RuntimeError, "unknown binary op %S", op_bin);
      goto done;
    }
  }

  op = PyTuple_New(3);
  PyTuple_SET_ITEM(op, 0, op_lhs);
  PyTuple_SET_ITEM(op, 1, op_rhs);
  PyTuple_SET_ITEM(op, 2, op_bin);

done:
  Py_DECREF(op_str);
  Py_DECREF(lines);
  Py_DECREF(str_old);
  return op;
}

PyObject *_AoC_y2022_d10_compute_binary_operation(PyObject *op, PyObject *old) {
  PyObject *op_lhs = PyTuple_GET_ITEM(op, 0);
  PyObject *op_rhs = PyTuple_GET_ITEM(op, 1);
  PyObject *op_bin = PyTuple_GET_ITEM(op, 2);
  if (op_lhs == Py_None) {
    op_lhs = old;
  }
  if (op_rhs == Py_None) {
    op_rhs = old;
  }
  PyObject *res = NULL;
  switch (PyUnicode_READ_CHAR(op_bin, 0)) {
    case '*': {
      res = PyNumber_Multiply(op_lhs, op_rhs);
    } break;
    case '+': {
      res = PyNumber_Add(op_lhs, op_rhs);
    } break;
  }
  return res;
}

PyObject *_AoC_y2022_d10_parse_modtest(PyObject *section) {
  PyObject *lines = PyUnicode_Splitlines(section, 0);

  PyObject *test_line = PyList_GetItem(lines, 3);
  PyObject *divisor_str = AoC_unicode_split(test_line, "Test: divisible by ", 1);
  PyObject *divisor = PyLong_FromUnicodeObject(PyList_GetItem(divisor_str, 1), 10);

  PyObject *on_true = AoC_unicode_split(PyList_GetItem(lines, 4), "throw to monkey ", 1);
  Py_SETREF(on_true, PyLong_FromUnicodeObject(PyList_GetItem(on_true, 1), 10));

  PyObject *on_false = AoC_unicode_split(PyList_GetItem(lines, 5), "throw to monkey ", 1);
  Py_SETREF(on_false, PyLong_FromUnicodeObject(PyList_GetItem(on_false, 1), 10));

  PyObject *modtest = PyTuple_New(3);
  PyTuple_SET_ITEM(modtest, 0, divisor);
  PyTuple_SET_ITEM(modtest, 1, on_true);
  PyTuple_SET_ITEM(modtest, 2, on_false);

  Py_DECREF(divisor_str);
  Py_DECREF(lines);
  return modtest;
}

Py_ssize_t _AoC_y2022_d10_apply_test(PyObject *modtest, PyObject *value) {
  PyObject *const0 = PyLong_FromLong(0);
  PyObject *divisor = PyTuple_GET_ITEM(modtest, 0);
  PyObject *rem = PyNumber_Remainder(value, divisor);
  Py_ssize_t res = -1;
  switch (PyObject_RichCompareBool(rem, const0, Py_EQ)) {
    case 1: {
      res = PyLong_AsSize_t(PyTuple_GET_ITEM(modtest, 1));
    } break;
    case 0: {
      res = PyLong_AsSize_t(PyTuple_GET_ITEM(modtest, 2));
    } break;
  }
  Py_DECREF(rem);
  Py_DECREF(const0);
  return res;
}

PyObject *_AoC_y2022_d10_run_monkey_business(PyObject *sections, long worry_divisor_,
                                             size_t num_rounds) {
  PyObject *monkey_business = NULL;
  PyObject *worry_divisor = PyLong_FromLong(worry_divisor_);
  PyObject *lcm_test_divisor = PyLong_FromLong(1);

  Py_ssize_t num_monkeys = PyList_Size(sections);
  PyObject *monkey_state = PyList_New(num_monkeys);

  if (!worry_divisor || !lcm_test_divisor || !monkey_state) {
    goto done;
  }

  for (Py_ssize_t i_monkey = 0; i_monkey < num_monkeys; ++i_monkey) {
    PyObject *section = PyList_GetItem(sections, i_monkey);
    PyObject *state = PyDict_New();
    PyObject *inspect_count = PyLong_FromLong(0);
    PyObject *start_items = _AoC_y2022_d10_parse_starting_items(section);
    PyObject *modtest = _AoC_y2022_d10_parse_modtest(section);
    PyObject *binop = _AoC_y2022_d10_parse_binary_operation(section);
    if (!state || !inspect_count || !start_items || !modtest || !binop) {
      Py_XDECREF(binop);
      Py_XDECREF(modtest);
      Py_XDECREF(start_items);
      Py_XDECREF(inspect_count);
      Py_XDECREF(state);
      goto done;
    }
    PyDict_SetItemString(state, "items", start_items);
    PyDict_SetItemString(state, "inspect_count", PyLong_FromLong(0));
    PyDict_SetItemString(state, "modtest", modtest);
    PyDict_SetItemString(state, "binop", binop);
    PyList_SET_ITEM(monkey_state, i_monkey, state);
    PyObject *test_divisor = PyTuple_GET_ITEM(modtest, 0);
    Py_SETREF(lcm_test_divisor, PyNumber_Multiply(lcm_test_divisor, test_divisor));
  }

  for (size_t round = 0; round < num_rounds; ++round) {
    for (Py_ssize_t i_monkey = 0; i_monkey < num_monkeys; ++i_monkey) {
      PyObject *state = PyList_GetItem(monkey_state, i_monkey);
      PyObject *items = PyDict_GetItemString(state, "items");
      PyObject *binop = PyDict_GetItemString(state, "binop");
      PyObject *modtest = PyDict_GetItemString(state, "modtest");
      Py_ssize_t num_items = PyList_Size(items);
      for (Py_ssize_t i_item = 0; i_item < num_items; ++i_item) {
        PyObject *item = PyList_GetItem(items, i_item);
        Py_INCREF(item);
        Py_SETREF(item, _AoC_y2022_d10_compute_binary_operation(binop, item));
        Py_SETREF(item, PyNumber_FloorDivide(item, worry_divisor));
        Py_SETREF(item, PyNumber_Remainder(item, lcm_test_divisor));
        Py_ssize_t target_monkey = _AoC_y2022_d10_apply_test(modtest, item);
        if (target_monkey < 0) {
          goto done;
        }
        PyObject *target_state = PyList_GetItem(monkey_state, target_monkey);
        PyObject *target_list = PyDict_GetItemString(target_state, "items");
        if (PyList_Append(target_list, item) < 0) {
          goto done;
        }
      }
      PyObject *prev_inspect_count = PyDict_GetItemString(state, "inspect_count");
      PyObject *inspect_count = PyLong_FromSsize_t(num_items);
      Py_SETREF(inspect_count, PyNumber_Add(prev_inspect_count, inspect_count));
      PyDict_SetItemString(state, "inspect_count", inspect_count);
      if (PyList_SetSlice(items, 0, num_items, NULL) < 0) {
        goto done;
      }
    }
  }

  PyObject *top1 = PyLong_FromLong(0);
  PyObject *top2 = PyLong_FromLong(0);
  for (Py_ssize_t i_monkey = 0; i_monkey < num_monkeys; ++i_monkey) {
    PyObject *state = PyList_GetItem(monkey_state, i_monkey);
    PyObject *inspect_count = PyDict_GetItemString(state, "inspect_count");
    if (PyObject_RichCompareBool(inspect_count, top1, Py_GT) == 1) {
      Py_SETREF(top2, top1);
      Py_SETREF(top1, inspect_count);
    } else if (PyObject_RichCompareBool(inspect_count, top2, Py_GT) == 1) {
      Py_SETREF(top2, inspect_count);
    }
  }
  monkey_business = PyNumber_Multiply(top1, top2);
  Py_DECREF(top1);
  Py_DECREF(top2);

done:
  Py_XDECREF(monkey_state);
  Py_XDECREF(lcm_test_divisor);
  Py_XDECREF(worry_divisor);
  return monkey_business;
}

PyObject *AoC_y2022_d11(PyObject *unicode_input) {
  PyObject *part1 = NULL;
  PyObject *part2 = NULL;
  PyObject *solution = NULL;
  PyObject *sections = AoC_unicode_split(unicode_input, "\n\n", -1);
  if (!sections) {
    PyErr_Format(PyExc_RuntimeError, "could not split input into sections");
    goto done;
  }
  {
    long worry_divisor = 3;
    size_t num_rounds = 20;
    part1 = _AoC_y2022_d10_run_monkey_business(sections, worry_divisor, num_rounds);
    if (!part1) {
      goto done;
    }
  }
  {
    long worry_divisor = 1;
    size_t num_rounds = 10000;
    part2 = _AoC_y2022_d10_run_monkey_business(sections, worry_divisor, num_rounds);
    if (!part2) {
      goto done;
    }
  }
  solution = PyUnicode_FromFormat("%S %S", part1, part2);

done:
  Py_XDECREF(part1);
  Py_XDECREF(part2);
  Py_XDECREF(sections);
  return solution;
}

#endif  // _AOC_Y2022_D11_H_INCLUDED
