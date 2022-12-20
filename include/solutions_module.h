#ifndef _AOC_Y2022_SOLUTIONS_H_INCLUDED
#define _AOC_Y2022_SOLUTIONS_H_INCLUDED
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
#include "d09.h"
#include "d10.h"
#include "d11.h"
#include "d12.h"
#include "d13.h"  // NOTE: defines a custom PyTypeObject
#include "d14.h"
#include "d15.h"
#include "d17.h"
#include "d18.h"
#include "d20.h"

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
    case 9:
      return AoC_y2022_d09(unicode_input);
    case 10:
      return AoC_y2022_d10(unicode_input);
    case 11:
      return AoC_y2022_d11(unicode_input);
    case 12:
      return AoC_y2022_d12(unicode_input);
    case 13:
      return AoC_y2022_d13(unicode_input);
    case 14:
      return AoC_y2022_d14(unicode_input);
    case 15:
      return AoC_y2022_d15(unicode_input);
    case 17:
      return AoC_y2022_d17(unicode_input);
    case 18:
      return AoC_y2022_d18(unicode_input);
    case 20:
      return AoC_y2022_d20(unicode_input);
  }
  return PyErr_Format(PyExc_ValueError,
                      "no solution implemented for day %d",
                      day);
}

static PyObject *AoC_solve_y2022(PyObject *module, PyObject *args) {
  int day;
  if (!PyArg_ParseTuple(args, "i", &day)) {
    PyErr_SetString(PyExc_RuntimeError,
                    "failed parsing positional arg 'day' as int");
    goto error;
  }
  if (!(1 <= day && day <= 25)) {
    PyErr_Format(PyExc_ValueError,
                 "positional arg 'day' must be in range [1, 25], not %d",
                 day);
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
    PyErr_Format(PyExc_RuntimeError,
                 "unknown failure when solving day %d",
                 day);
  }
  return 0;
}

// Methods available in module 'aoc_solve'
static PyMethodDef AoC_solve_methods[] = {
    {
        "y2022",
        (PyCFunction)AoC_solve_y2022,
        METH_VARARGS,
        "Advent of Code 2022 solver",
    },
    // Custom modules end
    {
        0,
        0,
        0,
        0,
    },
};

// Definition of module 'aoc_solve'
static struct PyModuleDef AoC_solve_module = {
    PyModuleDef_HEAD_INIT,
    "aoc_solve",
    "Advent of Code solvers",
    -1,
    AoC_solve_methods,
};

PyMODINIT_FUNC PyInit_AoC_solve(void) {
  PyObject *module = 0;
  if (PyType_Ready(&AoC_y2022_d13_PacketType) < 0) {
    return 0;
  }

  module = PyModule_Create(&AoC_solve_module);
  if (!module) {
    return 0;
  }

  Py_INCREF(&AoC_y2022_d13_PacketType);
  if (PyModule_AddObject(module,
                         AoC_y2022_d13_PacketType.tp_name,
                         (PyObject *)&AoC_y2022_d13_PacketType) < 0) {
    Py_DECREF(&AoC_y2022_d13_PacketType);
    Py_DECREF(module);
    return 0;
  }

  return module;
}

PyStatus _AoC_init_python(const int argc, char *const *argv, const int debug) {
  /*
   * https://docs.python.org/3/c-api/init_config.html#initialization-with-pyconfig
   */
  PyConfig config;
  PyConfig_InitPythonConfig(&config);

  if (debug) {
    config.optimization_level = 0;
    config.bytes_warning = 2;
    config.tracemalloc = 1;
    config.malloc_stats = 1;
    config.show_ref_count = 1;
    config.dump_refs = 1;
  } else {
    config.optimization_level = 2;
    config.bytes_warning = 2;
  }

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
  // Make 'aoc_solve' available as a builtin module
  if (PyImport_AppendInittab(AoC_solve_module.m_name, PyInit_AoC_solve) == -1) {
    PyErr_Print();
    fprintf(stderr,
            "Failed to extend builtin modules table with '%s'\n",
            AoC_solve_module.m_name);
    return EXIT_FAILURE;
  }
  PyStatus status = _AoC_init_python(argc, argv, _AOC_Y2022_DEBUG_MAIN);
  if (PyStatus_Exception(status)) {
    Py_ExitStatusException(status);
  }
  return Py_RunMain();
}

#endif  // _AOC_Y2022_SOLUTIONS_H_INCLUDED
