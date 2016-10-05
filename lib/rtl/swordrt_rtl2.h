//===-- swordrt_rtl.h ----------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is a part of Archer/SwordRT, an OpenMP race detector.
//===----------------------------------------------------------------------===//

#ifndef SWORDRT_RTL_H
#define SWORDRT_RTL_H

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_VECTOR_SIZE 40
#include <boost/bloom_filter/basic_bloom_filter.hpp>
#include <boost/bloom_filter/counting_bloom_filter.hpp>
#include <boost/bloom_filter/detail/exceptions.hpp>
#include <boost/bloom_filter/hash/murmurhash3.hpp>
#include <boost/cstdint.hpp>
#include <boost/mpl/vector.hpp>

#include <omp.h>
#include <ompt.h>
#include <stdio.h>

#include <cstdint>
#include <iostream>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <unordered_map>

#define SWORDRT_DEBUG 1

std::mutex pmtx;
#ifdef SWORDRT_DEBUG
#define ASSERT(x) assert(x);
#define DEBUG(stream, x) 										\
		do {													\
			std::unique_lock<std::mutex> lock(pmtx);		\
			stream << "DEBUG INFO[" << x << "][" << __FUNCTION__ << ":" << __FILE__ << ":" << std::dec << __LINE__ << "]" << std::endl;	\
		} while(0)
#else
#define ASSERT(x)
#define DEBUG(stream, x)
#endif

#define ALWAYS_INLINE			__attribute__((always_inline))
#define CALLERPC 				((size_t) __builtin_return_address(0))

#define NUM_OF_REPORTED_RACES		10000
#define NUM_OF_ITEMS				1000000
#define TID_NUM_OF_BITS				8

typedef struct ThreadInfo {
	size_t *stack;
	size_t stacksize;
	int __swordomp_status__;
	uint8_t __swordomp_is_critical__;
} ThreadInfo;

enum AccessSize {
	size1 = 0,
	size2,
	size4,
	size8,
	size16
};

enum AccessType {
	none = 0,
	unsafe_read,
	unsafe_write,
	atomic_read,
	atomic_write,
	mutex_read,
	mutex_write,
	nutex_read,
	nutex_write
};

#define DECLARE_FILTER(name) boost::bloom_filters::counting_bloom_filter<size_t, NUM_OF_ITEMS, TID_NUM_OF_BITS, hash_function> filter_##name
#define CONTAINS(access, name, hash_values) filter_##name.contains(access, tid, hash_values)
#define CONTAINS_HASH(hash_values, access, name) filter_##name.contains(hash_values, access, tid)
#define INSERT_HASH(hash_values, access, name) filter_##name.insert(hash_values, access, tid)
#define CLEAR_FILTER(name) filter_##name.clear()
// More parallels r/w use parallel id?
// Nutex: use nutex name

static ompt_get_thread_id_t ompt_get_thread_id;

#define TLS
//#define NOTLS
#ifdef TLS
thread_local uint64_t tid = 0;
thread_local size_t *stack;
thread_local size_t stacksize;
thread_local int __swordomp_status__ = 0;
thread_local uint8_t __swordomp_is_critical__ = false;
#elif NOTLS
extern thread_local uint64_t tid;
extern thread_local size_t *stack;
extern thread_local size_t stacksize;
extern thread_local int __swordomp_status__;
extern thread_local uint8_t __swordomp_is_critical__;
#else
#define MAX_THREADS 256
thread_local uint64_t tid;
ThreadInfo threadInfo[MAX_THREADS];
#endif

// n = 1,000,000, p = 1.0E-10 (1 in 10,000,000,000) → m = 47,925,292 (5.71MB), k = 33
#define MURMUR3
#ifdef MURMUR3
typedef boost::mpl::vector<
		boost::bloom_filters::murmurhash3<size_t, 137> // 1
