#ifndef _AOC_COMMON_H_INCLUDED
#define _AOC_COMMON_H_INCLUDED
#define PY_SSIZE_T_CLEAN
#include "Python.h"

// https://docs.python.org/3/c-api/init_config.html#initialization-with-pyconfig
PyStatus _aoc_common_init_python(int argc, char *const *argv) {
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

#endif  // _AOC_COMMON_H_INCLUDED
