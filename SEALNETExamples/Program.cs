using System;
using Microsoft.Research.SEAL;
using System.Collections.Generic;

namespace SEALNETExamples
{
    public class Program
    {
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
            parms.SetPolyModulus("1x^2048 + 1");

            /*
            Next we choose the coefficient modulus. SEAL comes with default values for the coefficient
            modulus for some of the most reasonable choices of PolyModulus. They are as follows:

            /----------------------------------------------------------------------\
            | PolyModulus  | default CoeffModulus                       | security |
            | -------------|--------------------------------------------|----------|
            | 1x^2048 + 1  | 2^60 - 2^14 + 1 (60 bits)                  | 119 bit  |
            | 1x^4096 + 1  | 2^116 - 2^18 + 1 (116 bits)                | 122 bit  |
            | 1x^8192 + 1  | 2^226 - 2^26 + 1 (226 bits)                | 124 bit  |
            | 1x^16384 + 1 | 2^435 - 2^33 + 1 (435 bits)                | 130 bit  |
            | 1x^32768 + 1 | 2^889 - 2^54 - 2^53 - 2^52 + 1 (889 bits)  | 127 bit  |
            \----------------------------------------------------------------------/

            These can be conveniently accessed using ChooserEvaluator.DefaultParameterOptions, which returns 
            the above list of options as a Dictionary, keyed by the degree of the polynomial modulus. The security 
            levels are estimated based on https://eprint.iacr.org/2015/046 and https://eprint.iacr.org/2017/047. 
            We strongly recommend that the user consult an expert in the security of RLWE-based cryptography to 
            estimate the security of a particular choice of parameters.

            The user can also easily choose their custom coefficient modulus. For best performance, it should 
            be a prime of the form 2^A - B, where B is congruent to 1 modulo 2*degree(PolyModulus), and as small 
            as possible. Roughly speaking, When the rest of the parameters are held fixed, increasing CoeffModulus
            decreases the security level. Thus we would not recommend using a value for CoeffModulus much larger 
            than those listed above (the defaults). In general, we highly recommend the user to consult with an expert 
            in the security of RLWE-based cryptography when selecting their parameters to ensure an appropriate level 
            of security.

            The size of CoeffModulus affects the total noise budget that a freshly encrypted ciphertext has. More 
            precisely, every ciphertext starts with a certain amount of noise budget, which is consumed in homomorphic
            operations - in particular in multiplication. Once the noise budget reaches 0, the ciphertext becomes 
            impossible to decrypt. The total noise budget in a freshly encrypted ciphertext is very roughly given by 
            log2(CoeffModulus/PlainModulus), so increasing coeff_modulus will allow the user to perform more
            homomorphic operations on the ciphertexts without corrupting them. However, we must again warn that
            increasing CoeffModulus has a strong negative effect on the security level.
            */
            parms.SetCoeffModulus(ChooserEvaluator.DefaultParameterOptions[2048]);

            /*
            Now we set the plaintext modulus. This can be any positive integer, even though here we take it to be a 
            power of two. A larger plaintext modulus causes the noise to grow faster in homomorphic multiplication, 
            and also lowers the maximum amount of noise in ciphertexts that the system can tolerate (see above).
            On the other hand, a larger plaintext modulus typically allows for better homomorphic integer arithmetic,
            although this depends strongly on which encoder is used to encode integers into plaintext polynomials.
            */
            parms.SetPlainModulus(1 << 8);

            /*
            Once all parameters are set, we need to call EncryptionParameters::validate(), which evaluates the
            properties of the parameters, their validity for homomorphic encryption, and performs some important
            pre-computation.
            */
            parms.Validate();

