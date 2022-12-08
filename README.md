![](./haha.png)

# Advent of Code 2022 - Python C API

Silly little Rube Goldberg machine.
Nothing intelligent to see here.

## Usage

1. Download and build Python from source:
    ```
    make python
    ```
2. Compile solutions and link compiled Python:
    ```
    make py_aoc
    ```
3. Put inputs under `./txt/inputs/??.txt` and solve (for example day 1) using:
    ```
    ./solve 1
    ```

## Explain

`py_aoc` is a program that runs the regular Python interpreter [entrypoint](./py_aoc.c#L105) by linking to the compiled `libpython3` library (should appear under `./python/lib` after running `make python`):
```
> ./out/py_aoc
Python 3.11.0 (main, Dec  5 2022, 23:22:09) [GCC 10.2.1 20210110] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> print("hi")
hi
```

However, it also contains a builtin-module `aoc_solve` with a single function: `y2022`.
This function takes a single integer argument: the day to solve.

For example, solving day 1:
```
> ./out/py_aoc -c "from aoc_solve import y2022; print(y2022(1))"
68442 204837
```

Steps in more detail:
1. [Read](./py_aoc.c#L46:54) UTF-8 input from [](./txt/input/01.txt) into a single `str`.
2. Call [the function](./py_aoc.c#L15:L16) for solving day 1, included from [](./include/d01.h).
3. Compute numerical solutions to part 1 and 2, then [format](./include/d01.h#L53) them as a space separated `str`.
4. Return the solution and print to stdout.
