//
// MIT License
//
// Copyright (c) 2023 Alexandre Arsenault
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#pragma once

#include "fst/utility.h"
#include "fst/memory.h"
#include "fst/pointer.h"
#include "fst/array.h"

FST_BEGIN_NAMESPACE
    namespace detail
    {
        enum class function_type : uint8_t {
            empty,
            pointer,
            static_functor,
            static_trivial_functor,
            dynamic_functor
            //member_function
        };

        // clang-format off
        template <class F> struct ftraits;
        template <class F> struct ftraits<F&> : ftraits<F> {};
        template <class F> struct ftraits<F&&> : ftraits<F> {};
        template <class R, class... _Args> struct ftraits<R (*)(_Args...)> : ftraits<R(_Args...)> {};
        template <class C, class R, class... _Args> struct ftraits<R (C::*)(_Args...)> : ftraits<R(C&, _Args...)> {};
        template <class C, class R, class... _Args> struct ftraits<R (C::*)(_Args...) const> : ftraits<R(C&, _Args...)> {};
        template <class C, class R> struct ftraits<R C::*> : ftraits<R(C&)> {};
        template <class F> struct ftraits : ftraits<decltype(&F::operator())> {};
        // clang-format on

        template <class R, class... _Args>
        struct ftraits<R(_Args...)>
        {
            struct vfunction_base
            {
                virtual ~vfunction_base() noexcept = default;
                virtual R call(_Args...) noexcept = 0;
                virtual size_t size() const noexcept = 0;
                virtual void copy(void* dst, void* src) noexcept = 0;
                virtual void move(void* dst, void* src) noexcept = 0;
            };

            template <class _FctChild, class Enable = void>
            struct vfunction : vfunction_base
            {
                inline vfunction(_FctChild&& f) noexcept
                    : _fct(__fst::forward<_FctChild>(f))
                {}

                virtual ~vfunction() noexcept override = default;

                virtual R call(_Args... args) noexcept override { return _fct(args...); }

                virtual size_t size() const noexcept override { return sizeof(vfunction); }

                virtual void copy(void* dst, void* src) noexcept override
                {
                    _FctChild* d = (_FctChild*) dst;
                    _FctChild* s = (_FctChild*) src;
                    fst_placement_new(d) _FctChild(*s);
                }

                virtual void move(void* dst, void* src) noexcept override
                {
                    _FctChild* d = (_FctChild*) dst;
                    _FctChild* s = (_FctChild*) src;
                    fst_placement_new(d) _FctChild(__fst::move(*s));
                }

                _FctChild _fct;
            };

            template <class _FctChild>
            struct vfunction<_FctChild, __fst::enable_if_t<__fst::is_member_function_pointer_v<_FctChild>>> : vfunction_base
            {
                static_assert(__fst::always_false<_FctChild>, "error");
            };

            //
            struct function_base
            {
                static constexpr size_t data_size = 31;
                using cb_function_type = R (*)(_Args...);
                function_base() noexcept = default;

                template <class _T>
                inline function_base(R (_T::*fct)(_Args...), _T* obj) noexcept
                {
                    auto wfct = [=](_Args... args) -> R
                    {
                        return ((*obj).*(fct))(args...);
                    };

                    using ftype = vfunction<decltype(wfct)>;

                    fst_placement_new(_data.data()) ftype(__fst::move(wfct));
                    type() = function_type::static_trivial_functor;
                }

                FST_PRAGMA_PUSH()
                FST_PRAGMA_DISABLE_WARNING_MSVC(4061)
                FST_PRAGMA_DISABLE_WARNING_MSVC(4062)
                inline R operator()(_Args... args) const noexcept
                {

                    switch (type())
                    {
                    case function_type::pointer: {
                        cb_function_type f = *(cb_function_type*) _data.data();
                        return f(__fst::forward<_Args>(args)...);
                    }

                    case function_type::dynamic_functor: {
                        return dynamic_base()->call(__fst::forward<_Args>(args)...);
                    }

                    case function_type::static_functor:
                    case function_type::static_trivial_functor: {
                        return static_base()->call(__fst::forward<_Args>(args)...);
                    }
                    default: return R();
                    }
                }
                FST_PRAGMA_POP()

                inline vfunction_base*& dynamic_base() const noexcept { return ((vfunction_base**) _data.data())[0]; }

                inline vfunction_base* static_base() const noexcept { return (vfunction_base*) _data.data(); }

                inline function_type& type() noexcept { return _type; }
                inline function_type type() const noexcept { return _type; }

                __fst::array<__fst::byte, data_size> _data;
                function_type _type = function_type::empty;
            };
        };
    } // namespace detail

    template <class _Fct>
    class function : private detail::ftraits<_Fct>::function_base
    {
        using base_type = typename detail::ftraits<_Fct>::function_base;
        using fct_type = __fst::decay_t<_Fct>;

        using function_type = detail::function_type;
        using base_type::data_size;
        using base_type::_data;
        using base_type::type;
        using base_type::dynamic_base;
        using base_type::static_base;
        using vfunction_base = typename detail::ftraits<_Fct>::vfunction_base;

        template <class _ChildFct>
        using vfunction = typename detail::ftraits<_Fct>::template vfunction<_ChildFct>;

      public:
        inline function() noexcept = default;

        inline function(fct_type&& fct) noexcept
        {
            fct_type& ft = *(fct_type*) _data.data();
            ft = __fst::forward<fct_type>(fct);
            type() = function_type::pointer;
        }

        template <class _Func, class _T, __fst::enable_if_t<__fst::is_member_function_pointer_v<_Func>, int> = 0>
        inline function(_Func&& fct, _T* obj) noexcept
            : base_type(fct, obj)
        {
            //using ftype = vfunction<_Func>;
        }

        template <class _Func, __fst::enable_if_t<!__fst::is_member_function_pointer_v<_Func>, int> = 0>
        inline function(_Func&& fct) noexcept
        {
            using ftype = vfunction<_Func>;

            if constexpr (sizeof(ftype) > data_size)
            {
                ftype* fct_base = (ftype*) __fst::allocate(sizeof(ftype));
                fst_assert(fct_base, "allocation failed");
                fct_base = fst_placement_new(fct_base) ftype(__fst::forward<_Func>(fct));
                dynamic_base() = fct_base;
                type() = function_type::dynamic_functor;
            }
            else if constexpr (__fst::is_trivially_copyable_v<_Func>)
            {
                fst_placement_new(_data.data()) vfunction<_Func>(__fst::forward<_Func>(fct));
                type() = function_type::static_trivial_functor;
            }
            else
            {
                fst_placement_new(_data.data()) vfunction<_Func>(__fst::forward<_Func>(fct));
                type() = function_type::static_functor;
            }
        }

        inline function(const function& other) noexcept
            : base_type()
        {
            type() = other.type();
            switch (type())
            {
            case function_type::pointer:
            case function_type::static_trivial_functor: {
                _data = other._data;
                return;
            }

            case function_type::dynamic_functor: {
                vfunction_base* other_base_ptr = other.dynamic_base();
                size_t size = other_base_ptr->size();

                vfunction_base* self_base_ptr = (vfunction_base*) __fst::allocate(size);
                fst_assert(self_base_ptr, "allocation failed");

                other_base_ptr->copy(self_base_ptr, other_base_ptr);
                dynamic_base() = self_base_ptr;
                return;
            }

            case function_type::static_functor: {
                vfunction_base* df = (vfunction_base*) _data.data();
                vfunction_base* f = (vfunction_base*) other._data.data();
                f->copy(df, f);
                return;
            }
            }
        }

        inline function(function&& other) noexcept
        {
            type() = other.type();
            switch (type())
            {
            case function_type::pointer:
            case function_type::static_trivial_functor:
            case function_type::dynamic_functor: {
                _data = other._data;
                other.type() = function_type::empty;
                return;
            }

            case function_type::static_functor: {
                vfunction_base* df = static_base();
                vfunction_base* f = other.static_base();
                f->move(df, f);
                f->~vfunction_base();
                other.type() = function_type::empty;
                return;
            }
            }
        }

        inline ~function() noexcept { reset(); }

        inline function& operator=(const function& other) noexcept
        {
            reset();
            type() = other.type();

            switch (type())
            {
            case function_type::pointer:
            case function_type::static_trivial_functor: {
                _data = other._data;
                return *this;
            }

            case function_type::dynamic_functor: {
                vfunction_base* other_base_ptr = other.dynamic_base();
                size_t size = other_base_ptr->size();

                vfunction_base* self_base_ptr = (vfunction_base*) __fst::allocate(size);
                fst_assert(self_base_ptr, "allocation failed");
                other_base_ptr->copy(self_base_ptr, other_base_ptr);
                dynamic_base() = self_base_ptr;
                return *this;
            }

            case function_type::static_functor: {
                vfunction_base* df = (vfunction_base*) _data.data();
                vfunction_base* f = (vfunction_base*) other._data.data();
                f->copy(df, f);
                return *this;
            }
            }

            return *this;
        }

        inline function& operator=(function&& other) noexcept
        {
            reset();
            type() = other.type();

            switch (type())
            {
            case function_type::pointer:
            case function_type::static_trivial_functor:
            case function_type::dynamic_functor: {
                _data = other._data;
                other.type() = function_type::empty;
                return *this;
            }

            case function_type::static_functor: {
                vfunction_base* df = static_base();
                vfunction_base* f = other.static_base();
                f->move(df, f);
                f->~vfunction_base();
                other.type() = function_type::empty;
                return *this;
            }
            }

            return *this;
        }

        using base_type::operator();

      private:
        inline void reset() noexcept
        {
            switch (__fst::exchange(type(), function_type::empty))
            {
            case function_type::dynamic_functor: {
                vfunction_base* f = dynamic_base();
                fst_assert(f, "tried to destroy nullptr");
                f->~vfunction_base();
                __fst::deallocate(f);
                return;
            }

            case function_type::static_functor: {
                static_base()->~vfunction_base();
                return;
            }
            }
        }
    };

FST_END_NAMESPACE
