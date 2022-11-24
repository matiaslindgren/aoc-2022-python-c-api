#ifndef _AOC_COMMON_H_INCLUDED
#define _AOC_COMMON_H_INCLUDED
#define PY_SSIZE_T_CLEAN
#include "Python.h"

// https://docs.python.org/3/c-api/init_config.html#initialization-with-pyconfig
PyStatus _AoC_init_python(int argc, char *const *argv) {
  PyConfig config;
  PyConfig_InitPythonConfig(&config);

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

int _AoC_list_contains_null(PyObject *list) {
  Py_ssize_t n = PyList_Size(list);
  for (Py_ssize_t i = 0; i < n; ++i) {
    PyObject *item = PyList_GetItem(list, i);
    if (item == NULL) {
      return 1;
    }
  }
  return 0;
}

#endif  // _AOC_COMMON_H_INCLUDED