//		boost::bloom_filters::murmurhash3<size_t, 3>, // 2
//		boost::bloom_filters::murmurhash3<size_t, 5>, // 3
//		boost::bloom_filters::murmurhash3<size_t, 7>, // 4
//		boost::bloom_filters::murmurhash3<size_t, 11>, // 5
//		boost::bloom_filters::murmurhash3<size_t, 13>, // 6
//		boost::bloom_filters::murmurhash3<size_t, 17>, // 7
//		boost::bloom_filters::murmurhash3<size_t, 19>, // 8
//		boost::bloom_filters::murmurhash3<size_t, 23>, // 9
//		boost::bloom_filters::murmurhash3<size_t, 29>, // 10
//		boost::bloom_filters::murmurhash3<size_t, 31>, // 11
//		boost::bloom_filters::murmurhash3<size_t, 37>, // 12
//		boost::bloom_filters::murmurhash3<size_t, 41>, // 13
//		boost::bloom_filters::murmurhash3<size_t, 43>, // 14
//		boost::bloom_filters::murmurhash3<size_t, 47>, // 15
//		boost::bloom_filters::murmurhash3<size_t, 53>, // 16
//		boost::bloom_filters::murmurhash3<size_t, 59>, // 17
//		boost::bloom_filters::murmurhash3<size_t, 61>, // 18
//		boost::bloom_filters::murmurhash3<size_t, 67>, // 19
//		boost::bloom_filters::murmurhash3<size_t, 71>, // 20
//		boost::bloom_filters::murmurhash3<size_t, 73>, // 21
//		boost::bloom_filters::murmurhash3<size_t, 79>, // 22
//		boost::bloom_filters::murmurhash3<size_t, 83>, // 23
//		boost::bloom_filters::murmurhash3<size_t, 89>, // 24
//		boost::bloom_filters::murmurhash3<size_t, 97>, // 25
//		boost::bloom_filters::murmurhash3<size_t, 101>, // 26
//		boost::bloom_filters::murmurhash3<size_t, 103>, // 27
//		boost::bloom_filters::murmurhash3<size_t, 107>, // 28
//		boost::bloom_filters::murmurhash3<size_t, 109>, // 29
//		boost::bloom_filters::murmurhash3<size_t, 113>, // 30
//		boost::bloom_filters::murmurhash3<size_t, 127>, // 31
//		boost::bloom_filters::murmurhash3<size_t, 131>, // 32
//		boost::bloom_filters::murmurhash3<size_t, 137>  // 33
> hash_function;
#else
typedef boost::mpl::vector<
		boost::bloom_filters::boost_hash<size_t, 2>, // 1
		boost::bloom_filters::boost_hash<size_t, 3>, // 2
		boost::bloom_filters::boost_hash<size_t, 5>, // 3
		boost::bloom_filters::boost_hash<size_t, 7>, // 4
		boost::bloom_filters::boost_hash<size_t, 11>, // 5
		boost::bloom_filters::boost_hash<size_t, 13>, // 6
		boost::bloom_filters::boost_hash<size_t, 17>, // 7
		boost::bloom_filters::boost_hash<size_t, 19>, // 8
		boost::bloom_filters::boost_hash<size_t, 23>, // 9
		boost::bloom_filters::boost_hash<size_t, 29>, // 10
		boost::bloom_filters::boost_hash<size_t, 31>, // 11
		boost::bloom_filters::boost_hash<size_t, 37>, // 12
		boost::bloom_filters::boost_hash<size_t, 41>, // 13
		boost::bloom_filters::boost_hash<size_t, 43>, // 14
		boost::bloom_filters::boost_hash<size_t, 47>, // 15
		boost::bloom_filters::boost_hash<size_t, 53>, // 16
		boost::bloom_filters::boost_hash<size_t, 59>, // 17
		boost::bloom_filters::boost_hash<size_t, 61>, // 18
		boost::bloom_filters::boost_hash<size_t, 67>, // 19
		boost::bloom_filters::boost_hash<size_t, 71>, // 20
		boost::bloom_filters::boost_hash<size_t, 73>, // 21
		boost::bloom_filters::boost_hash<size_t, 79>, // 22
		boost::bloom_filters::boost_hash<size_t, 83>, // 23
		boost::bloom_filters::boost_hash<size_t, 89>, // 24
		boost::bloom_filters::boost_hash<size_t, 97>, // 25
		boost::bloom_filters::boost_hash<size_t, 101>, // 26
		boost::bloom_filters::boost_hash<size_t, 103>, // 27
		boost::bloom_filters::boost_hash<size_t, 107>, // 28
		boost::bloom_filters::boost_hash<size_t, 109>, // 29
		boost::bloom_filters::boost_hash<size_t, 113>, // 30
		boost::bloom_filters::boost_hash<size_t, 127>, // 31
		boost::bloom_filters::boost_hash<size_t, 131>, // 32
		boost::bloom_filters::boost_hash<size_t, 137>  // 33
