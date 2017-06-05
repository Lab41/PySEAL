#include "CppUnitTest.h"
#include "randomgen.h"
#include "util/randomtostd.h"
#include <cstdint>
#include <memory>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace seal::util;
using namespace seal;
using namespace std;

namespace SEALTest
{
    namespace util
    {
        TEST_CLASS(RandomToStandard)
        {
        public:
            TEST_METHOD(RandomToStandardGenerate)
            {
                unique_ptr<UniformRandomGenerator> generator(UniformRandomGeneratorFactory::default_factory()->create());
                RandomToStandardAdapter rand(generator.get());
                Assert::IsTrue(rand.generator() == generator.get());
                Assert::AreEqual(static_cast<uint32_t>(0), rand.min());
                Assert::AreEqual(static_cast<uint32_t>(UINT32_MAX), rand.max());
                bool lower_half = false;
                bool upper_half = false;
                bool even = false;
                bool odd = false;
                for (int i = 0; i < 10; i++)
                {
                    uint32_t value = rand();
                    if (value < UINT32_MAX / 2)
                    {
                        lower_half = true;
                    }
                    else
                    {
                        upper_half = true;
                    }
                    if ((value % 2) == 0)
                    {
                        even = true;
                    }
                    else
                    {
                        odd = true;
                    }
                }
                Assert::IsTrue(lower_half);
                Assert::IsTrue(upper_half);
                Assert::IsTrue(even);
                Assert::IsTrue(odd);
            }
        };
    }
}
