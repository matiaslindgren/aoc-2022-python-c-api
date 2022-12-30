#ifndef _AOC_Y2022_D16_H_INCLUDED
#define _AOC_Y2022_D16_H_INCLUDED
#include <limits.h>

#include "common.h"

// Floyd-Warshall
PyObject *_AoC_y2022_d16_all_pairs_shortest_paths(PyObject *valves) {
  const Py_ssize_t n = PyList_Size(valves);
  PyObject *dist = PyList_New(n * n);

  for (Py_ssize_t i = 0; i < n; ++i) {
    for (Py_ssize_t j = 0; j < n; ++j) {
      PyList_SET_ITEM(dist, i * n + j, PyLong_FromLong(LONG_MAX));
    }
  }

  for (Py_ssize_t i = 0; i < n; ++i) {
    PyObject *valve = PyList_GetItem(valves, i);
    PyObject *adjacent = PyTuple_GetItem(valve, 2);
    for (Py_ssize_t i_adj = 0; i_adj < PyList_Size(adjacent); ++i_adj) {
      PyObject *adj_id = PyList_GetItem(adjacent, i_adj);
      Py_ssize_t j = 0;
      while (j < PyList_Size(valves)) {
        PyObject *adj = PyList_GetItem(valves, j);
        if (PyUnicode_Compare(PyTuple_GetItem(adj, 0), adj_id) == 0) {
          break;
        }
        ++j;
      }
      PyList_SetItem(dist, i * n + j, PyLong_FromLong(1));
      PyList_SetItem(dist, j * n + i, PyLong_FromLong(1));
    }
  }

  for (Py_ssize_t i = 0; i < n; ++i) {
    PyList_SetItem(dist, i * n + i, PyLong_FromLong(0));
  }

  for (Py_ssize_t k = 0; k < n; ++k) {
    for (Py_ssize_t i = 0; i < n; ++i) {
      for (Py_ssize_t j = 0; j < n; ++j) {
        PyObject *dist_cur = PyList_GetItem(dist, i * n + j);
        PyObject *dist_new = PyNumber_Add(PyList_GetItem(dist, i * n + k),
                                          PyList_GetItem(dist, k * n + j));
        if (PyObject_RichCompareBool(dist_new, dist_cur, Py_LT) == 1) {
          PyList_SetItem(dist, i * n + j, dist_new);
        } else {
          Py_DECREF(dist_new);
        }
      }
    }
  }

  return dist;
}

PyObject *_AoC_y2022_d16_copy_list(PyObject *l1) {
  const Py_ssize_t n = PyList_Size(l1);
  PyObject *l2 = PyList_New(n);
  for (Py_ssize_t i = 0; i < n; ++i) {
    PyList_SET_ITEM(l2, i, Py_NewRef(PyList_GetItem(l1, i)));
  }
  return l2;
}

PyObject *_AoC_y2022_d16_total_pressure(PyObject *valves, PyObject *solution) {
  PyObject *total_pressure = PyLong_FromLong(0);
  for (Py_ssize_t i = 0; i < PyList_Size(valves); ++i) {
    PyObject *time_opened = PyList_GetItem(solution, i);
    if (time_opened == Py_None) {
      continue;
    }
    PyObject *flow = PyTuple_GetItem(PyList_GetItem(valves, i), 1);
    PyObject *pressure = PyNumber_Multiply(flow, time_opened);
    Py_SETREF(total_pressure, PyNumber_Add(total_pressure, pressure));
    Py_DECREF(pressure);
  }
  return total_pressure;
}

PyObject *_AoC_y2022_d16_make_state(const Py_ssize_t src,
                                    PyObject *time_left,
                                    PyObject *solution) {
  PyObject *state = PyTuple_New(3);
  PyTuple_SET_ITEM(state, 0, PyLong_FromSsize_t(src));
  PyTuple_SET_ITEM(state, 1, time_left);
  PyTuple_SET_ITEM(state, 2, solution);
  return state;
}

