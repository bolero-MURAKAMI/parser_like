//DYZ parser generator
//parser.hpp
//(c)DtYaZsK 2014

#pragma once

#include<string>
#include<list>
#include<type_traits>

namespace dyz_parser
{
	namespace parse_helper
	{
		struct token_holder_type{};
		struct tuple_holder_type{};
		struct space_holder_type{};
		struct str_literal_type{};

		enum class data_type
		{
			token, string, tuple, str_literal, space
		};

		struct data
		{
			std::string str_;
			data_type type_;
			data(token_holder_type) :type_(data_type::token){}
			data(tuple_holder_type) :type_(data_type::tuple){}
			data(space_holder_type) :type_(data_type::space){}
			data(str_literal_type) :type_(data_type::str_literal){}
			data(std::string str) :str_(str), type_(data_type::string){}
		};
	}

	namespace placeholder
	{
		const parse_helper::token_holder_type token_holder;
		const parse_helper::tuple_holder_type tuple_holder;
		const parse_helper::space_holder_type space_holder;
		const parse_helper::str_literal_type string_literal_holder;
	}

	struct sentence_data
	{
		std::list<parse_helper::data> sentence;
		template<class T, class... Ts>void init_impl(T arg, Ts... args)
		{
			init_impl(args...);
			sentence.emplace_front(arg);
		}
		void init_impl()
		{

		}
		template<class... Ts>void init(Ts... args)
		{
			init_impl(args...);
			if (sizeof...(Ts) <= 1)
				return;
			auto ite = sentence.begin();
			auto before = sentence.begin();
			++ite;
			auto end = sentence.end();
			for (; ite != end; ++ite)
			{
				before = std::prev(ite);
				if ((*before).type_ == parse_helper::data_type::token &&
					(*ite).type_ == parse_helper::data_type::token)
				{
					sentence.emplace(ite, " ");
					sentence.emplace(ite, placeholder::space_holder);
				}
				else if ((*before).type_ == parse_helper::data_type::token &&
					(*ite).type_ == parse_helper::data_type::string&&
					token_string_check(*(*ite).str_.c_str()))
				{
					sentence.emplace(ite, " ");
					sentence.emplace(ite, placeholder::space_holder);
				}
				else if((*ite).type_ == parse_helper::data_type::token &&
					(*before).type_ == parse_helper::data_type::string &&
					token_string_check(*(std::prev((*before).str_.end()))))
				{
					sentence.emplace(ite, " ");
					sentence.emplace(ite, placeholder::space_holder);
				}
			}
		}

		template<class... Ts>sentence_data(Ts... arg)
		{
			init(arg...);
		}
	};
}
