#ifndef _AOC_Y2022_D23_H_INCLUDED
#define _AOC_Y2022_D23_H_INCLUDED
#include <limits.h>

#include "common.h"

enum _AoC_y2022_d23_direction {
  direction_north = 0,
  direction_south,
  direction_west,
  direction_east,
  num_directions,
};

PyObject *_AoC_y2022_d23_map_pos(const Py_ssize_t y, const Py_ssize_t x) {
  return PyTuple_Pack(2, PyLong_FromSsize_t(y), PyLong_FromSsize_t(x));
}

PyObject *_AoC_y2022_d23_parse_elves(PyObject *map_lines) {
  Py_ssize_t height = PyList_Size(map_lines);
  PyObject *line0 = PyList_GetItem(map_lines, 0);
  Py_ssize_t width = PyUnicode_GET_LENGTH(line0);
  PyObject *elves = PySet_New(0);
  for (Py_ssize_t y = 0; y < height; ++y) {
    for (Py_ssize_t x = 0; x < width; ++x) {
      PyObject *line = PyList_GetItem(map_lines, y);
      if (PyUnicode_READ_CHAR(line, x) == '#') {
        PySet_Add(elves, _AoC_y2022_d23_map_pos(y, x));
      }
    }
  }
  return elves;
}

Py_ssize_t _AoC_y2022_d23_part1(PyObject *elves) {
  long ymin = LONG_MAX;
  long xmin = LONG_MAX;
  long ymax = LONG_MIN;
  long xmax = LONG_MIN;
  {
    PyObject *elves_iter = PyObject_GetIter(elves);
    PyObject *elf_pos;
    while ((elf_pos = PyIter_Next(elves_iter))) {
      const Py_ssize_t y = PyLong_AsSsize_t(PyTuple_GetItem(elf_pos, 0));
      const Py_ssize_t x = PyLong_AsSsize_t(PyTuple_GetItem(elf_pos, 1));
      ymin = Py_MIN(ymin, y);
      xmin = Py_MIN(xmin, x);
      ymax = Py_MAX(ymax, y);
      xmax = Py_MAX(xmax, x);
      Py_DECREF(elf_pos);
    }
  }

  Py_ssize_t ground_count = 0;
  for (Py_ssize_t y = ymin; y <= ymax; ++y) {
    for (Py_ssize_t x = xmin; x <= xmax; ++x) {
      PyObject *pos = _AoC_y2022_d23_map_pos(y, x);
      ground_count += !PySet_Contains(elves, pos);
    }
  }
  return ground_count;
}

PyObject *_AoC_y2022_d23_count_proposals(PyObject *elves,
                                         PyObject *proposed_moves) {
  PyObject *counts = PyDict_New();
  PyObject *items = PyDict_Items(proposed_moves);
  for (Py_ssize_t i = 0; i < PyList_Size(items); ++i) {
    PyObject *item = PyList_GetItem(items, i);
    PyObject *src = PyTuple_GetItem(item, 0);
    PyObject *dst = PyTuple_GetItem(item, 1);
    if (1 == PyObject_RichCompareBool(src, dst, Py_EQ)) {
      continue;
    }
    if (!PyDict_Contains(counts, dst)) {
      PyDict_SetItem(counts, dst, PyLong_FromLong(0));
    }
    PyObject *count = PyDict_GetItem(counts, dst);
    PyObject *one = PyLong_FromLong(1);
    PyDict_SetItem(counts, dst, PyNumber_Add(count, one));
    Py_DECREF(one);
  }
  return counts;
}

int _AoC_y2022_d23_contains_elf(PyObject *elves,
                                const Py_ssize_t y,
                                const Py_ssize_t x) {
  PyObject *pos = _AoC_y2022_d23_map_pos(y, x);
  int yes = PySet_Contains(elves, pos);
  Py_DECREF(pos);
  return yes;
}

int _AoC_y2022_d23_has_adjacent_elf(PyObject *elves,
                                    const Py_ssize_t y,
                                    const Py_ssize_t x) {
  for (long dy = -1; dy <= 1; ++dy) {
    for (long dx = -1; dx <= 1; ++dx) {
      if (dx == 0 && dy == 0) {
        continue;
      }
      if (_AoC_y2022_d23_contains_elf(elves, y + dy, x + dx)) {
        return 1;
      }
    }
  }
  return 0;
}

