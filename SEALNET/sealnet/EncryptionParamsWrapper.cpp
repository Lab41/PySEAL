#include "sealnet/EncryptionParamsWrapper.h"
#include "sealnet/Common.h"

using namespace System;
using namespace System::IO;
using namespace System::Collections::Generic;
using namespace std;

namespace seal
{
    /**
    <summary>Enables access to private members of seal::EncryptionParameters.</summary>
    */
    struct EncryptionParameters::EncryptionParametersPrivateHelper
    {
        static void set_poly_modulus(seal::EncryptionParameters *parms, const seal::BigPoly &poly_modulus)
        {
            parms->poly_modulus_.resize(1, 1);
            parms->poly_modulus_ = poly_modulus;
        }

        static void set_coeff_modulus(seal::EncryptionParameters *parms, const std::vector<seal::SmallModulus> &coeff_modulus)
        {
            parms->coeff_modulus_ = coeff_modulus;
        }

        static void set_plain_modulus(seal::EncryptionParameters *parms, const seal::SmallModulus &plain_modulus)
        {
            parms->plain_modulus_ = plain_modulus;
        }

        static void set_noise_standard_deviation(seal::EncryptionParameters *parms, double noise_standard_deviation)
        {
            parms->noise_standard_deviation_ = noise_standard_deviation;
        }

        static void set_noise_max_deviation(seal::EncryptionParameters *parms, double noise_max_deviation)
        {
            parms->noise_max_deviation_ = noise_max_deviation;
        }

        static void compute_hash(seal::EncryptionParameters *parms)
        {
            parms->compute_hash();
        }
    };
}

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
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

            EncryptionParameters::EncryptionParameters(EncryptionParameters ^copy)
            {
                if (copy == nullptr)
                {
                    throw gcnew ArgumentNullException("copy cannot be null");
                }
                try
                {
                    parms_ = new seal::EncryptionParameters(copy->GetParms());
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
                    *parms_ = assign->GetParms();
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

            EncryptionParameters::EncryptionParameters(const seal::EncryptionParameters &parms)
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

            seal::EncryptionParameters &EncryptionParameters::GetParms()
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

            void EncryptionParameters::PolyModulus::set(BigPoly ^polyModulus)
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

            void EncryptionParameters::CoeffModulus::set(List<SmallModulus^> ^coeffModulus)
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
                    vector<seal::SmallModulus> v_coeff_modulus;
                    for each (SmallModulus ^mod in coeffModulus)
                    {
                        if (mod == nullptr)
                        {
                            throw gcnew ArgumentNullException("coeffModulus cannot be null");
                        }
                        v_coeff_modulus.emplace_back(mod->GetModulus());
                        GC::KeepAlive(mod);
                    }
                    parms_->set_coeff_modulus(v_coeff_modulus);
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

            void EncryptionParameters::PlainModulus::set(SmallModulus ^plainModulus)
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
                    parms_->set_plain_modulus(plainModulus->GetModulus());
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

            BigPoly ^EncryptionParameters::PolyModulus::get()
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                return gcnew BigPoly(parms_->poly_modulus());
            }

            List<SmallModulus^> ^EncryptionParameters::CoeffModulus::get()
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                auto result = gcnew List<SmallModulus^>;
                for (auto mod : parms_->coeff_modulus())
                {
                    result->Add(gcnew SmallModulus(mod));
                }
                return result;
            }

            SmallModulus ^EncryptionParameters::PlainModulus::get()
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                return gcnew SmallModulus(parms_->plain_modulus());
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
                int32_t coeff_mod_count32 = parms_->coeff_modulus().size();
                Write(stream, reinterpret_cast<const char*>(&coeff_mod_count32), sizeof(int32_t));
                auto coeffModulus = CoeffModulus;
                for (int i = 0; i < coeff_mod_count32; i++)
                {
                    coeffModulus[i]->Save(stream);
                }
                GC::KeepAlive(coeffModulus);
                PlainModulus->Save(stream);
                double tempDouble;
                tempDouble = parms_->noise_standard_deviation();
                Write(stream, reinterpret_cast<const char*>(&tempDouble), sizeof(double));
                tempDouble = parms_->noise_max_deviation();
                Write(stream, reinterpret_cast<const char*>(&tempDouble), sizeof(double));
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

                auto polyModulus = gcnew BigPoly;
                polyModulus->Load(stream);
                seal::EncryptionParameters::EncryptionParametersPrivateHelper::set_poly_modulus(parms_, polyModulus->GetPolynomial());
                GC::KeepAlive(polyModulus);

                int32_t coeff_mod_count32 = 0;
                Read(stream, reinterpret_cast<char*>(&coeff_mod_count32), sizeof(int32_t));
                vector<seal::SmallModulus> v_coeff_modulus;
                auto coeffModulus = gcnew SmallModulus;
                for (int i = 0; i < coeff_mod_count32; i++)
                {
                    coeffModulus->Load(stream);
                    v_coeff_modulus.emplace_back(coeffModulus->GetModulus());
                }
                seal::EncryptionParameters::EncryptionParametersPrivateHelper::set_coeff_modulus(parms_, v_coeff_modulus);
                GC::KeepAlive(coeffModulus);

                auto plainModulus = gcnew SmallModulus;
                plainModulus->Load(stream);
                seal::EncryptionParameters::EncryptionParametersPrivateHelper::set_plain_modulus(parms_, plainModulus->GetModulus());
                GC::KeepAlive(plainModulus);

                double tempDouble;
                Read(stream, reinterpret_cast<char*>(&tempDouble), sizeof(double));
                seal::EncryptionParameters::EncryptionParametersPrivateHelper::set_noise_standard_deviation(parms_, tempDouble);
                Read(stream, reinterpret_cast<char*>(&tempDouble), sizeof(double));
                seal::EncryptionParameters::EncryptionParametersPrivateHelper::set_noise_max_deviation(parms_, tempDouble);

                // Update parameter hash
                seal::EncryptionParameters::EncryptionParametersPrivateHelper::compute_hash(parms_);
            }

            Tuple<UInt64, UInt64, UInt64, UInt64> ^EncryptionParameters::HashBlock::get()
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                return gcnew Tuple<UInt64, UInt64, UInt64, UInt64>(
                    parms_->hash_block()[0],
                    parms_->hash_block()[1],
                    parms_->hash_block()[2],
                    parms_->hash_block()[3]);
            }

            bool EncryptionParameters::Equals(EncryptionParameters ^compare)
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                if (compare == nullptr)
                {
                    return false;
                }
                try
                {
                    auto result = *parms_ == compare->GetParms();
                    GC::KeepAlive(compare);
                    return result;
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

            bool EncryptionParameters::Equals(Object ^compare)
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                EncryptionParameters ^compareparms = dynamic_cast<EncryptionParameters^>(compare);
                if (compareparms != nullptr)
                {
                    return Equals(compareparms);
                }
                return false;
            }

            int EncryptionParameters::GetHashCode()
            {
                if (parms_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameters is disposed");
                }
                return ComputeArrayHashCode(parms_->hash_block().data(), sizeof(parms_->hash_block()) / seal::util::bytes_per_uint64);
            }
        }
    }
}