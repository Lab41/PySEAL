using System;
using Microsoft.Research.SEAL;

using System.Collections.Generic;

namespace SEALNETExamples
{
    class Program
    {
        static void PrintExampleBanner(string title)
        {
            if (!String.IsNullOrEmpty(title))
            {
                var titleLength = title.Length;
                var bannerLength = titleLength + 2 + 2 * 10;
                var bannerTop = new string('*', bannerLength);
                var bannerMiddle = new string('*', 10) + " " + title + " " + new string('*', 10);

                Console.WriteLine("\n{0}\n{1}\n{2}\n", bannerTop, bannerMiddle, bannerTop);
            }
        }

        static void Main()
        {
            // Example: Basics
            ExampleBasics();

            // Example: Weighted Average
            ExampleWeightedAverage();

            // Example: Automatic Parameter Selection
            ExampleParameterSelection();

            // Wait for ENTER before closing screen.
            Console.WriteLine("Press ENTER to exit");
            Console.ReadLine();
        }

        static void ExampleBasics()
        {
            PrintExampleBanner("Example: Basics");

            // In this example we demonstrate using some of the basic arithmetic operations on integers.

            // Create encryption parameters.
            var parms = new EncryptionParameters();

            /*
            First choose the polynomial modulus. This must be a power-of-2 cyclotomic polynomial,
            i.e. a polynomial of the form "1x^(power-of-2) + 1". We recommend using polynomials of
            degree at least 1024.
            */
            parms.PolyModulus.Set("1x^4096 + 1");

            /*
            Next choose the coefficient modulus. The values we recommend to be used are:

            [ degree(poly_modulus), coeff_modulus ]
            [ 1024, "FFFFFFFFC001" ],
            [ 2048, "7FFFFFFFFFFFFFFFFFFF001"],
            [ 4096, "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"],
            [ 8192, "1FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC000001"],
            [ 16384, "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000000001"].

            These can be conveniently accessed using ChooserEvaluator.DefaultParameterOptions(),
            which returns the above list of options as a Dictionary, keyed by the degree of the polynomial modulus.
            */
            parms.CoeffModulus.Set(ChooserEvaluator.DefaultParameterOptions[4096]);

            /*
            Now we set the plaintext modulus. This can be any integer, even though here we take it to be a power of two.
            A larger plaintext modulus causes the noise to grow faster in homomorphic multiplication, and
            also lowers the maximum amount of noise in ciphertexts that the system can tolerate.
            On the other hand, a larger plaintext modulus typically allows for better homomorphic integer arithmetic,
            although this depends strongly on which encoder is used to encode integers into plaintext polynomials.
            */
            parms.PlainModulus.Set(1 << 10);

            /*
            The decomposition bit count affects the behavior of the relinearization (key switch) operation,
            which is typically performed after each homomorphic multiplication. A smaller decomposition
            bit count makes relinearization slower, but improves the noise growth behavior on multiplication.
            Conversely, a larger decomposition bit count makes homomorphic multiplication faster at the cost
            of increased noise growth.
            */
            parms.DecompositionBitCount = 32;

            /*
            We use a constant standard deviation for the error distribution. Using a larger standard
            deviation will result in larger noise growth, but in theory should make the system more secure.
            */
            parms.NoiseStandardDeviation = ChooserEvaluator.DefaultNoiseStandardDeviation;

            // For the bound on the error distribution we can take for instance 5 * standard_deviation.
            parms.NoiseMaxDeviation = 5 * parms.NoiseStandardDeviation;

            Console.WriteLine("Encryption parameters specify {0} coefficients with {1} bits per coefficient",
                parms.PolyModulus.GetSignificantCoeffCount(), parms.CoeffModulus.GetSignificantBitCount());

            // Encode two integers as polynomials.
            const int value1 = 5;
            const int value2 = -7;
            var encoder = new BalancedEncoder(parms.PlainModulus);
            var encoded1 = encoder.Encode(value1);
            var encoded2 = encoder.Encode(value2);
            Console.WriteLine("Encoded {0} as polynomial {1}", value1, encoded1);
            Console.WriteLine("Encoded {0} as polynomial {1}", value2, encoded2);

            // Generate keys.
            Console.WriteLine("Generating keys...");
            var generator = new KeyGenerator(parms);
            generator.Generate();
            Console.WriteLine("... key generation completed");
            var publicKey = generator.PublicKey;
            var secretKey = generator.SecretKey;
            var evaluationKeys = generator.EvaluationKeys;
            //Console.WriteLine("Public Key = {0}", publicKey);
            //Console.WriteLine("Secret Key = {0}", secretKey);

            // Encrypt values.
            Console.WriteLine("Encrypting values...");
            var encryptor = new Encryptor(parms, publicKey);
            var encrypted1 = encryptor.Encrypt(encoded1);
            var encrypted2 = encryptor.Encrypt(encoded2);

            // Perform arithmetic on encrypted values.
            Console.WriteLine("Performing encrypted arithmetic...");
            var evaluator = new Evaluator(parms, evaluationKeys);
            Console.WriteLine("... Performing negation...");
            var encryptedNegated1 = evaluator.Negate(encrypted1);
            Console.WriteLine("... Performing addition...");
            var encryptedSum = evaluator.Add(encrypted1, encrypted2);
            Console.WriteLine("... Performing subtraction...");
            var encryptedDiff = evaluator.Sub(encrypted1, encrypted2);
            Console.WriteLine("... Performing multiplication...");
            var encryptedProduct = evaluator.Multiply(encrypted1, encrypted2);

            // Decrypt results.
            Console.WriteLine("Decrypting results...");
            var decryptor = new Decryptor(parms, secretKey);
            var decrypted1 = decryptor.Decrypt(encrypted1);
            var decrypted2 = decryptor.Decrypt(encrypted2);
            var decryptedNegated1 = decryptor.Decrypt(encryptedNegated1);
            var decryptedSum = decryptor.Decrypt(encryptedSum);
            var decryptedDiff = decryptor.Decrypt(encryptedDiff);
            var decryptedProduct = decryptor.Decrypt(encryptedProduct);

            // Decode results.
            var decoded1 = encoder.DecodeInt32(decrypted1);
            var decoded2 = encoder.DecodeInt32(decrypted2);
            var decodedNegated1 = encoder.DecodeInt32(decryptedNegated1);
            var decodedSum = encoder.DecodeInt32(decryptedSum);
            var decodedDiff = encoder.DecodeInt32(decryptedDiff);
            var decodedProduct = encoder.DecodeInt32(decryptedProduct);

            // Display results.
            Console.WriteLine("{0} after encryption/decryption = {1}", value1, decoded1);
            Console.WriteLine("{0} after encryption/decryption = {1}", value2, decoded2);
            Console.WriteLine("encrypted negate of {0} = {1}", value1, decodedNegated1);
            Console.WriteLine("encrypted addition of {0} and {1} = {2}", value1, value2, decodedSum);
            Console.WriteLine("encrypted subtraction of {0} and {1} = {2}", value1, value2, decodedDiff);
            Console.WriteLine("encrypted multiplication of {0} and {1} = {2}", value1, value2, decodedProduct);

            // How did the noise grow in these operations?
            var maxNoiseBitCount = Utilities.InherentNoiseMax(parms).GetSignificantBitCount();
            Console.WriteLine("Noise in encryption of {0}: {1}/{2} bits", value1, Utilities.InherentNoise(encrypted1, parms, secretKey).GetSignificantBitCount(), maxNoiseBitCount);
            Console.WriteLine("Noise in encryption of {0}: {1}/{2} bits", value2, Utilities.InherentNoise(encrypted1, parms, secretKey).GetSignificantBitCount(), maxNoiseBitCount);
            Console.WriteLine("Noise in the sum: {0}/{1} bits", Utilities.InherentNoise(encryptedSum, parms, secretKey).GetSignificantBitCount(), maxNoiseBitCount);
            Console.WriteLine("Noise in the product: {0}/{1} bits", Utilities.InherentNoise(encryptedProduct, parms, secretKey).GetSignificantBitCount(), maxNoiseBitCount);
        }

