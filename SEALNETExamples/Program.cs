using System;
using Microsoft.Research.SEAL;
using System.Collections.Generic;

using System.IO;

namespace SEALNETExamples
{
    public class SEALExamples
    {
        /*
        Helper function: Prints the name of the example in a fancy banner.
        */
        private static void PrintExampleBanner(string title)
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

        /*
        Helper function: Prints the parameters in a SEALContext.
        */
        private static void PrintParameters(SEALContext context)
        {
            /*
            Print the size of the true (product) coefficient modulus.
            */
            Console.WriteLine("/ Encryption parameters:");
            Console.WriteLine($"| PolyModulus: {context.PolyModulus}");
            Console.WriteLine("| CoeffModulus size: {0} bits",
                context.TotalCoeffModulus.GetSignificantBitCount());
            Console.WriteLine($"| PlainModulus: {context.PlainModulus.Value}");
            Console.WriteLine($"\\ NoiseStandardDeviation: {context.NoiseStandardDeviation}\n");
        }

        /*
        Helper function: We need to print some large matrices which is a bit of a pain. 
        */
        private static void PrintMatrix(List<UInt64> matrix)
        {
            Console.WriteLine("");
            int rowSize = matrix.Count / 2;

            /*
            We're not going to print every column of the matrix (there are 2048). 
            Instead print this many slots from beginning and end of the matrix.             
            */
            int printSize = 5;

            Console.Write("    [");
            for (int i = 0; i < printSize; i++)
            {
                Console.Write($"{matrix[i], 3},");
            }
            Console.Write("  ...,");
            for (int i = rowSize - printSize; i < rowSize; i++)
            {
                Console.Write((i != rowSize - 1) ? $"{matrix[i], 3}," : $"{matrix[i], 3} ]\n");
            }
            Console.Write("    [");
            for (int i = rowSize; i < rowSize + printSize; i++)
            {
                Console.Write($"{matrix[i], 3},");
            }
            Console.Write("  ...,");
            for (int i = 2 * rowSize - printSize; i < 2 * rowSize; i++)
            {
                Console.Write((i != 2 * rowSize - 1) ? $"{matrix[i], 3}," : $"{matrix[i], 3} ]\n");
            }
            Console.WriteLine("");
        }

        public static void Main()
        {
            while (true)
            {
                Console.WriteLine("\nSEAL Examples:\n");
                Console.WriteLine("  1. Basics I");
                Console.WriteLine("  2. Basics II");
                Console.WriteLine("  3. Weighted Average");
                Console.WriteLine("  4. Batching with PolyCRTBuilder");
                Console.WriteLine("  5. Automatic Parameter Selection");
                Console.WriteLine("  0. Exit");

                /*
                Print how much memory we have allocated in global memory pool. 
                */
                Console.WriteLine("\nTotal memory allocated by global memory pool: {0} MB",
                    MemoryPoolHandle.Global().AllocByteCount >> 20);

                Console.Write("\nRun example: ");
                string selString = Console.ReadLine();
                if (!int.TryParse(selString, out int selection))
                {
                    Console.WriteLine("Invalid option.");
                    Console.WriteLine("\nPress any key to continue ...");
                    Console.ReadKey();
                    Console.Clear();
                    continue;
                }

                switch (selection)
                {
                    case 1:
                        ExampleBasicsI();
                        break;

                    case 2:
                        ExampleBasicsII();
                        break;

                    case 3:
                        ExampleWeightedAverage();
                        break;

                    case 4:
                        ExampleBatching();
                        break;

                    case 5:
                        ExampleParameterSelection();
                        break;

                    case 0:
                        return;

                    default:
                        Console.WriteLine("Invalid option.");
                        break;
                }

                Console.WriteLine("\nPress any key to continue ...");
                Console.ReadKey();
                Console.Clear();
            }
        }

