#ifndef _AOC_COMMON_H_INCLUDED
#define _AOC_COMMON_H_INCLUDED
#define PY_SSIZE_T_CLEAN
#include "Python.h"

// https://docs.python.org/3/c-api/init_config.html#initialization-with-pyconfig
PyStatus _AoC_init_python(int argc, char *const *argv) {
  PyConfig config;
  PyConfig_InitPythonConfig(&config);

  config.optimization_level = 2;

  PyStatus status = PyConfig_SetBytesArgv(&config, argc, argv);
  if (PyStatus_Exception(status)) {
    goto done;
  }

  status = PyConfig_Read(&config);
  if (PyStatus_Exception(status)) {
    goto done;
  }

  status = Py_InitializeFromConfig(&config);

done:
  PyConfig_Clear(&config);
  return status;
}

void _AoC_debug_dump_lines(PyObject *lines) {
  Py_ssize_t num_lines = PyList_Size(lines);
  for (Py_ssize_t i = 0; i < num_lines; ++i) {
    PyObject *line = PyList_GetItem(lines, i);
    PySys_FormatStdout("%zd: %U\n", i, line);
  }
}

int _AoC_list_is_ok(PyObject *list) {
  if (!PyList_CheckExact(list)) {
    return 0;
  }
  Py_ssize_t n = PyList_Size(list);
  for (Py_ssize_t i = 0; i < n; ++i) {
    PyObject *item = PyList_GetItem(list, i);
    if (item == NULL) {
      return 0;
    }
  }
  return 1;
}

PyObject *AoC_unicode_split(PyObject *s, const char *sep, Py_ssize_t maxsplit) {
  PyObject *unicode_sep = PyUnicode_FromString(sep);
  PyObject *parts = PyUnicode_Split(s, unicode_sep, maxsplit);
  Py_DECREF(unicode_sep);
  return parts;
}

#endif  // _AOC_COMMON_H_INCLUDED
