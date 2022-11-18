#define PY_SSIZE_T_CLEAN
#include <Python.h>

int main(int argc, char *argv[]) {
  wchar_t *program = Py_DecodeLocale(argv[0], NULL);
  if (program == NULL) {
    fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
    exit(1);
  }

  PyConfig config;
  PyConfig_InitPythonConfig(&config);

  PyStatus status = PyConfig_SetString(&config, &config.program_name, program);
  if (PyStatus_Exception(status)) {
    goto exception;
  }
  status = Py_InitializeFromConfig(&config);
  if (PyStatus_Exception(status)) {
    goto exception;
  }

  int res = PyRun_SimpleString(
      "from time import time,ctime\n"
      "print('Today is', ctime(time()))\n");
  if (res < 0) {
    exit(1);
  }

  PyConfig_Clear(&config);
  return 0;

exception:
  PyConfig_Clear(&config);
  Py_ExitStatusException(status);
  return 1;
}
