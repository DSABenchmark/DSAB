#include "StreamData.h"
#include "sketchList.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <map>
#include <math.h>
#include <time.h>
#include <vector>
#include <tuple>
#include "SketchBase.h"
#include "factor.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
using namespace std;

void* ClassFactory::getClassByName(string className){
    map<string, PTRCreateObject>::const_iterator iter;
    iter = m_classMap.find(className) ;
    if ( iter == m_classMap.end() )
        return NULL ;
    else
        return iter->second() ;
}

void ClassFactory::registClass(string name, PTRCreateObject method){
    m_classMap.insert(pair<string, PTRCreateObject>(name, method)) ;
}

ClassFactory& ClassFactory::getInstance(){
    static ClassFactory sLo_factory;
    return sLo_factory ;
}

tuple<string,double> parseArg(string argument) {
    int equPos = argument.find('=');
    string key(argument, 0, equPos);
    double value = stod(string(argument, equPos+1, argument.length()));
    return make_tuple(key, value);
}

void splitString(const string& s, vector<string>& v, const string& sep){
  string::size_type pos1, pos2;
  pos2 = s.find(sep);
  pos1 = 0;
  while(string::npos != pos2)
  {
    v.push_back(s.substr(pos1, pos2-pos1));
 
    pos1 = pos2 + sep.size();
    pos2 = s.find(sep, pos1);
  }
  if(pos1 != s.length())
    v.push_back(s.substr(pos1));
}



int main(int argc, char *argv[]) {
    /* 
    datasetName+sketchName+task1+...+key1=value1+...: argv[k]   k>=2
    */
    rapidjson::Document document;
    rapidjson::Value lst(rapidjson::kArrayType);
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    boost::uuids::random_generator rgen;
    string uname;
    const int bytesPerStr = 4;
    vector< vector<string> > args;
    vector<string> datasets;
    for(int n=1;n<argc;n++) {
        vector<string> items;
        splitString(string(argv[n]), items, "+");
        args.push_back(items);
        datasets.push_back(items[0]);
    }
    for(auto iter = datasets.begin(); iter != datasets.end(); iter++) {
        string datasetName(*iter);
        // cout << "Current dataset: " << datasetName << endl;
        StreamData dat((string("../../dataset/")+datasetName).c_str(), bytesPerStr);
        unordered_map<string, int> item2freq;
        unordered_map<string, int> item2idx;
        int idx =0;
        char str[bytesPerStr];
        vector<string> v;
        while (dat.GetNext(str))
        {
            v.push_back(string(str, bytesPerStr));
            ++item2freq[string(str, bytesPerStr)];
            item2idx[string(str, bytesPerStr)]= idx++;
        }
        // cout << "Finished parse dataset " << datasetName << endl;
        // start sketch experiments!
        for(int k=0; k<args.size(); k++) {
            vector<string> items = args[k];
            if(items[0] == datasetName) {
                string sketchName(items[1]);
                vector<string> tasks;
                vector< tuple<string,double> > keyValuePairs;                
                int i=2;
                while(items[i].find("=") == string::npos) {
                    tasks.push_back(items[i]);
                    i++;
                }
                int split = i;
                for(;i<items.size();i++){
                    tuple<string,double> keyValue = parseArg(items[i]);
                    keyValuePairs.push_back(keyValue);
                }
                // Now we have datasetName, sketchName, tasks, and keyValuePairs!!
                SketchBase* player = (SketchBase*)ClassFactory::getInstance().getClassByName(string(sketchName));
                for(auto iter=keyValuePairs.begin(); iter!=keyValuePairs.end(); iter++) {
                    player->parameterSet(get<0>(*iter),get<1>(*iter));
                }
                player->init();
                clock_t start,finish;
                start = clock();
                for(auto iter = v.begin(); iter!=v.end(); iter++) {
                    player->Insert(iter->c_str(), bytesPerStr);
                }
                finish = clock();
                // cout << "Total time use: " << finish - start << endl;
                // Now perform tasks
                for (auto iter=tasks.begin(); iter!=tasks.end(); iter++) {
                    string task(*iter);
                    if(task == "freq") {
                        int totNum = v.size();
                        uname = boost::uuids::to_string(rgen());
                        // cout << uname << endl;
                        ofstream fout("../../experiment/output/"+uname);
                        int num_item = 0, num_correct = 0;
                        for (const auto& p: item2freq) {
                            int a=p.second , b=player->frequencyQuery(p.first.c_str(), bytesPerStr);
                            fout << a << " " << b << endl;
                            if (a==b) num_correct++;
                            num_item++;
                        }
                        fout.close();
                        // json setup
                        rapidjson::Document info;
                        info.SetObject();
                        rapidjson::Value v;
                        // char bf[]

                        info.AddMember("datasetName", rapidjson::StringRef(datasetName.c_str()), info.GetAllocator());
                        info.AddMember("sketchName", rapidjson::StringRef(sketchName.c_str()), info.GetAllocator());
                        info.AddMember("task", rapidjson::StringRef(task.c_str()), info.GetAllocator());
                        info.AddMember("totNum", totNum, info.GetAllocator());
                        info.AddMember("num_item", num_item, info.GetAllocator());
                        info.AddMember("num_correct", num_correct, info.GetAllocator());
                        info.AddMember("uuid", rapidjson::StringRef(uname.c_str()), info.GetAllocator());
                        // for(int i=split;i<items.size();i++){
                        //     int equPos = items[i].find("=");
                        //     string key = items[i].substr(0, equPos);
                        //     cout << key << endl;
                        //     double value = stod(string(items[i], equPos+1, items[i].length()));

                        //     rapidjson::Document document;
                        //     rapidjson::Value tmp;
                        //     char buf[30]={0};
                        //     strcpy(buf, key.c_str());
                        //     int len = key.length(); 
                        //     for(int j=len;j<30;j++) buf[j] = '\0';
                        //     tmp.SetString(buf, len, document.GetAllocator());
                        //     memset(buf, 0, sizeof(buf));

                        //     info.AddMember(tmp, value, info.GetAllocator());
                        // }

                        for(auto iter=keyValuePairs.begin(); iter!=keyValuePairs.end(); iter++) {
                            tuple<string,double> tmp = *iter;
                            string argName = get<0>(tmp);
                            // char tmp[30];
                            // strcpy(tmp, get<0>(*iter).c_str());
                            // cout << tmp << endl;
                            double argValue = get<1>(*iter);
                            if (argName=="hash_num")
                                info.AddMember("hash_num", argValue, info.GetAllocator());
                            else if(argName=="bit_per_counter")
                                info.AddMember("bit_per_counter", argValue, info.GetAllocator());
                            else if(argName=="counter_per_array")
                                info.AddMember("counter_per_array", argValue, info.GetAllocator());
                        }
                        // lst.PushBack(info, allocator);
                        rapidjson::StringBuffer buffer;
                        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                        info.Accept(writer);
                        cout << buffer.GetString() << endl;
                    }
                    
                }
            }
        }
    }
    // document.SetObject();
    // document.AddMember("jsonFiles", lst, allocator);
    // rapidjson::StringBuffer buffer;
    // rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    // document.Accept(writer);
    // cout << buffer.GetString() << endl;
}















































