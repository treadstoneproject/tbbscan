// Copyright (c) 2013, Facebook, Inc.  All rights reserved.
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree. An additional grant
// of patent rights can be found in the PATENTS file in the same directory.
// Copyright (c) 2012 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// A database can be configured with a custom filter_policy object.
// This object is responsible for creating a small filter_policy from a set
// of keys.  These filter_policys are stored in utils and are consulted
// automatically by utils to decide whether or not to read some
// information from disk. In many cases, a filter_policy can cut down the
// number of disk seeks form a handful to a single disk seek per
// DB::Get() call.
//
// Most people will want to use the builtin bloom filter_policy support (see
// NewBloomfilter_policy() below).

#ifndef UTILS_FILTER_POLICY_H_
#define UTILS_FILTER_POLICY_H_

#include <string>
#include "tbbscan/data_structure/slice.hpp"

namespace utils {

class slice;

class filter_policy {
 public:
  virtual ~filter_policy();

  // Return the name of this policy.  Note that if the filter_policy encoding
  // changes in an incompatible way, the name returned by this method
  // must be changed.  Otherwise, old incompatible filter_policys may be
  // passed to methods of this type.
  virtual const char* name() const = 0;

  // keys[0,n-1] contains a list of keys (potentially with duplicates)
  // that are ordered according to the user supplied comparator.
  // Append a filter_policy that summarizes keys[0,n-1] to *dst.
  //
  // Warning: do not change the initial contents of *dst.  Instead,
  // append the newly constructed filter_policy to *dst.
  virtual void create_filter(const slice* keys, int n, std::string* dst)
      const = 0;

  // "filter_policy" contains the data appended by a preceding call to
  // create_filter_policy() on this class.  This method must return true if
  // the key was in the list of keys passed to create_filter_policy().
  // This method may return true or false if the key was not on the
  // list, but it should aim to return false with a high probability.
  virtual bool key_may_match(const slice& key, const slice& filter_policy) const = 0;
};

// Return a new filter_policy policy that uses a bloom filter with approximately
// the specified number of bits per key.  A good value for bits_per_key
// is 10, which yields a filter_policy with ~ 1% false positive rate.
//
// Callers must delete the result after any database that is using the
// result has been closed.
//
// Note: if you are using a custom comparator that ignores some parts
// of the keys being compared, you must not use NewBloomfilter_policy()
// and must provide your own filter_policy that also ignores the
// corresponding parts of the keys.  For example, if the comparator
// ignores trailing spaces, it would be incorrect to use a
// filter_policy (like new_bloom_filter_policy) that does not ignore
// trailing spaces in keys.
extern const filter_policy* newbloom_filter_policy(int bits_per_key);

}

#endif  // UTILS_FILTER_POLICY_H_
