#ifndef _AOC_Y2022_D24_H_INCLUDED
#define _AOC_Y2022_D24_H_INCLUDED
#include <limits.h>

#include "common.h"

PyObject *_AoC_y2022_d24_make_point3(const long y, const long x, const long t) {
  return PyTuple_Pack(3,
                      PyLong_FromLong(y),
                      PyLong_FromLong(x),
                      PyLong_FromLong(t));
}

PyObject *_AoC_y2022_d24_make_blizzard(const long y,
                                       const long x,
                                       const long dy,
                                       const long dx) {
  return PyTuple_Pack(4,
                      PyLong_FromLong(y),
                      PyLong_FromLong(x),
                      PyLong_FromLong(dy),
                      PyLong_FromLong(dx));
}

int _AoC_y2022_d24_map_states_has_cycle(PyObject *state, PyObject *states) {
  const Py_ssize_t num_states = PyList_Size(states);
  if (!num_states) {
    return 0;
  }
  PyObject *init_state = PyList_GetItem(states, 0);
  return 1 == PyObject_RichCompareBool(state, init_state, Py_EQ);
}

long _AoC_y2022_d24_find_shortest_path(const long num_states,
                                       const long height,
                                       const long width,
                                       PyObject *blizzards,
                                       const long t_begin,
                                       const long y_src,
                                       const long x_src,
                                       const long y_dst,
                                       const long x_dst) {
  PyObject *visited = PySet_New(0);
  PyObject *queue = PyList_New(0);
  long t_min = LONG_MAX - 1;

  PyList_Append(queue, _AoC_y2022_d24_make_point3(y_src, x_src, t_begin));
  while (PyList_Size(queue)) {
    PyObject *p = PyList_GetItem(queue, 0);
    const long y = PyLong_AsLong(PyTuple_GetItem(p, 0));
    const long x = PyLong_AsLong(PyTuple_GetItem(p, 1));
    const long t = PyLong_AsLong(PyTuple_GetItem(p, 2));
    PySequence_DelItem(queue, 0);

    const int at_dst = y == y_dst && x == x_dst;
    if (at_dst) {
      t_min = Py_MIN(t_min, t);
      continue;
    }

    const int at_src = y == y_src && x == x_src;
    if (!at_src) {
      const int out_of_bounds =
          y < 0 || y > height - 1 || x < 0 || x > width - 1;
      if (out_of_bounds) {
        continue;
      }
      const Py_ssize_t blizzard_idx =
          (t % num_states) * height * width + y * width + x;
      if (PyList_GetItem(blizzards, blizzard_idx) == Py_True) {
        continue;
      }
    }

    PyObject *cache_key = _AoC_y2022_d24_make_point3(y, x, t % num_states);
    if (PySet_Contains(visited, cache_key)) {
      Py_DECREF(cache_key);
      continue;
    }
    PySet_Add(visited, cache_key);
    Py_DECREF(cache_key);

    for (int step = 0; step < 5; ++step) {
      const long dy = (step == 0) - (step == 3);
      const long dx = (step == 1) - (step == 2);
      PyList_Append(queue, _AoC_y2022_d24_make_point3(y + dy, x + dx, t + 1));
    }
  }

  Py_DECREF(visited);
  Py_DECREF(queue);

  return t_min;
}

PyObject *AoC_y2022_d24(PyObject *unicode_input) {
  PyObject *solution = 0;
  PyObject *blizzards = 0;
  PyObject *states = 0;
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  if (!lines) {
    goto done;
  }

  const long height = PyList_Size(lines) - 2;
  PyObject *line0 = PyList_GetItem(lines, 0);
  const long width = PyUnicode_GET_LENGTH(line0) - 2;

  PyObject *state = PyList_New(0);
  for (long y = 0; y < height; ++y) {
    PyObject *row = PyList_GetItem(lines, y + 1);
    for (long x = 0; x < width; ++x) {
      const Py_UCS4 ch = PyUnicode_READ_CHAR(row, x + 1);
      PyObject *blizzard = 0;
      switch (ch) {
        case '>': {
          blizzard = _AoC_y2022_d24_make_blizzard(y, x, 0, 1);
        } break;
        case 'v': {
          blizzard = _AoC_y2022_d24_make_blizzard(y, x, 1, 0);
        } break;
        case '<': {
          blizzard = _AoC_y2022_d24_make_blizzard(y, x, 0, -1);
        } break;
        case '^': {
          blizzard = _AoC_y2022_d24_make_blizzard(y, x, -1, 0);
        } break;
      }
      if (blizzard) {
        PyList_Append(state, blizzard);
      }
    }
  }

  states = PyList_New(0);
  for (Py_ssize_t step = 0; !_AoC_y2022_d24_map_states_has_cycle(state, states);
       ++step) {
    PyList_Append(states, Py_NewRef(state));
    PyObject *state0 = state;
    PyObject *state1 = PyList_New(PyList_Size(state0));
    for (Py_ssize_t i = 0; i < PyList_Size(state0); ++i) {
      PyObject *blizzard0 = PyList_GetItem(state0, i);
      const long y0 = PyLong_AsLong(PyTuple_GetItem(blizzard0, 0));
      const long x0 = PyLong_AsLong(PyTuple_GetItem(blizzard0, 1));
      const long dy = PyLong_AsLong(PyTuple_GetItem(blizzard0, 2));
      const long dx = PyLong_AsLong(PyTuple_GetItem(blizzard0, 3));
      const long y1 = (y0 + height + dy) % height;
      const long x1 = (x0 + width + dx) % width;
      PyObject *blizzard1 = _AoC_y2022_d24_make_blizzard(y1, x1, dy, dx);
      PyList_SET_ITEM(state1, i, blizzard1);
    }
    PyList_Sort(state1);
    Py_SETREF(state, state1);
  }

  Py_DECREF(state);

  const long num_states = PyList_Size(states);
  blizzards = PyList_New(num_states * height * width);
  for (Py_ssize_t i = 0; i < PyList_Size(blizzards); ++i) {
    PyList_SET_ITEM(blizzards, i, PyBool_FromLong(0));
  }
  for (Py_ssize_t step = 0; step < num_states; ++step) {
    state = PyList_GetItem(states, step);
    for (Py_ssize_t i = 0; i < PyList_Size(state); ++i) {
      PyObject *blizzard = PyList_GetItem(state, i);
      const long yb = PyLong_AsLong(PyTuple_GetItem(blizzard, 0));
      const long xb = PyLong_AsLong(PyTuple_GetItem(blizzard, 1));
      const Py_ssize_t idx = step * height * width + yb * width + xb;
      PyList_SetItem(blizzards, idx, PyBool_FromLong(1));
    }
  }

  Py_DECREF(states);

  const long part1 = _AoC_y2022_d24_find_shortest_path(num_states,
                                                       height,
                                                       width,
                                                       blizzards,
                                                       0,
                                                       -1,
                                                       0,
                                                       height,
                                                       width - 1);

  long part2 = _AoC_y2022_d24_find_shortest_path(num_states,
                                                 height,
                                                 width,
                                                 blizzards,
                                                 part1,
                                                 height,
                                                 width - 1,
                                                 -1,
                                                 0);
  part2 = _AoC_y2022_d24_find_shortest_path(num_states,
                                            height,
                                            width,
                                            blizzards,
                                            part2,
                                            -1,
                                            0,
                                            height,
                                            width - 1);
  solution = PyUnicode_FromFormat("%ld %ld", part1, part2);

done:
  Py_XDECREF(blizzards);
  Py_XDECREF(lines);
  return solution;
}

#endif  // _AOC_Y2022_D24_H_INCLUDED
