#include "util/json.h"
#include "util/log.h"
#include "rapidjson/error/en.h"
#include "rapidyaml.hpp"

int get_line_number(const std::string& str, int pos)
{
	int ln = 1;
	auto p = str.find_first_of('\n');
	while (p != std::string::npos)
	{
		if (p > pos)
		{
			return ln;
		}
		ln++;
		p = str.find_first_of('\n', p + 1);
	}
	return -1;
}

int find_last_newline(const char* str, int end_pos)
{
	while (--end_pos > 0)
	{
		if (str[end_pos] == '\n')
		{
			return end_pos;
		}
	}
	return -1;
}

int find_next_newline(const char* str, int end_pos)
{
	int length = (int)strlen(str);
	while (++end_pos < length)
	{
		if (str[end_pos] == '\n')
		{
			return end_pos;
		}
	}
	return length - 1;
}

bool asyncflow::util::JsonUtil::ParseJson(const std::string& json_str, rapidjson::Document& doc)
{
	if (json_str.empty())
	{
		ASYNCFLOW_ERR("parse json error : json string is empty");
		return false;
	}
	rapidjson::ParseResult ok = doc.Parse<rapidjson::kParseCommentsFlag | rapidjson::kParseTrailingCommasFlag>(json_str.c_str());
	if (!ok)
	{

		int pos = (int)ok.Offset();
		auto jcstr = json_str.c_str();
		int startPos = find_last_newline(jcstr, pos);
		int startpos2 = find_last_newline(jcstr, startPos);
		int endPos = find_next_newline(jcstr, pos);		
		startPos++;		

		auto currentline = std::string(jcstr + startPos, endPos - startPos);

		ASYNCFLOW_ERR("JSON parse error: {0} at line {2}, position ({1})", rapidjson::GetParseError_En(ok.Code())
			, ok.Offset(), get_line_number(json_str, pos));
		if (startpos2 != -1)
		{
			auto last_line = std::string(jcstr + startpos2 + 1, startPos - startpos2 - 2);
			ASYNCFLOW_ERR("{0}", last_line);
		}
		ASYNCFLOW_ERR("{0}", currentline);
		char* line = new char[endPos - startPos + 1];
		for (int i = 0; i < endPos - startPos; i++)
		{
			line[i] = ' ';
		}
		line[pos - startPos] = '^';
		line[pos - startPos + 1] = 0;
		ASYNCFLOW_ERR("{0}", line);
		delete[] line;
		return false;
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////

void asyncflow::util::YamlErrorHandler::on_error(const char* msg, size_t len, ryml::Location loc)
{
	throw std::runtime_error(ryml::formatrs<std::string>("{}:{}:{} ({}B): ERROR: {}",
		loc.name, loc.line, loc.col, loc.offset, ryml::csubstr(msg, len)));
}

ryml::Callbacks asyncflow::util::YamlErrorHandler::callbacks()
{
	return ryml::Callbacks(this, nullptr, nullptr, YamlErrorHandler::s_error);
}

void asyncflow::util::YamlErrorHandler::s_error(const char* msg, size_t len, ryml::Location loc, void* this_)
{
	return ((YamlErrorHandler*)this_)->on_error(msg, len, loc);
}