void _AoC_y2022_d23_propose_move(PyObject *moves,
                                 const Py_ssize_t y1,
                                 const Py_ssize_t x1,
                                 const Py_ssize_t y2,
                                 const Py_ssize_t x2) {
  PyObject *src = _AoC_y2022_d23_map_pos(y1, x1);
  PyObject *dst = _AoC_y2022_d23_map_pos(y2, x2);
  PyDict_SetItem(moves, src, dst);
}

void _AoC_y2022_d23_consider_move(
    PyObject *elves,
    PyObject *proposed_moves,
    const Py_ssize_t y,
    const Py_ssize_t x,
    const enum _AoC_y2022_d23_direction current_facing) {
  for (int next_dir = 0; next_dir < num_directions; ++next_dir) {
    enum _AoC_y2022_d23_direction facing =
        (current_facing + next_dir) % num_directions;
    int contains_elf = 0;
    switch (facing) {
      case direction_north:
      case direction_south: {
        int north = facing == direction_north;
        int south = facing == direction_south;
        for (Py_ssize_t dx = -1; dx <= 1; ++dx) {
          if (_AoC_y2022_d23_contains_elf(elves, y - north + south, x + dx)) {
            contains_elf = 1;
          }
        }
      } break;
      case direction_west:
      case direction_east: {
        int west = facing == direction_west;
        int east = facing == direction_east;
        for (Py_ssize_t dy = -1; dy <= 1; ++dy) {
          if (_AoC_y2022_d23_contains_elf(elves, y + dy, x - west + east)) {
            contains_elf = 1;
          }
        }
      } break;
      default:
        break;
    }
    if (contains_elf) {
      continue;
    }
    switch (facing) {
      case direction_north:
        _AoC_y2022_d23_propose_move(proposed_moves, y, x, y - 1, x);
        return;
      case direction_south:
        _AoC_y2022_d23_propose_move(proposed_moves, y, x, y + 1, x);
        return;
      case direction_west:
        _AoC_y2022_d23_propose_move(proposed_moves, y, x, y, x - 1);
        return;
      case direction_east:
        _AoC_y2022_d23_propose_move(proposed_moves, y, x, y, x + 1);
        return;
      default:
        return;
    }
  }
}

PyObject *AoC_y2022_d23(PyObject *unicode_input) {
  PyObject *solution = 0;
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  PyObject *elves = 0;
  if (!lines) {
    goto done;
  }

  elves = _AoC_y2022_d23_parse_elves(lines);

  Py_ssize_t part1 = 0;
  Py_ssize_t part2 = 0;

  enum _AoC_y2022_d23_direction current_facing = direction_north;
  for (Py_ssize_t round = 0; part2 == 0; ++round) {
    /* _AoC_y2022_d23_dump_map(&map); */
    PyObject *proposed_moves = PyDict_New();
    {
      PyObject *elves_iter = PyObject_GetIter(elves);
      PyObject *elf_pos;
      while ((elf_pos = PyIter_Next(elves_iter))) {
        const Py_ssize_t y = PyLong_AsSsize_t(PyTuple_GetItem(elf_pos, 0));
        const Py_ssize_t x = PyLong_AsSsize_t(PyTuple_GetItem(elf_pos, 1));
        if (_AoC_y2022_d23_has_adjacent_elf(elves, y, x)) {
          _AoC_y2022_d23_consider_move(elves,
                                       proposed_moves,
                                       y,
                                       x,
                                       current_facing);
        }
        Py_DECREF(elf_pos);
      }
    }

    PyObject *proposed_counts =
        _AoC_y2022_d23_count_proposals(elves, proposed_moves);
    PyObject *items = PyDict_Items(proposed_moves);
    for (Py_ssize_t i = 0; i < PyList_Size(items); ++i) {
      PyObject *item = PyList_GetItem(items, i);
      PyObject *src = PyTuple_GetItem(item, 0);
      PyObject *dst = PyTuple_GetItem(item, 1);
      PyObject *count = PyDict_GetItem(proposed_counts, dst);
      if (PyLong_AsLong(count) == 1) {
        PySet_Add(elves, dst);
        PySet_Discard(elves, src);
      }
    }

    if (round == 10) {
      part1 = _AoC_y2022_d23_part1(elves);
    }
    if (!PyDict_Size(proposed_counts)) {
      part2 = round + 1;
    }

    Py_DECREF(proposed_counts);
    Py_DECREF(proposed_moves);
    current_facing = (current_facing + 1) % num_directions;
  }

  solution = PyUnicode_FromFormat("%zd %zd", part1, part2);

done:
  Py_XDECREF(elves);
  Py_XDECREF(lines);
  return solution;
}

#endif  // _AOC_Y2022_D23_H_INCLUDED
