#include <iostream>
#include "seal.h"

#include <vector>

using namespace std;
using namespace seal;

void print_example_banner(string title);

void example_basics();
void example_weighted_average();
void example_parameter_selection();

int main()
{
    // Example: Basics
    example_basics();

    // Example: Weighted Average
    example_weighted_average();

    // Example: Automatic Parameter Selection
    example_parameter_selection();

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

    // In this example we demonstrate using some of the basic arithmetic operations on integers.

    // Create encryption parameters.
    EncryptionParameters parms;

    /*
    First choose the polynomial modulus. This must be a power-of-2 cyclotomic polynomial,
    i.e. a polynomial of the form "1x^(power-of-2) + 1". We recommend using polynomials of
    degree at least 1024.
    */
    parms.poly_modulus() = "1x^4096 + 1";

    /*
    Next choose the coefficient modulus. The values we recommend to be used are:

    [ degree(poly_modulus), coeff_modulus ]
    [ 1024, "FFFFFFFFC001" ],
    [ 2048, "7FFFFFFFFFFFFFFFFFFF001"],
    [ 4096, "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"],
    [ 8192, "1FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC000001"],
    [ 16384, "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000000001"].

    These can be conveniently accessed using ChooserEvaluator::default_parameter_options(),
    which returns the above list of options as an std::map, keyed by the degree of the polynomial modulus.
    */
    parms.coeff_modulus() = ChooserEvaluator::default_parameter_options().at(4096);

    /*
    Now we set the plaintext modulus. This can be any integer, even though here we take it to be a power of two.
    A larger plaintext modulus causes the noise to grow faster in homomorphic multiplication, and
    also lowers the maximum amount of noise in ciphertexts that the system can tolerate.
    On the other hand, a larger plaintext modulus typically allows for better homomorphic integer arithmetic,
    although this depends strongly on which encoder is used to encode integers into plaintext polynomials.
    */
    parms.plain_modulus() = 1 << 10;

    /*
    The decomposition bit count affects the behavior of the relinearization (key switch) operation,
    which is typically performed after each homomorphic multiplication. A smaller decomposition
    bit count makes relinearization slower, but improves the noise growth behavior on multiplication.
    Conversely, a larger decomposition bit count makes homomorphic multiplication faster at the cost
    of increased noise growth.
    */
    parms.decomposition_bit_count() = 32;

    /*
    We use a constant standard deviation for the error distribution. Using a larger standard
    deviation will result in larger noise growth, but in theory should make the system more secure.
    */
    parms.noise_standard_deviation() = ChooserEvaluator::default_noise_standard_deviation();

    // For the bound on the error distribution we can take for instance 5 * standard_deviation.
    parms.noise_max_deviation() = 5 * parms.noise_standard_deviation();

    cout << "Encryption parameters specify " << parms.poly_modulus().significant_coeff_count() << " coefficients with "
        << parms.coeff_modulus().significant_bit_count() << " bits per coefficient" << endl;

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
    BigPoly public_key = generator.public_key();
    BigPoly secret_key = generator.secret_key();
    EvaluationKeys evaluation_keys = generator.evaluation_keys();
    //cout << "Public Key = " << public_key.to_string() << endl;
    //cout << "Secret Key = " << secret_key.to_string() << endl;

    // Encrypt values.
    cout << "Encrypting values..." << endl;
    Encryptor encryptor(parms, public_key);
    BigPoly encrypted1 = encryptor.encrypt(encoded1);
    BigPoly encrypted2 = encryptor.encrypt(encoded2);

    // Perform arithmetic on encrypted values.
    cout << "Performing encrypted arithmetic..." << endl;
    Evaluator evaluator(parms, evaluation_keys);
    cout << "... Performing negation..." << endl;
    BigPoly encryptednegated1 = evaluator.negate(encrypted1);
    cout << "... Performing addition..." << endl;
    BigPoly encryptedsum = evaluator.add(encrypted1, encrypted2);
    cout << "... Performing subtraction..." << endl;
    BigPoly encrypteddiff = evaluator.sub(encrypted1, encrypted2);
    cout << "... Performing multiplication..." << endl;
    BigPoly encryptedproduct = evaluator.multiply(encrypted1, encrypted2);

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

    parms.poly_modulus() = "1x^2048 + 1";
    parms.coeff_modulus() = ChooserEvaluator::default_parameter_options().at(2048);
    parms.plain_modulus() = 1 << 8;

    // Since we are not doing any encrypted*encrypted multiplication in this example,
    // the decomposition bit count has no practical significance.
    parms.decomposition_bit_count() = 32;

    parms.noise_standard_deviation() = ChooserEvaluator::default_noise_standard_deviation();
    parms.noise_max_deviation() = 5 * parms.noise_standard_deviation();

    cout << "Encryption parameters specify " << parms.poly_modulus().significant_coeff_count() << " coefficients with "
        << parms.coeff_modulus().significant_bit_count() << " bits per coefficient" << endl;

    // Generate keys.
    cout << "Generating keys..." << endl;
    KeyGenerator generator(parms);
    generator.generate();
    cout << "... key generation complete" << endl;
    BigPoly public_key = generator.public_key();
    BigPoly secret_key = generator.secret_key();
    EvaluationKeys evaluation_keys = generator.evaluation_keys();

    /*
    We will need a fractional encoder for dealing with the rational numbers.
    Here we reserve 128 coefficients of the polynomial for the integral part (low-degree terms)
    and 64 coefficients for the fractional part (high-degree terms).
    */
    BalancedFractionalEncoder encoder(parms.plain_modulus(), parms.poly_modulus(), 128, 64);

    // Create the rest of the tools
    Encryptor encryptor(parms, public_key);
    Evaluator evaluator(parms, evaluation_keys);
    Decryptor decryptor(parms, secret_key);

    // First we encrypt the rational numbers
    cout << "Encrypting ... ";
    vector<BigPoly> encrypted_rationals;
    for (int i = 0; i < 10; ++i)
    {
        BigPoly encoded_number = encoder.encode(rational_numbers[i]);
        encrypted_rationals.push_back(encryptor.encrypt(encoded_number));
        cout << rational_numbers[i];
        if (i < 9)
        {
            cout << ", ";
        }
    }
    cout << "." << endl;

    // Next we encode the coefficients. There is no reason to encrypt these since they are not private data.
    cout << "Encoding ... ";
    vector<BigPoly> encoded_coefficients;
    for (int i = 0; i < 10; ++i)
    {
        encoded_coefficients.push_back(encoder.encode(coefficients[i]));
        cout << coefficients[i];
        if (i < 9)
        {
            cout << ", ";
        }
    }
    cout << ". " << endl;
    
    // We also need to encode 0.1. We will multiply the result by this to perform division by 10.
    BigPoly div_by_ten = encoder.encode(0.1);

    // Now compute all the products of the encrypted rational numbers with the plaintext coefficients
    cout << "Computing products ... ";
    vector<BigPoly> encrypted_products;
    for (int i = 0; i < 10; ++i)
    {
        /*
        We use Evaluator::multiply_plain(...) instead of Evaluator::multiply(...) (which would 
        require also the coefficient to be encrypted). This has much better noise growth
        behavior than multiplying two encrypted numbers does.
        */
        BigPoly enc_plain_product = evaluator.multiply_plain(encrypted_rationals[i], encoded_coefficients[i]);
        encrypted_products.push_back(enc_plain_product);
    }
    cout << "done." << endl;

    // Now we add together these products. The most convenient way to do that is
    // to use the function Evaluator::add_many(...).
    cout << "Add up all 10 ciphertexts ... ";
    BigPoly encrypted_dot_product = evaluator.add_many(encrypted_products);
    cout << " done." << endl;

    // Finally we divide by 10 to obtain the result.
    cout << "Divide by 10 ... ";
    BigPoly encrypted_result = evaluator.multiply_plain(encrypted_dot_product, div_by_ten);
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
    
    // First create a ChooserPoly representing the input data. You can think of this modeling a freshly
    // encrypted cipheretext of a plaintext polynomial with length at most 10 coefficients, where the
    // coefficients have absolute value at most 1.
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
    BigPoly public_key = generator.public_key();
    BigPoly secret_key = generator.secret_key();
    EvaluationKeys evaluation_keys = generator.evaluation_keys();

    // Create the encoding/encryption tools
    BalancedEncoder encoder(optimal_parms.plain_modulus());
    Encryptor encryptor(optimal_parms, public_key);
    Evaluator evaluator(optimal_parms, evaluation_keys);
    Decryptor decryptor(optimal_parms, secret_key);

    // Now perform the computations on real encrypted data.
    const int input_value = 12345;
    BigPoly plain_input = encoder.encode(input_value);
    cout << "Encoded " << input_value << " as polynomial " << plain_input.to_string() << endl;

    cout << "Encrypting ... ";
    BigPoly input = encryptor.encrypt(plain_input);
    cout << "done." << endl;

    // Compute the first term
    cout << "Computing first term ... ";
    BigPoly cubed_input = evaluator.exponentiate(input, 3);
    BigPoly term1 = evaluator.multiply_plain(cubed_input, encoder.encode(42));
    cout << "done." << endl;

    // Compute the second term
    cout << "Computing second term ... ";
    BigPoly term2 = evaluator.multiply_plain(input, encoder.encode(27));
    cout << "done." << endl;

    // Subtract the first two terms
    cout << "Subtracting first two terms ... ";
    BigPoly sum12 = evaluator.sub(term1, term2);
    cout << "done." << endl;

    // Add the constant term 1
    cout << "Adding one ... ";
    BigPoly result = evaluator.add_plain(sum12, encoder.encode(1));
    cout << "done." << endl;

    // Decrypt and decode
    cout << "Decrypting ... ";
    BigPoly plain_result = decryptor.decrypt(result);
    cout << "done." << endl;
    
    // Finally print the result
    cout << "Polynomial 42x^3-27x+1 evaluated at x=12345: " << encoder.decode_uint64(plain_result) << endl;

    // How much noise did we end up with?
    cout << "Noise in the result: " << inherent_noise(result, optimal_parms, secret_key).significant_bit_count()
        << "/" << inherent_noise_max(optimal_parms).significant_bit_count() << " bits" << endl;
};
