#define PY_SSIZE_T_CLEAN
#include "Python.h"

static PyObject *solve_aoc_y2022(PyObject *module, PyObject *args) {
  unsigned char day;
  const char *input;
  if (!PyArg_ParseTuple(args, "bs", &day, &input)) {
    PyErr_SetString(PyExc_RuntimeError,
                    "Failed parsing positional args as 'unsigned char' and 'const char*'");
    return NULL;
  }
  if (day < 1 || 25 < day) {
    PyErr_SetString(PyExc_RuntimeError, "Argument 'day' must be in range [1, 25]");
    return NULL;
  }
  printf("day %u\n", day);
  printf("input %s\n", input);
  PyObject *solution;
  solution = Py_BuildValue("s", "TODO result");
  return solution;
}

// Methods available in module 'solve_aoc'
static PyMethodDef solve_aoc_methods[] = {
    {
        "y2022",
        (PyCFunction)solve_aoc_y2022,
        METH_VARARGS,
        "Advent of Code 2022 solver",
    },
    // End of array sentinel
    {
        NULL,
        NULL,
        0,
        NULL,
    },
};

// Definition of module 'solve_aoc'
static struct PyModuleDef solve_aoc_module = {
    PyModuleDef_HEAD_INIT, "solve_aoc", "Advent of Code solvers", -1, solve_aoc_methods,
};

PyMODINIT_FUNC PyInit_solve_aoc(void) { return PyModule_Create(&solve_aoc_module); }

// https://docs.python.org/3/c-api/init_config.html#initialization-with-pyconfig
PyStatus _init_python(int argc, char *const *argv) {
  PyStatus status;

  PyConfig config;
  PyConfig_InitPythonConfig(&config);
  const char *program_name = argv[0];
  status = PyConfig_SetBytesString(&config, &config.program_name, program_name);
  if (PyStatus_Exception(status)) {
    goto done;
  }

  status = PyConfig_SetBytesArgv(&config, argc, argv);
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

int main(int argc, char *const *argv) {
  // Make 'solve_aoc' available as a builtin module
  if (PyImport_AppendInittab(solve_aoc_module.m_name, PyInit_solve_aoc) == -1) {
    PyErr_Print();
    fprintf(stderr, "Failed to extend builtin modules table with 'solve_aoc'\n");
    exit(1);
  }
  PyStatus status = _init_python(argc, argv);
  if (PyStatus_Exception(status)) {
    Py_ExitStatusException(status);
  }
  // TODO module is not in globals
  if (!PyImport_ImportModule(solve_aoc_module.m_name)) {
    PyErr_Print();
    fprintf(stderr, "Failed to import module '%s'\n", solve_aoc_module.m_name);
    exit(1);
  }
  return Py_RunMain();
}
