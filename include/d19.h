#ifndef _AOC_Y2022_D19_H_INCLUDED
#define _AOC_Y2022_D19_H_INCLUDED
#include "common.h"

PyObject *_AoC_y2022_d19_parse_blueprints(PyObject *lines) {
  const Py_ssize_t num_blueprints = PyList_Size(lines);
  PyObject *blueprints = PyList_New(num_blueprints);
  if (!blueprints) {
    return 0;
  }
  const Py_ssize_t cost_indexes[] = {
      6,
      12,
      18,
      21,
      27,
      30,
  };
  const size_t num_indexes = sizeof(cost_indexes) / sizeof(cost_indexes[0]);
  for (Py_ssize_t i = 0; i < num_blueprints; ++i) {
    PyObject *line = PyList_GetItem(lines, i);
    PyObject *parts = PyUnicode_Split(line, 0, -1);
    PyObject *blueprint = PyTuple_New(num_indexes);
    for (Py_ssize_t j = 0; j < PyTuple_Size(blueprint); ++j) {
      PyObject *cost = PyList_GetItem(parts, cost_indexes[j]);
      PyTuple_SET_ITEM(blueprint, j, PyLong_FromUnicodeObject(cost, 10));
    }
    PyList_SET_ITEM(blueprints, i, blueprint);
  }
  return blueprints;
}

long _AoC_y2022_d19_find_max_geodes(long *best,
                                    PyObject *cache,
                                    PyObject *blueprint,
                                    const long time_left,
                                    const long ore,
                                    const long cla,
                                    const long obs,
                                    const long geo,
                                    const long ore_prod,
                                    const long cla_prod,
                                    const long obs_prod,
                                    const long geo_prod) {
  long ret = 0;

  PyObject *cache_key = PyTuple_New(9);
  for (Py_ssize_t i = 0; i < PyTuple_Size(cache_key); ++i) {
    const long value = (const long[]){
        time_left,
        ore,
        cla,
        obs,
        geo,
        ore_prod,
        cla_prod,
        obs_prod,
        geo_prod,
    }[i];
    PyTuple_SET_ITEM(cache_key, i, PyLong_FromLong(value));
  }

  PyObject *cached_value = PyDict_GetItem(cache, cache_key);
  if (cached_value) {
    ret = PyLong_AsLong(cached_value);
    goto end;
  }

  if (time_left <= 0) {
    ret = geo;
    goto end;
  }
  if (time_left <= 1 && !geo_prod) {
    goto end;
  }
  if (time_left <= 2 && !obs_prod) {
    goto end;
  }
  {
    const long max_geo_prod = (time_left - 1) * time_left / 2;
    if (geo + time_left * geo_prod + max_geo_prod < *best) {
      goto end;
    }
  }

  const long ore_ore_cost = PyLong_AsLong(PyTuple_GetItem(blueprint, 0));
  const long cla_ore_cost = PyLong_AsLong(PyTuple_GetItem(blueprint, 1));
  const long obs_ore_cost = PyLong_AsLong(PyTuple_GetItem(blueprint, 2));
  const long obs_cla_cost = PyLong_AsLong(PyTuple_GetItem(blueprint, 3));
  const long geo_ore_cost = PyLong_AsLong(PyTuple_GetItem(blueprint, 4));
  const long geo_obs_cost = PyLong_AsLong(PyTuple_GetItem(blueprint, 5));

  const int can_build_geo_robot = ore >= geo_ore_cost && obs >= geo_obs_cost;
  if (time_left > 1 && can_build_geo_robot) {
    long max_geo_after_more_geo_prod =
        _AoC_y2022_d19_find_max_geodes(best,
                                       cache,
                                       blueprint,
                                       time_left - 1,
                                       ore + ore_prod - geo_ore_cost,
                                       cla + cla_prod,
                                       obs + obs_prod - geo_obs_cost,
                                       geo + geo_prod,
                                       ore_prod,
                                       cla_prod,
                                       obs_prod,
                                       geo_prod + 1);
    ret = Py_MAX(ret, max_geo_after_more_geo_prod);
    goto end;
  }

  const int can_build_obs_robot = ore >= obs_ore_cost && cla >= obs_cla_cost;
  const int excessive_obs_prod = obs_prod > geo_obs_cost;
  if (time_left > 2 && can_build_obs_robot && !excessive_obs_prod) {
    long max_geo_after_more_obs_prod =
        _AoC_y2022_d19_find_max_geodes(best,
                                       cache,
                                       blueprint,
                                       time_left - 1,
                                       ore + ore_prod - obs_ore_cost,
                                       cla + cla_prod - obs_cla_cost,
                                       obs + obs_prod,
                                       geo + geo_prod,
                                       ore_prod,
                                       cla_prod,
                                       obs_prod + 1,
                                       geo_prod);
    ret = Py_MAX(ret, max_geo_after_more_obs_prod);
  }

  const int can_build_cla_robot = ore >= cla_ore_cost;
  const int excessive_cla_prod = cla_prod > obs_cla_cost;
  if (time_left > 2 && can_build_cla_robot && !excessive_cla_prod) {
    long max_geo_after_more_cla_prod =
        _AoC_y2022_d19_find_max_geodes(best,
                                       cache,
                                       blueprint,
                                       time_left - 1,
                                       ore + ore_prod - cla_ore_cost,
                                       cla + cla_prod,
                                       obs + obs_prod,
                                       geo + geo_prod,
                                       ore_prod,
                                       cla_prod + 1,
                                       obs_prod,
                                       geo_prod);
    ret = Py_MAX(ret, max_geo_after_more_cla_prod);
  }

  const int can_build_ore_robot = ore >= ore_ore_cost;
  const int excessive_ore_prod =
      ore_prod >
      Py_MAX(Py_MAX(Py_MAX(ore_ore_cost, cla_ore_cost), obs_ore_cost),
             geo_ore_cost);
  if (time_left > 2 && can_build_ore_robot && !excessive_ore_prod) {
    long max_geo_after_more_ore_prod =
        _AoC_y2022_d19_find_max_geodes(best,
                                       cache,
                                       blueprint,
                                       time_left - 1,
                                       ore + ore_prod - ore_ore_cost,
                                       cla + cla_prod,
                                       obs + obs_prod,
                                       geo + geo_prod,
                                       ore_prod + 1,
                                       cla_prod,
                                       obs_prod,
                                       geo_prod);
    ret = Py_MAX(ret, max_geo_after_more_ore_prod);
  }

  long max_geo_after_no_builds = _AoC_y2022_d19_find_max_geodes(best,
                                                                cache,
                                                                blueprint,
                                                                time_left - 1,
                                                                ore + ore_prod,
                                                                cla + cla_prod,
                                                                obs + obs_prod,
                                                                geo + geo_prod,
                                                                ore_prod,
                                                                cla_prod,
                                                                obs_prod,
                                                                geo_prod);
  ret = Py_MAX(ret, max_geo_after_no_builds);

end:
  PyDict_SetItem(cache, cache_key, PyLong_FromLong(ret));
  Py_DECREF(cache_key);
  *best = Py_MAX(*best, ret);
  return ret;
}

