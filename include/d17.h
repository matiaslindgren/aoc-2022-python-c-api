#ifndef _AOC_Y2022_D17_H_INCLUDED
#define _AOC_Y2022_D17_H_INCLUDED
#include "common.h"

const Py_ssize_t _AoC_y2022_d17_grid_width = 7;
const Py_ssize_t _AoC_y2022_d17_rock_max_size = 4;

enum _AoC_y2022_d17_rock {
  rock_horizontal = 0,
  rock_plus,
  rock_angle,
  rock_vertical,
  rock_square,
  num_rock_types,
};

Py_ssize_t _AoC_y2022_d17_get_grid_height(PyObject *grid) {
  return PyList_Size(grid) / _AoC_y2022_d17_grid_width;
}

int _AoC_y2022_d17_grid_pos_is_blocked(PyObject *grid,
                                       Py_ssize_t y,
                                       Py_ssize_t x) {
  const Py_ssize_t width = _AoC_y2022_d17_grid_width;
  const Py_ssize_t height = _AoC_y2022_d17_get_grid_height(grid);
  if (x < 0 || x >= width || y < 0 || y >= height) {
    return 1;
  }
  return PyList_GetItem(grid, y * width + x) == Py_True;
}

PyObject *_AoC_y2022_d17_make_rock(enum _AoC_y2022_d17_rock type) {
  const Py_ssize_t max_size = _AoC_y2022_d17_rock_max_size;
  PyObject *shape = PyList_New(max_size * max_size);
  for (Py_ssize_t i = 0; i < PyList_Size(shape); ++i) {
    PyList_SetItem(shape, i, Py_NewRef(Py_False));
  }

  switch (type) {
    case rock_horizontal: {
      PyList_SetItem(shape, 0, Py_NewRef(Py_True));
      PyList_SetItem(shape, 1, Py_NewRef(Py_True));
      PyList_SetItem(shape, 2, Py_NewRef(Py_True));
      PyList_SetItem(shape, 3, Py_NewRef(Py_True));
    } break;
    case rock_plus: {
      PyList_SetItem(shape, 1, Py_NewRef(Py_True));
      PyList_SetItem(shape, 4, Py_NewRef(Py_True));
      PyList_SetItem(shape, 5, Py_NewRef(Py_True));
      PyList_SetItem(shape, 6, Py_NewRef(Py_True));
      PyList_SetItem(shape, 9, Py_NewRef(Py_True));
    } break;
    case rock_angle: {
      PyList_SetItem(shape, 2, Py_NewRef(Py_True));
      PyList_SetItem(shape, 6, Py_NewRef(Py_True));
      PyList_SetItem(shape, 8, Py_NewRef(Py_True));
      PyList_SetItem(shape, 9, Py_NewRef(Py_True));
      PyList_SetItem(shape, 10, Py_NewRef(Py_True));
    } break;
    case rock_vertical: {
      PyList_SetItem(shape, 0, Py_NewRef(Py_True));
      PyList_SetItem(shape, 4, Py_NewRef(Py_True));
      PyList_SetItem(shape, 8, Py_NewRef(Py_True));
      PyList_SetItem(shape, 12, Py_NewRef(Py_True));
    } break;
    case rock_square: {
      PyList_SetItem(shape, 0, Py_NewRef(Py_True));
      PyList_SetItem(shape, 1, Py_NewRef(Py_True));
      PyList_SetItem(shape, 4, Py_NewRef(Py_True));
      PyList_SetItem(shape, 5, Py_NewRef(Py_True));
    } break;
    default: {
      return PyErr_Format(PyExc_RuntimeError, "cannot make unknown rock type");
      break;
    }
  }

  PyObject *rock = PyTuple_New(3);
  Py_ssize_t top = 0;
  Py_ssize_t left = 2;
  PyTuple_SET_ITEM(rock, 0, shape);
  PyTuple_SET_ITEM(rock, 1, PyLong_FromSsize_t(top));
  PyTuple_SET_ITEM(rock, 2, PyLong_FromSsize_t(left));
  return rock;
}

Py_ssize_t _AoC_y2022_d17_find_rock_height(PyObject *rock) {
  PyObject *shape = PyTuple_GetItem(rock, 0);
  Py_ssize_t height = 0;
  const Py_ssize_t max_size = _AoC_y2022_d17_rock_max_size;
  for (Py_ssize_t i = 0; i < PyList_Size(shape); ++i) {
    if (PyList_GetItem(shape, i) == Py_True) {
      height = Py_MAX(height, 1 + (i / max_size));
    }
  }
  return height;
}

