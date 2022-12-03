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

  config.malloc_stats = 1;
  config.show_ref_count = 1;

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

PyObject *AoC_slurp_file(const char *filename) {
  FILE *fp = fopen(filename, "r");
  if (!fp) {
    PyErr_Format(PyExc_OSError, "failed opening file '%s' for reading\n", filename);
    goto error;
  }

  if (fseek(fp, 0, SEEK_END)) {
    PyErr_Format(PyExc_OSError, "failed seeking to the end of '%s'\n", filename);
    goto error;
  }

  const long file_size = ftell(fp);
  if (file_size == -1) {
    PyErr_Format(PyExc_OSError, "could not get file size of '%s'\n", filename);
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
    PyErr_Format(PyExc_OSError, "tried reading %ld bytes from '%s' but read only %lu\n", file_size,
                 filename, num_read);
    Py_DECREF(py_buf);
    goto error;
  }

  PyObject *file_data = PyUnicode_FromEncodedObject(py_buf, "utf-8", NULL);
  Py_DECREF(py_buf);
  if (!file_data) {
    goto error;
  }
  return file_data;

error:
  if (fp) {
    fclose(fp);
  }
  return NULL;
}

void _AoC_debug_dump_lines(PyObject *lines) {
  Py_ssize_t num_lines = PyList_Size(lines);
  for (Py_ssize_t i = 0; i < num_lines; ++i) {
    PyObject *line = PyList_GetItem(lines, i);
    PySys_FormatStdout("%zd: %S\n", i, line);
  }
}

int _AoC_list_is_ok(PyObject *list) {
  if (!PyList_CheckExact(list)) {
    return 0;
  }
  Py_ssize_t n = PyList_Size(list);
  for (Py_ssize_t i = 0; i < n; ++i) {
    PyObject *item = PyList_GetItem(list, i);
    if (item == NULL) {
      return 0;
    }
  }
  return 1;
}

PyObject *AoC_unicode_split(PyObject *s, const char *sep, Py_ssize_t maxsplit) {
  PyObject *unicode_sep = PyUnicode_FromString(sep);
  // TODO do we need to check for NULL from PyUnicode_Split?
  PyObject *parts = PyUnicode_Split(s, unicode_sep, maxsplit);
  Py_DECREF(unicode_sep);
  return parts;
}

PyObject *AoC_unicode_split_sections(PyObject *s) { return AoC_unicode_split(s, "\n\n", -1); }

int AoC_PyUnicode_Equals_ASCII(PyObject *unicode, const char *str) {
  return PyUnicode_CompareWithASCIIString(unicode, str) == 0;
}

#endif  // _AOC_COMMON_H_INCLUDED
