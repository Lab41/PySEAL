#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include "seal/util/uintcore.h"
#include "seal/encryptionparams.h"
#include "seal/memorypoolhandle.h"

namespace seal
{
    /**
    Class to store a ciphertext element. The data for a ciphertext consists of two or more polynomials, 
    which are in SEAL stored in a CRT form with respect to the factors of the coefficient modulus. This
    data itself is not meant to be modified directly by the user, but is instead operated on by functions
    in the Evaluator class. The size of the backing array of a ciphertext depends on the encryption
    parameters and the size of the ciphertext (at least 2). If the degree of the poly_modulus encryption 
    parameter is N, and the number of primes in the coeff_modulus encryption parameter is K, then the 
    ciphertext backing array requires precisely 8*N*K*size bytes of memory. A ciphertext also carries
    with it the hash of its associated encryption parameters, which is used to check the validity of the
    ciphertext for homomorphic operations and decryption.

    @par Memory Management
    SEAL allocates the memory for a ciphertext by default from the global memory pool, but a user can
    alternatively specify a different memory pool to be used through the MemoryPoolHandle class. This
    can be important, as constructing or resizing several ciphertexts at once allocated in the global
    memory pool can quickly lead to contention and poor performance in multi-threaded applications.
    In addition to its size which denotes the number of polynomials, a ciphertext also has a capacity
    which denotes the number of polynomials that fit in the current allocation. In high-performance
    applications unnecessary re-allocations should also be avoided by reserving enough memory for the
    ciphertext to begin with either by providing the capacity to the constructor as an extra argument,
    or by calling the reserve function at any time.

    @par Aliased Ciphertexts
    By default a ciphertext manages its own memory. However, in some cases the user might want to e.g.
    pre-allocate a large continuous block of memory to be used by several ciphertexts. In this case 
    it is possible to create aliased ciphertexts by simply giving the constructor a pointer to the
    memory location, and the relevant size parameters, including the capacity. The allocation size of
    an aliased ciphertext cannot be changed with the reserve function, unless it is first reallocated
    in a memory pool using the unalias function.

    @par Thread Safety
    In general, reading from ciphertext is thread-safe as long as no other thread is concurrently
    mutating it. This is due to the underlying data structure storing the ciphertext not being
    thread-safe.

    @see Plaintext for the class that stores plaintexts.
    */
    class Ciphertext
    {
    public:
        /**
        Constructs an empty ciphertext allocating no memory.
        */
        Ciphertext() = default;

        /**
        Constructs an empty ciphertext allocating no memory. The memory pool is set to the pool pointed 
        to by the given MemoryPoolHandle.

        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if pool is uninitialized
        */
        Ciphertext(const MemoryPoolHandle &pool) :
            pool_(pool)
        {
            if (!pool)
            {
                throw std::invalid_argument("pool is uninitialized");
            }
        }

        /**
        Constructs an empty ciphertext with capacity 2. In addition to the capacity, the allocation 
        size is determined by the given encryption parameters. The memory pool is set to the pool pointed 
        to by the given MemoryPoolHandle, or the global memory pool by default.

        @param[in] parms Encryption parameters determining the size characteristics of the ciphertext
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if pool is uninitialized
        */
        explicit Ciphertext(const EncryptionParameters &parms, const MemoryPoolHandle &pool = MemoryPoolHandle::Global())
        {
            // Allocate memory but don't resize
            reserve(parms, 2, pool);
        }

        /**
        Constructs an empty ciphertext with given capacity. In addition to the capacity, the allocation
        size is determined by the given encryption parameters. The memory pool is set to the pool pointed 
        to by the given MemoryPoolHandle, or the global memory pool by default.

        @param[in] parms Encryption parameters determining the size characteristics of the ciphertext
        @param[in] size_capacity The capacity
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if size_capacity is less than 2
        @throws std::invalid_argument if pool is uninitialized
        */
        explicit Ciphertext(const EncryptionParameters &parms, int size_capacity, const MemoryPoolHandle &pool = MemoryPoolHandle::Global())
        {
            // Allocate memory but don't resize
            reserve(parms, size_capacity, pool);
        }

