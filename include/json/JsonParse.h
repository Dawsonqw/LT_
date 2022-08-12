#ifndef JSONPARSE_H
#define JSONPARSE_H
#include <string>
#include <json/json.h>
#include <fstream>
#include <stdio.h>

namespace LT{
    class JsonParse{
private:
	Json::Value m_root;
	Json::Reader m_reader;
	bool m_openerr;

public:
	JsonParse(const std::string& FileName) 
	{
		std::fstream ifs;
		ifs.open(FileName);
		m_openerr = m_reader.parse(ifs, m_root);
	}
	
	std::string JsonGetString(const std::string& key,const std::string& defaultval) 
	{
		if (!m_openerr) {
			//g_  logger->debug("open json failed");
			return defaultval;
		}
		std::string Value = m_root.get(key, defaultval).asString();
		return Value;
	}
};
}

#endif