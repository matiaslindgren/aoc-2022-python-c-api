#include <stdio.h>

#include "common.h"
#include "d01.h"
#include "d02.h"
#include "d03.h"
#include "d04.h"
#include "d05.h"
#include "d06.h"
#include "d07.h"
#include "d08.h"

static PyObject *_AoC_solve_y2022(int day, PyObject *unicode_input) {
  switch (day) {
    case 1:
      return AoC_y2022_d01(unicode_input);
    case 2:
      return AoC_y2022_d02(unicode_input);
    case 3:
      return AoC_y2022_d03(unicode_input);
    case 4:
      return AoC_y2022_d04(unicode_input);
    case 5:
      return AoC_y2022_d05(unicode_input);
    case 6:
      return AoC_y2022_d06(unicode_input);
    case 7:
      return AoC_y2022_d07(unicode_input);
    case 8:
      return AoC_y2022_d08(unicode_input);
  }
  return PyErr_Format(PyExc_ValueError, "no solution implemented for day %d", day);
}

static PyObject *AoC_solve_y2022(PyObject *module, PyObject *args) {
  int day;
  if (!PyArg_ParseTuple(args, "i", &day)) {
    PyErr_SetString(PyExc_RuntimeError, "failed parsing positional arg 'day' as int");
    goto error;
  }
  if (!(1 <= day && day <= 25)) {
    PyErr_Format(PyExc_ValueError, "positional arg 'day' must be in range [1, 25], not %d", day);
    goto error;
  }

  PyObject *input_path = PyUnicode_FromFormat("./txt/input/%.2d.txt", day);
  if (!input_path) {
    goto error;
  }
  PyObject *unicode_input = AoC_slurp_file(input_path);
  Py_DECREF(input_path);
  if (!unicode_input) {
    goto error;
  }

  PyObject *solution = _AoC_solve_y2022(day, unicode_input);
  Py_DECREF(unicode_input);
  if (!solution) {
    goto error;
  }
  return solution;

error:
  if (!PyErr_Occurred()) {
    PyErr_Format(PyExc_RuntimeError, "unknown failure when solving day %d", day);
  }
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
    fprintf(stderr, "Failed to extend builtin modules table with '%s'\n", AoC_solve_module.m_name);
    exit(1);
  }
  PyStatus status = _AoC_init_python(argc, argv);
  if (PyStatus_Exception(status)) {
    Py_ExitStatusException(status);
  }
  return Py_RunMain();
}
