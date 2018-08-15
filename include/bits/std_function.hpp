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
#include <cstddef>
#include <typeinfo>

namespace std {
	/** @class std::bad_function_call functional
	 * @brief std::bad_function_call is the type of the exception thrown by std::function::operator()
	 *   if the function wrapper has no target.
	 * 
	 * @since c++11
	 */
	class bad_function_call : public exception {
	public:
		~bad_function_call() override = default;
		const char* what() const
		#if __is_cpp_std_11
			noexcept
		#endif
		override { return "bad function call"; }
	};

	namespace detail {
#if !__is_cpp_std_17
	// DELETEME: use std::tuple instead
		template <class ...Args> struct maybe_binary_or_unary_function { };
		template <class Arg> struct maybe_binary_or_unary_function<Arg> {
			using argument_type = Arg;
		};
		template <class Arg1, class Arg2> struct maybe_binary_or_unary_function<Arg1, Arg2> {
			using first_argument_type = Arg1;
			using second_argument_type = Arg2;
		};
#endif

		// TODO: implement
		template <class F, class R, class ...Args> struct is_invocable_r : true_type { };
		template <class F, class R, class ...Args>
		inline constexpr bool is_invocable_r_v = is_invocable_r<F, R, Args...>::value;
	}

	/** @class std::function functional
	 * @brief Class template std::function is a general-purpose polymorphic function wrapper.
	 * 
	 * Instances of std::function can store, copy, and invoke any Callable target -- functions,
	 * lambda expressions, bind expressions, or other function objects,
	 * as well as pointers to member functions and pointers to data members.
	 * 
	 * The stored callable object is called the target of std::function.
	 * If a std::function contains no target, it is called empty.
	 * Invoking the target of an empty std::function results in std::bad_function_call exception being thrown.
	 * std::function satisfies the requirements of CopyConstructible and CopyAssignable.
	 * 
	 * @tparam F - function type
	 * 
	 * @since c++11
	 */
	template <class F> class _UCXXEXPORT function; /*undefined*/
	template <class R, class ...Args> class function<R(Args...)>
	#if !__is_cpp_std_17
		: detail::maybe_binary_or_unary_function<Args...>
	#endif
	{
		// DELETEME: use std::invoke instead
		class invocable {
		public:
			virtual ~invocable() = default;
			virtual invocable *copy() const = 0;
			virtual const type_info& target_type() const noexcept = 0;
			virtual void *target() noexcept = 0;
			virtual const void *target() const noexcept = 0;
			virtual R invoke(Args &&...) const = 0;
		};
		template <class F> class functor : public invocable {
			F _f;
		public:
			using functor_type = F;
			functor(const F &f) : _f(f) { }
			functor(F &&f) : _f(std::move(f)) { }
			~functor() override = default;
			functor *copy() const override { return new functor(F(_f)); }
			const type_info& target_type() const noexcept override { return typeid(_f); }
			void *target() noexcept override { return reinterpret_cast<void *>(&_f); }
			const void *target() const noexcept { return reinterpret_cast<const void *>(&_f); }
			R invoke(Args &&...args) const override { return _f(forward<Args>(args)...); }
		};

		unique_ptr<invocable> _target;

	public:
		using result_type = R;
		
		// @brief Creates an empty function.
		function(nullptr_t p = nullptr) noexcept : _target(nullptr) { }
		// @brief Copies the target of other to the target of *this.
		function(const function &other) { *this = other; }
		// @brief Moves the target of other to the target of *this.
		function(function &&other) { *this = std::move(other); }
		/* @brief Initializes the target with std::move(f).
		 * @tparam F - must meet the requirements of Callable.
		 */
		template <class F, class = enable_if_t<
			detail::is_invocable_r_v<F, R, Args...>
		>> function(F f) {
			_target.reset(new functor<F>(std::move(f)));
		}

		// @brief Assigns a copy of target of other.
		function &operator =(const function &other) {
			_target = other._target->copy();
			return *this;
		}
		// @brief Moves the target of other to *this.
		function &operator =(function &&other) {
			_target = other._target->copy();
			other._target = nullptr;
			return *this;
		}
		// @brief Drops the current target.
		function &operator =(nullptr_t) {
			_target = nullptr;
			return *this;
		}
		/* @brief Sets the target of *this to the callable f.
		 * @tparam F - must meet the requirements of Callable.
		 */
		template <class F> enable_if_t<
			detail::is_invocable_r_v<F, R, Args...>, function &
		> operator =(F f) {
			_target = make_unique<functor<F>>(f);
			return *this;
		}
		/* @brief Sets the target of *this to a copy of f.
		 * @tparam F - must meet the requirements of Callable.
		 */
		template <class F> enable_if_t<
			detail::is_invocable_r_v<F, R, Args...>, function &
		> operator =(reference_wrapper<F> f) {
			_target = reference_wrapper<F>(f);
			return *this;
		}

		// @brief Exchanges the stored callable objects of *this and other.
		void swap(function &other) noexcept {
			_target.swap(other._target);
		}
	#if !__is_cpp_std_14
		// TODO: assign with alloc
		// @brief Initializes the target with f.
		template <class F/*, class Alloc*/> void assign(F &&f/*, const Alloc& alloc*/) {
			function(forward<F>(f)).swap(*this);
		}
	#endif

		// @brief Checks whether *this stores a callable function target, i.e. is not empty.
		explicit operator bool() const noexcept {
			return _target != nullptr;
		}

		// @brief Invokes the stored callable function target with the parameters args.
		R operator()(Args &&...args) const {
			if (_target == nullptr) {
				throw bad_function_call();
			}
			return _target->invoke(forward<Args>(args)...);
		}

		// @brief Returns the type of the stored function.
		const type_info& target_type() const noexcept {
			return _target != nullptr ? _target->target_type() : typeid(void);
		}

		/* @brief Returns a pointer to the stored callable function target.
		 * @tparam T - must be equal to stored function.
		 */
		template <class T> T *target() noexcept {
			if (_target != nullptr && typeid(T) == target_type()) {
				return reinterpret_cast<T *>(_target->target());
			} else {
				return nullptr;
			}
		}
		template <class T> const T *target() const noexcept {
			if (_target != nullptr && typeid(T) == target_type()) {
				return reinterpret_cast<const T *>(_target->target());
			} else {
				return nullptr;
			}
		}
	};
	template <class R, class ...Args> class function<R(*)(Args...)> : public function<R(Args...)> {
	public:
		using function<R(Args...)>::function;
	};
	template <class F, class R, class ...Args> class function<R(F::*)(Args...)> : public function<R(Args...)> {
	public:
		using function<R(Args...)>::function;
	};

	template <class R, class ...Args> void swap(function<R(Args...)> &lhs, function<R(Args...)> &rhs) {
		lhs.swap(rhs);
	}
}

template <class R, class ...Args> bool operator ==(const std::function<R(Args...)> &f, std::nullptr_t) noexcept {
	return !static_cast<bool>(f);
}
template <class R, class ...Args> bool operator ==(std::nullptr_t, const std::function<R(Args...)> &f) noexcept {
	return !static_cast<bool>(f);
}
template <class R, class ...Args> bool operator !=(const std::function<R(Args...)> &f, std::nullptr_t) noexcept {
	return static_cast<bool>(f);
}
template <class R, class ...Args> bool operator !=(std::nullptr_t, const std::function<R(Args...)> &f) noexcept {
	return static_cast<bool>(f);
}