> hash_function;
#endif

#ifdef MURMUR3
typedef boost::mpl::vector<
		boost::bloom_filters::murmurhash3<size_t, 2> // 1
//		boost::bloom_filters::murmurhash3<size_t, 3>, // 2
//		boost::bloom_filters::murmurhash3<size_t, 5>, // 3
//		boost::bloom_filters::murmurhash3<size_t, 7>, // 4
//		boost::bloom_filters::murmurhash3<size_t, 11>, // 5
//		boost::bloom_filters::murmurhash3<size_t, 13>, // 6
//		boost::bloom_filters::murmurhash3<size_t, 17>, // 7
//		boost::bloom_filters::murmurhash3<size_t, 19>, // 8
//		boost::bloom_filters::murmurhash3<size_t, 23>, // 9
//		boost::bloom_filters::murmurhash3<size_t, 29>, // 10
//		boost::bloom_filters::murmurhash3<size_t, 31>, // 11
//		boost::bloom_filters::murmurhash3<size_t, 37>, // 12
//		boost::bloom_filters::murmurhash3<size_t, 41>, // 13
//		boost::bloom_filters::murmurhash3<size_t, 43>, // 14
//		boost::bloom_filters::murmurhash3<size_t, 47>, // 15
//		boost::bloom_filters::murmurhash3<size_t, 53>, // 16
//		boost::bloom_filters::murmurhash3<size_t, 59>, // 17
//		boost::bloom_filters::murmurhash3<size_t, 61>, // 18
//		boost::bloom_filters::murmurhash3<size_t, 67>, // 19
//		boost::bloom_filters::murmurhash3<size_t, 71>, // 20
//		boost::bloom_filters::murmurhash3<size_t, 73>, // 21
//		boost::bloom_filters::murmurhash3<size_t, 79>, // 22
//		boost::bloom_filters::murmurhash3<size_t, 83>, // 23
//		boost::bloom_filters::murmurhash3<size_t, 89>, // 24
//		boost::bloom_filters::murmurhash3<size_t, 97>, // 25
//		boost::bloom_filters::murmurhash3<size_t, 101>, // 26
//		boost::bloom_filters::murmurhash3<size_t, 103>, // 27
//		boost::bloom_filters::murmurhash3<size_t, 107>, // 28
//		boost::bloom_filters::murmurhash3<size_t, 109>, // 29
//		boost::bloom_filters::murmurhash3<size_t, 113>, // 30
//		boost::bloom_filters::murmurhash3<size_t, 127>, // 31
//		boost::bloom_filters::murmurhash3<size_t, 131>, // 32
//		boost::bloom_filters::murmurhash3<size_t, 137>  // 33
> hash_function_rc;
#else
typedef boost::mpl::vector<
		boost::bloom_filters::boost_hash<size_t, 2>, // 1
		boost::bloom_filters::boost_hash<size_t, 3>, // 2
		boost::bloom_filters::boost_hash<size_t, 5>, // 3
		boost::bloom_filters::boost_hash<size_t, 7>, // 4
		boost::bloom_filters::boost_hash<size_t, 11>, // 5
		boost::bloom_filters::boost_hash<size_t, 13>, // 6
		boost::bloom_filters::boost_hash<size_t, 17>, // 7
		boost::bloom_filters::boost_hash<size_t, 19>, // 8
		boost::bloom_filters::boost_hash<size_t, 23>, // 9
		boost::bloom_filters::boost_hash<size_t, 29>, // 10
		boost::bloom_filters::boost_hash<size_t, 31>, // 11
		boost::bloom_filters::boost_hash<size_t, 37>, // 12
		boost::bloom_filters::boost_hash<size_t, 41>, // 13
		boost::bloom_filters::boost_hash<size_t, 43>, // 14
		boost::bloom_filters::boost_hash<size_t, 47>, // 15
		boost::bloom_filters::boost_hash<size_t, 53>, // 16
		boost::bloom_filters::boost_hash<size_t, 59>, // 17
		boost::bloom_filters::boost_hash<size_t, 61>, // 18
		boost::bloom_filters::boost_hash<size_t, 67>, // 19
		boost::bloom_filters::boost_hash<size_t, 71>, // 20
		boost::bloom_filters::boost_hash<size_t, 73>, // 21
		boost::bloom_filters::boost_hash<size_t, 79>, // 22
		boost::bloom_filters::boost_hash<size_t, 83>, // 23
		boost::bloom_filters::boost_hash<size_t, 89>, // 24
		boost::bloom_filters::boost_hash<size_t, 97>, // 25
		boost::bloom_filters::boost_hash<size_t, 101>, // 26
		boost::bloom_filters::boost_hash<size_t, 103>, // 27
		boost::bloom_filters::boost_hash<size_t, 107>, // 28
		boost::bloom_filters::boost_hash<size_t, 109>, // 29
		boost::bloom_filters::boost_hash<size_t, 113>, // 30
		boost::bloom_filters::boost_hash<size_t, 127>, // 31
		boost::bloom_filters::boost_hash<size_t, 131>, // 32
		boost::bloom_filters::boost_hash<size_t, 137>  // 33
