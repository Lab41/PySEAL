This is an unofficial fork of the Microsoft SEAL homomorphic encryption library demonstrating a python wrapper proof of concept that is dockerized

Original home: https://www.microsoft.com/en-us/research/project/simple-encrypted-arithmetic-library-seal-2/

This repository is a proof of concept for adding a Python wrapper to the (Simple Encrypted Arithmetic Library (SEAL))[https://sealcrypto.org/], a homomorphic encryption library, developed by researchers in the Cryptography Research Group at Microsoft Research. Currently, this uses the version 2.2 codebase and dockerizes the library build (including a shared runtime), C++ example build, and Python wrapper build. The Python wrapper is using pybind11.
