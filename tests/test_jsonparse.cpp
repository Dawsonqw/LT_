#include "JsonConfig.h"
#include "stdio.h"
using namespace std;
int main(){
    LT::JsonParse jsonParse("../conf/Config.json");
    if(jsonParse.OpenFile()){
        std::string ret=jsonParse.JsonGetVal("fiber","lag","java");
        printf("ret:%s\n",ret.c_str());
    }else{
        printf("error\n");
    }

    LT::JsonManger jsonManger;
    std::string ret=jsonManger.GetVal("../conf/Config.json","fiber","lag","c+cc");
    printf("manger:%s\n",ret.c_str());
    getchar();

    return 0;
}