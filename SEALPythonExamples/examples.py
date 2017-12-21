import seal
from seal import ChooserEvaluator,     \
                 Ciphertext,           \
                 Decryptor,            \
                 Encryptor,            \
                 EncryptionParameters, \
                 Evaluator,            \
                 IntegerEncoder,       \
                 KeyGenerator,         \
                 MemoryPoolHandle,     \
                 Plaintext, \
                 SEALContext, \
                 EvaluationKeys

def example_basics_i():
    print_example_banner("Example: Basics I");

    # In this example we demonstrate setting up encryption parameters and other
    # relevant objects for performing simple computations on encrypted integers.

    # SEAL uses the Fan-Vercauteren (FV) homomorphic encryption scheme. We refer to
    # https://eprint.iacr.org/2012/144 for full details on how the FV scheme works.
    # For better performance, SEAL implements the "FullRNS" optimization of FV, as
    # described in https://eprint.iacr.org/2016/510.

    # The first task is to set up an instance of the EncryptionParameters class.
    # It is critical to understand how these different parameters behave, how they
    # affect the encryption scheme, performance, and the security level. There are
    # three encryption parameters that are necessary to set:

    #     - poly_modulus (polynomial modulus);
    #     - coeff_modulus ([ciphertext] coefficient modulus);
    #     - plain_modulus (plaintext modulus).

    # A fourth parameter -- noise_standard_deviation -- has a default value of 3.19
    # and should not be necessary to modify unless the user has a specific reason
    # to and knows what they are doing.

    # The encryption scheme implemented in SEAL cannot perform arbitrary computations
    # on encrypted data. Instead, each ciphertext has a specific quantity called the
    # `invariant noise budget' -- or `noise budget' for short -- measured in bits.
    # The noise budget of a freshly encrypted ciphertext (initial noise budget) is
    # determined by the encryption parameters. Homomorphic operations consume the
    # noise budget at a rate also determined by the encryption parameters. In SEAL
    # the two basic homomorphic operations are additions and multiplications, of
    # which additions can generally be thought of as being nearly free in terms of
    # noise budget consumption compared to multiplications. Since noise budget
    # consumption is compounding in sequential multiplications, the most significant
    # factor in choosing appropriate encryption parameters is the multiplicative
    # depth of the arithmetic circuit that needs to be evaluated. Once the noise
    # budget in a ciphertext reaches zero, it becomes too corrupted to be decrypted.
    # Thus, it is essential to choose the parameters to be large enough to support
    # the desired computation; otherwise the result is impossible to make sense of
    # even with the secret key.
    parms = EncryptionParameters()

    # We first set the polynomial modulus. This must be a power-of-2 cyclotomic
    # polynomial, i.e. a polynomial of the form "1x^(power-of-2) + 1". The polynomial
    # modulus should be thought of mainly affecting the security level of the scheme;
    # larger polynomial modulus makes the scheme more secure. At the same time, it
    # makes ciphertext sizes larger, and consequently all operations slower.
    # Recommended degrees for poly_modulus are 1024, 2048, 4096, 8192, 16384, 32768,
    # but it is also possible to go beyond this. Since we perform only a very small
    # computation in this example, it suffices to use a very small polynomial modulus
    parms.set_poly_modulus("1x^2048 + 1")

    # Next we choose the [ciphertext] coefficient modulus (coeff_modulus). The size
    # of the coefficient modulus should be thought of as the most significant factor
    # in determining the noise budget in a freshly encrypted ciphertext: bigger means
    # more noise budget. Unfortunately, a larger coefficient modulus also lowers the
    # security level of the scheme. Thus, if a large noise budget is required for
    # complicated computations, a large coefficient modulus needs to be used, and the
    # reduction in the security level must be countered by simultaneously increasing
    # the polynomial modulus.

    # To make parameter selection easier for the user, we have constructed sets of
    # largest allowed coefficient moduli for 128-bit and 192-bit security levels
    # for different choices of the polynomial modulus. These recommended parameters
    # follow the Security white paper at http://HomomorphicEncryption.org. However,
    # due to the complexity of this topic, we highly recommend the user to directly
    # consult an expert in homomorphic encryption and RLWE-based encryption schemes
    # to determine the security of their parameter choices.

    # Our recommended values for the coefficient modulus can be easily accessed
    # through the functions

    #     coeff_modulus_128bit(int)
    #     coeff_modulus_192bit(int)

    # for 128-bit and 192-bit security levels. The integer parameter is the degree
    # of the polynomial modulus.

    # In SEAL the coefficient modulus is a positive composite number -- a product
    # of distinct primes of size up to 60 bits. When we talk about the size of the
    # coefficient modulus we mean the bit length of the product of the small primes.
    # The small primes are represented by instances of the SmallModulus class; for
    # example coeff_modulus_128bit(int) returns a vector of SmallModulus instances.

    # It is possible for the user to select their own small primes. Since SEAL uses
    # the Number Theoretic Transform (NTT) for polynomial multiplications modulo the
    # factors of the coefficient modulus, the factors need to be prime numbers
    # congruent to 1 modulo 2*degree(poly_modulus). We have generated a list of such
    # prime numbers of various sizes, that the user can easily access through the
    # functions

    #     small_mods_60bit(int)
    #     small_mods_50bit(int)
    #     small_mods_40bit(int)
    #     small_mods_30bit(int)

    # each of which gives access to an array of primes of the denoted size. These
    # primes are located in the source file util/globals.cpp.

    # Performance is mainly affected by the size of the polynomial modulus, and the
    # number of prime factors in the coefficient modulus. Thus, it is important to
    # use as few factors in the coefficient modulus as possible.

    # In this example we use the default coefficient modulus for a 128-bit security
    # level. Concretely, this coefficient modulus consists of only one 56-bit prime
    # factor: 0xfffffffff00001.
    parms.set_coeff_modulus(seal.coeff_modulus_128(2048))

    # The plaintext modulus can be any positive integer, even though here we take
    # it to be a power of two. In fact, in many cases one might instead want it to
    # be a prime number; we will see this in example_batching(). The plaintext
    # modulus determines the size of the plaintext data type, but it also affects
    # the noise budget in a freshly encrypted ciphertext, and the consumption of
    # the noise budget in homomorphic multiplication. Thus, it is essential to try
    # to keep the plaintext data type as small as possible for good performance.
    # The noise budget in a freshly encrypted ciphertext is

    #     ~ log2(coeff_modulus/plain_modulus) (bits)

    # and the noise budget consumption in a homomorphic multiplication is of the
    # form log2(plain_modulus) + (other terms).
    parms.set_plain_modulus(1 << 8)

    # Now that all parameters are set, we are ready to construct a SEALContext
    # object. This is a heavy class that checks the validity and properties of
    # the parameters we just set, and performs and stores several important
    # pre-computations.
    context = SEALContext(parms)

    # Print the parameters that we have chosen
    print_parameters(context);

    # Plaintexts in the FV scheme are polynomials with coefficients integers modulo
    # plain_modulus. To encrypt for example integers instead, one can use an
    # `encoding scheme' to represent the integers as such polynomials. SEAL comes
    # with a few basic encoders:

    # [IntegerEncoder]
    # Given an integer base b, encodes integers as plaintext polynomials as follows.
    # First, a base-b expansion of the integer is computed. This expansion uses
    # a `balanced' set of representatives of integers modulo b as the coefficients.
    # Namely, when b is odd the coefficients are integers between -(b-1)/2 and
    # (b-1)/2. When b is even, the integers are between -b/2 and (b-1)/2, except
    # when b is two and the usual binary expansion is used (coefficients 0 and 1).
    # Decoding amounts to evaluating the polynomial at x=b. For example, if b=2,
    # the integer

    #     26 = 2^4 + 2^3 + 2^1

    # is encoded as the polynomial 1x^4 + 1x^3 + 1x^1. When b=3,

    #     26 = 3^3 - 3^0

    # is encoded as the polynomial 1x^3 - 1. In memory polynomial coefficients are
    # always stored as unsigned integers by storing their smallest non-negative
    # representatives modulo plain_modulus. To create a base-b integer encoder,
    # use the constructor IntegerEncoder(plain_modulus, b). If no b is given, b=2
    # is used.

    # [FractionalEncoder]
    # The FractionalEncoder encodes fixed-precision rational numbers as follows.
    # It expands the number in a given base b, possibly truncating an infinite
    # fractional part to finite precision, e.g.

    #     26.75 = 2^4 + 2^3 + 2^1 + 2^(-1) + 2^(-2)

    # when b=2. For the sake of the example, suppose poly_modulus is 1x^1024 + 1.
    # It then represents the integer part of the number in the same way as in
    # IntegerEncoder (with b=2 here), and moves the fractional part instead to the
    # highest degree part of the polynomial, but with signs of the coefficients
    # changed. In this example we would represent 26.75 as the polynomial

    #     -1x^1023 - 1x^1022 + 1x^4 + 1x^3 + 1x^1.

    # In memory the negative coefficients of the polynomial will be represented as
    # their negatives modulo plain_modulus.

    # [PolyCRTBuilder]
    # If plain_modulus is a prime congruent to 1 modulo 2*degree(poly_modulus), the
    # plaintext elements can be viewed as 2-by-(degree(poly_modulus) / 2) matrices
    # with elements integers modulo plain_modulus. When a desired computation can be
    # vectorized, using PolyCRTBuilder can result in massive performance improvements
    # over naively encrypting and operating on each input number separately. Thus,
    # in more complicated computations this is likely to be by far the most important
    # and useful encoder. In example_batching() we show how to use and operate on
    # encrypted matrix plaintexts.

    # For performance reasons, in homomorphic encryption one typically wants to keep
    # the plaintext data types as small as possible, which can make it challenging to
    # prevent data type overflow in more complicated computations, especially when
    # operating on rational numbers that have been scaled to integers. When using
    # PolyCRTBuilder estimating whether an overflow occurs is a fairly standard task,
    # as the matrix slots are integers modulo plain_modulus, and each slot is operated
    # on independently of the others. When using IntegerEncoder or FractionalEncoder
    # it is substantially more difficult to estimate when an overflow occurs in the
    # plaintext, and choosing the plaintext modulus very carefully to be large enough
    # is critical to avoid unexpected results. Specifically, one needs to estimate how
    # large the largest coefficient in  the polynomial view of all of the plaintext
    # elements becomes, and choose the plaintext modulus to be larger than this value.
    # SEAL comes with an automatic parameter selection tool that can help with this
    # task, as is demonstrated in example_parameter_selection().

    # Here we choose to create an IntegerEncoder with base b=2.
    encoder = IntegerEncoder(context.plain_modulus())

    # We are now ready to generate the secret and public keys. For this purpose we need
    # an instance of the KeyGenerator class. Constructing a KeyGenerator automatically
    # generates the public and secret key, which can then be read to local variables.
    # To create a fresh pair of keys one can call KeyGenerator::generate() at any time.
    keygen = KeyGenerator(context)
    public_key = keygen.public_key()
    secret_key = keygen.secret_key()

    # To be able to encrypt, we need to construct an instance of Encryptor. Note that
    # the Encryptor only requires the public key.
    encryptor = Encryptor(context, public_key)

    # Computations on the ciphertexts are performed with the Evaluator class.
    evaluator = Evaluator(context)

    # We will of course want to decrypt our results to verify that everything worked,
    # so we need to also construct an instance of Decryptor. Note that the Decryptor
    # requires the secret key.
    decryptor = Decryptor(context, secret_key)

    # We start by encoding two integers as plaintext polynomials.
    value1 = 5;
    plain1 = encoder.encode(value1);
    print("Encoded " + (str)(value1) + " as polynomial " + plain1.to_string() + " (plain1)")

    value2 = -7;
    plain2 = encoder.encode(value2);
    print("Encoded " + (str)(value2) + " as polynomial " + plain2.to_string() + " (plain2)")

    # Encrypting the values is easy.
    encrypted1 = Ciphertext()
    encrypted2 = Ciphertext()
    print("Encrypting plain1: ")
    encryptor.encrypt(plain1, encrypted1)
    print("Done (encrypted1)")

    print("Encrypting plain2: ")
    encryptor.encrypt(plain2, encrypted2)
    print("Done (encrypted2)")

    # To illustrate the concept of noise budget, we print the budgets in the fresh
    # encryptions.
    print("Noise budget in encrypted1: " + (str)(decryptor.invariant_noise_budget(encrypted1)) + " bits")
    print("Noise budget in encrypted2: " + (str)(decryptor.invariant_noise_budget(encrypted2)) + " bits")

    # As a simple example, we compute (-encrypted1 + encrypted2) * encrypted2.

    # Negation is a unary operation.
    evaluator.negate(encrypted1)

    # Negation does not consume any noise budget.
    print("Noise budget in -encrypted1: " + (str)(decryptor.invariant_noise_budget(encrypted1)) + " bits")

    # Addition can be done in-place (overwriting the first argument with the result,
    # or alternatively a three-argument overload with a separate destination variable
    # can be used. The in-place variants are always more efficient. Here we overwrite
    # encrypted1 with the sum.
    evaluator.add(encrypted1, encrypted2)

    # It is instructive to think that addition sets the noise budget to the minimum
    # of the input noise budgets. In this case both inputs had roughly the same
    # budget going on, and the output (in encrypted1) has just slightly lower budget.
    # Depending on probabilistic effects, the noise growth consumption may or may
    # not be visible when measured in whole bits.
    print("Noise budget in -encrypted1 + encrypted2: " + (str)(decryptor.invariant_noise_budget(encrypted1)) + " bits")

    # Finally multiply with encrypted2. Again, we use the in-place version of the
    # function, overwriting encrypted1 with the product.
    evaluator.multiply(encrypted1, encrypted2)

    # Multiplication consumes a lot of noise budget. This is clearly seen in the
    # print-out. The user can change the plain_modulus to see its effect on the
    # rate of noise budget consumption.
    print("Noise budget in (-encrypted1 + encrypted2) * encrypted2: " + (str)(decryptor.invariant_noise_budget(encrypted1)) + " bits")

    # Now we decrypt and decode our result.
    plain_result = Plaintext()
    print("Decrypting result: ")
    decryptor.decrypt(encrypted1, plain_result)
    print("Done")

    # Print the result plaintext polynomial.
    print("Plaintext polynomial: " + plain_result.to_string())

    # Decode to obtain an integer result.
    print("Decoded integer: " + (str)(encoder.decode_int32(plain_result)))

