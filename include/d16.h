#ifndef _AOC_Y2022_D16_H_INCLUDED
#define _AOC_Y2022_D16_H_INCLUDED
#include "common.h"

PyObject *_AoC_y2022_d16_total_pressure(PyObject *flows, PyObject *solution) {
  PyObject *total_pressure = PyLong_FromLong(0);
  for (Py_ssize_t i_valve = 0; i_valve < PyList_Size(flows); ++i_valve) {
    PyObject *flow = PyList_GetItem(flows, i_valve);
    PyObject *time_left = PyList_GetItem(solution, i_valve);
    PyObject *pressure = PyNumber_Multiply(flow, time_left);
    Py_SETREF(total_pressure, PyNumber_Add(total_pressure, pressure));
    Py_DECREF(pressure);
  }
  return total_pressure;
}

PyObject *_AoC_y2022_d16_copy_list(PyObject *l1) {
  const Py_ssize_t n = PyList_Size(l1);
  PyObject *l2 = PyList_New(n);
  for (Py_ssize_t i = 0; i < n; ++i) {
    PyList_SET_ITEM(l2, i, Py_NewRef(PyList_GetItem(l1, i)));
  }
  return l2;
}

PyObject *_AoC_y2022_d16_find_max_pressures(const Py_ssize_t src,
                                            PyObject *const flows,
                                            PyObject *const skip_valves,
                                            PyObject *const shortest_paths,
                                            PyObject *const solution,
                                            const long time_left) {
  PyObject *best_solution = Py_NewRef(solution);
  PyObject *best_pressure = _AoC_y2022_d16_total_pressure(flows, best_solution);
  if (time_left <= 0) {
    goto done;
  }

  const Py_ssize_t num_valves = PyList_Size(flows);
  for (Py_ssize_t dst = 0; dst < num_valves; ++dst) {
    if (skip_valves) {
      PyObject *dst_key = PyLong_FromSsize_t(dst);
      const int skip = PySet_Contains(skip_valves, dst_key);
      Py_DECREF(dst_key);
      if (skip) {
        continue;
      }
    }
    if (src == dst) {
      continue;
    }
    const int already_visited =
        PyLong_AsLong(PyList_GetItem(solution, dst)) > 0;
    if (already_visited) {
      continue;
    }

    PyObject *src2dst =
        PyTuple_Pack(2, PyLong_FromLong(src), PyLong_FromLong(dst));
    PyObject *dist = PyDict_GetItem(shortest_paths, src2dst);
    Py_DECREF(src2dst);
    const long next_time_left = time_left - PyLong_AsLong(dist) - 1;
    if (next_time_left < 0) {
      continue;
    }
    PyObject *next_solution = _AoC_y2022_d16_copy_list(solution);
    PyList_SetItem(next_solution, dst, PyLong_FromLong(next_time_left));
    PyObject *candidate_solution =
        _AoC_y2022_d16_find_max_pressures(dst,
                                          flows,
                                          skip_valves,
                                          shortest_paths,
                                          next_solution,
                                          next_time_left);
    PyObject *pressure =
        _AoC_y2022_d16_total_pressure(flows, candidate_solution);
    if (1 == PyObject_RichCompareBool(best_pressure, pressure, Py_LT)) {
      Py_SETREF(best_solution, candidate_solution);
      Py_SETREF(best_pressure, pressure);
    } else {
      Py_DECREF(candidate_solution);
      Py_DECREF(pressure);
    }
    Py_DECREF(next_solution);
  }

done:
  Py_DECREF(best_pressure);
  return best_solution;
}

PyObject *_AoC_y2022_d16_find_next_min_dist_node(PyObject *visited,
                                                 PyObject *distances) {
  PyObject *min_id = 0;

  PyObject *valve_ids = PyDict_Keys(visited);
  const Py_ssize_t num_valves = PyList_Size(valve_ids);

  for (Py_ssize_t i = 0; i < num_valves; ++i) {
    PyObject *cur_id = PyList_GetItem(valve_ids, i);
    if (PyDict_GetItem(visited, cur_id) == Py_True) {
      continue;
    }
    if (!min_id) {
      min_id = cur_id;
      continue;
    }
    PyObject *dist_cur = PyDict_GetItem(distances, cur_id);
    PyObject *dist_min = PyDict_GetItem(distances, min_id);
    if (PyObject_RichCompareBool(dist_cur, dist_min, Py_LT) == 1) {
      min_id = cur_id;
    }
  }

  return min_id;
}

