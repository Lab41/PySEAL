from seal import ChooserEvaluator,     \
                 Ciphertext,           \
                 Decryptor,            \
                 Encryptor,            \
                 EncryptionParameters, \
                 Evaluator,            \
                 IntegerEncoder,       \
                 KeyGenerator,         \
                 MemoryPoolHandle,     \
                 Plaintext

def example_basics():
    """
    In this example we demonstrate using some of the basic arithmetic operations
    on integers.

    SEAL uses the Fan-Vercauteren (FV) homomorphic encryption scheme. We refer
    to https://eprint.iacr.org/2012/144 for full details on how the FV scheme
    works.
    """
    print_example_banner('Example: Basics')

    # Create encryption parameters.
    parms = EncryptionParameters()

    # We first choose the polynomial modulus. This must be a power-of-2
    # cyclotomic polynomial, i.e. a polynomial of the form "1x^(power-of-2) + 1"
    # We recommend using polynomials of degree at least 1024.
    parms.set_poly_modulus('1x^2048 + 1')

    # Next we choose the coefficient modulus. SEAL comes with default values for
    # the coefficient modulus for some of the most reasonable choices of
    # poly_modulus. They are as follows:
    #
    # /----------------------------------------------------------------------\
    # | poly_modulus | default coeff_modulus                      | security |
    # | -------------|--------------------------------------------|----------|
    # | 1x^2048 + 1  | 2^60 - 2^14 + 1 (60 bits)                  | 119 bit  |
    # | 1x^4096 + 1  | 2^116 - 2^18 + 1 (116 bits)                | 122 bit  |
    # | 1x^8192 + 1  | 2^226 - 2^26 + 1 (226 bits)                | 124 bit  |
    # | 1x^16384 + 1 | 2^435 - 2^33 + 1 (435 bits)                | 130 bit  |
    # | 1x^32768 + 1 | 2^889 - 2^54 - 2^53 - 2^52 + 1 (889 bits)  | 127 bit  |
    # \----------------------------------------------------------------------/
    #
    # These can be conveniently accessed using
    # ChooserEvaluator::default_parameter_options(), which returns the above
    # list of options as an std::map, keyed by the degree of the polynomial
    # modulus. The security levels are estimated based on
    # https://eprint.iacr.org/2015/046 and https://eprint.iacr.org/2017/047. We
    # strongly recommend that the user consult an expert in the security of
    # RLWE-based cryptography to estimate the security of a particular choice
    # of parameters.
    #
    # The user can also easily choose their custom coefficient modulus. For best
    # performance, it should  be a prime of the form 2^A - B, where B is
    # congruent to 1 modulo 2*degree(poly_modulus), and as small as possible.
    # Roughly speaking, When the rest of the parameters are held fixed,
    # increasing coeff_modulus decreases the security level. Thus we would not
    # recommend using a value for coeff_modulus much larger than those listed
    # above (the defaults). In general, we highly recommend the user to consult
    # with an expert in the security of RLWE-based cryptography when selecting
    # their parameters to ensure an appropriate level of security.
    #
    # The size of coeff_modulus affects the total noise budget that a freshly
    # encrypted ciphertext has. More precisely, every ciphertext starts with a
    # certain amount of noise budget, which is consumed in homomorphic
    # operations - in particular in multiplication. Once the noise budget
    # reaches 0, the ciphertext becomes impossible to decrypt. The total noise
    # budget in a freshly encrypted ciphertext is very roughly given by
    # log2(coeff_modulus/plain_modulus), so increasing coeff_modulus will allow
    # the user to perform more homomorphic operations on the ciphertexts without
    #  corrupting them. However, we must again warn that increasing
    # coeff_modulus has a strong negative effect on the security level.
    parms.set_coeff_modulus(ChooserEvaluator.default_parameter_options()[2048]);

    # Now we set the plaintext modulus. This can be any positive integer, even
    # though here we take it to be a power of two. A larger plaintext modulus
    # causes the noise to grow faster in homomorphic multiplication, and also
    # lowers the maximum amount of noise in ciphertexts that the system can
    # tolerate (see above). On the other hand, a larger plaintext modulus
    # typically allows for better homomorphic integer arithmetic, although this
    # depends strongly on which encoder is used to encode integers into
    # plaintext polynomials.
    parms.set_plain_modulus(1 << 8)


    # Once all parameters are set, we need to call
    # EncryptionParameters::validate(), which evaluates the properties of the
    # parameters, their validity for homomorphic encryption, and performs some
    # important pre-computation.
    parms.validate()

    # Plaintext elements in the FV scheme are polynomials (represented by the
    # Plaintext class) with coefficients integers modulo plain_modulus. To
    # encrypt for example integers instead, one must use an "encoding scheme",
    # i.e. a specific way of representing integers as such polynomials. SEAL
    # comes with a few basic encoders:
    #
    # IntegerEncoder:
    # Given an integer base b, encodes integers as plaintext polynomials in the
    # following way. First, a base-b expansion of the integer is computed. This
    # expansion uses a "balanced" set of representatives of integers modulo b as
    # the coefficients. Namely, when b is off the coefficients are integers
    # between -(b-1)/2 and (b-1)/2. When b is even, the integers are between
    # -b/2 and (b-1)/2, except when b is two and the usual binary expansion is
    # used (coefficients 0 and 1). Decoding amounts to evaluating the polynomial
    # at x=b. For example, if b=2, the integer 26 = 2^4 + 2^3 + 2^1 is encoded
    # as the polynomial 1x^4 + 1x^3 + 1x^1. When b=3, 26 = 3^3 - 3^0 is encoded
    # as the polynomial 1x^3 - 1. In reality, coefficients of polynomials are
    # always unsigned integers, and in this case are stored as their smallest
    # non-negative representatives modulo plain_modulus. To create an integer
    # encoder with a base b, use IntegerEncoder(plain_modulus, b).  If no b is
    # given to the constructor, the default value of b=2 is used.
    #
    # FractionalEncoder:
    # Encodes fixed-precision rational numbers as follows. First expand the
    # number in a given base b, possibly truncating an infinite fractional part
    # to finite precision, e.g. 26.75 = 2^4 + 2^3 + 2^1 + 2^(-1) + 2^(-2) when
    # =2. For the sake of the example, suppose poly_modulus is 1x^1024 + 1. Next
    # represent the integer part of the number in the same way as in
    # IntegerEncoder (with b=2 here). Finally, represent the fractional part in
    # the leading coefficients of the polynomial, but when doing so invert the
    # signs of the coefficients. So in this example we would represent 26.75 as
    # the polynomial -1x^1023 - 1x^1022 + 1x^4 + 1x^3 + 1x^1. The negative
    # coefficients of the polynomial will again be represented as their
    # negatives modulo plain_modulus.
    #
    # PolyCRTBuilder:
    # If poly_modulus is 1x^N + 1, PolyCRTBuilder allows "batching" of N
    # plaintext integers modulo plain_modulus into one plaintext polynomial,
    # where homomorphic operations can be carried out very efficiently in a SIMD
    # manner by operating on such a "composed" plaintext or ciphertext
    # polynomials. For full details on this very powerful technique we
    # recommend https://eprint.iacr.org/2012/565.pdf and
    # https://eprint.iacr.org/2011/133.
    #
    # A crucial fact to understand is that when homomorphic operations are
    # performed on ciphertexts, they will carry over to the underlying
    # plaintexts, and as a result of additions and multiplications the
    # coefficients in the plaintext polynomials will increase from what they
    # originally were in freshly encoded polynomials. This becomes a problem
    # when the coefficients reach the size of plain_modulus, in which case they
    # will get automatically reduced modulo plain_modulus, and might render the
    # underlying plaintext polynomial impossible to be correctly decoded back
    # into an integer or rational number. Therefore, it is typically crucial to
    # have a good sense of how large the coefficients will grow in the
    # underlying plaintext polynomials when homomorphic computations are carried
    #  out on the ciphertexts, and make sure that plain_modulus is chosen to be
    # at least as large as this number.

    # Here we choose to create an IntegerEncoder with base b=2.
    memorypool = MemoryPoolHandle.acquire_global()
    encoder = IntegerEncoder(parms.plain_modulus(),
                             2,
                             memorypool)

    # Encode two integers as polynomials.
    value1 = 5
    value2 = -7
    encoded1 = encoder.encode(value1)
    encoded2 = encoder.encode(value2)
    print("Encoded " + str(value1) + " as polynomial " + encoded1.to_string())
    print("Encoded " + str(value2) + " as polynomial " + encoded2.to_string())

    # Generate keys.
    print("Generating keys ...")
    generator = KeyGenerator(parms, memorypool);
    generator.generate(0);
    print("... key generation complete")
    public_key = generator.public_key()
    secret_key = generator.secret_key()

    # Encrypt values.
    print("Encrypting values...")
    encryptor = Encryptor(parms, public_key, memorypool)
    encrypted1 = encryptor.encrypt(encoded1)
    encrypted2 = encryptor.encrypt(encoded2)

    # Perform arithmetic on encrypted values.
    print("Performing arithmetic on ecrypted numbers ...")
    evaluator = Evaluator(parms, memorypool)
    print("Performing homomorphic negation ...")
    encryptednegated1 = evaluator.negate(encrypted1)
    print("Performing homomorphic addition ...")
    encryptedsum = evaluator.add(encrypted1, encrypted2)
    print("Performing homomorphic subtraction ...")
    encrypteddiff = evaluator.sub(encrypted1, encrypted2)
    print("Performing homomorphic multiplication ...")
    encryptedproduct = evaluator.multiply(encrypted1, encrypted2)

    # Decrypt results.
    print("Decrypting results ...")
    decryptor = Decryptor(parms, secret_key, memorypool)
    decrypted1 = decryptor.decrypt(encrypted1)
    decrypted2 = decryptor.decrypt(encrypted2)
    decryptednegated1 = decryptor.decrypt(encryptednegated1)
    decryptedsum = decryptor.decrypt(encryptedsum)
    decrypteddiff = decryptor.decrypt(encrypteddiff)
    decryptedproduct = decryptor.decrypt(encryptedproduct)

    # Decode results.
    decoded1 = encoder.decode_int32(decrypted1)
    decoded2 = encoder.decode_int32(decrypted2)
    decodednegated1 = encoder.decode_int32(decryptednegated1)
    decodedsum = encoder.decode_int32(decryptedsum)
    decodeddiff = encoder.decode_int32(decrypteddiff)
    decodedproduct = encoder.decode_int32(decryptedproduct)

    # Display results.
    print("Original = " + str(value1) + "; after encryption/decryption = "
        + str(decoded1))
    print("Original = " + str(value2) + "; after encryption/decryption = "
        + str(decoded2))
    print("Encrypted negate of " + str(value1) + " = " + str(decodednegated1))
    print("Encrypted addition of " + str(value1) + " and " + str(value2) + " = "
        + str(decodedsum))
    print("Encrypted subtraction of " + str(value1) + " and " + str(value2)
        + " = " + str(decodeddiff))
    print("Encrypted multiplication of " + str(value1) + " and " + str(value2)
        + " = " + str(decodedproduct))

    # How much noise budget did we use in these operations?
    print("Noise budget in encryption of " + str(value1) + ": "
        + str(decryptor.invariant_noise_budget(encrypted1)) + " bits")
    print("Noise budget in encryption of " + str(value2) + ": "
        + str(decryptor.invariant_noise_budget(encrypted2)) + " bits")
    print("Noise budget in sum: "
        + str(decryptor.invariant_noise_budget(encryptedsum)) + " bits")
    print("Noise budget in product: "
        + str(decryptor.invariant_noise_budget(encryptedproduct)) + " bits")

def main():
    # Example: Basics
    example_basics()
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

if __name__ == '__main__':
    main()
