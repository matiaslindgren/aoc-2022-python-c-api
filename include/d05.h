#ifndef _AOC_Y2022_D05_H_INCLUDED
#define _AOC_Y2022_D05_H_INCLUDED
#include "common.h"

PyObject *_AoC_y2022_d05_unicode_concat(PyObject *lhs, PyObject *rhs) {
  PyObject *tmp = PyUnicode_Concat(lhs, rhs);
  Py_SETREF(lhs, tmp);
  return lhs;
}

enum parts { part1, part2, num_parts };

PyObject *AoC_y2022_d05(PyObject *unicode_input) {
  const Py_ssize_t num_stacks = 9;
  // these need to be DECREF'd on return
  PyObject *stacks[num_parts][num_stacks];
  PyObject *sections = NULL;
  PyObject *moves = NULL;
  PyObject *solution = NULL;

  for (Py_ssize_t i = 0; i < num_stacks; ++i) {
    for (size_t part = part1; part < num_parts; ++part) {
      stacks[part][i] = PyList_New(0);
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
        for (size_t part = part1; part < num_parts; ++part) {
          if (PyList_Append(stacks[part][stack_idx], crate) == -1) {
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
    for (size_t part = part1; part < num_parts; ++part) {
      for (long num_left = move_count; num_left > 0; --num_left) {
        const Py_ssize_t crate_pos = (part == part2) ? num_left - 1 : 0;
        PyObject *crate = PyList_GetItem(stacks[part][src], crate_pos);
        if (PyList_Insert(stacks[part][dst], 0, crate) == -1) {
          goto done;
        }
        if (PySequence_DelItem(stacks[part][src], crate_pos) == -1) {
          goto done;
        }
      }
    }
  }

  solution = PyUnicode_FromString("");
  for (size_t part = part1; part < num_parts; ++part) {
    for (Py_ssize_t i = 0; i < num_stacks; ++i) {
      PyObject *top_crate = PyList_GetItem(stacks[part][i], 0);
      solution = _AoC_y2022_d05_unicode_concat(solution, top_crate);
    }
    solution = _AoC_y2022_d05_unicode_concat(solution, PyUnicode_FromString(" "));
  }

done:
  if (sections) {
    Py_DECREF(sections);
  }
  if (moves) {
    Py_DECREF(moves);
  }
  for (size_t part = part1; part < num_parts; ++part) {
    for (size_t stack_idx = 0; stack_idx < num_stacks; ++stack_idx) {
      PyObject *stack = stacks[part][stack_idx];
      for (Py_ssize_t i = 0; i < PyList_Size(stack); ++i) {
        Py_DECREF(PyList_GetItem(stack, i));
      }
      Py_DECREF(stack);
    }
  }
  return solution;
}

#endif  // _AOC_Y2022_D05_H_INCLUDED
