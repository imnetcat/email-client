﻿#pragma once
#include "../config.h"
#ifdef INDEBUG
#include "../exception.h"
#include "set_ostream.h"
#include "map_ostream.h"
#include "vec_ostream.h"
#include <iostream>
#include <sstream>
#include <exception>
#include <functional>
namespace Core
{
	namespace Testing
	{
		bool operator ==(const std::exception& lhs, const std::exception& rhs);
		bool operator ==(const Exceptions::base& lhs, const Exceptions::base& rhs);

		template<typename ExceptType, class Func, typename... Args>
		bool AssertExceptions(ExceptType expected, Func f, Args... args)
		{
			bool flag = false;
			try
			{
				f(args...);
			}
			catch (const ExceptType except)
			{
				if (except == expected)
					flag = true;
			}
			catch(...) { }

			return flag;
		}

		template<typename ExceptType>
		bool AssertExceptions(ExceptType expected, std::function<void()> f)
		{
			bool flag = false;
			try
			{
				f();
			}
			catch (const ExceptType except)
			{
				if(except == expected)
					flag = true;
			}
			catch (...) {}

			return flag;
		}
	}
}
#endif