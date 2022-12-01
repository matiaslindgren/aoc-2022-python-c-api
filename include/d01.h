#ifndef _AOC_Y2022_D01_H_INCLUDED
#define _AOC_Y2022_D01_H_INCLUDED
#include "common.h"

PyObject *_AoC_y2022_d01_parse_one_section(PyObject *section_str) {
  PyObject *calories_str = AoC_unicode_split(section_str, "\n", -1);
  PyObject *calories = PyList_New(0L);
  Py_ssize_t num_calories = PyList_Size(calories_str);
  for (Py_ssize_t i = 0; i < num_calories; ++i) {
    PyObject *cal_str = PyList_GetItem(calories_str, i);
    if (PyUnicode_GetLength(cal_str) > 0) {
      Py_INCREF(cal_str);
      PyObject *cal = PyLong_FromUnicodeObject(cal_str, 10);
      Py_DECREF(cal_str);
      int list_error = PyList_Append(calories, cal);
      Py_DECREF(cal);
      if (list_error == -1) {
        goto error;
      }
    }
  }
  Py_DECREF(calories_str);
  return calories;

error:
  Py_DECREF(calories_str);
  Py_DECREF(calories);
  return NULL;
}

PyObject *_AoC_y2022_d01_parse_sections(PyObject *unicode_input) {
  PyObject *sections_str = AoC_unicode_split_sections(unicode_input);
  PyObject *sections = PyList_New(0L);
  Py_ssize_t num_sections = PyList_Size(sections_str);
  for (Py_ssize_t i = 0; i < num_sections; ++i) {
    PyObject *section_str = PyList_GetItem(sections_str, i);
    Py_INCREF(section_str);
    PyObject *section = _AoC_y2022_d01_parse_one_section(section_str);
    Py_DECREF(section_str);
    if (!section) {
      goto error;
    }
    int list_error = PyList_Append(sections, section);
    Py_DECREF(section);
    if (list_error == -1) {
      goto error;
    }
  }
  Py_DECREF(sections_str);
  return sections;

error:
  Py_DECREF(sections_str);
  Py_DECREF(sections);
  return NULL;
}

PyObject *AoC_y2022_d01(PyObject *unicode_input) {
  PyObject *sections = _AoC_y2022_d01_parse_sections(unicode_input);
  Py_ssize_t num_sections = PyList_Size(sections);
  PyObject *calorie_sums = PyList_New(num_sections);
  for (Py_ssize_t i_section = 0; i_section < num_sections; ++i_section) {
    PyObject *section = PyList_GetItem(sections, i_section);
    Py_INCREF(section);
    PyObject *sum = PyLong_FromLong(0L);
    if (!sum) {
      Py_DECREF(section);
      goto error;
    }
    Py_ssize_t num_calories = PyList_Size(section);
    for (Py_ssize_t i = 0; i < num_calories; ++i) {
      PyObject *cal = PyList_GetItem(section, i);
      PyObject *res = PyNumber_Add(sum, cal);
      Py_SETREF(sum, res);
    }
    Py_DECREF(section);
    int list_error = PyList_SetItem(calorie_sums, i_section, sum);
    if (list_error == -1) {
      goto error;
    }
  }
  Py_DECREF(sections);

  if (PyList_Sort(calorie_sums) == -1) {
    goto error;
  }
  if (PyList_Reverse(calorie_sums) == -1) {
    goto error;
  }

  PyObject *top1 = PyList_GetItem(calorie_sums, 0);
  Py_INCREF(top1);
  PyObject *top3 = PyLong_FromLong(0L);
  for (Py_ssize_t i = 0; i < 3; ++i) {
    PyObject *cal = PyList_GetItem(calorie_sums, i);
    PyObject *res = PyNumber_Add(top3, cal);
    Py_SETREF(top3, res);
  }
  Py_DECREF(calorie_sums);
  PyObject *solution = PyUnicode_FromFormat("%S %S", top1, top3);
  Py_DECREF(top1);
  Py_DECREF(top3);
  return solution;

error:
  Py_DECREF(sections);
  Py_DECREF(calorie_sums);
  return NULL;
}
#endif  // _AOC_Y2022_D01_H_INCLUDED