// Dijkstra, undirected, every edge has length 1
PyObject *_AoC_y2022_d16_shortest_path(PyObject *valves,
                                       PyObject *src,
                                       PyObject *dst) {
  PyObject *valve_ids = PyDict_Keys(valves);
  const Py_ssize_t num_valves = PyList_Size(valve_ids);

  PyObject *visited = PyDict_New();
  PyObject *distances = PyDict_New();
  for (Py_ssize_t i = 0; i < num_valves; ++i) {
    PyObject *v = PyList_GetItem(valve_ids, i);
    PyDict_SetItem(visited, v, PyBool_FromLong(0));
    PyDict_SetItem(distances, v, PyLong_FromLong(num_valves));
  }

  PyDict_SetItem(distances, src, PyLong_FromLong(0));

  while (PyDict_GetItem(visited, dst) == Py_False) {
    PyObject *cur = _AoC_y2022_d16_find_next_min_dist_node(visited, distances);
    PyDict_SetItem(visited, cur, PyBool_FromLong(1));
    PyObject *adjacent =
        PyDict_GetItemString(PyDict_GetItem(valves, cur), "adjacent");

    for (Py_ssize_t i_adj = 0; i_adj < PyList_Size(adjacent); ++i_adj) {
      PyObject *adj = PyList_GetItem(adjacent, i_adj);
      if (PyDict_GetItem(visited, adj) == Py_True) {
        continue;
      }
      PyObject *dist_to_cur = PyDict_GetItem(distances, cur);
      PyObject *dist_to_adj = PyDict_GetItem(distances, adj);
      PyObject *one = PyLong_FromLong(1);
      PyObject *cur_to_adj = PyNumber_Add(dist_to_cur, one);
      Py_DECREF(one);
      if (PyObject_RichCompareBool(cur_to_adj, dist_to_adj, Py_LT) == 1) {
        PyDict_SetItem(distances, adj, cur_to_adj);
      } else {
        Py_DECREF(cur_to_adj);
      }
    }
  }

  PyObject *min_dist = Py_NewRef(PyDict_GetItem(distances, dst));
  Py_DECREF(distances);
  Py_DECREF(visited);
  return min_dist;
}

PyObject *_AoC_y2022_d16_all_pairs_shortest_paths(PyObject *valves) {
  PyObject *shortest_paths = PyDict_New();

  PyObject *valve_ids = PyDict_Keys(valves);
  const Py_ssize_t num_valves = PyList_Size(valve_ids);

  for (Py_ssize_t i = 0; i < num_valves; ++i) {
    PyObject *src = PyList_GetItem(valve_ids, i);
    for (Py_ssize_t j = i + 1; j < num_valves; ++j) {
      PyObject *dst = PyList_GetItem(valve_ids, j);
      PyObject *dist = _AoC_y2022_d16_shortest_path(valves, src, dst);
      PyObject *src2dst = PyTuple_Pack(2, src, dst);
      PyObject *dst2src = PyTuple_Pack(2, dst, src);
      PyDict_SetItem(shortest_paths, src2dst, dist);
      PyDict_SetItem(shortest_paths, dst2src, dist);
      Py_DECREF(src2dst);
      Py_DECREF(dst2src);
    }
  }

  return shortest_paths;
}

