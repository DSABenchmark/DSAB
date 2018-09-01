#ifndef CMHEAP_H //must change this MACRO
#define CMHEAP_H //must change this MACRO
#include "../SketchBase.h" //DO NOT change this include
#include "../factor.h"//DO NOT change this include
#include "../hash/hashfunction.h" //If you want to use DSAB-builtin hashfunction must include this
#include "../hash/cuckoo_hashing.h" //If you want to use DSAB-builtin hashtable must include this

/*----optional according to your need----*/
#include<string>
#include<iostream>
#include<memory.h>
#include <unordered_map>
#include <algorithm>
#include <sstream>
using std::min;
using std::swap;
/*----optional according to your need----*/

/*----builtin hashfunction----*/
/*
DSAB-builtin hashfunction type:BOBHash\
HOW TO USE:
define: e.g. BOBHash myhash
setseed: e.g. myhash.SetSeed(1001)
calculate hash: e.g. myhash.Run(const char *str, const int & len)
*/
/*----builtin hashfunction----*/

/*----builtin hashTable----*/
/*
DSAB-builtin hashTable type:cuckoo_hashtable\
HOW TO USE: 
define: cuckoo::CuckooHashing<key_len, capacity> ht;
!!!MUST init: ht.init(capacity)
bool insert(uint8_t * key, uint32_t val, int from_k = -1, int remained = 5)
bool query(uint8_t * key, uint32_t & val)
bool find(uint8_t * key)
bool erase(uint8_t * key)
*/
/*----builtin hashTable----*/


/*----SketchBase virtual function must be finished----*/
/*
virtual ~SketchBase();
virtual void parameterSet(const string& parameterName, double & parameterValue)=0;
virtual init() = 0;
virtual void Insert(const char *str, const int & len) = 0;
virtual int frequencyQuery(const char *str, const int & len) = 0;
virtual vector<string>  topkQuery(const int & k) = 0;
virtual void reset() = 0;//reset sketch to the initial state
*/
/*----SketchBase virtual function must be finished----*/

bool cmp(const pair<string, uint32_t> a, const pair<string, uint32_t> b) {
	return a.second > b.second;//×Ô¶¨ÒåµÄ±È½Ïº¯Êý
}
class CountMinHeap : public SketchBase {
private:
	
