#include "common.h"
#include "d01.h"
#include "d04.h"

static PyObject *_AoC_solve_y2022(unsigned char day, PyObject *unicode_input) {
  switch (day) {
    case 1: {
      return AoC_y2022_d01(unicode_input);
    }
    case 4: {
      return AoC_y2022_d04(unicode_input);
    }
  }
  return NULL;
}

static PyObject *AoC_solve_y2022(PyObject *module, PyObject *args) {
  unsigned char day;
  PyObject *unicode_input;
  if (!PyArg_ParseTuple(args, "bU", &day, &unicode_input)) {
    PyErr_SetString(PyExc_RuntimeError,
                    "Failed parsing positional args as 'unsigned char' and Python Unicode object");
    goto error;
  }

  PyObject *solution = _AoC_solve_y2022(day, unicode_input);
  if (solution == NULL) {
    // TODO add new exception to module
    PyErr_Format(PyExc_RuntimeError, "Failed to find solution for day %u", day);
    goto error;
  }
  return solution;

error:
  return NULL;
}

// Methods available in module 'aoc_solve'
static PyMethodDef AoC_solve_methods[] = {
    {
        "y2022",
        (PyCFunction)AoC_solve_y2022,
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

// Definition of module 'aoc_solve'
static struct PyModuleDef AoC_solve_module = {
    PyModuleDef_HEAD_INIT, "aoc_solve", "Advent of Code solvers", -1, AoC_solve_methods,
};

PyMODINIT_FUNC PyInit_AoC_solve(void) { return PyModule_Create(&AoC_solve_module); }

int main(int argc, char *const *argv) {
  // Make 'aoc_solve' available as a builtin module
  if (PyImport_AppendInittab(AoC_solve_module.m_name, PyInit_AoC_solve) == -1) {
    PyErr_Print();
    fprintf(stderr, "Failed to extend builtin modules table with 'aoc_solve'\n");
    exit(1);
  }
  PyStatus status = _AoC_init_python(argc, argv);
  if (PyStatus_Exception(status)) {
    Py_ExitStatusException(status);
  }
  return Py_RunMain();
}
