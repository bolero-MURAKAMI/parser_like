//DYZ parser generator
//string_traits.hpp
//(c)DtYaZsK 2014

#pragma once

#include<cstddef>	// for std::size_t
#include<string>
#include<cctype>
#include<type_traits>

namespace dyz_parser
{
	namespace detail
	{
		extern void* enabler;	// for enable_if

		template<class T>
		struct is_char			// char only (possibly cv-qualified)
			: public std::is_same<typename std::remove_cv<T>::type, char>
		{};
		template<class Iterator>
		struct is_char_iterator
			: public is_char<typename std::iterator_traits<Iterator>::value_type>
		{};
	}

	template<class InputIterator, class StrIterator,
		typename std::enable_if<detail::is_char_iterator<InputIterator>::value && detail::is_char_iterator<StrIterator>::value>::type*& = detail::enabler>
	inline bool string_match(InputIterator beg, InputIterator end, StrIterator str)
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
	template<class InputIterator, typename std::enable_if<detail::is_char_iterator<InputIterator>::value>::type*& = detail::enabler>
	inline std::size_t string_literal_check(InputIterator beg, InputIterator end)
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
		return 0;
	}

	inline bool token_string_check(char c)
	{
		return std::isalnum(c) || c=='_';
	}

}
