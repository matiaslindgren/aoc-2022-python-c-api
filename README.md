![](./haha.png)

# Advent of Code 2022 - Python C API

Silly little Rube Goldberg machine.
Nothing intelligent to see here.

## Requirements

- clang
- gcc
- make
- curl
- tar

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

`py_aoc` is a program that runs the Python interpreter [entrypoint](./src/py_aoc.c#L105) by [linking](./Makefile#L21) to the [compiled](./Makefile#L48-L50) `libpython3` library:
```
> ./out/py_aoc
Python 3.11.0 (main, Dec  5 2022, 23:22:09) [GCC 10.2.1 20210110] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> print("hi")
hi
```

`py_aoc` also contains a [builtin-module `aoc_solve`](./src/py_aoc.c#L88-L90) with a single function named [`y2022`](./src/py_aoc.c#L72-L77), which accepts the day to solve as the only argument.

For example, solving day 1:
```
> ./out/py_aoc -c "from aoc_solve import y2022; print(y2022(1))"
68442 204837
```

Steps in more detail:
1. [Read](./include/common.h#L35) [UTF-8 input](./txt/input/01.txt) into a [single `str`](./src/py_aoc.c#L46:L50).
2. Call [the solver of day 1](./src/py_aoc.c#L15:L16) by passing the input as a string argument.
3. [Compute](./include/d01.h#L5) numerical solutions to part 1 and 2, then [format](./include/d01.h#L53) them into a single space-separated `str`.
4. Cleanup [refcounts](https://docs.python.org/3/c-api/intro.html#reference-counts), return the solution and print to stdout.
