#ifndef _AOC_Y2022_D05_H_INCLUDED
#define _AOC_Y2022_D05_H_INCLUDED
#include "common.h"

PyObject *_AoC_y2022_d05_unicode_concat(PyObject *lhs, PyObject *rhs) {
  PyObject *tmp = PyUnicode_Concat(lhs, rhs);
  Py_SETREF(lhs, tmp);
  return lhs;
}

PyObject *AoC_y2022_d05(PyObject *unicode_input) {
  const Py_ssize_t num_stacks = 9;

  PyObject *stacks = PyList_New(0);
  if (!stacks) {
    goto done;
  }
  PyObject *sections = NULL;
  PyObject *moves = NULL;
  PyObject *solution = NULL;

  enum parts {
    part1 = 0,
    part2 = 1,
    num_parts = 2,
  };

  for (Py_ssize_t i = 0; i < num_parts * num_stacks; ++i) {
    if (PyList_Append(stacks, PyList_New(0)) < 0) {
      goto done;
    }
  }

  sections = AoC_unicode_split(unicode_input, "\n\n", 1);
  if (!sections) {
    goto done;
  }

  PyObject *stacks_str = PyList_GetItem(sections, 0);
  const Py_ssize_t stack_str_len = PyUnicode_GetLength(stacks_str);
  for (size_t stack_idx = 0; stack_idx < num_stacks; ++stack_idx) {
    for (Py_ssize_t i = 1 + 4 * stack_idx; i < stack_str_len;
         i += 4 * num_stacks) {
      if (Py_UNICODE_ISALPHA(PyUnicode_ReadChar(stacks_str, i))) {
        PyObject *crate = PyUnicode_Substring(stacks_str, i, i + 1);
        if (!crate) {
          goto done;
        }
        for (Py_ssize_t part = part1; part < num_parts; ++part) {
          PyObject *stack =
              PyList_GetItem(stacks, part * num_stacks + stack_idx);
          if (stack) {
            if (!(PyList_Append(stack, crate) < 0)) {
              continue;
            }
          }
          goto done;
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
    PyObject *move_count_py =
        PyLong_FromUnicodeObject(PyList_GetItem(move_parts, 1), 10);
    PyObject *src_py =
        PyLong_FromUnicodeObject(PyList_GetItem(move_parts, 3), 10);
    PyObject *dst_py =
        PyLong_FromUnicodeObject(PyList_GetItem(move_parts, 5), 10);
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
    for (Py_ssize_t part = part1; part < num_parts; ++part) {
      PyObject *src_stack = PyList_GetItem(stacks, part * num_stacks + src);
      PyObject *dst_stack = PyList_GetItem(stacks, part * num_stacks + dst);
      if (!src_stack || !dst_stack) {
        goto done;
      }
      for (long num_left = move_count; num_left > 0; --num_left) {
        const Py_ssize_t crate_pos = (part == part2) ? num_left - 1 : 0;
        PyObject *crate = PyList_GetItem(src_stack, crate_pos);
        if (!crate) {
          goto done;
        }
        if (PyList_Insert(dst_stack, 0, crate) < 0) {
          goto done;
        }
        if (PySequence_DelItem(src_stack, crate_pos) < 0) {
          goto done;
        }
      }
    }
  }

  solution = PyUnicode_FromString("");
  for (Py_ssize_t part = 0; part < num_parts; ++part) {
    for (Py_ssize_t i = 0; i < num_stacks; ++i) {
      PyObject *stack = PyList_GetItem(stacks, part * num_stacks + i);
      PyObject *top_crate = PyList_GetItem(stack, 0);
      solution = _AoC_y2022_d05_unicode_concat(solution, top_crate);
    }
    solution =
        _AoC_y2022_d05_unicode_concat(solution, PyUnicode_FromString(" "));
  }

done:
  if (stacks) {
    for (Py_ssize_t i = 0; i < PyList_Size(stacks); ++i) {
      Py_XDECREF(PyList_GetItem(stacks, i));
    }
  }
  Py_XDECREF(stacks);
  Py_XDECREF(sections);
  Py_XDECREF(moves);
  return solution;
}

#endif  // _AOC_Y2022_D05_H_INCLUDED