        public static void ExampleBasicsI()
        {
            PrintExampleBanner("Example: Basics I");

            /*
            In this example we demonstrate setting up encryption parameters and other 
            relevant objects for performing simple computations on encrypted integers.

            SEAL uses the Fan-Vercauteren (FV) homomorphic encryption scheme. We refer to
            https://eprint.iacr.org/2012/144 for full details on how the FV scheme works.
            For better performance, SEAL implements the "FullRNS" optimization of FV, as 
            described in https://eprint.iacr.org/2016/510.
            */

            /*
            The first task is to set up an instance of the EncryptionParameters class.
            It is critical to understand how these different parameters behave, how they
            affect the encryption scheme, performance, and the security level. There are 
            three encryption parameters that are necessary to set: 

                - PolyModulus (polynomial modulus);
                - CoeffModulus ([ciphertext] coefficient modulus);
                - PlainModulus (plaintext modulus).

            A fourth parameter -- NoiseStandardDeviation -- has a default value of 3.19 
            and should not be necessary to modify unless the user has a specific reason 
            to and knows what they are doing.

            The encryption scheme implemented in SEAL cannot perform arbitrary computations
            on encrypted data. Instead, each ciphertext has a specific quantity called the
            `invariant noise budget' -- or `noise budget' for short -- measured in bits. 
            The noise budget of a freshly encrypted ciphertext (initial noise budget) is 
            determined by the encryption parameters. Homomorphic operations consume the 
            noise budget at a rate also determined by the encryption parameters. In SEAL 
            the two basic homomorphic operations are additions and multiplications, of 
            which additions can generally be thought of as being nearly free in terms of 
            noise budget consumption compared to multiplications. Since noise budget 
            consumption is compounding in sequential multiplications, the most significant 
            factor in choosing appropriate encryption parameters is the multiplicative 
            depth of the arithmetic circuit that needs to be evaluated. Once the noise 
            budget in a ciphertext reaches zero, it becomes too corrupted to be decrypted. 
            Thus, it is essential to choose the parameters to be large enough to support 
            the desired computation; otherwise the result is impossible to make sense of 
            even with the secret key.
            */
            var parms = new EncryptionParameters();

            /*
            We first set the polynomial modulus. This must be a power-of-2 cyclotomic 
            polynomial, i.e. a polynomial of the form "1x^(power-of-2) + 1". The polynomial
            modulus should be thought of mainly affecting the security level of the scheme;
            larger polynomial modulus makes the scheme more secure. At the same time, it
            makes ciphertext sizes larger, and consequently all operations slower. Recommended
            degrees for PolyModulus are 1024, 2048, 4096, 8192, 16384, 32768, but it is also 
            possible to go beyond this. Since we perform only a very small computation in this 
            example, it suffices to use a very small polynomial modulus.
            */
            parms.PolyModulus = "1x^2048 + 1";

            /*
            Next we choose the [ciphertext] coefficient modulus (CoeffModulus). The size of 
            the coefficient modulus should be thought of as the most significant factor in 
            determining the noise budget in a freshly encrypted ciphertext: bigger means
            more noise budget. Unfortunately, a larger coefficient modulus also lowers the
            security level of the scheme. Thus, if a large noise budget is required for
            complicated computations, a large coefficient modulus needs to be used, and the
            reduction in the security level must be countered by simultaneously increasing
            the polynomial modulus. 

            To make parameter selection easier for the user, we have constructed sets of 
            largest allowed coefficient moduli for 128-bit and 192-bit security levels
            for different choices of the polynomial modulus. These recommended parameters
            follow the Security white paper at http://HomomorphicEncryption.org. However,
            due to the complexity of this topic, we highly recommend the user to directly 
            consult an expert in homomorphic encryption and RLWE-based encryption schemes 
            to determine the security of their parameter choices. 

            Our recommended values for the coefficient modulus can be easily accessed 
            through the functions 
            
                DefaultParams.CoeffModulus128(int)
                DefaultParams.CoeffModulus192(int)

            for 128-bit and 192-bit security levels. The integer parameter is the degree
            of the polynomial modulus.

            In SEAL the coefficient modulus is a positive composite number -- a product
            of distinct primes of size up to 60 bits. When we talk about the size of the 
            coefficient modulus we mean the bit length of the product of the small primes. 
            The small primes are represented by instances of the SmallModulus class; for 
            example DefaultParams.CoeffModulus128(int) returns a vector of SmallModulus 
            instances. 

            It is possible for the user to select their own small primes. Since SEAL uses
            the Number Theoretic Transform (NTT) for polynomial multiplications modulo the
            factors of the coefficient modulus, the factors need to be prime numbers
            congruent to 1 modulo 2*degree(PolyModulus). We have generated a list of such
            prime numbers of various sizes, that the user can easily access through the
            functions 
            
                DefaultParams.SmallMods60bit(int)
                DefaultParams.SmallMods50bit(int)
                DefaultParams.SmallMods40bit(int)
                DefaultParams.SmallMods30bit(int)

            each of which gives access to an array of primes of the denoted size. These 
            primes are located in the source file util/globals.cpp in the C++ library.

            Performance is mainly affected by the size of the polynomial modulus, and the
            number of prime factors in the coefficient modulus. Thus, it is important to
            use as few factors in the coefficient modulus as possible.

            In this example we use the default coefficient modulus for a 128-bit security
            level. Concretely, this coefficient modulus consists of only one 56-bit prime 
            factor: 0xfffffffff00001.
            */
            parms.CoeffModulus = DefaultParams.CoeffModulus128(2048);

            /*
            The plaintext modulus can be any positive integer, even though here we take 
            it to be a power of two. In fact, in many cases one might instead want it to 
            be a prime number; we will see this in ExampleBatching(). The plaintext 
            modulus determines the size of the plaintext data type, but it also affects 
            the noise budget in a freshly encrypted ciphertext, and the consumption of 
            the noise budget in homomorphic multiplication. Thus, it is essential to try 
            to keep the plaintext data type as small as possible for good performance.
            The noise budget in a freshly encrypted ciphertext is 

                ~ log2(CoeffModulus/PlainModulus) (bits)

            and the noise budget consumption in a homomorphic multiplication is of the 
            form log2(PlainModulus) + (other terms).
            */
            parms.PlainModulus = 1 << 8;

            /*
            Now that all parameters are set, we are ready to construct a SEALContext 
            object. This is a heavy class that checks the validity and properties of 
            the parameters we just set, and performs and stores several important 
            pre-computations.
            */
            var context = new SEALContext(parms);

            /*
            Print the parameters that we have chosen.
            */
            PrintParameters(context);

            /*
            Plaintexts in the FV scheme are polynomials with coefficients integers modulo 
            plain_modulus. To encrypt for example integers instead, one can use an 
            `encoding scheme' to represent the integers as such polynomials. SEAL comes 
            with a few basic encoders:

            [IntegerEncoder]
            Given an integer base b, encodes integers as plaintext polynomials as follows. 
            First, a base-b expansion of the integer is computed. This expansion uses 
            a `balanced' set of representatives of integers modulo b as the coefficients. 
            Namely, when b is odd the coefficients are integers between -(b-1)/2 and 
            (b-1)/2. When b is even, the integers are between -b/2 and (b-1)/2, except 
            when b is two and the usual binary expansion is used (coefficients 0 and 1). 
            Decoding amounts to evaluating the polynomial at x=b. For example, if b=2, 
            the integer 

                26 = 2^4 + 2^3 + 2^1

            is encoded as the polynomial 1x^4 + 1x^3 + 1x^1. When b=3, 

                26 = 3^3 - 3^0 

            is encoded as the polynomial 1x^3 - 1. In memory polynomial coefficients are 
            always stored as unsigned integers by storing their smallest non-negative 
            representatives modulo PlainModulus. To create a base-b integer encoder, 
            use the constructor IntegerEncoder(PlainModulus, b). If no b is given, b=2 
            is used.

            [FractionalEncoder]
            The FractionalEncoder encodes fixed-precision rational numbers as follows. 
            It expands the number in a given base b, possibly truncating an infinite 
            fractional part to finite precision, e.g. 

                26.75 = 2^4 + 2^3 + 2^1 + 2^(-1) + 2^(-2) 

            when b=2. For the sake of the example, suppose PolyModulus is 1x^1024 + 1. 
            It then represents the integer part of the number in the same way as in 
            IntegerEncoder (with b=2 here), and moves the fractional part instead to the 
            highest degree part of the polynomial, but with signs of the coefficients 
            changed. In this example we would represent 26.75 as the polynomial 

                -1x^1023 - 1x^1022 + 1x^4 + 1x^3 + 1x^1. 

            In memory the negative coefficients of the polynomial will be represented as 
            their negatives modulo PlainModulus.

            [PolyCRTBuilder]
            If PlainModulus is a prime congruent to 1 modulo 2*degree(PolyModulus), the 
            plaintext elements can be viewed as 2-by-(degree(PolyModulus) / 2) matrices
            with elements integers modulo PlainModulus. When a desired computation can be
            vectorized, using PolyCRTBuilder can result in massive performance improvements 
            over naively encrypting and operating on each input number separately. Thus, 
            in more complicated computations this is likely to be by far the most important 
            and useful encoder. In ExampleBatching() we show how to use and operate on 
            encrypted matrix plaintexts.

            For performance reasons, in homomorphic encryption one typically wants to keep
            the plaintext data types as small as possible, which can make it challenging to 
            prevent data type overflow in more complicated computations, especially when 
            operating on rational numbers that have been scaled to integers. When using 
            PolyCRTBuilder estimating whether an overflow occurs is a fairly standard task, 
            as the matrix slots are integers modulo PlainModulus, and each slot is operated 
            on independently of the others. When using IntegerEncoder or FractionalEncoder
            it is substantially more difficult to estimate when an overflow occurs in the 
            plaintext, and choosing the plaintext modulus very carefully to be large enough 
            is critical to avoid unexpected results. Specifically, one needs to estimate how 
            large the largest coefficient in  the polynomial view of all of the plaintext 
            elements becomes, and choose the plaintext modulus to be larger than this value. 
            SEAL comes with an automatic parameter selection tool that can help with this 
            task, as is demonstrated in ExampleParameterSelection().

            Here we choose to create an IntegerEncoder with base b=2.
            */
            var encoder = new IntegerEncoder(context.PlainModulus);

            /*
            We are now ready to generate the secret and public keys. For this purpose we need
            an instance of the KeyGenerator class. Constructing a KeyGenerator automatically 
            generates the public and secret key, which can then be read to local variables. 
            To create a fresh pair of keys one can call KeyGenerator.Generate() at any time.
            */
            var keygen = new KeyGenerator(context);
            var publicKey = keygen.PublicKey;
            var secretKey = keygen.SecretKey;

            /*
            To be able to encrypt, we need to construct an instance of Encryptor. Note that
            the Encryptor only requires the public key.
            */
            var encryptor = new Encryptor(context, publicKey);

            /*
            Computations on the ciphertexts are performed with the Evaluator class.
            */
            var evaluator = new Evaluator(context);

            /*
            We will of course want to decrypt our results to verify that everything worked,
            so we need to also construct an instance of Decryptor. Note that the Decryptor
            requires the secret key.
            */
            var decryptor = new Decryptor(context, secretKey);

            /*
            We start by encoding two integers as plaintext polynomials.
            */
            int value1 = 5;
            var plain1 = encoder.Encode(value1);
            Console.WriteLine($"Encoded {value1} as polynomial {plain1} (plain1)");

            int value2 = 7;
            var plain2 = encoder.Encode(value2);
            Console.WriteLine($"Encoded {value2} as polynomial {plain2} (plain2)");

            /*
            Encrypting the values is easy.
            */
            var encrypted1 = new Ciphertext();
            var encrypted2 = new Ciphertext();
            Console.Write("Encrypting plain1: ");
            encryptor.Encrypt(plain1, encrypted1);
            Console.WriteLine("Done (encrypted1)");

            Console.Write("Encrypting plain2: ");
            encryptor.Encrypt(plain2, encrypted2);
            Console.WriteLine("Done (encrypted2)");

            /*
            To illustrate the concept of noise budget, we print the budgets in the fresh 
            encryptions.
            */
            Console.WriteLine("Noise budget in encrypted1: {0} bits",
                decryptor.InvariantNoiseBudget(encrypted1));
            Console.WriteLine("Noise budget in encrypted2: {0} bits",
                decryptor.InvariantNoiseBudget(encrypted2));

            /*
            As a simple example, we compute (-encrypted1 + encrypted2) * encrypted2.
            */

            /*
            Negation is a unary operation.
            */
            evaluator.Negate(encrypted1);

            /*
            Negation does not consume any noise budget.
            */
            Console.WriteLine("Noise budget in -encrypted1: {0} bits",
                decryptor.InvariantNoiseBudget(encrypted1));

            /*
            Addition can be done in-place (overwriting the first argument with the result,
            or alternatively a three-argument overload with a separate destination variable
            can be used. The in-place variants are always more efficient. Here we overwrite
            encrypted1 with the sum.
            */
            evaluator.Add(encrypted1, encrypted2);

            /*
            It is instructive to think that addition sets the noise budget to the minimum
            of the input noise budgets. In this case both inputs had roughly the same
            budget going on, and the output (in encrypted1) has just slightly lower budget.
            Depending on probabilistic effects, the noise growth consumption may or may 
            not be visible when measured in whole bits.
            */
            Console.WriteLine("Noise budget in -encrypted1 + encrypted2: {0} bits",
                decryptor.InvariantNoiseBudget(encrypted1));

            /*
            Finally multiply with encrypted2. Again, we use the in-place version of the
            function, overwriting encrypted1 with the product.
            */
            evaluator.Multiply(encrypted1, encrypted2);

            /*
            Multiplication consumes a lot of noise budget. This is clearly seen in the
            print-out. The user can change the PlainModulus to see its effect on the
            rate of
            noise budget consumption.
            */
            Console.WriteLine("Noise budget in (-encrypted1 + encrypted2) * encrypted2: {0} bits",
                decryptor.InvariantNoiseBudget(encrypted1));

            /*
            Now we decrypt and decode our result.
            */
            var plainResult = new Plaintext();
            Console.Write("Decrypting result: ");
            decryptor.Decrypt(encrypted1, plainResult);
            Console.WriteLine("Done");

            /*
            Print the result plaintext polynomial.
            */
            Console.WriteLine($"Plaintext polynomial: {plainResult}");

            /*
            Decode to obtain an integer result.
            */
            Console.WriteLine($"Decoded integer: {encoder.DecodeInt32(plainResult)}");

            /*
            We finish by running garbage collection to make sure all local objects are
            destroyed and memory returned to the memory pool. This is very important to
            ensure correct behavior of the SEAL memory pool in .NET applications.
            */
            GC.Collect();
        }

