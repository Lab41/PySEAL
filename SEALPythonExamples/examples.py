from seal import EncryptionParameters

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
