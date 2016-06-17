#include <random>
#include <chrono>
#include "randomgen.h"

using namespace std;

namespace seal
{
    UniformRandomGeneratorFactory *UniformRandomGeneratorFactory::default_factory_ = new StandardRandomAdapterFactory<random_device>();
}
