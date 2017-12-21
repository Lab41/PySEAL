#include "CppUnitTest.h"
#include "seal/encryptionparams.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal;
using namespace std;

namespace SEALTest
{
    TEST_CLASS(EncryptionParametersTest)
    {
    public:
        TEST_METHOD(EncryptionParametersSet)
        {
            EncryptionParameters parms;
            parms.set_noise_standard_deviation(0);
            parms.set_coeff_modulus({ });
            parms.set_plain_modulus(0);
            parms.set_poly_modulus("0");
            parms.set_random_generator(UniformRandomGeneratorFactory::default_factory());

            Assert::AreEqual(0.0, parms.noise_standard_deviation());
            Assert::AreEqual(0.0, parms.noise_max_deviation());
            Assert::IsTrue(parms.coeff_modulus().size() == 0);
            Assert::IsTrue(parms.plain_modulus() == 0);
            Assert::IsTrue(parms.poly_modulus().to_string() == "0");
            Assert::IsTrue(parms.random_generator() == UniformRandomGeneratorFactory::default_factory());
        
            parms.set_noise_standard_deviation(0);
            parms.set_coeff_modulus({ 2 });
            parms.set_plain_modulus(2);
            parms.set_poly_modulus("1");
            parms.set_random_generator(UniformRandomGeneratorFactory::default_factory());

            Assert::AreEqual(0.0, parms.noise_standard_deviation());
            Assert::AreEqual(0.0, parms.noise_max_deviation());
            Assert::IsTrue(parms.coeff_modulus()[0] == 2);
            Assert::IsTrue(parms.plain_modulus() == 2);
            Assert::IsTrue(parms.poly_modulus().to_string() == "1");
            Assert::IsTrue(parms.random_generator() == UniformRandomGeneratorFactory::default_factory());
        
            parms.set_noise_standard_deviation(3.19);
            parms.set_coeff_modulus({ 2, 3});
            parms.set_plain_modulus(2);
            parms.set_poly_modulus("1x^2 + 1");
            parms.set_random_generator(UniformRandomGeneratorFactory::default_factory());

            Assert::AreEqual(3.19, parms.noise_standard_deviation());
            Assert::AreEqual(3.19 * 6, parms.noise_max_deviation());
            Assert::IsTrue(parms.coeff_modulus()[0] == 2);
            Assert::IsTrue(parms.coeff_modulus()[1] == 3);
            Assert::IsTrue(parms.plain_modulus() == 2);
            Assert::IsTrue(parms.poly_modulus().to_string() == "1x^2 + 1");
            Assert::IsTrue(parms.random_generator() == UniformRandomGeneratorFactory::default_factory());

            parms.set_noise_standard_deviation(3.19);
            parms.set_coeff_modulus({ 
                small_mods_30bit(0), small_mods_40bit(0), small_mods_50bit(0)
            });
            parms.set_plain_modulus(2);
            parms.set_poly_modulus("1x^128 + 1");
            parms.set_random_generator(UniformRandomGeneratorFactory::default_factory());

            Assert::AreEqual(3.19, parms.noise_standard_deviation());
            Assert::AreEqual(3.19 * 6, parms.noise_max_deviation());
            Assert::IsTrue(parms.coeff_modulus()[0] == small_mods_30bit(0));
            Assert::IsTrue(parms.coeff_modulus()[1] == small_mods_40bit(0));
            Assert::IsTrue(parms.coeff_modulus()[2] == small_mods_50bit(0));
            Assert::IsTrue(parms.plain_modulus() == 2);
            Assert::IsTrue(parms.poly_modulus().to_string() == "1x^128 + 1");
            Assert::IsTrue(parms.random_generator() == UniformRandomGeneratorFactory::default_factory());
        }

        TEST_METHOD(EncryptionParametersCompare)
        {
            EncryptionParameters parms1;
            parms1.set_noise_standard_deviation(3.19);
            parms1.set_coeff_modulus({ small_mods_30bit(0) });
            parms1.set_plain_modulus(1 << 6);
            parms1.set_poly_modulus("1x^64 + 1");
            parms1.set_random_generator(UniformRandomGeneratorFactory::default_factory());

            EncryptionParameters parms2(parms1);
            Assert::IsTrue(parms1 == parms2);
            
            EncryptionParameters parms3;
            parms3 = parms2;
            Assert::IsTrue(parms3 == parms2);
            parms3.set_coeff_modulus({ small_mods_30bit(1) });
            Assert::IsFalse(parms3 == parms2);

            parms3 = parms2;
            Assert::IsTrue(parms3 == parms2);
            parms3.set_coeff_modulus({ 
                small_mods_30bit(0), small_mods_30bit(1) 
            });
            Assert::IsFalse(parms3 == parms2);

            parms3 = parms2;
            parms3.set_poly_modulus("1x^128 + 1");
            Assert::IsFalse(parms3 == parms1);

            parms3 = parms2;
            parms3.set_plain_modulus((1 << 6) + 1);
            Assert::IsFalse(parms3 == parms2);

            parms3 = parms2;
            parms3.set_noise_standard_deviation(3.18);
            Assert::IsFalse(parms3 == parms2);

            parms3 = parms2;
            parms3.set_random_generator(nullptr);
            Assert::IsTrue(parms3 == parms2);

            parms3 = parms2;
            parms3.set_poly_modulus("1");
            parms3.set_poly_modulus("1x^128 + 1");
            parms3.set_poly_modulus("1x^64 + 1");
            Assert::IsTrue(parms3 == parms1);

            parms3 = parms2;
            parms3.set_coeff_modulus({ 2 });
            parms3.set_coeff_modulus({ small_mods_50bit(0) });
            parms3.set_coeff_modulus(parms2.coeff_modulus());
            Assert::IsTrue(parms3 == parms2);
        }

        TEST_METHOD(EncryptionParametersSaveLoad)
        {
            stringstream stream;

            EncryptionParameters parms;
            EncryptionParameters parms2;
            parms.save(stream);
            parms2.load(stream);
            Assert::AreEqual(parms.noise_standard_deviation(), parms2.noise_standard_deviation());
            Assert::AreEqual(parms.noise_max_deviation(), parms2.noise_max_deviation());
            Assert::IsTrue(parms.coeff_modulus() == parms2.coeff_modulus());
            Assert::IsTrue(parms.plain_modulus() == parms2.plain_modulus());
            Assert::IsTrue(parms.poly_modulus() == parms2.poly_modulus());
            Assert::IsTrue(parms == parms2);

            parms.set_noise_standard_deviation(3.19);
            parms.set_coeff_modulus({ small_mods_30bit(0) });
            parms.set_plain_modulus(1 << 6);
            parms.set_poly_modulus("1x^64 + 1");
            parms.save(stream);
            parms2.load(stream);
            Assert::AreEqual(parms.noise_standard_deviation(), parms2.noise_standard_deviation());
            Assert::AreEqual(parms.noise_max_deviation(), parms2.noise_max_deviation());
            Assert::IsTrue(parms.coeff_modulus() == parms2.coeff_modulus());
            Assert::IsTrue(parms.plain_modulus() == parms2.plain_modulus());
            Assert::IsTrue(parms.poly_modulus() == parms2.poly_modulus());
            Assert::IsTrue(parms == parms2);

            parms.set_noise_standard_deviation(3.19);
            parms.set_coeff_modulus({ 
                small_mods_30bit(0), small_mods_60bit(0), small_mods_60bit(1) 
            });
            parms.set_plain_modulus(1 << 30);
            parms.set_poly_modulus("1x^256 + 1");
            parms.save(stream);
            parms2.load(stream);
            Assert::AreEqual(parms.noise_standard_deviation(), parms2.noise_standard_deviation());
            Assert::AreEqual(parms.noise_max_deviation(), parms2.noise_max_deviation());
            Assert::IsTrue(parms.coeff_modulus() == parms2.coeff_modulus());
            Assert::IsTrue(parms.plain_modulus() == parms2.plain_modulus());
            Assert::IsTrue(parms.poly_modulus() == parms2.poly_modulus());
            Assert::IsTrue(parms == parms2);
        }
    };
}