            /*
            Plaintext elements in the FV scheme are polynomials (represented by the Plaintext class) with coefficients 
            integers modulo PlainModulus. To encrypt for example integers instead, one must use an "encoding scheme", 
            i.e. a specific way of representing integers as such polynomials. SEAL comes with a few basic encoders:

            IntegerEncoder:
            Given an integer base b, encodes integers as plaintext polynomials in the following way. First, a base-b
            expansion of the integer is computed. This expansion uses a "balanced" set of representatives of integers
            modulo b as the coefficients. Namely, when b is off the coefficients are integers between -(b-1)/2 and
            (b-1)/2. When b is even, the integers are between -b/2 and (b-1)/2, except when b is two and the usual
            binary expansion is used (coefficients 0 and 1). Decoding amounts to evaluating the polynomial at x=b.
            For example, if b=2, the integer 26 = 2^4 + 2^3 + 2^1 is encoded as the polynomial 1x^4 + 1x^3 + 1x^1.
            When b=3, 26 = 3^3 - 3^0 is encoded as the polynomial 1x^3 - 1. In reality, coefficients of polynomials
            are always unsigned integers, and in this case are stored as their smallest non-negative representatives
            modulo plain_modulus. To create an integer encoder with a base b, use IntegerEncoder(PlainModulus, b). 
            If no b is given to the constructor, the default value of b=2 is used.

            FractionalEncoder:
            Encodes fixed-precision rational numbers as follows. First expand the number in a given base b, possibly 
            truncating an infinite fractional part to finite precision, e.g. 26.75 = 2^4 + 2^3 + 2^1 + 2^(-1) + 2^(-2)
            when b=2. For the sake of the example, suppose PolyModulus is 1x^1024 + 1. Next represent the integer part 
            of the number in the same way as in IntegerEncoder (with b=2 here). Finally, represent the fractional part 
            in the leading coefficients of the polynomial, but when doing so invert the signs of the coefficients. So 
            in this example we would represent 26.75 as the polynomial -1x^1023 - 1x^1022 + 1x^4 + 1x^3 + 1x^1. The 
            negative coefficients of the polynomial will again be represented as their negatives modulo PlainModulus.

            PolyCRTBuilder:
            If PolyModulus is 1x^N + 1, PolyCRTBuilder allows "batching" of N plaintext integers modulo plain_modulus 
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

            Here we choose to create an IntegerEncoder with base b=2.
            */
            var encoder = new IntegerEncoder(parms.PlainModulus);

            // Encode two integers as polynomials.
            const int value1 = 5;
            const int value2 = -7;
            var encoded1 = encoder.Encode(value1);
            var encoded2 = encoder.Encode(value2);
            Console.WriteLine("Encoded {0} as polynomial {1}", value1, encoded1);
            Console.WriteLine("Encoded {0} as polynomial {1}", value2, encoded2);

            // Generate keys.
            Console.WriteLine("Generating keys ...");
            var generator = new KeyGenerator(parms);
            generator.Generate();
            Console.WriteLine("... key generation completed");
            var publicKey = generator.PublicKey;
            var secretKey = generator.SecretKey;

            // Encrypt values.
            Console.WriteLine("Encrypting values...");
            var encryptor = new Encryptor(parms, publicKey);
            var encrypted1 = encryptor.Encrypt(encoded1);
            var encrypted2 = encryptor.Encrypt(encoded2);

            // Perform arithmetic on encrypted values.
            Console.WriteLine("Performing arithmetic on ecrypted numbers ...");
            var evaluator = new Evaluator(parms);
            Console.WriteLine("Performing homomorphic negation ...");
            var encryptedNegated1 = evaluator.Negate(encrypted1);
            Console.WriteLine("Performing homomorphic addition ...");
            var encryptedSum = evaluator.Add(encrypted1, encrypted2);
            Console.WriteLine("Performing homomorphic subtraction ...");
            var encryptedDiff = evaluator.Sub(encrypted1, encrypted2);
            Console.WriteLine("Performing homomorphic multiplication ...");
            var encryptedProduct = evaluator.Multiply(encrypted1, encrypted2);

            // Decrypt results.
            Console.WriteLine("Decrypting results ...");
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
            Console.WriteLine("Original = {0}; after encryption/decryption = {1}", value1, decoded1);
            Console.WriteLine("Original = {0}; after encryption/decryption = {1}", value2, decoded2);
            Console.WriteLine("Encrypted negate of {0} = {1}", value1, decodedNegated1);
            Console.WriteLine("Encrypted addition of {0} and {1} = {2}", value1, value2, decodedSum);
            Console.WriteLine("Encrypted subtraction of {0} and {1} = {2}", value1, value2, decodedDiff);
            Console.WriteLine("Encrypted multiplication of {0} and {1} = {2}", value1, value2, decodedProduct);

