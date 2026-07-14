/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "BitSet.h"
#include "BitUtil.h"

namespace Lucene {

BitSet::BitSet(uint32_t size) : bitSet(size) {
}

BitSet::~BitSet() {
}

std::vector<uint64_t> BitSet::getBits() {
    std::vector<uint64_t> blocks(bitSet.num_blocks());
    if (!blocks.empty()) {
        boost::to_block_range(bitSet, blocks.begin());
    }
    return blocks;
}

void BitSet::clear() {
    bitSet.clear();
}

void BitSet::clear(uint32_t bitIndex) {
    if (bitIndex <= bitSet.size()) {
        bitSet.set(bitIndex, false);
    }
}

void BitSet::fastClear(uint32_t bitIndex) {
    bitSet.set(bitIndex, false);
}

void BitSet::clear(uint32_t fromIndex, uint32_t toIndex) {
    toIndex = std::min(toIndex, (uint32_t)bitSet.size());
    for (bitset_type::size_type i = std::min(fromIndex, (uint32_t)bitSet.size()); i < toIndex; ++i) {
        bitSet.set(i, false);
    }
}

void BitSet::fastClear(uint32_t fromIndex, uint32_t toIndex) {
    for (bitset_type::size_type i = fromIndex; i < toIndex; ++i) {
        bitSet.set(i, false);
    }
}

void BitSet::set(uint32_t bitIndex) {
    if (bitIndex >= bitSet.size()) {
        resize(bitIndex + 1);
    }
    bitSet.set(bitIndex, true);
}

void BitSet::fastSet(uint32_t bitIndex) {
    bitSet.set(bitIndex, true);
}

void BitSet::set(uint32_t bitIndex, bool value) {
    if (bitIndex >= bitSet.size()) {
        resize(bitIndex + 1);
    }
    bitSet.set(bitIndex, value);
}

void BitSet::fastSet(uint32_t bitIndex, bool value) {
    bitSet.set(bitIndex, value);
}

void BitSet::set(uint32_t fromIndex, uint32_t toIndex) {
    if (toIndex >= bitSet.size()) {
        resize(toIndex + 1);
    }
    for (bitset_type::size_type i = fromIndex; i < toIndex; ++i) {
        bitSet.set(i, true);
    }
}

void BitSet::fastSet(uint32_t fromIndex, uint32_t toIndex) {
    for (bitset_type::size_type i = fromIndex; i < toIndex; ++i) {
        bitSet.set(i, true);
    }
}

void BitSet::set(uint32_t fromIndex, uint32_t toIndex, bool value) {
    if (toIndex >= bitSet.size()) {
        resize(toIndex + 1);
    }
    for (bitset_type::size_type i = fromIndex; i < toIndex; ++i) {
        bitSet.set(i, value);
    }
}

void BitSet::fastSet(uint32_t fromIndex, uint32_t toIndex, bool value) {
    for (bitset_type::size_type i = fromIndex; i < toIndex; ++i) {
        bitSet.set(i, value);
    }
}

void BitSet::flip(uint32_t bitIndex) {
    if (bitIndex >= bitSet.size()) {
        resize(bitIndex + 1);
    }
    bitSet.flip(bitIndex);
}

void BitSet::fastFlip(uint32_t bitIndex) {
    bitSet.flip(bitIndex);
}

void BitSet::flip(uint32_t fromIndex, uint32_t toIndex) {
    if (toIndex >= bitSet.size()) {
        resize(toIndex + 1);
    }
    for (bitset_type::size_type i = fromIndex; i < toIndex; ++i) {
        bitSet.flip(i);
    }
}

void BitSet::fastFlip(uint32_t fromIndex, uint32_t toIndex) {
    for (bitset_type::size_type i = fromIndex; i < toIndex; ++i) {
        bitSet.flip(i);
    }
}

uint32_t BitSet::size() const {
    return bitSet.num_blocks() * sizeof(bitset_type::block_type) * 8;
}

uint32_t BitSet::numBlocks() const {
    return bitSet.num_blocks();
}

bool BitSet::isEmpty() const {
    return bitSet.none();
}

bool BitSet::get(uint32_t bitIndex) const {
    return bitIndex < bitSet.size() ? bitSet.test(bitIndex) : false;
}

bool BitSet::fastGet(uint32_t bitIndex) const {
    return bitSet.test(bitIndex);
}

int32_t BitSet::nextSetBit(uint32_t fromIndex) const {
    bitset_type::size_type next = fromIndex == 0 ? bitSet.find_first() : bitSet.find_next(fromIndex - 1);
    return next == bitset_type::npos ? -1 : next;
}

void BitSet::_and(const BitSetPtr& set) {
    // this = this AND set, keeping this's size (bits beyond set's range become 0).
    // Fast path: equal sizes (the common case) operate in place with no temporary;
    // dynamic_bitset's operator&= requires matching sizes, so only copy/resize when they differ.
    if (bitSet.size() == set->bitSet.size()) {
        bitSet &= set->bitSet;
        return;
    }
    bitset_type other(set->bitSet);
    other.resize(bitSet.size());
    bitSet &= other;
}

void BitSet::_or(const BitSetPtr& set) {
    // this = this OR set, growing this to the larger of the two sizes.
    if (set->bitSet.size() > bitSet.size()) {
        resize(set->bitSet.size());
    }
    if (bitSet.size() == set->bitSet.size()) {
        bitSet |= set->bitSet;
        return;
    }
    bitset_type other(set->bitSet);
    other.resize(bitSet.size());
    bitSet |= other;
}

void BitSet::_xor(const BitSetPtr& set) {
    // this = this XOR set, growing this to the larger of the two sizes.
    if (set->bitSet.size() > bitSet.size()) {
        resize(set->bitSet.size());
    }
    if (bitSet.size() == set->bitSet.size()) {
        bitSet ^= set->bitSet;
        return;
    }
    bitset_type other(set->bitSet);
    other.resize(bitSet.size());
    bitSet ^= other;
}

void BitSet::andNot(const BitSetPtr& set) {
    // this = this AND NOT set, keeping this's size.
    if (bitSet.size() == set->bitSet.size()) {
        bitSet -= set->bitSet;
        return;
    }
    bitset_type other(set->bitSet);
    other.resize(bitSet.size());
    bitSet -= other;
}

bool BitSet::intersectsBitSet(const BitSetPtr& set) const {
    return bitSet.intersects(set->bitSet);
}

uint32_t BitSet::cardinality() {
    return (uint32_t)bitSet.count();
}

void BitSet::resize(uint32_t size) {
    // dynamic_bitset::resize allocates the required blocks, zero-fills any new
    // bits and masks the unused bits in the final block.
    bitSet.resize(size);
}

bool BitSet::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }
    BitSetPtr otherBitSet(boost::dynamic_pointer_cast<BitSet>(other));
    if (!otherBitSet) {
        return false;
    }
    // Two bit sets are equal if they have the same set bits, even when their
    // sizes differ (the extra bits of the larger must all be zero). Normalise
    // both to the larger size (zero-padded) and compare.
    bitset_type a(bitSet);
    bitset_type b(otherBitSet->bitSet);
    bitset_type::size_type maxSize = std::max(a.size(), b.size());
    a.resize(maxSize);
    b.resize(maxSize);
    return a == b;
}

int32_t BitSet::hashCode() {
    // Start with a zero hash and use a mix that results in zero if the input is zero.
    // This effectively truncates trailing zeros without an explicit check.
    int64_t hash = 0;
    uint32_t maxSize = bitSet.num_blocks();
    if (maxSize > 0) {
        std::vector<uint64_t> blocks(maxSize);
        boost::to_block_range(bitSet, blocks.begin());
        for (uint32_t bit = 0; bit < maxSize; ++bit) {
            hash ^= blocks[bit];
            hash = (hash << 1) | (hash >> 63); // rotate left
        }
    }
    // Fold leftmost bits into right and add a constant to prevent empty sets from
    // returning 0, which is too common.
    return (int32_t)((hash >> 32) ^ hash) + 0x98761234;
}

LuceneObjectPtr BitSet::clone(const LuceneObjectPtr& other) {
    LuceneObjectPtr clone = other ? other : newLucene<BitSet>();
    BitSetPtr cloneBitSet(boost::dynamic_pointer_cast<BitSet>(LuceneObject::clone(clone)));
    cloneBitSet->bitSet = bitSet;
    return cloneBitSet;
}

}
