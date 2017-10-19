#include <pybind11/pybind11.h>
#include "encryptionparams.h"

namespace py = pybind11;

using namespace pybind11::literals;
using namespace seal;
using namespace std;

// http://pybind11.readthedocs.io/en/stable/classes.html

PYBIND11_MODULE(seal, m) {
    py::class_<EncryptionParameters>(m, "EncryptionParameters")
        .def(py::init<>())
        .def(py::init<const MemoryPoolHandle &>())
        .def("set_poly_modulus",
          (void (EncryptionParameters::*)(const std::string &)) &EncryptionParameters::set_poly_modulus,
          "Set poly modulus");
}
