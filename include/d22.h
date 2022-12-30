#ifndef _AOC_Y2022_D22_H_INCLUDED
#define _AOC_Y2022_D22_H_INCLUDED
#include "common.h"

enum _AoC_y2022_d22_map_facing {
  map_facing_right = 0,
  map_facing_down,
  map_facing_left,
  map_facing_up,
  num_map_facings,
};

enum _AoC_y2022_d22_map_cell {
  map_cell_void = 0,
  map_cell_floor,
  map_cell_wall,
};

struct _AoC_y2022_d22_map2d {
  Py_ssize_t width;
  Py_ssize_t height;
  PyObject *cells;
};

PyObject *_AoC_y2022_d22_parse_moves(PyObject *line) {
  PyObject *moves = PyList_New(0);

  const Py_ssize_t line_len = PyUnicode_GET_LENGTH(line);
  Py_ssize_t num_begin = 0;
  Py_ssize_t num_end = 0;

  for (Py_ssize_t i = 0; num_begin < num_end || i < line_len; ++i) {
    if (i < line_len && Py_UNICODE_ISDIGIT(PyUnicode_READ_CHAR(line, i))) {
      num_end = i + 1;
      continue;
    }
    if (num_begin < num_end) {
      PyList_Append(moves, PyUnicode_Substring(line, num_begin, num_end));
    }
    num_begin = i + 1;
    num_end = 0;
    if (i < line_len) {
      PyList_Append(moves, PyUnicode_Substring(line, i, i + 1));
    }
  }

  return moves;
}

Py_ssize_t _AoC_y2022_d22_map2d_idx(struct _AoC_y2022_d22_map2d *const map2d,
                                    const Py_ssize_t y,
                                    const Py_ssize_t x) {
  return y * map2d->width + x;
}

PyObject *_AoC_y2022_d22_get_cell(struct _AoC_y2022_d22_map2d *const map2d,
                                  const Py_ssize_t y,
                                  const Py_ssize_t x) {
  return PyList_GetItem(map2d->cells, _AoC_y2022_d22_map2d_idx(map2d, y, x));
}

struct _AoC_y2022_d22_map2d *_AoC_y2022_d22_parse_map2d(
    struct _AoC_y2022_d22_map2d *map2d,
    PyObject *map_lines) {
  Py_ssize_t max_line_width = 0;
  for (Py_ssize_t y = 0; y < PyList_Size(map_lines); ++y) {
    PyObject *line = PyList_GetItem(map_lines, y);
    max_line_width = Py_MAX(max_line_width, PyUnicode_GET_LENGTH(line));
  }

  map2d->width = max_line_width + 2;
  map2d->height = PyList_Size(map_lines) + 2;
  map2d->cells = PyList_New(map2d->height * map2d->width);

  for (Py_ssize_t y_pad = 0; y_pad <= 1; ++y_pad) {
    for (Py_ssize_t x = 0; x < map2d->width; ++x) {
      PyObject *cell = PyLong_FromLong(map_cell_void);
      const Py_ssize_t idx =
          _AoC_y2022_d22_map2d_idx(map2d, y_pad * (map2d->height - 1), x);
      PyList_SET_ITEM(map2d->cells, idx, cell);
    }
  }
  for (Py_ssize_t x_pad = 0; x_pad <= 1; ++x_pad) {
    for (Py_ssize_t y = 0; y < map2d->height; ++y) {
      PyObject *cell = PyLong_FromLong(map_cell_void);
      const Py_ssize_t idx =
          _AoC_y2022_d22_map2d_idx(map2d, y, x_pad * (map2d->width - 1));
      PyList_SET_ITEM(map2d->cells, idx, cell);
    }
  }

  for (Py_ssize_t y = 1; y < map2d->height - 1; ++y) {
    PyObject *map_line = PyList_GetItem(map_lines, y - 1);
    const Py_ssize_t line_len = PyUnicode_GET_LENGTH(map_line);
    for (Py_ssize_t x = 1; x < map2d->width - 1; ++x) {
      const Py_ssize_t idx = _AoC_y2022_d22_map2d_idx(map2d, y, x);
      PyObject *cell = 0;
      if (x - 1 < line_len) {
        const Py_UCS4 ch = PyUnicode_READ_CHAR(map_line, x - 1);
        switch (ch) {
          case ' ':
            cell = PyLong_FromLong(map_cell_void);
            break;
          case '.':
            cell = PyLong_FromLong(map_cell_floor);
            break;
          case '#':
            cell = PyLong_FromLong(map_cell_wall);
            break;
        }
      } else {
        cell = PyLong_FromLong(map_cell_void);
      }
      if (!cell) {
        return 0;
      }
      PyList_SET_ITEM(map2d->cells, idx, cell);
    }
  }
  return map2d;
}

