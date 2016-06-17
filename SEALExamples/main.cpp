#include <iostream>
#include <vector>
#include <sstream>

#include "seal.h"

using namespace std;
using namespace seal;

void print_example_banner(string title);

void example_basics();
void example_weighted_average();
void example_parameter_selection();
void example_batching();
void example_relinearization();

int main()
{
    // Example: Basics
    example_basics();

    // Example: Weighted Average
    example_weighted_average();

    // Example: Automatic Parameter Selection
    example_parameter_selection();

    // Example: Batching using CRT
    example_batching();

    // Example: Relinearization
    example_relinearization();

    // Wait for ENTER before closing screen.
    cout << "Press ENTER to exit" << endl;
    char ignore;
    cin.get(ignore);

    return 0;
}

void print_example_banner(string title)
{
    if (!title.empty())
    {
        size_t title_length = title.length();
        size_t banner_length = title_length + 2 + 2 * 10;
        string banner_top(banner_length, '*');
        string banner_middle = string(10, '*') + " " + title + " " + string(10, '*');

        cout << endl
            << banner_top << endl
            << banner_middle << endl
            << banner_top << endl
            << endl;
    }
}

void example_basics()
{
    print_example_banner("Example: Basics");

    /*
    In this example we demonstrate using some of the basic arithmetic operations on integers.

    SEAL uses the Fan-Vercauteren (FV) homomorphic encryption scheme. We refer to
    https://eprint.iacr.org/2012/144 for full details on how the FV scheme works.
    */

    // Create encryption parameters.
    EncryptionParameters parms;

    /*
    First choose the polynomial modulus. This must be a power-of-2 cyclotomic polynomial,
    i.e. a polynomial of the form "1x^(power-of-2) + 1". We recommend using polynomials of
    degree at least 1024.
    */
    parms.poly_modulus() = "1x^2048 + 1";

    /*
    Next choose the coefficient modulus. The values we recommend to be used are:

    [ degree(poly_modulus), coeff_modulus ]
    [ 1024, "FFFFFFF00001" ],
    [ 2048, "3FFFFFFFFFFFFFFFFFF00001"],
    [ 4096, "3FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC0000001"],
    [ 8192, "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE00000001"],
    [ 16384, "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000000001"].

    These can be conveniently accessed using ChooserEvaluator::default_parameter_options(),
    which returns the above list of options as an std::map, keyed by the degree of the polynomial modulus.
    
    The user can also relatively easily choose their custom coefficient modulus. It should be a prime number
    of the form 2^A - 2^B + 1, where A > B > degree(poly_modulus). Moreover, B should be as small as possible
    for improved efficiency in modular reduction. For security, we recommend strictly adhering to the following 
    size bounds: (see Lepoint-Naehrig (2014) [https://eprint.iacr.org/2014/062])
    /------------------------------------------------------------------\
    | poly_modulus | coeff_modulus bound | default coeff_modulus       |
    | -------------|---------------------|-----------------------------|
    | 1x^1024 + 1  | 48 bits             | 2^48 - 2^20 + 1 (47 bits)   |
    | 1x^2048 + 1  | 96 bits             | 2^94 - 2^20 + 1 (93 bits)   |
    | 1x^4096 + 1  | 192 bits            | 2^190 - 2^30 + 1 (189 bits) |
    | 1x^8192 + 1  | 384 bits            | 2^383 - 2^33 + 1 (382 bits) |
    | 1x^16384 + 1 | 768 bits            | 2^767 - 2^56 + 1 (766 bits) |
    \------------------------------------------------------------------/

    The size of coeff_modulus affects the upper bound on the "inherent noise" that a ciphertext can contain
    before becoming corrupted. More precisely, every ciphertext starts with a certain amount of noise in it,
    which grows in all homomorphic operations - in particular in multiplication. Once a ciphertext contains
    too much noise, it becomes impossible to decrypt. The upper bound on the noise is roughly given by 
    coeff_modulus/plain_modulus (see below), so increasing coeff_modulus will allow the user to perform more
    homomorphic operations on the ciphertexts without corrupting them. We would like to stress, however, that
    the bounds given above for coeff_modulus should absolutely not be exceeded.
    */
    parms.coeff_modulus() = ChooserEvaluator::default_parameter_options().at(2048);

    /*
    Now we set the plaintext modulus. This can be any positive integer, even though here we take it to be a 
    power of two. A larger plaintext modulus causes the noise to grow faster in homomorphic multiplication, 
    and also lowers the maximum amount of noise in ciphertexts that the system can tolerate (see above).
    On the other hand, a larger plaintext modulus typically allows for better homomorphic integer arithmetic,
    although this depends strongly on which encoder is used to encode integers into plaintext polynomials.
    */
    parms.plain_modulus() = 1 << 8;

    cout << "Encryption parameters specify " << parms.poly_modulus().significant_coeff_count() << " coefficients with "
        << parms.coeff_modulus().significant_bit_count() << " bits per coefficient" << endl;

    /*
    Plaintext elements in the FV scheme are polynomials (represented by the BigPoly class) with coefficients 
    integers modulo plain_modulus. To encrypt integers instead, one must use an "encoding scheme", i.e. 
    a specific way of representing integers as such polynomials. SEAL comes with a few basic encoders:

    BinaryEncoder:
    Encodes positive integers as plaintext polynomials where the coefficients are either 0 or 1 according
    to the binary representation of the integer to be encoded. Decoding amounts to evaluating the polynomial
    at x=2. For example, the integer 26 = 2^4 + 2^3 + 2^1 is encoded as the polynomial 1x^4 + 1x^3 + 1x^1.
    Negative integers are encoded similarly but with each coefficient coefficient of the polynomial replaced 
    with its negative modulo plain_modulus.

    BalancedEncoder:
    Given an odd integer base b, encodes integers as plaintext polynomials where the coefficients are according
    to the "balanced" base-b representation of the integer to be encoded, i.e. where each coefficient is in the
    range -(b-1)/2,...,(b-1)/2. Decoding amounts to evaluating the polynomial at x=b. For example, when b=3 the 
    integer 25 = 3^3 - 3^1 + 3^0 is encoded as the polynomial 1x^3 - 1x^1 + 1.
    
    BinaryFractionalEncoder:
    Encodes rational numbers as follows. First represent the number in binary, possibly truncating an infinite
    fractional part to some fixed precision, e.g. 26.75 = 2^4 + 2^3 + 2^1 + 2^(-1) + 2^(-2). For the sake of
    the example, suppose poly_modulus is 1x^1024 + 1. Next represent the integer part of the number in the same
    was as in BinaryEncoder. Finally, represent the fractional part in the leading coefficients of the polynomial, 
    but when doing so invert the signs of the coefficients. So in this example we would represent 26.75 as the 
    polynomial -1x^1023 - 1x^1022 + 1x^4 + 1x^3 + 1x^1. The negative coefficients of the polynomial will again be 
    represented as their negatives modulo plain_modulus.

    BalancedFractionalEncoder:
    Same as BinaryFractionalEncoder, except instead of using base 2 uses any odd base b and balanced 
    representatives for the coefficients, i.e. integers in the range -(b-1)/2,...,(b-1)/2.

    PolyCRTBuilder:
    If poly_modulus is 1x^N + 1, PolyCRTBuilder allows "batching" of N plaintext integers modulo plain_modulus 
    into one plaintext polynomial, where homomorphic operations can be carried out very efficiently in a SIMD 
    manner by operating on such a "composed" plaintext or ciphertext polynomials. For full details on this very
    powerful technique we recommend https://eprint.iacr.org/2012/565.pdf and https://eprint.iacr.org/2011/133.

    A crucial fact to understand is that when homomorphic operations are performed on ciphertexts, they will
    carry over to the underlying plaintexts, and as a result of additions and multiplications the coefficients
    in the plaintext polynomials will increase from what they originally were in freshly encoded polynomials.
    This becomes a problem when the coefficients reach the size of plain_modulus, in which case they will get
    automatically reduced modulo plain_modulus, and might render the underlying plaintext polynomial impossible
    to be correctly decoded back into an integer or rational number. Therefore, it is typically crucial to
    have a good sense of how large the coefficients will grow in the underlying plaintext polynomials when
    homomorphic computations are carried out on the ciphertexts, and make sure that plain_modulus is chosen to
    be at least as large as this number.
    */

    // Encode two integers as polynomials.
    const int value1 = 5;
    const int value2 = -7;
    BalancedEncoder encoder(parms.plain_modulus());
    BigPoly encoded1 = encoder.encode(value1);
    BigPoly encoded2 = encoder.encode(value2);
    cout << "Encoded " << value1 << " as polynomial " << encoded1.to_string() << endl;
    cout << "Encoded " << value2 << " as polynomial " << encoded2.to_string() << endl;

    // Generate keys.
    cout << "Generating keys..." << endl;
    KeyGenerator generator(parms);
    generator.generate();
    cout << "... key generation complete" << endl;
    BigPolyArray public_key = generator.public_key();
    BigPoly secret_key = generator.secret_key();
    //cout << "Public Key = " << public_key.to_string() << endl;
    //cout << "Secret Key = " << secret_key.to_string() << endl;

    // Encrypt values.
    cout << "Encrypting values..." << endl;
    Encryptor encryptor(parms, public_key);
    BigPolyArray encrypted1 = encryptor.encrypt(encoded1);
    BigPolyArray encrypted2 = encryptor.encrypt(encoded2);

    // Perform arithmetic on encrypted values.
    cout << "Performing encrypted arithmetic..." << endl;
    Evaluator evaluator(parms);
    cout << "... Performing negation..." << endl;
    BigPolyArray encryptednegated1 = evaluator.negate(encrypted1);
    cout << "... Performing addition..." << endl;
    BigPolyArray encryptedsum = evaluator.add(encrypted1, encrypted2);
    cout << "... Performing subtraction..." << endl;
    BigPolyArray encrypteddiff = evaluator.sub(encrypted1, encrypted2);
    cout << "... Performing multiplication..." << endl;
    BigPolyArray encryptedproduct = evaluator.multiply(encrypted1, encrypted2);

    // Decrypt results.
    cout << "Decrypting results..." << endl;
    Decryptor decryptor(parms, secret_key);
    BigPoly decrypted1 = decryptor.decrypt(encrypted1);
    BigPoly decrypted2 = decryptor.decrypt(encrypted2);
    BigPoly decryptednegated1 = decryptor.decrypt(encryptednegated1);
    BigPoly decryptedsum = decryptor.decrypt(encryptedsum);
    BigPoly decrypteddiff = decryptor.decrypt(encrypteddiff);
    BigPoly decryptedproduct = decryptor.decrypt(encryptedproduct);

    // Decode results.
    int decoded1 = encoder.decode_int32(decrypted1);
    int decoded2 = encoder.decode_int32(decrypted2);
    int decodednegated1 = encoder.decode_int32(decryptednegated1);
    int decodedsum = encoder.decode_int32(decryptedsum);
    int decodeddiff = encoder.decode_int32(decrypteddiff);
    int decodedproduct = encoder.decode_int32(decryptedproduct);

    // Display results.
    cout << value1 << " after encryption/decryption = " << decoded1 << endl;
    cout << value2 << " after encryption/decryption = " << decoded2 << endl;
    cout << "encrypted negate of " << value1 << " = " << decodednegated1 << endl;
    cout << "encrypted addition of " << value1 << " and " << value2 << " = " << decodedsum << endl;
    cout << "encrypted subtraction of " << value1 << " and " << value2 << " = " << decodeddiff << endl;
    cout << "encrypted multiplication of " << value1 << " and " << value2 << " = " << decodedproduct << endl;

    // How did the noise grow in these operations?
    int max_noise_bit_count = inherent_noise_max(parms).significant_bit_count();
    cout << "Noise in encryption of " << value1 << ": " << inherent_noise(encrypted1, parms, secret_key).significant_bit_count()
        << "/" << max_noise_bit_count << " bits" << endl;
    cout << "Noise in encryption of " << value2 << ": " << inherent_noise(encrypted2, parms, secret_key).significant_bit_count()
        << "/" << max_noise_bit_count << " bits" << endl;
    cout << "Noise in the sum: " << inherent_noise(encryptedsum, parms, secret_key).significant_bit_count()
        << "/" << max_noise_bit_count << " bits" << endl;
    cout << "Noise in the product: " << inherent_noise(encryptedproduct, parms, secret_key).significant_bit_count()
        << "/" << max_noise_bit_count << " bits" << endl;
}

