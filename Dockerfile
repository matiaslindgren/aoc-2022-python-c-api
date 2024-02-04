FROM ubuntu:22.04
RUN apt update --yes && apt install --yes make clang llvm python3-dev