Py_ssize_t _AoC_y2022_d22_wrapping_2d_step_horizontal(
    struct _AoC_y2022_d22_map2d *const map2d,
    const Py_ssize_t y,
    Py_ssize_t x,
    int step) {
  PyObject *cell = 0;
  while (!cell || PyLong_AsLong(cell) == map_cell_void) {
    x = (x + map2d->width + step) % map2d->width;
    cell = _AoC_y2022_d22_get_cell(map2d, y, x);
  }
  if (PyLong_AsLong(cell) == map_cell_wall) {
    return 0;
  }
  return x;
}

Py_ssize_t _AoC_y2022_d22_wrapping_2d_step_vertical(
    struct _AoC_y2022_d22_map2d *const map2d,
    Py_ssize_t y,
    const Py_ssize_t x,
    int step) {
  PyObject *cell = 0;
  while (!cell || PyLong_AsLong(cell) == map_cell_void) {
    y = (y + map2d->height + step) % map2d->height;
    cell = _AoC_y2022_d22_get_cell(map2d, y, x);
  }
  if (PyLong_AsLong(cell) == map_cell_wall) {
    return 0;
  }
  return y;
}

PyObject *_AoC_y2022_d22_find_part1(struct _AoC_y2022_d22_map2d *const map2d,
                                    PyObject *moves) {
  Py_ssize_t y = 1;
  Py_ssize_t x = _AoC_y2022_d22_wrapping_2d_step_horizontal(map2d, y, 1, 1);
  enum _AoC_y2022_d22_map_facing map_facing = map_facing_right;

  for (Py_ssize_t i = 0; i < PyList_Size(moves); ++i) {
    PyObject *move = PyList_GetItem(moves, i);

    const Py_UCS4 move_ch = PyUnicode_READ_CHAR(move, 0);
    if (Py_UNICODE_ISDIGIT(move_ch)) {
      PyObject *num_steps = PyLong_FromUnicodeObject(move, 10);
      for (Py_ssize_t step = 0; step < PyLong_AsSsize_t(num_steps); ++step) {
        switch (map_facing) {
          case map_facing_right: {
            Py_ssize_t x1 =
                _AoC_y2022_d22_wrapping_2d_step_horizontal(map2d, y, x, 1);
            if (x1) {
              x = x1;
            }
          } break;
          case map_facing_left: {
            Py_ssize_t x1 =
                _AoC_y2022_d22_wrapping_2d_step_horizontal(map2d, y, x, -1);
            if (x1) {
              x = x1;
            }
          } break;
          case map_facing_down: {
            Py_ssize_t y1 =
                _AoC_y2022_d22_wrapping_2d_step_vertical(map2d, y, x, 1);
            if (y1) {
              y = y1;
            }
          } break;
          case map_facing_up: {
            Py_ssize_t y1 =
                _AoC_y2022_d22_wrapping_2d_step_vertical(map2d, y, x, -1);
            if (y1) {
              y = y1;
            }
          } break;
          default:
            break;
        }
      }
      Py_DECREF(num_steps);
    } else {
      switch (move_ch) {
        case 'R': {
          map_facing = (map_facing + 1) % num_map_facings;
        } break;
        case 'L': {
          map_facing = (map_facing + num_map_facings - 1) % num_map_facings;
        } break;
      }
    }
  }

  return PyLong_FromLong(1000 * y + 4 * x + map_facing);
}

