#include "common.h"
#include "d01.h"
#include "d04.h"

static PyObject *_solve_aoc_y2022(unsigned char day, PyObject *unicode_input) {
  switch (day) {
    case 1: {
      return aoc_y2022_d01(unicode_input);
    }
    case 4: {
      return aoc_y2022_d04(unicode_input);
    }
  }
  return NULL;
}

static PyObject *solve_aoc_y2022(PyObject *module, PyObject *args) {
  unsigned char day;
  PyObject *unicode_input;
  if (!PyArg_ParseTuple(args, "bU", &day, &unicode_input)) {
    PyErr_SetString(PyExc_RuntimeError,
                    "Failed parsing positional args as 'unsigned char' and Python Unicode object");
    goto error;
  }

  PyObject *solution = _solve_aoc_y2022(day, unicode_input);
  if (solution == NULL) {
    // TODO add new exception to module
    PyErr_Format(PyExc_RuntimeError, "Failed to find solution for day %u", day);
    goto error;
  }
  return solution;

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

int main(int argc, char *const *argv) {
  // Make 'solve_aoc' available as a builtin module
  if (PyImport_AppendInittab(solve_aoc_module.m_name, PyInit_solve_aoc) == -1) {
    PyErr_Print();
    fprintf(stderr, "Failed to extend builtin modules table with 'solve_aoc'\n");
    exit(1);
  }
  PyStatus status = _aoc_common_init_python(argc, argv);
  if (PyStatus_Exception(status)) {
    Py_ExitStatusException(status);
  }
  return Py_RunMain();
}