void example_weighted_average()
{
    print_example_banner("Example: Weighted Average");

    // In this example we demonstrate computing a weighted average of 10 rational numbers.
    
    // The 10 rational numbers we use are:
    const vector<double> rational_numbers { 3.1, 4.159, 2.65, 3.5897, 9.3, 2.3, 8.46, 2.64, 3.383, 2.795 };

    // The 10 weights are:
    const vector<double> coefficients { 0.1, 0.05, 0.05, 0.2, 0.05, 0.3, 0.1, 0.025, 0.075, 0.05 };

    // Create encryption parameters
    EncryptionParameters parms;

    parms.poly_modulus() = "1x^1024 + 1";
    parms.coeff_modulus() = ChooserEvaluator::default_parameter_options().at(1024);
    parms.plain_modulus() = 1 << 8;

    cout << "Encryption parameters specify " << parms.poly_modulus().significant_coeff_count() << " coefficients with "
        << parms.coeff_modulus().significant_bit_count() << " bits per coefficient" << endl;

    // Generate keys.
    cout << "Generating keys..." << endl;
    KeyGenerator generator(parms);
    generator.generate();
    cout << "... key generation complete" << endl;
    BigPolyArray public_key = generator.public_key();
    BigPoly secret_key = generator.secret_key();

    /*
    We will need a fractional encoder for dealing with the rational numbers.
    Here we reserve 128 coefficients of the polynomial for the integral part (low-degree terms)
    and 64 coefficients for the fractional part (high-degree terms).
    */
    BalancedFractionalEncoder encoder(parms.plain_modulus(), parms.poly_modulus(), 128, 64);

    // Create the rest of the tools
    Encryptor encryptor(parms, public_key);
    Evaluator evaluator(parms);
    Decryptor decryptor(parms, secret_key);

    // First we encrypt the rational numbers
    cout << "Encrypting ... ";
    vector<BigPolyArray> encrypted_rationals;
    for (int i = 0; i < 10; ++i)
    {
        BigPoly encoded_number = encoder.encode(rational_numbers[i]);
        encrypted_rationals.push_back(encryptor.encrypt(encoded_number));
        ostringstream rational;
        rational << rational_numbers[i];
        cout << rational.str().substr(0,6) << ((i < 9) ? ", " : ".\n");
    }

    // Next we encode the coefficients. There is no reason to encrypt these since they are not private data.
    cout << "Encoding ... ";
    vector<BigPoly> encoded_coefficients;
    for (int i = 0; i < 10; ++i)
    {
        encoded_coefficients.push_back(encoder.encode(coefficients[i]));
        ostringstream coefficient;
        coefficient << coefficients[i];
        cout << coefficient.str().substr(0,6) << ((i < 9) ? ", " : ".\n");
    }
    
    // We also need to encode 0.1. We will multiply the result by this to perform division by 10.
    BigPoly div_by_ten = encoder.encode(0.1);

    // Now compute all the products of the encrypted rational numbers with the plaintext coefficients
    cout << "Computing products ... ";
    vector<BigPolyArray> encrypted_products;
    for (int i = 0; i < 10; ++i)
    {
        /*
        We use Evaluator::multiply_plain(...) instead of Evaluator::multiply(...) (which would 
        require also the coefficient to be encrypted). This has much better noise growth
        behavior than multiplying two encrypted numbers does.
        */
        BigPolyArray enc_plain_product = evaluator.multiply_plain(encrypted_rationals[i], encoded_coefficients[i]);
        encrypted_products.push_back(enc_plain_product);
    }
    cout << "done." << endl;

    // Now we add together these products. The most convenient way to do that is
    // to use the function Evaluator::add_many(...).
    cout << "Add up all 10 ciphertexts ... ";
    BigPolyArray encrypted_dot_product = evaluator.add_many(encrypted_products);
    cout << " done." << endl;

    // Finally we divide by 10 to obtain the result.
    cout << "Divide by 10 ... ";
    BigPolyArray encrypted_result = evaluator.multiply_plain(encrypted_dot_product, div_by_ten);
    cout << "done." << endl;

    // Decrypt
    cout << "Decrypting ... ";
    BigPoly plain_result = decryptor.decrypt(encrypted_result);
    cout << "done." << endl;

    // Print the answer
    double result = encoder.decode(plain_result);
    cout << "Weighted average: " << result << endl;

    // How much noise did we end up with?
    cout << "Noise in the result: " << inherent_noise(encrypted_result, parms, secret_key).significant_bit_count()
        << "/" << inherent_noise_max(parms).significant_bit_count() << " bits" << endl;
}

