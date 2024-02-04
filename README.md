# Advent of Code 2022 - Python C API

Silly little Rube Goldberg machine.
Nothing intelligent to see here.

## Requirements

- clang
- GNU make
- python3 development headers (3.10, 3.11, or 3.12), e.g. python3-dev on Ubuntu and Debian or python312-devel on openSUSE.

## Usage

1. Build solutions
    ```
    make py_aoc
    ```
2. Put inputs under `./txt/input/2022/??` and solve (for example day 1) using:
    ```
    make run_d01
    ```

## (optional) Using Docker

```
./run_in_docker.bash
```

## Explanation

`py_aoc` embeds the Python interpreter and runs its [entrypoint](./include/solutions_module.h#L237):
```
> ./out/py_aoc
Python 3.11.0 (main, Dec 15 2022, 19:30:38) [Clang 11.0.1 ] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> print("hi")
hi
```

In addition to being a normal Python interpreter, `py_aoc` defines a Python builtin-module named [`aoc_solve`](./include/solutions_module.h#L157-L163) containing a single function named [`y2022`](./include/solutions_module.h#L141-L146).
The function takes a single integer argument between 1 to 25 for the Advent of Code 2022 challenge to solve.

For example, solving day 2:
```
> ./out/py_aoc -c "from aoc_solve import y2022; print(y2022(2))"
0.4 ms
15632 14416
```

Steps in more detail:
1. [Read](./include/solutions_module.h#L109) the entire [UTF-8 input file](./txt/input/2022/02) into a single Python `str`.
2. Pass input `str` as argument to the [solver function](./include/solutions_module.h#L37:L38).
3. [Compute](./include/d02.h#L24-L56) numerical solutions to part 1 and 2, then [format](./include/d02.h#L58) the numbers into a single space-separated `str`.
4. Cleanup refcounts, return the solution, and print to stdout.

![](./haha.png)
