#include "JsonConfig.h"
#include "log.h"

namespace LT {

    static lsinks lsink{g_sink,c_sink};
    static auto g_logger = std::make_shared<spdlog::logger>("json",lsink);


    JsonParse::JsonParse(const std::string &FileName) : m_filename(FileName), m_openerr(false) {
        std::ifstream ifs;
        ifs.open(m_filename, std::ios::binary);
        m_openerr = m_reader.parse(ifs, m_root);
        if(!m_openerr) {
            g_logger->debug("json 文件打开失败");
        }
        ifs.close();
    }

    bool JsonParse::OpenFile() const {
        return m_openerr;
    }

    std::string JsonParse::JsonGetVal(const std::string &ObjName, const std::string &KeyName, const std::string &defaultval) {
        if(!m_openerr){
            return defaultval;
        }

        if (m_root[ObjName].isNull()) {
            g_logger->debug("json节点{} 访问失败", ObjName);
            return defaultval;
        }

        Json::Value Obj = m_root[ObjName];

        Json::Value::Members member=Obj.getMemberNames();
        ///获取对象中所有成员迭代器
        for(auto it=member.begin();it!=member.end();it++){
            if(*it==KeyName)
            {
                return Obj[KeyName].asString();
            }
        }
        return defaultval;
    }


    std::string JsonManger::GetVal(const std::string& filename,const std::string &ObjName, const std::string &KeyName, const std::string& defaultval)
    {
        auto it=m_configers.find(filename);
        ///没能找到 新创建一个
        if(it==m_configers.end()){
            printf("新创建一个");
            JsonParse::ptr Obj=std::make_shared<JsonParse>(filename);
            if(Obj->OpenFile()){
                g_logger->debug("openfile filename {} error",filename);
                return defaultval;
            }
            return Obj->JsonGetVal(ObjName,KeyName,defaultval);
        }
        //找到了直接调用
        return it->second->JsonGetVal(ObjName,KeyName,defaultval);
    }
}