void example_parameter_selection()
{
    print_example_banner("Example: Automatic Parameter Selection");

    /*
    Here we demonstrate the automatic parameter selection tool. Suppose we want to find parameters
    that are optimized in a way that allows us to evaluate the polynomial 42x^3-27x+1. We need to know
    the size of the input data, so let's assume that x is an integer with base-3 representation of length
    at most 10.
    */
    cout << "Finding optimized parameters for computing 42x^3-27x+1 ... ";

    ChooserEncoder chooser_encoder;
    ChooserEvaluator chooser_evaluator;
    
    /*
    First create a ChooserPoly representing the input data. You can think of this modeling a freshly
    encrypted cipheretext of a plaintext polynomial with length at most 10 coefficients, where the
    coefficients have absolute value at most 1.
    */
    ChooserPoly cinput(10, 1);

    // Compute the first term
    ChooserPoly ccubed_input = chooser_evaluator.exponentiate(cinput, 3);
    ChooserPoly cterm1 = chooser_evaluator.multiply_plain(ccubed_input, chooser_encoder.encode(42));

    // Compute the second term
    ChooserPoly cterm2 = chooser_evaluator.multiply_plain(cinput, chooser_encoder.encode(27));

    // Subtract the first two terms
    ChooserPoly csum12 = chooser_evaluator.sub(cterm1, cterm2);

    // Add the constant term 1
    ChooserPoly cresult = chooser_evaluator.add_plain(csum12, chooser_encoder.encode(1));

    // To find an optimized set of parameters, we use ChooserEvaluator::select_parameters(...).
    EncryptionParameters optimal_parms;
    chooser_evaluator.select_parameters(cresult, optimal_parms);

    cout << "done." << endl;

    // Let's print these to see what was recommended
    cout << "Selected parameters:" << endl;
    cout << "{ poly_modulus: " << optimal_parms.poly_modulus().to_string() << endl;
    cout << "{ coeff_modulus: " << optimal_parms.coeff_modulus().to_string() << endl;
    cout << "{ plain_modulus: " << optimal_parms.plain_modulus().to_dec_string() << endl;
    cout << "{ decomposition_bit_count: " << optimal_parms.decomposition_bit_count() << endl;
    cout << "{ noise_standard_deviation: " << optimal_parms.noise_standard_deviation() << endl;
    cout << "{ noise_max_deviation: " << optimal_parms.noise_max_deviation() << endl;

    // Let's try to actually perform the homomorphic computation using the recommended parameters.
    // Generate keys.
    cout << "Generating keys..." << endl;
    KeyGenerator generator(optimal_parms);
    generator.generate();
    cout << "... key generation complete" << endl;
    BigPolyArray public_key = generator.public_key();
    BigPoly secret_key = generator.secret_key();

    // Create the encoding/encryption tools
    BalancedEncoder encoder(optimal_parms.plain_modulus());
    Encryptor encryptor(optimal_parms, public_key);
    Evaluator evaluator(optimal_parms);
    Decryptor decryptor(optimal_parms, secret_key);

    // Now perform the computations on real encrypted data.
    int input_value = 12345;
    BigPoly plain_input = encoder.encode(input_value);
    cout << "Encoded " << input_value << " as polynomial " << plain_input.to_string() << endl;

    cout << "Encrypting ... ";
    BigPolyArray input = encryptor.encrypt(plain_input);
    cout << "done." << endl;

    // Compute the first term
    cout << "Computing first term ... ";
    BigPolyArray cubed_input = evaluator.exponentiate(input, 3);
    BigPolyArray term1 = evaluator.multiply_plain(cubed_input, encoder.encode(42));
    cout << "done." << endl;

    // Compute the second term
    cout << "Computing second term ... ";
    BigPolyArray term2 = evaluator.multiply_plain(input, encoder.encode(27));
    cout << "done." << endl;

    // Subtract the first two terms
    cout << "Subtracting first two terms ... ";
    BigPolyArray sum12 = evaluator.sub(term1, term2);
    cout << "done." << endl;

    // Add the constant term 1
    cout << "Adding one ... ";
    BigPolyArray result = evaluator.add_plain(sum12, encoder.encode(1));
    cout << "done." << endl;

    // Decrypt and decode
    cout << "Decrypting ... ";
    BigPoly plain_result = decryptor.decrypt(result);
    cout << "done." << endl;
    
    // Finally print the result
    cout << "Polynomial 42x^3-27x+1 evaluated at x=12345: " << encoder.decode_int64(plain_result) << endl;

    // How much noise did we end up with?
    cout << "Noise in the result: " << inherent_noise(result, optimal_parms, secret_key).significant_bit_count()
        << "/" << inherent_noise_max(optimal_parms).significant_bit_count() << " bits" << endl;
}