def example_basics_ii():
    print_example_banner("Example: Basics II")

    # In this example we explain what relinearization is, how to use it, and how
    # it affects noise budget consumption.

    # First we set the parameters, create a SEALContext, and generate the public
    # and secret keys. We use slightly larger parameters than be fore to be able
    # to do more homomorphic multiplications.
    parms = EncryptionParameters()
    parms.set_poly_modulus("1x^8192 + 1")

    # The default coefficient modulus consists of the following primes:

    #     0x7fffffffba0001,
    #     0x7fffffffaa0001,
    #     0x7fffffff7e0001,
    #     0x3fffffffd60001.

    # The total size is 219 bits.
    parms.set_coeff_modulus(seal.coeff_modulus_128(8192))
    parms.set_plain_modulus(1 << 10)

    context = SEALContext(parms)
    print_parameters(context)

    keygen = KeyGenerator(context)
    public_key = keygen.public_key()
    secret_key = keygen.secret_key()

    # We also set up an Encryptor, Evaluator, and Decryptor here. We will
    # encrypt polynomials directly in this example, so there is no need for
    # an encoder.
    encryptor = Encryptor(context, public_key)
    evaluator = Evaluator(context)
    decryptor = Decryptor(context, secret_key)

    # There are actually two more types of keys in SEAL: `evaluation keys' and
    # `Galois keys'. Here we will discuss evaluation keys, and Galois keys will
    # be discussed later in example_batching().

    # In SEAL, a valid ciphertext consists of two or more polynomials with
    # coefficients integers modulo the product of the primes in coeff_modulus.
    # The current size of a ciphertext can be found using Ciphertext::size().
    # A freshly encrypted ciphertext always has size 2.
    #plain1 = Plaintext("1x^2 + 2x^1 + 3")
    plain1 = Plaintext("1x^2 + 2x^1 + 3")
    encrypted = Ciphertext()

    print("")
    print("Encrypting " + plain1.to_string() + ": ")
    encryptor.encrypt(plain1, encrypted)
    print("Done")
    print("Size of a fresh encryption: " + (str)(encrypted.size()))
    print("Noise budget in fresh encryption: " + (str)(decryptor.invariant_noise_budget(encrypted)) + " bits")

    # Homomorphic multiplication results in the output ciphertext growing in size.
    # More precisely, if the input ciphertexts have size M and N, then the output
    # ciphertext after homomorphic multiplication will have size M+N-1. In this
    # case we square encrypted twice to observe this growth (also observe noise
    # budget consumption).
    evaluator.square(encrypted)
    print("Size after squaring: " + (str)(encrypted.size()))
    print("Noise budget after squaring: " + (str)(decryptor.invariant_noise_budget(encrypted)) + " bits")
    plain2 = Plaintext()
    decryptor.decrypt(encrypted, plain2)
    print("Second power: " + plain2.to_string())

    evaluator.square(encrypted);
    print("Size after squaring: " + (str)(encrypted.size()))
    print("Noise budget after squaring: " + (str)(decryptor.invariant_noise_budget(encrypted)) + " bits")

    # It does not matter that the size has grown -- decryption works as usual.
    # Observe from the print-out that the coefficients in the plaintext have
    # grown quite large. One more squaring would cause some of them to wrap
    # around plain_modulus (0x400), and as a result we would no longer obtain
    # the expected result as an integer-coefficient polynomial. We can fix this
    # problem to some extent by increasing plain_modulus. This would make sense,
    # since we still have plenty of noise budget left.
    plain2 = Plaintext()
    decryptor.decrypt(encrypted, plain2)
    print("Fourth power: " + plain2.to_string())

    # The problem here is that homomorphic operations on large ciphertexts are
    # computationally much more costly than on small ciphertexts. Specifically,
    # homomorphic multiplication on input ciphertexts of size M and N will require
    # O(M*N) polynomial multiplications to be performed, and an addition will
    # require O(M+N) additions. Relinearization reduces the size of the ciphertexts
    # after multiplication back to the initial size (2). Thus, relinearizing one
    # or both inputs before the next multiplication, or e.g. before serializing the
    # ciphertexts, can have a huge positive impact on performance.

    # Another problem is that the noise budget consumption in multiplication is
    # bigger when the input ciphertexts sizes are bigger. In a complicated
    # computation the contribution of the sizes to the noise budget consumption
    # can actually become the dominant term. We will point this out again below
    # once we get to our example.

    # Relinearization itself has both a computational cost and a noise budget cost.
    # These both depend on a parameter called `decomposition bit count', which can
    # be any integer at least 1 [dbc_min()] and at most 60 [dbc_max()]. A large
    # decomposition bit count makes relinearization fast, but consumes more noise
    # budget. A small decomposition bit count can make relinearization slower, but
    # might not change the noise budget by any observable amount.

    # Relinearization requires a special type of key called `evaluation keys'.
    # These can be created by the KeyGenerator for any decomposition bit count.
    # To relinearize a ciphertext of size M >= 2 back to size 2, we actually need
    # M-2 evaluation keys. Attempting to relinearize a too large ciphertext with
    # too few evaluation keys will result in an exception being thrown.

    # We repeat our computation, but this time relinearize after both squarings.
    # Since our ciphertext never grows past size 3 (we relinearize after every
    # multiplication), it suffices to generate only one evaluation key.

    # First, we need to create evaluation keys. We use a decomposition bit count
    # of 16 here, which can be thought of as quite small.
    ev_keys16 = EvaluationKeys()

    # This function generates one single evaluation key. Another overload takes
    # the number of keys to be generated as an argument, but one is all we need
    # in this example (see above).
    keygen.generate_evaluation_keys(16, ev_keys16)

    print("")
    print("Encrypting " + plain1.to_string() + ": ")
    encryptor.encrypt(plain1, encrypted)
    print("Done")
    print("Size of a fresh encryption: " + (str)(encrypted.size()))
    print("Noise budget in fresh encryption: " + (str)(decryptor.invariant_noise_budget(encrypted)) + " bits")

    evaluator.square(encrypted)
    print("Size after squaring: " + (str)(encrypted.size()))
    print("Noise budget after squaring: " + (str)(decryptor.invariant_noise_budget(encrypted)) + " bits")

    evaluator.relinearize(encrypted, ev_keys16)
    print("Size after relinearization: " + (str)(encrypted.size()))
    print("Noise budget after relinearizing (dbs = " + (str)(ev_keys16.decomposition_bit_count()) + "): " +
        (str)(decryptor.invariant_noise_budget(encrypted)) + " bits")

    evaluator.square(encrypted)
    print("Size after second squaring: " + (str)(encrypted.size()) + " bits")
    print("Noise budget after second squaring: " + (str)(decryptor.invariant_noise_budget(encrypted)) + " bits")

    evaluator.relinearize(encrypted, ev_keys16)
    print("Size after relinearization: " + (str)(encrypted.size()))
    print("Noise budget after relinearizing (dbs = " + (str)(ev_keys16.decomposition_bit_count()) + "): " +
        (str)(decryptor.invariant_noise_budget(encrypted)) + " bits")

    decryptor.decrypt(encrypted, plain2)
    print("Fourth power: " + plain2.to_string())

    # Of course the result is still the same, but this time we actually
    # used less of our noise budget. This is not surprising for two reasons:

    #     - We used a very small decomposition bit count, which is why
    #       relinearization itself did not consume the noise budget by any
    #       observable amount;
    #     - Since our ciphertext sizes remain small throughout the two
    #       squarings, the noise budget consumption rate in multiplication
    #       remains as small as possible. Recall from above that operations
    #       on larger ciphertexts actually cause more noise growth.

    # To make matters even more clear, we repeat the computation a third time,
    # now using the largest possible decomposition bit count (60). We are not
    # measuring the time here, but relinearization with these evaluation keys
    # is significantly faster than with ev_keys16.
    ev_keys60 = EvaluationKeys()
    keygen.generate_evaluation_keys(seal.dbc_max(), ev_keys60)

    print("")
    print("Encrypting " + plain1.to_string() + ": ")
    encryptor.encrypt(plain1, encrypted)
    print("Done")
    print("Size of a fresh encryption: " + (str)(encrypted.size()))
    print("Noise budget in fresh encryption: " + (str)(decryptor.invariant_noise_budget(encrypted)) + " bits")

    evaluator.square(encrypted)
    print("Size after squaring: " + (str)(encrypted.size()))
    print("Noise budget after squaring: " + (str)(decryptor.invariant_noise_budget(encrypted)) + " bits")
    evaluator.relinearize(encrypted, ev_keys60)
    print("Size after relinearization: " + (str)(encrypted.size()))
    print("Noise budget after relinearizing (dbc = " + (str)(ev_keys60.decomposition_bit_count()) +
        "): " + (str)(decryptor.invariant_noise_budget(encrypted)) + " bits")

    evaluator.square(encrypted)
    print("Size after second squaring: " + (str)(encrypted.size()))
    print("Noise budget after second squaring: " + (str)(decryptor.invariant_noise_budget) + " bits")
    evaluator.relinearize(encrypted, ev_keys60)
    print("Size after relinearization: " + (str)(encrypted.size()))
    print("Noise budget after relinearizing (dbc = " + (str)(ev_keys60.decomposition_bit_count()) +
        "): " + (str)(decryptor.invariant_noise_budget(encrypted)) + " bits")

    decryptor.decrypt(encrypted, plain2)
    print("Fourth power: " + plain2.to_string())

    # Observe from the print-out that we have now used significantly more of our
    # noise budget than in the two previous runs. This is again not surprising,
    # since the first relinearization chops off a huge part of the noise budget.

    # However, note that the second relinearization does not change the noise
    # budget by any observable amount. This is very important to understand when
    # optimal performance is desired: relinearization always drops the noise
    # budget from the maximum (freshly encrypted ciphertext) down to a fixed
    # amount depending on the encryption parameters and the decomposition bit
    # count. On the other hand, homomorphic multiplication always consumes the
    # noise budget from its current level. This is why the second relinearization
    # does not change the noise budget anymore: it is already consumed past the
    # fixed amount determinted by the decomposition bit count and the encryption
    # parameters.

    # We now perform a third squaring and observe an even further compounded
    # decrease in the noise budget. Again, relinearization does not consume the
    # noise budget at this point by any observable amount, even with the largest
    # possible decomposition bit count.
    evaluator.square(encrypted)
    print("Size after third squaring " + (str)(encrypted.size()))
    print("Noise budget after third squaring: " + (str)(decryptor.invariant_noise_budget(encrypted)) + " bits")
    evaluator.relinearize(encrypted, ev_keys60)
    print("Size after relinearization: " + (str)(encrypted.size()))
    print("Noise budget after relinearizing (dbc = " + (str)(ev_keys60.decomposition_bit_count()) +
        "): " + (str)(decryptor.invariant_noise_budget(encrypted)) + " bits")

    decryptor.decrypt(encrypted, plain2)
    print("Eighth power: " + plain2.to_string())

    # Observe from the print-out that the polynomial coefficients are no longer
    # correct as integers: they have been reduced modulo plain_modulus, and there
    # was no warning sign about this. It might be necessary to carefully analyze
    # the computation to make sure such overflow does not occur unexpectedly.

    # These experiments suggest that an optimal strategy might be to relinearize
    # first with evaluation keys with a small decomposition bit count, and later
    # with evaluation keys with a larger decomposition bit count (for performance)
    # when noise budget has already been consumed past the bound determined by the
    # larger decomposition bit count. For example, above the best strategy might
    # have been to use ev_keys16 in the first relinearization, and ev_keys60 in the
    # next two relinearizations for optimal noise budget consumption/performance
    # trade-off. Luckily, in most use-cases it is not so critical to squeeze out
    # every last bit of performance, especially when slightly larger parameters
    # are used.

def main():
    # Example: Basics I
    example_basics_i()
    # Example: Basics II
    example_basics_ii()
    # Example: Weighted Average
    # example_weighted_average()
    # Example: Automatic Parameter Selection
    # example_parameter_selection()
    # Example: Batching using CRT
    # example_batching();
    # Example: Relinearization
    # example_relinearization();
    # Example: Timing of basic operations
    # example_timing()
    # Wait for ENTER before closing screen.
    input('Press ENTER to exit')

def print_example_banner(title, ch='*', length=78):
    spaced_text = ' %s ' % title
    print(spaced_text.center(length, ch))

def print_parameters(context):
    print("/ Encryption parameters:")
    print("| poly_modulus: " + context.poly_modulus().to_string())

    # Print the size of the true (product) coefficient modulus
    print("| coeff_modulus_size: " + (str)(context.total_coeff_modulus().significant_bit_count()) + " bits")

    print("| plain_modulus: " + (str)(context.plain_modulus().value()))
    print("| noise_standard_deviation: " + (str)(context.noise_standard_deviation))

if __name__ == '__main__':
    main()
