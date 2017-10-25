from seal import EncryptionParameters, ChooserEvaluator

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