void example_batching()
{
    print_example_banner("Example: Batching using CRT");

    // Create encryption parameters
    EncryptionParameters parms;

    /*
    For PolyCRTBuilder we need to use a plain modulus congruent to 1 modulo 2*degree(poly_modulus), and
    preferably a prime number. We could for example use the following parameters:

    parms.poly_modulus() = "1x^2048 + 1";
    parms.coeff_modulus() = ChooserEvaluator::default_parameter_options().at(2048);
    parms.plain_modulus() = 12289;

    However, the primes suggested by ChooserEvaluator::default_parameter_options() are highly non-optimal 
    in this case. The reason is that the noise growth in many homomorphic operations depends on the remainder
    coeff_modulus % plain_modulus, which is typically close to plain_modulus unless the parameters are carefully 
    chosen. The primes in ChooserEvaluator::default_parameter_options() are chosen so that this remainder is 1 
    when plain_modulus is a (not too large) power of 2, so in the earlier examples this was not an issue. 
    However, here we are forced to take plain_modulus to be odd, and as a result the default parameters are no
    longer optimal at all in this sense.

    Thus, for improved performance when using PolyCRTBuilder, we recommend the user to use their own
    custom coeff_modulus. It should be a prime of the form 2^A - D, where D is as small as possible.
    The plain_modulus should be simultaneously chosen to be a prime congruent to 1 modulo 2*degree(poly_modulus),
    so that in addition coeff_modulus % plain_modulus is 1. Finally, coeff_modulus should be bounded by the 
    same strict upper bounds that were mentioned in example_basics():
    /------------------------------------\
    | poly_modulus | coeff_modulus bound |
    | -------------|---------------------|
    | 1x^1024 + 1  | 48 bits             |
    | 1x^2048 + 1  | 96 bits             |
    | 1x^4096 + 1  | 192 bits            |
    | 1x^8192 + 1  | 384 bits            |
    | 1x^16384 + 1 | 768 bits            |
    \------------------------------------/

    One issue with using such custom primes, however, is that they are never NTT primes, i.e. not congruent 
    to 1 modulo 2*degree(poly_modulus), and hence might not allow for certain optimizations to be used in 
    polynomial arithmetic. Another issue is that the search-to-decision reduction of RLWE does not apply to 
    non-NTT primes, but this is not known to result in any concrete reduction in the security level.

    In this example we use the prime 2^95 - 613077 as our coefficient modulus. The user should try switching 
    between this and ChooserEvaluator::default_parameter_options().at(2048) to observe the difference in the 
    noise level at the end of the computation. This difference becomes significantly greater when using larger
    values for plain_modulus.
    */
    parms.poly_modulus() = "1x^2048 + 1";
    //parms.coeff_modulus() = "7FFFFFFFFFFFFFFFFFF6A52B";
    parms.coeff_modulus() = ChooserEvaluator::default_parameter_options().at(2048);
    parms.plain_modulus() = 12289;

    cout << "Encryption parameters specify " << parms.poly_modulus().significant_coeff_count() << " coefficients with "
        << parms.coeff_modulus().significant_bit_count() << " bits per coefficient" << endl;

    // Create the PolyCRTBuilder
    PolyCRTBuilder crtbuilder(parms.plain_modulus(), parms.poly_modulus());
    size_t slot_count = crtbuilder.get_slot_count();

    // Create a vector of values that are to be stored in the slots. We initialize all values to 0 at this point.
    vector<BigUInt> values(slot_count, BigUInt(parms.plain_modulus().bit_count(), static_cast<uint64_t>(0)));

    // Set the first few entries of the values vector to be non-zero
    values[0] = 2;
    values[1] = 3;
    values[2] = 5;
    values[3] = 7;
    values[4] = 11;
    values[5] = 13;

    // Now compose these into one polynomial using PolyCRTBuilder
    cout << "Plaintext slot contents (slot, value): ";
    for (size_t i = 0; i < 6; ++i)
    {
        cout << "(" << i << ", " << values[i].to_dec_string() << ")" << ((i != 5) ? ", " : "\n");
    }
    BigPoly plain_composed_poly = crtbuilder.compose(values);

    // Let's do some homomorphic operations now. First we need all the encryption tools.
    // Generate keys.
    cout << "Generating keys..." << endl;
    KeyGenerator generator(parms);
    generator.generate();
    cout << "... key generation complete" << endl;
    BigPolyArray public_key = generator.public_key();
    BigPoly secret_key = generator.secret_key();

    // Create the encryption tools
    Encryptor encryptor(parms, public_key);
    Evaluator evaluator(parms);
    Decryptor decryptor(parms, secret_key);

    // Encrypt plain_composed_poly
    cout << "Encrypting ... ";
    BigPolyArray encrypted_composed_poly = encryptor.encrypt(plain_composed_poly);
    cout << "done." << endl;

    // Let's square the encrypted_composed_poly
    cout << "Squaring the encrypted polynomial ... ";
    BigPolyArray encrypted_square = evaluator.exponentiate(encrypted_composed_poly, 2);
    cout << "done." << endl;
    cout << "Decrypting the squared polynomial ... ";
    BigPoly plain_square = decryptor.decrypt(encrypted_square);
    cout << "done." << endl;
    
    // Print the squared slots
    cout << "Squared slot contents (slot, value): ";
    for (size_t i = 0; i < 6; ++i)
    {
        cout << "(" << i << ", " << crtbuilder.get_slot(plain_square, i).to_dec_string() << ")" << ((i != 5) ? ", " : "\n");
    }

    // Now let's try to multiply the squares with the plaintext coefficients (3, 1, 4, 1, 5, 9, 0, 0, ..., 0).
    // First create the coefficient vector
    vector<BigUInt> plain_coeff_vector(slot_count, BigUInt(parms.plain_modulus().bit_count(), static_cast<uint64_t>(0)));
    plain_coeff_vector[0] = 3;
    plain_coeff_vector[1] = 1;
    plain_coeff_vector[2] = 4;
    plain_coeff_vector[3] = 1;
    plain_coeff_vector[4] = 5;
    plain_coeff_vector[5] = 9;

    // Use PolyCRTBuilder to compose plain_coeff_vector into a polynomial
    BigPoly plain_coeff_poly = crtbuilder.compose(plain_coeff_vector);

    // Print the coefficient vector
    cout << "Coefficient slot contents (slot, value): ";
    for (size_t i = 0; i < 6; ++i)
    {
        cout << "(" << i << ", " << crtbuilder.get_slot(plain_coeff_poly, i).to_dec_string() << ")" << ((i != 5) ? ", " : "\n");
    }

    // Now use multiply_plain to multiply each encrypted slot with the corresponding coefficient
    cout << "Multiplying squared slots with the coefficients ... ";
    BigPolyArray encrypted_scaled_square = evaluator.multiply_plain(encrypted_square, plain_coeff_poly);
    cout << " done." << endl;
    
    cout << "Decrypting the scaled squared polynomial ... ";
    BigPoly plain_scaled_square = decryptor.decrypt(encrypted_scaled_square);
    cout << "done." << endl;

    // Print the scaled squared slots
    cout << "Scaled squared slot contents (slot, value): ";
    for (size_t i = 0; i < 6; ++i)
    {
        cout << "(" << i << ", " << crtbuilder.get_slot(plain_scaled_square, i).to_dec_string() << ")" << ((i != 5) ? ", " : "\n");
    }

    // How much noise did we end up with?
    cout << "Noise in the result: " << inherent_noise(encrypted_scaled_square, parms, secret_key).significant_bit_count()
        << "/" << inherent_noise_max(parms).significant_bit_count() << " bits" << endl;
}