long _AoC_y2022_d19_find_max_geodes_recur(PyObject *blueprint,
                                          const long time_limit) {
  PyObject *cache = PyDict_New();
  long max_geodes = 0;
  max_geodes = _AoC_y2022_d19_find_max_geodes(&max_geodes,
                                              cache,
                                              blueprint,
                                              time_limit,
                                              0,
                                              0,
                                              0,
                                              0,
                                              1,
                                              0,
                                              0,
                                              0);
  Py_DECREF(cache);
  return max_geodes;
}

PyObject *AoC_y2022_d19(PyObject *unicode_input) {
  PyObject *solution = 0;
  PyObject *blueprints = 0;
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  if (!lines) {
    goto done;
  }

  blueprints = _AoC_y2022_d19_parse_blueprints(lines);
  if (!blueprints) {
    goto done;
  }

  long part1 = 0;
  long part2 = 1;
  for (Py_ssize_t i = 0; i < PyList_Size(blueprints); ++i) {
    PyObject *blueprint = PyList_GetItem(blueprints, i);
    const long id = (long)i + 1;
    part1 += id * _AoC_y2022_d19_find_max_geodes_recur(blueprint, 24);
    if (i < 3) {
      part2 *= _AoC_y2022_d19_find_max_geodes_recur(blueprint, 32);
    }
  }
  solution = PyUnicode_FromFormat("%ld %ld", part1, part2);

done:
  Py_XDECREF(lines);
  Py_XDECREF(blueprints);
  return solution;
}

#endif  // _AOC_Y2022_D19_H_INCLUDED
