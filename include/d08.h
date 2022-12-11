#ifndef _AOC_Y2022_D08_H_INCLUDED
#define _AOC_Y2022_D08_H_INCLUDED
#include "common.h"

int _AoC_y2022_d08_is_visible(PyObject *heightmap,
                              Py_ssize_t grid_h,
                              Py_ssize_t grid_w,
                              Py_ssize_t y0,
                              Py_ssize_t x0) {
  PyObject *height0 = PyList_GetItem(heightmap, y0 * (grid_w + 2) + x0);

  int ok_from_north = 1;
  for (Py_ssize_t y1 = y0 - 1; y1 > 0; --y1) {
    PyObject *height1 = PyList_GetItem(heightmap, y1 * (grid_w + 2) + x0);
    if (!PyObject_RichCompareBool(height1, height0, Py_LT)) {
      ok_from_north = 0;
      break;
    }
  }
  if (ok_from_north) {
    return 1;
  }

  int ok_from_east = 1;
  for (Py_ssize_t x1 = x0 + 1; x1 < grid_w + 1; ++x1) {
    PyObject *height1 = PyList_GetItem(heightmap, y0 * (grid_w + 2) + x1);
    if (!PyObject_RichCompareBool(height1, height0, Py_LT)) {
      ok_from_east = 0;
      break;
    }
  }
  if (ok_from_east) {
    return 1;
  }

  int ok_from_south = 1;
  for (Py_ssize_t y1 = y0 + 1; y1 < grid_h + 1; ++y1) {
    PyObject *height1 = PyList_GetItem(heightmap, y1 * (grid_w + 2) + x0);
    if (!PyObject_RichCompareBool(height1, height0, Py_LT)) {
      ok_from_south = 0;
      break;
    }
  }
  if (ok_from_south) {
    return 1;
  }

  int ok_from_west = 1;
  for (Py_ssize_t x1 = x0 - 1; x1 > 0; --x1) {
    PyObject *height1 = PyList_GetItem(heightmap, y0 * (grid_w + 2) + x1);
    if (!PyObject_RichCompareBool(height1, height0, Py_LT)) {
      ok_from_west = 0;
      break;
    }
  }

  return ok_from_west;
}

int _AoC_y2022_d08_scenic_score(PyObject *heightmap,
                                Py_ssize_t grid_h,
                                Py_ssize_t grid_w,
                                Py_ssize_t y0,
                                Py_ssize_t x0) {
  PyObject *height0 = PyList_GetItem(heightmap, y0 * (grid_w + 2) + x0);

  int score_north = 0;
  for (Py_ssize_t y1 = y0 - 1; y1 > 0; --y1) {
    ++score_north;
    PyObject *height1 = PyList_GetItem(heightmap, y1 * (grid_w + 2) + x0);
    if (!PyObject_RichCompareBool(height1, height0, Py_LT)) {
      break;
    }
  }

  int score_east = 0;
  for (Py_ssize_t x1 = x0 + 1; x1 < grid_w + 1; ++x1) {
    ++score_east;
    PyObject *height1 = PyList_GetItem(heightmap, y0 * (grid_w + 2) + x1);
    if (!PyObject_RichCompareBool(height1, height0, Py_LT)) {
      break;
    }
  }

  int score_south = 0;
  for (Py_ssize_t y1 = y0 + 1; y1 < grid_h + 1; ++y1) {
    ++score_south;
    PyObject *height1 = PyList_GetItem(heightmap, y1 * (grid_w + 2) + x0);
    if (!PyObject_RichCompareBool(height1, height0, Py_LT)) {
      break;
    }
  }

  int score_west = 0;
  for (Py_ssize_t x1 = x0 - 1; x1 > 0; --x1) {
    ++score_west;
    PyObject *height1 = PyList_GetItem(heightmap, y0 * (grid_w + 2) + x1);
    if (!PyObject_RichCompareBool(height1, height0, Py_LT)) {
      break;
    }
  }

  return score_north * score_east * score_south * score_west;
}

PyObject *AoC_y2022_d08(PyObject *unicode_input) {
  PyObject *solution = NULL;
  PyObject *heightmap = NULL;
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  if (!lines) {
    PyErr_Format(PyExc_RuntimeError, "could not split input into lines");
    goto done;
  }

  Py_ssize_t grid_h = PyList_Size(lines);
  Py_ssize_t grid_w = PyUnicode_GetLength(PyList_GetItem(lines, 0));

  heightmap = PyList_New((grid_h + 2) * (grid_w + 2));
  for (Py_ssize_t i = 0; i < PyList_Size(heightmap); ++i) {
    PyList_SET_ITEM(heightmap, i, PyLong_FromLong(-1));
  }

  for (Py_ssize_t y = 0; y < grid_h; ++y) {
    PyObject *line = PyList_GetItem(lines, y);
    for (Py_ssize_t x = 0; x < grid_w; ++x) {
      PyObject *tree_heigth_str = PyUnicode_Substring(line, x, x + 1);
      PyObject *tree_heigth = PyLong_FromUnicodeObject(tree_heigth_str, 10);
      PyList_SetItem(heightmap, (y + 1) * (grid_w + 2) + x + 1, tree_heigth);
      Py_DECREF(tree_heigth_str);
    }
  }

  long part1 = 0;
  long part2 = 0;
  for (Py_ssize_t y = 1; y < grid_h + 1; ++y) {
    for (Py_ssize_t x = 1; x < grid_w + 1; ++x) {
      if (_AoC_y2022_d08_is_visible(heightmap, grid_h, grid_w, y, x)) {
        ++part1;
      }
      long scenic_score = _AoC_y2022_d08_scenic_score(heightmap, grid_h, grid_w, y, x);
      if (scenic_score > part2) {
        part2 = scenic_score;
      }
    }
  }

  PyObject *part1_py = PyLong_FromLong(part1);
  PyObject *part2_py = PyLong_FromLong(part2);
  solution = PyUnicode_FromFormat("%S %S", part1_py, part2_py);
  Py_DECREF(part1_py);
  Py_DECREF(part2_py);

done:
  Py_XDECREF(lines);
  Py_XDECREF(heightmap);
  return solution;
}

#endif  // _AOC_Y2022_D08_H_INCLUDED