        /**
        Constructs an aliased ciphertext with backing array located at the given address and with given 
        size. The capacity is set to equal the size. In addition to the capacity, the allocation size is
        determined by the given encryption parameters.

        @param[in] parms Encryption parameters determining the size characteristics of the ciphertext
        @param[in] size The size of the ciphertext
        @param[in] ciphertext_array A pointer to the backing array of the ciphertext
        @throws std::invalid_argument if size is less than 2
        @throws std::invalid_argument if ciphertext_array is null and the allocation size is positive
        */
        explicit Ciphertext(const EncryptionParameters &parms, int size, std::uint64_t *ciphertext_array)
        {
            alias(parms, size, ciphertext_array);
        }

        /**
        Constructs an aliased ciphertext with backing array located at the given address, with given 
        capacity, and with given size. In addition to the capacity, In addition to the size, the
        allocation size is determined by the given encryption parameters.

        @param[in] parms Encryption parameters determining the size characteristics of the ciphertext
        @param[in] size_capacity The capacity
        @param[in] size The size of the ciphertext
        @param[in] ciphertext_array A pointer to the backing array of the ciphertext
        @throws std::invalid_argument if size_capacity is less than 2 or if size_capacity is less than size
        @throws std::invalid_argument if ciphertext_array is null and the allocation size is positive
        */
        explicit Ciphertext(const EncryptionParameters &parms, int size_capacity, int size, std::uint64_t *ciphertext_array)
        {
            alias(parms, size_capacity, size, ciphertext_array);
        }

        /**
        Constructs a new ciphertext by copying a given one. If copy carries an initialized memory pool,
        the allocation for the new ciphertext is made from that pool. Otherwise the global memory pool
        is used instead.

        @param[in] copy The ciphertext to copy from
        */
        Ciphertext(const Ciphertext &copy) :
            pool_(copy.pool_ ? copy.pool_ : MemoryPoolHandle::Global()),
            hash_block_(copy.hash_block_),
            size_capacity_(copy.size_capacity_),
            size_(copy.size_),
            poly_coeff_count_(copy.poly_coeff_count_),
            coeff_mod_count_(copy.coeff_mod_count_),

            // pool_ is guaranteed to be good at this point so allocate memory
            ciphertext_array_(util::allocate_uint(size_capacity_ * poly_coeff_count_ * coeff_mod_count_, pool_))
        {
            // Copy over value
            util::set_uint_uint(copy.ciphertext_array_.get(), size_ * poly_coeff_count_ * coeff_mod_count_,
                ciphertext_array_.get());
        }

        /**
        Creates a new ciphertext by moving a given one.

        @param[in] source The ciphertext to move from
        */
        Ciphertext(Ciphertext &&source) = default;

        /**
        Changes the ciphertext to be an aliased ciphertext with backing array located at the given 
        address and with given size. The capacity is set to equal the size. In addition to the capacity, 
        the allocation size is determined by the given encryption parameters. Any previous allocation is
        released back to the memory pool (unless aliased).

        @param[in] parms Encryption parameters determining the size characteristics of the ciphertext
        @param[in] size The size of the ciphertext
        @param[in] ciphertext_array A pointer to the backing array of the ciphertext
        @throws std::invalid_argument if size is less than 2
        @throws std::invalid_argument if ciphertext_array is null and the allocation size is positive
        */
        inline void alias(const EncryptionParameters &parms, int size, std::uint64_t *ciphertext_array)
        {
            alias(parms, size, size, ciphertext_array);
        }

        /**
        Constructs an aliased ciphertext with backing array located at the given address, with given
        capacity, and with given size. In addition to the capacity, In addition to the size, the
        allocation size is determined by the given encryption parameters.

        Changes the ciphertext to be an aliased ciphertext with backing array located at the given
        address, with given capacity, and with given size. In addition to the capacity, the allocation 
        size is determined by the given encryption parameters. Any previous allocation is released 
        back to the memory pool (unless aliased).

        @param[in] parms Encryption parameters determining the size characteristics of the ciphertext
        @param[in] size_capacity The capacity
        @param[in] size The size of the ciphertext
        @param[in] ciphertext_array A pointer to the backing array of the ciphertext
        @throws std::invalid_argument if size_capacity is less than 2 or if size_capacity is less than size
        @throws std::invalid_argument if ciphertext_array is null and the allocation size is positive
        */
        inline void alias(const EncryptionParameters &parms, int size_capacity, int size, std::uint64_t *ciphertext_array)
        {
            if (ciphertext_array == nullptr && parms.coeff_modulus().size() != 0)
            {
                throw std::invalid_argument("ciphertext_array cannot be null");
            }
            if (size < 2)
            {
                throw std::invalid_argument("size must be at least 2");
            }
            if (size_capacity < size)
            {
                throw std::invalid_argument("size_capacity cannot be smaller than size");
            }

            // Need to set hash block first
            hash_block_ = parms.hash_block();

            size_capacity_ = size_capacity;
            size_ = size;

            // This is guaranteed to be at least 1 (EncryptionParameters::set_poly_modulus)
            // if set_poly_modulus has been called
            poly_coeff_count_ = parms.poly_modulus().coeff_count();

            // This can be 0 (EncryptionParameters::set_coeff_modulus)
            coeff_mod_count_ = parms.coeff_modulus().size();

            ciphertext_array_ = util::Pointer::Aliasing(ciphertext_array);
        }