void example_relinearization()
{
    print_example_banner("Example: Relinearization");

    /*
    A valid ciphertext consists of at least two polynomials. To read the current size of a ciphertext the
    user can use BigPolyArray::size(). A fresh ciphertext always has size 2, and performing homomorphic multiplication
    results in the output ciphertext growing in size. More precisely, if the input ciphertexts have size M and N,
    then the output ciphertext after homomorphic multiplication will have size M+N-1.

    The multiplication operation on input ciphertexts of size M and N will require M*N polynomial multiplications to be 
    performed. Therefore, the multiplication of large ciphertexts could be very computationally costly and in some situations 
    the user might prefer to reduce the size of the ciphertexts by performing a so-called relinearization operation.

    The function Evaluator::relinearize(...) can reduce the size of an input ciphertext of size M to any size in 
    2, 3, ..., M. Relinearizing one or both of two ciphertexts before performing multiplication on them may significantly
    reduce the computational cost of the multiplication. However, note that the relinearization process also requires 
    several polynomial multiplications to be performed. In particular relinearizing a ciphertext of size K to size L 
    will itself require 2*(K-L)*[floor(log_2(coeff_modulus)/dbc)+1] polynomial multiplications, where dbc is the 
    decomposition_bit_count (see below). It is also important to understand that relinearization grows the inherent noise 
    in a ciphertext by an additive factor proportional to 2^dbc, which can in some cases be very large. When using 
    relinearization it is necessary that the decomposition_bit_count is specified in the encryption parameters, 
    and that enough evaluation keys are given to the constructor of Evaluator. 

    The decomposition_bit_count affects both performance and noise growth in relinearization, as was explained above.
    Simply put, the larger dbc is, the faster relinearization is, and the larger the additive noise growth factor is
    (see above). However, if some multiplications have already been performed on a ciphertext so that the noise has
    grown to some reasonable level, relinearization might have no practical effect anymore on noise due to the additive 
    factor being possibly (much) smaller than what the current noise is. This is why it makes almost never sense to 
    relinearize after the first multiplication since the noise will still be so small that any reasonably large dbc
    would increase the noise by a significant amount. In many cases it might not be beneficial to relinearize at all,
    especially if the computation to be performed amounts to evaluating some fairly low degree polynomial. If the 
    degree is higher, then in some cases it might be beneficial to relinearize at some stage in the computation.
    See below for how to choose a good value for the decomposition_bit_count.

    If the intention of the evaluating party is to hide the structure of the computation that has been performed on 
    the ciphertexts, it might be necessary to relinearize to hide the number of multiplications that the ciphertexts
    have gone through. In addition, after relinearizing (to size 2) it might be a good idea to re-randomize the 
    ciphertext by adding to it a fresh encryption of 0.
   
    In this example we will demonstrate using Evaluator::relinearize(...) and illustrate how it reduces the ciphertext 
    sizes. We will also observe the effects it has on noise.
    */

    // Set up encryption parameters
    EncryptionParameters parms;
    parms.poly_modulus() = "1x^2048 + 1";
    parms.coeff_modulus() = ChooserEvaluator::default_parameter_options().at(2048);
    parms.plain_modulus() = 1 << 16;

    /*
    The choice of decomposition_bit_count (dbc) can affect the performance of relinearization noticeably. A somewhat 
    optimal choice is to choose it between 1/5 and 1/2 of the significant bit count of the coefficient modulus (see 
    table below). It turns out that if dbc cannot (due to noise growth) be more than one fifth of the significant 
    bit count of the coefficient modulus, then it is in fact better to just move up to a larger poly_modulus and 
    coeff_modulus, and set dbc to be as large as possible.
    /--------------------------------------------------------\
    | poly_modulus | coeff_modulus bound | dbc min | dbc max |
    | -------------|---------------------|-------------------|
    | 1x^1024 + 1  | 48 bits             | 10      | 24      |
    | 1x^2048 + 1  | 96 bits             | 20      | 48      |
    | 1x^4096 + 1  | 192 bits            | 39      | 96      |
    | 1x^8192 + 1  | 384 bits            | 77      | 192     |
    | 1x^16384 + 1 | 768 bits            | 154     | 384     |
    \--------------------------------------------------------/

    A smaller decomposition_bit_count will make relinearization slower. A higher decomposition_bit_count will increase
    noise growth while not making relinearization any faster. Here, the coeff_modulus has 96 significant bits, so 
    we choose decomposition_bit_count to be half of this.
    */
    parms.decomposition_bit_count() = 48;

    cout << "Encryption parameters specify " << parms.poly_modulus().significant_coeff_count() << " coefficients with "
        << parms.coeff_modulus().significant_bit_count() << " bits per coefficient" << endl;

    /*
    Generate keys

    By default, KeyGenerator::generate() will generate no evaluation keys. This means that we cannot perform any 
    relinearization. However, this is sufficient for performing all other homomorphic evaluation operations as 
    they do not use evaluation keys.
    */
    cout << "Generating keys..." << endl;
    KeyGenerator generator(parms);
    generator.generate();
    cout << "... key generation complete" << endl;
    BigPolyArray public_key = generator.public_key();
    BigPoly secret_key = generator.secret_key();

    /*
    Suppose we want to homomorphically multiply four ciphertexts together. Does it make sense to relinearize 
    at an intermediate step of the computation? We demonstrate how relinearization at different stages affects
    the results.
    */

    // Encrypt the plaintexts to generate the four fresh ciphertexts
    BigPoly plain1("4");
    BigPoly plain2("3x^1");
    BigPoly plain3("2x^2");
    BigPoly plain4("1x^3");
    cout << "Encrypting values as { encrypted1, encrypted2, encrypted3, encrypted4 }" << endl;
    Encryptor encryptor(parms, public_key);
    BigPolyArray encrypted1 = encryptor.encrypt(plain1);
    BigPolyArray encrypted2 = encryptor.encrypt(plain2);
    BigPolyArray encrypted3 = encryptor.encrypt(plain3);
    BigPolyArray encrypted4 = encryptor.encrypt(plain4);

    // What are the noises in the four ciphertexts?
    cout << "Noises in the four ciphertexts: " 
        << inherent_noise(encrypted1, parms, secret_key).significant_bit_count()
            << "/" << inherent_noise_max(parms).significant_bit_count() << " bits, "
        << inherent_noise(encrypted2, parms, secret_key).significant_bit_count()
            << "/" << inherent_noise_max(parms).significant_bit_count() << " bits, "
        << inherent_noise(encrypted3, parms, secret_key).significant_bit_count()
            << "/" << inherent_noise_max(parms).significant_bit_count() << " bits, "
        << inherent_noise(encrypted4, parms, secret_key).significant_bit_count()
            << "/" << inherent_noise_max(parms).significant_bit_count() << " bits"
        << endl;

    // Construct an Evaluator
    Evaluator evaluator(parms);

    // Perform first part of computation
    cout << "Computing enc_prod1 as encrypted1*encrypted2" << endl;
    BigPolyArray enc_prod1 = evaluator.multiply(encrypted1, encrypted2);
    cout << "Computing enc_prod2 as encrypted3*encrypted4" << endl;
    BigPolyArray enc_prod2 = evaluator.multiply(encrypted3, encrypted4);

    // Now enc_prod1 and enc_prod2 both have size 3
    cout << "Sizes of enc_prod1 and enc_prod2: " << enc_prod1.size() << ", " << enc_prod2.size() << endl;

    // What are the noises in the products?
    cout << "Noises in enc_prod1 and enc_prod2: " 
        << inherent_noise(enc_prod1, parms, secret_key).significant_bit_count()
            << "/" << inherent_noise_max(parms).significant_bit_count() << " bits, "
        << inherent_noise(enc_prod2, parms, secret_key).significant_bit_count()
            << "/" << inherent_noise_max(parms).significant_bit_count() << " bits"
        << endl;

    // Compute product of all four
    cout << "Computing enc_result as enc_prod1*enc_prod2" << endl;
    BigPolyArray enc_result = evaluator.multiply(enc_prod1, enc_prod2);
    
    // Now enc_result has size 5
    cout << "Size of enc_result: " << enc_result.size() << endl;

    // What is the noise in the result?
    cout << "Noise in enc_result: " << inherent_noise(enc_result, parms, secret_key).significant_bit_count()
        << "/" << inherent_noise_max(parms).significant_bit_count() << " bits" << endl;

    /*
    We didn't create any evaluation keys, so we can't relinearize at all with the current Evaluator.
    The size of our final ciphertext enc_result is 5, so for example to relinearize this down to size 2
    we will need 3 evaluation keys. In general, relinearizing down from size K to any smaller size (but at least 2)
    requires at least K-2 evaluation keys, so in this case we will need at least 2 evaluation keys.

    We can create these new evaluation keys by calling KeyGenerator::generate_evaluation_keys(...). Alternatively,
    we could have created them already in the beginning by instead of calling generator.generate(2) instead of
    generator.generate().

    We will also need a new Evaluator, as the previous one was constructed without enough (indeed, any)
    evaluation keys. It is not possible to add new evaluation keys to a previously created Evaluator.
    */
    generator.generate_evaluation_keys(3);
    EvaluationKeys evaluation_keys = generator.evaluation_keys();
    Evaluator evaluator2(parms, evaluation_keys);

    /*
    We can relinearize enc_result back to size 2 if we want to. In fact, we could also relinearize it to size 3 or 4,
    or more generally to any size less than the current size but at least 2. The way to do this would be to call
    Evaluator::relinearize(enc_result, destination_size).
    */
    cout << "Relinearizing enc_result to size 2 (stored in enc_relin_result)" << endl;
    BigPolyArray enc_relin_result = evaluator2.relinearize(enc_result);

    /*
    What did that do to size and noise?
    In fact noise remained essentially the same, because at this point the size of noise is already significantly 
    larger than the additive term contributed by the relinearization process. We still remain below the noise bound.
    */
    cout << "Size of enc_relin_result: " << enc_relin_result.size() << endl;
    cout << "Noise in enc_relin_result: " << inherent_noise(enc_relin_result, parms, secret_key).significant_bit_count()
        << "/" << inherent_noise_max(parms).significant_bit_count() << " bits" << endl;

    // What if we do intermediate relinearization of enc_prod1 and enc_prod2?
    cout << "Relinearizing enc_prod1 and enc_prod2 to size 2" << endl;
    BigPolyArray enc_relin_prod1 = evaluator2.relinearize(enc_prod1);
    BigPolyArray enc_relin_prod2 = evaluator2.relinearize(enc_prod2);

    // What happened to sizes and noises? Noises grew by a significant amount!
    cout << "Sizes of enc_relin_prod1 and enc_relin_prod2: " << enc_relin_prod1.size() << ", " << enc_relin_prod2.size() << endl;
    cout << "Noises in enc_relin_prod1 and enc_relin_prod2: "
        << inherent_noise(enc_relin_prod1, parms, secret_key).significant_bit_count()
            << "/" << inherent_noise_max(parms).significant_bit_count() << " bits, "
        << inherent_noise(enc_relin_prod2, parms, secret_key).significant_bit_count()
            << "/" << inherent_noise_max(parms).significant_bit_count() << " bits"
        << endl;

    // Now multiply the relinearized products together
    cout << "Computing enc_intermediate_relin_result as enc_relin_prod1*enc_relin_prod2" << endl;
    BigPolyArray enc_intermediate_relin_result = evaluator2.multiply(enc_relin_prod1, enc_relin_prod2);

    /* 
    What did that do to size and noise?
    We are above the noise bound in this case. The resulting ciphertext is corrupted. It is instructive to 
    try and see how a smaller decomposition_bit_count affects the results, e.g. try setting it to 24. 
    Also here plain_modulus was set to be quite large to emphasize the effect.
    */
    cout << "Size of enc_intermediate_relin_result: " << enc_intermediate_relin_result.size() << endl;
    cout << "Noise in enc_intermediate_relin_result: " << inherent_noise(enc_intermediate_relin_result, parms, secret_key).significant_bit_count()
        << "/" << inherent_noise_max(parms).significant_bit_count() << " bits" << endl;
}