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

`py_aoc` is a regular Python interpreter with a builtin-function `aoc_solve.y2022` that works as an entrypoint for solving a particular AoC day.

For example, solving day 4:
```
> ./out/py_aoc
Python 3.11.0 (main, Dec  5 2022, 23:22:09) [GCC 10.2.1 20210110] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> from aoc_solve import y2022
>>> y2022(4)
'456 808'
```
