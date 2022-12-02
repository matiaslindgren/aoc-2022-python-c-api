#ifndef _AOC_Y2022_D02_H_INCLUDED
#define _AOC_Y2022_D02_H_INCLUDED
#include "common.h"

int _AoC_y2022_d02_encode_hand(int rock, int paper, int scissors) {
  return rock + 2 * paper + 3 * scissors;
}

PyObject *AoC_y2022_d02(PyObject *unicode_input) {
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  if (!lines) {
    PyErr_Format(PyExc_RuntimeError, "could not split input to lines");
    goto error;
  }

  const long draw_points = 3;
  const long win_points = 6;
  long part1 = 0;
  long part2 = 0;
  Py_ssize_t num_lines = PyList_Size(lines);
  for (Py_ssize_t i = 0; i < num_lines; ++i) {
    PyObject *line = PyList_GetItem(lines, i);
    Py_INCREF(line);
    PyObject *pair = AoC_unicode_split(line, " ", 1);
    Py_DECREF(line);
    PyObject *lhs = PyList_GetItem(pair, 0);
    Py_INCREF(lhs);
    PyObject *rhs = PyList_GetItem(pair, 1);
    Py_INCREF(rhs);
    Py_DECREF(pair);
    int lhs_a = AoC_PyUnicode_Equals_ASCII(lhs, "A");
    int lhs_b = AoC_PyUnicode_Equals_ASCII(lhs, "B");
    int lhs_c = AoC_PyUnicode_Equals_ASCII(lhs, "C");
    Py_DECREF(lhs);
    int rhs_x = AoC_PyUnicode_Equals_ASCII(rhs, "X");
    int rhs_y = AoC_PyUnicode_Equals_ASCII(rhs, "Y");
    int rhs_z = AoC_PyUnicode_Equals_ASCII(rhs, "Z");
    Py_DECREF(rhs);
    int lhs_hand = _AoC_y2022_d02_encode_hand(lhs_a, lhs_b, lhs_c);
    int rhs_hand = _AoC_y2022_d02_encode_hand(rhs_x, rhs_y, rhs_z);
    int hand_to_win_lhs = lhs_hand % 3 + 1;
    int hand_to_lose_lhs = (lhs_hand + 1) % 3 + 1;
    part1 += rhs_hand;
    part1 += draw_points * (rhs_hand == lhs_hand);
    part1 += win_points * (rhs_hand == hand_to_win_lhs);
    part2 += rhs_x * hand_to_lose_lhs;
    part2 += rhs_y * (draw_points + lhs_hand);
    part2 += rhs_z * (win_points + hand_to_win_lhs);
  }
  Py_DECREF(lines);

  PyObject *part1_py = PyLong_FromLong(part1);
  PyObject *part2_py = PyLong_FromLong(part2);
  PyObject *solution = PyUnicode_FromFormat("%S %S", part1_py, part2_py);
  Py_DECREF(part1_py);
  Py_DECREF(part2_py);
  return solution;

error:
  return NULL;
}
#endif  // _AOC_Y2022_D02_H_INCLUDED
