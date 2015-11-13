#include "slotbigpoly.h"
#include "internal\common.h"
#include "internal\uintarith.h"
#include "internal\polyarith.h"
#include <algorithm>

using namespace std;
using namespace fhelib::internal;

namespace fhelib
{
	namespace slot_encoding
	{
		void SlotBigPoly::left_shift(int shift_by)
		{
			if (shift_ + shift_by >= slot_size_ || shift_by < 0)
			{
				throw out_of_range("shift");
			}
			if (shift_by == 0)
			{
				return;
			}

			int poly_coeff_count = poly_.coeff_count();
			int poly_new_coeff_count = poly_.significant_coeff_count() + shift_by;
			int poly_coeff_bit_count = poly_.coeff_bit_count();
			int poly_coeff_uint64_count = divide_round_up(poly_coeff_bit_count, bits_per_uint64);
			if (poly_coeff_count < poly_new_coeff_count)
			{
				poly_coeff_count = poly_new_coeff_count;
				poly_.resize(poly_coeff_count, poly_coeff_bit_count);
			}

			//int new_poly_coeff_count = max(poly_.significant_coeff_count() + shift_by, poly_.coeff_count());
			//int poly_coeff_bit_count = poly_.coeff_bit_count();
			//int poly_coeff_uint64_count = divide_round_up(poly_coeff_bit_count, bits_per_uint64);

			//poly_.resize(new_poly_coeff_count, poly_coeff_bit_count);
			left_shift_uint(poly_.pointer(), shift_by * poly_coeff_uint64_count * bits_per_uint64, poly_coeff_count * poly_coeff_uint64_count, poly_.pointer());
			//poly_.resize(poly_.significant_coeff_count(), poly_coeff_bit_count);

			shift_ += shift_by;
		}

		void SlotBigPoly::set_shift(int shift)
		{
			if (shift == -1)
			{
				int poly_coeff_bit_count = poly_.coeff_bit_count();
				int poly_coeff_uint64_count = divide_round_up(poly_coeff_bit_count, bits_per_uint64);
				int old_poly_coeff_count = poly_.significant_coeff_count();
				int poly_slot_count = (old_poly_coeff_count / slot_size_ + (old_poly_coeff_count % slot_size_ != 0));
				int poly_coeff_count = poly_slot_count * slot_size_;

				// Resize poly to have poly_coeff_count equal to a multiple of slot_size
				if (poly_.coeff_count() < poly_coeff_count)
				{
					poly_.resize(poly_coeff_count, poly_coeff_bit_count);
				}

				int max_shift = 0;
				BigUInt slot_uint(poly_coeff_uint64_count * bits_per_uint64 * slot_size_);
				for (int slot_index = 0; slot_index < poly_slot_count; ++slot_index)
				{
					set_uint_uint(poly_[slot_index * slot_size_].pointer(), poly_coeff_uint64_count * slot_size_, slot_uint.pointer());
					int slot_shift = slot_uint.significant_bit_count() / poly_coeff_bit_count;
					if (slot_shift > max_shift)
					{
						max_shift = slot_shift;
					}
				}

				shift_ = max_shift;
			}
			else if (shift >= 0 && shift <= slot_size_ - 1)
			{
				shift_ = shift;
			}
			else
			{
				throw out_of_range("shift");
			}
		}

		void SlotBigPoly::set_slot_size(int slot_size)
		{
			if (slot_size <= 0 || shift_ >= slot_size)
			{
				throw out_of_range("slot_size");
			}
			slot_size_ = slot_size;
		}

		SlotBigPoly::SlotBigPoly() : slot_size_(1), shift_(0), poly_(BigPoly())
		{
		}

		SlotBigPoly::SlotBigPoly(int slot_size) : slot_size_(slot_size), shift_(0), poly_(BigPoly())
		{
			if (slot_size <= 0)
			{
				throw out_of_range("slot_size");
			}
		}

		SlotBigPoly::SlotBigPoly(int slot_size, int coeff_count, int coeff_bit_count)
			: slot_size_(slot_size), shift_(0), poly_(BigPoly(coeff_count, coeff_bit_count))
		{
			if (slot_size <= 0)
			{
				throw out_of_range("slot_size");
			}
		}

		SlotBigPoly::SlotBigPoly(int slot_size, const char *hex_poly) : slot_size_(slot_size), shift_(0), poly_(BigPoly(hex_poly))
		{
			if (slot_size <= 0)
			{
				throw out_of_range("slot_size");
			}
			set_shift();
		}

		SlotBigPoly::SlotBigPoly(int slot_size, int coeff_count, int coeff_bit_count, const char *hex_poly)
			: slot_size_(slot_size), shift_(0), poly_(BigPoly(coeff_count, coeff_bit_count, hex_poly))
		{
			if (slot_size <= 0)
			{
				throw out_of_range("slot_size");
			}
			set_shift();
		}

		SlotBigPoly::SlotBigPoly(int slot_size, int coeff_count, int coeff_bit_count, uint64_t *value)
			: slot_size_(slot_size), shift_(0), poly_(BigPoly(coeff_count, coeff_bit_count, value))
		{
			if (slot_size <= 0)
			{
				throw out_of_range("slot_size");
			}
			set_shift();
		}

		SlotBigPoly::SlotBigPoly(int slot_size, BigPoly &poly) : slot_size_(slot_size), shift_(0), poly_(poly)
		{
			if (slot_size <= 0)
			{
				throw out_of_range("slot_size");
			}
			set_shift();
		}

		void SlotBigPoly::set(BigPoly &poly, int shift)
		{
			// We need to possibly resize to copy the value exactly
			if (poly_.coeff_count() != poly.coeff_count() || poly_.coeff_bit_count() != poly.coeff_bit_count())
			{
				poly_.resize(poly.coeff_count(), poly.coeff_bit_count());
			}
			poly_ = poly;
			set_shift(shift);
		}

		void SlotBigPoly::set(BigPoly &poly, int slot_size, int shift = -1)
		{
			if (slot_size >= shift)
			{
				throw out_of_range("slot_size >= shift");
			}
			// Need to resize so BigPoly.operator =(...) does not do something unexpected. We want to exactly copy the value.
			poly_.resize(poly.coeff_count(), poly.coeff_bit_count());
			poly_ = poly;
			slot_size_ = slot_size;
			set_shift(shift);
		}

		void SlotBigPoly::set(int slot_size, int shift)
		{
			if (slot_size >= shift)
			{
				throw out_of_range("slot_size >= shift");
			}
			slot_size_ = slot_size;
			set_shift(shift);
		}

		void SlotBigPoly::set_zero()
		{
			poly_.set_zero();
			shift_ = 0;
		}

		void SlotBigPoly::save(std::ostream &stream) const
		{
			uint32_t slot_size32 = static_cast<uint32_t>(slot_size_);
			uint32_t shift32 = static_cast<uint32_t>(shift_);
			stream.write(reinterpret_cast<const char*>(&slot_size32), sizeof(uint32_t));
			stream.write(reinterpret_cast<const char*>(&shift32), sizeof(uint32_t));
			poly_.save(stream);
		}

		void SlotBigPoly::load(std::istream &stream)
		{
			uint32_t read_shift = 0;
			uint32_t read_slot_size = 0;
			stream.read(reinterpret_cast<char*>(&read_slot_size), sizeof(uint32_t));
			stream.read(reinterpret_cast<char*>(&read_shift), sizeof(uint32_t));
			poly_.load(stream);
			set_shift(read_shift);
		}
	}
}