> hash_function_rc;
#endif

thread_local std::vector<size_t> hash_values(boost::mpl::size<hash_function>::value);
thread_local std::vector<size_t> hash_value_pc(boost::mpl::size<hash_function_rc>::value);

class SwordRT {

public:
	SwordRT();
	~SwordRT();

//	inline bool Contains(size_t access, const char *filter_type);
//	inline bool Contains(size_t access, const char *filter_type, std::vector<size_t>& hash_values);
//	inline bool Contains(std::vector<size_t>& hash_values, size_t access, const char *filter_type);
//	inline void Insert(size_t access, uint64_t tid, const char *filter_type);
//	inline void Insert(std::vector<size_t>& hash_values, size_t access, uint64_t tid, const char *filter_type);
	inline void CheckMemoryAccess(size_t access, size_t pc, AccessSize access_size, AccessType access_type, const char *nutex_name = "");
	inline void ReportRace(size_t access, size_t pc, uint64_t tid, AccessSize access_size, AccessType access_type, const char *nutex_name = "");
	void clear();

private:
	DECLARE_FILTER(unsafe_read);
	DECLARE_FILTER(unsafe_write);
	DECLARE_FILTER(mutex_read);
	DECLARE_FILTER(mutex_write);
	DECLARE_FILTER(atomic_read);
	DECLARE_FILTER(atomic_write);
	boost::bloom_filters::basic_bloom_filter<size_t, NUM_OF_REPORTED_RACES, hash_function_rc> reported_races;

};

#endif  // SWORDRT_RTL_H