        /**
        Reallocates an aliased ciphertext from the memory pool pointed to by the given 
        MemoryPoolHandle. An aliased ciphertext is no longer aliased after this function is called.

        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if the ciphertext is not aliased
        @throws std::invalid_argument if pool is uninitialized
        */
        void unalias(const MemoryPoolHandle &pool);

        /**
        Reallocates an aliased ciphertext from the memory pool pointed to by the currently held
        MemoryPoolHandle. If the currently held MemoryPoolHandle is uninitialized, it is set to 
        point to the global memory pool instead, and the memory is allocated from the global
        memory pool. An aliased ciphertext is no longer aliased after this function is called.

        @throws std::invalid_argument if the ciphertext is not aliased
        */
        inline void unalias()
        {
            if (!pool_)
            {
                pool_ = MemoryPoolHandle::Global();
            }
            unalias(pool_);
        }

        /**
        Allocates enough memory to accommodate the backing array of a ciphertext with given
        capacity. In addition to the capacity, the allocation size is determined by the given 
        encryption parameters, and is made from the memory pool pointed to by the given 
        MemoryPoolHandle.

        @param[in] parms Encryption parameters determining the size characteristics of the ciphertext
        @param[in] size_capacity The capacity
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if size_capacity is less than 2
        @throws std::invalid_argument if pool is uninitialized    
        @throws std::logic_error if the ciphertext is aliased
        */
        inline void reserve(const EncryptionParameters &parms, int size_capacity, const MemoryPoolHandle &pool)
        {
            // Need to set hash block first
            hash_block_ = parms.hash_block();

            reserve(size_capacity, parms.poly_modulus().coeff_count(), parms.coeff_modulus().size(), pool);
        }

        /**
        Allocates enough memory to accommodate the backing array of a ciphertext with given
        capacity. In addition to the capacity, the allocation size is determined by the given
        encryption parameters, and is made from the memory pool pointed to by the currently held
        MemoryPoolHandle. If the currently held MemoryPoolHandle is uninitialized, it is set to 
        point to the global memory pool instead, and the memory is allocated from the global
        memory pool.

        @param[in] parms Encryption parameters determining the size characteristics of the ciphertext
        @param[in] size_capacity The capacity
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if size_capacity is less than 2
        @throws std::invalid_argument if pool is uninitialized
        @throws std::logic_error if the ciphertext is aliased
        */
        inline void reserve(const EncryptionParameters &parms, int size_capacity)
        {
            if (!pool_)
            {
                pool_ = MemoryPoolHandle::Global();
            }
            reserve(parms, size_capacity, pool_);
        }

        /**
        Allocates enough memory to accommodate the backing array of a ciphertext with given
        capacity. In addition to the capacity, the allocation size is determined by the current
        encryption parameters, and is made from the memory pool pointed to by the given
        MemoryPoolHandle.

        @param[in] size_capacity The capacity
        @param[in] pool The MemoryPoolHandle pointing to a valid memory pool
        @throws std::invalid_argument if size_capacity is less than 2
        @throws std::invalid_argument if pool is uninitialized
        @throws std::logic_error if the ciphertext is aliased
        */
        inline void reserve(int size_capacity, const MemoryPoolHandle &pool)
        {
            reserve(size_capacity, poly_coeff_count_, coeff_mod_count_, pool);
        }

        /**
        Allocates enough memory to accommodate the backing array of a ciphertext with given
        capacity. In addition to the capacity, the allocation size is determined by the current
        encryption parameters, and is made from the memory pool pointed to by the currently held
        MemoryPoolHandle. If the currently held MemoryPoolHandle is uninitialized, it is set to
        point to the global memory pool instead, and the memory is allocated from the global
        memory pool.

        @param[in] size_capacity The capacity
        @throws std::invalid_argument if size_capacity is less than 2
        @throws std::logic_error if the ciphertext is aliased
        */
        inline void reserve(int size_capacity)
        {
            if (!pool_)
            {
                pool_ = MemoryPoolHandle::Global();
            }
            reserve(size_capacity, pool_);
        }

