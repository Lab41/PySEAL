#include "CppUnitTest.h"
#include "util/ntt.h"
#include "util/mempool.h"
#include "util/uintcore.h"
#include "util/polycore.h"
#include "util/uintarithmod.h"
#include "bigpoly.h"
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
        TEST_CLASS(NTT)
        {
        public:
            TEST_METHOD(NTTTablesBasicsTest)
            {
                MemoryPoolHandle pool = MemoryPoolHandle::acquire_new();
                NTTTables tables(pool);
                Pointer modulus_anchor(allocate_uint(1, pool));
                modulus_anchor[0] = 37;
                int coeff_count_power = 1;
                int coeff_count = 1 << coeff_count_power;
                Modulus modulus(modulus_anchor.get(), 1, pool);
                tables.generate(coeff_count_power, modulus);
                Assert::AreEqual(tables.coeff_count(), int(2));
                Assert::IsTrue(tables.is_generated());
                Assert::AreEqual(tables.coeff_uint64_count(), int(1));
                Assert::AreEqual(tables.coeff_count_power(), int(1));
            }

            TEST_METHOD(NTTTablesPrimitiveRootsTest)
            {   
                MemoryPoolHandle pool = MemoryPoolHandle::acquire_new();
                NTTTables tables(pool);
                {
                    Pointer modulus_anchor(allocate_uint(1, pool));
                    modulus_anchor[0] = 37;
                    int coeff_count_power = 1;
                    int coeff_count = 1 << coeff_count_power;
                    Modulus modulus(modulus_anchor.get(), 1, pool);
                    tables.generate(coeff_count_power, modulus);
                    Assert::AreEqual(*tables.get_from_root_powers(1), static_cast<uint64_t>(6)); 
                    Assert::AreEqual(*tables.get_from_root_powers(0), static_cast<uint64_t>(1));
                }

                {
                    Pointer modulus_anchor(allocate_uint(1, pool));
                    modulus_anchor[0] = 17;
                    int coeff_count_power = 2;
                    int coeff_count = 1 << coeff_count_power;
                    Modulus modulus(modulus_anchor.get(), 1, pool);
                    tables.generate(coeff_count_power, modulus);
                    Assert::AreEqual(*tables.get_from_root_powers(0), static_cast<uint64_t>(1));
                    Assert::AreEqual(*tables.get_from_root_powers(1), static_cast<uint64_t>(4));
                    Assert::AreEqual(*tables.get_from_root_powers(2), static_cast<uint64_t>(2));
                    Assert::AreEqual(*tables.get_from_root_powers(3), static_cast<uint64_t>(8));
                }

                {
                    Pointer modulus_anchor(allocate_uint(2, pool));
                    modulus_anchor[0] = 0xFFFFFFFFDFFFFF01;
                    modulus_anchor[1] = 0xFFF;
                    Modulus modulus(modulus_anchor.get(), 2, pool);
                    int coeff_count_power = 3;
                    int coeff_count = 1 << coeff_count_power;
                    tables.generate(coeff_count_power, modulus);
                    Assert::AreEqual(tables.get_from_root_powers(0)[0], static_cast<uint64_t>(1));
                    Assert::AreEqual(tables.get_from_root_powers(0)[1], static_cast<uint64_t>(0));
                    Assert::AreEqual(tables.get_from_root_powers(1)[0], static_cast<uint64_t>(12050986380748263604));
                    Assert::AreEqual(tables.get_from_root_powers(1)[1], static_cast<uint64_t>(803));
                    Assert::AreEqual(tables.get_from_root_powers(2)[0], static_cast<uint64_t>(16523763614833373397));
                    Assert::AreEqual(tables.get_from_root_powers(2)[1], static_cast<uint64_t>(3850));
                    Assert::AreEqual(tables.get_from_root_powers(3)[0], static_cast<uint64_t>(234973435109839645));
                    Assert::AreEqual(tables.get_from_root_powers(3)[1], static_cast<uint64_t>(1320));
                    Assert::AreEqual(tables.get_from_root_powers(4)[0], static_cast<uint64_t>(2712658429840611173));
                    Assert::AreEqual(tables.get_from_root_powers(4)[1], static_cast<uint64_t>(214));
                    Assert::AreEqual(tables.get_from_root_powers(5)[0], static_cast<uint64_t>(6289473728937792308));
                    Assert::AreEqual(tables.get_from_root_powers(5)[1], static_cast<uint64_t>(2866));
                    Assert::AreEqual(tables.get_from_root_powers(6)[0], static_cast<uint64_t>(14420463455403283083));
                    Assert::AreEqual(tables.get_from_root_powers(6)[1], static_cast<uint64_t>(225));
                    Assert::AreEqual(tables.get_from_root_powers(7)[0], static_cast<uint64_t>(8504020249989480993));
                    Assert::AreEqual(tables.get_from_root_powers(7)[1], static_cast<uint64_t>(943));
                }
            }

            TEST_METHOD(NegacyclicNTTTest)
            {
                MemoryPoolHandle pool = MemoryPoolHandle::acquire_new();
                NTTTables tables(pool);
                {
                    Pointer modulus_anchor(allocate_uint(1, pool));
                    modulus_anchor[0] = 37;
                    int coeff_count_power = 1;
                    int coeff_count = 1 << coeff_count_power;
                    Modulus modulus(modulus_anchor.get(), 1, pool);
                    tables.generate(coeff_count_power, modulus);
                    Pointer poly(allocate_poly(2, 1, pool));
                    poly[0] = 0;
                    poly[1] = 0;
                    ntt_negacyclic_harvey(poly.get(), tables, pool);
                    Assert::AreEqual(poly[0], static_cast<uint64_t>(0));
                    Assert::AreEqual(poly[1], static_cast<uint64_t>(0));

                    poly[0] = 1;
                    poly[1] = 0;
                    ntt_negacyclic_harvey(poly.get(), tables, pool);
                    Assert::AreEqual(poly[0], static_cast<uint64_t>(1));
                    Assert::AreEqual(poly[1], static_cast<uint64_t>(1));

                    poly[0] = 12;
                    poly[1] = 18;
                    ntt_negacyclic_harvey(poly.get(), tables, pool);
                    Assert::AreEqual(poly[0], static_cast<uint64_t>(9));
                    Assert::AreEqual(poly[1], static_cast<uint64_t>(15));
                }

                {
                    Pointer modulus_anchor(allocate_uint(1, pool));
                    modulus_anchor[0] = 12289;
                    int coeff_count_power = 3;
                    int coeff_count = 1 << coeff_count_power;
                    Modulus modulus(modulus_anchor.get(), 1, pool);
                    tables.generate(coeff_count_power, modulus);
                    Pointer poly(allocate_zero_poly(8, 1, pool));
                    ntt_negacyclic_harvey(poly.get(), tables, pool);
                    for (int i = 0; i < 8; ++i)
                    {
                        Assert::AreEqual(poly[i], static_cast<uint64_t>(0));
                    } 

                    for (int i = 0; i < 8; ++i)
                    {
                        poly[i] = i + 1;
                    }
                    ntt_negacyclic_harvey(poly.get(), tables, pool);

                    Assert::AreEqual(poly[0], static_cast<uint64_t>(2285));
                    Assert::AreEqual(poly[1], static_cast<uint64_t>(6357));
                    Assert::AreEqual(poly[2], static_cast<uint64_t>(1586));
                    Assert::AreEqual(poly[3], static_cast<uint64_t>(9352));
                    Assert::AreEqual(poly[4], static_cast<uint64_t>(404));
                    Assert::AreEqual(poly[5], static_cast<uint64_t>(729));
                    Assert::AreEqual(poly[6], static_cast<uint64_t>(6197));
                    Assert::AreEqual(poly[7], static_cast<uint64_t>(9965));
                }
            }


            TEST_METHOD(InverseNegacyclicNTTTest)
            {
                MemoryPoolHandle pool = MemoryPoolHandle::acquire_new();
                NTTTables tables(pool);
                {
                    Pointer modulus_anchor(allocate_uint(1, pool));
                    modulus_anchor[0] = 37;
                    int coeff_count_power = 1;
                    int coeff_count = 1 << coeff_count_power;
                    Modulus modulus(modulus_anchor.get(), 1, pool);
                    tables.generate(coeff_count_power, modulus);
                    Pointer poly(allocate_poly(2, 1, pool));
                    poly[0] = 0;
                    poly[1] = 0;
                    inverse_ntt_negacyclic_harvey(poly.get(), tables, pool);
                    Assert::AreEqual(poly[0], static_cast<uint64_t>(0));
                    Assert::AreEqual(poly[1], static_cast<uint64_t>(0));

                    poly[0] = 2;
                    poly[1] = 2;
                    inverse_ntt_negacyclic_harvey(poly.get(), tables, pool);
                    Assert::AreEqual(poly[0], static_cast<uint64_t>(2));
                    Assert::AreEqual(poly[1], static_cast<uint64_t>(0));

                    poly[0] = 9;
                    poly[1] = 15;
                    inverse_ntt_negacyclic_harvey(poly.get(), tables, pool);
                    Assert::AreEqual(poly[0], static_cast<uint64_t>(12));
                    Assert::AreEqual(poly[1], static_cast<uint64_t>(18));
                }

                {
                    Pointer modulus_anchor(allocate_uint(1, pool));
                    modulus_anchor[0] = 65537;
                    int coeff_count_power = 3;
                    int coeff_count = 1 << coeff_count_power;
                    Modulus modulus(modulus_anchor.get(), 1, pool);
                    tables.generate(coeff_count_power, modulus);
                    Pointer poly(allocate_zero_poly(8, 1, pool));
                    Pointer temp(allocate_zero_poly(8, 1, pool));

                    inverse_ntt_negacyclic_harvey(poly.get(), tables, pool);
                    for (int i = 0; i < 8; ++i)
                    {
                        Assert::AreEqual(poly[i], static_cast<uint64_t>(0));
                    }

                    random_device rd; 
                    for (int i = 0; i < 8; ++i)
                    {
                        poly[i] = static_cast<uint64_t>(rd()) % modulus.get()[0]; 
                        temp[i] = poly[i]; 
                    }
                    
                    ntt_negacyclic_harvey(poly.get(), tables, pool);
                    inverse_ntt_negacyclic_harvey(poly.get(), tables, pool); 
                    for (int i = 0; i < 8; ++i)
                    {
                        Assert::AreEqual(temp[i], poly[i]);
                    }

                }
            }
        };
    }
}