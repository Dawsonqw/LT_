#ifndef LT_JSONCONFIG_H
#define LT_JSONCONFIG_H
#include <string>
#include <json/json.h>
#include <unordered_map>
#include <fstream>
#include <stdio.h>


#include "singleton.h"

///需求：在任意地方拿到json中对应的key的值

//访问api： Get(objname,keyname,value,defaultvalue)
namespace LT {
    class JsonParse {
    private:
        Json::Value m_root;

        Json::Reader m_reader;

        std::string m_filename;

        bool m_openerr;
    public:
        typedef std::shared_ptr<JsonParse> ptr;

        JsonParse() = delete;

        JsonParse(const std::string &FileName);

        bool OpenFile() const;

        std::string JsonGetVal(const std::string &ObjName, const std::string &KeyName, const std::string& defaultval);
    };


    class JsonManger{
    private:
        std::map<std::string,JsonParse::ptr> m_configers;

    public:
        std::string GetVal(const std::string& filename,const std::string &ObjName, const std::string &KeyName, const std::string& defaultval);
    };


    typedef Singleton<JsonManger> JsonMg;
}
#endif //LT_JSONCONFIG_H
