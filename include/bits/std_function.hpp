/*	Copyright (C) 2018 Anton Mikhaylov aka anion155
	This file is part of the uClibc++ Library.
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#pragma once

#include <basic_definitions>
#include <memory>

namespace std {
	/** @class std::function functional
	 * 
	 * Class template std::function is a general-purpose polymorphic function wrapper.
	 * Instances of std::function can store, copy, and invoke any Callable target -- functions,
	 * lambda expressions, bind expressions, or other function objects,
	 * as well as pointers to member functions and pointers to data members.
	 * 
	 * The stored callable object is called the target of std::function.
	 * If a std::function contains no target, it is called empty.
	 * Invoking the target of an empty std::function results in std::bad_function_call exception being thrown.
	 * std::function satisfies the requirements of CopyConstructible and CopyAssignable.
	 * 
	 * @since c++11
	 */
	template <class> class _UCXXEXPORT function; /*undefined*/

	// DELETEME: use std::tuple instead
	namespace detail {
		template <class ...Args> struct maybe_binary_or_unary_function { };
		template <class Arg> struct maybe_binary_or_unary_function<Arg> {
			using argument_type = Arg;
		};
		template <class Arg1, class Arg2> struct maybe_binary_or_unary_function<Arg1, Arg2> {
			using first_argument_type = Arg1;
			using second_argument_type = Arg2;
		};
	}

	template <class R, class ...Args> class function<R(Args...)>
		: detail::maybe_binary_or_unary_function<Args...> {
	public:
		using result_type = R;

		// DELETEME: use std::invoke instead
		class invocable {
    public:
      virtual ~invocable() = default;
      virtual R invoke(Args &&...) = 0;
    };
  	template <class F> class functor : public invocable {
      F _f;
    public:
      functor(const F &f) : _f(f) { }
      functor(F &&f) : _f(move(f)) { }
      ~functor() override = default;
      R invoke(Args &&...args) override { return _f(forward<Args>(args)...); }
    };

		unique_ptr<invocable> _target;
	};
}
