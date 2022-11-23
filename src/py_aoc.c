#define PY_SSIZE_T_CLEAN
#include "Python.h"
#include "d01.h"

static PyObject *solve_aoc_y2022(PyObject *module, PyObject *args) {
  unsigned char day;
  PyObject *input;
  if (!PyArg_ParseTuple(args, "bU", &day, &input)) {
    PyErr_SetString(PyExc_RuntimeError,
                    "Failed parsing positional args as 'unsigned char' and Python Unicode object");
    goto error;
  }
  PySys_FormatStdout("day %u input %U\n", day, input);
  switch (day) {
    case 1: {
      return aoc_y2022_d01(input);
    }
    default: {
      // TODO add new exception to module
      PyErr_Format(PyExc_RuntimeError, "No solutions for day %u", day);
      goto error;
    }
  }

error:
  return NULL;
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
  return Py_RunMain();
}
