using System;
using Microsoft.Research.SEAL;

using System.Collections.Generic;

namespace SEALNETExamples
{
    public class Program
    {
        public static void PrintExampleBanner(string title)
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

        public static void Main()
        {
            // Example: Basics
            ExampleBasics();

            // Example: Weighted Average
            ExampleWeightedAverage();

            // Example: Automatic Parameter Selection
            ExampleParameterSelection();

            // Example: Batching using CRT
            ExampleBatching();

            // Example: Relinearization
            ExampleRelinearization();

            // Wait for ENTER before closing screen.
            Console.WriteLine("Press ENTER to exit");
            Console.ReadLine();
        }

        public static void ExampleBasics()
        {
            PrintExampleBanner("Example: Basics");

            /*
            In this example we demonstrate using some of the basic arithmetic operations on integers.

            SEAL uses the Fan-Vercauteren (FV) homomorphic encryption scheme. We refer to
            https://eprint.iacr.org/2012/144 for full details on how the FV scheme works.
            */

            // Create encryption parameters.
            var parms = new EncryptionParameters();

            /*
            First choose the polynomial modulus. This must be a power-of-2 cyclotomic polynomial,
            i.e. a polynomial of the form "1x^(power-of-2) + 1". We recommend using polynomials of
            degree at least 1024.
            */
            parms.PolyModulus.Set("1x^2048 + 1");

            /*
            Next choose the coefficient modulus. The values we recommend to be used are:

            [ degree(PolyModulus), CoeffModulus ]
            [ 1024, "FFFFFFF00001" ],
            [ 2048, "3FFFFFFFFFFFFFFFFFF00001"],
            [ 4096, "3FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC0000001"],
            [ 8192, "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE00000001"],
            [ 16384, "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000000001"].
            
            These can be conveniently accessed using ChooserEvaluator.DefaultParameterOptions, which returns 
            the above list of options as a Dictionary, keyed by the degree of the polynomial modulus.
            
            The user can also relatively easily choose their custom coefficient modulus. It should be a prime number
            of the form 2^A - 2^B + 1, where A > B > degree(PolyModulus). Moreover, B should be as small as possible
            for improved efficiency in modular reduction. For security, we recommend strictly adhering to the following 
            size bounds: (see Lepoint-Naehrig (2014) [https://eprint.iacr.org/2014/062])
            /-----------------------------------------------------------------\
            | PolyModulus  | CoeffModulus bound | default CoeffModulus        |
            | -------------|--------------------|-----------------------------|
            | 1x^1024 + 1  | 48 bits            | 2^48 - 2^20 + 1 (47 bits)   |
            | 1x^2048 + 1  | 96 bits            | 2^94 - 2^20 + 1 (93 bits)   |
            | 1x^4096 + 1  | 192 bits           | 2^190 - 2^30 + 1 (189 bits) |
            | 1x^8192 + 1  | 384 bits           | 2^383 - 2^33 + 1 (382 bits) |
            | 1x^16384 + 1 | 768 bits           | 2^767 - 2^56 + 1 (766 bits) |
            \-----------------------------------------------------------------/

            The size of CoeffModulus affects the upper bound on the "inherent noise" that a ciphertext can contain
            before becoming corrupted. More precisely, every ciphertext starts with a certain amount of noise in it,
            which grows in all homomorphic operations - in particular in multiplication. Once a ciphertext contains
            too much noise, it becomes impossible to decrypt. The upper bound on the noise is roughly given by 
            CoeffModulus/PlainModulus (see below), so increasing CoeffModulus will allow the user to perform more
            homomorphic operations on the ciphertexts without corrupting them. We would like to stress, however, that
            the bounds given above for CoeffModulus should absolutely not be exceeded.
            */
            parms.CoeffModulus.Set(ChooserEvaluator.DefaultParameterOptions[2048]);

            /*
            Now we set the plaintext modulus. This can be any positive integer, even though here we take it to be a 
            power of two. A larger plaintext modulus causes the noise to grow faster in homomorphic multiplication, 
            and also lowers the maximum amount of noise in ciphertexts that the system can tolerate (see above).
            On the other hand, a larger plaintext modulus typically allows for better homomorphic integer arithmetic,
            although this depends strongly on which encoder is used to encode integers into plaintext polynomials.
            */
            parms.PlainModulus.Set(1 << 8);

            Console.WriteLine("Encryption parameters specify {0} coefficients with {1} bits per coefficient",
                parms.PolyModulus.GetSignificantCoeffCount(), parms.CoeffModulus.GetSignificantBitCount());

            /*
            Plaintext elements in the FV scheme are polynomials (represented by the BigPoly class) with coefficients 
            integers modulo plain_modulus. To encrypt integers instead, one must use an "encoding scheme", i.e. 
            a specific way of representing integers as such polynomials. SEAL comes with a few basic encoders:

            BinaryEncoder:
            Encodes positive integers as plaintext polynomials where the coefficients are either 0 or 1 according
            to the binary representation of the integer to be encoded. Decoding amounts to evaluating the polynomial
            at x=2. For example, the integer 26 = 2^4 + 2^3 + 2^1 is encoded as the polynomial 1x^4 + 1x^3 + 1x^1.
            Negative integers are encoded similarly but with each coefficient coefficient of the polynomial replaced 
            with its negative modulo PlainModulus.

            BalancedEncoder:
            Given an odd integer base b, encodes integers as plaintext polynomials where the coefficients are according
            to the "balanced" base b representation of the integer to be encoded, i.e. where each coefficient is in the
            range -(b-1)/2,...,(b-1)/2. Decoding amounts to evaluating the polynomial at x=b. For example, when b=3 the 
            integer 25 = 3^3 - 3^1 + 3^0 is encoded as the polynomial 1x^3 - 1x^1 + 1.

            BinaryFractionalEncoder:
            Encodes rational numbers as follows. First represent the number in binary, possibly truncating an infinite
            fractional part to some fixed precision, e.g. 26.75 = 2^4 + 2^3 + 2^1 + 2^(-1) + 2^(-2). For the sake of
            the example, suppose PolyModulus is 1x^1024 + 1. Next represent the integer part of the number in the same
            was as in BinaryEncoder. Finally, represent the fractional part in the leading coefficients of the polynomial, 
            but when doing so invert the signs of the coefficients. So in this example we would represent 26.75 as the 
            polynomial -1x^1023 - 1x^1022 + 1x^4 + 1x^3 + 1x^1. The negative coefficients of the polynomial will again be 
            represented as their negatives modulo PlainModulus.

            BalancedFractionalEncoder:
            Same as BinaryFractionalEncoder, except instead of using base 2 uses any odd base b and balanced 
            representatives for the coefficients, i.e. integers in the range -(b-1)/2,...,(b-1)/2.

            PolyCRTBuilder:
            If PolyModulus is 1x^n + 1, PolyCRTBuilder allows "batching" of n plaintext integers modulo PlainModulus 
            into one plaintext polynomial, where homomorphic operations can be carried out very efficiently in a SIMD 
            manner by operating on such a "composed" plaintext or ciphertext polynomials. For full details on this very
            powerful technique we recommend https://eprint.iacr.org/2012/565.pdf and https://eprint.iacr.org/2011/133.

            A crucial fact to understand is that when homomorphic operations are performed on ciphertexts, they will
            carry over to the underlying plaintexts, and as a result of additions and multiplications the coefficients
            in the plaintext polynomials will increase from what they originally were in freshly encoded polynomials.
            This becomes a problem when the coefficients reach the size of PlainModulus, in which case they will get
            automatically reduced modulo PlainModulus, and might render the underlying plaintext polynomial impossible
            to be correctly decoded back into an integer or rational number. Therefore, it is typically crucial to
            have a good sense of how large the coefficients will grow in the underlying plaintext polynomials when
            homomorphic computations are carried out on the ciphertexts, and make sure that PlainModulus is chosen to
            be at least as large as this number.
            */

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
            //Console.WriteLine("Public Key = {0}", publicKey);
            //Console.WriteLine("Secret Key = {0}", secretKey);

            // Encrypt values.
            Console.WriteLine("Encrypting values...");
            var encryptor = new Encryptor(parms, publicKey);
            var encrypted1 = encryptor.Encrypt(encoded1);
            var encrypted2 = encryptor.Encrypt(encoded2);

            // Perform arithmetic on encrypted values.
            Console.WriteLine("Performing encrypted arithmetic...");
            var evaluator = new Evaluator(parms);
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

        public static void ExampleWeightedAverage()
        {
            PrintExampleBanner("Example: Weighted Average");

            // In this example we demonstrate computing a weighted average of 10 rational numbers.

            // The 10 rational numbers we use are:
            var rationalNumbers = new List<double> { 3.1, 4.159, 2.65, 3.5897, 9.3, 2.3, 8.46, 2.64, 3.383, 2.795 };

            // The 10 weights are:
            var coefficients = new List<double> { 0.1, 0.05, 0.05, 0.2, 0.05, 0.3, 0.1, 0.025, 0.075, 0.05 };

            // Create encryption parameters.
            var parms = new EncryptionParameters();

            parms.PolyModulus.Set("1x^1024 + 1");
            parms.CoeffModulus.Set(ChooserEvaluator.DefaultParameterOptions[1024]);
            parms.PlainModulus.Set(1 << 8);

            Console.WriteLine("Encryption parameters specify {0} coefficients with {1} bits per coefficient",
                parms.PolyModulus.GetSignificantCoeffCount(), parms.CoeffModulus.GetSignificantBitCount());

            // Generate keys.
            Console.WriteLine("Generating keys...");
            var generator = new KeyGenerator(parms);

            /*
            Since we are not using homomorphic multiplication at all, there is no need to
            create any evaluation keys. We can give the number of evaluation keys to be generated
            as a parameter to KeyGenerator.Generate().
            */
            generator.Generate(0);

            Console.WriteLine("... key generation completed");
            var publicKey = generator.PublicKey;
            var secretKey = generator.SecretKey;

            /*
            We will need a fractional encoder for dealing with the rational numbers.
            Here we reserve 128 coefficients of the polynomial for the integral part (low-degree terms)
            and 64 coefficients for the fractional part (high-degree terms).
            */
            var encoder = new BalancedFractionalEncoder(parms.PlainModulus, parms.PolyModulus, 128, 64);

            // Create the rest of the tools
            var encryptor = new Encryptor(parms, publicKey);
            var evaluator = new Evaluator(parms);
            var decryptor = new Decryptor(parms, secretKey);

            // First we encrypt the rational numbers
            Console.Write("Encrypting ... ");
            var encryptedRationals = new List<BigPolyArray>();
            for (int i = 0; i < 10; ++i)
            {
                var encodedNumber = encoder.Encode(rationalNumbers[i]);
                encryptedRationals.Add(encryptor.Encrypt(encodedNumber));
                Console.Write(rationalNumbers[i] + ((i < 9) ? ", " : ".\n"));
            }

            // Next we encode the coefficients. There is no reason to encrypt these since they are not private data.
            Console.Write("Encoding ... ");
            var encodedCoefficients = new List<BigPoly>();
            for (int i = 0; i < 10; ++i)
            {
                encodedCoefficients.Add(encoder.Encode(coefficients[i]));
                Console.Write(coefficients[i] + ((i < 9) ? ", " : ".\n"));
            }

            // We also need to encode 0.1. We will multiply the result by this to perform division by 10.
            var divByTen = encoder.Encode(0.1);

            // Now compute all the products of the encrypted rational numbers with the plaintext coefficients
            Console.Write("Computing products ... ");
            var encryptedProducts = new List<BigPolyArray>();
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

        public static void ExampleParameterSelection()
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

            /*
            First create a ChooserPoly representing the input data. You can think of this modeling a freshly
            encrypted cipheretext of a plaintext polynomial with length at most 10 coefficients, where the
            coefficients have absolute value at most 1.
            */
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
            Console.WriteLine("{{ poly_modulus: {0}", optimalParms.PolyModulus);
            Console.WriteLine("{{ coeff_modulus: {0}", optimalParms.CoeffModulus);
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

            // Create the encoding/encryption tools
            var encoder = new BalancedEncoder(optimalParms.PlainModulus);
            var encryptor = new Encryptor(optimalParms, publicKey);
            var evaluator = new Evaluator(optimalParms);
            var decryptor = new Decryptor(optimalParms, secretKey);

            // Now perform the computations on real encrypted data.
            const int inputValue = 12345;
            var plainInput = encoder.Encode(inputValue);
            Console.WriteLine("Encoded {0} as polynomial {1}", inputValue, plainInput);

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
            Console.WriteLine("Polynomial 42x^3-27x+1 evaluated at x=12345: {0}", encoder.DecodeInt64(plainResult));

            // How much noise did we end up with?
            Console.WriteLine("Noise in the result: {0}/{1} bits", Utilities.InherentNoise(result, optimalParms, secretKey).GetSignificantBitCount(),
                Utilities.InherentNoiseMax(optimalParms).GetSignificantBitCount());
        }

        public static void ExampleBatching()
        {
            PrintExampleBanner("Example: Batching using CRT");

            // Create encryption parameters
            var parms = new EncryptionParameters();

            /*
            For PolyCRTBuilder we need to use a plain modulus congruent to 1 modulo 2*degree(PolyModulus), and
            preferably a prime number. We could for example use the following parameters:

            parms.PolyModulus.Set("1x^2048 + 1");
            parms.CoeffModulus.Set(ChooserEvaluator.DefaultParameterOptions[2048]);
            parms.PlainModulus.Set(12289);

            However, the primes suggested by ChooserEvaluator.DefaultParameterOptions are highly non-optimal in this 
            case. The reason is that the noise growth in many homomorphic operations depends on the remainder
            CoeffModulus % PlainModulus, which is typically close to PlainModulus unless the parameters are carefully 
            chosen. The primes in ChooserEvaluator.DefaultParameterOptions are chosen so that this remainder is 1 
            when PlainModulus is a (not too large) power of 2, so in the earlier examples this was not an issue. 
            However, here we are forced to take PlainModulus to be odd, and as a result the default parameters are no
            longer optimal at all in this sense.

            Thus, for improved performance when using PolyCRTBuilder, we recommend the user to use their own
            custom CoeffModulus. It should be a prime of the form 2^A - D, where D is as small as possible.
            The PlainModulus should be simultaneously chosen to be a prime congruent to 1 modulo 2*degree(PolyModulus),
            so that in addition CoeffModulus % PlainModulus is 1. Finally, CoeffModulus should be bounded by the 
            same strict upper bounds that were mentioned in ExampleBasics():
            /------------------------------------\
            | PolyModulus | CoeffModulus bound |
            | -------------|---------------------|
            | 1x^1024 + 1  | 48 bits             |
            | 1x^2048 + 1  | 96 bits             |
            | 1x^4096 + 1  | 192 bits            |
            | 1x^8192 + 1  | 384 bits            |
            | 1x^16384 + 1 | 768 bits            |
            \------------------------------------/

            One issue with using such custom primes, however, is that they are never NTT primes, i.e. not congruent 
            to 1 modulo 2*degree(PolyModulus), and hence might not allow for certain optimizations to be used in 
            polynomial arithmetic. Another issue is that the search-to-decision reduction of RLWE does not apply to 
            non-NTT primes, but this is not known to result in any concrete reduction in the security level.

            In this example we use the prime 2^95 - 613077 as our coefficient modulus. The user should try switching 
            between this and ChooserEvaluator.DefaultParameterOptions[2048] to observe the difference in the noise 
            level at the end of the computation. This difference becomes significantly greater when using larger
            values for PlainModulus.
            */
            parms.PolyModulus.Set("1x^2048 + 1");
            parms.CoeffModulus.Set("7FFFFFFFFFFFFFFFFFF6A52B");
            //parms.CoeffModulus.Set(ChooserEvaluator.DefaultParameterOptions[2048]);
            parms.PlainModulus.Set(12289);

            Console.WriteLine("Encryption parameters specify {0} coefficients with {1} bits per coefficient",
                parms.PolyModulus.GetSignificantCoeffCount(), parms.CoeffModulus.GetSignificantBitCount());

            // Create the PolyCRTBuilder
            var crtbuilder = new PolyCRTBuilder(parms.PlainModulus, parms.PolyModulus);
            int slotCount = crtbuilder.SlotCount;

            // Create a list of values that are to be stored in the slots. We initialize all values to 0 at this point.
            var values = new List<BigUInt>(slotCount);
            for(int i = 0; i< slotCount; ++i)
            {
                values.Add(new BigUInt(parms.PlainModulus.BitCount, 0));
            }

            // Set the first few entries of the values list to be non-zero
            values[0].Set(2);
            values[1].Set(3);
            values[2].Set(5);
            values[3].Set(7);
            values[4].Set(11);
            values[5].Set(13);

            // Now compose these into one polynomial using PolyCRTBuilder
            Console.Write("Plaintext slot contents (slot, value): ");
            for (int i = 0; i < 6; ++i)
            {
                string toWrite = "(" + i.ToString() + ", " + values[i].ToDecimalString() + ")";
                toWrite += (i != 5) ? ", " : "\n";
                Console.Write(toWrite);
            }
            var plainComposedPoly = crtbuilder.Compose(values);

            // Let's do some homomorphic operations now. First we need all the encryption tools.
            // Generate keys.
            Console.WriteLine("Generating keys...");
            var generator = new KeyGenerator(parms);
            generator.Generate();
            Console.WriteLine("... key generation completed");
            var publicKey = generator.PublicKey;
            var secretKey = generator.SecretKey;

            // Create the encryption tools
            var encryptor = new Encryptor(parms, publicKey);
            var evaluator = new Evaluator(parms);
            var decryptor = new Decryptor(parms, secretKey);

            // Encrypt plainComposed_poly
            Console.Write("Encrypting ... ");
            var encryptedComposedPoly = encryptor.Encrypt(plainComposedPoly);
            Console.WriteLine("done.");

            // Let's square and then decrypt the encryptedComposedPoly
            Console.Write("Squaring the encrypted polynomial ... ");
            var encryptedSquare = evaluator.Exponentiate(encryptedComposedPoly, 2);
            Console.WriteLine("done.");

            Console.Write("Decrypting the squared polynomial ... ");
            var plainSquare = decryptor.Decrypt(encryptedSquare);
            Console.WriteLine("done.");

            // Print the squared slots
            Console.Write("Squared slot contents (slot, value): ");
            for (int i = 0; i < 6; ++i)
            {
                string toWrite = "(" + i.ToString() + ", " + crtbuilder.GetSlot(plainSquare, i).ToDecimalString() + ")";
                toWrite += (i != 5) ? ", " : "\n";
                Console.Write(toWrite);
            }

            // Now let's try to multiply the squares with the plaintext coefficients (3, 1, 4, 1, 5, 9, 0, 0, ..., 0).
            // First create the coefficient list
            var plainCoeffList = new List<BigUInt>(slotCount);
            for (int i = 0; i < slotCount; ++i)
            {
                plainCoeffList.Add(new BigUInt(parms.PlainModulus.BitCount, 0));
            }

            plainCoeffList[0].Set(3);
            plainCoeffList[1].Set(1);
            plainCoeffList[2].Set(4);
            plainCoeffList[3].Set(1);
            plainCoeffList[4].Set(5);
            plainCoeffList[5].Set(9);

            // Use PolyCRTBuilder to compose plainCoeffList into a polynomial
            var plainCoeffPoly = crtbuilder.Compose(plainCoeffList);

            // Print the coefficient list
            Console.Write("Coefficient slot contents (slot, value): ");
            for (int i = 0; i < 6; ++i)
            {
                string toWrite = "(" + i.ToString() + ", " + crtbuilder.GetSlot(plainCoeffPoly, i).ToDecimalString() + ")";
                toWrite += (i != 5) ? ", " : "\n";
                Console.Write(toWrite);
            }

            // Now use MultiplyPlain to multiply each encrypted slot with the corresponding coefficient
            Console.Write("Multiplying squared slots with the coefficients ... ");
            var encryptedScaledSquare = evaluator.MultiplyPlain(encryptedSquare, plainCoeffPoly);
            Console.WriteLine(" done.");

            // Decrypt it
            Console.Write("Decrypting the scaled squared polynomial ... ");
            var plainScaledSquare = decryptor.Decrypt(encryptedScaledSquare);
            Console.WriteLine("done.");

            // Print the scaled squared slots
            Console.Write("Scaled squared slot contents (slot, value): ");
            for (int i = 0; i < 6; ++i)
            {
                string toWrite = "(" + i.ToString() + ", " + crtbuilder.GetSlot(plainScaledSquare, i).ToDecimalString() + ")";
                toWrite += (i != 5) ? ", " : "\n";
                Console.Write(toWrite);
            }

            // How much noise did we end up with?
            Console.WriteLine("Noise in the result: {0}/{1} bits", Utilities.InherentNoise(encryptedScaledSquare, parms, secretKey).GetSignificantBitCount(),
                Utilities.InherentNoiseMax(parms).GetSignificantBitCount());
        }

        public static void ExampleRelinearization()
        {
            PrintExampleBanner("Example: Relinearization");

            /*
            A valid ciphertext consists of at least two polynomials. To read the current size of a ciphertext the
            user can use BigPolyArray.Size. A fresh ciphertext always has size 2, and performing homomorphic multiplication
            results in the output ciphertext growing in size. More precisely, if the input ciphertexts have size M and N,
            then the output ciphertext after homomorphic multiplication will have size M+N-1.

            The multiplication operation on input ciphertexts of size M and N will require M*N polynomial multiplications
            to be performed. Therefore, if the ciphertexts grow to be large, multiplication could potentially become
            computationally very costly and in some situations the user might prefer to reduce the size of the ciphertexts 
            by performing a so-called relinearization operation.

            The function Evaluator.Relinearize(...) can reduce the size of an input ciphertext of size M to any size in 
            2, 3, ..., M. Relinearizing one or both of two ciphertexts before performing multiplication on them may significantly
            reduce the computational cost of the multiplication. However, note that the relinearization process also requires 
            several polynomial multiplications to be performed. In particular relinearizing a ciphertext of size K to size L 
            will itself require 2*(K-L)*[floor(log_2(CoeffModulus)/dbc)+1] polynomial multiplications, where dbc is the 
            DecompositionBitCount (see below). It is also important to understand that relinearization grows the inherent noise 
            in a ciphertext by an additive factor proportional to 2^dbc, which can in some cases be very large. When using 
            relinearization it is necessary that the DecompositionBitCount is specified in the encryption parameters, 
            and that enough evaluation keys are given to the constructor of Evaluator. 

            The DecompositionBitCount affects both performance and noise growth in relinearization, as was explained above.
            Simply put, the larger dbc is, the faster relinearization is, and the larger the additive noise growth factor is
            (see above). However, if some multiplications have already been performed on a ciphertext so that the noise has
            grown to some reasonable level, relinearization might have no practical effect anymore on noise due to the additive 
            factor being possibly (much) smaller than what the current noise is. This is why it makes almost never sense to 
            relinearize after the first multiplication since the noise will still be so small that any reasonably large dbc
            would increase the noise by a significant amount. In many cases it might not be beneficial to relinearize at all,
            especially if the computation to be performed amounts to evaluating some fairly low degree polynomial. If the 
            degree is higher, then in some cases it might be beneficial to relinearize at some stage in the computation.
            See below for how to choose a good value for the DecompositionBitCount.

            If the intention of the evaluating party is to hide the structure of the computation that has been performed on 
            the ciphertexts, it might be necessary to relinearize to hide the number of multiplications that the ciphertexts
            have gone through. In addition, after relinearizing (to size 2) it might be a good idea to re-randomize the 
            ciphertext by adding to it a fresh encryption of 0.

            In this example we will demonstrate using Evaluator.Relinearize(...) and illustrate how it reduces the ciphertext 
            sizes. We will also observe the effects it has on noise.
            */

            // Set up encryption parameters
            var parms = new EncryptionParameters();
            parms.PolyModulus.Set("1x^2048 + 1");
            parms.CoeffModulus.Set(ChooserEvaluator.DefaultParameterOptions[2048]);
            parms.PlainModulus.Set(1 << 16);

            /*
            The choice of DecompositionBitCount (dbc) can affect the performance of relinearization noticeably. A somewhat 
            optimal choice is to choose it between 1/5 and 1/2 of the significant bit count of the coefficient modulus (see 
            table below). It turns out that if dbc cannot (due to noise growth) be more than one fifth of the significant 
            bit count of the coefficient modulus, then it is in fact better to just move up to a larger PolyModulus and 
            CoeffModulus, and set dbc to be as large as possible.
            /--------------------------------------------------------\
            | poly_modulus | coeff_modulus bound | dbc min | dbc max |
            | -------------|---------------------|------------------ |
            | 1x^1024 + 1  | 48 bits             | 10      | 24      |
            | 1x^2048 + 1  | 96 bits             | 20      | 48      |
            | 1x^4096 + 1  | 192 bits            | 39      | 96      |
            | 1x^8192 + 1  | 384 bits            | 77      | 192     |
            | 1x^16384 + 1 | 768 bits            | 154     | 384     |
            \--------------------------------------------------------/

            A smaller DecompositionBitCount will make relinearization slower. A higher DecompositionBitCount will increase
            noise growth while not making relinearization any faster. Here, the CoeffModulus has 96 significant bits, so 
            we choose DecompositionBitCount to be half of this.
            */
            parms.DecompositionBitCount = 48;

            Console.WriteLine("Encryption parameters specify {0} coefficients with {1} bits per coefficient",
                parms.PolyModulus.GetSignificantCoeffCount(), parms.CoeffModulus.GetSignificantBitCount());

            /*
            Generate keys

            By default, KeyGenerator.Generate() will generate no evaluation keys. This means that we cannot perform any 
            relinearization. However, this is sufficient for performing all other homomorphic evaluation operations as 
            they do not use evaluation keys.
            */
            Console.WriteLine("Generating keys...");
            var generator = new KeyGenerator(parms);
            generator.Generate();
            Console.WriteLine("... key generation complete");
            BigPolyArray publicKey = generator.PublicKey;
            BigPoly secretKey = generator.SecretKey;

            /*
            Suppose we want to homomorphically multiply four ciphertexts together. Does it make sense to relinearize 
            at an intermediate step of the computation? We demonstrate how relinearization at different stages affects
            the results.
            */

            // Encrypt the plaintexts to generate the four fresh ciphertexts
            var plain1 = new BigPoly("4");
            var plain2 = new BigPoly("3x^1");
            var plain3 = new BigPoly("2x^2");
            var plain4 = new BigPoly("1x^3");
            Console.WriteLine("Encrypting values as { encrypted1, encrypted2, encrypted3, encrypted4 }");
            var encryptor = new Encryptor(parms, publicKey);
            var encrypted1 = encryptor.Encrypt(plain1);
            var encrypted2 = encryptor.Encrypt(plain2);
            var encrypted3 = encryptor.Encrypt(plain3);
            var encrypted4 = encryptor.Encrypt(plain4);

            // What are the noises in the four ciphertexts?
            var maxNoiseBitCount = Utilities.InherentNoiseMax(parms).GetSignificantBitCount();
            Console.WriteLine("Noises in the four ciphertexts: {0}/{1} bits, {2}/{3} bits, {4}/{5} bits, {6}/{7} bits",
                Utilities.InherentNoise(encrypted1, parms, secretKey).GetSignificantBitCount(), maxNoiseBitCount,
                Utilities.InherentNoise(encrypted2, parms, secretKey).GetSignificantBitCount(), maxNoiseBitCount,
                Utilities.InherentNoise(encrypted3, parms, secretKey).GetSignificantBitCount(), maxNoiseBitCount,
                Utilities.InherentNoise(encrypted4, parms, secretKey).GetSignificantBitCount(), maxNoiseBitCount);

            // Construct an Evaluator
            var evaluator = new Evaluator(parms);

            // Perform first part of computation
            Console.WriteLine("Computing encProd1 as encrypted1*encrypted2");
            var encProd1 = evaluator.Multiply(encrypted1, encrypted2);
            Console.WriteLine("Computing encProd2 as encrypted3*encrypted4");
            var encProd2 = evaluator.Multiply(encrypted3, encrypted4);

            // Now encProd1 and encProd2 both have size 3
            Console.WriteLine($"Sizes of enc_prod1 and enc_prod2: {encProd1.Size}, {encProd2.Size}");

            // What are the noises in the products?
            Console.WriteLine("Noises in encProd1 and encProd2: {0}/{1} bits, {2}/{3} bits",
                Utilities.InherentNoise(encProd1, parms, secretKey).GetSignificantBitCount(), maxNoiseBitCount,
                Utilities.InherentNoise(encProd2, parms, secretKey).GetSignificantBitCount(), maxNoiseBitCount);

            // Compute product of all four
            Console.WriteLine("Computing encResult as encProd1*encProd2");
            var encResult = evaluator.Multiply(encProd1, encProd2);

            // Now enc_result has size 5
            Console.WriteLine($"Size of enc_result: {encResult.Size}");

            // What is the noise in the result?
            Console.WriteLine("Noise in enc_result: {0}/{1} bits", 
                Utilities.InherentNoise(encResult, parms, secretKey).GetSignificantBitCount(), maxNoiseBitCount);

            /*
            We didn't create any evaluation keys, so we can't relinearize at all with the current Evaluator.
            The size of our final ciphertext enc_result is 5, so for example to relinearize this down to size 2
            we will need 3 evaluation keys. In general, relinearizing down from size K to any smaller size (but at least 2)
            requires at least K-2 evaluation keys, so in this case we will need at least 2 evaluation keys.

            We can create these new evaluation keys by calling KeyGenerator.GenerateEvaluationKeys(...). Alternatively,
            we could have created them already in the beginning by instead of calling generator.Generate(2) instead of
            generator.Generate().

            We will also need a new Evaluator, as the previous one was constructed without enough (indeed, any)
            evaluation keys. It is not possible to add new evaluation keys to a previously created Evaluator.
            */
            generator.GenerateEvaluationKeys(3);
            var evaluationKeys = generator.EvaluationKeys;
            var evaluator2 = new Evaluator(parms, evaluationKeys);

            /*
            We can relinearize encResult back to size 2 if we want to. In fact, we could also relinearize it to size 3 or 4,
            or more generally to any size less than the current size but at least 2. The way to do this would be to call
            Evaluator.Relinearize(encResult, destinationSize).
            */
            Console.WriteLine("Relinearizing encResult to size 2 (stored in encRelinResult)");
            var encRelinResult = evaluator2.Relinearize(encResult);

            /*
            What did that do to size and noise?
            In fact noise remained essentially the same, because at this point the size of noise is already significantly 
            larger than the additive term contributed by the relinearization process. We still remain below the noise bound.
            */
            Console.WriteLine($"Size of enc_relin_result: {encRelinResult.Size}");
            Console.WriteLine("Noise in enc_relin_result: {0}/{1} bits",
                Utilities.InherentNoise(encRelinResult, parms, secretKey).GetSignificantBitCount(), maxNoiseBitCount);

            // What if we do intermediate relinearization of encProd1 and encProd2?
            Console.WriteLine("Relinearizing encProd1 and encProd2 to size 2");
            var encRelinProd1 = evaluator2.Relinearize(encProd1);
            var encRelinProd2 = evaluator2.Relinearize(encProd2);

            // What happened to sizes and noises? Noises grew by a significant amount!
            Console.WriteLine($"Sizes of encRelinProd1 and encRelinProd2: {encRelinProd1.Size}, {encRelinProd2.Size}");
            Console.WriteLine("Noises in encRelinProd1 and encRelinProd2: {0}/{1} bits, {2}/{3} bits",
                Utilities.InherentNoise(encRelinProd1, parms, secretKey).GetSignificantBitCount(), maxNoiseBitCount,
                Utilities.InherentNoise(encRelinProd2, parms, secretKey).GetSignificantBitCount(), maxNoiseBitCount);

            // Now multiply the relinearized products together
            Console.WriteLine("Computing encIntermediateRelinResult as encRelinProd1*encRelinProd2");
            var encIntermediateRelinResult = evaluator2.Multiply(encRelinProd1, encRelinProd2);

            /* 
            What did that do to size and noise?
            We are above the noise bound in this case. The resulting ciphertext is corrupted. It is instructive to 
            try and see how a smaller DecompositionBitCount affects the results, e.g. try setting it to 24. 
            Also here PlainModulus was set to be quite large to emphasize the effect.
            */
            Console.WriteLine($"Size of encIntermediateRelinResult: {encIntermediateRelinResult.Size}");
            Console.WriteLine("Noise in encIntermediateRelinResult: {0}/{1} bits",
                Utilities.InherentNoise(encIntermediateRelinResult, parms, secretKey).GetSignificantBitCount(), maxNoiseBitCount);
        }
    }
}