        public static void ExampleBasicsII()
        {
            PrintExampleBanner("Example: Basics II");

            /*
            In this example we explain what relinearization is, how to use it, and how 
            it affects noise budget consumption.

            First we set the parameters, create a SEALContext, and generate the public
            and secret keys. We use slightly larger parameters than be fore to be able
            to do more homomorphic multiplications.
            */
            var parms = new EncryptionParameters();
            parms.PolyModulus = "1x^8192 + 1";

            /*
            The default coefficient modulus consists of the following primes:

                0x7fffffffba0001, 
                0x7fffffffaa0001, 
                0x7fffffff7e0001, 
                0x3fffffffd60001.

            The total size is 219 bits.
            */
            parms.CoeffModulus = DefaultParams.CoeffModulus128(8192);
            parms.PlainModulus = 1 << 10;

            var context = new SEALContext(parms);
            PrintParameters(context);

            var keygen = new KeyGenerator(context);
            var publicKey = keygen.PublicKey;
            var secretKey = keygen.SecretKey;

            /*
            We also set up an Encryptor, Evaluator, and Decryptor here. We will
            encrypt polynomials directly in this example, so there is no need for 
            an encoder.
            */
            var encryptor = new Encryptor(context, publicKey);
            var evaluator = new Evaluator(context);
            var decryptor = new Decryptor(context, secretKey);

            /*
            There are actually two more types of keys in SEAL: `evaluation keys' and
            `Galois keys'. Here we will discuss evaluation keys, and Galois keys will
            be discussed later in ExampleBatching().

            In SEAL, a valid ciphertext consists of two or more polynomials with 
            coefficients integers modulo the product of the primes in CoeffModulus. 
            The current size of a ciphertext can be found using Ciphertext.Size.
            A freshly encrypted ciphertext always has size 2.
            */
            var plain1 = new Plaintext("1x^2 + 2x^1 + 3");
            var encrypted = new Ciphertext();
            Console.Write($"Encrypting {plain1}: ");
            encryptor.Encrypt(plain1, encrypted);
            Console.WriteLine("Done");
            Console.WriteLine($"Size of a fresh encryption: {encrypted.Size}");
            Console.WriteLine("Noise budget in fresh encryption: {0} bits",
                decryptor.InvariantNoiseBudget(encrypted));

            /*
            Homomorphic multiplication results in the output ciphertext growing in size. 
            More precisely, if the input ciphertexts have size M and N, then the output 
            ciphertext after homomorphic multiplication will have size M+N-1. In this
            case we square encrypted twice to observe this growth (also observe noise
            budget consumption).
            */
            evaluator.Square(encrypted);
            Console.WriteLine($"Size after squaring: {encrypted.Size}");
            Console.WriteLine("Noise budget after squaring: {0} bits",
                decryptor.InvariantNoiseBudget(encrypted));

            evaluator.Square(encrypted);
            Console.WriteLine($"Size after second squaring: {encrypted.Size}");
            Console.WriteLine("Noise budget after second squaring: {0} bits",
                decryptor.InvariantNoiseBudget(encrypted));

            /*
            It does not matter that the size has grown -- decryption works as usual.
            Observe from the print-out that the coefficients in the plaintext have
            grown quite large. One more squaring would cause some of them to wrap 
            around plain_modulus (0x400), and as a result we would no longer obtain 
            the expected result as an integer-coefficient polynomial. We can fix this
            problem to some extent by increasing PlainModulus. This would make sense,
            since we still have plenty of noise budget left.
            */
            var plain2 = new Plaintext();
            decryptor.Decrypt(encrypted, plain2);
            Console.WriteLine($"Fourth power: {plain2}\n");

            /*
            The problem here is that homomorphic operations on large ciphertexts are
            computationally much more costly than on small ciphertexts. Specifically,
            homomorphic multiplication on input ciphertexts of size M and N will require 
            O(M*N) polynomial multiplications to be performed, and an addition will
            require O(M+N) additions. Relinearization reduces the size of the ciphertexts
            after multiplication back to the initial size (2). Thus, relinearizing one
            or both inputs before the next multiplication, or e.g. before serializing the
            ciphertexts, can have a huge positive impact on performance.

            Another problem is that the noise budget consumption in multiplication is
            bigger when the input ciphertexts sizes are bigger. In a complicated
            computation the contribution of the sizes to the noise budget consumption
            can actually become the dominant term. We will point this out again below
            once we get to our example.

            Relinearization itself has both a computational cost and a noise budget cost.
            These both depend on a parameter called `decomposition bit count', which can
            be any integer at least 1 [DefaultParms.DBCmin] and at most 60 [DefaultParams.DBCmax]. 
            A large decomposition bit count makes relinearization fast, but consumes
            more noise budget. A small decomposition bit count can make relinearization 
            slower, but might not change the noise budget by any observable amount.

            Relinearization requires a special type of key called `evaluation keys'.
            These can be created by the KeyGenerator for any decomposition bit count.
            To relinearize a ciphertext of size M >= 2 back to size 2, we actually need 
            M-2 evaluation keys. Attempting to relinearize a too large ciphertext with
            too few evaluation keys will result in an exception being thrown.

            We repeat our computation, but this time relinearize after both squarings.
            Since our ciphertext never grows past size 3 (we relinearize after every
            multiplication), it suffices to generate only one evaluation key.

            First, we need to create evaluation keys. We use a decomposition bit count 
            of 16 here, which can be thought of as quite small.
            */
            var evKeys16 = new EvaluationKeys();

            /*
            This function generates one single evaluation key. Another overload takes 
            the number of keys to be generated as an argument, but one is all we need
            in this example (see above).
            */
            keygen.GenerateEvaluationKeys(16, evKeys16);

            Console.Write($"Encrypting {plain1}: ");
            encryptor.Encrypt(plain1, encrypted);
            Console.WriteLine("Done");
            Console.WriteLine($"Size of a fresh encryption: {encrypted.Size}");
            Console.WriteLine("Noise budget in fresh encryption: {0} bits",
                decryptor.InvariantNoiseBudget(encrypted));

            evaluator.Square(encrypted);
            Console.WriteLine($"Size after squaring: {encrypted.Size}");
            Console.WriteLine("Noise budget after squaring: {0} bits",
                decryptor.InvariantNoiseBudget(encrypted));

            evaluator.Relinearize(encrypted, evKeys16);
            Console.WriteLine($"Size after relinearization: {encrypted.Size}");
            Console.WriteLine("Noise budget after relinearizing (dbc = {0}): {1} bits",
                evKeys16.DecompositionBitCount,
                decryptor.InvariantNoiseBudget(encrypted));

            evaluator.Square(encrypted);
            Console.WriteLine($"Size after second squaring: {encrypted.Size}");
            Console.WriteLine("Noise budget after second squaring: {0} bits",
                decryptor.InvariantNoiseBudget(encrypted));

            evaluator.Relinearize(encrypted, evKeys16);
            Console.WriteLine($"Size after relinearization: {encrypted.Size}");
            Console.WriteLine("Noise budget after relinearizing (dbc = {0}): {1} bits",
                evKeys16.DecompositionBitCount,
                decryptor.InvariantNoiseBudget(encrypted));

            decryptor.Decrypt(encrypted, plain2);
            Console.WriteLine($"Fourth power: {plain2}\n");

            /*
            Of course the result is still the same, but this time we actually
            used less of our noise budget. This is not surprising for two reasons:

                - We used a very small decomposition bit count, which is why
                  relinearization itself did not consume the noise budget by any
                  observable amount;
                - Since our ciphertext sizes remain small throughout the two
                  squarings, the noise budget consumption rate in multiplication
                  remains as small as possible. Recall from above that operations
                  on larger ciphertexts actually cause more noise growth.

            To make matters even more clear, we repeat the computation a third time,
            now using the largest possible decomposition bit count (60). We are not 
            measuring the time here, but relinearization with these evaluation keys
            is significantly faster than with evKeys16.
            */
            var evKeys60 = new EvaluationKeys();
            keygen.GenerateEvaluationKeys(DefaultParams.DBCmax, evKeys60);

            Console.Write($"Encrypting {plain1}: ");
            encryptor.Encrypt(plain1, encrypted);
            Console.WriteLine("Done");
            Console.WriteLine($"Size of a fresh encryption: {encrypted.Size}");
            Console.WriteLine("Noise budget in fresh encryption: {0} bits",
                decryptor.InvariantNoiseBudget(encrypted));

            evaluator.Square(encrypted);
            Console.WriteLine($"Size after squaring: {encrypted.Size}");
            Console.WriteLine("Noise budget after squaring: {0} bits",
                decryptor.InvariantNoiseBudget(encrypted));

            evaluator.Relinearize(encrypted, evKeys60);
            Console.WriteLine($"Size after relinearization: {encrypted.Size}");
            Console.WriteLine("Noise budget after relinearizing (dbc = {0}): {1} bits",
                evKeys60.DecompositionBitCount,
                decryptor.InvariantNoiseBudget(encrypted));

            evaluator.Square(encrypted);
            Console.WriteLine($"Size after second squaring: {encrypted.Size}");
            Console.WriteLine("Noise budget after second squaring: {0} bits",
                decryptor.InvariantNoiseBudget(encrypted));

            evaluator.Relinearize(encrypted, evKeys60);
            Console.WriteLine($"Size after relinearization: {encrypted.Size}");
            Console.WriteLine("Noise budget after relinearizing (dbc = {0}): {1} bits",
                evKeys60.DecompositionBitCount,
                decryptor.InvariantNoiseBudget(encrypted));

            decryptor.Decrypt(encrypted, plain2);
            Console.WriteLine($"Fourth power: {plain2}\n");

            /*
            Observe from the print-out that we have now used significantly more of our
            noise budget than in the two previous runs. This is again not surprising, 
            since the first relinearization chops off a huge part of the noise budget.

            However, note that the second relinearization does not change the noise
            budget by any observable amount. This is very important to understand when
            optimal performance is desired: relinearization always drops the noise
            budget from the maximum (freshly encrypted ciphertext) down to a fixed 
            amount depending on the encryption parameters and the decomposition bit 
            count. On the other hand, homomorphic multiplication always consumes the
            noise budget from its current level. This is why the second relinearization
            does not change the noise budget anymore: it is already consumed past the
            fixed amount determinted by the decomposition bit count and the encryption
            parameters. 

            We now perform a third squaring and observe an even further compounded
            decrease in the noise budget. Again, relinearization does not consume the
            noise budget at this point by any observable amount, even with the largest
            possible decomposition bit count.
            */
            evaluator.Square(encrypted);
            Console.WriteLine($"Size after third squaring: {encrypted.Size}");
            Console.WriteLine("Noise budget after third squaring: {0} bits",
                decryptor.InvariantNoiseBudget(encrypted));

            evaluator.Relinearize(encrypted, evKeys60);
            Console.WriteLine($"Size after relinearization: {encrypted.Size}");
            Console.WriteLine("Noise budget after relinearizing (dbc = {0}): {1} bits",
                evKeys60.DecompositionBitCount,
                decryptor.InvariantNoiseBudget(encrypted));

            decryptor.Decrypt(encrypted, plain2);
            Console.WriteLine($"Eighth power: {plain2}");
            /*
            Observe from the print-out that the polynomial coefficients are no longer
            correct as integers: they have been reduced modulo PlainModulus, and there
            was no warning sign about this. It might be necessary to carefully analyze
            the computation to make sure such overflow does not occur unexpectedly.

            These experiments suggest that an optimal strategy might be to relinearize
            first with evaluation keys with a small decomposition bit count, and later
            with evaluation keys with a larger decomposition bit count (for performance)
            when noise budget has already been consumed past the bound determined by the
            larger decomposition bit count. For example, above the best strategy might
            have been to use evKeys16 in the first relinearization, and evKeys60 in the
            next two relinearizations for optimal noise budget consumption/performance
            trade-off. Luckily, in most use-cases it is not so critical to squeeze out
            every last bit of performance, especially when slightly larger parameters
            are used.
            */

            /*
            Run garbage collection to help the global memory pool.
            */
            GC.Collect();
        }

