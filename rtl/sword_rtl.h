//===-- sword_rtl.h ------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is a part of Sword/Sword, an OpenMP race detector.
//===----------------------------------------------------------------------===//

#ifndef SWORD_RTL_H
#define SWORD_RTL_H

#include "sword_common.h"

#include <fcntl.h>
#include <sys/stat.h>

#include <future>
#include <vector>

#define ALWAYS_INLINE			__attribute__((always_inline))
#define CALLERPC 				((size_t) __builtin_return_address(0))

struct ParallelData {
private:
	unsigned state;
	ompt_id_t parallel_id;
	unsigned level;
	std::string path;
	unsigned offset;
	unsigned span;

public:
	ParallelData() {
		state = 0;
		parallel_id = 0;
		level = 0;
		path = "";
		offset = 0;
		span = 0;
	}

	ParallelData(ompt_id_t pid, unsigned l, std::string p, unsigned o, unsigned s) {
		state = 0;
		parallel_id = pid;
		level = l;
		path = p;
		offset = o;
		span = s;
	}

	void setData(ParallelData *pd) {
		state = pd->getState();
		parallel_id = pd->getParallelID();
		level = pd->getParallelLevel();
		path = pd->getPath();
		offset = pd->getOffset();
		span = pd->getSpan();
	}

	void setData(ompt_id_t pid, unsigned l, std::string p, unsigned o, unsigned s) {
		state = 0;
		parallel_id = pid;
		level = l;
		path = p;
		offset = o;
		span = s;
	}

	void setState(unsigned v) {
		state = v;
	}

	void setPath(std::string p) {
		path = p;
	}

	void setParallelID(ompt_id_t pid) {
		parallel_id = pid;
	}

	void setParallelLevel(unsigned l) {
		level = l;
	}

	void setOffset(unsigned o) {
		offset = o;
	}

	void setSpan(unsigned s) {
		span = s;
	}

	unsigned getState() {
		return state;
	}

	ompt_id_t getParallelID() {
		return parallel_id;
	}

	unsigned getParallelLevel() {
		return level;
	}

	std::string getPath() {
		return path;
	}

	std::string getPath(int level) {
		switch(level) {
		case 0:
			return path;
		case -1:
			return path.substr(0, path.find_last_of("/"));
		default:
			return path;
		}
	}

	unsigned getOffset() {
		return offset;
	}

	unsigned getSpan() {
		return span;
	}
};

// Global Variables
std::atomic<ompt_id_t> current_parallel_idx(0);

// Thread Local Variables
thread_local unsigned char __LZO_MMODEL *out;
thread_local int tid = 0;
thread_local int __sword_status__ = 0;
thread_local TraceItem *accesses;
thread_local TraceItem *accesses1;
thread_local TraceItem *accesses2;
thread_local uint64_t idx = 0;
thread_local ompt_id_t parallel_idx = 0;
thread_local FILE *datafile = NULL;
thread_local char *buffer = NULL;
thread_local std::future<bool> fut;
thread_local size_t offset = 0;
thread_local ParallelData pdata;

#endif  // SWORD_RTL_H
