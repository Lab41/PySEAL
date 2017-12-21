#include "CppUnitTest.h"
#include "seal/util/mempool.h"
#include "seal/util/uintcore.h"
#include "seal/util/polycore.h"
#include "seal/util/smallntt.h"
#include "seal/defaultparams.h"
#include "seal/util/numth.h"
#include <random>
#include <cstdint>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal;
using namespace seal::util;
using namespace std;

namespace SEALTest
{
    namespace util
    {
        TEST_CLASS(SmallNTTTablesTest)
        {
        public:
            TEST_METHOD(SmallNTTBasics)
            {
                MemoryPoolHandle pool = MemoryPoolHandle::Global();
                SmallNTTTables tables(pool);
                int coeff_count_power = 1;
                int coeff_count = 1 << coeff_count_power;
                SmallModulus modulus(small_mods_60bit(0));
                tables.generate(coeff_count_power, modulus);
                Assert::AreEqual(2, tables.coeff_count());
                Assert::IsTrue(tables.is_generated());
                Assert::AreEqual(1, tables.coeff_count_power());

                coeff_count_power = 2;
                coeff_count = 1 << coeff_count_power;
                modulus = small_mods_50bit(0);
                tables.generate(coeff_count_power, modulus);
                Assert::AreEqual(4, tables.coeff_count());
                Assert::IsTrue(tables.is_generated());
                Assert::AreEqual(2, tables.coeff_count_power());

                coeff_count_power = 10;
                coeff_count = 1 << coeff_count_power;
                modulus = small_mods_40bit(0);
                tables.generate(coeff_count_power, modulus);
                Assert::AreEqual(1024, tables.coeff_count());
                Assert::IsTrue(tables.is_generated());
                Assert::AreEqual(10, tables.coeff_count_power());
            }

            TEST_METHOD(SmallNTTPrimitiveRootsTest)
            {
            	MemoryPoolHandle pool = MemoryPoolHandle::Global();
            	SmallNTTTables tables(pool);

                int coeff_count_power = 1;
                SmallModulus modulus(0xffffffffffc0001ULL);
                tables.generate(coeff_count_power, modulus);
                Assert::AreEqual(1ULL, tables.get_from_root_powers(0));
                Assert::AreEqual(288794978602139552ULL, tables.get_from_root_powers(1));
                uint64_t inv;
                try_mod_inverse(tables.get_from_root_powers(1), modulus.value(), inv);
                Assert::AreEqual(inv, tables.get_from_inv_root_powers(1));

                coeff_count_power = 2;
                tables.generate(coeff_count_power, modulus);
                Assert::AreEqual(1ULL, tables.get_from_root_powers(0));
                Assert::AreEqual(288794978602139552ULL, tables.get_from_root_powers(1));
                Assert::AreEqual(178930308976060547ULL, tables.get_from_root_powers(2));
                Assert::AreEqual(748001537669050592ULL, tables.get_from_root_powers(3));           	
            }

            TEST_METHOD(NegacyclicSmallNTTTest)
            {
            	MemoryPoolHandle pool = MemoryPoolHandle::Global();
            	SmallNTTTables tables(pool);

                int coeff_count_power = 1;
                int coeff_count = 1 << coeff_count_power;
                SmallModulus modulus(0xffffffffffc0001ULL);
                tables.generate(coeff_count_power, modulus);
                Pointer poly(allocate_poly(2, 1, pool));
                poly[0] = 0;
                poly[1] = 0;
                ntt_negacyclic_harvey(poly.get(), tables);
                Assert::AreEqual(0ULL, poly[0]);
                Assert::AreEqual(0ULL, poly[1]);

                poly[0] = 1;
                poly[1] = 0;
                ntt_negacyclic_harvey(poly.get(), tables);
                Assert::AreEqual(1ULL, poly[0]);
                Assert::AreEqual(1ULL, poly[1]);

                poly[0] = 1;
                poly[1] = 1;
                ntt_negacyclic_harvey(poly.get(), tables);
                Assert::AreEqual(288794978602139553ULL, poly[0]);
                Assert::AreEqual(864126526004445282ULL, poly[1]);
            }

            TEST_METHOD(InverseNegacyclicSmallNTTTest)
            {
            	MemoryPoolHandle pool = MemoryPoolHandle::Global();
            	SmallNTTTables tables(pool);

                int coeff_count_power = 3;
                int coeff_count = 1 << coeff_count_power;
                SmallModulus modulus(0xffffffffffc0001ULL);
                tables.generate(coeff_count_power, modulus);
                Pointer poly(allocate_zero_poly(800, 1, pool));
                Pointer temp(allocate_zero_poly(800, 1, pool));

                inverse_ntt_negacyclic_harvey(poly.get(), tables);
                for (int i = 0; i < 800; i++)
                {
                    Assert::AreEqual(0ULL, poly[i]);
                }

                random_device rd;
                for (int i = 0; i < 800; i++)
                {
                    poly[i] = static_cast<uint64_t>(rd()) % modulus.value();
                    temp[i] = poly[i];
                }

                ntt_negacyclic_harvey(poly.get(), tables);
                inverse_ntt_negacyclic_harvey(poly.get(), tables);
                for (int i = 0; i < 800; i++)
                {
                    Assert::AreEqual(temp[i], poly[i]);
                }
            }
        };
    }
}