void _AoC_y2022_d17_rock_step_horizontal(PyObject *grid,
                                         PyObject *rock,
                                         int step) {
  PyObject *shape = PyTuple_GetItem(rock, 0);
  Py_ssize_t top = PyLong_AsSsize_t(PyTuple_GetItem(rock, 1));
  Py_ssize_t left = PyLong_AsSsize_t(PyTuple_GetItem(rock, 2));
  const Py_ssize_t max_size = _AoC_y2022_d17_rock_max_size;
  for (Py_ssize_t i = 0; i < PyList_Size(shape); ++i) {
    if (PyList_GetItem(shape, i) == Py_True) {
      const Py_ssize_t y_rock = top + (i / max_size);
      const Py_ssize_t x_rock = left + (i % max_size);
      if (_AoC_y2022_d17_grid_pos_is_blocked(grid, y_rock, x_rock + step)) {
        return;
      }
    }
  }
  PyTuple_SetItem(rock, 2, PyLong_FromSsize_t(left + step));
}

int _AoC_y2022_d17_rock_step_down(PyObject *grid, PyObject *rock) {
  PyObject *shape = PyTuple_GetItem(rock, 0);
  Py_ssize_t top = PyLong_AsSsize_t(PyTuple_GetItem(rock, 1));
  Py_ssize_t left = PyLong_AsSsize_t(PyTuple_GetItem(rock, 2));
  const Py_ssize_t max_size = _AoC_y2022_d17_rock_max_size;
  for (Py_ssize_t i = 0; i < PyList_Size(shape); ++i) {
    if (PyList_GetItem(shape, i) == Py_True) {
      const Py_ssize_t y_rock = top + (i / max_size);
      const Py_ssize_t x_rock = left + (i % max_size);
      if (_AoC_y2022_d17_grid_pos_is_blocked(grid, y_rock + 1, x_rock)) {
        return 0;
      }
    }
  }
  PyTuple_SetItem(rock, 1, PyLong_FromSsize_t(top + 1));
  return 1;
}

Py_ssize_t _AoC_y2022_d17_find_grid_top_rock(PyObject *grid) {
  Py_ssize_t y_top_rock = 0;
  for (Py_ssize_t i = 0; i < PyList_Size(grid); ++i) {
    y_top_rock = i / _AoC_y2022_d17_grid_width;
    if (PyList_GetItem(grid, i) == Py_True) {
      break;
    }
  }
  return y_top_rock;
}

void _AoC_y2022_d17_resize_grid_for_rock(PyObject *grid, PyObject *rock) {
  const Py_ssize_t grid_width = _AoC_y2022_d17_grid_width;
  Py_ssize_t y_top_rock = _AoC_y2022_d17_find_grid_top_rock(grid);
  Py_ssize_t rock_height = _AoC_y2022_d17_find_rock_height(rock);
  Py_ssize_t num_empty = rock_height + 3 - y_top_rock;
  if (num_empty < 0) {
    PyList_SetSlice(grid, 0, grid_width * (-num_empty), 0);
  } else {
    for (Py_ssize_t i = 0; i < grid_width * num_empty; ++i) {
      PyList_Insert(grid, 0, Py_NewRef(Py_False));
    }
  }
}

Py_ssize_t _AoC_y2022_d17_tower_height(PyObject *grid) {
  return _AoC_y2022_d17_get_grid_height(grid) -
         _AoC_y2022_d17_find_grid_top_rock(grid);
}

void _AoC_y2022_d17_finalize_rock(PyObject *grid, PyObject *rock) {
  PyObject *shape = PyTuple_GetItem(rock, 0);
  Py_ssize_t top = PyLong_AsSsize_t(PyTuple_GetItem(rock, 1));
  Py_ssize_t left = PyLong_AsSsize_t(PyTuple_GetItem(rock, 2));
  const Py_ssize_t max_size = _AoC_y2022_d17_rock_max_size;
  const Py_ssize_t grid_width = _AoC_y2022_d17_grid_width;
  for (Py_ssize_t i = 0; i < PyList_Size(shape); ++i) {
    if (PyList_GetItem(shape, i) == Py_True) {
      const Py_ssize_t y_rock = top + (i / max_size);
      const Py_ssize_t x_rock = left + (i % max_size);
      const Py_ssize_t grid_pos = y_rock * grid_width + x_rock;
      PyList_SetItem(grid, grid_pos, Py_NewRef(Py_True));
    }
  }
}

PyObject *_AoC_y2022_d17_find_cycle(PyObject *v) {
  // https://en.wikipedia.org/wiki/Cycle_detection#Tortoise_and_hare
  if (PyList_Size(v) < 3) {
    return 0;
  }
  Py_ssize_t t = 1;
  Py_ssize_t h = 2;
  while (0 == PyObject_RichCompareBool(PyList_GetItem(v, t),
                                       PyList_GetItem(v, h),
                                       Py_EQ)) {
    if (h >= PyList_Size(v)) {
      return 0;
    }
    t += 1;
    h += 2;
  }

  Py_ssize_t cycle_begin = 0;
  t = 0;
  while (0 == PyObject_RichCompareBool(PyList_GetItem(v, t),
                                       PyList_GetItem(v, h),
                                       Py_EQ)) {
    if (h >= PyList_Size(v)) {
      return 0;
    }
    ++t;
    ++h;
    ++cycle_begin;
  }

  Py_ssize_t cycle_length = 1;
  h = t + 1;
  while (0 == PyObject_RichCompareBool(PyList_GetItem(v, t),
                                       PyList_GetItem(v, h),
                                       Py_EQ)) {
    ++h;
    ++cycle_length;
  }

  PyObject *cycle = PyTuple_New(2);
  PyTuple_SET_ITEM(cycle, 0, PyLong_FromSsize_t(cycle_begin));
  PyTuple_SET_ITEM(cycle, 1, PyLong_FromSsize_t(cycle_length));
  return cycle;
}

