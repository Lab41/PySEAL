#include "CppUnitTest.h"
#include "seal/galoiskeys.h"
#include "seal/context.h"
#include "seal/keygenerator.h"
#include "seal/util/uintcore.h"
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal;
using namespace seal::util;
using namespace std;

namespace SEALTest
{
    TEST_CLASS(GaloisKeysTest)
    {
    public:
        TEST_METHOD(GaloisKeysSaveLoad)
        {
            stringstream stream;
            {
                EncryptionParameters parms;
                parms.set_noise_standard_deviation(3.19);
                parms.set_poly_modulus("1x^64 + 1");
                parms.set_plain_modulus(65537);
                parms.set_coeff_modulus({ small_mods_60bit(0) });
                SEALContext context(parms);
                KeyGenerator keygen(context);

                GaloisKeys keys;
                GaloisKeys test_keys;
                Assert::AreEqual(keys.decomposition_bit_count(), 0);
                keys.save(stream);
                test_keys.load(stream);
                Assert::AreEqual(keys.size(), test_keys.size());
                Assert::IsTrue(keys.hash_block() == test_keys.hash_block());
                Assert::AreEqual(keys.decomposition_bit_count(), test_keys.decomposition_bit_count());
                Assert::AreEqual(0, keys.size());

                keygen.generate_galois_keys(1, keys);
                Assert::AreEqual(keys.decomposition_bit_count(), 1);
                keys.save(stream);
                test_keys.load(stream);
                Assert::AreEqual(keys.size(), test_keys.size());
                Assert::IsTrue(keys.hash_block() == test_keys.hash_block());
                Assert::AreEqual(keys.decomposition_bit_count(), test_keys.decomposition_bit_count());
                for (int j = 0; j < test_keys.size(); j++)
                {
                    for (int i = 0; i < test_keys.data()[j].size(); i++)
                    {
                        Assert::AreEqual(keys.data()[j][i].size(), test_keys.data()[j][i].size());
                        Assert::AreEqual(keys.data()[j][i].uint64_count(), test_keys.data()[j][i].uint64_count());
                        Assert::IsTrue(is_equal_uint_uint(keys.data()[j][i].pointer(), test_keys.data()[j][i].pointer(), keys.data()[j][i].uint64_count()));
                    }
                }
                Assert::AreEqual(10, keys.size());

                keygen.generate_galois_keys(8, keys);
                Assert::AreEqual(keys.decomposition_bit_count(), 8);
                keys.save(stream);
                test_keys.load(stream);
                Assert::AreEqual(keys.size(), test_keys.size());
                Assert::IsTrue(keys.hash_block() == test_keys.hash_block());
                Assert::AreEqual(keys.decomposition_bit_count(), test_keys.decomposition_bit_count());
                for (int j = 0; j < test_keys.size(); j++)
                {
                    for (int i = 0; i < test_keys.data()[j].size(); i++)
                    {
                        Assert::AreEqual(keys.data()[j][i].size(), test_keys.data()[j][i].size());
                        Assert::AreEqual(keys.data()[j][i].uint64_count(), test_keys.data()[j][i].uint64_count());
                        Assert::IsTrue(is_equal_uint_uint(keys.data()[j][i].pointer(), test_keys.data()[j][i].pointer(), keys.data()[j][i].uint64_count()));
                    }
                }
                Assert::AreEqual(10, keys.size());

                keygen.generate_galois_keys(60, keys);
                Assert::AreEqual(keys.decomposition_bit_count(), 60);
                keys.save(stream);
                test_keys.load(stream);
                Assert::AreEqual(keys.size(), test_keys.size());
                Assert::IsTrue(keys.hash_block() == test_keys.hash_block());
                Assert::AreEqual(keys.decomposition_bit_count(), test_keys.decomposition_bit_count());
                for (int j = 0; j < test_keys.size(); j++)
                {
                    for (int i = 0; i < test_keys.data()[j].size(); i++)
                    {
                        Assert::AreEqual(keys.data()[j][i].size(), test_keys.data()[j][i].size());
                        Assert::AreEqual(keys.data()[j][i].uint64_count(), test_keys.data()[j][i].uint64_count());
                        Assert::IsTrue(is_equal_uint_uint(keys.data()[j][i].pointer(), test_keys.data()[j][i].pointer(), keys.data()[j][i].uint64_count()));
                    }
                }
                Assert::AreEqual(10, keys.size());
            }
            {
                EncryptionParameters parms;
                parms.set_noise_standard_deviation(3.19);
                parms.set_poly_modulus("1x^256 + 1");
                parms.set_plain_modulus(65537);
                parms.set_coeff_modulus({ small_mods_60bit(0), small_mods_50bit(0) });
                SEALContext context(parms);
                KeyGenerator keygen(context);

                GaloisKeys keys;
                GaloisKeys test_keys;
                Assert::AreEqual(keys.decomposition_bit_count(), 0);
                keys.save(stream);
                test_keys.load(stream);
                Assert::AreEqual(keys.size(), test_keys.size());
                Assert::IsTrue(keys.hash_block() == test_keys.hash_block());
                Assert::AreEqual(keys.decomposition_bit_count(), test_keys.decomposition_bit_count());
                Assert::AreEqual(0, keys.size());

                keygen.generate_galois_keys(8, keys);
                Assert::AreEqual(keys.decomposition_bit_count(), 8);
                keys.save(stream);
                test_keys.load(stream);
                Assert::AreEqual(keys.size(), test_keys.size());
                Assert::IsTrue(keys.hash_block() == test_keys.hash_block());
                Assert::AreEqual(keys.decomposition_bit_count(), test_keys.decomposition_bit_count());
                for (int j = 0; j < test_keys.size(); j++)
                {
                    for (int i = 0; i < test_keys.data()[j].size(); i++)
                    {
                        Assert::AreEqual(keys.data()[j][i].size(), test_keys.data()[j][i].size());
                        Assert::AreEqual(keys.data()[j][i].uint64_count(), test_keys.data()[j][i].uint64_count());
                        Assert::IsTrue(is_equal_uint_uint(keys.data()[j][i].pointer(), test_keys.data()[j][i].pointer(), keys.data()[j][i].uint64_count()));
                    }
                }
                Assert::AreEqual(14, keys.size());

                keygen.generate_galois_keys(60, keys);
                Assert::AreEqual(keys.decomposition_bit_count(), 60);
                keys.save(stream);
                test_keys.load(stream);
                Assert::AreEqual(keys.size(), test_keys.size());
                Assert::IsTrue(keys.hash_block() == test_keys.hash_block());
                Assert::AreEqual(keys.decomposition_bit_count(), test_keys.decomposition_bit_count());
                for (int j = 0; j < test_keys.size(); j++)
                {
                    for (int i = 0; i < test_keys.data()[j].size(); i++)
                    {
                        Assert::AreEqual(keys.data()[j][i].size(), test_keys.data()[j][i].size());
                        Assert::AreEqual(keys.data()[j][i].uint64_count(), test_keys.data()[j][i].uint64_count());
                        Assert::IsTrue(is_equal_uint_uint(keys.data()[j][i].pointer(), test_keys.data()[j][i].pointer(), keys.data()[j][i].uint64_count()));
                    }
                }
                Assert::AreEqual(14, keys.size());
            }
        }
    };
}