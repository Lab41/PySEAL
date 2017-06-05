#include <vector>
#include "PolyCRTWrapper.h"
#include "BigPolyWrapper.h"
#include "BigUIntWrapper.h"
#include "EncryptionParamsWrapper.h"
#include "Common.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace std;

namespace Microsoft
{
    namespace Research
    {
        namespace SEAL
        {
            PolyCRTBuilder::PolyCRTBuilder(EncryptionParameters ^parms) : polyCRTBuilder_(nullptr)
            {
                if (parms == nullptr)
                {
                    throw gcnew ArgumentNullException("parms cannot be null");
                }
                try
                {
                    polyCRTBuilder_ = new seal::PolyCRTBuilder(parms->GetParameters());
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

            PolyCRTBuilder::PolyCRTBuilder(EncryptionParameters ^parms, MemoryPoolHandle ^pool) : polyCRTBuilder_(nullptr)
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
                    polyCRTBuilder_ = new seal::PolyCRTBuilder(parms->GetParameters(), pool->GetHandle());
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

            PolyCRTBuilder::!PolyCRTBuilder()
            {
                if (polyCRTBuilder_ != nullptr)
                {
                    delete polyCRTBuilder_;
                    polyCRTBuilder_ = nullptr;
                }
            }

            PolyCRTBuilder::PolyCRTBuilder(PolyCRTBuilder ^copy) : polyCRTBuilder_(nullptr)
            {
                if (copy == nullptr)
                {
                    throw gcnew ArgumentNullException("copy cannot be null");
                }
                try
                {
                    polyCRTBuilder_ = new seal::PolyCRTBuilder(copy->GetPolyCRTBuilder());
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

            PolyCRTBuilder::~PolyCRTBuilder()
            {
                this->!PolyCRTBuilder();
            }

            seal::PolyCRTBuilder &PolyCRTBuilder::GetPolyCRTBuilder()
            {
                if (polyCRTBuilder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PolyCRTBuilder is disposed");
                }
                return *polyCRTBuilder_;
            }

            void PolyCRTBuilder::Compose(List<BigUInt^> ^values, BigPoly ^destination)
            {
                if (polyCRTBuilder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PolyCRTBuilder is disposed");
                }
                if (values == nullptr)
                {
                    throw gcnew ArgumentNullException("values cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    vector<seal::BigUInt> v_values;
                    for each (BigUInt ^val in values)
                    {
                        if (val == nullptr)
                        {
                            throw gcnew ArgumentNullException("values cannot contain null values");
                        }
                        v_values.emplace_back(val->GetUInt());
                        GC::KeepAlive(val);
                    }

                    polyCRTBuilder_->compose(v_values, destination->GetPolynomial());
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

            void PolyCRTBuilder::Compose(List<UInt64> ^values, BigPoly ^destination)
            {
                if (polyCRTBuilder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PolyCRTBuilder is disposed");
                }
                if (values == nullptr)
                {
                    throw gcnew ArgumentNullException("values cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    vector<std::uint64_t> v_values;
                    for each (UInt64 val in values)
                    {
                        v_values.emplace_back(val);
                    }

                    polyCRTBuilder_->compose(v_values, destination->GetPolynomial());
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

            BigPoly ^PolyCRTBuilder::Compose(List<BigUInt^> ^values)
            {
                if (polyCRTBuilder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PolyCRTBuilder is disposed");
                }
                if (values == nullptr)
                {
                    throw gcnew ArgumentNullException("values cannot be null");
                }
                try
                {
                    vector<seal::BigUInt> v_values;
                    for each (BigUInt ^val in values)
                    {
                        if (val == nullptr)
                        {
                            throw gcnew ArgumentNullException("values cannot contain null values");
                        }
                        v_values.emplace_back(val->GetUInt());
                        GC::KeepAlive(val);
                    }

                    return gcnew BigPoly(polyCRTBuilder_->compose(v_values));
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

            BigPoly ^PolyCRTBuilder::Compose(List<UInt64> ^values)
            {
                if (polyCRTBuilder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PolyCRTBuilder is disposed");
                }
                if (values == nullptr)
                {
                    throw gcnew ArgumentNullException("values cannot be null");
                }
                try
                {
                    vector<std::uint64_t> v_values;
                    for each (UInt64 val in values)
                    {
                        v_values.emplace_back(val);
                    }

                    return gcnew BigPoly(polyCRTBuilder_->compose(v_values));
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

            void PolyCRTBuilder::Decompose(BigPoly ^poly, List<BigUInt^> ^destination)
            {
                if (polyCRTBuilder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PolyCRTBuilder is disposed");
                }
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                try
                {
                    vector<seal::BigUInt> v_destination;
                    polyCRTBuilder_->decompose(poly->GetPolynomial(), v_destination);
                    GC::KeepAlive(poly);

                    for (size_t i = 0; i < v_destination.size(); ++i)
                    {
                        destination->Add(gcnew BigUInt(v_destination[i]));
                    }
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

            List<BigUInt^> ^PolyCRTBuilder::Decompose(BigPoly ^poly)
            {
                if (polyCRTBuilder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PolyCRTBuilder is disposed");
                }
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                try
                {
                    auto result = gcnew List<BigUInt^>;
                    result->Capacity = polyCRTBuilder_->get_slot_count();

                    vector<seal::BigUInt> v_result = polyCRTBuilder_->decompose(poly->GetPolynomial());
                    GC::KeepAlive(poly);

                    for (size_t i = 0; i < v_result.size(); ++i)
                    {
                        result->Add(gcnew BigUInt(v_result[i]));
                    }

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

            int PolyCRTBuilder::SlotCount::get()
            {
                if (polyCRTBuilder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PolyCRTBuilder is disposed");
                }
                return polyCRTBuilder_->get_slot_count();
            }
        }
    }
}