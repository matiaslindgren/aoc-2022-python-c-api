#ifndef _AOC_COMMON_H_INCLUDED
#define _AOC_COMMON_H_INCLUDED
#include <stdio.h>

#define PY_SSIZE_T_CLEAN
#include "Python.h"

/*
 * https://docs.python.org/3/c-api/init_config.html#initialization-with-pyconfig
 */
PyStatus _AoC_init_python(int argc, char *const *argv) {
  PyConfig config;
  PyConfig_InitPythonConfig(&config);

  config.optimization_level = 2;
  config.bytes_warning = 2;

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

PyObject *AoC_slurp_file(PyObject *filename) {
  Py_INCREF(filename);
  const char *raw_filename = PyUnicode_AsUTF8(filename);
  if (!raw_filename || PyErr_Occurred()) {
    if (!PyErr_Occurred()) {
      PyErr_Format(PyExc_RuntimeError, "failed encoding filename '%S' as UTF-8\n", filename);
    }
    Py_DECREF(filename);
    return NULL;
  }

  FILE *fp = fopen(raw_filename, "r");
  if (!fp) {
    PyErr_Format(PyExc_OSError, "failed opening file '%S' for reading\n", filename);
    goto error;
  }

  if (fseek(fp, 0, SEEK_END)) {
    PyErr_Format(PyExc_OSError, "failed seeking to the end of '%S'\n", filename);
    goto error;
  }

  const long file_size = ftell(fp);
  if (file_size == -1) {
    PyErr_Format(PyExc_OSError, "could not get file size of '%S'\n", filename);
    goto error;
  }

  PyObject *py_buf = PyBytes_FromStringAndSize(NULL, file_size);
  if (!py_buf) {
    PyErr_NoMemory();
    goto error;
  }

  char *buf = PyBytes_AsString(py_buf);
  rewind(fp);
  size_t num_read = fread(buf, 1, file_size, fp);
  fclose(fp);
  // NOTE: wrong if file_size is larger than SIZE_MAX / 2
  if (num_read < (size_t)file_size) {
    PyErr_Format(PyExc_OSError,
                 "tried reading %ld bytes from '%S' but read only %lu\n",
                 file_size,
                 filename,
                 num_read);
    Py_DECREF(py_buf);
    goto error;
  }

  PyObject *file_data = PyUnicode_FromEncodedObject(py_buf, "utf-8", NULL);
  Py_DECREF(py_buf);
  if (!file_data) {
    goto error;
  }
  Py_DECREF(filename);
  return file_data;

error:
  if (fp) {
    fclose(fp);
  }
  Py_DECREF(filename);
  return NULL;
}

int _AoC_dump_check_lines(PyObject *lines) {
  if (!PyList_Check(lines)) {
    return 1;
  }
  int errors = 0;
  Py_ssize_t num_lines = PyList_Size(lines);
  for (Py_ssize_t i = 0; i < num_lines; ++i) {
    PyObject *line = PyList_GetItem(lines, i);
    if (!line) {
      PySys_FormatStdout("!! %zd: NULL\n", i);
      errors = 1;
    } else {
      PySys_FormatStdout("%zd: %S\n", i, line);
    }
  }
  return errors;
}

PyObject *AoC_unicode_split(PyObject *s, const char *sep, Py_ssize_t maxsplit) {
  PyObject *unicode_sep = PyUnicode_FromString(sep);
  PyObject *parts = PyUnicode_Split(s, unicode_sep, maxsplit);
  Py_DECREF(unicode_sep);
  if (!parts) {
    PyErr_Format(PyExc_RuntimeError,
                 "failed splitting input %zd times at '%S'",
                 maxsplit,
                 unicode_sep);
    return NULL;
  }
  return parts;
}

int AoC_PyUnicode_Equals_ASCII(PyObject *unicode, const char *str) {
  return PyUnicode_CompareWithASCIIString(unicode, str) == 0;
}

PyObject *AoC_PyLong_Add(PyObject *lhs_py, long rhs) {
  PyObject *rhs_py = PyLong_FromLong(rhs);
  if (!rhs_py) {
    return PyErr_Format(PyExc_RuntimeError, "failed creating PyLong object from %ld", rhs);
  }
  PyObject *res = PyNumber_Add(lhs_py, rhs_py);
  if (!res) {
    PyErr_Format(PyExc_RuntimeError, "failed computing %S + %S", lhs_py, rhs_py);
    Py_DECREF(rhs_py);
    return NULL;
  }
  Py_DECREF(lhs_py);
  Py_DECREF(rhs_py);
  return res;
}

#endif  // _AOC_COMMON_H_INCLUDED
