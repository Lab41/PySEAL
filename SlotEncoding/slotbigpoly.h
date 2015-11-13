#ifndef FHELIB_SLOTBIGPOLY_H
#define FHELIB_SLOTBIGPOLY_H

#include "bigpoly.h"
#include <vector>

namespace fhelib
{
	namespace slot_encoding
	{
		class SlotBigPoly
		{
		public:
			SlotBigPoly();

			SlotBigPoly(int slot_size);

			SlotBigPoly(int slot_size, int coeff_count, int coeff_bit_count);

			SlotBigPoly(int slot_size, const char *hex_poly);

			SlotBigPoly(int slot_size, int coeff_count, int coeff_bit_count, const char *hex_poly);

			SlotBigPoly(int slot_size, int coeff_count, int coeff_bit_count, std::uint64_t *value);
			
			SlotBigPoly(int slot_size, BigPoly &poly);

			// Setting int shift to a non-negative value forces shift_ to equal that value. Otherwise shift_ will be determined from the polynomial poly,
			// which is good unless you really know what you are doing.
			void set(BigPoly &poly, int shift = -1);

			void set(BigPoly &poly, int slot_size, int shift);

			void set(int slot_size, int shift);

			void set_shift(int shift = -1);

			void left_shift(int shift_by);

			void set_slot_size(int slot_size);

			inline const BigPoly &poly() const
			{
				return poly_;
			}

			inline BigPoly &poly()
			{
				return poly_;
			}

			inline int slot_size() const
			{
				return slot_size_;
			}

			inline int slot_count() const
			{
				return (poly_.significant_coeff_count() / slot_size_ + (poly_.significant_coeff_count() % slot_size_ != 0));
			}

			inline int shift() const
			{
				return shift_;
			}

			inline int coeff_count() const
			{
				return poly_.coeff_count();
			}

			inline int coeff_bit_count() const
			{
				return poly_.coeff_bit_count();
			}

			inline const std::uint64_t *pointer() const
			{
				return poly_.pointer();
			}

			inline std::uint64_t *pointer()
			{
				return poly_.pointer();
			}

			inline int uint64_count() const
			{
				return poly_.uint64_count();
			}

			inline int significant_coeff_count() const
			{
				return poly_.significant_coeff_count();
			}

			inline std::string to_string() const
			{
				return poly_.to_string();
			}

			inline bool operator ==(const SlotBigPoly &compare) const
			{
				return (slot_size_ == compare.slot_size_) && (shift_ == compare.shift_) && (poly_ == compare.poly_);
			}

			inline bool operator !=(const SlotBigPoly &compare) const
			{
				return !operator ==(compare);
			}

			inline bool is_zero() const
			{
				return poly_.is_zero();
			}

			inline const BigUInt &operator[](int coeff_index) const
			{
				return poly_[coeff_index];
			}

			inline BigUInt &operator[](int coeff_index)
			{
				return poly_[coeff_index];
			}

			void set_zero();

			void save(std::ostream &stream) const;

			void load(std::istream &stream);

		private:
			BigPoly poly_;

			int slot_size_;

			int shift_;
		};
	}
}

#endif // FHELIB_SLOTBIGPOLY_H