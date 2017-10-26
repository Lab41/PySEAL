#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "biguint.h"
#include "chooser.h"
#include "encryptionparams.h"

namespace py = pybind11;

using namespace pybind11::literals;
using namespace seal;
using namespace std;

// http://pybind11.readthedocs.io/en/stable/classes.html

PYBIND11_MODULE(seal, m) {

  py::class_<BigUInt>(m, "BigUInt")
    .def(py::init<>())
    .def("to_double", &BigUInt::to_double,
          "Returns the BigUInt value as a double. Note that precision may be lost during the conversion.");

  py::class_<ChooserEvaluator>(m, "ChooserEvaluator")
    .def_static("default_parameter_options", &ChooserEvaluator::default_parameter_options,
               "Retrieve default parameter options");

  py::class_<EncryptionParameters>(m, "EncryptionParameters")
    .def(py::init<>())
    .def(py::init<const MemoryPoolHandle &>())
    .def("set_coeff_modulus",
        (void (EncryptionParameters::*)(const BigUInt &)) &EncryptionParameters::set_coeff_modulus,
        "Set coefficient modulus parameter")
    .def("set_coeff_modulus",
        (void (EncryptionParameters::*)(std::uint64_t)) &EncryptionParameters::set_coeff_modulus,
        "Set coefficient modulus parameter")
    .def("set_coeff_modulus",
        (void (EncryptionParameters::*)(const std::string &)) &EncryptionParameters::set_coeff_modulus,
        "Set coefficient modulus parameter")
    .def("set_plain_modulus",
        (void (EncryptionParameters::*)(const BigUInt &)) &EncryptionParameters::set_plain_modulus,
        "Set plaintext modulus parameter")
    .def("set_plain_modulus",
        (void (EncryptionParameters::*)(std::uint64_t)) &EncryptionParameters::set_plain_modulus,
        "Set plaintext modulus parameter")
    .def("set_plain_modulus",
        (void (EncryptionParameters::*)(const std::string &)) &EncryptionParameters::set_plain_modulus,
        "Set plaintext modulus parameter")
    .def("set_poly_modulus",
        (void (EncryptionParameters::*)(const std::string &)) &EncryptionParameters::set_poly_modulus,
        "Set polynomial modulus parameter")
    .def("validate", &EncryptionParameters::validate,
        "Validates parameters");

  py::class_<EncryptionParameterQualifiers>(m, "EncryptionParameterQuailifers");
}
