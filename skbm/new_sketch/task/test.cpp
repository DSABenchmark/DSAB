#include "test.h"
void frequencyTest(vector<string> & v,unordered_map<string, int> & item2freq, SketchBase& sketch,const int bytesPerStr,string frequency_file_name )
{
    string sketch_name = sketch.sketch_name;

    for(auto iter = v.begin();iter!=v.end();iter++)
        sketch.Insert(iter->c_str(), bytesPerStr);

    /*accuracy test*/
    ofstream frequency_file;
    frequency_file.open("../../../experiment/output/"+frequency_file_name);
    for (const auto& p: item2freq)
    {
        frequency_file << p.second << "\t";
        frequency_file << sketch.frequencyQuery(p.first.c_str(), bytesPerStr) << "\t";
        frequency_file << endl;
    }
    frequency_file.close();
}
void topkTest(vector<string> & v,vector<itemType> & frequentItem,int k,SketchBase& sketch,const int bytesPerStr, string topk_file_name )
{
    for(auto iter = v.begin();iter!=v.end();iter++)
        sketch.Insert(iter->c_str(), bytesPerStr);
    /*accuracy test*/
    vector<string> queryResult = sketch.topkQuery(k);
    ofstream topk_file;
    topk_file.open("../../../experiment/output/"+topk_file_name);
    for(int i =0;i<k;++k)
    {
        topk_file<<frequentItem[i].id<<" "<<queryResult[i];
    }
    topk_file.close();
}
void insertionSpeedTest(vector<string> & v,SketchBase& sketch,const int bytesPerStr,string throughput_file_name)
{
    /*throuput test*/
    clock_t start,finish;
    start = clock();
    for(auto iter = v.begin();iter!=v.end();iter++)
        sketch.Insert(iter->c_str(), bytesPerStr);
    finish = clock();

    ofstream throughput_file;
    throughput_file.open("../../../experiment/output/"+throughput_file_name);
    // throughput_file << "TotalNum\tTime(s)" << endl;
    throughput_file << v.size() << "\t";
    throughput_file << double(finish-start)/CLOCKS_PER_SEC << "\t";
    throughput_file.close();
}