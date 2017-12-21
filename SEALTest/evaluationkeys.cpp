#include "CppUnitTest.h"
#include "seal/evaluationkeys.h"
#include "seal/context.h"
#include "seal/keygenerator.h"
#include "seal/util/uintcore.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal;
using namespace seal::util;
using namespace std;

namespace SEALTest
{
    TEST_CLASS(EvaluationKeysTest)
    {
    public:
        TEST_METHOD(EvaluationKeysSaveLoad)
        {
            stringstream stream;
            {
                EncryptionParameters parms;
                parms.set_noise_standard_deviation(3.19);
                parms.set_poly_modulus("1x^64 + 1");
                parms.set_plain_modulus(1 << 6);
                parms.set_coeff_modulus({ small_mods_60bit(0) });
                SEALContext context(parms);
                KeyGenerator keygen(context);

                EvaluationKeys keys;
                EvaluationKeys test_keys;
                Assert::AreEqual(keys.decomposition_bit_count(), 0);
                keys.save(stream);
                test_keys.load(stream);
                Assert::AreEqual(keys.size(), test_keys.size());
                Assert::IsTrue(keys.hash_block() == test_keys.hash_block());
                Assert::AreEqual(keys.decomposition_bit_count(), test_keys.decomposition_bit_count());
                Assert::AreEqual(0, keys.size());

                keygen.generate_evaluation_keys(1, 1, keys);
                Assert::AreEqual(keys.decomposition_bit_count(), 1);
                keys.save(stream);
                test_keys.load(stream);
                Assert::AreEqual(keys.size(), test_keys.size());
                Assert::IsTrue(keys.hash_block() == test_keys.hash_block());
                Assert::AreEqual(keys.decomposition_bit_count(), test_keys.decomposition_bit_count());
                for (int j = 0; j < test_keys.size(); j++)
                {
                    for (int i = 0; i < test_keys.key(j + 2).size(); i++)
                    {
                        Assert::AreEqual(keys.key(j + 2)[i].size(), test_keys.key(j + 2)[i].size());
                        Assert::AreEqual(keys.key(j + 2)[i].uint64_count(), test_keys.key(j + 2)[i].uint64_count());
                        Assert::IsTrue(is_equal_uint_uint(keys.key(j + 2)[i].pointer(), test_keys.key(j + 2)[i].pointer(), keys.key(j + 2)[i].uint64_count()));
                    }
                }

                keygen.generate_evaluation_keys(2, 1, keys);
                Assert::AreEqual(keys.decomposition_bit_count(), 2);
                keys.save(stream);
                test_keys.load(stream);
                Assert::AreEqual(keys.size(), test_keys.size());
                Assert::IsTrue(keys.hash_block() == test_keys.hash_block());
                Assert::AreEqual(keys.decomposition_bit_count(), test_keys.decomposition_bit_count());
                for (int j = 0; j < test_keys.size(); j++)
                {
                    for (int i = 0; i < test_keys.key(j + 2).size(); i++)
                    {
                        Assert::AreEqual(keys.key(j + 2)[i].size(), test_keys.key(j + 2)[i].size());
                        Assert::AreEqual(keys.key(j + 2)[i].uint64_count(), test_keys.key(j + 2)[i].uint64_count());
                        Assert::IsTrue(is_equal_uint_uint(keys.key(j + 2)[i].pointer(), test_keys.key(j + 2)[i].pointer(), keys.key(j + 2)[i].uint64_count()));
                    }
                }

                keygen.generate_evaluation_keys(59, 2, keys);
                Assert::AreEqual(keys.decomposition_bit_count(), 59);
                keys.save(stream);
                test_keys.load(stream);
                Assert::AreEqual(keys.size(), test_keys.size());
                Assert::IsTrue(keys.hash_block() == test_keys.hash_block());
                Assert::AreEqual(keys.decomposition_bit_count(), test_keys.decomposition_bit_count());
                for (int j = 0; j < test_keys.size(); j++)
                {
                    for (int i = 0; i < test_keys.key(j + 2).size(); i++)
                    {
                        Assert::AreEqual(keys.key(j + 2)[i].size(), test_keys.key(j + 2)[i].size());
                        Assert::AreEqual(keys.key(j + 2)[i].uint64_count(), test_keys.key(j + 2)[i].uint64_count());
                        Assert::IsTrue(is_equal_uint_uint(keys.key(j + 2)[i].pointer(), test_keys.key(j + 2)[i].pointer(), keys.key(j + 2)[i].uint64_count()));
                    }
                }

                keygen.generate_evaluation_keys(60, 5, keys);
                Assert::AreEqual(keys.decomposition_bit_count(), 60);
                keys.save(stream);
                test_keys.load(stream);
                Assert::AreEqual(keys.size(), test_keys.size());
                Assert::IsTrue(keys.hash_block() == test_keys.hash_block());
                Assert::AreEqual(keys.decomposition_bit_count(), test_keys.decomposition_bit_count());
                for (int j = 0; j < test_keys.size(); j++)
                {
                    for (int i = 0; i < test_keys.key(j + 2).size(); i++)
                    {
                        Assert::AreEqual(keys.key(j + 2)[i].size(), test_keys.key(j + 2)[i].size());
                        Assert::AreEqual(keys.key(j + 2)[i].uint64_count(), test_keys.key(j + 2)[i].uint64_count());
                        Assert::IsTrue(is_equal_uint_uint(keys.key(j + 2)[i].pointer(), test_keys.key(j + 2)[i].pointer(), keys.key(j + 2)[i].uint64_count()));
                    }
                }
            }
            {
                EncryptionParameters parms;
                parms.set_noise_standard_deviation(3.19);
                parms.set_poly_modulus("1x^256 + 1");
                parms.set_plain_modulus(1 << 6);
                parms.set_coeff_modulus({ small_mods_60bit(0), small_mods_50bit(0) });
                SEALContext context(parms);
                KeyGenerator keygen(context);

                EvaluationKeys keys;
                EvaluationKeys test_keys;
                Assert::AreEqual(keys.decomposition_bit_count(), 0);
                keys.save(stream);
                test_keys.load(stream);
                Assert::AreEqual(keys.size(), test_keys.size());
                Assert::IsTrue(keys.hash_block() == test_keys.hash_block());
                Assert::AreEqual(keys.decomposition_bit_count(), test_keys.decomposition_bit_count());
                Assert::AreEqual(0, keys.size());

                keygen.generate_evaluation_keys(8, 1, keys);
                Assert::AreEqual(keys.decomposition_bit_count(), 8);
                keys.save(stream);
                test_keys.load(stream);
                Assert::AreEqual(keys.size(), test_keys.size());
                Assert::IsTrue(keys.hash_block() == test_keys.hash_block());
                Assert::AreEqual(keys.decomposition_bit_count(), test_keys.decomposition_bit_count());
                for (int j = 0; j < test_keys.size(); j++)
                {
                    for (int i = 0; i < test_keys.key(j + 2).size(); i++)
                    {
                        Assert::AreEqual(keys.key(j + 2)[i].size(), test_keys.key(j + 2)[i].size());
                        Assert::AreEqual(keys.key(j + 2)[i].uint64_count(), test_keys.key(j + 2)[i].uint64_count());
                        Assert::IsTrue(is_equal_uint_uint(keys.key(j + 2)[i].pointer(), test_keys.key(j + 2)[i].pointer(), keys.key(j + 2)[i].uint64_count()));
                    }
                }

                keygen.generate_evaluation_keys(8, 2, keys);
                Assert::AreEqual(keys.decomposition_bit_count(), 8);
                keys.save(stream);
                test_keys.load(stream);
                Assert::AreEqual(keys.size(), test_keys.size());
                Assert::IsTrue(keys.hash_block() == test_keys.hash_block());
                Assert::AreEqual(keys.decomposition_bit_count(), test_keys.decomposition_bit_count());
                for (int j = 0; j < test_keys.size(); j++)
                {
                    for (int i = 0; i < test_keys.key(j + 2).size(); i++)
                    {
                        Assert::AreEqual(keys.key(j + 2)[i].size(), test_keys.key(j + 2)[i].size());
                        Assert::AreEqual(keys.key(j + 2)[i].uint64_count(), test_keys.key(j + 2)[i].uint64_count());
                        Assert::IsTrue(is_equal_uint_uint(keys.key(j + 2)[i].pointer(), test_keys.key(j + 2)[i].pointer(), keys.key(j + 2)[i].uint64_count()));
                    }
                }

                keygen.generate_evaluation_keys(59, 2, keys);
                Assert::AreEqual(keys.decomposition_bit_count(), 59);
                keys.save(stream);
                test_keys.load(stream);
                Assert::AreEqual(keys.size(), test_keys.size());
                Assert::IsTrue(keys.hash_block() == test_keys.hash_block());
                Assert::AreEqual(keys.decomposition_bit_count(), test_keys.decomposition_bit_count());
                for (int j = 0; j < test_keys.size(); j++)
                {
                    for (int i = 0; i < test_keys.key(j + 2).size(); i++)
                    {
                        Assert::AreEqual(keys.key(j + 2)[i].size(), test_keys.key(j + 2)[i].size());
                        Assert::AreEqual(keys.key(j + 2)[i].uint64_count(), test_keys.key(j + 2)[i].uint64_count());
                        Assert::IsTrue(is_equal_uint_uint(keys.key(j + 2)[i].pointer(), test_keys.key(j + 2)[i].pointer(), keys.key(j + 2)[i].uint64_count()));
                    }
                }

                keygen.generate_evaluation_keys(60, 5, keys);
                Assert::AreEqual(keys.decomposition_bit_count(), 60);
                keys.save(stream);
                test_keys.load(stream);
                Assert::AreEqual(keys.size(), test_keys.size());
                Assert::IsTrue(keys.hash_block() == test_keys.hash_block());
                Assert::AreEqual(keys.decomposition_bit_count(), test_keys.decomposition_bit_count());
                for (int j = 0; j < test_keys.size(); j++)
                {
                    for (int i = 0; i < test_keys.key(j + 2).size(); i++)
                    {
                        Assert::AreEqual(keys.key(j + 2)[i].size(), test_keys.key(j + 2)[i].size());
                        Assert::AreEqual(keys.key(j + 2)[i].uint64_count(), test_keys.key(j + 2)[i].uint64_count());
                        Assert::IsTrue(is_equal_uint_uint(keys.key(j + 2)[i].pointer(), test_keys.key(j + 2)[i].pointer(), keys.key(j + 2)[i].uint64_count()));
                    }
                }
            }
        }
    };
}