        public static void ExampleWeightedAverage()
        {
            PrintExampleBanner("Example: Weighted Average");

            /*
            In this example we demonstrate the FractionalEncoder, and use it to compute
            a weighted average of 10 encrypted rational numbers. In this computation we
            perform homomorphic multiplications of ciphertexts by plaintexts, which is
            much faster than regular multiplications of ciphertexts by ciphertexts.
            Moreover, such `plain multiplications' never increase the ciphertext size,
            which is why we have no need for evaluation keys in this example.

            We start by creating encryption parameters, setting up the SEALContext, keys, 
            and other relevant objects. Since our computation has multiplicative depth of
            only two, it suffices to use a small PolyModulus.
            */
            var parms = new EncryptionParameters();
            parms.PolyModulus = "1x^2048 + 1";
            parms.CoeffModulus = DefaultParams.CoeffModulus128(2048);
            parms.PlainModulus = 1 << 8;

            var context = new SEALContext(parms);
            PrintParameters(context);

            var keygen = new KeyGenerator(context);
            var publicKey = keygen.PublicKey;
            var secretKey = keygen.SecretKey;

            /*
            We also set up an Encryptor, Evaluator, and Decryptor here.
            */
            var encryptor = new Encryptor(context, publicKey);
            var evaluator = new Evaluator(context);
            var decryptor = new Decryptor(context, secretKey);

            /*
            Create a list of 10 rational numbers (as doubles).
            */
            var rationalNumbers = new List<double> {
                3.1, 4.159, 2.65, 3.5897, 9.3, 2.3, 8.46, 2.64, 3.383, 2.795
            };

            /*
            Create a list of weights.
            */
            var coefficients = new List<double> {
                0.1, 0.05, 0.05, 0.2, 0.05, 0.3, 0.1, 0.025, 0.075, 0.05
            };

            /*
            We need a FractionalEncoder to encode the rational numbers into plaintext
            polynomials. In this case we decide to reserve 64 coefficients of the 
            polynomial for the integral part (low-degree terms) and expand the fractional 
            part to 32 digits of precision (in base 3) (high-degree terms). These numbers 
            can be changed according to the precision that is needed; note that these 
            choices leave a lot of unused space in the 2048-coefficient polynomials.
            */
            var encoder = new FractionalEncoder(context.PlainModulus, context.PolyModulus, 64, 32, 3);

            /*
            We create a list of ciphertexts for encrypting the rational numbers.
            */
            var encryptedRationals = new List<Ciphertext>();
            Console.Write("Encoding and encrypting: ");
            for (int i = 0; i < 10; i++)
            {
                /*
                We create our Ciphertext objects into the vector by passing the 
                encryption parameters as an argument to the constructor. This ensures 
                that enough memory is allocated for a size 2 ciphertext. In this example 
                our ciphertexts never grow in size (plain multiplication does not cause 
                ciphertext growth), so we can expect the ciphertexts to remain in the same 
                location in memory throughout the computation. In more complicated examples  
                one might want to call a constructor that reserves enough memory for the 
                ciphertext to grow to a specified size to avoid costly memory moves when
                multiplications and relinearizations are performed.
                */
                encryptedRationals.Add(new Ciphertext(parms));
                encryptor.Encrypt(encoder.Encode(rationalNumbers[i]), encryptedRationals[i]);
                Console.Write(rationalNumbers[i] + ((i < 9) ? ", " : "\n"));
            }

            /*
            Next we encode the coefficients. There is no reason to encrypt these since they 
            are not private data.
            */
            var encodedCoefficients = new List<Plaintext>();
            Console.Write("Encoding plaintext coefficients: ");
            for (int i = 0; i < 10; i++)
            {
                encodedCoefficients.Add(encoder.Encode(coefficients[i]));
                Console.Write(coefficients[i] + ((i < 9) ? ", " : "\n"));
            }

            /*
            We also need to encode 0.1. Multiplication by this plaintext will have the 
            effect of dividing by 10. Note that in SEAL it is impossible to divide
            ciphertext by another ciphertext, but in this way division by a plaintext is
            possible.
            */
            var divByTen = encoder.Encode(0.1);

            /*
            Now compute each multiplication.
            */
            Console.Write("Computing products: ");
            for (int i = 0; i < 10; i++)
            {
                /*
                Note how we use plain multiplication instead of usual multiplication. The
                result overwrites the first argument in the function call.
                */
                evaluator.MultiplyPlain(encryptedRationals[i], encodedCoefficients[i]);
            }
            Console.WriteLine("Done");

            /*
            To obtain the linear sum we need to still compute the sum of the ciphertexts
            in encryptedRationals. There is an easy way to add together a vector of 
            Ciphertexts.
            */
            var encryptedResult = new Ciphertext();
            Console.Write("Adding up all 10 ciphertexts: ");
            evaluator.AddMany(encryptedRationals, encryptedResult);
            Console.WriteLine("Done");

            /*
            Perform division by 10 by plain multiplication with divByTen.
            */
            Console.Write("Dividing by 10: ");
            evaluator.MultiplyPlain(encryptedResult, divByTen);
            Console.WriteLine("Done");

            /*
            How much noise budget do we have left?
            */
            Console.WriteLine("Noise budget in result: {0} bits",
                decryptor.InvariantNoiseBudget(encryptedResult));

            /*
            Decrypt, decode, and print result.
            */
            var plainResult = new Plaintext();
            Console.Write("Decrypting result: ");
            decryptor.Decrypt(encryptedResult, plainResult);
            Console.WriteLine("Done");
            double result = encoder.Decode(plainResult);
            Console.WriteLine($"Weighted average: {result}");

            /*
            Run garbage collection to help the global memory pool.
            */
            GC.Collect();
        }

