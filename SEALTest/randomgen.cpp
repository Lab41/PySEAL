#include "CppUnitTest.h"
#include "randomgen.h"
#include "keygenerator.h"
#include <random>
#include <cstdint>
#include <memory>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal;
using namespace std;

namespace SEALTest
{
    namespace
    {
        class CustomRandomEngine : public UniformRandomGenerator
        {
        public:
            CustomRandomEngine()
            {
            }

            uint32_t generate() override
            {
                count_++;
                return engine_();
            }

            static int count()
            {
                return count_;
            }

        private:
            default_random_engine engine_;

            static int count_;
        };

        class CustomRandomEngineFactory : public UniformRandomGeneratorFactory
        {
        public:
            UniformRandomGenerator *create() override
            {
                return new CustomRandomEngine();
            }
        };

        int CustomRandomEngine::count_ = 0;
    }

    TEST_CLASS(RandomGenerator)
    {
    public:
        TEST_METHOD(UniformRandomCreateDefault)
        {
            unique_ptr<UniformRandomGenerator> generator(UniformRandomGeneratorFactory::default_factory()->create());
            bool lower_half = false;
            bool upper_half = false;
            bool even = false;
            bool odd = false;
            for (int i = 0; i < 10; ++i)
            {
                uint32_t value = generator->generate();
                if (value < UINT32_MAX / 2)
                {
                    lower_half = true;
                }
                else
                {
                    upper_half = true;
                }
                if ((value % 2) == 0)
                {
                    even = true;
                }
                else
                {
                    odd = true;
                }
            }
            Assert::IsTrue(lower_half);
            Assert::IsTrue(upper_half);
            Assert::IsTrue(even);
            Assert::IsTrue(odd);
        }

        TEST_METHOD(StandardRandomAdapterGenerate)
        {
            StandardRandomAdapter<default_random_engine> generator;
            generator.generator().seed(0);
            bool lower_half = false;
            bool upper_half = false;
            bool even = false;
            bool odd = false;
            for (int i = 0; i < 10; ++i)
            {
                uint32_t value = generator.generate();
                if (value < UINT32_MAX / 2)
                {
                    lower_half = true;
                }
                else
                {
                    upper_half = true;
                }
                if ((value % 2) == 0)
                {
                    even = true;
                }
                else
                {
                    odd = true;
                }
            }
            Assert::IsTrue(lower_half);
            Assert::IsTrue(upper_half);
            Assert::IsTrue(even);
            Assert::IsTrue(odd);
        }

        TEST_METHOD(CustomRandomGenerator)
        {
            CustomRandomEngineFactory factory;
            
            EncryptionParameters parms;
            BigUInt coeff_modulus;
            BigUInt plain_modulus;
            BigPoly poly_modulus;
            parms.set_decomposition_bit_count(4);
            parms.set_noise_standard_deviation(3.19);
            parms.set_noise_max_deviation(35.06);
            coeff_modulus.resize(48);
            coeff_modulus = "FFFFFFFFC001";
            plain_modulus.resize(7);
            plain_modulus = 1 << 6;
            poly_modulus.resize(65, 1);
            poly_modulus[0] = 1;
            poly_modulus[64] = 1;
            parms.set_poly_modulus(const_cast<const BigPoly &>(poly_modulus));
            parms.set_plain_modulus(const_cast<const BigUInt &>(plain_modulus));
            parms.set_coeff_modulus(const_cast<const BigUInt &>(coeff_modulus));
            parms.set_random_generator(&factory);
            parms.validate();

            Assert::AreEqual(0, CustomRandomEngine::count());

            KeyGenerator keygen(parms);
            keygen.generate();

            Assert::AreNotEqual(0, CustomRandomEngine::count());
        }
    };
}
