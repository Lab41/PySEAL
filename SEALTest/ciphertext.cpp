#include "CppUnitTest.h"
#include "seal/ciphertext.h"
#include "seal/context.h"
#include "seal/keygenerator.h"
#include "seal/encryptor.h"
#include "seal/memorypoolhandle.h"
#include "seal/defaultparams.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal;
using namespace seal::util;
using namespace std;

namespace SEALTest
{
    TEST_CLASS(CiphertextTest)
    {
    public:
        TEST_METHOD(CiphertextBasics)
        {
            EncryptionParameters parms;

            parms.set_poly_modulus("1x^2 + 1");
            parms.set_coeff_modulus({ 2 });
            parms.set_plain_modulus(2);
            parms.set_noise_standard_deviation(1.0);

            Ciphertext ctxt(parms);
            ctxt.reserve(10);
            Assert::AreEqual(2, ctxt.size());
            Assert::AreEqual(2 * 3 * 1, ctxt.uint64_count());
            Assert::AreEqual(10 * 3 * 1, ctxt.uint64_count_capacity());
            Assert::IsTrue(ctxt.hash_block() == parms.hash_block());
            const uint64_t *ptr = ctxt.pointer();

            ctxt.reserve(5);
            Assert::AreEqual(2, ctxt.size());
            Assert::AreEqual(2 * 3 * 1, ctxt.uint64_count());
            Assert::AreEqual(5 * 3 * 1, ctxt.uint64_count_capacity());
            Assert::IsTrue(ptr != ctxt.pointer());
            Assert::IsTrue(ctxt.hash_block() == parms.hash_block());
            ptr = ctxt.pointer();

            ctxt.reserve(10);
            Assert::AreEqual(2, ctxt.size());
            Assert::AreEqual(2 * 3 * 1, ctxt.uint64_count());
            Assert::AreEqual(10 * 3 * 1, ctxt.uint64_count_capacity());
            Assert::IsTrue(ptr != ctxt.pointer());
            Assert::IsTrue(ctxt.hash_block() == parms.hash_block());
            ptr = ctxt.pointer();

            ctxt.reserve(2);
            Assert::AreEqual(2, ctxt.size());
            Assert::AreEqual(2 * 3 * 1, ctxt.uint64_count_capacity());
            Assert::AreEqual(2 * 3 * 1, ctxt.uint64_count());
            Assert::IsTrue(ptr != ctxt.pointer());
            Assert::IsTrue(ctxt.hash_block() == parms.hash_block());
            ptr = ctxt.pointer();

            ctxt.reserve(5);
            Assert::AreEqual(2, ctxt.size());
            Assert::AreEqual(5 * 3 * 1, ctxt.uint64_count_capacity());
            Assert::AreEqual(2 * 3 * 1, ctxt.uint64_count());
            Assert::IsTrue(ptr != ctxt.pointer());
            Assert::IsTrue(ctxt.hash_block() == parms.hash_block());
            ptr = ctxt.pointer();

            MemoryPoolHandle pool = MemoryPoolHandle::Global();
            parms.set_coeff_modulus({ 2, 3, 5 });
            Pointer ctxt_alloc(allocate_uint(20 * 3 * 3, pool));
            ctxt.alias(parms, 20, 2, ctxt_alloc.get());
            ctxt_alloc[0] = 1;
            ctxt_alloc[1] = 2;
            Assert::AreEqual(2, ctxt.size());
            Assert::AreEqual(20 * 3 * 3, ctxt.uint64_count_capacity());
            Assert::AreEqual(2 * 3 * 3, ctxt.uint64_count());
            Assert::IsTrue(ctxt_alloc.get() == ctxt.pointer());
            Assert::IsTrue(ctxt.is_alias());
            Assert::IsTrue(ctxt.hash_block() == parms.hash_block());

            ctxt.unalias();
            Assert::AreEqual(2, ctxt.size());
            Assert::AreEqual(2 * 3 * 3, ctxt.uint64_count());
            Assert::AreEqual(20 * 3 * 3, ctxt.uint64_count_capacity());
            Assert::IsTrue(ctxt_alloc.get() != ctxt.pointer());
            Assert::AreEqual(1ULL, ctxt[0]);
            Assert::AreEqual(2ULL, ctxt[1]);
            Assert::IsTrue(ctxt.hash_block() == parms.hash_block());
        }

        TEST_METHOD(SaveLoadCiphertext)
        {
            stringstream stream;
            EncryptionParameters parms;
            parms.set_poly_modulus("1x^2 + 1");
            parms.set_coeff_modulus({ 2 });
            parms.set_plain_modulus(2);
            parms.set_noise_standard_deviation(1.0);

            Ciphertext ctxt(parms); 
            Ciphertext ctxt2;
            ctxt.save(stream);
            ctxt2.load(stream);
            Assert::IsTrue(ctxt.hash_block() == ctxt2.hash_block());
            
            MemoryPoolHandle pool = MemoryPoolHandle::Global();
            Pointer ctxt_alloc(allocate_zero_uint(parms.poly_modulus().coeff_count() * parms.coeff_modulus().size() * 3, pool));
            ctxt_alloc[0] = 1;
            ctxt_alloc[1] = 1;
            ctxt_alloc[2] = 0;
            ctxt_alloc[3] = 1;
            ctxt_alloc[4] = 1;
            ctxt_alloc[5] = 0;
            ctxt.alias(parms, 3, 2, ctxt_alloc.get());
            ctxt.save(stream);
            ctxt2.load(stream);
            Assert::IsTrue(ctxt.hash_block() == ctxt2.hash_block());
            Assert::IsTrue(is_equal_uint_uint(ctxt.pointer(), ctxt2.pointer(), parms.poly_modulus().coeff_count() * parms.coeff_modulus().size() * 2));
            Assert::IsTrue(ctxt.pointer() != ctxt2.pointer());

            ctxt.unalias();
            parms.set_poly_modulus("1x^1024 + 1");
            parms.set_coeff_modulus(coeff_modulus_128(1024));
            parms.set_plain_modulus(0xF0F0);
            parms.set_noise_standard_deviation(3.14159);
            SEALContext context(parms);
            KeyGenerator keygen(context);
            Encryptor encryptor(context, keygen.public_key());
            encryptor.encrypt(Plaintext("Ax^10 + 9x^9 + 8x^8 + 7x^7 + 6x^6 + 5x^5 + 4x^4 + 3x^3 + 2x^2 + 1"), ctxt);
            ctxt.save(stream);
            ctxt2.load(stream);
            Assert::IsTrue(ctxt.hash_block() == ctxt2.hash_block());
            Assert::IsTrue(is_equal_uint_uint(ctxt.pointer(), ctxt2.pointer(), parms.poly_modulus().coeff_count() * parms.coeff_modulus().size() * 2));
            Assert::IsTrue(ctxt.pointer() != ctxt2.pointer());
        }
    };
}