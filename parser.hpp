//DYZ parser generator
//parser.hpp
//(c)DtYaZsK 2014

#pragma once

#include"sentence_data.hpp"
#include"string_traits.hpp"
#include<list>
#include<string>

namespace dyz_parser
{
	struct parse_result
	{
		std::list<std::string> tokens_;
		std::list<std::list<std::string>> tuples_;
		std::list<std::string> str_lites_;
		std::list<sentence_data>::const_iterator ite_;
		bool is_valid;
		size_t type_num;
	};
	
	namespace detail
	{
		//parse type
		inline size_t parse_type_case_token(std::string::const_iterator beg, std::string::const_iterator end)
		{
			int ret = 0;
			for (; beg!=end && token_string_check(*beg); ++beg, ++ret);
			return ret;
		}

		inline size_t parse_type_case_tuple(
			std::string::const_iterator beg, 
			std::string::const_iterator end)
		{
			if (*beg != '(')
				return 0;
			auto ite = beg;
			for (int i = 1; i != 0;)
			{
				++ite;
				if (ite == end)
				{
					return 0;
				}
				if (*ite == ')')
				{
					--i;
				}
				else if (*ite == '(')
				{
					++i;
				}
			}
			++ite;
			return std::distance(beg, ite);
		}

		inline size_t parse_type_case_space(
			std::string::const_iterator beg, 
			std::string::const_iterator end)
		{
			size_t ret = 0;
			for (; *beg == ' '; ++beg, ++ret);
			return ret;
		}

		inline bool parse_type_impl(
			sentence_data const& data, 
			std::string const& line)
		{
			auto ite = line.begin();
			auto end = line.end();
			for (auto const& d : data.sentence)
			{
				if (d.type_ == parse_helper::data_type::token)
				{
					if (ite == end)
						return false;
					size_t shift = parse_type_case_token(ite, end);
					if (!shift)
						return false;
					ite = std::next(ite, shift);
				}
				else if (d.type_ == parse_helper::data_type::string)
				{
					if (ite == end)
						return false;
					if (!string_match(ite, end, d.str_.c_str()))
						return false;
					ite = std::next(ite, d.str_.size());
				}
				else if (d.type_ == parse_helper::data_type::tuple)
				{
					if (ite == end)
						return false;
					size_t shift = parse_type_case_tuple(ite, end);
					if (!shift)
						return false;
					ite = std::next(ite, shift);
				}
				else if (d.type_ == parse_helper::data_type::str_literal)
				{
					if (ite == end)
						return false;
					auto shift = string_literal_check(ite, end);
					if (!shift)
						return false;
					ite = std::next(ite, shift);
				}
				else
				{
					while (true)
					{
						if (ite == end)
							break;
						if (*ite != ' ')
							break;
						++ite;
					}
				}
			}
			return ite == end;
		}

		inline auto parse_type(std::list<sentence_data> const& data, std::string const& line)
			->std::pair<int,decltype(data.begin())>
		{
			int i = 0;
			for (auto d = data.begin(); d != data.end(); ++d)
			{
				if (parse_type_impl(*d, line))
					return std::make_pair(i, d);
				++i;
			}
			return std::make_pair(-1,data.begin());
		}

		//parse
		inline std::pair<size_t,std::string> read_token(std::string::const_iterator beg, std::string::const_iterator end)
		{
			auto ite = beg;
			for (; ite != end && token_string_check(*ite); ++ite);
			return std::make_pair(std::distance(beg,ite), std::string(beg, ite));
		}

		inline std::pair<size_t, std::list<std::string>>
				read_tuple(std::string::const_iterator beg,std::string::const_iterator end)
		{
			auto ite = beg;
			auto sta = std::next(beg);
			std::list<std::string> ret;
			for (int i = 1; i != 0;)
			{
				++ite;
				if (ite == end)
				{
					return std::make_pair(std::distance(beg, ite), ret);
				}
				if (*ite == '(')
				{
					++i;
				}
				else if (*ite == ')')
				{
					--i;
				}
				else if (i == 1 && *ite == ',')
				{
					ret.emplace_back(sta, ite);
					sta = std::next(ite);
				}
			}
			ret.emplace_back(sta, ite);
			++ite;
			return std::make_pair(std::distance(beg, ite), ret);
		}

		inline std::pair<size_t, std::string> read_str_lite(std::string::const_iterator beg,std::string::const_iterator end)
		{
			auto shift = string_literal_check(beg, end);
			auto e = std::next(beg, shift - 1);
			return std::make_pair(shift, std::string(std::next(beg, 1), e));
		}

		inline parse_result parse(sentence_data const& data, std::string const& line)
		{
			auto beg = line.begin();
			auto end = line.end();
			parse_result ret;
			ret.is_valid = true;
			for (const auto& d : data.sentence)
			{
				if (d.type_ == parse_helper::data_type::token)
				{
					auto value = read_token(beg, end);
					ret.tokens_.push_back(std::get<1>(value));
					beg = std::next(beg, std::get<0>(value));
				}
				else if (d.type_ == parse_helper::data_type::string)
				{
					beg = std::next(beg, d.str_.size());
				}
				else if (d.type_ == parse_helper::data_type::tuple)
				{
					auto value = read_tuple(beg, end);
					ret.tuples_.push_back(std::get<1>(value));
					beg = std::next(beg, std::get<0>(value));
				}
				else if (d.type_ == parse_helper::data_type::str_literal)
				{
					auto value = read_str_lite(beg, end);
					ret.str_lites_.push_back(std::get<1>(value));
					beg = std::next(beg, std::get<0>(value));
				}
				else
				{
					for (; beg != end && *beg == ' '; ++beg);
				}
			}
			return ret;
		}
	}

	inline parse_result parse_line(std::list<sentence_data> const& data, std::string line)
	{
		auto type = detail::parse_type(data, line);
		if (std::get<0>(type)==-1)
		{
			parse_result ret;
			ret.is_valid = false;
			return ret;
		}
		auto ret= detail::parse(*std::get<1>(type), line);
		ret.type_num = std::get<0>(type);
		return ret;
	}
}