PyObject *_AoC_y2022_d16_find_max_total_pressure(PyObject *const valves,
                                                 PyObject *const distances,
                                                 const long time_limit) {
  const Py_ssize_t num_valves = PyList_Size(valves);
  if (!num_valves) {
    return PyLong_FromLong(0);
  }

  const Py_ssize_t start_valve = 0;
  PyObject *best_solution = PyList_New(num_valves);
  for (Py_ssize_t i = 0; i < num_valves; ++i) {
    PyList_SET_ITEM(best_solution, i, Py_NewRef(Py_None));
  }

  PyList_SetItem(best_solution, start_valve, PyLong_FromLong(0));
  PyObject *max_pressure = PyLong_FromLong(0);

  PyObject *branches = PyList_New(1);
  PyList_SET_ITEM(branches,
                  0,
                  _AoC_y2022_d16_make_state(start_valve,
                                            PyLong_FromLong(time_limit),
                                            best_solution));

  while (PyList_Size(branches)) {
    PyObject *item = PyList_GetItem(branches, PyList_Size(branches) - 1);
    Py_INCREF(item);
    const Py_ssize_t src = PyLong_AsSsize_t(PyTuple_GetItem(item, 0));
    PyObject *time_left = Py_NewRef(PyTuple_GetItem(item, 1));
    PyObject *solution = Py_NewRef(PyTuple_GetItem(item, 2));
    PySequence_DelItem(branches, PyList_Size(branches) - 1);

    PyObject *pressure = _AoC_y2022_d16_total_pressure(valves, solution);
    if (PyObject_RichCompareBool(pressure, max_pressure, Py_GT) == 1) {
      best_solution = solution;
      Py_SETREF(max_pressure, pressure);
    } else {
      Py_DECREF(pressure);
    }

    if (!PyLong_AsLong(time_left)) {
      goto end;
    }

    PyObject *one = PyLong_FromLong(1);
    Py_SETREF(time_left, PyNumber_Subtract(time_left, one));
    Py_DECREF(one);

    for (Py_ssize_t dst = 0; dst < num_valves; ++dst) {
      const int already_open =
          src == dst || PyList_GetItem(solution, dst) != Py_None;
      if (already_open) {
        continue;
      }

      PyObject *dist = PyList_GetItem(distances, src * num_valves + dst);
      const int no_time_left =
          1 == PyObject_RichCompareBool(dist, time_left, Py_GE);
      if (no_time_left) {
        continue;
      }

      PyObject *time_opened = PyNumber_Subtract(time_left, dist);
      PyObject *next_solution = _AoC_y2022_d16_copy_list(solution);
      PyList_SetItem(next_solution, dst, Py_NewRef(time_opened));
      PyObject *next_state =
          _AoC_y2022_d16_make_state(dst, time_opened, next_solution);
      PyList_Append(branches, next_state);
      Py_DECREF(next_state);
    }

  end:
    Py_DECREF(time_left);
    Py_DECREF(solution);
    Py_DECREF(item);
  }

  Py_DECREF(branches);

  return max_pressure;
}

PyObject *_AoC_y2022_d16_parse_valves(PyObject *lines) {
  const Py_ssize_t num_valves = PyList_Size(lines);
  PyObject *valves = PyList_New(num_valves);

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

    PyObject *valve = PyTuple_Pack(3, valve_id, flow, adjacent);
    PyList_SET_ITEM(valves, i, valve);
    Py_DECREF(valve_id);
    Py_DECREF(flow);
    Py_DECREF(adjacent);
  }

  PyList_Sort(valves);
  return valves;
}

