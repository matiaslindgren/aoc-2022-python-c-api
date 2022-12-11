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

PyObject *_AoC_y2022_d10_parse_and_apply_operation(PyObject *section, PyObject *old) {
  PyObject *res = NULL;
  PyObject *str_old = PyUnicode_FromFormat("old");
  PyObject *lines = PyUnicode_Splitlines(section, 0);
  PyObject *op_line = PyList_GetItem(lines, 2);
  PyObject *op_str = AoC_unicode_split(op_line, "Operation: ", 1);
  Py_SETREF(op_str, AoC_unicode_split(PyList_GetItem(op_str, 1), " ", -1));
  PyObject *op_lhs = PyList_GetItem(op_str, 2);
  PyObject *op_bin = PyList_GetItem(op_str, 3);
  PyObject *op_rhs = PyList_GetItem(op_str, 4);

  if (PyUnicode_Compare(op_lhs, str_old) == 0) {
    op_lhs = old;
    Py_INCREF(op_lhs);
  } else {
    op_lhs = PyLong_FromUnicodeObject(op_lhs, 10);
  }
  if (PyUnicode_Compare(op_rhs, str_old) == 0) {
    op_rhs = old;
    Py_INCREF(op_rhs);
  } else {
    op_rhs = PyLong_FromUnicodeObject(op_rhs, 10);
  }
  switch (PyUnicode_READ_CHAR(op_bin, 0)) {
    case '*': {
      res = PyNumber_Multiply(op_lhs, op_rhs);
    } break;
    case '+': {
      res = PyNumber_Add(op_lhs, op_rhs);
    } break;
    default: {
      PyErr_Format(PyExc_RuntimeError, "unknown binary op %S", op_bin);
      goto done;
    }
  }

done:
  Py_DECREF(op_rhs);
  Py_DECREF(op_lhs);
  Py_DECREF(op_str);
  Py_DECREF(lines);
  Py_DECREF(str_old);
  return res;
}

Py_ssize_t _AoC_y2022_d10_parse_target_monkey(PyObject *line) {
  PyObject *target_str = AoC_unicode_split(line, "throw to monkey ", 1);
  PyObject *target_int = PyLong_FromUnicodeObject(PyList_GetItem(target_str, 1), 10);
  Py_ssize_t target = PyLong_AsSize_t(target_int);
  Py_DECREF(target_str);
  Py_DECREF(target_int);
  return target;
}

PyObject *_AoC_y2022_d10_parse_test_divisor(PyObject *section) {
  PyObject *lines = PyUnicode_Splitlines(section, 0);
  PyObject *test_line = PyList_GetItem(lines, 3);
  PyObject *divisor_str = AoC_unicode_split(test_line, "Test: divisible by ", 1);
  PyObject *divisor = PyLong_FromUnicodeObject(PyList_GetItem(divisor_str, 1), 10);
  Py_DECREF(divisor_str);
  Py_DECREF(lines);
  return divisor;
}

Py_ssize_t _AoC_y2022_d10_parse_and_apply_test(PyObject *section, PyObject *value) {
  PyObject *const0 = PyLong_FromLong(0);
  Py_ssize_t res = -1;
  PyObject *lines = PyUnicode_Splitlines(section, 0);
  PyObject *divisor = _AoC_y2022_d10_parse_test_divisor(section);
  PyObject *rem = PyNumber_Remainder(value, divisor);

  switch (PyObject_RichCompareBool(rem, const0, Py_EQ)) {
    case 1: {
      res = _AoC_y2022_d10_parse_target_monkey(PyList_GetItem(lines, 4));
    } break;
    case 0: {
      res = _AoC_y2022_d10_parse_target_monkey(PyList_GetItem(lines, 5));
    } break;
    case -1:
    default: {
      goto done;
    } break;
  }

done:
  Py_DECREF(rem);
  Py_DECREF(divisor);
  Py_DECREF(lines);
  Py_DECREF(const0);
  return res;
}

