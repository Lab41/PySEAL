#include <msclr\marshal_cppstd.h>
#include <cstddef>
#include "EncryptionParamsWrapper.h"
#include "Common.h"
#include "BigPolyWrapper.h"
#include "BigUIntWrapper.h"

using namespace System;
using namespace System::IO;
using namespace msclr::interop;
using namespace std;

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            EncryptionParameterQualifiers::EncryptionParameterQualifiers(const seal::EncryptionParameterQualifiers &qualifiers) : qualifiers_(nullptr)
            {
                try
                {
                    qualifiers_ = new seal::EncryptionParameterQualifiers(qualifiers);
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
            }

            EncryptionParameterQualifiers::~EncryptionParameterQualifiers()
            {
                this->!EncryptionParameterQualifiers();
            }

            EncryptionParameterQualifiers::!EncryptionParameterQualifiers()
            {
                if (qualifiers_ != nullptr)
                {
                    delete qualifiers_;
                    qualifiers_ = nullptr;
                }
            }

            seal::EncryptionParameterQualifiers &EncryptionParameterQualifiers::GetQualifiers()
            {
                if (qualifiers_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameterQualifiers is disposed");
                }
                return *qualifiers_;
            }

            bool EncryptionParameterQualifiers::ParametersSet::get()
            {
                if (qualifiers_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameterQualifiers is disposed");
                }
                return qualifiers_->parameters_set;
            }

            bool EncryptionParameterQualifiers::EnableRelinearization::get()
            {
                if (qualifiers_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameterQualifiers is disposed");
                }
                return qualifiers_->enable_relinearization;
            }

            bool EncryptionParameterQualifiers::EnableNussbaumer::get()
            {
                if (qualifiers_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameterQualifiers is disposed");
                }
                return qualifiers_->enable_nussbaumer;
            }

            bool EncryptionParameterQualifiers::EnableNTT::get()
            {
                if (qualifiers_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameterQualifiers is disposed");
                }
                return qualifiers_->enable_ntt;
            }

            bool EncryptionParameterQualifiers::EnableBatching::get()
            {
                if (qualifiers_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameterQualifiers is disposed");
                }
                return qualifiers_->enable_batching;
            }

            bool EncryptionParameterQualifiers::EnableNTTInMultiply::get()
            {
                if (qualifiers_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameterQualifiers is disposed");
                }
                return qualifiers_->enable_ntt_in_multiply;
            }

            EncryptionParameters::EncryptionParameters() : parms_(nullptr)
            {
                try
                {
                    parms_ = new seal::EncryptionParameters();
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
            }

            EncryptionParameters::EncryptionParameters(MemoryPoolHandle ^pool) : parms_(nullptr)
            {
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    parms_ = new seal::EncryptionParameters(pool->GetHandle());
                    GC::KeepAlive(pool);
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
            }

            EncryptionParameters::EncryptionParameters(EncryptionParameters ^copy) : parms_(nullptr)
            {
                if (copy == nullptr)
                {
                    throw gcnew ArgumentNullException("copy cannot be null");
                }
                try
                {
                    parms_ = new seal::EncryptionParameters(copy->GetParameters());
                    GC::KeepAlive(copy);
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
            }

            void EncryptionParameters::Set(EncryptionParameters ^assign)
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                if (assign == nullptr)
                {
                    throw gcnew ArgumentNullException("assign cannot be null");
                }
                try
                {
                    *parms_ = assign->GetParameters();
                    GC::KeepAlive(assign);
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
            }

            void EncryptionParameters::SetPolyModulus(BigPoly ^polyModulus)
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                if (polyModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("polyModulus cannot be null");
                }
                try
                {
                    parms_->set_poly_modulus(polyModulus->GetPolynomial());
                    GC::KeepAlive(polyModulus);
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
            }

            void EncryptionParameters::SetPolyModulus(System::String ^polyModulus)
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                if (polyModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("polyModulus cannot be null");
                }
                marshal_context ^context = gcnew marshal_context();
                try
                {
                    parms_->set_poly_modulus(context->marshal_as<const char*>(polyModulus));
                    GC::KeepAlive(polyModulus);
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
                finally
                {
                    delete context;
                }
            }

            void EncryptionParameters::SetCoeffModulus(BigUInt ^coeffModulus)
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                if (coeffModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("coeffModulus cannot be null");
                }
                try
                {
                    parms_->set_coeff_modulus(coeffModulus->GetUInt());
                    GC::KeepAlive(coeffModulus);
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
            }

            void EncryptionParameters::SetCoeffModulus(System::UInt64 coeffModulus)
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                try
                {
                    parms_->set_coeff_modulus(coeffModulus);
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
            }

            void EncryptionParameters::SetCoeffModulus(System::String ^coeffModulus)
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                if (coeffModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("coeffModulus cannot be null");
                }
                marshal_context ^context = gcnew marshal_context();
                try
                {
                    parms_->set_coeff_modulus(context->marshal_as<const char*>(coeffModulus));
                    GC::KeepAlive(coeffModulus);
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
                finally
                {
                    delete context;
                }
            }

            EncryptionParameters::EncryptionParameters(const seal::EncryptionParameters &parms) : parms_(nullptr)
            {
                try
                {
                    parms_ = new seal::EncryptionParameters(parms);
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
            }

            BigPoly ^EncryptionParameters::PolyModulus::get()
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                return gcnew BigPoly(parms_->poly_modulus());
            }

            BigUInt ^EncryptionParameters::CoeffModulus::get()
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                return gcnew BigUInt(parms_->coeff_modulus());
            }

#ifndef DISABLE_NTT_IN_MULTIPLY
            BigUInt ^EncryptionParameters::AuxCoeffModulus::get()
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                return gcnew BigUInt(&parms_->aux_coeff_modulus());
            }

            void EncryptionParameters::SetAuxCoeffModulus(BigUInt ^auxCoeffModulus)
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                if (auxCoeffModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("auxCoeffModulus cannot be null");
                }
                try
                {
                    parms_->set_aux_coeff_modulus(auxCoeffModulus->GetUInt());
                    GC::KeepAlive(auxCoeffModulus);
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
            }

            void EncryptionParameters::SetAuxCoeffModulus(System::UInt64 auxCoeffModulus)
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                try
                {
                    parms_->set_aux_coeff_modulus(auxCoeffModulus);
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
            }

            void EncryptionParameters::SetAuxCoeffModulus(System::String ^auxCoeffModulus)
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                if (auxCoeffModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("auxCoeffModulus cannot be null");
                }
                marshal_context ^context = gcnew marshal_context();
                try
                {
                    parms_->set_aux_coeff_modulus(context->marshal_as<const char*>(auxCoeffModulus));
                    GC::KeepAlive(auxCoeffModulus);
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
                finally
                {
                    delete context;
                }
            }
#endif
            BigUInt ^EncryptionParameters::PlainModulus::get()
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                return gcnew BigUInt(parms_->plain_modulus());
            }

            void EncryptionParameters::SetPlainModulus(BigUInt ^plainModulus)
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                if (plainModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("plainModulus cannot be null");
                }
                try
                {
                    parms_->set_plain_modulus(plainModulus->GetUInt());
                    GC::KeepAlive(plainModulus);
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
            }

            void EncryptionParameters::SetPlainModulus(System::UInt64 plainModulus)
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                try
                {
                    parms_->set_plain_modulus(plainModulus);
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
            }

            void EncryptionParameters::SetPlainModulus(System::String ^plainModulus)
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                if (plainModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("plainModulus cannot be null");
                }
                marshal_context ^context = gcnew marshal_context();
                try
                {
                    parms_->set_plain_modulus(context->marshal_as<const char*>(plainModulus));
                    GC::KeepAlive(plainModulus);
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
                finally
                {
                    delete context;
                }
            }

            double EncryptionParameters::NoiseStandardDeviation::get()
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                return parms_->noise_standard_deviation();
            }

            void EncryptionParameters::SetNoiseStandardDeviation(double value)
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                try
                {
                    parms_->set_noise_standard_deviation(value);
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
            }

            double EncryptionParameters::NoiseMaxDeviation::get()
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                return parms_->noise_max_deviation();
            }

            void EncryptionParameters::SetNoiseMaxDeviation(double value)
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                try
                {
                    parms_->set_noise_max_deviation(value);
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
            }

            int EncryptionParameters::DecompositionBitCount::get()
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                return parms_->decomposition_bit_count();
            }

            void EncryptionParameters::SetDecompositionBitCount(int value)
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                try
                {
                    parms_->set_decomposition_bit_count(value);
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
            }

            EncryptionParameterQualifiers ^EncryptionParameters::Validate()
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                try
                {
                    return gcnew EncryptionParameterQualifiers(parms_->validate());
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
                throw gcnew Exception("Unexpected exception");
            }

            int EncryptionParameters::InherentNoiseBitsMax()
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                try
                {
                    return parms_->inherent_noise_bits_max();
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
                throw gcnew Exception("Unexpected exception");
            }

            void EncryptionParameters::InherentNoiseMax(BigUInt ^destination)
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    parms_->inherent_noise_max(destination->GetUInt());
                    GC::KeepAlive(destination);
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
            }

            void EncryptionParameters::Invalidate()
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                try
                {
                    parms_->invalidate();
                }
                catch (const exception &e)
                {
                    HandleException(&e);
                }
                catch (...)
                {
                    HandleException(nullptr);
                }
            }

            void EncryptionParameters::Save(Stream ^stream)
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                if (stream == nullptr)
                {
                    throw gcnew ArgumentNullException("stream cannot be null");
                }
                PolyModulus->Save(stream);
                CoeffModulus->Save(stream);
