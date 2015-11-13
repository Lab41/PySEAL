using System;
using Microsoft.Research.SEAL;

namespace SEALNETExamples
{
    class Program
    {
        static void Main()
        {
            // Execute example.
            BasicExample();

            // Wait for ENTER before closing screen.
            Console.WriteLine("Press ENTER to exit");
            Console.ReadLine();
        }

        static void BasicExample()
        {
            // Create encryption parameters.
            var parms = new EncryptionParameters
            {
                DecompositionBitCount = 32,
                NoiseStandardDeviation = 3.19,
                NoiseMaxDeviation = 35.06
            };
            parms.CoeffModulus.Set("7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
            parms.PlainModulus.Set(1 << 10);
            parms.PolyModulus.Set("1x^4096 + 1");
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
        }
    }
}
