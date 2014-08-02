#ifndef CONCURRENCY_BLOOMFILTER_CONCURRENCY_HPP
#define CONCURRENCY_BLOOMFILTER_CONCURRENCY_HPP

/*
* Copyright 2014 MTSec, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

/*  Titles																								Authors					 Date
 * -Bloom filter run on load balance tbb and bloom filter
 *                                                        R.Chatsiri       02/06/2014
 * -Bloom filter
 *                                                        R.Chatsiri
 */
#include "utils/hash.hpp"
#include "utils/md5_hash.hpp"
#include "tbbscan/data_structure/filter_policy.hpp"

namespace concurrency
{


		/* 
    static uint32_t bloom_hash(const utils::slice& key)
    {
    return utils::convert::md5_hash(key.data());
    }
    */
		
    static uint32_t bloom_hash(const utils::slice& key)
    {
        return utils::hash(key.data(), key.size(), 0xbc9f1d34);
    }

    class bloom_filter_policy : public utils::filter_policy
    {
        private:
            size_t bits_per_key_;
            size_t k_;
            uint32_t *hash_func_;

            void initialize() {
                // We intentionally round down to reduce probing cost a little bit
                k_ = static_cast<size_t>(bits_per_key_ * 0.69);  // 0.69 =~ ln(2)

                if (k_ < 1) k_ = 1;

                if (k_ > 30) k_ = 30;
            }

        public:
            /*
            explicit bloom_filter_policy(int bits_per_key,
            uint32_t (*hash_func)(const utils::slice& key))
            : bits_per_key_(bits_per_key), hash_func_(hash_func) {
            initialize();
            }
            */

            explicit bloom_filter_policy(int bits_per_key, const utils::slice& key)
                : bits_per_key_(bits_per_key) {
                *hash_func_ = bloom_hash(key);
                initialize();
            }

            explicit bloom_filter_policy(int bits_per_key)
                : bits_per_key_(bits_per_key) {
                //hash_func_ bloom_hash;
                initialize();
            }

            virtual const char *name() const {
                return "tbbscan.buildbloomfiltercuda";
            }

            virtual void create_filter(const utils::slice *keys, int n, std::string *dst) const;
            virtual bool key_may_match(const utils::slice& key, const utils::slice& bloom_filter) const;
    };

}


#endif /* CONCURRENCY_BLOOMFILTER_CONCURRENCY_HPP */