        /**
        Resets the ciphertext. This function releases any memory allocated by the ciphertext,
        returning it to the memory pool pointed to by the current MemoryPoolHandle, if the ciphertext
        is not aliased. It also sets all encryption parameter specific size information to zero.
        */
        inline void release()
        {
            // C++11 compatibility
            hash_block_ = { { 0 } };
            size_capacity_ = 2;
            size_ = 2;
            poly_coeff_count_ = 0;
            coeff_mod_count_ = 0;
            ciphertext_array_.release();
        }

        /**
        Copies a given ciphertext to the current one.

        @param[in] assign The ciphertext to copy from
        */
        Ciphertext &operator =(const Ciphertext &assign);

        /**
        Moves a given ciphertext to the current one.

        @param[in] assign The ciphertext to move from
        */
        Ciphertext &operator =(Ciphertext &&assign) = default;

        /**
        Returns a constant pointer to the beginning of the ciphertext data.
        */
        inline const std::uint64_t *pointer() const
        {
            return ciphertext_array_.get();
        }

        /**
        Returns a constant pointer to a particular polynomial in the ciphertext data. Note that
        SEAL stores each polynomial in the ciphertext modulo all of the K primes in the coefficient 
        modulus. The pointer returned by this function is to the beginning (constant coefficient) 
        of the first one of these K polynomials.

        @param[in] poly_index The index of the polynomial in the ciphertext
        @throws std::out_of_range if poly_index is less than 0 or bigger than the size of the ciphertext
        */
        inline const std::uint64_t *pointer(int poly_index) const
        {
            int poly_uint64_count = poly_coeff_count_ * coeff_mod_count_;
            if (poly_uint64_count == 0)
            {
                return nullptr;
            }
            if (poly_index < 0 || poly_index >= size_)
            {
                throw std::out_of_range("poly_index must be within [0, size)");
            }
            return ciphertext_array_.get() + poly_index * poly_uint64_count;
        }

        /**
        Returns a constant reference to a polynomial coefficient at a particular index in the
        ciphertext data. If the polynomial modulus has degree N, and the number of primes in
        the coefficient modulus is K, then the ciphertext contains size*N*K coefficients.
        Thus, the coeff_index has a range of [0, size*N*K).

        @param[in] coeff_index The index of the coefficient
        @throws std::out_of_range if coeff_index is less than 0 or too large
        */
        inline const std::uint64_t &operator[](int coeff_index) const
        {
            if (coeff_index < 0 || coeff_index >= size_ * poly_coeff_count_ * coeff_mod_count_)
            {
                throw std::out_of_range("coeff_index out of range");
            }
            return ciphertext_array_[coeff_index];
        }

        /**
        Returns whether the current ciphertext is an alias.
        */
        inline bool is_alias() const
        {
            return ciphertext_array_.is_alias();
        }

        /**
        Returns the number of primes in the coefficient modulus of the associated encryption 
        parameters. This directly affects the allocation size of the ciphertext.
        */
        inline int coeff_mod_count() const
        {
            return coeff_mod_count_;
        }

        /**
        Returns the coefficient count of the polynomial modulus of the associated encryption
        parameters. This directly affects the allocation size of the ciphertext.
        */
        inline int poly_coeff_count() const
        {
            return poly_coeff_count_;
        }

        /**
        Returns the capacity of the allocation. This means the largest size of the ciphertext
        that can be stored in the current allocation with the current encryption parameters.
        */
        inline int size_capacity() const
        {
            return size_capacity_;
        }

        /**
        Returns the size of the ciphertext.
        */
        inline int size() const
        {
            return size_;
        }

        /**
        Returns the total size of the current allocation in 64-bit words.
        */
        inline int uint64_count_capacity() const
        {
            return size_capacity_ * poly_coeff_count_ * coeff_mod_count_;
        }

        /**
        Returns the total size of the current ciphertext in 64-bit words.
        */
        inline int uint64_count() const
        {
            return size_ * poly_coeff_count_ * coeff_mod_count_;
        }

