#include "CppUnitTest.h"
#include "seal/polycrt.h"
#include "seal/context.h"
#include "seal/keygenerator.h"
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal;
using namespace seal::util;
using namespace std;

namespace SEALTest
{
    TEST_CLASS(PolyCRTBuilderTest)
    {
    public:
        TEST_METHOD(BatchUnbatchUIntVector)
        {
            EncryptionParameters parms;
            parms.set_poly_modulus("1x^64 + 1");
            parms.set_coeff_modulus({ small_mods_60bit(0) });
            parms.set_plain_modulus(257);

            SEALContext context(parms);
            Assert::IsTrue(context.qualifiers().enable_batching);

            PolyCRTBuilder crtbuilder(context);
            Assert::AreEqual(64, crtbuilder.slot_count());
            vector<uint64_t> plain_vec;
            for (int i = 0; i < crtbuilder.slot_count(); i++)
            {
                plain_vec.push_back(i);
            }

            Plaintext plain;
            crtbuilder.compose(plain_vec, plain);
            vector<uint64_t> plain_vec2;
            crtbuilder.decompose(plain, plain_vec2);
            Assert::IsTrue(plain_vec == plain_vec2);

            for (int i = 0; i < crtbuilder.slot_count(); i++)
            {
                plain_vec[i] = 5;
            }
            crtbuilder.compose(plain_vec, plain);
            Assert::IsTrue(plain.to_string() == "5");
            crtbuilder.decompose(plain, plain_vec2);
            Assert::IsTrue(plain_vec == plain_vec2);

            vector<uint64_t> short_plain_vec;
            for (int i = 0; i < 20; i++)
            {
                short_plain_vec.push_back(i);
            }
            crtbuilder.compose(short_plain_vec, plain);
            vector<uint64_t> short_plain_vec2;
            crtbuilder.decompose(plain, short_plain_vec2);
            Assert::AreEqual(20ULL, short_plain_vec.size());
            Assert::AreEqual(64ULL, short_plain_vec2.size());
            for (int i = 0; i < 20; i++)
            {
                Assert::AreEqual(short_plain_vec[i], short_plain_vec2[i]);
            }
            for (int i = 20; i < crtbuilder.slot_count(); i++)
            {
                Assert::AreEqual(0ULL, short_plain_vec2[i]);
            }
        }

        TEST_METHOD(BatchUnbatchIntVector)
        {
            EncryptionParameters parms;
            parms.set_poly_modulus("1x^64 + 1");
            parms.set_coeff_modulus({ small_mods_60bit(0) });
            parms.set_plain_modulus(257);

            SEALContext context(parms);
            Assert::IsTrue(context.qualifiers().enable_batching);

            PolyCRTBuilder crtbuilder(context);
            Assert::AreEqual(64, crtbuilder.slot_count());
            vector<int64_t> plain_vec;
            for (int i = 0; i < crtbuilder.slot_count(); i++)
            {
                plain_vec.push_back(i * (1 - 2 * (i % 2)));
            }

            Plaintext plain;
            crtbuilder.compose(plain_vec, plain);
            vector<int64_t> plain_vec2;
            crtbuilder.decompose(plain, plain_vec2);
            Assert::IsTrue(plain_vec == plain_vec2);

            for (int i = 0; i < crtbuilder.slot_count(); i++)
            {
                plain_vec[i] = -5;
            }
            crtbuilder.compose(plain_vec, plain);
            Assert::IsTrue(plain.to_string() == "FC");
            crtbuilder.decompose(plain, plain_vec2);
            Assert::IsTrue(plain_vec == plain_vec2);

            vector<int64_t> short_plain_vec;
            for (int i = 0; i < 20; i++)
            {
                short_plain_vec.push_back(i * (1 - 2 * (i % 2)));
            }
            crtbuilder.compose(short_plain_vec, plain);
            vector<int64_t> short_plain_vec2;
            crtbuilder.decompose(plain, short_plain_vec2);
            Assert::AreEqual(20ULL, short_plain_vec.size());
            Assert::AreEqual(64ULL, short_plain_vec2.size());
            for (int i = 0; i < 20; i++)
            {
                Assert::IsTrue(short_plain_vec[i] == short_plain_vec2[i]);
            }
            for (int i = 20; i < crtbuilder.slot_count(); i++)
            {
                Assert::IsTrue(0LL == short_plain_vec2[i]);
            }
        }

        TEST_METHOD(BatchUnbatchPlaintext)
        {
            EncryptionParameters parms;
            parms.set_poly_modulus("1x^64 + 1");
            parms.set_coeff_modulus({ small_mods_60bit(0) });
            parms.set_plain_modulus(257);

            SEALContext context(parms);
            Assert::IsTrue(context.qualifiers().enable_batching);

            PolyCRTBuilder crtbuilder(context);
            Assert::AreEqual(64, crtbuilder.slot_count());
            Plaintext plain(crtbuilder.slot_count());
            for (int i = 0; i < crtbuilder.slot_count(); i++)
            {
                plain[i] = i;
            }

            crtbuilder.compose(plain);
            crtbuilder.decompose(plain);
            for (int i = 0; i < crtbuilder.slot_count(); i++)
            {
                Assert::IsTrue(plain[i] == i);
            }

            for (int i = 0; i < crtbuilder.slot_count(); i++)
            {
                plain[i] = 5;
            }
            crtbuilder.compose(plain);
            Assert::IsTrue(plain.to_string() == "5");
            crtbuilder.decompose(plain);
            for (int i = 0; i < crtbuilder.slot_count(); i++)
            {
                Assert::AreEqual(5ULL, plain[i]);
            }

            Plaintext short_plain(20);
            for (int i = 0; i < 20; i++)
            {
                short_plain[i] = i;
            }
            crtbuilder.compose(short_plain);
            crtbuilder.decompose(short_plain);
            for (int i = 0; i < 20; i++)
            {
                Assert::IsTrue(short_plain[i] == i);
            }
            for (int i = 20; i < crtbuilder.slot_count(); i++)
            {
                Assert::IsTrue(short_plain[i] == 0);
            }
        }
    };
}