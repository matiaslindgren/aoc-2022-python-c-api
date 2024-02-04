#ifndef _AOC_Y2022_D07_H_INCLUDED
#define _AOC_Y2022_D07_H_INCLUDED
#include "common.h"

PyObject *_AoC_y2022_d07_path_size(PyObject *path, PyObject *children) {
  PyObject *path_size = PyLong_FromLong(0);
  PyObject *path_children = PyDict_GetItem(children, path);
  if (!path_children) {
    return path_size;
  }
  PyObject *child_path;
  PyObject *child_size;
  Py_ssize_t dict_items_pos = 0;
  while (
      PyDict_Next(path_children, &dict_items_pos, &child_path, &child_size)) {
    Py_SETREF(path_size, PyNumber_Add(path_size, child_size));
    if (PyDict_Contains(children, child_path)) {
      PyObject *subpath_size = _AoC_y2022_d07_path_size(child_path, children);
      Py_SETREF(path_size, PyNumber_Add(path_size, subpath_size));
    }
  }
  return path_size;
}

PyObject *_AoC_y2022_d07_format_path(PyObject *dirs, PyObject *stem) {
  PyObject *sep = PyUnicode_FromString("/");
  PyObject *path = PyUnicode_Join(sep, dirs);
  if (stem) {
    Py_SETREF(path, PyUnicode_Concat(path, sep));
    Py_SETREF(path, PyUnicode_Concat(path, stem));
  }
  Py_DECREF(sep);
  return path;
}

PyObject *AoC_y2022_d07(PyObject *unicode_input) {
  PyObject *solution = NULL;
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  PyObject *children = PyDict_New();
  PyObject *dir_stack = PyList_New(0);
  PyObject *all_paths = PySet_New(NULL);
  if (!lines || !children || !dir_stack || !all_paths) {
    goto done;
  }

  Py_ssize_t num_lines = PyList_Size(lines);
  for (Py_ssize_t i = 0; i < num_lines; ++i) {
    PyObject *line = PyList_GetItem(lines, i);
    if (PyUnicode_READ_CHAR(line, 0) == '$') {
      PyObject *cmd_parts = PyUnicode_Split(line, NULL, 2);
      PyObject *cmd = PyList_GetItem(cmd_parts, 1);
      if (AoC_PyUnicode_Equals_ASCII(cmd, "cd")) {
        PyObject *dir = PyList_GetItem(cmd_parts, 2);
        if (AoC_PyUnicode_Equals_ASCII(dir, "..")) {
          if (PySequence_DelItem(dir_stack, PyList_Size(dir_stack) - 1) < 0) {
            Py_DECREF(cmd_parts);
            goto done;
          }
        } else {
          if (PyList_Append(dir_stack, dir) < 0) {
            Py_DECREF(cmd_parts);
            goto done;
          }
          if (PySet_Add(all_paths,
                        _AoC_y2022_d07_format_path(dir_stack, NULL)) < 0) {
            goto done;
          }
        }
      } else if (AoC_PyUnicode_Equals_ASCII(cmd, "ls")) {
      } else {
        PyErr_Format(PyExc_RuntimeError, "unknown command %S\n", line);
        Py_DECREF(cmd_parts);
        goto done;
      }
      Py_DECREF(cmd_parts);
    } else {
      PyObject *ls_line_parts = PyUnicode_Split(line, NULL, 1);
      PyObject *lhs = PyList_GetItem(ls_line_parts, 0);
      PyObject *rhs = PyList_GetItem(ls_line_parts, 1);
      PyObject *size = NULL;
      if (AoC_PyUnicode_Equals_ASCII(lhs, "dir")) {
        size = PyLong_FromLong(0);
      } else {
        size = PyLong_FromUnicodeObject(lhs, 10);
      }
      PyObject *curdir_path = _AoC_y2022_d07_format_path(dir_stack, NULL);
      PyObject *curdir_children =
          PyDict_SetDefault(children, curdir_path, PyDict_New());
      PyObject *child_path = _AoC_y2022_d07_format_path(dir_stack, rhs);
      if (PyDict_SetItem(curdir_children, child_path, size) < 0) {
        Py_DECREF(ls_line_parts);
        goto done;
      }
      Py_DECREF(ls_line_parts);
    }
  }

  PyObject *root_path = PyUnicode_FromFormat("/");
  if (!PySet_Contains(all_paths, root_path)) {
    Py_DECREF(root_path);
    PyErr_Format(PyExc_RuntimeError, "expected a root dir (/) to be present");
    goto done;
  }
  PyObject *root_size = _AoC_y2022_d07_path_size(root_path, children);
  PyObject *max_usage = PyLong_FromLong(40000000);
  PyObject *space_to_del = PyNumber_Subtract(root_size, max_usage);
  Py_DECREF(max_usage);
  Py_DECREF(root_path);

  PyObject *part1 = PyLong_FromLong(0);
  PyObject *part2 = root_size;

  PyObject *part1_limit = PyLong_FromLong(100000);

  while (PySet_Size(all_paths)) {
    PyObject *path = PySet_Pop(all_paths);
    Py_INCREF(path);
    PyObject *path_size = _AoC_y2022_d07_path_size(path, children);
    if (PyObject_RichCompareBool(path_size, part1_limit, Py_LE) == 1) {
      Py_SETREF(part1, PyNumber_Add(part1, path_size));
    }
    if (PyObject_RichCompareBool(path_size, space_to_del, Py_GE) == 1) {
      if (PyObject_RichCompareBool(path_size, part2, Py_LT) == 1) {
        Py_SETREF(part2, path_size);
      }
    }
    Py_DECREF(path);
  }

  solution = PyUnicode_FromFormat("%S %S", part1, part2);
  Py_DECREF(part1);
  Py_DECREF(part2);
  Py_DECREF(part1_limit);
  Py_DECREF(space_to_del);
  Py_DECREF(root_size);

done:
  Py_XDECREF(all_paths);
  Py_XDECREF(dir_stack);
  Py_XDECREF(children);
  Py_XDECREF(lines);
  return solution;
}

#endif  // _AOC_Y2022_D07_H_INCLUDED
