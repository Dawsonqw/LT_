#include "JsonConfig.h"
#include "stdio.h"
using namespace std;
int main(){
    LT::JsonParse jsonParse("../conf/ConfigTest.json");
    if(jsonParse.OpenFile()){
        std::string ret=jsonParse.JsonGetVal("Test","lag","java");
        printf("ret:%s\n",ret.c_str());
    }else{
        printf("error\n");
    }

    LT::JsonManger jsonManger;
    std::string ret=jsonManger.GetVal("../conf/ConfigTest.json","fiber","lag","c+cc");
    printf("manger:%s\n",ret.c_str());

    return 0;
}