PyObject *_AoC_y2022_d22_parse_map3d_face(
    struct _AoC_y2022_d22_map2d *const map2d,
    const Py_ssize_t y_face,
    const Py_ssize_t x_face,
    const Py_ssize_t face_size) {
  PyObject *cells = PyList_New(face_size * face_size);
  for (Py_ssize_t y = 0; y < face_size; ++y) {
    for (Py_ssize_t x = 0; x < face_size; ++x) {
      const Py_ssize_t y_map = y_face * face_size + y + 1;
      const Py_ssize_t x_map = x_face * face_size + x + 1;
      PyObject *cell = _AoC_y2022_d22_get_cell(map2d, y_map, x_map);
      if (PyLong_AsLong(cell) == map_cell_void) {
        Py_DECREF(cells);
        return 0;
      }
      PyList_SET_ITEM(cells, y * face_size + x, Py_NewRef(cell));
    }
  }
  PyObject *face = PyDict_New();
  PyDict_SetItemString(face, "y", PyLong_FromSsize_t(y_face));
  PyDict_SetItemString(face, "x", PyLong_FromSsize_t(x_face));
  PyDict_SetItemString(face, "cells", cells);
  return face;
}

PyObject *_AoC_y2022_d22_make_transition(
    const Py_ssize_t dst,
    const enum _AoC_y2022_d22_map_facing dst_side) {
  return PyTuple_Pack(2, PyLong_FromSsize_t(dst), PyLong_FromLong(dst_side));
}

PyObject *_AoC_y2022_d22_example_transitions() {
  const enum _AoC_y2022_d22_map_facing r = map_facing_right;
  const enum _AoC_y2022_d22_map_facing d = map_facing_down;
  const enum _AoC_y2022_d22_map_facing l = map_facing_left;
  const enum _AoC_y2022_d22_map_facing u = map_facing_up;
  const enum _AoC_y2022_d22_map_facing n = num_map_facings;

  PyObject *t = PyList_New(6 * n);

  PyList_SET_ITEM(t, 0 * n + r, _AoC_y2022_d22_make_transition(5, r));
  PyList_SET_ITEM(t, 0 * n + d, _AoC_y2022_d22_make_transition(3, u));
  PyList_SET_ITEM(t, 0 * n + l, _AoC_y2022_d22_make_transition(2, u));
  PyList_SET_ITEM(t, 0 * n + u, _AoC_y2022_d22_make_transition(1, u));

  PyList_SET_ITEM(t, 1 * n + r, _AoC_y2022_d22_make_transition(2, l));
  PyList_SET_ITEM(t, 1 * n + d, _AoC_y2022_d22_make_transition(4, d));
  PyList_SET_ITEM(t, 1 * n + l, _AoC_y2022_d22_make_transition(5, d));
  PyList_SET_ITEM(t, 1 * n + u, _AoC_y2022_d22_make_transition(0, u));

  PyList_SET_ITEM(t, 2 * n + r, _AoC_y2022_d22_make_transition(3, l));
  PyList_SET_ITEM(t, 2 * n + d, _AoC_y2022_d22_make_transition(4, l));
  PyList_SET_ITEM(t, 2 * n + l, _AoC_y2022_d22_make_transition(1, r));
  PyList_SET_ITEM(t, 2 * n + u, _AoC_y2022_d22_make_transition(0, l));

  PyList_SET_ITEM(t, 3 * n + r, _AoC_y2022_d22_make_transition(5, u));
  PyList_SET_ITEM(t, 3 * n + d, _AoC_y2022_d22_make_transition(4, u));
  PyList_SET_ITEM(t, 3 * n + l, _AoC_y2022_d22_make_transition(2, r));
  PyList_SET_ITEM(t, 3 * n + u, _AoC_y2022_d22_make_transition(0, d));

  PyList_SET_ITEM(t, 4 * n + r, _AoC_y2022_d22_make_transition(5, l));
  PyList_SET_ITEM(t, 4 * n + d, _AoC_y2022_d22_make_transition(1, d));
  PyList_SET_ITEM(t, 4 * n + l, _AoC_y2022_d22_make_transition(2, d));
  PyList_SET_ITEM(t, 4 * n + u, _AoC_y2022_d22_make_transition(3, d));

  PyList_SET_ITEM(t, 5 * n + r, _AoC_y2022_d22_make_transition(0, r));
  PyList_SET_ITEM(t, 5 * n + d, _AoC_y2022_d22_make_transition(1, l));
  PyList_SET_ITEM(t, 5 * n + l, _AoC_y2022_d22_make_transition(4, r));
  PyList_SET_ITEM(t, 5 * n + u, _AoC_y2022_d22_make_transition(3, r));

  return t;
}

