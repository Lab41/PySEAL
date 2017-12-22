This repository is a proof of concept for adding a Python wrapper to the
(Simple Encrypted Arithmetic Library (SEAL))[http://sealcrypto.org/], a homomorphic encryption library,
developed by researchers in the Cryptography Research Group at Microsoft Research. Currently, this uses
the version 2.3 codebase and dockerizes the library build (including a shared runtime), C++ example build,
and Python wrapper build. The Python wrapper is using pybind11.

To build the wrapped Python version of SEAL, first run the executable build-docker.sh. This creates a seal
package that can be imported in Python; to see examples of how to use this package in cryptography tasks,
run the executable run-docker.sh (which runs the examples implemented in SEALPythonExamples/examples.py).

Original home: https://www.microsoft.com/en-us/research/project/simple-encrypted-arithmetic-library/
