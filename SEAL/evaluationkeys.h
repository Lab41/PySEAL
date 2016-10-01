#pragma once

#include <iostream>
#include <vector>
#include "bigpolyarray.h"

namespace seal 
{
    /**
    Class to store evaluation keys. Each instance stores a std::vector of objects of type std::pair<BigPolyArray, BigPolyArray>, 
    where the evaluation keys are stored. Each entry of the vector is called an evaluation key. The number of evaluation keys 
    to store can be specified in the constructor of EvaluationKeys. Each entry of the vector is the key associated with a 
    particular power of the secret key, and can be repeatedly used by the relinearization operation typically performed either 
    automatically or manually by the user after homomorphic multiplication. 
    
    A reference (either const or not) to the evaluation keys vector can be obtained using keys(). The evaluation keys can be cleared 
    (and the vector resized to 0) using clear(). The evaluation keys can be saved and loaded from a stream with the save() and 
    load() functions.

    @par Thread Safety
    In general, reading from EvaluationKeys is thread-safe as long as no other thread is concurrently mutating it. 
    This is due to the underlying data structure storing the evaluation keys not being thread-safe.

    @warning This class does not perform any sanity checks for the evaluation keys that it stores.
    */
    class EvaluationKeys 
    {
    public:
        /**
        Creates an empty set of evaluation keys.
        */
        EvaluationKeys()
        {
        }

        /**
        Creates an EvaluationKeys instance initialized with a given vector of evaluation keys.

        @param[in] keys The keys
        */
        EvaluationKeys(std::vector<std::pair<BigPolyArray, BigPolyArray> > keys) : keys_(keys)
        {
        }

        /**
        Returns a const reference to the evaluation key stored at the given index.

        @throws std::out_of_range If the given index is not within [0, size())
        */
        const std::pair<BigPolyArray, BigPolyArray> &operator[](int key_index) const;

        /**
        Returns a reference to the evaluation key stored at the given index.

        @throws std::out_of_range If the given index is not within [0, size())
        */
        std::pair<BigPolyArray, BigPolyArray> &operator[](int key_index);
        
        /**
        Returns the current number of evaluation keys.
        */
        std::size_t size() const
        {
            return keys_.size();
        }

        /**
        Saves the EvaluationKeys instance to an output stream. The output is in binary format and not human-readable. The output
        stream must have the "binary" flag set.

        @param[in] stream The stream to save the EvaluationKeys to
        @see load() to load a saved EvaluationKeys instance.
        */
        void save(std::ostream &stream) const;
        
        /**
        Loads an EvaluationKeys instance from an input stream overwriting the current EvaluationKeys instance.

        @param[in] stream The stream to load the EvaluationKeys instance from
        @see save() to save an EvaluationKeys instance.
        */
        void load(std::istream &stream);
        
        /**
        Returns a const reference to the vector of evaluation keys.
        */
        const std::vector<std::pair<BigPolyArray, BigPolyArray> > &keys() const
        {
            return keys_;
        }

        /**
        Returns a reference to the vector of evaluation keys.
        */
        std::vector<std::pair<BigPolyArray, BigPolyArray> > &keys()
        {
            return keys_;
        }

        /**
        Sets the vector of evaluation keys to be empty.
        */
        void clear();
    
    private:
        /**
        The vector of evaluation keys (each of type std::pair<BigPolyArray, BigPolyArray>). 
        Each key corresponds to one particular power of the secret key whose influence in 
        a ciphertext is removed using the relinearize() function of Evaluator.
        */
        std::vector<std::pair<BigPolyArray, BigPolyArray> > keys_;
    };
}