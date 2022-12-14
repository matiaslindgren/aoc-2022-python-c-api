#ifndef _AOC_Y2022_D13_H_INCLUDED
#define _AOC_Y2022_D13_H_INCLUDED
#include <structmember.h>

#include "common.h"

typedef struct {
  PyObject_HEAD PyObject *packet;
} AoC_y2022_d13_PacketObject;

static void AoC_y2022_d13_Packet_dealloc(AoC_y2022_d13_PacketObject *self) {
  Py_XDECREF(self->packet);
  Py_TYPE(self)->tp_free((PyObject *)self);
}

int _AoC_y2022_d13_packet_compare(PyObject *lhs, PyObject *rhs);

PyObject *AoC_y2022_d13_Packet_compare(PyObject *lhs, PyObject *rhs, int op) {
  PyObject *lhs_packet = ((AoC_y2022_d13_PacketObject *)lhs)->packet;
  PyObject *rhs_packet = ((AoC_y2022_d13_PacketObject *)rhs)->packet;
  switch (op) {
    case Py_LT: {
      if (_AoC_y2022_d13_packet_compare(lhs_packet, rhs_packet) < 0) {
        Py_RETURN_TRUE;
      }
      Py_RETURN_FALSE;
    } break;
    case Py_EQ: {
      return PyUnicode_RichCompare(lhs_packet, rhs_packet, Py_EQ);
    } break;
  }
  Py_RETURN_NOTIMPLEMENTED;
}

static PyTypeObject AoC_y2022_d13_PacketType = {
    PyVarObject_HEAD_INIT(0, 0).tp_name = "y2022_d13_Packet",
    .tp_basicsize = sizeof(AoC_y2022_d13_PacketObject),
    .tp_new = PyType_GenericNew,
    .tp_dealloc = (destructor)(AoC_y2022_d13_Packet_dealloc),
    .tp_richcompare = (richcmpfunc)(AoC_y2022_d13_Packet_compare),
};

static PyObject *AoC_y2022_d13_Packet_FromUnicodeObject(PyObject *packet_str) {
  PyObject *packet = PyType_GenericNew(&AoC_y2022_d13_PacketType, 0, 0);
  ((AoC_y2022_d13_PacketObject *)packet)->packet = Py_NewRef(packet_str);
  return packet;
}

int _AoC_y2022_d13_packet_is_numeric(PyObject *packet) {
  for (Py_ssize_t i = 0; i < PyUnicode_GetLength(packet); ++i) {
    switch (PyUnicode_READ_CHAR(packet, i)) {
      case '[':
      case ']':
      case ',': {
        return 0;
      }
    }
  }
  return 1;
}

PyObject *_AoC_y2022_d13_split_packets(PyObject *packet) {
  PyObject *packets = PyList_New(0);
  if (_AoC_y2022_d13_packet_is_numeric(packet)) {
    PyList_Append(packets, packet);
    return packets;
  }
  long depth = 0;
  Py_ssize_t subpacket_len = 0;
  Py_ssize_t i = 1;
  for (; i < PyUnicode_GetLength(packet) - 1; ++i) {
    Py_UCS4 ch = PyUnicode_READ_CHAR(packet, i);
    depth += ch == '[';
    depth -= ch == ']';
    if (ch == ',' && depth == 0) {
      PyObject *subpacket = PyUnicode_Substring(packet, i - subpacket_len, i);
      PyList_Append(packets, subpacket);
      subpacket_len = 0;
    } else {
      ++subpacket_len;
    }
  }
  if (subpacket_len) {
    PyObject *subpacket = PyUnicode_Substring(packet, i - subpacket_len, i);
    PyList_Append(packets, subpacket);
  }
  return packets;
}

int _AoC_y2022_d13_packet_compare(PyObject *lhs, PyObject *rhs) {
  if (_AoC_y2022_d13_packet_is_numeric(lhs) &&
      _AoC_y2022_d13_packet_is_numeric(rhs)) {
    lhs = PyLong_FromUnicodeObject(lhs, 10);
    rhs = PyLong_FromUnicodeObject(rhs, 10);
    int lhs_less = PyObject_RichCompareBool(lhs, rhs, Py_LT) == 1;
    int rhs_less = PyObject_RichCompareBool(lhs, rhs, Py_GT) == 1;
    Py_DECREF(lhs);
    Py_DECREF(rhs);
    if (lhs_less) {
      return -1;
    }
    if (rhs_less) {
      return 1;
    }
    return 0;
  }
  PyObject *lhs_packets = _AoC_y2022_d13_split_packets(lhs);
  PyObject *rhs_packets = _AoC_y2022_d13_split_packets(rhs);
  Py_ssize_t lhs_len = PyList_Size(lhs_packets);
  Py_ssize_t rhs_len = PyList_Size(rhs_packets);
  for (Py_ssize_t pos = 0; pos < lhs_len && pos < rhs_len; ++pos) {
    PyObject *lhs_packet = PyList_GetItem(lhs_packets, pos);
    PyObject *rhs_packet = PyList_GetItem(rhs_packets, pos);
    int comp = _AoC_y2022_d13_packet_compare(lhs_packet, rhs_packet);
    if (comp) {
      Py_DECREF(lhs_packets);
      Py_DECREF(rhs_packets);
      return comp;
    }
  }
  Py_DECREF(lhs_packets);
  Py_DECREF(rhs_packets);
  if (lhs_len < rhs_len) {
    return -1;
  }
  if (lhs_len > rhs_len) {
    return 1;
  }
  return 0;
}

PyObject *AoC_y2022_d13(PyObject *unicode_input) {
  PyObject *solution = 0;
  PyObject *packets = PyList_New(0);
  PyObject *lines = PyUnicode_Splitlines(unicode_input, 0);
  if (!lines || !packets) {
    goto done;
  }

  for (Py_ssize_t i = 0; i < PyList_Size(lines); ++i) {
    PyObject *line = PyList_GetItem(lines, i);
    if (PyUnicode_GetLength(line) > 0) {
      PyObject *packet = AoC_y2022_d13_Packet_FromUnicodeObject(line);
      if (PyList_Append(packets, packet) < 0) {
        goto done;
      }
    }
  }

  long part1 = 0;
  for (Py_ssize_t i = 0; i < PyList_Size(packets); i += 2) {
    PyObject *lhs = PyList_GetItem(packets, i);
    PyObject *rhs = PyList_GetItem(packets, i + 1);
    if (PyObject_RichCompareBool(lhs, rhs, Py_LT) == 1) {
      part1 += i / 2 + 1;
    }
  }

  PyObject *dividers[] = {
      AoC_y2022_d13_Packet_FromUnicodeObject(PyUnicode_FromFormat("[[2]]")),
      AoC_y2022_d13_Packet_FromUnicodeObject(PyUnicode_FromFormat("[[6]]")),
  };
  for (size_t i = 0; i < 2; ++i) {
    if (PyList_Append(packets, dividers[i]) < 0) {
      goto done;
    }
  }
  if (PyList_Sort(packets) < 0) {
    goto done;
  }
  long part2 = 1;
  for (Py_ssize_t i = 0; i < PyList_Size(packets); ++i) {
    PyObject *packet = PyList_GetItem(packets, i);
    for (size_t j = 0; j < 2; ++j) {
      if (PyObject_RichCompareBool(packet, dividers[j], Py_EQ) == 1) {
        part2 *= (i + 1);
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
  Py_XDECREF(packets);
  return solution;
}

#endif  // _AOC_Y2022_D13_H_INCLUDED