        static void ExampleWeightedAverage()
        {
            PrintExampleBanner("Example: Weighted Average");

            // In this example we demonstrate computing a weighted average of 10 rational numbers.

            // The 10 rational numbers we use are:
            var rationalNumbers = new List<double> { 3.1, 4.159, 2.65, 3.5897, 9.3, 2.3, 8.46, 2.64, 3.383, 2.795 };

            // The 10 weights are:
            var coefficients = new List<double> { 0.1, 0.05, 0.05, 0.2, 0.05, 0.3, 0.1, 0.025, 0.075, 0.05 };

            // Create encryption parameters.
            var parms = new EncryptionParameters();

            parms.PolyModulus.Set("1x^2048 + 1");
            parms.CoeffModulus.Set(ChooserEvaluator.DefaultParameterOptions[2048]);
            parms.PlainModulus.Set(1 << 8);

            // Since we are not doing any encrypted*encrypted multiplication in this example,
            // the decomposition bit count has no practical significance.
            parms.DecompositionBitCount= 32;

            parms.NoiseStandardDeviation = ChooserEvaluator.DefaultNoiseStandardDeviation;
            parms.NoiseMaxDeviation = 5 * parms.NoiseStandardDeviation;

            Console.WriteLine("Encryption parameters specify {0} coefficients with {1} bits per coefficient",
                parms.PolyModulus.GetSignificantCoeffCount(), parms.CoeffModulus.GetSignificantBitCount());

            // Generate keys.
            Console.WriteLine("Generating keys...");
            var generator = new KeyGenerator(parms);
            generator.Generate();
            Console.WriteLine("... key generation completed");
            var publicKey = generator.PublicKey;
            var secretKey = generator.SecretKey;
            var evaluationKeys = generator.EvaluationKeys;

            /*
            We will need a fractional encoder for dealing with the rational numbers.
            Here we reserve 128 coefficients of the polynomial for the integral part (low-degree terms)
            and 64 coefficients for the fractional part (high-degree terms).
            */
            var encoder = new BalancedFractionalEncoder(parms.PlainModulus, parms.PolyModulus, 128, 64);

            // Create the rest of the tools
            var encryptor = new Encryptor(parms, publicKey);
            var evaluator = new Evaluator(parms, evaluationKeys);
            var decryptor = new Decryptor(parms, secretKey);

            // First we encrypt the rational numbers
            Console.Write("Encrypting ... ");
            var encryptedRationals = new List<BigPoly>();
            for (int i = 0; i < 10; ++i)
            {
                var encodedNumber = encoder.Encode(rationalNumbers[i]);
                encryptedRationals.Add(encryptor.Encrypt(encodedNumber));
                Console.Write(rationalNumbers[i]);
                if (i < 9)
                {
                    Console.Write(", ");
                }
            }
            Console.WriteLine(".");

            // Next we encode the coefficients. There is no reason to encrypt these since they are not private data.
            Console.Write("Encoding ... ");
            var encodedCoefficients = new List<BigPoly>();
            for (int i = 0; i < 10; ++i)
            {
                encodedCoefficients.Add(encoder.Encode(coefficients[i]));
                Console.Write(coefficients[i]);
                if (i < 9)
                {
                    Console.Write(", ");
                }
            }
            Console.WriteLine(".");

            // We also need to encode 0.1. We will multiply the result by this to perform division by 10.
            var divByTen = encoder.Encode(0.1);

            // Now compute all the products of the encrypted rational numbers with the plaintext coefficients
            Console.Write("Computing products ... ");
            var encryptedProducts = new List<BigPoly>();
            for (int i = 0; i < 10; ++i)
            {
                /*
                We use Evaluator.MultiplyPlain(...) instead of Evaluator.Multiply(...) (which would 
                require also the coefficient to be encrypted). This has much better noise growth
                behavior than multiplying two encrypted numbers does.
                */
                var encPlainProduct = evaluator.MultiplyPlain(encryptedRationals[i], encodedCoefficients[i]);
                encryptedProducts.Add(encPlainProduct);
            }
            Console.WriteLine("done.");

            // Now we add together these products. The most convenient way to do that is
            // to use the function Evaluator.AddMany(...).
            Console.Write("Add up all 10 ciphertexts ... ");
            var encryptedDotProduct = evaluator.AddMany(encryptedProducts);
            Console.WriteLine("done");

            // Finally we divide by 10 to obtain the result.
            Console.Write("Divide by 10 ... ");
            var encryptedResult = evaluator.MultiplyPlain(encryptedDotProduct, divByTen);
            Console.WriteLine("done");

            // Decrypt
            Console.Write("Decrypting ... ");
            var plainResult = decryptor.Decrypt(encryptedResult);
            Console.WriteLine("done");

            // Print the answer
            double result = encoder.Decode(plainResult);
            Console.WriteLine("Weighted average: {0}", result);

            // How much noise did we end up with?
            Console.WriteLine("Noise in the result: {0}/{1} bits", Utilities.InherentNoise(encryptedResult, parms, secretKey).GetSignificantBitCount(), 
                Utilities.InherentNoiseMax(parms).GetSignificantBitCount());
        }

