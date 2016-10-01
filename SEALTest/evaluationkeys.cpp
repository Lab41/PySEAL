#include "CppUnitTest.h"
#include "evaluationkeys.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal;
using namespace std;

namespace SEALTest
{
    TEST_CLASS(FVEvk)
    {

    public:
        TEST_METHOD(SaveLoadEvaluationKeys)
        {
            /**
            Note: this test extracts size using size() function of vector and checks individual keys using the format keys()[i].first[j]. 
            See below TEST_METHOD for testing the operator[] and size() functions of EvaluationKeys.
            */

            stringstream stream;

            BigPolyArray arr1(3, 5, 10);
            arr1[0][0] = 1;
            arr1[0][1] = 2;
            arr1[0][2] = 3;
            arr1[1][0] = 4;
            arr1[1][1] = 5;
            arr1[2][0] = 6;

            BigPolyArray arr2(3, 5, 10);
            arr2[0][0] = 7;
            arr2[0][1] = 8;
            arr2[0][2] = 9;
            arr2[1][0] = 0;
            arr2[1][1] = 1;
            arr2[2][0] = 2;

            BigPolyArray arr3(3, 5, 10);
            arr3[0][0] = 3;
            arr3[0][1] = 4;
            arr3[0][2] = 5;
            arr3[1][0] = 6;
            arr3[1][1] = 7;
            arr3[2][0] = 8;

            BigPolyArray arr4(3, 5, 10);
            arr4[0][0] = 9;
            arr4[0][1] = 0;
            arr4[0][2] = 1;
            arr4[1][0] = 2;
            arr4[1][1] = 3;
            arr4[2][0] = 4;

            std::pair<BigPolyArray, BigPolyArray> test_keys_1 = std::pair<BigPolyArray, BigPolyArray>(arr1, arr2);
            std::pair<BigPolyArray, BigPolyArray> test_keys_2 = std::pair<BigPolyArray, BigPolyArray>(arr3, arr4);

           std::vector<std::pair<BigPolyArray, BigPolyArray> > test_keys_vector;
           test_keys_vector.emplace_back(test_keys_1);
           test_keys_vector.emplace_back(test_keys_2);
          
           EvaluationKeys test_evk(test_keys_vector);
           size_t expectedsize = 2;
           Assert::AreEqual(expectedsize, test_evk.keys().size());

           EvaluationKeys test_evk2;
           Assert::AreNotEqual(test_evk2.keys().size(), test_evk.keys().size());

           test_evk.save(stream);
           test_evk2.load(stream);

           Assert::AreEqual(test_evk2.keys().size(), test_evk.keys().size());

           Assert::IsTrue(test_evk2.keys()[0].first.size() == test_evk.keys()[0].first.size());
           Assert::IsTrue(test_evk2.keys()[0].second.size() == test_evk.keys()[0].second.size());
           Assert::IsTrue(test_evk2.keys()[1].first.size() == test_evk.keys()[1].first.size());
           Assert::IsTrue(test_evk2.keys()[1].second.size() == test_evk.keys()[1].second.size());

           Assert::IsTrue(test_evk2.keys()[0].first.coeff_count() == test_evk.keys()[0].first.coeff_count());
           Assert::IsTrue(test_evk2.keys()[0].second.coeff_count() == test_evk.keys()[0].second.coeff_count());
           Assert::IsTrue(test_evk2.keys()[1].first.coeff_count() == test_evk.keys()[1].first.coeff_count());
           Assert::IsTrue(test_evk2.keys()[1].second.coeff_count() == test_evk.keys()[1].second.coeff_count());

           Assert::IsTrue(test_evk2.keys()[0].first.coeff_bit_count() == test_evk.keys()[0].first.coeff_bit_count());
           Assert::IsTrue(test_evk2.keys()[0].second.coeff_bit_count() == test_evk.keys()[0].second.coeff_bit_count());
           Assert::IsTrue(test_evk2.keys()[1].first.coeff_bit_count() == test_evk.keys()[1].first.coeff_bit_count());
           Assert::IsTrue(test_evk2.keys()[1].second.coeff_bit_count() == test_evk.keys()[1].second.coeff_bit_count());

           Assert::IsTrue(test_evk2.keys()[0].first[0] == test_evk.keys()[0].first[0]);
           Assert::IsTrue(test_evk2.keys()[0].first[1] == test_evk.keys()[0].first[1]);
           Assert::IsTrue(test_evk2.keys()[0].first[2] == test_evk.keys()[0].first[2]);
           Assert::IsTrue(test_evk2.keys()[0].second[0] == test_evk.keys()[0].second[0]);
           Assert::IsTrue(test_evk2.keys()[0].second[1] == test_evk.keys()[0].second[1]);
           Assert::IsTrue(test_evk2.keys()[0].second[2] == test_evk.keys()[0].second[2]);

           Assert::IsTrue(test_evk2.keys()[1].first[0] == test_evk.keys()[1].first[0]);
           Assert::IsTrue(test_evk2.keys()[1].first[1] == test_evk.keys()[1].first[1]);
           Assert::IsTrue(test_evk2.keys()[1].first[2] == test_evk.keys()[1].first[2]);
           Assert::IsTrue(test_evk2.keys()[1].second[0] == test_evk.keys()[1].second[0]);
           Assert::IsTrue(test_evk2.keys()[1].second[1] == test_evk.keys()[1].second[1]);
           Assert::IsTrue(test_evk2.keys()[1].second[2] == test_evk.keys()[1].second[2]);

        }

        TEST_METHOD(EvalKeysAccessAndSize)
        {
            BigPolyArray arr1(3, 5, 10);
            arr1[0] = "3";
            arr1[1] = "1x^1";
            arr1[2] = "4x^2";

            BigPolyArray arr2(3, 5, 10);
            arr2[0] = "1";
            arr2[1] = "5";
            arr2[2] = "9";

            BigPolyArray arr3(3, 5, 10);
            arr3[0] = "2";
            arr3[1] = "6";
            arr3[2] = "5";

            BigPolyArray arr4(3, 5, 10);
            arr4[0] = "3";
            arr4[1] = "5";
            arr4[2] = "8x^2 + 9x^1 + 7";

            std::pair<BigPolyArray, BigPolyArray> test_keys_1 = std::pair<BigPolyArray, BigPolyArray>(arr1, arr2);
            std::pair<BigPolyArray, BigPolyArray> test_keys_2 = std::pair<BigPolyArray, BigPolyArray>(arr3, arr4);

            std::vector<std::pair<BigPolyArray, BigPolyArray> > test_keys_vector;
            test_keys_vector.emplace_back(test_keys_1);
            test_keys_vector.emplace_back(test_keys_2);

            EvaluationKeys test_evk(test_keys_vector);

            // test size()
            size_t expectedsize = 2;
            Assert::AreEqual(expectedsize, test_evk.size());

            // test operator[]
            Assert::IsTrue(test_evk[0].first[0].to_string() == "3");
            Assert::IsTrue(test_evk[0].first[1].to_string() == "1x^1");
            Assert::IsTrue(test_evk[0].first[2].to_string() == "4x^2");
            Assert::IsTrue(test_evk[0].second[0].to_string() == "1");
            Assert::IsTrue(test_evk[0].second[1].to_string() == "5");
            Assert::IsTrue(test_evk[0].second[2].to_string() == "9");
            Assert::IsTrue(test_evk[1].first[0].to_string() == "2");
            Assert::IsTrue(test_evk[1].first[1].to_string() == "6");
            Assert::IsTrue(test_evk[1].first[2].to_string() == "5");
            Assert::IsTrue(test_evk[1].second[0].to_string() == "3");
            Assert::IsTrue(test_evk[1].second[1].to_string() == "5");
            Assert::IsTrue(test_evk[1].second[2].to_string() == "8x^2 + 9x^1 + 7");

            // test clear()
            test_evk.clear();
            size_t newexpectedsize = 0;
            Assert::AreEqual(newexpectedsize, test_evk.size());
        }
    };
}