        /**
        Saves the ciphertext to an output stream. The output is in binary format and not 
        human-readable. The output stream must have the "binary" flag set.

        @param[in] stream The stream to save the ciphertext to
        @see load() to load a saved ciphertext.
        */
        void save(std::ostream &stream) const;
        void python_save(std::string &path) const;
        /**
        Loads a ciphertext from an input stream overwriting the current ciphertext.

        @param[in] stream The stream to load the ciphertext from
        @see save() to save a ciphertext.
        */
        void load(std::istream &stream);
        void python_load(std::string &path);
        /**
        Returns a constant reference to the hash block.

        @see EncryptionParameters for more information about the hash block.
        */
        inline const EncryptionParameters::hash_block_type &hash_block() const
        {
            return hash_block_;
        }

        /**
        Enables access to private members of seal::Ciphertext for .NET wrapper.
        */
        struct CiphertextPrivateHelper;

    private:
        void reserve(int size_capacity, int poly_coeff_count, int coeff_mod_count,
            const MemoryPoolHandle &pool);

        void resize(int size, int poly_coeff_count, int coeff_mod_count, const MemoryPoolHandle &pool);

        inline void resize(int size, int poly_coeff_count, int coeff_mod_count)
        {
            if (!is_alias() && !pool_)
            {
                pool_ = MemoryPoolHandle::Global();
            }
            resize(size, poly_coeff_count, coeff_mod_count, pool_);
        }

        inline void resize(const EncryptionParameters &parms, int size, const MemoryPoolHandle &pool)
        {
            // Need to set hash block first
            hash_block_ = parms.hash_block();

            resize(size, parms.poly_modulus().coeff_count(), parms.coeff_modulus().size(), pool);
        }

        inline void resize(const EncryptionParameters &parms, int size)
        {
            // Set the pool if not an alias and there is no pool set yet
            if (!is_alias() && !pool_)
            {
                pool_ = MemoryPoolHandle::Global();
            }
            resize(parms, size, pool_);
        }

        inline void resize(int size, const MemoryPoolHandle &pool)
        {
            resize(size, poly_coeff_count_, coeff_mod_count_, pool);
        }

        inline void resize(int size)
        {
            if (!is_alias() && !pool_)
            {
                pool_ = MemoryPoolHandle::Global();
            }
            resize(size, poly_coeff_count_, coeff_mod_count_, pool_);
        }
#ifdef SEAL_EXPOSE_MUTABLE_CIPHERTEXT
    public:
#endif
        inline void set_zero(int poly_index)
        {
            if (poly_index < 0 || poly_index >= size_)
            {
                throw std::out_of_range("poly_index must be within [0, size)");
            }
            util::set_zero_uint(poly_coeff_count_ * coeff_mod_count_, ciphertext_array_.get() + poly_index * poly_coeff_count_ * coeff_mod_count_);
        }

        inline void set_zero()
        {
            util::set_zero_uint(size_ * poly_coeff_count_ * coeff_mod_count_, ciphertext_array_.get());
        }

        inline std::uint64_t *mutable_pointer()
        {
            return ciphertext_array_.get();
        }

        /**
        Returns a reference to the underlying BigPolyArray. The user should never have
        a reason to modify the ciphertext by hand.
        */
        inline std::uint64_t *mutable_pointer(int poly_index)
        {
            int poly_uint64_count = poly_coeff_count_ * coeff_mod_count_;
            if (poly_uint64_count == 0)
            {
                return nullptr;
            }
            if (poly_index < 0 || poly_index >= size_)
            {
                throw std::out_of_range("poly_index must be within [0, size)");
            }
            return ciphertext_array_.get() + poly_index * poly_uint64_count;
        }
#ifdef SEAL_EXPOSE_MUTABLE_CIPHERTEXT
    private:
#endif
#ifdef SEAL_EXPOSE_MUTABLE_HASH_BLOCK
    public:
#endif
        /**
        Returns a reference to the hash block. The user should normally never have
        a reason to modify the hash block by hand.

        @see EncryptionParameters for more information about the hash block.
        */
        inline EncryptionParameters::hash_block_type &mutable_hash_block()
        {
            return hash_block_;
        }
#ifdef SEAL_EXPOSE_MUTABLE_HASH_BLOCK
    private:
#endif
        MemoryPoolHandle pool_;

        // C++11 compatibility
        EncryptionParameters::hash_block_type hash_block_{ { 0 } };
        
        int size_capacity_ = 2;

        int size_ = 2;

        int poly_coeff_count_ = 0;

        int coeff_mod_count_ = 0;

        util::Pointer ciphertext_array_;

        friend class Decryptor;

        friend class Encryptor;

        friend class Evaluator;

        friend class KeyGenerator;
    };
}
