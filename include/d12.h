#ifndef _AOC_Y2022_D12_H_INCLUDED
#define _AOC_Y2022_D12_H_INCLUDED
#include "common.h"

struct _AoC_y2022_d12_graph {
  PyObject *elevation;
  const Py_ssize_t height;
  const Py_ssize_t width;
};

Py_ssize_t _AoC_y2022_d12_find_next_unvisited(PyObject *visited,
                                              PyObject *distances) {
  Py_ssize_t min_pos = 0;
  for (Py_ssize_t i = 0; i < PyList_Size(visited); ++i) {
    if (PyList_GetItem(visited, i) == Py_True) {
      continue;
    }
    if (PyList_GetItem(visited, min_pos) == Py_True) {
      min_pos = i;
    } else {
      PyObject *dist_min = PyList_GetItem(distances, min_pos);
      PyObject *dist_cur = PyList_GetItem(distances, i);
      if (PyObject_RichCompareBool(dist_cur, dist_min, Py_LT) == 1) {
        min_pos = i;
      }
    }
  }
  return min_pos;
}

Py_ssize_t _AoC_y2022_d12_find_elevation(struct _AoC_y2022_d12_graph graph,
                                         long value) {
  Py_ssize_t i = 0;
  for (; i < PyList_Size(graph.elevation); ++i) {
    if (PyLong_AsLong(PyList_GetItem(graph.elevation, i)) == value) {
      return i;
    }
  }
  return i;
}

PyObject *_AoC_y2022_d12_find_shortest_path(struct _AoC_y2022_d12_graph graph,
                                            Py_ssize_t src_pos,
                                            Py_ssize_t dst_pos) {
  Py_ssize_t num_nodes = PyList_Size(graph.elevation);
  PyObject *visited = PyList_New(num_nodes);
  PyObject *distances = PyList_New(num_nodes);
  for (Py_ssize_t i = 0; i < num_nodes; ++i) {
    int is_padding = i < graph.width || i >= (graph.height - 1) * graph.width ||
                     i % graph.width == 0 || (i + 1) % graph.width == 0;
    PyList_SetItem(visited, i, PyBool_FromLong(is_padding));
    PyList_SetItem(distances, i, PyLong_FromLong(num_nodes));
  }
  PyList_SetItem(distances, src_pos, PyLong_FromLong(0));
  while (PyList_GetItem(visited, dst_pos) == Py_False) {
    Py_ssize_t current = _AoC_y2022_d12_find_next_unvisited(visited, distances);
    PyList_SetItem(visited, current, PyBool_FromLong(1));
    for (size_t i = 0; i < 4; ++i) {
      Py_ssize_t adjacent = (Py_ssize_t[]){
          current - graph.width,  // N
          current + 1,            // E
          current + graph.width,  // S
          current - 1,            // W
      }[i];
      if (PyList_GetItem(visited, adjacent) == Py_True) {
        continue;
      }
      PyObject *cur_elevation = PyList_GetItem(graph.elevation, current);
      PyObject *adj_elevation = PyList_GetItem(graph.elevation, adjacent);
      PyObject *delta_py = PyNumber_Subtract(adj_elevation, cur_elevation);
      long delta = PyLong_AsLong(delta_py);
      Py_DECREF(delta_py);
      if (delta > 1) {
        continue;
      }
      PyObject *dist_to_cur = PyList_GetItem(distances, current);
      PyObject *dist_to_adj = PyList_GetItem(distances, adjacent);
      PyObject *step_dist = PyLong_FromLong(PyLong_AsLong(adj_elevation) > 0);
      PyObject *cur_to_adj = PyNumber_Add(dist_to_cur, step_dist);
      Py_DECREF(step_dist);
      if (PyObject_RichCompareBool(cur_to_adj, dist_to_adj, Py_LT) == 1) {
        PyList_SetItem(distances, adjacent, cur_to_adj);
      } else {
        Py_DECREF(cur_to_adj);
      }
    }
  }
  PyObject *min_dist = PyList_GetItem(distances, dst_pos);
  Py_INCREF(min_dist);
  Py_DECREF(distances);
  Py_DECREF(visited);
  return min_dist;
}

struct _AoC_y2022_d12_graph _AoC_y2022_d12_parse_graph(PyObject *input) {
  PyObject *lines = PyUnicode_Splitlines(input, 0);
  if (!lines) {
    goto error;
  }
  const Py_ssize_t height = 2 + PyList_Size(lines);
  const Py_ssize_t width = 2 + PyUnicode_GetLength(PyList_GetItem(lines, 0));
  const Py_ssize_t num_nodes = height * width;
  PyObject *elevation = PyList_New(num_nodes);
  if (!elevation) {
    Py_DECREF(lines);
    goto error;
  }
  for (Py_ssize_t row = 0; row < height; ++row) {
    for (Py_ssize_t col = 0; col < width; ++col) {
      PyObject *node = 0;
      const int in_padding =
          row == 0 || row == height - 1 || col == 0 || col == width - 1;
      if (in_padding) {
        node = PyLong_FromLong(0xffff);
      } else {
        PyObject *line = PyList_GetItem(lines, row - 1);
        Py_UCS4 ch = PyUnicode_READ_CHAR(line, col - 1);
        switch (ch) {
          case 'S': {
            node = PyLong_FromLong(0);
          } break;
          case 'E': {
            node = PyLong_FromLong('z' - 'a' + 2);
          } break;
          default: {
            node = PyLong_FromLong(ch - 'a' + 1);
          } break;
        }
      }
      PyList_SET_ITEM(elevation, row * width + col, node);
    }
  }
  Py_DECREF(lines);
  return (struct _AoC_y2022_d12_graph){
      .elevation = elevation,
      .height = height,
      .width = width,
  };

error:
  return (struct _AoC_y2022_d12_graph){0};
}

PyObject *AoC_y2022_d12(PyObject *unicode_input) {
  struct _AoC_y2022_d12_graph graph = _AoC_y2022_d12_parse_graph(unicode_input);
  Py_ssize_t src_pos = _AoC_y2022_d12_find_elevation(graph, 0);
  Py_ssize_t dst_pos = _AoC_y2022_d12_find_elevation(graph, 'z' - 'a' + 2);

  PyObject *part1 = _AoC_y2022_d12_find_shortest_path(graph, src_pos, dst_pos);

  PyObject *part2_elevation = PyList_New(PyList_Size(graph.elevation));
  for (Py_ssize_t i = 0; i < PyList_Size(part2_elevation); ++i) {
    long elevation = PyLong_AsLong(PyList_GetItem(graph.elevation, i));
    if (elevation > 0) {
      --elevation;
    }
    PyList_SET_ITEM(part2_elevation, i, PyLong_FromLong(elevation));
  }
  Py_SETREF(graph.elevation, part2_elevation);

  PyObject *part2 = _AoC_y2022_d12_find_shortest_path(graph, src_pos, dst_pos);

  PyObject *solution = PyUnicode_FromFormat("%S %S", part1, part2);
  Py_DECREF(graph.elevation);
  Py_DECREF(part1);
  Py_DECREF(part2);
  return solution;
}

#endif  // _AOC_Y2022_D12_H_INCLUDED