            // How much noise budget did we use in these operations?
            Console.WriteLine("Noise budget in encryption of {0}: {1} bits", value1, decryptor.InvariantNoiseBudget(encrypted1));
            Console.WriteLine("Noise budget in encryption of {0}: {1} bits", value2, decryptor.InvariantNoiseBudget(encrypted2));
            Console.WriteLine("Noise budget in sum: {0} bits", decryptor.InvariantNoiseBudget(encryptedSum));
            Console.WriteLine("Noise budget in product: {0} bits", decryptor.InvariantNoiseBudget(encryptedProduct));
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
            parms.SetPolyModulus("1x^2048 + 1");
            parms.SetCoeffModulus(ChooserEvaluator.DefaultParameterOptions[2048]);
            parms.SetPlainModulus(1 << 8);
            parms.Validate();

            // Generate keys.
            Console.WriteLine("Generating keys ...");
            var generator = new KeyGenerator(parms);
            generator.Generate();
            Console.WriteLine("... key generation completed");
            var publicKey = generator.PublicKey;
            var secretKey = generator.SecretKey;

            /*
            We will need a fractional encoder for dealing with the rational numbers. Here we reserve 
            64 coefficients of the polynomial for the integral part (low-degree terms) and expand the 
            fractional part to 32 terms of precision (base 3) (high-degree terms).
            */
            var encoder = new FractionalEncoder(parms.PlainModulus, parms.PolyModulus, 64, 32, 3);

            // Create the rest of the tools
            var encryptor = new Encryptor(parms, publicKey);
            var evaluator = new Evaluator(parms);
            var decryptor = new Decryptor(parms, secretKey);

            // First we encrypt the rational numbers
            Console.Write("Encrypting ... ");
            var encryptedRationals = new List<Ciphertext>();
            for (int i = 0; i < 10; ++i)
            {
                var encodedNumber = encoder.Encode(rationalNumbers[i]);
                encryptedRationals.Add(encryptor.Encrypt(encodedNumber));
                Console.Write(rationalNumbers[i] + ((i < 9) ? ", " : ".\n"));
            }

            // Next we encode the coefficients. There is no reason to encrypt these since they are not private data.
            Console.Write("Encoding ... ");
            var encodedCoefficients = new List<Plaintext>();
            for (int i = 0; i < 10; ++i)
            {
                encodedCoefficients.Add(encoder.Encode(coefficients[i]));
                Console.Write(coefficients[i] + ((i < 9) ? ", " : ".\n"));
            }

            // We also need to encode 0.1. We will multiply the result by this to perform division by 10.
            var divByTen = encoder.Encode(0.1);

            // Now compute all the products of the encrypted rational numbers with the plaintext coefficients
            Console.Write("Computing products ... ");
            var encryptedProducts = new List<Ciphertext>();
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

            // Print the result
            double result = encoder.Decode(plainResult);
            Console.WriteLine("Weighted average: {0}", result);

            // How much noise budget are we left with?
            Console.WriteLine("Noise budget in result: {0} bits", decryptor.InvariantNoiseBudget(encryptedResult));
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

            // To find an optimized set of parameters, we use ChooserEvaluator.SelectParameters(...).
            var optimalParms = new EncryptionParameters();
            chooserEvaluator.SelectParameters(new List<ChooserPoly> { cresult }, 0, optimalParms);

            // We still need to validate the returned parameters
            optimalParms.Validate();

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
            Console.WriteLine("Generating keys ...");
            var generator = new KeyGenerator(optimalParms);

            /*
            Need to generate one evaluation key because below we will use Evaluator.Exponentiate(...),
            which relinearizes after every multiplication it performs (see ExampleRelinearization()
            for more details.
            */
            generator.Generate(1);
            Console.WriteLine("... key generation completed");
            var publicKey = generator.PublicKey;
            var secretKey = generator.SecretKey;
            var evaluationKeys = generator.EvaluationKeys;

