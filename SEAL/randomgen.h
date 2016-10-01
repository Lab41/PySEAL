#pragma once

#include <cstdint>

namespace seal
{
    /**
    Provides the base-class for a uniform random number generator. Instances of this class are typically returned from the
    UniformRandomGeneratorFactory class. This class is meant for users to sub-class to implement their own random number
    generators. The implementation should provide a uniform random unsigned 32-bit value for each call to generate(). Note
    that the library will never make concurrent calls to generate() to the same instance (but individual instances of the
    same class may have concurrent calls). The uniformity and unpredictability of the numbers generated is essential for
    making a secure cryptographic system.
    
    @see UniformRandomGeneratorFactory for the base-class of a factory class that generates UniformRandomGenerator
    instances.
    @see StandardRandomAdapter for an implementation of UniformRandomGenerator to support the C++ standard library's random
    number generators.
    */
    class UniformRandomGenerator
    {
    public:
        /**
        Generates a new uniform unsigned 32-bit random number. Note that the implementation does not need to be thread-safe.
        */
        virtual std::uint32_t generate() = 0;

        /**
        Destroys the random number generator.
        */
        virtual ~UniformRandomGenerator()
        {
        }
    };

    /**
    Provides an implementation of UniformRandomGenerator for the standard C++ library's uniform random number generators.
    @tparam RNG specifies the type of the standard C++ library's random number generator (e.g., std::default_random_engine)
    */
    template <typename RNG>
    class StandardRandomAdapter : public UniformRandomGenerator
    {
    public:
        /**
        Creates a new random number generator (of type RNG).
        */
        StandardRandomAdapter()
        {
        }

        /**
        Returns a reference to the random number generator.
        */
        const RNG &generator() const
        {
            return generator_;
        }

        /**
        Returns a reference to the random number generator.
        */
        RNG &generator()
        {
            return generator_;
        }

        /**
        Generates a new uniform unsigned 32-bit random number.
        */
        virtual std::uint32_t generate() override
        {
            if (RNG::min() == 0 && RNG::max() >= UINT32_MAX)
            {
                return static_cast<std::uint32_t>(generator_());
            }
            if (RNG::max() - RNG::min() >= UINT32_MAX)
            {
                return static_cast<std::uint32_t>(generator_() - RNG::min());
            }
            if (RNG::min() == 0)
            {
                std::uint64_t max_value = RNG::max();
                std::uint64_t value = static_cast<std::uint64_t>(generator_());
                std::uint64_t max = max_value;
                while (max < UINT32_MAX)
                {
                    value *= max_value;
                    max *= max_value;
                    value += static_cast<std::uint64_t>(generator_());
                }
                return static_cast<std::uint32_t>(value);
            }
            std::uint64_t max_value = RNG::max() - RNG::min();
            std::uint64_t value = static_cast<std::uint64_t>(generator_() - RNG::min());
            std::uint64_t max = max_value;
            while (max < UINT32_MAX)
            {
                value *= max_value;
                max *= max_value;
                value += static_cast<std::uint64_t>(generator_() - RNG::min());
            }
            return static_cast<std::uint32_t>(value);
        }

    private:
        RNG generator_;
    };

    /**
    Provides the base-class for a factory instance that creates instances of UniformRandomGenerator. This class is meant for
    users to sub-class to implement their own random number generators. Note that each instance returned may be used concurrently
    across separate threads, but each individual instance does not need to be thread-safe.
    @see UniformRandomGenerator for details relating to the random number generator instances.
    @see StandardRandomAdapterFactory for an implementation of UniformRandomGeneratorFactory that supports the standard C++
    library's random number generators.
    */
    class UniformRandomGeneratorFactory
    {
    public:
        /**
        Creates a new uniform random number generator. The caller of create needs to ensure the returned instance is destroyed
        once it is no longer in-use to prevent a memory leak.
        */
        virtual UniformRandomGenerator *create() = 0;

        /**
        Destroys the random number generator factory.
        */
        virtual ~UniformRandomGeneratorFactory()
        {
        }

        /**
        Returns the default random number generator factory. This instance should not be destroyed.
        */
        static UniformRandomGeneratorFactory *default_factory()
        {
            return default_factory_;
        }

    private:
        static UniformRandomGeneratorFactory *default_factory_;
    };

    /**
    Provides an implementation of UniformRandomGeneratorFactory for the standard C++ library's random number generators.
    @tparam RNG specifies the type of the standard C++ library's random number generator (e.g., std::default_random_engine)
    */
    template <typename RNG>
    class StandardRandomAdapterFactory : public UniformRandomGeneratorFactory
    {
    public:
        /**
        Creates a new uniform random number generator.
        */
        UniformRandomGenerator *create() override
        {
            return new StandardRandomAdapter<RNG>();
        }
    };
}