	/*----optional according to your need----*/
	int capacity;
	int hash_num;
	typedef pair <uint8_t[4], int> KV;
	KV * heap;
	int heap_element_num;
	int mem_in_bytes;
	int w;
	int ** cm_sketch;
	BOBHash * hash;
	cuckoo::CuckooHashing<4> ht;
	/*----optional according to your need----*/
	// heap
	void heap_adjust_down(int i) {
		while (i < heap_element_num / 2) {
			int l_child = 2 * i + 1;
			int r_child = 2 * i + 2;
			int larger_one = i;
			if (l_child < heap_element_num && heap[l_child].second < heap[larger_one].second) {
				larger_one = l_child;
			}
			if (r_child < heap_element_num && heap[r_child].second < heap[larger_one].second) {
				larger_one = r_child;
			}
			if (larger_one != i) {
				swap(heap[i], heap[larger_one]);
				swap(ht[heap[i].first], ht[heap[larger_one].first]);
				heap_adjust_down(larger_one);
			}
			else {
				break;
			}
		}
	}
	void heap_adjust_up(int i) {
		while (i > 1) {
			int parent = (i - 1) / 2;
			if (heap[parent].second <= heap[i].second) {
				break;
			}
			swap(heap[i], heap[parent]);
			swap(ht[heap[i].first], ht[heap[parent].first]);
			i = parent;
		}
	}

public:
	using SketchBase::sketch_name;//DO NOT change this declaration
	CountMinHeap()
	{
		/*constructed function MUST BT non-parameter!!!*/
		sketch_name = "CountMinHeap";//please keep sketch_name the same as class name and .h file name
	}
	void parameterSet(const std::string& parameterName, double & parameterValue)
	{
		/*MUST have this function even empty function body*/

		/*----optional according to your need----*/
		if (parameterName == "hash_num")
		{

			hash_num = parameterValue;
			return;
		}
		if (parameterName == "capacity")
		{
			capacity = parameterValue;
			return;
		}
		if (parameterName == "mem_in_bytes")
		{
			mem_in_bytes = parameterValue;
			return;
		}
		/*----optional according to your need----*/
	}
	void init()
	{
		/*MUST have this function even empty function body*/

		/*----optional according to your need----*/
		heap_element_num = 0;
		w = mem_in_bytes / 4 / hash_num;
		heap = new KV[capacity];
		memset(heap, 0, sizeof(heap));
		for (int i = 0; i < capacity; ++i) {
			heap[i].second = 0;
		}
		cm_sketch = new int*[hash_num];
		hash = new BOBHash[hash_num];
		for (int i = 0; i<hash_num; ++i)
		{
			cm_sketch[i] = new int[w];
			memset(cm_sketch[i], 0, sizeof(int)*w);
			random_device rd;
			hash[i].SetSeed(uint32_t(rd() % MAX_PRIME32));
		}
		/*----optional according to your need----*/
	}
	void Insert(const char *str, const int & len)
	{
		/*MUST have this function DO NOT change parameter type*/

		/*----optional according to your need----*/
		int tmin = 1 << 30, ans = tmin;

		for (int i = 0; i < hash_num; ++i) {
			int idx = hash[i].run((char *)key, len) % w;
			cm_sketch[i][idx]++;
			int val = cm_sketch[i][idx];
			ans = std::min(val, ans);
		}

		string str_key = string((const char *)key, len);
		if (ht.find(key))
		{
			heap[ht[key]].second++;
			heap_adjust_down(ht[key]);
		}
		else if (heap_element_num < capacity) {
			memcpy(heap[heap_element_num].first, key, len);
			heap[heap_element_num].second = ans;
			ht[key] = heap_element_num++;
			heap_adjust_up(heap_element_num - 1);
		}
		else if ( ans > heap[0].second) {
			KV & kv = heap[0];
			ht.erase(kv.first);
			memcpy(kv.first, key, len);
			kv.second = ans;
			ht[key] = 0;
			heap_adjust_down(0);
		}
		/*----optional according to your need----*/
	}
	int frequencyQuery(const char *str, const int & len)
	{
		/*MUST have this function DO NOT change function head and parameter type */

		/*----optional according to your need----*/
		int tmin = 1 << 30, ans = tmin;
		for (int i = 0; i < hash_num; ++i) {
			int idx = hash[i].run((char*)key, len) % w;
			int val = cm_sketch[i][idx];
			ans = std::min(val, ans);
		}
		return ans;
		/*----optional according to your need----*/
	}
	vector<string>  topkQuery(const int & k)
	{
		/*MUST have this function DO NOT change function head and parameter type */

		/*----optional according to your need----*/
		vector<string> topkItem;
		vector<pair<string, uint32_t> > tmp;
		for (int i = 0; i < capacity; ++i) {
				tmp.emplace_back(make_pair(string((const char *)heap[i].first, 4), heap[i].second));
		}
		sort(tmp.begin(), tmp.end(), cmp);
		for (int i = 0; i < k; ++i)
		{
			topkItem.push_back(tmp[i]);
		}
		return topkItem;
		/*----optional according to your need----*/
	}
	void reset()
	{
		/*MUST have this function,reset sketch to the initial state */

		/*----optional according to your need----*/
		heap_element_num = 0;
		for (int i; i<hash_num; ++i)
		{
			memset(cm_sketch[i], 0, sizeof(int) * w);
		}
		/*----optional according to your need----*/
	}
	~CmSketch()
	{
		/*MUST have this function */

		/*----optional according to your need----*/
		for (int i = 0; i < hash_num; ++i) {
			delete [] hash[i];
			delete [] cm_sketch[i];
		}
		delete [] hash;
		delete [] cm_sketch;
		return;
		/*----optional according to your need----*/
	}

	/*----optional You can add your function----*/
};
REGISTER(CountMinHeap);
#endif//DO NOT change this file