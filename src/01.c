#define PY_SSIZE_T_CLEAN
#include "Python.h"

int main(int argc, char **argv) {
  PyStatus status;
  PyConfig config;
  PyConfig_InitPythonConfig(&config);
  config.isolated = 1;
  status = PyConfig_SetBytesArgv(&config, argc, argv);
  if (PyStatus_Exception(status)) {
    goto exception;
  }
  status = Py_InitializeFromConfig(&config);
  if (PyStatus_Exception(status)) {
    goto exception;
  }
  PyConfig_Clear(&config);
  return Py_RunMain();

exception:
  PyConfig_Clear(&config);
  if (PyStatus_IsExit(status)) {
    return status.exitcode;
  }
  Py_ExitStatusException(status);
}