PyObject *_AoC_y2022_d22_full_input_transitions() {
  const enum _AoC_y2022_d22_map_facing r = map_facing_right;
  const enum _AoC_y2022_d22_map_facing d = map_facing_down;
  const enum _AoC_y2022_d22_map_facing l = map_facing_left;
  const enum _AoC_y2022_d22_map_facing u = map_facing_up;
  const enum _AoC_y2022_d22_map_facing n = num_map_facings;

  PyObject *t = PyList_New(6 * n);

  PyList_SET_ITEM(t, 0 * n + r, _AoC_y2022_d22_make_transition(1, l));
  PyList_SET_ITEM(t, 0 * n + d, _AoC_y2022_d22_make_transition(2, u));
  PyList_SET_ITEM(t, 0 * n + l, _AoC_y2022_d22_make_transition(3, l));
  PyList_SET_ITEM(t, 0 * n + u, _AoC_y2022_d22_make_transition(5, l));

  PyList_SET_ITEM(t, 1 * n + r, _AoC_y2022_d22_make_transition(4, r));
  PyList_SET_ITEM(t, 1 * n + d, _AoC_y2022_d22_make_transition(2, r));
  PyList_SET_ITEM(t, 1 * n + l, _AoC_y2022_d22_make_transition(0, r));
  PyList_SET_ITEM(t, 1 * n + u, _AoC_y2022_d22_make_transition(5, d));

  PyList_SET_ITEM(t, 2 * n + r, _AoC_y2022_d22_make_transition(1, d));
  PyList_SET_ITEM(t, 2 * n + d, _AoC_y2022_d22_make_transition(4, u));
  PyList_SET_ITEM(t, 2 * n + l, _AoC_y2022_d22_make_transition(3, u));
  PyList_SET_ITEM(t, 2 * n + u, _AoC_y2022_d22_make_transition(0, d));

  PyList_SET_ITEM(t, 3 * n + r, _AoC_y2022_d22_make_transition(4, l));
  PyList_SET_ITEM(t, 3 * n + d, _AoC_y2022_d22_make_transition(5, u));
  PyList_SET_ITEM(t, 3 * n + l, _AoC_y2022_d22_make_transition(0, l));
  PyList_SET_ITEM(t, 3 * n + u, _AoC_y2022_d22_make_transition(2, l));

  PyList_SET_ITEM(t, 4 * n + r, _AoC_y2022_d22_make_transition(1, r));
  PyList_SET_ITEM(t, 4 * n + d, _AoC_y2022_d22_make_transition(5, r));
  PyList_SET_ITEM(t, 4 * n + l, _AoC_y2022_d22_make_transition(3, r));
  PyList_SET_ITEM(t, 4 * n + u, _AoC_y2022_d22_make_transition(2, d));

  PyList_SET_ITEM(t, 5 * n + r, _AoC_y2022_d22_make_transition(4, d));
  PyList_SET_ITEM(t, 5 * n + d, _AoC_y2022_d22_make_transition(1, u));
  PyList_SET_ITEM(t, 5 * n + l, _AoC_y2022_d22_make_transition(0, u));
  PyList_SET_ITEM(t, 5 * n + u, _AoC_y2022_d22_make_transition(3, d));

  return t;
}