PyObject *AoC_y2022_d11(PyObject *unicode_input) {
  PyObject *const1 = PyLong_FromLong(1);
  PyObject *const3 = PyLong_FromLong(3);
  PyObject *monkey_items = NULL;
  PyObject *monkey_inspections = NULL;
  PyObject *solution = NULL;
  PyObject *part1 = NULL;
  PyObject *part2 = NULL;
  PyObject *sections = AoC_unicode_split(unicode_input, "\n\n", -1);
  if (!sections) {
    PyErr_Format(PyExc_RuntimeError, "could not split input into sections");
    goto done;
  }

  Py_ssize_t num_monkeys = PyList_Size(sections);

  {
    monkey_items = PyList_New(num_monkeys);
    monkey_inspections = PyList_New(num_monkeys);

    for (Py_ssize_t i_monkey = 0; i_monkey < num_monkeys; ++i_monkey) {
      PyObject *section = PyList_GetItem(sections, i_monkey);
      PyObject *start_items = _AoC_y2022_d10_parse_starting_items(section);
      PyList_SET_ITEM(monkey_items, i_monkey, start_items);
      PyList_SET_ITEM(monkey_inspections, i_monkey, PyLong_FromLong(0));
    }

    for (size_t round = 0; round < 20; ++round) {
      for (Py_ssize_t i_monkey = 0; i_monkey < num_monkeys; ++i_monkey) {
        PyObject *section = PyList_GetItem(sections, i_monkey);
        PyObject *items_to_throw = PyList_GetItem(monkey_items, i_monkey);
        Py_ssize_t num_items = PyList_Size(items_to_throw);
        for (Py_ssize_t i_item = 0; i_item < num_items; ++i_item) {
          PyObject *item = PyList_GetItem(items_to_throw, i_item);
          Py_INCREF(item);
          Py_SETREF(item, _AoC_y2022_d10_parse_and_apply_operation(section, item));
          Py_SETREF(item, PyNumber_FloorDivide(item, const3));
          Py_ssize_t target_monkey = _AoC_y2022_d10_parse_and_apply_test(section, item);
          if (target_monkey < 0) {
            goto done;
          }
          PyObject *target_list = PyList_GetItem(monkey_items, target_monkey);
          if (PyList_Append(target_list, item) < 0) {
            goto done;
          }
        }
        PyObject *prev_inspect_count = PyList_GetItem(monkey_inspections, i_monkey);
        PyObject *inspect_count = PyLong_FromSsize_t(num_items);
        Py_SETREF(inspect_count, PyNumber_Add(prev_inspect_count, inspect_count));
        PyList_SetItem(monkey_inspections, i_monkey, inspect_count);
        PySequence_DelSlice(items_to_throw, 0, num_items);
      }
    }

    PyList_Sort(monkey_inspections);
    PyList_Reverse(monkey_inspections);

    PyObject *top1 = PyList_GetItem(monkey_inspections, 0);
    PyObject *top2 = PyList_GetItem(monkey_inspections, 1);
    part1 = PyNumber_Multiply(top1, top2);
  }

  {
    Py_SETREF(monkey_items, PyList_New(num_monkeys));
    Py_SETREF(monkey_inspections, PyList_New(num_monkeys));
    PyObject *lcm_test_divisor = PyLong_FromLong(1);

    for (Py_ssize_t i_monkey = 0; i_monkey < num_monkeys; ++i_monkey) {
      PyObject *section = PyList_GetItem(sections, i_monkey);
      PyObject *start_items = _AoC_y2022_d10_parse_starting_items(section);
      PyList_SET_ITEM(monkey_items, i_monkey, start_items);
      PyList_SET_ITEM(monkey_inspections, i_monkey, PyLong_FromLong(0));
      PyObject *test_divisor = _AoC_y2022_d10_parse_test_divisor(section);
      Py_SETREF(lcm_test_divisor, PyNumber_Multiply(lcm_test_divisor, test_divisor));
    }

    for (size_t round = 0; round < 10000; ++round) {
      for (Py_ssize_t i_monkey = 0; i_monkey < num_monkeys; ++i_monkey) {
        PyObject *section = PyList_GetItem(sections, i_monkey);
        PyObject *items_to_throw = PyList_GetItem(monkey_items, i_monkey);
        Py_ssize_t num_items = PyList_Size(items_to_throw);
        for (Py_ssize_t i_item = 0; i_item < num_items; ++i_item) {
          PyObject *item = PyList_GetItem(items_to_throw, i_item);
          Py_INCREF(item);
          Py_SETREF(item, _AoC_y2022_d10_parse_and_apply_operation(section, item));
          Py_SETREF(item, PyNumber_Remainder(item, lcm_test_divisor));
          Py_ssize_t target_monkey = _AoC_y2022_d10_parse_and_apply_test(section, item);
          if (target_monkey < 0) {
            goto done;
          }
          PyObject *target_list = PyList_GetItem(monkey_items, target_monkey);
          if (PyList_Append(target_list, item) < 0) {
            goto done;
          }
        }
        PyObject *prev_inspect_count = PyList_GetItem(monkey_inspections, i_monkey);
        PyObject *inspect_count = PyLong_FromSsize_t(num_items);
        Py_SETREF(inspect_count, PyNumber_Add(prev_inspect_count, inspect_count));
        PyList_SetItem(monkey_inspections, i_monkey, inspect_count);
        PySequence_DelSlice(items_to_throw, 0, num_items);
      }
    }
    Py_DECREF(lcm_test_divisor);

    PyList_Sort(monkey_inspections);
    PyList_Reverse(monkey_inspections);

    PyObject *top1 = PyList_GetItem(monkey_inspections, 0);
    PyObject *top2 = PyList_GetItem(monkey_inspections, 1);
    part2 = PyNumber_Multiply(top1, top2);
  }

  solution = PyUnicode_FromFormat("%S %S", part1, part2);

done:
  Py_XDECREF(part1);
  Py_XDECREF(part2);
  Py_XDECREF(monkey_items);
  Py_XDECREF(monkey_inspections);
  Py_XDECREF(sections);
  Py_XDECREF(const3);
  Py_XDECREF(const1);
  return solution;
}

#endif  // _AOC_Y2022_D11_H_INCLUDED