Py_ssize_t _AoC_y2022_d17_infer_tower_height(PyObject *height_deltas,
                                             PyObject *cycle,
                                             const Py_ssize_t limit) {
  const Py_ssize_t cycle_begin = PyLong_AsSsize_t(PyTuple_GetItem(cycle, 0));
  const Py_ssize_t cycle_length = PyLong_AsSsize_t(PyTuple_GetItem(cycle, 1));
  Py_ssize_t tower_height = 1;
  Py_ssize_t rocks_stopped = 0;
  for (; rocks_stopped < cycle_begin; ++rocks_stopped) {
    PyObject *height_delta = PyList_GetItem(height_deltas, rocks_stopped);
    tower_height += PyLong_AsSsize_t(PyTuple_GetItem(height_delta, 0));
  }
  Py_ssize_t cycle_delta = 0;
  {
    PyObject *tmp = PyList_New(cycle_length);
    for (Py_ssize_t j = 0; j < cycle_length; ++j) {
      PyObject *delta = Py_NewRef(
          PyTuple_GetItem(PyList_GetItem(height_deltas, j + cycle_begin), 0));
      PyList_SET_ITEM(tmp, j, delta);
      cycle_delta += PyLong_AsSsize_t(delta);
    }
    height_deltas = tmp;
  }
  for (; rocks_stopped + cycle_length < limit; rocks_stopped += cycle_length) {
    tower_height += cycle_delta;
  }
  for (; rocks_stopped < limit; ++rocks_stopped) {
    Py_ssize_t j = (rocks_stopped - cycle_begin) % cycle_length;
    tower_height += PyLong_AsSsize_t(PyList_GetItem(height_deltas, j));
  }
  return tower_height;
}

PyObject *AoC_y2022_d17(PyObject *unicode_input) {
  PyObject *jet_pattern = PyUnicode_Splitlines(unicode_input, 0);
  if (!jet_pattern) {
    return 0;
  }
  Py_SETREF(jet_pattern, Py_NewRef(PyList_GetItem(jet_pattern, 0)));

  PyObject *grid = PyList_New(0);
  PyObject *height_deltas = PyList_New(0);
  PyObject *rock = 0;
  PyObject *cycle = 0;
  enum _AoC_y2022_d17_rock next_rock_type = rock_horizontal;
  Py_ssize_t jet_pattern_len = PyUnicode_GET_LENGTH(jet_pattern);

  for (Py_ssize_t i = 0; !cycle; i = (i + 1) % jet_pattern_len) {
    if (!rock) {
      rock = _AoC_y2022_d17_make_rock(next_rock_type);
      next_rock_type = (next_rock_type + 1) % num_rock_types;
      _AoC_y2022_d17_resize_grid_for_rock(grid, rock);
    }
    switch (PyUnicode_READ_CHAR(jet_pattern, i)) {
      case '<': {
        _AoC_y2022_d17_rock_step_horizontal(grid, rock, -1);
      } break;
      case '>': {
        _AoC_y2022_d17_rock_step_horizontal(grid, rock, 1);
      } break;
      default: {
        PyErr_Format(PyExc_RuntimeError, "input has unknown chars");
        return 0;
      }
    }
    if (!_AoC_y2022_d17_rock_step_down(grid, rock)) {
      Py_ssize_t h0 = _AoC_y2022_d17_tower_height(grid);
      _AoC_y2022_d17_finalize_rock(grid, rock);
      Py_DECREF(rock);
      rock = 0;
      Py_ssize_t h1 = _AoC_y2022_d17_tower_height(grid);
      PyObject *t = PyTuple_New(3);
      PyTuple_SET_ITEM(t, 0, PyLong_FromSsize_t(h1 - h0));
      PyTuple_SET_ITEM(t, 1, PyLong_FromSsize_t(i));
      PyTuple_SET_ITEM(t, 2, PyLong_FromSsize_t(next_rock_type));
      PyList_Append(height_deltas, t);
      if (PyList_Size(height_deltas) > jet_pattern_len) {
        cycle = _AoC_y2022_d17_find_cycle(height_deltas);
      }
    }
  }
  Py_DECREF(grid);
  Py_ssize_t part1 =
      _AoC_y2022_d17_infer_tower_height(height_deltas, cycle, 2022);
  Py_ssize_t part2 =
      _AoC_y2022_d17_infer_tower_height(height_deltas, cycle, 1000000000000);
  return PyUnicode_FromFormat("%zd %zd", part1, part2);
}

#endif  // _AOC_Y2022_D17_H_INCLUDED
