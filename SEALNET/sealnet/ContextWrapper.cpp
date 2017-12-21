#include "sealnet/ContextWrapper.h"
#include "sealnet/EncryptionParamsWrapper.h"
#include "sealnet/Common.h"
#include "sealnet/SmallModulusWrapper.h"

using namespace System;
using namespace System::IO;
using namespace std;
using namespace System::Collections::Generic;

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            EncryptionParameterQualifiers::EncryptionParameterQualifiers(const seal::EncryptionParameterQualifiers &qualifiers) : 
                qualifiers_(nullptr)
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

            bool EncryptionParameterQualifiers::EnableFFT::get()
            {
                if (qualifiers_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameterQualifiers is disposed");
                }
                return qualifiers_->enable_fft;
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

            bool EncryptionParameterQualifiers::EnableFastPlainLift::get()
            {
                if (qualifiers_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("EncryptionParameterQualifiers is disposed");
                }
                return qualifiers_->enable_fast_plain_lift;
            }

            SEALContext::SEALContext(EncryptionParameters ^parms)
            {
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                try
                {
                    context_ = new seal::SEALContext(parms->GetParms());
                    GC::KeepAlive(parms);
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

            SEALContext::SEALContext(EncryptionParameters ^parms, MemoryPoolHandle ^pool)
            {
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                if (pool == nullptr)
                {
                    throw gcnew ArgumentNullException("pool cannot be null");
                }
                try
                {
                    context_ = new seal::SEALContext(parms->GetParms(), pool->GetHandle());
                    GC::KeepAlive(parms);
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

            SEALContext::SEALContext(SEALContext ^copy)
            {
                if (copy == nullptr)
                {
                    throw gcnew ArgumentNullException("copy cannot be null");
                }
                try
                {
                    context_ = new seal::SEALContext(copy->GetContext());
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

            SEALContext::SEALContext(const seal::SEALContext &context) : context_(nullptr)
            {
                try
                {
                    context_ = new seal::SEALContext(context);
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

            EncryptionParameters ^SEALContext::Parms::get()
            {
                if (context_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SEALContext is disposed");
                }
                return gcnew EncryptionParameters(context_->parms());
            }

            EncryptionParameterQualifiers ^SEALContext::Qualifiers::get()
            {
                if (context_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SEALContext is disposed");
                }
                return gcnew EncryptionParameterQualifiers(context_->qualifiers());
            }

            BigPoly ^SEALContext::PolyModulus::get()
            {
                if (context_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SEALContext is disposed");
                }
                return gcnew BigPoly(context_->poly_modulus());
            }

            List<SmallModulus^> ^SEALContext::CoeffModulus::get()
            {
                if (context_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SEALContext is disposed");
                }
                auto result = gcnew List<SmallModulus^>;
                for (auto mod : context_->coeff_modulus())
                {
                    result->Add(gcnew SmallModulus(mod));
                }
                return result;
            }

            SmallModulus ^SEALContext::PlainModulus::get()
            {
                if (context_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SEALContext is disposed");
                }
                return gcnew SmallModulus(context_->plain_modulus());
            }

            double SEALContext::NoiseStandardDeviation::get()
            {
                if (context_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SEALContext is disposed");
                }
                return context_->noise_standard_deviation();
            }

            double SEALContext::NoiseMaxDeviation::get()
            {
                if (context_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SEALContext is disposed");
                }
                return context_->noise_max_deviation();
            }

            BigUInt ^SEALContext::TotalCoeffModulus::get()
            {
                if (context_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SEALContext is disposed");
                }
                return gcnew BigUInt(context_->total_coeff_modulus());
            }

            seal::SEALContext &SEALContext::GetContext()
            {
                if (context_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("SEALContext is disposed");
                }
                return *context_;
            }

            SEALContext::~SEALContext()
            {
                this->!SEALContext();
            }

            SEALContext::!SEALContext()
            {
                if (context_ != nullptr)
                {
                    delete context_;
                    context_ = nullptr;
                }
            }
        }
    }
}