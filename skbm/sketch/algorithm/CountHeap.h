#ifndef CountHeap_H //must change this MACRO
#define CountHeap_H //must change this MACRO
#include "../SketchBase.h" //DO NOT change this include
#include "../factor.h"//DO NOT change this include
#include "../hash/hashfunction.h"//If you want to use DSAB-builtin hashfunction must include this
#include "../hash/cuckoo_hashing.h" //If you want to use DSAB-builtin hashtable must include thiss

/*----optional according to your need----*/
#include<string>
#include<iostream>
#include<memory.h>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <time.h>
using std::min;
using std::swap;
#define SQR(X) (X) * (X)
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
define: cuckoo::CuckooHashing<key_len> ht;
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


class CountHeap: public SketchBase {
private:
	/*----optional according to your need----*/
	typedef pair <string, int> KV;
	typedef pair <int, string> VK;
	VK heap*;
	int capacity;
	int heap_element_num;
	int mem_in_bytes;
	int hash_num;
	int w;
	int ** c_sketch;
	BOBHash * hash;//optional DSAB-builtin hashfunction
	BOBHash * hash_polar;
	unordered_map<string, uint32_t> ht;
    /*----optional according to your need----*/
	// heap
	void heap_adjust_down(int i) {
		while (i < heap_element_num / 2) {
			int l_child = 2 * i + 1;
			int r_child = 2 * i + 2;
			int larger_one = i;
			if (l_child < heap_element_num && heap[l_child] < heap[larger_one]) {
				larger_one = l_child;
			}
			if (r_child < heap_element_num && heap[r_child] < heap[larger_one]) {
				larger_one = r_child;
			}
			if (larger_one != i) {
				swap(heap[i], heap[larger_one]);
				swap(ht[heap[i].second], ht[heap[larger_one].second]);
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
			if (heap[parent] <= heap[i]) {
				break;
			}
			swap(heap[i], heap[parent]);
			swap(ht[heap[i].second], ht[heap[parent].second]);
			i = parent;
		}
	}

public:
    using SketchBase::sketch_name;//DO NOT change this declaration
    CountHeap()
    {
        /*constructed function MUST BT non-parameter!!!*/
        sketch_name =  "CountHeap";//please keep sketch_name the same as class name and .h file name
    }
    void parameterSet(const std::string& parameterName, double & parameterValue)
    {
        /*MUST have this function even empty function body*/

        /*----optional according to your need----*/
        if (parameterName=="hash_num")
        {

            hash_num = parameterValue;
            return;
        }
        if (parameterName=="mem_in_bytes")
        {
            mem_in_bytes = parameterValue;
            return;
        }
         if (parameterName=="capacity")
        {
            capacity = parameterValue;
            return;
        }
        /*----optional according to your need----*/
    }
    void init()
    {
        /*MUST have this function even empty function body*/

        /*----optional according to your need----*/
		w = mem_in_bytes / 4 / hash_num;
		heap = new VK[capacity];
		memset(heap, 0, capacity * sizeof(VK));
		for (int i = 0; i < capacity; ++i) {
			heap[i].first = 0;
		}

        c_sketch = new int*[hash_num];
        hash = new BOBHash[hash_num];
		hash_polar = new BOBHash[hash_num];
		srand(time(0));
        for (int i = 0; i<hash_num; ++i)
        {
			
			hash[i].SetSeed(uint32_t(rand() % MAX_PRIME32));
			hash_polar[i].SetSeed(uint32_t(rand() % MAX_PRIME32));
			c_sketch = new int[w];
			memset(c_sketch[i], 0, sizeof(int)*w);
        }
        /*----optional according to your need----*/
    }
    void Insert(const char *str, const int & len)
    {
        /*MUST have this function DO NOT change parameter type*/

        /*----optional according to your need----*/
		int ans[hash_num];

		for (int i = 0; i < hash_num; ++i) {
			int idx = hash[i]->run((char *)key, len) % w;
			int polar = hash_polar[i]->run((char *)key, len) % 2;

			c_sketch[i][idx] += polar ? 1 : -1;

			int val = c_sketch[i][idx];

			ans[i] = polar ? val : -val;
		}

		sort(ans, ans + hash_num);

		int tmin;
		if (hash_num % 2 == 0) {
			tmin = (ans[hash_num / 2] + ans[hash_num / 2 - 1]) / 2;
		}
		else {
			tmin = ans[hash_num / 2];
		}
		tmin = (tmin <= 1) ? 1 : tmin;

		string str_key = string((const char *)key, len);
		if (ht.find(str_key) != ht.end()) {
			heap[ht[str_key]].first++;
			heap_adjust_down(ht[str_key]);
		}
		else if (heap_element_num < capacity) {
			heap[heap_element_num].second = str_key;
			heap[heap_element_num].first = tmin;
			ht[str_key] = heap_element_num++;
			heap_adjust_up(heap_element_num - 1);
		}
		else if (tmin > heap[0].first) {
			VK & kv = heap[0];
			ht.erase(kv.second);
			kv.second = str_key;
			kv.first = tmin;
			ht[str_key] = 0;
			heap_adjust_down(0);
		}
        /*----optional according to your need----*/
    }
    int frequencyQuery(const char *str, const int & len)
    {
         /*MUST have this function DO NOT change function head and parameter type */

        /*----optional according to your need----*/
		int ans[hash_num];

		for (int i = 0; i < hash_num; ++i) {
			int idx = hash[i].run(key,len) % w;
			int polar = hash_polar[i].run((char *)key, len) % 2;

			int val = c_sketch[i][idx];

			ans[i] = polar ? val : -val;
		}

		sort(ans, ans + hash_num);

		int tmin;
		if (hash_num % 2 == 0) {
			tmin = (ans[hash_num / 2] + ans[hash_num / 2 - 1]) / 2;
		}
		else {
			tmin = ans[hash_num / 2];
		}
		return (tmin <= 1) ? 1 : tmin;

        /*----optional according to your need----*/
    }
    vector<string>  topkQuery(const int & k)
    {
        /*MUST have this function DO NOT change function head and parameter type */

        /*----optional according to your need----*/
        vector<string> topkItem;

		VK * a = new VK[capacity];
		for (int i = 0; i < capacity; ++i) {
			a[i] = heap[i];
		}
		sort(a, a + capacity);
		int i;
		for (i = 0; i < k && i < capacity; ++i) {
			topkItem.pus_back(a[capacity - 1 - i].second);
		
		}
        return topkItem;
        /*----optional according to your need----*/
    }
    void reset()
    {
         /*MUST have this function,reset sketch to the initial state */

        /*----optional according to your need----*/
        for (int i; i<hash_num; ++i)
        {
            memset(c_sketch[i],0,sizeof(int)*w);
        }
		memset(heap, 0, sizeof(VK)*capacity);
		heap_element_num = 0;
        /*----optional according to your need----*/
    }
    ~CmSketch()
    {
        /*MUST have this function */

        /*----optional according to your need----*/
        for (int i; i<hash_num; ++i)
        {
            delete [] c_sketch[i];
			delete hash[i];
			delete hash_polar[i];
        }
        delete [] c_sketch;
        delete [] hash;
		delete [] hash_polar;
        /*----optional according to your need----*/
    }

    /*----optional You can add your function----*/
};
REGISTER(CountHeap);
#endif//DO NOT change this file