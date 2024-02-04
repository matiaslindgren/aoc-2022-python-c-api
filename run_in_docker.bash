#!/usr/bin/env bash
set -ue

workdir=/aoc
image=aoc:1

docker build --tag $image .
docker run \
  --workdir $workdir \
  -v $(pwd)/src:${workdir}/src \
  -v $(pwd)/include:${workdir}/include \
  -v $(pwd)/txt:${workdir}/txt \
  -v $(pwd)/Makefile:${workdir}/Makefile \
  -v $(pwd)/test:${workdir}/test \
  --pull never \
  --rm \
  -it \
  $image \
  bash
