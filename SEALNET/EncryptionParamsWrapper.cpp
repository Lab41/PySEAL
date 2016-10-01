#include <cstddef>
#include "EncryptionParamsWrapper.h"
#include "Common.h"
#include "BigPolyWrapper.h"
#include "BigUIntWrapper.h"

using namespace System;
using namespace System::IO;
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
                return gcnew BigPoly(&parms_->poly_modulus());
            }

            BigUInt ^EncryptionParameters::CoeffModulus::get()
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                return gcnew BigUInt(&parms_->coeff_modulus());
            }

            BigUInt ^EncryptionParameters::AuxCoeffModulus::get()
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                return gcnew BigUInt(&parms_->aux_coeff_modulus());
            }

            BigUInt ^EncryptionParameters::PlainModulus::get()
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                return gcnew BigUInt(&parms_->plain_modulus());
            }

            double EncryptionParameters::NoiseStandardDeviation::get()
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                return parms_->noise_standard_deviation();
            }

            void EncryptionParameters::NoiseStandardDeviation::set(double value)
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                parms_->noise_standard_deviation() = value;
            }

            double EncryptionParameters::NoiseMaxDeviation::get()
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                return parms_->noise_max_deviation();
            }

            void EncryptionParameters::NoiseMaxDeviation::set(double value)
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                parms_->noise_max_deviation() = value;
            }

            int EncryptionParameters::DecompositionBitCount::get()
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                return parms_->decomposition_bit_count();
            }

            void EncryptionParameters::DecompositionBitCount::set(int value)
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                parms_->decomposition_bit_count() = value;
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
                AuxCoeffModulus->Save(stream);
                PlainModulus->Save(stream);
                Write(stream, reinterpret_cast<const char*>(&parms_->noise_standard_deviation()), sizeof(double));
                Write(stream, reinterpret_cast<const char*>(&parms_->noise_max_deviation()), sizeof(double));
                int32_t decomp_bit_count32 = static_cast<int32_t>(parms_->decomposition_bit_count());
                Write(stream, reinterpret_cast<const char*>(&decomp_bit_count32), sizeof(int32_t));
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
                PolyModulus->Load(stream);
                CoeffModulus->Load(stream);
                AuxCoeffModulus->Load(stream);
                PlainModulus->Load(stream);
                Read(stream, reinterpret_cast<char*>(&parms_->noise_standard_deviation()), sizeof(double));
                Read(stream, reinterpret_cast<char*>(&parms_->noise_max_deviation()), sizeof(double));
                int32_t decomp_bit_count32 = 0;
                Read(stream, reinterpret_cast<char*>(&decomp_bit_count32), sizeof(int32_t));
                parms_->decomposition_bit_count() = decomp_bit_count32;
            }

            EncryptionParameterQualifiers ^EncryptionParameters::Qualifiers::get()
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                try
                {
                    return gcnew EncryptionParameterQualifiers(parms_->get_qualifiers());
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