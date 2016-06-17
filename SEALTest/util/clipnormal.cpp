#include "CppUnitTest.h"
#include "randomgen.h"
#include "util/randomtostd.h"
#include "util/clipnormal.h"
#include <memory>
#include <cmath>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal::util;
using namespace seal;
using namespace std;

namespace SEALTest
{
    namespace util
    {
        TEST_CLASS(ClipNormal)
        {
        public:
            TEST_METHOD(ClipNormalGenerate)
            {
                unique_ptr<UniformRandomGenerator> generator(UniformRandomGeneratorFactory::default_factory()->create());
                RandomToStandardAdapter rand(generator.get());
                ClippedNormalDistribution dist(50.0, 10.0, 20.0);

                Assert::AreEqual(50.0, dist.mean());
                Assert::AreEqual(10.0, dist.standard_deviation());
                Assert::AreEqual(20.0, dist.max_deviation());
                Assert::AreEqual(30.0, dist.min());
                Assert::AreEqual(70.0, dist.max());
                double average = 0;
                double stddev = 0;
                for (int i = 0; i < 100; ++i)
                {
                    double value = dist(rand);
                    average += value;
                    stddev += (value - 50.0) * (value - 50.0);
                    Assert::IsTrue(value >= 30.0 && value <= 70.0);
                }
                average /= 100;
                stddev /= 100;
                stddev = sqrt(stddev);
                Assert::IsTrue(average >= 40.0 && average <= 60.0);
                Assert::IsTrue(stddev >= 5.0 && stddev <= 15.0);
            }
        };
    }
}
