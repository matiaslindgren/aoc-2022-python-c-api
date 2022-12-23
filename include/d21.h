#ifndef _AOC_Y2022_D21_H_INCLUDED
#define _AOC_Y2022_D21_H_INCLUDED
#include "common.h"

PyObject *_AoC_y2022_d21_eval_binop(PyObject *lhs,
                                    PyObject *rhs,
                                    PyObject *op2) {
  switch (PyUnicode_READ_CHAR(op2, 0)) {
    case '+':
      return PyNumber_Add(lhs, rhs);
    case '-':
      return PyNumber_Subtract(lhs, rhs);
    case '*':
      return PyNumber_Multiply(lhs, rhs);
    case '/':
      return PyNumber_FloorDivide(lhs, rhs);
  }
  PyErr_Format(PyExc_RuntimeError, "unknown binop %S", op2);
  return 0;
}

PyObject *_AoC_y2022_d21_eval(PyObject *prog, PyObject *dst) {
  PyObject *expr = PyDict_GetItem(prog, dst);
  // TODO types
  if (!expr) {
    return dst;
  }
  if (PyTuple_Size(expr) == 1) {
    PyObject *val = PyTuple_GetItem(expr, 0);
    return val;
  }
  PyObject *lhs = PyTuple_GetItem(expr, 0);
  PyObject *rhs = PyTuple_GetItem(expr, 1);
  PyObject *op2 = PyTuple_GetItem(expr, 2);
  PyObject *lhs_val = _AoC_y2022_d21_eval(prog, lhs);
  PyObject *rhs_val = _AoC_y2022_d21_eval(prog, rhs);
  int is_equation = (PyTuple_Check(lhs_val) || PyTuple_Check(rhs_val) ||
                     PyUnicode_Check(lhs_val) || PyUnicode_Check(rhs_val));
  if (is_equation) {
    return PyTuple_Pack(3, lhs_val, rhs_val, op2);
  }
  return _AoC_y2022_d21_eval_binop(lhs_val, rhs_val, op2);
}

PyObject *_AoC_y2022_d21_solve_for(PyObject *lhs,
                                   PyObject *rhs,
                                   PyObject *var) {
  if (PyUnicode_Check(lhs) && PyUnicode_Compare(lhs, var) == 0) {
    return rhs;
  }
  if (PyUnicode_Check(rhs) && PyUnicode_Compare(rhs, var) == 0) {
    return lhs;
  }
  PyObject *res = Py_NewRef(PyLong_Check(lhs) ? lhs : rhs);
  PyObject *eqn = PyLong_Check(lhs) ? rhs : lhs;
  PyObject *eqn_lhs = PyTuple_GetItem(eqn, 0);
  PyObject *eqn_rhs = PyTuple_GetItem(eqn, 1);
  PyObject *eqn_op2 = PyTuple_GetItem(eqn, 2);
  PyObject *eqn_val = 0;
  PyObject *eqn_eqn = 0;
  if (PyLong_Check(eqn_lhs)) {
    eqn_val = eqn_lhs;
    eqn_eqn = eqn_rhs;
    switch (PyUnicode_READ_CHAR(eqn_op2, 0)) {
      case '-': {
        Py_SETREF(res, PyNumber_Negative(res));
      } break;
      case '/': {
        PyObject *one = PyLong_FromLong(1);
        Py_SETREF(res, PyNumber_TrueDivide(one, res));
      } break;
    }
  } else {
    eqn_val = eqn_rhs;
    eqn_eqn = eqn_lhs;
  }
  switch (PyUnicode_READ_CHAR(eqn_op2, 0)) {
    case '+': {
      Py_SETREF(res, PyNumber_Subtract(res, eqn_val));
    } break;
    case '-': {
      Py_SETREF(res, PyNumber_Add(res, eqn_val));
    } break;
    case '*': {
      Py_SETREF(res, PyNumber_FloorDivide(res, eqn_val));
    } break;
    case '/': {
      Py_SETREF(res, PyNumber_Multiply(res, eqn_val));
    } break;
  }
  return _AoC_y2022_d21_solve_for(res, eqn_eqn, var);
}

PyObject *AoC_y2022_d21(PyObject *unicode_input) {
  PyObject *solution = 0;
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  PyObject *prog = PyDict_New();
  if (!lines || !prog) {
    goto done;
  }

  Py_ssize_t num_lines = PyList_Size(lines);
  for (Py_ssize_t i = 0; i < num_lines; ++i) {
    PyObject *line = PyList_GetItem(lines, i);
    PyObject *line_parts = AoC_unicode_split(line, ": ", 1);
    PyObject *dst = Py_NewRef(PyList_GetItem(line_parts, 0));
    PyObject *expr_str = PyList_GetItem(line_parts, 1);
    PyObject *expr_parts = PyUnicode_Split(expr_str, 0, 2);
    PyObject *expr = 0;
    if (PyList_Size(expr_parts) == 1) {
      PyObject *val_str = PyList_GetItem(expr_parts, 0);
      PyObject *val = PyLong_FromUnicodeObject(val_str, 10);
      expr = PyTuple_Pack(1, val);
    } else {
      PyObject *lhs = Py_NewRef(PyList_GetItem(expr_parts, 0));
      PyObject *op2 = Py_NewRef(PyList_GetItem(expr_parts, 1));
      PyObject *rhs = Py_NewRef(PyList_GetItem(expr_parts, 2));
      expr = PyTuple_Pack(3, lhs, rhs, op2);
    }
    PyDict_SetItem(prog, dst, expr);
    Py_DECREF(line_parts);
    Py_DECREF(expr_parts);
  }

  PyObject *root = PyUnicode_FromString("root");
  PyObject *part1 = _AoC_y2022_d21_eval(prog, root);

  PyObject *humn = PyUnicode_FromString("humn");
  PyDict_DelItem(prog, humn);
  PyObject *root_expr = PyDict_GetItem(prog, root);
  PyObject *lhs = PyTuple_GetItem(root_expr, 0);
  PyObject *rhs = PyTuple_GetItem(root_expr, 1);
  PyObject *lhs_val = _AoC_y2022_d21_eval(prog, lhs);
  PyObject *rhs_val = _AoC_y2022_d21_eval(prog, rhs);
  PyObject *part2 = _AoC_y2022_d21_solve_for(lhs_val, rhs_val, humn);

  solution = PyUnicode_FromFormat("%S %S", part1, part2);
  Py_DECREF(root);
  Py_DECREF(humn);
  Py_DECREF(part1);
  Py_DECREF(part2);

done:
  Py_XDECREF(lines);
  Py_XDECREF(prog);
  return solution;
}

#endif  // _AOC_Y2022_D21_H_INCLUDED