        static void ExampleParameterSelection()
        {
            PrintExampleBanner("Example: Automatic Parameter Selection");

            /*
            Here we demonstrate the automatic parameter selection tool. Suppose we want to find parameters
            that are optimized in a way that allows us to evaluate the polynomial 42x^3-27x+1. We need to know
            the size of the input data, so let's assume that x is an integer with base-3 representation of length
            at most 10.
            */
            Console.Write("Finding optimized parameters for computing 42x^3-27x+1 ... ");

            var chooserEncoder = new ChooserEncoder();
            var chooserEvaluator = new ChooserEvaluator();

            // First create a ChooserPoly representing the input data. You can think of this modeling a freshly
            // encrypted cipheretext of a plaintext polynomial with length at most 10 coefficients, where the
            // coefficients have absolute value at most 1.
            var cinput = new ChooserPoly(10, 1);

            // Compute the first term
            var ccubedInput = chooserEvaluator.Exponentiate(cinput, 3);
            var cterm1 = chooserEvaluator.MultiplyPlain(ccubedInput, chooserEncoder.Encode(42));

            // Compute the second term
            var cterm2 = chooserEvaluator.MultiplyPlain(cinput, chooserEncoder.Encode(27));

            // Subtract the first two terms
            var csum12 = chooserEvaluator.Sub(cterm1, cterm2);

            // Add the constant term 1
            var cresult = chooserEvaluator.AddPlain(csum12, chooserEncoder.Encode(1));

            // To find an optimized set of parameters, we use ChooserEvaluator::select_parameters(...).
            var optimalParms = new EncryptionParameters();
            chooserEvaluator.SelectParameters(cresult, optimalParms);

            Console.WriteLine("done.");

            // Let's print these to see what was recommended
            Console.WriteLine("Selected parameters:");
            Console.WriteLine("{{ poly_modulus: {0}", optimalParms.PolyModulus.ToString());
            Console.WriteLine("{{ coeff_modulus: {0}", optimalParms.CoeffModulus.ToString());
            Console.WriteLine("{{ plain_modulus: {0}", optimalParms.PlainModulus.ToDecimalString());
            Console.WriteLine("{{ decomposition_bit_count: {0}", optimalParms.DecompositionBitCount);
            Console.WriteLine("{{ noise_standard_deviation: {0}", optimalParms.NoiseStandardDeviation);
            Console.WriteLine("{{ noise_max_deviation: {0}", optimalParms.NoiseMaxDeviation);

            // Let's try to actually perform the homomorphic computation using the recommended parameters.
            // Generate keys.
            Console.WriteLine("Generating keys...");
            var generator = new KeyGenerator(optimalParms);
            generator.Generate();
            Console.WriteLine("... key generation completed");
            var publicKey = generator.PublicKey;
            var secretKey = generator.SecretKey;
            var evaluationKeys = generator.EvaluationKeys;

            // Create the encoding/encryption tools
            var encoder = new BalancedEncoder(optimalParms.PlainModulus);
            var encryptor = new Encryptor(optimalParms, publicKey);
            var evaluator = new Evaluator(optimalParms, evaluationKeys);
            var decryptor = new Decryptor(optimalParms, secretKey);

            // Now perform the computations on real encrypted data.
            const int inputValue = 12345;
            var plainInput = encoder.Encode(inputValue);
            Console.WriteLine("Encoded {0} as polynomial {1}", inputValue, plainInput.ToString());

            Console.Write("Encrypting ... ");
            var input = encryptor.Encrypt(plainInput);
            Console.WriteLine("done.");

            // Compute the first term
            Console.Write("Computing first term ... ");
            var cubedInput = evaluator.Exponentiate(input, 3);
            var term1 = evaluator.MultiplyPlain(cubedInput, encoder.Encode(42));
            Console.WriteLine("done.");

            // Compute the second term
            Console.Write("Computing second term ... ");
            var term2 = evaluator.MultiplyPlain(input, encoder.Encode(27));
            Console.WriteLine("done.");

            // Subtract the first two terms
            Console.Write("Subtracting first two terms ... ");
            var sum12 = evaluator.Sub(term1, term2);
            Console.WriteLine("done.");

            // Add the constant term 1
            Console.Write("Adding one ... ");
            var result = evaluator.AddPlain(sum12, encoder.Encode(1));
            Console.WriteLine("done.");

            // Decrypt and decode
            Console.Write("Decrypting ... ");
            var plainResult = decryptor.Decrypt(result);
            Console.WriteLine("done.");

            // Finally print the result
            Console.WriteLine("Polynomial 42x^3-27x+1 evaluated at x=12345: {0}", encoder.DecodeUInt64(plainResult));

            // How much noise did we end up with?
            Console.WriteLine("Noise in the result: {0}/{1} bits", Utilities.InherentNoise(result, optimalParms, secretKey).GetSignificantBitCount(),
                Utilities.InherentNoiseMax(optimalParms).GetSignificantBitCount());
        }
    }
}