        public static void ExampleParameterSelection()
        {
            PrintExampleBanner("Example: Automatic Parameter Selection");

            /*
            SEAL contains an automatic parameter selection tool that can help the user
            select optimal parameters that support a particular computation. In this
            example we show how the tool can be used to find parameters for evaluating
            the degree 3 polynomial 42x^3-27x+1 on an encrypted input encoded with the
            IntegerEncoder. For this to be possible, we need to know an upper bound on
            the size of the input, and in this example assume that x is an integer with
            base-3 representation of length at most 10.
            */
            Console.Write("Finding optimized parameters for computing 42x^3-27x+1: ");

            /*
            The set of tools in the parameter selector are ChooserPoly, ChooserEvaluator,
            ChooserEncoder, ChooserEncryptor, and ChooserDecryptor. Of these the most
            important ones are ChooserPoly, which is an object representing the input
            data both in plaintext and encrypted form, and ChooserEvaluator, which
            simulates plaintext coefficient growth and noise budget consumption in the
            computations. Here we use also the ChooserEncoder to conveniently obtain
            ChooserPoly objects modeling the plaintext coefficients 42, -27, and 1.

            Note that we are using the IntegerEncoder with base 3.
            */
            var chooserEncoder = new ChooserEncoder(3);
            var chooserEvaluator = new ChooserEvaluator();

            /*
            First we create a ChooserPoly representing the input data. You can think of 
            this modeling a freshly encrypted ciphertext of a plaintext polynomial of
            length at most 10 coefficients, where the coefficients have absolute value 
            at most 1 (as is the case when using IntegerEncoder with base 3).
            */
            var cInput = new ChooserPoly(10, 1);

            /*
            Normally Evaluator::exponentiate takes the evaluation keys as argument. Since 
            no keys exist here, we simply pass the desired decomposition bit count (15)
            to the ChooserEvaluator.Exponentiate function.

            Here we compute the first term.
            */
            var cCubedInput = chooserEvaluator.Exponentiate(cInput, 3, 15);
            var cTerm1 = chooserEvaluator.MultiplyPlain(cCubedInput, 
                chooserEncoder.Encode(42));

            /*
            Then compute the second term.
            */
            var cTerm2 = chooserEvaluator.MultiplyPlain(cInput, chooserEncoder.Encode(27));

            /*
            Subtract the first two terms.
            */
            var cSum12 = chooserEvaluator.Sub(cTerm1, cTerm2);

            /*
            Finally add a plaintext constant 1.
            */
            var cResult = chooserEvaluator.AddPlain(cSum12, chooserEncoder.Encode(1));

            /*
            The optimal parameters are now computed using the SelectParameters 
            function in ChooserEvaluator. It is possible to give this function the 
            results of several distinct computations (as ChooserPoly objects), all 
            of which are supposed to be possible to perform with the resulting set
            of parameters. However, here we have only one input ChooserPoly.
            */
            var optimalParms = new EncryptionParameters();
            chooserEvaluator.SelectParameters(new List<ChooserPoly> { cResult }, 0, optimalParms);
            Console.WriteLine("Done");

            /*
            Create an SEALContext object for the returned parameters
            */
            var optimalContext = new SEALContext(optimalParms);
            PrintParameters(optimalContext);

            /*
            Do the parameters actually make any sense? We can try to perform the 
            homomorphic computation using the given parameters and see what happens.
            */
            var keygen = new KeyGenerator(optimalContext);
            var publicKey = keygen.PublicKey;
            var secretKey = keygen.SecretKey;
            var evKeys = new EvaluationKeys();
            keygen.GenerateEvaluationKeys(15, evKeys);

            var encryptor = new Encryptor(optimalContext, publicKey);
            var evaluator = new Evaluator(optimalContext);
            var decryptor = new Decryptor(optimalContext, secretKey);
            var encoder = new IntegerEncoder(optimalContext.PlainModulus, 3);

            /*
            Now perform the computations on some real data.
            */
            int inputValue = 12345;
            var plainInput = encoder.Encode(inputValue);
            Console.WriteLine($"Encoded {inputValue} as polynomial {plainInput}");

            var input = new Ciphertext();
            Console.Write("Encrypting: ");
            encryptor.Encrypt(plainInput, input);
            Console.WriteLine("Done");

            Console.Write("Computing 42x^3-27x+1 on encrypted x=12345: ");
            var deg3Term = new Ciphertext();
            evaluator.Exponentiate(input, 3, evKeys, deg3Term);
            evaluator.MultiplyPlain(deg3Term, encoder.Encode(42));
            var deg1Term = new Ciphertext();
            evaluator.MultiplyPlain(input, encoder.Encode(27), deg1Term);
            evaluator.Sub(deg3Term, deg1Term);
            evaluator.AddPlain(deg3Term, encoder.Encode(1));
            Console.WriteLine("Done");

            /*
            Now deg3Term holds the result. We decrypt, decode, and print the result.
            */
            var plainResult = new Plaintext();
            Console.Write("Decrypting: ");
            decryptor.Decrypt(deg3Term, plainResult);
            Console.WriteLine("Done");
            Console.WriteLine("Polynomial 42x^3-27x+1 evaluated at x=12345: {0}",
                encoder.DecodeInt64(plainResult));

            /*
            We should have a reasonable amount of noise room left if the parameter
            selection was done properly. The user can experiment for instance by 
            changing the decomposition bit count, and observing how it affects the 
            result. Typically the budget should never be even close to 0. Instead, 
            SEAL uses heuristic upper bound estimates on the noise budget consumption,
            which ensures that the computation will succeed with very high probability
            with the selected parameters.
            */
            Console.WriteLine("Noise budget in result: {0} bits",
                decryptor.InvariantNoiseBudget(deg3Term));

            /*
            Run garbage collection to help the global memory pool.
            */
            GC.Collect();
        }