PyObject *_AoC_y2022_d16_select_subgraph(PyObject *valves,
                                         PyObject *distances,
                                         PyObject *indexes) {
  const Py_ssize_t num_valves = PyList_Size(valves);

  PyObject *index_map = PyList_New(0);
  PyObject *root_id = PyUnicode_FromString("AA");

  for (Py_ssize_t i = 0; i < num_valves; ++i) {
    PyObject *valve_id = PyTuple_GetItem(PyList_GetItem(valves, i), 0);
    const int is_root_node = PyUnicode_Compare(valve_id, root_id) == 0;
    PyObject *idx = PyLong_FromSsize_t(i);
    const int is_in_subgraph = PySet_Contains(indexes, idx);
    Py_DECREF(idx);
    if (is_root_node || is_in_subgraph) {
      PyList_Append(index_map, PyLong_FromSsize_t(i));
    }
  }

  const Py_ssize_t subset_size = PyList_Size(index_map);
  PyObject *valves_subset = PyList_New(subset_size);
  for (Py_ssize_t i = 0; i < subset_size; ++i) {
    const Py_ssize_t i2 = PyLong_AsSsize_t(PyList_GetItem(index_map, i));
    PyObject *valve = Py_NewRef(PyList_GetItem(valves, i2));
    PyList_SET_ITEM(valves_subset, i, valve);
  }

  PyObject *distances_subset = PyList_New(subset_size * subset_size);
  for (Py_ssize_t i = 0; i < subset_size; ++i) {
    for (Py_ssize_t j = 0; j < subset_size; ++j) {
      const Py_ssize_t i2 = PyLong_AsSsize_t(PyList_GetItem(index_map, i));
      const Py_ssize_t j2 = PyLong_AsSsize_t(PyList_GetItem(index_map, j));
      PyObject *dist = PyList_GetItem(distances, i2 * num_valves + j2);
      PyList_SET_ITEM(distances_subset, i * subset_size + j, Py_NewRef(dist));
    }
  }

  Py_DECREF(root_id);
  Py_DECREF(index_map);

  return PyTuple_Pack(2, valves_subset, distances_subset);
}

PyObject *_AoC_y2022_d16_find_part1(PyObject *valves, PyObject *distances) {
  return _AoC_y2022_d16_find_max_total_pressure(valves, distances, 30);
}

PyObject *_AoC_y2022_d16_subset_magnitude(const Py_ssize_t num_valves,
                                          PyObject *distances,
                                          PyObject *subset_indexes) {
  PyObject *indexes = PySequence_List(subset_indexes);
  PyList_Sort(indexes);
  const Py_ssize_t subset_size = PyList_Size(indexes);
  PyObject *magnitude = PyLong_FromLong(0);
  for (Py_ssize_t i = 0; i < subset_size; ++i) {
    for (Py_ssize_t j = i + 1; j < subset_size; ++j) {
      Py_ssize_t src = PyLong_AsSsize_t(PyList_GetItem(indexes, i));
      Py_ssize_t dst = PyLong_AsSsize_t(PyList_GetItem(indexes, j));
      PyObject *dist = PyList_GetItem(distances, src * num_valves + dst);
      Py_SETREF(magnitude, PyNumber_Add(magnitude, dist));
    }
  }
  Py_DECREF(indexes);
  return magnitude;
}

