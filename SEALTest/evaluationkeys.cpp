#include "CppUnitTest.h"
#include "evaluationkeys.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal;
using namespace std;

namespace SEALTest
{
    TEST_CLASS(EvalKeys)
    {
    public:
        TEST_METHOD(EvaluationKeysTest)
        {
            EvaluationKeys keys;
            Assert::AreEqual(keys.count(), 0);

            keys.clear();
            Assert::AreEqual(keys.count(), 0);

            keys.resize(2);
            Assert::AreEqual(keys.count(), 2);
            keys[0].resize(2, 32);
            keys[0][0] = 1;
            keys[0][1] = 2;
            keys[1].resize(3, 32);
            keys[1][0] = 3;
            keys[1][1] = 4;
            keys[1][2] = 5;
            Assert::IsTrue("2x^1 + 1" == keys[0].to_string());
            Assert::IsTrue("5x^2 + 4x^1 + 3" == keys[1].to_string());

            EvaluationKeys keys2;
            Assert::AreEqual(keys2.count(), 0);
            keys2 = keys;
            Assert::AreEqual(keys.count(), 2);
            Assert::IsTrue("2x^1 + 1" == keys2[0].to_string());
            Assert::IsTrue("5x^2 + 4x^1 + 3" == keys2[1].to_string());
            keys2[1].set_zero();
            Assert::IsTrue("5x^2 + 4x^1 + 3" == keys[1].to_string());

            keys.resize(3);
            Assert::AreEqual(keys.count(), 3);
            Assert::IsTrue("2x^1 + 1" == keys[0].to_string());
            Assert::IsTrue("5x^2 + 4x^1 + 3" == keys[1].to_string());
            Assert::IsTrue(keys[2].is_zero());

            keys.resize(1);
            Assert::AreEqual(keys.count(), 1);
            Assert::IsTrue("2x^1 + 1" == keys[0].to_string());

            keys.clear();
            Assert::AreEqual(keys.count(), 0);
        }

        TEST_METHOD(SaveLoadEvaluationKeys)
        {
            stringstream stream;

            EvaluationKeys keys(3);
            keys[0].resize(3, 32);
            keys[0][0] = 1;
            keys[0][1] = 2;
            keys[0][2] = 3;
            keys[1].resize(2, 96);
            keys[1][0] = 4;
            keys[1][1] = 5;
            keys[2].resize(1, 24);
            keys[2][0] = 6;

            EvaluationKeys keys2;
            keys.save(stream);
            keys2.load(stream);

            Assert::AreEqual(3, keys2.count());
            Assert::IsTrue(keys[0] == keys2[0]);
            Assert::IsTrue(keys[1] == keys2[1]);
            Assert::IsTrue(keys[2] == keys2[2]);
        }
    };
}
