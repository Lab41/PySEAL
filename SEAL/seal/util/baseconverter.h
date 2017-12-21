#pragma once

#include <stdexcept>
#include "seal/util/mempool.h"
#include "seal/memorypoolhandle.h"
#include "seal/smallmodulus.h"
#include "seal/util/smallntt.h"
#include "seal/biguint.h"

namespace seal
{
    namespace util
    {
        class BaseConverter
        {
        public:
            BaseConverter(const MemoryPoolHandle &pool = MemoryPoolHandle::Global()) : 
                pool_(pool)
            {
            }

            /**
            The main constructor for the full RNS variant implementation. The constructor gets an instance of SEALContext as input and extracts required 
            parameters for precomputing tables.
            In order to perform any full RNS operation, an instance of this class should be instantiated first.
            Refer to  "A Full RNS Variant of FV like Somewhat Homomorphic Encryption Schemes" for more details.
            */
            BaseConverter(const std::vector<SmallModulus> &coeff_base, int coeff_count, int coeff_power, 
                const SmallModulus &small_plain_mod, const MemoryPoolHandle &pool = MemoryPoolHandle::Global());

            BaseConverter(const BaseConverter &copy) = default;

            BaseConverter(BaseConverter &&source) = default;

            BaseConverter &operator =(BaseConverter &&assign) = default;

            /**
            Fast base converter from q to Bsk
            */
            void fastbconv(const std::uint64_t *input, std::uint64_t *destination, const MemoryPoolHandle &pool) const;

            /**
            Fast base converter from Bsk to q
            */
            void fastbconv_sk(const std::uint64_t *input, std::uint64_t *destination, const MemoryPoolHandle &pool) const;

            /**
            Reduction from Bsk U {m_tilde} to Bsk
            */
            void mont_rq(const std::uint64_t *input, std::uint64_t *destination) const;

            /**
            Fast base converter from q U Bsk to Bsk
            */
            void fast_floor(const std::uint64_t *input, std::uint64_t *destination, const MemoryPoolHandle &pool) const;

            /**
            Fast base converter from q to Bsk U {m_tilde}
            */
            void fastbconv_mtilde(const std::uint64_t *input, std::uint64_t *destination, const MemoryPoolHandle &pool) const;

            /**
            Fast base converter from q to plain_modulus U {gamma}
            */
            void fastbconv_plain_gamma(const std::uint64_t *input, std::uint64_t *destination, const MemoryPoolHandle &pool) const;

            void reset();

            inline bool is_generated() const
            {
                return generated_;
            }

            inline int coeff_base_mod_count() const
            {
                return coeff_base_mod_count_;
            }

            inline int aux_base_mod_count() const
            {
                return aux_base_mod_count_;
            }

            inline const std::vector<std::uint64_t> &get_plain_gamma_product() const
            {
                return plain_gamma_product_mod_coeff_array_;
            }

            inline const std::vector<std::uint64_t> &get_neg_inv_coeff() const
            {
                return neg_inv_coeff_products_all_mod_plain_gamma_array_;
            }

            inline const std::vector<SmallModulus> &get_plain_gamma_array() const
            {
                return plain_gamma_array_;
            }

            inline std::uint64_t get_inv_gamma() const
            {
                return inv_gamma_mod_plain_;
            }
            
            inline const std::vector<util::SmallNTTTables> &get_bsk_small_ntt_table() const
            {
                return bsk_small_ntt_table_;
            }

            inline int bsk_base_mod_count() const
            {
                return bsk_base_mod_count_;
            }

            inline const std::vector<SmallModulus> &get_bsk_mod_array() const
            {
                return bsk_base_array_;
            }

            inline const std::vector<std::uint64_t> &get_inv_coeff_mod_coeff_array() const
            {
                return inv_coeff_base_products_mod_coeff_array_;
            }

        private:
            MemoryPoolHandle pool_;
            
            bool generated_ = false;
            
            int coeff_base_mod_count_ = 0;

            int aux_base_mod_count_ = 0;

            int bsk_base_mod_count_ = 0;

            int coeff_count_ = 0;

            int plain_gamma_count_ = 0;

            // Array of coefficient small moduli
            std::vector<SmallModulus> coeff_base_array_;

            // Array of auxiliary moduli 
            std::vector<SmallModulus> aux_base_array_;

            // Array of auxiliary U {m_sk_} moduli
            std::vector<SmallModulus> bsk_base_array_;

            // Array of plain modulus U gamma
            std::vector<SmallModulus> plain_gamma_array_;
            
            // Matrix which contains the products of coeff moduli mod aux
            std::vector<std::vector<std::uint64_t> > coeff_base_products_mod_aux_bsk_array_;

            // Array of inverse coeff modulus products mod each small coeff mods 
            std::vector<std::uint64_t> inv_coeff_base_products_mod_coeff_array_;

            // Array of coeff moduli products mod m_tilde
            std::vector<std::uint64_t> coeff_base_products_mod_mtilde_array_;

            // Array of coeff modulus products times m_tilda mod each coeff modulus 
            std::vector<std::uint64_t> mtilde_inv_coeff_base_products_mod_coeff_array_;
            
            // Matrix of the inversion of coeff modulus products mod each auxiliary mods
            std::vector<std::uint64_t> inv_coeff_products_all_mod_aux_bsk_array_;
            
            // Matrix of auxiliary mods products mod each coeff modulus 
            std::vector<std::vector<std::uint64_t> > aux_base_products_mod_coeff_array_;

            // Array of inverse auxiliary mod products mod each auxiliary mods 
            std::vector<std::uint64_t> inv_aux_base_products_mod_aux_array_;

            // Array of auxiliary bases products mod m_sk_
            std::vector<std::uint64_t> aux_base_products_mod_msk_array_;

            // Coeff moduli products inverse mod m_tilde 
            std::uint64_t inv_coeff_products_mod_mtilde_ = 0;

            // Auxiliary base products mod m_sk_  (m1*m2*...*ml)-1 mod m_sk
            std::uint64_t inv_aux_products_mod_msk_ = 0;
            
            // Gamma inverse mod plain modulus
            std::uint64_t inv_gamma_mod_plain_ = 0;
          
            // Auxiliary base products mod coeff moduli (m1*m2*...*ml) mod qi
            std::vector<std::uint64_t> aux_products_all_mod_coeff_array_;

            // Array of m_tilde inverse mod Bsk = m U {msk}
            std::vector<std::uint64_t> inv_mtilde_mod_bsk_array_;

            // Array of all coeff base products mod Bsk
            std::vector<std::uint64_t> coeff_products_all_mod_bsk_array_;

            // Matrix of coeff base product mod plain modulus and gamma
            std::vector<std::vector<std::uint64_t> > coeff_products_mod_plain_gamma_array_;

            // Array of negative inverse all coeff base product mod plain modulus and gamma
            std::vector<std::uint64_t> neg_inv_coeff_products_all_mod_plain_gamma_array_;

            // Array of plain_gamma_product mod coeff base moduli
            std::vector<std::uint64_t> plain_gamma_product_mod_coeff_array_;
            
            // Array of small NTT tables for moduli in Bsk
            std::vector<SmallNTTTables> bsk_small_ntt_table_;

            SmallModulus m_tilde_;

            SmallModulus m_sk_;

            SmallModulus small_plain_mod_;

            SmallModulus gamma_;
        };
    }
}
