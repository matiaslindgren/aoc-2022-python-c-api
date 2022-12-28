![](./haha.png)

# Advent of Code 2022 - Python C API

Silly little Rube Goldberg machine.
Nothing intelligent to see here.

## Requirements

- clang
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
    make run_d01
    ```

## Explanation

`py_aoc` is a program that runs the Python interpreter [entrypoint](./include/solutions_module.h#L231) by [linking](./Makefile#L26) to the [compiled](./Makefile#L53-L57) `libpython3` library:
```
> ./out/py_aoc
Python 3.11.0 (main, Dec 15 2022, 19:30:38) [Clang 11.0.1 ] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> print("hi")
hi
```

`py_aoc` defines a Python builtin-module named [`aoc_solve`](./include/solutions_module.h#L151-L157) with a single function named [`y2022`](./include/solutions_module.h#L135-L140).
The function takes a single integer argument between 1 to 25 for the Advent of Code 2022 challenge to solve.

For example, solving day 2:
```
> ./out/py_aoc -c "from aoc_solve import y2022; print(y2022(2))"
0.4 ms
15632 14416
```

Steps in more detail:
1. [Read](./include/solutions_module.h#L103) [UTF-8 input](./txt/input/01.txt) into a single Python `str`.
2. Pass input `str` as argument to the [solver function](./include/solutions_module.h#L35:L36).
3. [Compute](./include/d02.h#L9) numerical solutions to part 1 and 2, then [format](./include/d02.h#L58) the numbers into a single space-separated `str`.
4. Cleanup refcounts, return the solution, and print to stdout.