#ifndef DISABLE_NTT_IN_MULTIPLY
                AuxCoeffModulus->Save(stream);
#endif
                PlainModulus->Save(stream);
                double tempDouble;
                tempDouble = parms_->noise_standard_deviation();
                Write(stream, reinterpret_cast<const char*>(&tempDouble), sizeof(double));
                tempDouble = parms_->noise_max_deviation();
                Write(stream, reinterpret_cast<const char*>(&tempDouble), sizeof(double));
                int32_t decompBitCount32 = static_cast<int32_t>(parms_->decomposition_bit_count());
                Write(stream, reinterpret_cast<const char*>(&decompBitCount32), sizeof(int32_t));
            }

            void EncryptionParameters::Load(Stream ^stream)
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                if (stream == nullptr)
                {
                    throw gcnew ArgumentNullException("stream cannot be null");
                }

                Invalidate();

                BigPoly ^polyModulus = gcnew BigPoly();
                BigUInt ^coeffModulus = gcnew BigUInt();
                BigUInt ^plainModulus = gcnew BigUInt();
                
                polyModulus->Load(stream);
                coeffModulus->Load(stream);
                plainModulus->Load(stream);
#ifndef DISABLE_NTT_IN_MULTIPLY
                BigUInt ^auxCoeffModulus = gcnew BigUInt();
                AuxCoeffModulus->Load(stream);
#endif
                SetPolyModulus(polyModulus);
                SetCoeffModulus(coeffModulus);
                SetPlainModulus(plainModulus);

                double tempDouble;
                Read(stream, reinterpret_cast<char*>(&tempDouble), sizeof(double));
                parms_->set_noise_standard_deviation(tempDouble);
                Read(stream, reinterpret_cast<char*>(&tempDouble), sizeof(double));
                parms_->set_noise_max_deviation(tempDouble);
                int32_t decompBitCount32 = 0;
                Read(stream, reinterpret_cast<char*>(&decompBitCount32), sizeof(int32_t));
                parms_->set_decomposition_bit_count(decompBitCount32);
            }

            EncryptionParameterQualifiers ^EncryptionParameters::Qualifiers::get()
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                return gcnew EncryptionParameterQualifiers(parms_->get_qualifiers());
            }

            seal::EncryptionParameters &EncryptionParameters::GetParameters()
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                return *parms_;
            }

            EncryptionParameters::~EncryptionParameters()
            {
                this->!EncryptionParameters();
            }

            EncryptionParameters::!EncryptionParameters()
            {
                if (parms_ != nullptr)
                {
                    delete parms_;
                    parms_ = nullptr;
                }
            }
        }
    }
}