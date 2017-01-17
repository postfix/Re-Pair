/*
 * hf_queue.hpp
 *
 *  Created on: Jan 12, 2017
 *      Author: nico
 *
 *  High-frequency pairs queue
 *
 *  This queue is a pair Q = <H,B> of structures, where:
 *
 *  - H: sigma x sigma -> int is a hash table pointing at elements in B
 *  - B is a linked list storing all high-frequency pairs
 *
 *  Supported operations:
 *
 *  operator[ab]: return triple <P_ab, L_ab, F_ab> relative to pair ab
 *  max()/min(): return pair ab with max/min F_ab
 *  remove(ab): delete pair ab from queue
 *  contains(ab): true iff ab is in the queue
 *  size(): current queue size
 *  decrease(ab): decrease by 1 ab's frequency F_ab. This function removes ab if its frequency goes below the queue's min frequency
 *  insert(list_el), where list_el = <ab, P_ab, L_ab, F_ab> is a linked list element
 *
 *
 */

#include <ll_vec.hpp>
#include <unordered_map>
#include <ll_el.hpp>

using namespace std;

#ifndef INTERNAL_HF_QUEUE_HPP_
#define INTERNAL_HF_QUEUE_HPP_


/*
 * template on linked list type and integer type
 */
template<typename ll_type = ll_vec32_t>
class hf_queue{

using itype = typename ll_type::int_type;
using ctype = typename ll_type::char_type;

using cpair = pair<ctype,ctype>;
using hash_t = std::unordered_map<cpair, itype>;

public:

	using triple_t = triple<itype>;
	using el_type = typename ll_type::el_type;

	/*
	 * default constructor. Note that object must be created with the other constructor in order to be
	 * usable (using object built with this constructor causes failed assertions)
	 */
	hf_queue(){

		max_size = 0;
		min_freq = 0;

	}

	/*
	 * build queue with max capacity equal to max_size and minimum allowed
	 * frequency of a pair equal to min_frequency (included). If a pair'sfrequency
	 * becomes strictly smaller than min_frequency, then the pair is removed from the queue
	 */
	hf_queue(itype max_size, itype min_freq) {

		assert(min_freq>1);

		this->min_freq = min_freq;
		this->max_size = max_size;

		H = hash_t(max_size*2);

	}

	/*
	 * return triple <P_ab, L_ab, F_ab> relative to pair ab
	 * complexity: O(1)
	 */
	triple_t operator[](cpair ab){

		assert(max_size>0);
		assert(contains(ab));

		auto e = B[H[ab]];

		return {e.P_ab, e.L_ab, e.F_ab};

	}

	cpair min(){

		assert(max_size>0);

		cpair p = B.min_pair();
		assert(contains(p));

		return p;

	}

	cpair max(){

		assert(max_size>0);

		cpair p = B.max_pair();
		assert(contains(p));

		return p;

	}

	void remove(cpair ab){

		assert(contains(ab));
		assert(max_size>0);

		B.remove(H[ab]);
		H.erase(ab);

		//if more than half of B's entries are empty, compact B.
		if(B.size() < B.capacity()/2) compact_ll();

	}

	bool contains(cpair ab){

		assert(max_size>0);

		return H.count(ab) == 1;

	}

	itype size(){

		assert(max_size>0);

		return B.size();

	}

	/*
	 * decrease by 1 F_ab
	 */
	void decrease(cpair ab){

		assert(contains(ab));
		assert(max_size>0);

		//frequency must be >0, otherwise we would alredy have removed the pair
		assert(B[H[ab]].F_ab>0);

		B[H[ab]].F_ab--;

		//if frequency becomes too small, remove pair
		if(B[H[ab]].F_ab < min_freq){

			remove(ab);

		}

	}

	void insert(el_type el){

		assert(max_size>0);

		cpair ab = el.ab;

		//must not already contain ab
		assert(not contains(ab));
		//must meet requirement on minimum frequency
		assert(el.F_ab >= min_freq);

		itype idx = B.insert(el);

		H.insert({ab,idx});
		assert(H[ab]==idx);

		assert(B[H[ab]].P_ab == el.P_ab);
		assert(B[H[ab]].L_ab == el.L_ab);
		assert(B[H[ab]].F_ab == el.F_ab);

		//must not exceed max capacity
		assert(size()<=max_size);

	}

private:

	/*
	 * compact memory used by the linked list and re-compute
	 * pair's indexes
	 */
	void compact_ll(){

		assert(max_size>0);

		B.compact();

		for(itype i=0;i<B.size();++i){

			auto ab = B[i].ab;
			H[ab] = i;

		}

	}

	itype max_size;
	itype min_freq;

	ll_type B;
	hash_t H;

	const static itype null = ~itype(0);

};

typedef hf_queue<ll_vec32_t> hf_queue32_t;
typedef hf_queue<ll_vec64_t> hf_queue64_t;


#endif /* INTERNAL_HF_QUEUE_HPP_ */