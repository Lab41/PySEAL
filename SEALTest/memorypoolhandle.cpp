#include "CppUnitTest.h"
#include "memorypoolhandle.h"
#include "evaluator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal;
using namespace seal::util;
using namespace std;

namespace SEALTest
{
    TEST_CLASS(MPHTests)
    {
    public:
        TEST_METHOD(MemoryPoolHandleTest)
        {
            MemoryPoolHandle pool;
            Assert::IsTrue(&static_cast<MemoryPool&>(pool) == MemoryPool::default_pool().get());
            pool = MemoryPoolHandle::acquire_global();
            Assert::IsTrue(&static_cast<MemoryPool&>(pool) == MemoryPool::default_pool().get());
            pool = MemoryPoolHandle::acquire_new();
            Assert::IsFalse(&pool.operator seal::util::MemoryPool &() == MemoryPool::default_pool().get());
            
            EncryptionParameters parms;
            BigUInt coeff_modulus;
            BigUInt plain_modulus;
            BigPoly poly_modulus;
            parms.set_decomposition_bit_count(4);
            parms.set_noise_standard_deviation(3.19);
            parms.set_noise_max_deviation(35.06);
            coeff_modulus.resize(48);
            coeff_modulus = "7FFFFC801";
            plain_modulus.resize(7);
            plain_modulus = 1 << 6;
            poly_modulus.resize(65, 1);
            poly_modulus[0] = 1;
            poly_modulus[64] = 1;
            parms.set_poly_modulus(poly_modulus);
            parms.set_plain_modulus(plain_modulus);
            parms.set_coeff_modulus(coeff_modulus);
            parms.validate();
            Evaluator evaluator(parms, pool);
            MemoryPoolHandle pool2(pool);
            Evaluator evaluator2(parms, pool2);
            Evaluator evaluator3(parms, MemoryPoolHandle::acquire_new());
            MemoryPoolHandle pool4;
            pool4 = pool2;
            Evaluator evaluator4(parms, pool4);
            Assert::IsTrue(&static_cast<MemoryPool&>(pool4) == &static_cast<MemoryPool&>(pool2));
            Assert::IsTrue(&static_cast<MemoryPool&>(pool4) == &static_cast<MemoryPool&>(pool));
        }
    };
}