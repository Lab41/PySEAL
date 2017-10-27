#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "bigpoly.h"
#include "bigpolyarray.h"
#include "biguint.h"
#include "chooser.h"
#include "ciphertext.h"
#include "decryptor.h"
#include "encoder.h"
#include "encryptor.h"
#include "encryptionparams.h"
#include "evaluator.h"
#include "keygenerator.h"
#include "memorypoolhandle.h"
#include "plaintext.h"

namespace py = pybind11;

using namespace pybind11::literals;
using namespace seal;
using namespace std;

// http://pybind11.readthedocs.io/en/stable/classes.html

PYBIND11_MODULE(seal, m) {

  py::class_<BigPoly>(m, "BigPoly")
    .def(py::init<>())
    .def("to_string", &BigPoly::to_string,
         "Returns a human-readable string description of the BigPoly");

  py::class_<BigPolyArray>(m, "BigPolyArray")
    .def(py::init<>());

  py::class_<BigUInt>(m, "BigUInt")
    .def(py::init<>())
    .def("to_double", &BigUInt::to_double,
          "Returns the BigUInt value as a double. Note that precision may be lost during the conversion.");

  py::class_<ChooserEvaluator>(m, "ChooserEvaluator")
    .def_static("default_parameter_options", &ChooserEvaluator::default_parameter_options,
               "Retrieve default parameter options");

  py::class_<Ciphertext>(m, "Ciphertext")
    .def(py::init<const BigPolyArray &>());

  py::class_<Decryptor>(m, "Decryptor")
    .def(py::init<const EncryptionParameters &, const BigPoly &, const MemoryPoolHandle &>())
    .def("decrypt", (BigPoly (Decryptor::*)(const BigPolyArray &)) &Decryptor::decrypt,
        "Decrypts a ciphertext and returns the result")
    .def("invariant_noise_budget", &Decryptor::invariant_noise_budget, "Returns noise budget");

  py::class_<Encryptor>(m, "Encryptor")
    .def(py::init<const EncryptionParameters &, const BigPolyArray &, const MemoryPoolHandle &>())
    .def("encrypt", (BigPolyArray (Encryptor::*)(const BigPoly &)) &Encryptor::encrypt,
        "Encrypts a plaintext and returns the result");

  py::class_<EncryptionParameters>(m, "EncryptionParameters")
    .def(py::init<>())
    .def(py::init<const MemoryPoolHandle &>())
    .def("plain_modulus", &EncryptionParameters::plain_modulus, "Returns the plaintext modulus")
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

  py::class_<Evaluator>(m, "Evaluator")
    .def(py::init<const EncryptionParameters &, const MemoryPoolHandle &>())
    .def("negate", (BigPolyArray (Evaluator::*)(const BigPolyArray &)) &Evaluator::negate,
        "Negates a ciphertext and returns the result")
    .def("add", (BigPolyArray (Evaluator::*)(const BigPolyArray &, const BigPolyArray &)) &Evaluator::add,
        "Adds two ciphertexts and returns the result")
    .def("sub", (BigPolyArray (Evaluator::*)(const BigPolyArray &, const BigPolyArray &)) &Evaluator::sub,
        "Subtracts two ciphertexts and returns the result")
    .def("multiply",
        (BigPolyArray (Evaluator::*)(const BigPolyArray &, const BigPolyArray &)) &Evaluator::multiply,
        "Multiplies two ciphertexts without performing relinearization, and returns the result");

  py::class_<IntegerEncoder>(m, "IntegerEncoder")
    .def(py::init<const BigUInt &, std::uint64_t, const MemoryPoolHandle &>())
    .def("encode", (BigPoly (IntegerEncoder::*)(std::uint64_t)) &IntegerEncoder::encode, "Encode integer")
    .def("encode", (BigPoly (IntegerEncoder::*)(std::int64_t)) &IntegerEncoder::encode, "Encode integer")
    .def("decode_int32", &IntegerEncoder::decode_int32,
        "Decodes a plaintext polynomial and returns the result as std::uint32_t");

  py::class_<KeyGenerator>(m, "KeyGenerator")
    .def(py::init<const EncryptionParameters &, const MemoryPoolHandle &>())
    .def("generate", &KeyGenerator::generate, "Generates keys")
    .def("public_key", &KeyGenerator::public_key, "Returns public key")
    .def("secret_key", &KeyGenerator::secret_key, "Returns secret key");

  py::class_<MemoryPoolHandle>(m, "MemoryPoolHandle")
    .def(py::init<>())
    .def_static("acquire_global", &MemoryPoolHandle::acquire_global,
               "Returns a MemoryPoolHandle pointing to the global memory pool");

  py::class_<Plaintext>(m, "Plaintext")
     .def(py::init<>())
     .def(py::init<const BigPoly &>())
     .def("to_string", &Plaintext::to_string, "Returns the plaintext as a formated string");
}