PyObject *_AoC_y2022_d16_find_part2(PyObject *valves, PyObject *distances) {
  PyObject *max_pressure = PyLong_FromLong(0);

  const Py_ssize_t num_valves = PyList_Size(valves);
  PyObject *partitions = PyList_New(1);
  {
    PyObject *partition1 = PySet_New(0);
    for (Py_ssize_t i = 1; i < num_valves; ++i) {
      PySet_Add(partition1, PyLong_FromSsize_t(i));
    }
    PyObject *partition2 = PySet_New(0);
    PyList_SET_ITEM(partitions, 0, PyTuple_Pack(2, partition1, partition2));
    Py_DECREF(partition1);
    Py_DECREF(partition2);
  }

  PyObject *seen_partitions = PySet_New(0);

  while (PyList_Size(partitions)) {
    PyObject *partition = Py_NewRef(PyList_GetItem(partitions, 0));
    PyObject *partition1 = PyTuple_GetItem(partition, 0);
    PyObject *partition2 = PyTuple_GetItem(partition, 1);
    PySequence_DelItem(partitions, 0);

    PyObject *lhs_magnitude =
        _AoC_y2022_d16_subset_magnitude(num_valves, distances, partition1);
    PyObject *rhs_magnitude =
        _AoC_y2022_d16_subset_magnitude(num_valves, distances, partition2);
    // braindead heuristic without proof
    if (PyLong_AsLong(lhs_magnitude) < 3 * PyLong_AsLong(rhs_magnitude)) {
      PyObject *total_pressure = PyLong_FromLong(0);
      for (size_t p = 0; p < 2; ++p) {
        PyObject *subset_indexes = (PyObject *[]){partition1, partition2}[p];
        PyObject *tmp =
            _AoC_y2022_d16_select_subgraph(valves, distances, subset_indexes);
        PyObject *valves_subset = PyTuple_GetItem(tmp, 0);
        PyObject *distances_subset = PyTuple_GetItem(tmp, 1);
        PyObject *pressure =
            _AoC_y2022_d16_find_max_total_pressure(valves_subset,
                                                   distances_subset,
                                                   26);
        Py_SETREF(total_pressure, PyNumber_Add(total_pressure, pressure));
        Py_DECREF(valves_subset);
        Py_DECREF(distances_subset);
        Py_DECREF(tmp);
        Py_DECREF(pressure);
      }
      if (PyObject_RichCompareBool(total_pressure, max_pressure, Py_GT) == 1) {
        Py_SETREF(max_pressure, total_pressure);
      } else {
        Py_DECREF(total_pressure);
      }
    }
    Py_DECREF(lhs_magnitude);
    Py_DECREF(rhs_magnitude);

    // build all partitions, add to queue if not seen
    PyObject *lhs_iter = PyObject_GetIter(partition1);
    PyObject *lhs_value;
    while ((lhs_value = PyIter_Next(lhs_iter))) {
      PyObject *lhs = PySet_New(partition1);
      PyObject *rhs = PySet_New(partition2);
      PySet_Discard(lhs, lhs_value);
      PySet_Add(rhs, lhs_value);

      const Py_ssize_t lhs_size = PySet_Size(lhs);
      const Py_ssize_t rhs_size = PySet_Size(rhs);
      if (lhs_size < rhs_size) {
        goto end;
      }

      PyObject *lhs_key = PySequence_List(lhs);
      PyObject *rhs_key = PySequence_List(rhs);
      PyList_Sort(lhs_key);
      PyList_Sort(rhs_key);
      Py_SETREF(lhs_key, PySequence_Tuple(lhs_key));
      Py_SETREF(rhs_key, PySequence_Tuple(rhs_key));
      const int skip = PySet_Contains(seen_partitions, lhs_key) ||
                       PySet_Contains(seen_partitions, rhs_key);
      PySet_Add(seen_partitions, lhs_key);
      PySet_Add(seen_partitions, rhs_key);
      Py_DECREF(lhs_key);
      Py_DECREF(rhs_key);
      if (!skip) {
        PyObject *new_partition = PyTuple_Pack(2, lhs, rhs);
        PyList_Append(partitions, new_partition);
        Py_DECREF(new_partition);
      }
    end:
      Py_DECREF(lhs);
      Py_DECREF(rhs);
      Py_DECREF(lhs_value);
    }

    Py_DECREF(lhs_iter);
    Py_DECREF(partition);
  }
  Py_DECREF(seen_partitions);

  return max_pressure;
}

PyObject *AoC_y2022_d16(PyObject *unicode_input) {
  PyObject *solution = 0;
  PyObject *valves = 0;
  PyObject *distances = 0;
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  if (!lines) {
    goto done;
  }

  valves = _AoC_y2022_d16_parse_valves(lines);
  if (!valves) {
    goto done;
  }
  distances = _AoC_y2022_d16_all_pairs_shortest_paths(valves);
  if (!distances) {
    goto done;
  }

  PyObject *nonzero_valve_indexes = PySet_New(0);
  for (Py_ssize_t i = 0; i < PyList_Size(valves); ++i) {
    PyObject *flow = PyTuple_GetItem(PyList_GetItem(valves, i), 1);
    if (PyLong_AsLong(flow)) {
      PySet_Add(nonzero_valve_indexes, PyLong_FromSsize_t(i));
    }
  }

  PyObject *tmp =
      _AoC_y2022_d16_select_subgraph(valves, distances, nonzero_valve_indexes);
  Py_SETREF(valves, PyTuple_GetItem(tmp, 0));
  Py_SETREF(distances, PyTuple_GetItem(tmp, 1));
  Py_DECREF(tmp);
  Py_DECREF(nonzero_valve_indexes);

  PyObject *part1 = _AoC_y2022_d16_find_part1(valves, distances);
  PyObject *part2 = _AoC_y2022_d16_find_part2(valves, distances);
  solution = PyUnicode_FromFormat("%S %S", part1, part2);
  Py_DECREF(part1);
  Py_DECREF(part2);

done:
  Py_XDECREF(distances);
  Py_XDECREF(valves);
  Py_XDECREF(lines);
  return solution;
}

#endif  // _AOC_Y2022_D16_H_INCLUDED
