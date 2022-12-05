#ifndef _AOC_Y2022_D05_H_INCLUDED
#define _AOC_Y2022_D05_H_INCLUDED
#include "common.h"

PyObject *AoC_y2022_d05(PyObject *unicode_input) {
  const Py_ssize_t num_stacks = 9;
  // these need to be DECREF'd on return
  PyObject *stacks[2][num_stacks];
  PyObject *sections = NULL;
  PyObject *moves = NULL;
  PyObject *solution = NULL;

  for (Py_ssize_t i = 0; i < num_stacks; ++i) {
    for (size_t part_idx = 0; part_idx < 2; ++part_idx) {
      stacks[part_idx][i] = PyList_New(0);
    }
  }

  sections = AoC_unicode_split(unicode_input, "\n\n", 1);
  if (!sections) {
    goto done;
  }

  PyObject *stacks_str = PyList_GetItem(sections, 0);
  const Py_ssize_t stack_str_len = PyUnicode_GET_LENGTH(stacks_str);
  for (size_t stack_idx = 0; stack_idx < num_stacks; ++stack_idx) {
    for (Py_ssize_t i = 1 + 4 * stack_idx; i < stack_str_len; i += 4 * num_stacks) {
      if (Py_UNICODE_ISALPHA(PyUnicode_ReadChar(stacks_str, i))) {
        PyObject *crate = PyUnicode_Substring(stacks_str, i, i + 1);
        if (!crate) {
          goto done;
        }
        for (size_t part_idx = 0; part_idx < 2; ++part_idx) {
          if (PyList_Append(stacks[part_idx][stack_idx], crate) == -1) {
            goto done;
          }
        }
      }
    }
  }

  moves = PyUnicode_Splitlines(PyList_GetItem(sections, 1), 0);
  if (!moves) {
    goto done;
  }
  for (Py_ssize_t i = 0; i < PyList_Size(moves); ++i) {
    PyObject *move_parts = PyUnicode_Split(PyList_GetItem(moves, i), NULL, -1);
    if (!move_parts) {
      goto done;
    }
    PyObject *move_count_py = PyLong_FromUnicodeObject(PyList_GetItem(move_parts, 1), 10);
    PyObject *src_py = PyLong_FromUnicodeObject(PyList_GetItem(move_parts, 3), 10);
    PyObject *dst_py = PyLong_FromUnicodeObject(PyList_GetItem(move_parts, 5), 10);
    Py_DECREF(move_parts);
    const long move_count = PyLong_AsLong(move_count_py);
    const size_t src = PyLong_AsSize_t(src_py) - 1;
    const size_t dst = PyLong_AsSize_t(dst_py) - 1;
    Py_DECREF(move_count_py);
    Py_DECREF(src_py);
    Py_DECREF(dst_py);
    if (PyErr_Occurred()) {
      goto done;
    }
    for (long num_left = move_count; num_left > 0; --num_left) {
      PyObject *crate = PyList_GetItem(stacks[0][src], 0);
      if (PyList_Insert(stacks[0][dst], 0, crate) == -1) {
        goto done;
      }
      if (PySequence_DelItem(stacks[0][src], 0) == -1) {
        goto done;
      }
    }
    for (long num_left = move_count; num_left > 0; --num_left) {
      const Py_ssize_t last_crate_pos = num_left - 1;
      PyObject *crate = PyList_GetItem(stacks[1][src], last_crate_pos);
      if (PyList_Insert(stacks[1][dst], 0, crate) == -1) {
        goto done;
      }
      if (PySequence_DelItem(stacks[1][src], last_crate_pos) == -1) {
        goto done;
      }
    }
  }

  solution = PyUnicode_FromString("");
  for (size_t part_idx = 0; part_idx < 2; ++part_idx) {
    for (Py_ssize_t i = 0; i < num_stacks; ++i) {
      PyObject *top_crate = PyList_GetItem(stacks[part_idx][i], 0);
      PyObject *tmp = PyUnicode_Concat(solution, top_crate);
      Py_SETREF(solution, tmp);
    }
    PyObject *sep = PyUnicode_FromString(" ");
    PyObject *tmp = PyUnicode_Concat(solution, sep);
    Py_DECREF(sep);
    Py_SETREF(solution, tmp);
  }

done:
  if (sections) {
    Py_DECREF(sections);
  }
  if (moves) {
    Py_DECREF(moves);
  }
  for (size_t part_idx = 0; part_idx < 2; ++part_idx) {
    for (size_t stack_idx = 0; stack_idx < num_stacks; ++stack_idx) {
      PyObject *stack = stacks[part_idx][stack_idx];
      for (Py_ssize_t i = 0; i < PyList_Size(stack); ++i) {
        Py_DECREF(PyList_GetItem(stack, i));
      }
      Py_DECREF(stack);
    }
  }
  return solution;
}

#endif  // _AOC_Y2022_D05_H_INCLUDED