PyObject *_AoC_y2022_d22_jump_to_face(
    PyObject *state,
    const enum _AoC_y2022_d22_map_facing dst_side,
    const Py_ssize_t face_size) {
  const long y1 = PyLong_AsLong(PyTuple_GetItem(state, 0));
  const long x1 = PyLong_AsLong(PyTuple_GetItem(state, 1));
  const enum _AoC_y2022_d22_map_facing src_side =
      PyLong_AsLong(PyTuple_GetItem(state, 3));
  long y2 = y1;
  long x2 = x1;

  switch (dst_side) {
    case map_facing_right: {
      x2 = face_size - 1;
    } break;
    case map_facing_down: {
      y2 = face_size - 1;
    } break;
    case map_facing_left: {
      x2 = 0;
    } break;
    case map_facing_up: {
      y2 = 0;
    } break;
    default:
      break;
  }

  if (src_side == dst_side) {
    switch (dst_side) {
      case map_facing_right:
      case map_facing_left: {
        y2 = face_size - y1 - 1;
      } break;
      case map_facing_down:
      case map_facing_up: {
        x2 = face_size - x1 - 1;
      } break;
      default:
        break;
    }
  }
  if ((src_side + 1) % num_map_facings == dst_side) {
    switch (dst_side) {
      case map_facing_right:
      case map_facing_left: {
        y2 = face_size - x1 - 1;
      } break;
      case map_facing_down:
      case map_facing_up: {
        x2 = y1;
      } break;
      default:
        break;
    }
  }
  if ((src_side + 2) % num_map_facings == dst_side) {
    switch (dst_side) {
      case map_facing_right:
      case map_facing_left: {
        y2 = y1;
      } break;
      case map_facing_down:
      case map_facing_up: {
        x2 = x1;
      } break;
      default:
        break;
    }
  }
  if ((src_side + 3) % num_map_facings == dst_side) {
    switch (dst_side) {
      case map_facing_right:
      case map_facing_left: {
        y2 = x1;
      } break;
      case map_facing_down:
      case map_facing_up: {
        x2 = face_size - y1 - 1;
      } break;
      default:
        break;
    }
  }

  return PyTuple_Pack(2, PyLong_FromLong(y2), PyLong_FromLong(x2));
}

PyObject *_AoC_y2022_d22_wrapping_3d_step(PyObject *faces,
                                          PyObject *state,
                                          PyObject *transitions,
                                          const Py_ssize_t face_size) {
  long y1 = PyLong_AsLong(PyTuple_GetItem(state, 0));
  long x1 = PyLong_AsLong(PyTuple_GetItem(state, 1));
  const Py_ssize_t face_idx = PyLong_AsSsize_t(PyTuple_GetItem(state, 2));
  enum _AoC_y2022_d22_map_facing map_facing =
      PyLong_AsLong(PyTuple_GetItem(state, 3));

  switch (map_facing) {
    case map_facing_right: {
      ++x1;
    } break;
    case map_facing_left: {
      --x1;
    } break;
    case map_facing_down: {
      ++y1;
    } break;
    case map_facing_up: {
      --y1;
    } break;
    default:
      break;
  }

  const int still_inside =
      x1 <= face_size - 1 && y1 <= face_size - 1 && x1 >= 0 && y1 >= 0;
  if (still_inside) {
    PyObject *face_cells =
        PyDict_GetItemString(PyList_GetItem(faces, face_idx), "cells");
    PyObject *cell = PyList_GetItem(face_cells, y1 * face_size + x1);
    if (PyLong_AsLong(cell) == map_cell_wall) {
      return Py_NewRef(state);
    }
    return PyTuple_Pack(4,
                        PyLong_FromLong(y1),
                        PyLong_FromLong(x1),
                        PyLong_FromSsize_t(face_idx),
                        PyLong_FromLong(map_facing));
  }

  PyObject *transition =
      PyList_GetItem(transitions, face_idx * num_map_facings + map_facing);
  const Py_ssize_t dst_face_idx =
      PyLong_AsSsize_t(PyTuple_GetItem(transition, 0));
  const enum _AoC_y2022_d22_map_facing dst_side =
      PyLong_AsLong(PyTuple_GetItem(transition, 1));

  PyObject *new_yx = _AoC_y2022_d22_jump_to_face(state, dst_side, face_size);
  const long y2 = PyLong_AsLong(PyTuple_GetItem(new_yx, 0));
  const long x2 = PyLong_AsLong(PyTuple_GetItem(new_yx, 1));
  PyObject *dst_face_cells =
      PyDict_GetItemString(PyList_GetItem(faces, dst_face_idx), "cells");
  PyObject *dst_cell = PyList_GetItem(dst_face_cells, y2 * face_size + x2);
  if (PyLong_AsLong(dst_cell) == map_cell_wall) {
    return Py_NewRef(state);
  }
  Py_DECREF(new_yx);

  map_facing = (dst_side + 2) % num_map_facings;

  return PyTuple_Pack(4,
                      PyLong_FromLong(y2),
                      PyLong_FromLong(x2),
                      PyLong_FromSsize_t(dst_face_idx),
                      PyLong_FromLong(map_facing));
}