PyObject *_AoC_y2022_d16_all_valve_partitions(PyObject *flows) {
  PyObject *queue = PyList_New(0);
  {
    const Py_ssize_t num_valves = PyList_Size(flows);
    PyObject *valves = PyList_New(num_valves - 1);
    for (Py_ssize_t i = 1; i < num_valves; ++i) {
      PyList_SET_ITEM(valves, i - 1, PyLong_FromSsize_t(i));
    }
    PyObject *a = PySet_New(valves);
    PyObject *b = PySet_New(0);
    PyList_Append(queue, PyTuple_Pack(2, a, b));
  }

  PyObject *partitions = PySet_New(0);
  PyObject *unique_subsets = PySet_New(0);

  while (PyList_Size(queue)) {
    PyObject *item = Py_NewRef(PyList_GetItem(queue, 0));
    PySequence_DelItem(queue, 0);
    PyObject *a = PyTuple_GetItem(item, 0);
    PyObject *b = PyTuple_GetItem(item, 1);
    if (PySet_Size(a) < PySet_Size(b)) {
      Py_DECREF(item);
      continue;
    }
    PyObject *a_list = PySequence_List(a);
    PyObject *b_list = PySequence_List(b);
    PyList_Sort(a_list);
    PyList_Sort(b_list);
    PyObject *a_tuple = PySequence_Tuple(a);
    PyObject *b_tuple = PySequence_Tuple(b);
    const Py_ssize_t bsize = PyTuple_Size(b_tuple);
    if (bsize) {
      if (!PySet_Contains(unique_subsets, a_tuple) &&
          !PySet_Contains(unique_subsets, b_tuple)) {
        PySet_Add(partitions, PyTuple_Pack(2, a_tuple, b_tuple));
      }
    }
    PySet_Add(unique_subsets, a_tuple);
    PySet_Add(unique_subsets, b_tuple);
    for (Py_ssize_t i = 0; i < PyList_Size(a_list); ++i) {
      PyObject *x = PyList_GetItem(a_list, i);
      if (!bsize ||
          (1 == PyObject_RichCompareBool(PyList_GetItem(b_list, bsize - 1),
                                         x,
                                         Py_LT))) {
        PyObject *a2 = PySet_New(a);
        PyObject *b2 = PySet_New(b);
        PySet_Discard(a2, x);
        PySet_Add(b2, x);
        PyList_Append(queue, PyTuple_Pack(2, a2, b2));
      }
    }
    Py_DECREF(a_list);
    Py_DECREF(b_list);
    Py_DECREF(item);
  }
  Py_DECREF(queue);
  Py_DECREF(unique_subsets);

  Py_SETREF(partitions, PySequence_List(partitions));
  return partitions;
}

PyObject *_AoC_y2022_d16_init_solution(PyObject *flows) {
  const Py_ssize_t num_flows = PyList_Size(flows);
  PyObject *solution = PyList_New(num_flows);
  for (Py_ssize_t i = 0; i < num_flows; ++i) {
    PyList_SET_ITEM(solution, i, PyLong_FromLong(0));
  }
  return solution;
}

PyObject *_AoC_y2022_d16_find_part1(PyObject *flows, PyObject *shortest_paths) {
  PyObject *solution =
      _AoC_y2022_d16_find_max_pressures(0,
                                        flows,
                                        0,
                                        shortest_paths,
                                        _AoC_y2022_d16_init_solution(flows),
                                        30);
  PyObject *total_pressure = _AoC_y2022_d16_total_pressure(flows, solution);
  Py_DECREF(solution);
  return total_pressure;
}

PyObject *_AoC_y2022_d16_find_part2(PyObject *flows, PyObject *shortest_paths) {
  PyObject *partitions = _AoC_y2022_d16_all_valve_partitions(flows);
  PyObject *max_total_pressure = PyLong_FromLong(0);
  for (Py_ssize_t i = 0; i < PyList_Size(partitions); ++i) {
    PyObject *partition = PyList_GetItem(partitions, i);
    PyObject *combined_pressure = PyLong_FromLong(0);
    for (Py_ssize_t p = 0; p < PyTuple_Size(partition); ++p) {
      PyObject *skip_valves = PySet_New(PyTuple_GetItem(partition, p));
      PyObject *max_pressures =
          _AoC_y2022_d16_find_max_pressures(0,
                                            flows,
                                            skip_valves,
                                            shortest_paths,
                                            _AoC_y2022_d16_init_solution(flows),
                                            26);
      PyObject *total_pressure =
          _AoC_y2022_d16_total_pressure(flows, max_pressures);
      Py_SETREF(combined_pressure,
                PyNumber_Add(combined_pressure, total_pressure));
      Py_DECREF(total_pressure);
      Py_DECREF(max_pressures);
      Py_DECREF(skip_valves);
    }
    if (1 == PyObject_RichCompareBool(max_total_pressure,
                                      combined_pressure,
                                      Py_LT)) {
      Py_SETREF(max_total_pressure, combined_pressure);
    } else {
      Py_DECREF(combined_pressure);
    }
  }
  Py_DECREF(partitions);
  return max_total_pressure;
}