            // Create the encoding/encryption tools
            var encoder = new IntegerEncoder(optimalParms.PlainModulus, 3);
            var encryptor = new Encryptor(optimalParms, publicKey);
            var evaluator = new Evaluator(optimalParms, evaluationKeys);
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

            // How much noise budget are we left with?
            Console.WriteLine("Noise budget in result: {0} bits", decryptor.InvariantNoiseBudget(result));
        }

        public static void ExampleBatching()
        {
            PrintExampleBanner("Example: Batching using CRT");

            // Create encryption parameters
            var parms = new EncryptionParameters();

            /*
            For PolyCRTBuilder it is necessary to have PlainModulus be a prime number congruent to 1 modulo 
            2*degree(PolyModulus). We can use for example the following parameters:
            */
            parms.SetPolyModulus("1x^4096 + 1");
            parms.SetCoeffModulus(ChooserEvaluator.DefaultParameterOptions[4096]);
            parms.SetPlainModulus(40961);
            parms.Validate();

            // Create the PolyCRTBuilder
            var crtbuilder = new PolyCRTBuilder(parms);
            int slotCount = crtbuilder.SlotCount;

            Console.WriteLine($"Encryption parameters allow {slotCount} slots.");

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
            Console.WriteLine("Generating keys ...");
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
            var encryptedSquare = evaluator.Square(encryptedComposedPoly);
            Console.WriteLine("done.");

            Console.Write("Decrypting the squared polynomial ... ");
            var plainSquare = decryptor.Decrypt(encryptedSquare);
            Console.WriteLine("done.");

            // Print the squared slots
            crtbuilder.Decompose(plainSquare, values);
            Console.Write("Squared slot contents (slot, value): ");
            for (int i = 0; i < 6; ++i)
            {
                string toWrite = "(" + i.ToString() + ", " + values[i].ToDecimalString() + ")";
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
                string toWrite = "(" + i.ToString() + ", " + plainCoeffList[i].ToDecimalString() + ")";
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
            crtbuilder.Decompose(plainScaledSquare, values);
            Console.Write("Scaled squared slot contents (slot, value): ");
            for (int i = 0; i < 6; ++i)
            {
                string toWrite = "(" + i.ToString() + ", " + values[i].ToDecimalString() + ")";
                toWrite += (i != 5) ? ", " : "\n";
                Console.Write(toWrite);
            }

            // How much noise budget are we left with?
            Console.WriteLine("Noise budget in result: {0} bits", decryptor.InvariantNoiseBudget(encryptedScaledSquare));
        }

        public static void ExampleRelinearization()
        {
            PrintExampleBanner("Example: Relinearization");

            /*
            A valid ciphertext consists of at least two polynomials. To read the current size of a ciphertext 
            the user can use Ciphertext.Size. A fresh ciphertext always has size 2, and performing 
            homomorphic multiplication results in the output ciphertext growing in size. More precisely, 
            if the input ciphertexts have size M and N, then the output ciphertext after homomorphic 
            multiplication will have size M+N-1.

            The multiplication operation on input ciphertexts of size M and N will require O(M*N) polynomial 
            multiplications to be performed. Therefore, the multiplication of large ciphertexts could be 
            very computationally costly and in some situations the user might prefer to reduce the size of 
            the ciphertexts by performing a so-called relinearization operation.

            The function Evaluator.Relinearize(...) can reduce the size of an input ciphertext of size M 
            to any size in 2, 3, ..., M-1. As was explained above, relinearizing one or both of two ciphertexts 
            before performing multiplication on them may significantly reduce the computational cost of the 
            multiplication. However, note that the relinearization process itself also requires several polynomial 
            multiplications to be performed. Using the Number Theoretic Transform (NTT) for relinearization, 
            reducing a ciphertext of size K to size L requires (K - L)*([floor(log_2(CoeffModulus)/dbc) + 3]) 
            NTT transforms, where dbc denotes the encryption parameter "DecompositionBitCount".

            Relinearization also affects the inherent noise in two ways. First, a larger ciphertexts produces
            more noise in homomorphic multiplication than a smaller one does. If the ciphertexts are small,
            the effect of the ciphertext size is insignificant, but if they are very large the effect can 
            easily become the biggest contributor to noise. Second, relinearization increases the inherent 
            noise in the ciphertext to be relinearized by an additive factor. This should be contrasted with 
            the multiplicative factor that homomorphic multiplication increases the noise by. The additive
            factor is proportional to 2^dbc, which can be either very small or very large compared to the 
            current level of inherent noise in the ciphertext. This means that if the ciphertextis very fresh 
            (has very little noise in it), relinearization might have a significant adverse effect on the 
            homomorphic computation ability, and it might make sense to instead use larger ciphertexts and 
            relinearize at a later point where the additive noise term vanishes into an already larger noise,
            or alternatively use a smaller dbc, which will result in slightly slower relinearization. 

            When using relinearization it is necessary that the DecompositionBitCount variable is set to 
            some positive value in the encryption parameters, and that enough evaluation keys are given to
            the constructor of Evaluator. We will discuss evaluation keys when we construct the key generator.

            We will provide two examples of relinearization.
            */

            /*
            Example 1: We demonstrate using Evaluator::relinearize(...) and illustrate how it reduces the 
            ciphertext sizes at the cost of increasing running time and noise in a particular computation.
            */
            ExampleRelinearizationPart1();
            Console.WriteLine();

            /*
            Example 2: We demonstrate how relinearization can reduce noise. In the corresponding example 
            in the C++ project SEALExamples (main.cpp) we also demonstrate a significant improvement in 
            the running time when doing relinearization. This is due to ciphertext sizes being smaller, 
            and as such faster to operate on.
            */
            ExampleRelinearizationPart2();
        }

        public static void ExampleRelinearizationPart1()
        {
            Console.WriteLine("Example 1: Performing relinearization too early can increase noise in the final result.");

            // Set up encryption parameters
            var parms = new EncryptionParameters();
            parms.SetPolyModulus("1x^4096 + 1");
            parms.SetCoeffModulus(ChooserEvaluator.DefaultParameterOptions[4096]);
            parms.SetPlainModulus(1 << 8);

            /*
            The choice of decomposition_bit_count (dbc) can affect the performance of relinearization
            noticeably. A reasonable choice for it is between 1/10 and 1/2 of the significant bit count 
            of the coefficient modulus. Sometimes when the dbc needs to be very small (due to noise growth), 
            it might make more sense to move up to a larger PolyModulus and CoeffModulus, and set dbc to 
            be as large as possible.

            A smaller dbc will make relinearization too slow. A higher dbc will increase noise growth 
            while not making relinearization any faster. Here, the CoeffModulus has 116 significant 
            bits, so we choose dbc to be half of this. We can expect to see extreme differences in 
            noise growth between the relinearizing and non-relinearizing cases due to the decomposition 
            bit count being so large.
            */
            parms.SetDecompositionBitCount(58);

            // Validate the parameters
            parms.Validate();

            /*
            By default, KeyGenerator.Generate() will generate no evaluation keys. This means that we 
            cannot perform any relinearization. However, this is sufficient for performing all other 
            homomorphic evaluation operations as they do not use evaluation keys, and is enough for
            now as we start by demonstrating the computation without relinearization.
            */
            Console.WriteLine("Generating keys ...");
            var generator = new KeyGenerator(parms);
            generator.Generate();
            Console.WriteLine("... key generation complete");
            var publicKey = generator.PublicKey;
            var secretKey = generator.SecretKey;

            /*
            Suppose we want to homomorphically multiply four ciphertexts together. Does it make sense 
            to relinearize at an intermediate stage of the computation?
            */

            // Encrypt plaintexts to generate the four fresh ciphertexts
            var plain1 = new Plaintext("5");
            var plain2 = new Plaintext("6");
            var plain3 = new Plaintext("7");
            var plain4 = new Plaintext("8");
            Console.WriteLine("Encrypting values { 5, 6, 7, 8 } as { encrypted1, encrypted2, encrypted3, encrypted4 }");
            var encryptor = new Encryptor(parms, publicKey);
            var encrypted1 = encryptor.Encrypt(plain1);
            var encrypted2 = encryptor.Encrypt(plain2);
            var encrypted3 = encryptor.Encrypt(plain3);
            var encrypted4 = encryptor.Encrypt(plain4);

            // We need a Decryptor to be able to measure the inherent noise
            var decryptor = new Decryptor(parms, secretKey);

            // What are the noise budgets in the four ciphertexts?
            Console.WriteLine("Noise budgets in the four ciphertexts: {0} bits, {1} bits, {2} bits, {3} bits",
                decryptor.InvariantNoiseBudget(encrypted1),
                decryptor.InvariantNoiseBudget(encrypted2),
                decryptor.InvariantNoiseBudget(encrypted3),
                decryptor.InvariantNoiseBudget(encrypted4));

            // Construct an Evaluator
            var evaluator = new Evaluator(parms);

            // Perform first part of computation
            Console.WriteLine("Computing encProd1 as encrypted1*encrypted2 ...");
            var encProd1 = evaluator.Multiply(encrypted1, encrypted2);
            Console.WriteLine("Computing encProd2 as encrypted3*encrypted4 ...");
            var encProd2 = evaluator.Multiply(encrypted3, encrypted4);

            Console.WriteLine();
            Console.WriteLine("Path 1: No relinearization.");

            // Compute product of all four
            Console.WriteLine("Computing encResult as encProd1*encProd2 ...");
            var encResult = evaluator.Multiply(encProd1, encProd2);

            // Now enc_result has size 5
            Console.WriteLine("Size of encResult: {0}", encResult.Size);

            // How much noise budget are we left with?
            var noiseBudgetNoRelin = decryptor.InvariantNoiseBudget(encResult);
            Console.WriteLine("Noise budget in encResult: {0} bits", noiseBudgetNoRelin);

            /*
            We didn't create any evaluation keys, so we can't relinearize at all with the current 
            Evaluator. In general, relinearizing down from size K to any smaller size (but at least 2) 
            requires at least K-2 evaluation keys. In this case we wish to relinearize encProd1 and
            encProd2, which both have size 3. Thus we need only one evaluation key.

            We can create this newn evaluation key by calling KeyGenerator.GenerateEvaluationKeys(...). 
            Alternatively, we could have created it already in the beginning by instead of calling 
            generator.Generate(1) instead of generator.Generate().

            We will also need a new Evaluator, as the previous one was constructed without enough 
            indeed, any) evaluation keys. It is not possible to add new evaluation keys to a previously 
            created Evaluator.
            */
            generator.GenerateEvaluationKeys(1);
            var evaluationKeys = generator.EvaluationKeys;
            var evaluator2 = new Evaluator(parms, evaluationKeys);

            // Now with relinearization
            Console.WriteLine("");
            Console.WriteLine("Path 2: With relinearization");

            // What if we do intermediate relinearization of encProd1 and encProd2?
            Console.WriteLine("Relinearizing encProd1 and encProd2 to size 2 ...");
            var encRelinProd1 = evaluator2.Relinearize(encProd1);
            var encRelinProd2 = evaluator2.Relinearize(encProd2);

            // Now multiply the relinearized products together
            Console.WriteLine("Computing encResult as encRelinProd1*encRelinProd2");
            encResult = evaluator2.Multiply(encRelinProd1, encRelinProd2);

            // Now enc_result has size 3
            Console.WriteLine("Size of encResult: {0}", encResult.Size);

            // How much noise budget are we left with?
            var noiseBudgetRelin = decryptor.InvariantNoiseBudget(encResult);
            Console.WriteLine("Noise budget in encResult: {0} bits", noiseBudgetRelin);

            /*
            While in this case the noise increased significantly due to relinearization, in other
            computations the situation might be entirely different. Indeed, recall that larger
            ciphertext sizes can have a huge adverse effect on noise growth in multiplication.
            Also recall that homomorphic multiplication is much slower when the ciphertexts are 
            larger.
            */
        }

        public static void ExampleRelinearizationPart2()
        {
            Console.WriteLine("Example 2: Effect on running time and noise in computing [(enc1*enc2)*(enc3*enc4)]^2.");

            var parms = new EncryptionParameters();
            parms.SetPolyModulus("1x^4096 + 1");
            parms.SetCoeffModulus(ChooserEvaluator.DefaultParameterOptions[4096]);
            parms.SetPlainModulus(1 << 6);

            /*
            We use a relatively small decomposition bit count here to avoid significant noise
            growth from the relinearization operation itself. Make this bigger and you will
            see both increased running time and decreased noise.
            */
            parms.SetDecompositionBitCount(16);

            // Validate the parameters
            parms.Validate();

            // We generate the encryption keys and one evaluation key.
            Console.WriteLine("Generating keys ...");
            var generator = new KeyGenerator(parms);
            generator.Generate(1);
            Console.WriteLine("... key generation complete");
            var publicKey = generator.PublicKey;
            var secretKey = generator.SecretKey;
            var evaluationKeys = generator.EvaluationKeys;

            // Encrypt plaintexts to generate the four fresh ciphertexts
            var plain1 = new Plaintext("4");
            var plain2 = new Plaintext("3x^1");
            var plain3 = new Plaintext("2x^2");
            var plain4 = new Plaintext("1x^3");
            Console.WriteLine("Encrypting values { 4, 3x, 2x^2, x^3 } as { encrypted1, encrypted2, encrypted3, encrypted4 }");
            var encryptor = new Encryptor(parms, publicKey);
            var encrypted1 = encryptor.Encrypt(plain1);
            var encrypted2 = encryptor.Encrypt(plain2);
            var encrypted3 = encryptor.Encrypt(plain3);
            var encrypted4 = encryptor.Encrypt(plain4);

            // We need a Decryptor to be able to measure the inherent noise
            var decryptor = new Decryptor(parms, secretKey);

            // Construct an Evaluator
            var evaluator = new Evaluator(parms, evaluationKeys);

            Console.WriteLine("Computing encProd12 as encrypted1*encrypted2 ...");
            var encProd12 = evaluator.Multiply(encrypted1, encrypted2);
            Console.WriteLine("Computing encProd34 as encrypted3*encrypted4 ...");
            var encProd34 = evaluator.Multiply(encrypted3, encrypted4);

            // First the result with no relinearization
            Console.WriteLine();
            Console.WriteLine("Path 1: No relinearization");

            // Compute product of all four
            Console.WriteLine("Computing encProd = encProd12*encProd34 ...");
            var encProd = evaluator.Multiply(encProd12, encProd34);

            Console.WriteLine("Computing encSquare = [encProd]^2 ...");
            var encSquare = evaluator.Square(encProd);

            // Print size and noise budget of result. 
            Console.WriteLine("Size of encSquare: {0}", encSquare.Size);
            Console.WriteLine("Noise budget in encSquare: {0} bits",
                decryptor.InvariantNoiseBudget(encSquare));

            // Now the same thing but with relinearization
            Console.WriteLine("");
            Console.WriteLine("Path 2: With relinerization");

            Console.WriteLine("Relinearizing encProd12 and encProd23 to size 2 ...");
            var encRelinProd12 = evaluator.Relinearize(encProd12);
            var encRelinProd34 = evaluator.Relinearize(encProd34);

            // Now multiply the relinearized products together
            Console.WriteLine("Computing encProd as encRelinProd12*encRelinProd34");
            encProd = evaluator.Multiply(encRelinProd12, encRelinProd34);

            Console.WriteLine("Computing encSquare = [encProd]^2 ... ");
            encSquare = evaluator.Square(encProd);

            // Print size and noise budget of result. 
            Console.WriteLine("Size of encSquare: {0}", encSquare.Size);
            Console.WriteLine("Noise budget in encSquare: {0} bits", decryptor.InvariantNoiseBudget(encSquare));
        }

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
    }
}