PyObject *_AoC_y2022_d22_find_part2(struct _AoC_y2022_d22_map2d *const map2d,
                                    PyObject *moves,
                                    int use_example) {
  const Py_ssize_t face_size = use_example ? 4 : 50;
  PyObject *transitions = use_example ? _AoC_y2022_d22_example_transitions()
                                      : _AoC_y2022_d22_full_input_transitions();
  const Py_ssize_t num_y_faces = (map2d->height - 2) / face_size;
  const Py_ssize_t num_x_faces = (map2d->width - 2) / face_size;

  PyObject *faces = PyList_New(0);
  for (Py_ssize_t y_face = 0; y_face < num_y_faces; ++y_face) {
    for (Py_ssize_t x_face = 0; x_face < num_x_faces; ++x_face) {
      PyObject *face =
          _AoC_y2022_d22_parse_map3d_face(map2d, y_face, x_face, face_size);
      if (face) {
        PyList_Append(faces, face);
      }
    }
  }

  PyObject *state = PyTuple_Pack(4,
                                 PyLong_FromLong(0),
                                 PyLong_FromLong(0),
                                 PyLong_FromLong(0),
                                 PyLong_FromLong(map_facing_right));

  for (Py_ssize_t i = 0; i < PyList_Size(moves); ++i) {
    PyObject *move = PyList_GetItem(moves, i);
    const Py_UCS4 move_ch = PyUnicode_READ_CHAR(move, 0);
    if (Py_UNICODE_ISDIGIT(move_ch)) {
      PyObject *num_steps = PyLong_FromUnicodeObject(move, 10);
      for (long step = 0; step < PyLong_AsLong(num_steps); ++step) {
        PyObject *next_state = _AoC_y2022_d22_wrapping_3d_step(faces,
                                                               state,
                                                               transitions,
                                                               face_size);
        Py_SETREF(state, next_state);
      }
      Py_DECREF(num_steps);
    } else {
      enum _AoC_y2022_d22_map_facing map_facing =
          PyLong_AsLong(PyTuple_GetItem(state, 3));
      switch (move_ch) {
        case 'R': {
          map_facing = (map_facing + 1) % num_map_facings;
        } break;
        case 'L': {
          map_facing = (map_facing + num_map_facings - 1) % num_map_facings;
        } break;
      }
      PyTuple_SetItem(state, 3, PyLong_FromLong(map_facing));
    }
  }

  const long y = PyLong_AsLong(PyTuple_GetItem(state, 0));
  const long x = PyLong_AsLong(PyTuple_GetItem(state, 1));

  const Py_ssize_t face_idx = PyLong_AsSsize_t(PyTuple_GetItem(state, 2));
  PyObject *face = PyList_GetItem(faces, face_idx);
  const long y_face = PyLong_AsLong(PyDict_GetItemString(face, "y"));
  const long x_face = PyLong_AsLong(PyDict_GetItemString(face, "x"));
  const long y_map = y_face * face_size + y + 1;
  const long x_map = x_face * face_size + x + 1;

  const long facing = PyLong_AsLong(PyTuple_GetItem(state, 3));

  Py_DECREF(state);
  Py_DECREF(faces);

  return PyLong_FromLong(1000 * y_map + 4 * x_map + facing);
}

PyObject *AoC_y2022_d22(PyObject *unicode_input) {
  PyObject *solution = 0;
  struct _AoC_y2022_d22_map2d map2d = {0};
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  if (!lines) {
    goto done;
  }

  PyObject *map_lines = PySequence_GetSlice(lines, 0, PyList_Size(lines) - 2);
  if (!_AoC_y2022_d22_parse_map2d(&map2d, map_lines)) {
    goto done;
  }
  PyObject *moves =
      _AoC_y2022_d22_parse_moves(PyList_GetItem(lines, PyList_Size(lines) - 1));

  PyObject *part1 = _AoC_y2022_d22_find_part1(&map2d, moves);
  const int use_example = 0;
  PyObject *part2 = _AoC_y2022_d22_find_part2(&map2d, moves, use_example);
  solution = PyUnicode_FromFormat("%S %S", part1, part2);
  Py_DECREF(part1);
  Py_DECREF(part2);

done:
  Py_XDECREF(map2d.cells);
  Py_XDECREF(lines);
  return solution;
}

#endif  // _AOC_Y2022_D22_H_INCLUDED
