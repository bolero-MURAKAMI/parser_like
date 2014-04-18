//DYZ parser generator
//string_traits.hpp
//(c)DtYaZsK 2014

#pragma once

#include<string>

namespace dyz_parser
{
	
	inline bool string_match(std::string::const_iterator beg, std::string::const_iterator end, const char* str)
	{
		while (true)
		{
			if (*str == '\0')
			{
				return true;
			}
			if (beg == end)
			{
				return false;
			}
			if (*beg != *str)
			{
				return false;
			}
			++beg;
			++str;
		}
	}
	inline size_t string_literal_check(std::string::const_iterator beg, std::string::const_iterator end)
	{
		if (*beg != '\"')
			return 0;
		bool is_escape = false;
		auto ite = beg;
		++ite;
		for (; ite != end; ++ite)
		{
			if (is_escape)
			{
				is_escape = false;
				continue;
			}
			if (*ite == '\"')
				return std::distance(beg, ite) + 1;
			if (*ite == '\\')
				is_escape = true;
		}
		return false;
	}

	inline bool token_string_check(char c)
	{
		return ((static_cast<int>('a') <= static_cast<int>(c)) &&
			(static_cast<int>(c) <= static_cast<int>('z'))) ||
			((static_cast<int>('A') <= static_cast<int>(c)) &&
			(static_cast<int>(c) <= static_cast<int>('Z'))) ||
			((static_cast<int>('0') <= static_cast<int>(c)) &&
			(static_cast<int>(c) <= static_cast<int>('9')));
	}

}
