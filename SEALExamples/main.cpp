#include <iostream>
#include "seal.h"

using namespace std;
using namespace seal;

void basic_example();

int main()
{
    // Execute example.
    basic_example();

    // Wait for ENTER before closing screen.
    cout << "Press ENTER to exit" << endl;
    char ignore;
    cin.get(ignore);
    return 0;
}

void basic_example()
{
    // Create encryption parameters.
    EncryptionParameters parms;
    parms.coeff_modulus() = "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF";
    parms.plain_modulus() = 1 << 10;
    parms.poly_modulus() = "1x^4096 + 1";
    parms.decomposition_bit_count() = 32;
    parms.noise_standard_deviation() = 3.19;
    parms.noise_max_deviation() = 35.06;
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
}