        public static void ExampleBatching()
        {
            PrintExampleBanner("Example: Batching with PolyCRTBuilder");

            /*
            In this fundamental example we discuss and demonstrate a powerful technique 
            called `batching'. If N denotes the degree of the polynomial modulus, and T
            the plaintext modulus, then batching is automatically enabled in SEAL if
            T is a prime and congruent to 1 modulo 2*N. In batching the plaintexts are
            viewed as matrices of size 2-by-(N/2) with each element an integer modulo T.
            Homomorphic operations act element-wise between encrypted matrices, allowing
            the user to obtain speeds-ups of several orders of magnitude in naively
            vectorizable computations. We demonstrate two more homomorphic operations
            which act on encrypted matrices by rotating the rows cyclically, or rotate 
            the columns (i.e. swap the rows). These operations require the construction
            of so-called `Galois keys', which are very similar to evaluation keys.
            */
            var parms = new EncryptionParameters();

            parms.PolyModulus = "1x^4096 + 1";
            parms.CoeffModulus = DefaultParams.CoeffModulus128(4096);

            /*
            Note that 40961 is a prime number and 2*4096 divides 40960.
            */
            parms.PlainModulus = 40961;

            var context = new SEALContext(parms);
            PrintParameters(context);

            /*
            We can see that batching is indeed enabled by looking at the encryption
            parameter qualifiers created by SEALContext.
            */
            var qualifiers = context.Qualifiers;
            Console.WriteLine($"Batching enabled: {qualifiers.EnableBatching}");

            var keygen = new KeyGenerator(context);
            var publicKey = keygen.PublicKey;
            var secretKey = keygen.SecretKey;

            /*
            We need to create Galois keys for performing matrix row and column rotations.
            Like evaluation keys, the behavior of Galois keys depends on a decomposition
            bit count. The noise budget consumption behavior of matrix row and column 
            rotations is exactly like that of relinearization. Thus, we refer the reader
            to ExampleBasicsII() for more details.

            Here we use a moderate size decomposition bit count.
            */
            var galKeys = new GaloisKeys();
            keygen.GenerateGaloisKeys(30, galKeys);

            /*
            Since we are going to do some multiplications we will also relinearize.
            */
            var evKeys = new EvaluationKeys();
            keygen.GenerateEvaluationKeys(30, evKeys);

            /*
            We also set up an Encryptor, Evaluator, and Decryptor here.
            */
            var encryptor = new Encryptor(context, publicKey);
            var evaluator = new Evaluator(context);
            var decryptor = new Decryptor(context, secretKey);

            /*
            Batching is done through an instance of the PolyCRTBuilder class so need
            to start by constructing one.
            */
            var crtbuilder = new PolyCRTBuilder(context);

            /*
            The total number of batching `slots' is degree(PolyModulus). The matrices 
            we encrypt are of size 2-by-(slotCount / 2).
            */
            int slotCount = crtbuilder.SlotCount;
            int rowSize = slotCount / 2;
            Console.WriteLine($"Plaintext matrix row size: {rowSize}");

            /*
            The matrix plaintext is simply given to PolyCRTBuilder as a flattened list
            of numbers of size slotCount. The first rowSize numbers form the first row, 
            and the rest form the second row. Here we create the following matrix:

                [ 0,  1,  2,  3,  0,  0, ...,  0 ]
                [ 4,  5,  6,  7,  0,  0, ...,  0 ]
            */
            var podMatrix = new List<UInt64>(slotCount);
            for (int i = 0; i < slotCount; i++)
            {
                podMatrix.Add(0);
            }
            podMatrix[0] = 0;
            podMatrix[1] = 1;
            podMatrix[2] = 2;
            podMatrix[3] = 3;
            podMatrix[rowSize] = 4;
            podMatrix[rowSize + 1] = 5;
            podMatrix[rowSize + 2] = 6;
            podMatrix[rowSize + 3] = 7;

            Console.WriteLine("Input plaintext matrix:");
            PrintMatrix(podMatrix);

            /*
            First we use PolyCRTBuilder to compose the matrix into a plaintext.
            */
            var plainMatrix = new Plaintext();
            crtbuilder.Compose(podMatrix, plainMatrix);

            /*
            Next we encrypt the plaintext as usual.
            */
            var encryptedMatrix = new Ciphertext();
            Console.Write("Encrypting: ");
            encryptor.Encrypt(plainMatrix, encryptedMatrix);
            Console.WriteLine("Done");
            Console.WriteLine("Noise budget in fresh encryption: {0} bits",
                decryptor.InvariantNoiseBudget(encryptedMatrix));

            /*
            Operating on the ciphertext results in homomorphic operations being performed
            simultaneously in all 4096 slots (matrix elements). To illustrate this, we 
            form another plaintext matrix

                [ 1,  2,  1,  2,  1,  2, ..., 2 ]
                [ 1,  2,  1,  2,  1,  2, ..., 2 ]

            and compose it into a plaintext.
            */
            var podMatrix2 = new List<UInt64>(slotCount);
            for (int i = 0; i < slotCount; i++)
            {
                podMatrix2.Add(((UInt64)i % 2) + 1);
            }
            var plainMatrix2 = new Plaintext();
            crtbuilder.Compose(podMatrix2, plainMatrix2);
            Console.WriteLine("Second input plaintext matrix:");
            PrintMatrix(podMatrix2);

            /*
            We now add the second (plaintext) matrix to the encrypted one using another 
            new operation -- plain addition -- and square the sum.
            */
            Console.Write("Adding and squaring: ");
            evaluator.AddPlain(encryptedMatrix, plainMatrix2);
            evaluator.Square(encryptedMatrix);
            evaluator.Relinearize(encryptedMatrix, evKeys);
            Console.WriteLine("Done");

            /*
            How much noise budget do we have left?
            */
            Console.WriteLine("Noise budget in result: {0} bits",
                decryptor.InvariantNoiseBudget(encryptedMatrix));

            /*
            We decrypt and decompose the plaintext to recover the result as a matrix.
            */
            var plainResult = new Plaintext();
            Console.Write("Decrypting result: ");
            decryptor.Decrypt(encryptedMatrix, plainResult);
            Console.WriteLine("Done");

            var podResult = new List<UInt64>();
            crtbuilder.Decompose(plainResult, podResult);

            Console.WriteLine("Result plaintext matrix:");
            PrintMatrix(podResult);

            /*
            Note how the operation was performed in one go for each of the elements of the 
            matrix. It is possible to achieve incredible performance improvements by using 
            this method when the computation is easily vectorizable.

            All of our discussion so far could have applied just as well for a simple vector
            data type (not matrix). Now we show how the matrix view of the plaintext can be 
            used for more functionality. Namely, it is possible to rotate the matrix rows
            cyclically, and same for the columns (i.e. swap the two rows). For this we need
            the Galois keys that we generated earlier.

            We return to the original matrix that we started with.
            */
            encryptor.Encrypt(plainMatrix, encryptedMatrix);
            Console.WriteLine("Unrotated matrix: ");
            PrintMatrix(podMatrix);
            Console.WriteLine("Noise budget in fresh encryption: {0} bits",
                decryptor.InvariantNoiseBudget(encryptedMatrix));

            /*
            Now rotate the rows to the left 3 steps, decrypt, decompose, and print.
            */
            evaluator.RotateRows(encryptedMatrix, 3, galKeys);
            Console.WriteLine("Rotated rows 3 steps left: ");
            decryptor.Decrypt(encryptedMatrix, plainResult);
            crtbuilder.Decompose(plainResult, podResult);
            PrintMatrix(podResult);
            Console.WriteLine("Noise budget after rotation: {0} bits",
                decryptor.InvariantNoiseBudget(encryptedMatrix));

            /*
            Rotate columns (swap rows), decrypt, decompose, and print.
            */
            evaluator.RotateColumns(encryptedMatrix, galKeys);
            Console.WriteLine("Rotated columns: ");
            decryptor.Decrypt(encryptedMatrix, plainResult);
            crtbuilder.Decompose(plainResult, podResult);
            PrintMatrix(podResult);
            Console.WriteLine("Noise budget after rotation: {0} bits",
                decryptor.InvariantNoiseBudget(encryptedMatrix));

            /*
            Rotate rows to the right 4 steps, decrypt, decompose, and print.
            */
            evaluator.RotateRows(encryptedMatrix, -4, galKeys);
            Console.WriteLine("Rotated rows 4 steps right: ");
            decryptor.Decrypt(encryptedMatrix, plainResult);
            crtbuilder.Decompose(plainResult, podResult);
            PrintMatrix(podResult);
            Console.WriteLine("Noise budget after rotation: {0} bits",
                decryptor.InvariantNoiseBudget(encryptedMatrix));

            /*
            The output is as expected. Note how the noise budget gets a big hit in the
            first rotation, but remains almost unchanged in the next rotations. This is 
            again the same phenomenon that occurs with relinearization, where the noise 
            budget is consumed down to some bound determined by the decomposition bit count 
            and the encryption parameters. For example, after some multiplications have 
            been performed, rotations might practically for free (noise budget-wise), but
            might be relatively expensive when the noise budget is nearly full, unless
            a small decomposition bit count is used, which again is computationally costly.
            */

            /*
            Run garbage collection to help the global memory pool.
            */
            GC.Collect();
        }
    }
}