PyObject *_AoC_y2022_d16_parse_valves(PyObject *lines) {
  const Py_ssize_t num_valves = PyList_Size(lines);
  PyObject *valves = PyDict_New();

  for (Py_ssize_t i = 0; i < num_valves; ++i) {
    PyObject *line = PyList_GetItem(lines, i);
    PyObject *line_parts = AoC_unicode_split(line, "; ", 1);

    PyObject *lhs = PyList_GetItem(line_parts, 0);
    PyObject *flow = AoC_unicode_split(lhs, "=", 1);
    Py_SETREF(flow, Py_NewRef(PyList_GetItem(flow, 1)));
    Py_SETREF(flow, PyLong_FromUnicodeObject(flow, 10));
    PyObject *valve_id = AoC_unicode_split(lhs, " ", 2);
    Py_SETREF(valve_id, Py_NewRef(PyList_GetItem(valve_id, 1)));

    PyObject *rhs = PyList_GetItem(line_parts, 1);
    PyObject *adjacent = AoC_unicode_split(rhs, " ", 4);
    Py_SETREF(adjacent, Py_NewRef(PyList_GetItem(adjacent, 4)));
    Py_SETREF(adjacent, AoC_unicode_split(adjacent, ", ", -1));

    Py_DECREF(line_parts);

    PyObject *valve = PyDict_New();
    PyDict_SetItemString(valve, "flow", flow);
    PyDict_SetItemString(valve, "adjacent", adjacent);
    PyDict_SetItem(valves, valve_id, valve);
  }

  return valves;
}

PyObject *_AoC_y2022_d16_compress_valves(PyObject *valves,
                                         PyObject *shortest_paths) {
  PyObject *valve_ids = PyDict_Keys(valves);
  PyList_Sort(valve_ids);

  PyObject *valve_indexes = PyDict_New();
  PyObject *flows = PyList_New(0);

  PyObject *root_id = PyUnicode_FromString("AA");

  for (Py_ssize_t i = 0; i < PyList_Size(valve_ids); ++i) {
    PyObject *valve_id = PyList_GetItem(valve_ids, i);
    PyObject *valve = PyDict_GetItem(valves, valve_id);
    PyObject *flow = PyDict_GetItemString(valve, "flow");
    if (PyUnicode_Compare(valve_id, root_id) == 0 || PyLong_AsLong(flow)) {
      const Py_ssize_t idx = PyList_Size(flows);
      PyList_Append(flows, Py_NewRef(flow));
      PyDict_SetItem(valve_indexes, valve_id, PyLong_FromSsize_t(idx));
    }
  }

  PyObject *indexed_shortest_paths = PyDict_New();
  PyObject *path_items = PyDict_Items(shortest_paths);
  for (Py_ssize_t i = 0; i < PyList_Size(path_items); ++i) {
    PyObject *path = PyList_GetItem(path_items, i);
    PyObject *src = PyTuple_GetItem(PyTuple_GetItem(path, 0), 0);
    PyObject *dst = PyTuple_GetItem(PyTuple_GetItem(path, 0), 1);
    PyObject *src_idx = PyDict_GetItem(valve_indexes, src);
    PyObject *dst_idx = PyDict_GetItem(valve_indexes, dst);
    if (src_idx && dst_idx) {
      PyObject *dist = PyTuple_GetItem(path, 1);
      PyObject *src2dst = PyTuple_Pack(2, src_idx, dst_idx);
      PyDict_SetItem(indexed_shortest_paths, src2dst, Py_NewRef(dist));
    }
  }

  Py_DECREF(valve_indexes);
  Py_DECREF(root_id);

  return PyTuple_Pack(2, flows, indexed_shortest_paths);
}

PyObject *AoC_y2022_d16(PyObject *unicode_input) {
  PyObject *solution = 0;
  PyObject *valves = 0;
  PyObject *shortest_paths = 0;
  PyObject *flows = 0;
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  if (!lines) {
    goto done;
  }

  valves = _AoC_y2022_d16_parse_valves(lines);
  if (!valves) {
    goto done;
  }

  shortest_paths = _AoC_y2022_d16_all_pairs_shortest_paths(valves);
  if (!shortest_paths) {
    goto done;
  }

  {
    PyObject *tmp = _AoC_y2022_d16_compress_valves(valves, shortest_paths);
    if (!tmp) {
      goto done;
    }
    flows = PyTuple_GetItem(tmp, 0);
    if (!flows) {
      goto done;
    }
    Py_SETREF(shortest_paths, PyTuple_GetItem(tmp, 1));
    if (!shortest_paths) {
      goto done;
    }
  }

  PyObject *part1 = _AoC_y2022_d16_find_part1(flows, shortest_paths);
  PyObject *part2 = _AoC_y2022_d16_find_part2(flows, shortest_paths);
  solution = PyUnicode_FromFormat("%S %S", part1, part2);
  Py_DECREF(part1);
  Py_DECREF(part2);

done:
  Py_XDECREF(flows);
  Py_XDECREF(shortest_paths);
  Py_XDECREF(valves);
  Py_XDECREF(lines);
  return solution;
}

#endif  // _AOC_Y2022_D16_H_INCLUDED
