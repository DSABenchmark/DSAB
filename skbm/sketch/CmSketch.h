#ifndef CM_SKETCH_H
#define CM_SKETCH_H
#include "SketchBase.h"
#include "factor.h"
#include "../hash/hashfunction.h"
#include<string>
#include<iostream>
#include<memory.h>
/*
int is type of frequencyQuery Return
hashfunction type:BOBHash\
virtual ~SketchBase() = 0;
virtual void parameterSet(const string& parameterName, double & parameterValue)=0;
virtual init() = 0;
virtual void Insert(const char *str, const int & len) = 0;
virtual int frequencyQuery(const char *str, const int & len) = 0;
virtual int topkQuery(const int & k) = 0;
virtual void reset() = 0;//reset sketch to the initial state
*/
class CmSketch: public SketchBase {
private:
    int hash_num;
    int counter_per_array;
    BOBHash * hash;
    int bit_per_counter;
    int **data;
public:
    using SketchBase::sketch_name;
    CmSketch()
    {
        sketch_name =  "NEWcmsketch";
    }
    void parameterSet(const std::string& parameterName, double  parameterValue)
    {

        if (parameterName=="hash_num")
        {

            hash_num = parameterValue;
            return;
        }
        if (parameterName=="bit_per_counter")
        {
            bit_per_counter = parameterValue;
            return;
        }
         if (parameterName=="counter_per_array")
        {
            counter_per_array = parameterValue;
            return;
        }

    }
    void init()
    {
        data = new int*[hash_num];
        hash = new BOBHash[hash_num];
        for (int i = 0; i<hash_num; ++i)
        {
            data[i] = new int[counter_per_array];
            memset(data[i],0,sizeof(int)*counter_per_array);
            hash[i].SetSeed(i+1000);
        }

    }
    void Insert(const char *str, const int & len)
    {
        for (int i = 0; i < hash_num; ++i)
        {
            ++data[i][hash[i].Run(str, len) % counter_per_array];
        }
    }
    int frequencyQuery(const char *str, const int & len)
    {
        int res = data[0][hash[0].Run(str, len) % counter_per_array];
        for (int i = 1; i < hash_num; ++i) {
            int t = data[i][hash[i].Run(str, len) % counter_per_array];
            res = res < t ? res : t;
        }
        return res;
    }
    int topkQuery(const int & k)
    {
        return 0;
    }
    void reset()
    {
        for (int i; i<hash_num; ++i)
        {
            memset(data[i],0,sizeof(int)*counter_per_array);
        }
    }
    ~CmSketch()
    {
        for (int i; i<hash_num; ++i)
        {
            delete [] data[i];
        }
        delete [] data;
    }
};
REGISTER(CmSketch);
#endif