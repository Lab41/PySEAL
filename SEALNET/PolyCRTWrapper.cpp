#include <vector>
#include "PolyCRTWrapper.h"
#include "BigPolyWrapper.h"
#include "BigUIntWrapper.h"
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
            PolyCRTBuilder::PolyCRTBuilder(BigUInt ^slotModulus, BigPoly ^polyModulus) : polyCRTBuilder_(nullptr)
            {
                if (slotModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("slotModulus cannot be null");
                }
                if (polyModulus == nullptr)
                {
                    throw gcnew ArgumentNullException("polyModulus cannot be null");
                }
                try
                {
                    polyCRTBuilder_ = new seal::PolyCRTBuilder(slotModulus->GetUInt(), polyModulus->GetPolynomial());
                    GC::KeepAlive(slotModulus);
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

            PolyCRTBuilder::!PolyCRTBuilder()
            {
                if (polyCRTBuilder_ != nullptr)
                {
                    delete polyCRTBuilder_;
                    polyCRTBuilder_ = nullptr;
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

            void PolyCRTBuilder::PrepareSlot(int index)
            {
                if (polyCRTBuilder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PolyCRTBuilder is disposed");
                }
                if (index < 0)
                {
                    throw gcnew ArgumentException("index cannot be negative");
                }
                try
                {
                    polyCRTBuilder_->prepare_slot(static_cast<size_t>(index));
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

            void PolyCRTBuilder::PrepareAllSlots()
            {
                if (polyCRTBuilder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PolyCRTBuilder is disposed");
                }
                try
                {
                    polyCRTBuilder_->prepare_all_slots();
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
                        v_values.push_back(val->GetUInt());
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
                        v_values.push_back(val->GetUInt());
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
                    result->Capacity = static_cast<int>(polyCRTBuilder_->get_slot_count());

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

            void PolyCRTBuilder::GetSlot(BigPoly ^poly, int index, BigUInt ^destination)
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
                if (index < 0)
                {
                    throw gcnew ArgumentException("index cannot be negative");
                }
                try
                {
                    polyCRTBuilder_->get_slot(poly->GetPolynomial(), static_cast<size_t>(index), destination->GetUInt());
                    GC::KeepAlive(poly);
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

            BigUInt ^PolyCRTBuilder::GetSlot(BigPoly ^poly, int index)
            {
                if (polyCRTBuilder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PolyCRTBuilder is disposed");
                }
                if (poly == nullptr)
                {
                    throw gcnew ArgumentNullException("poly cannot be null");
                }
                if (index < 0)
                {
                    throw gcnew ArgumentException("index cannot be negative");
                }
                try
                {
                    auto result = gcnew BigUInt(polyCRTBuilder_->get_slot(poly->GetPolynomial(), static_cast<size_t>(index)));
                    GC::KeepAlive(poly);
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

            void PolyCRTBuilder::AddToSlot(BigUInt ^value, int index, BigPoly ^destination)
            {
                if (polyCRTBuilder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PolyCRTBuilder is disposed");
                }
                if (value == nullptr)
                {
                    throw gcnew ArgumentNullException("value cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                if (index < 0)
                {
                    throw gcnew ArgumentException("index cannot be negative");
                }
                try
                {
                    polyCRTBuilder_->add_to_slot(value->GetUInt(), static_cast<size_t>(index), destination->GetPolynomial());
                    GC::KeepAlive(value);
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

            void PolyCRTBuilder::SetSlot(BigUInt ^value, int index, BigPoly ^destination)
            {
                if (polyCRTBuilder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PolyCRTBuilder is disposed");
                }
                if (value == nullptr)
                {
                    throw gcnew ArgumentNullException("value cannot be null");
                }
                if (destination == nullptr)
                {
                    throw gcnew ArgumentNullException("destination cannot be null");
                }
                if (index < 0)
                {
                    throw gcnew ArgumentException("index cannot be negative");
                }
                try
                {
                    polyCRTBuilder_->set_slot(value->GetUInt(), static_cast<size_t>(index), destination->GetPolynomial());
                    GC::KeepAlive(value);
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

            int PolyCRTBuilder::SlotCount::get()
            {
                if (polyCRTBuilder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PolyCRTBuilder is disposed");
                }
                return static_cast<int>(polyCRTBuilder_->get_slot_count());
            }

            BigPoly ^PolyCRTBuilder::GetSlotSumScalingPoly(SortedSet<int> ^indices)
            {
                if (polyCRTBuilder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PolyCRTBuilder is disposed");
                }
                if (indices == nullptr)
                {
                    throw gcnew ArgumentNullException("indices cannot be null");
                }
                try
                {
                    std::set<size_t> set_indices;
                    for each (int index in indices)
                    {
                        if (index < 0)
                        {
                            throw gcnew ArgumentException("index cannot be negative");
                        }
                        set_indices.insert(index);
                    }

                    return gcnew BigPoly(polyCRTBuilder_->get_slot_sum_scaling_poly(set_indices));
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

            BigPoly ^PolyCRTBuilder::GetSlotSumScalingPoly()
            {
                if (polyCRTBuilder_ == nullptr)
                {
                    throw gcnew ObjectDisposedException("PolyCRTBuilder is disposed");
                }
                try
                {
                    return gcnew BigPoly(polyCRTBuilder_->get_slot_sum_scaling_poly());
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
        }
    }
}