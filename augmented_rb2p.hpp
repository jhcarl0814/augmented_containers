#ifndef AUGMENTED_RB2P_HPP
#define AUGMENTED_RB2P_HPP

#include <utility>
#include <algorithm>
#include <ranges>
#include <memory>
#include <optional>
#include <array>
#include <unordered_set>
#include <coroutine>
#include <functional>
#include <variant>
#include <vector>
#include <iostream>
#include <map>

namespace augmented_containers
{
#ifndef AUGMENTED_CONTAINERS_AUGMENTED_SEQUENCE_ENUM
    #define AUGMENTED_CONTAINERS_AUGMENTED_SEQUENCE_ENUM
    enum class augmented_sequence_physical_representation_e { rb3p,
        rb2p,
        finger_tree,
    };

    enum class augmented_sequence_size_management_e { no_size,
        at_node_end,
        at_each_node_except_node_end,
    };
#endif // AUGMENTED_CONTAINERS_AUGMENTED_SEQUENCE_ENUM

    namespace detail
    {
        namespace language
        {
#ifndef AUGMENTED_CONTAINERS_LANGUAGE_POINTER_TRAITS_CAST
    #define AUGMENTED_CONTAINERS_LANGUAGE_POINTER_TRAITS_CAST
            template<typename target_pointer_t, typename source_pointer_t>
            target_pointer_t pointer_traits_static_cast(source_pointer_t source_pointer)
            {
                return std::pointer_traits<target_pointer_t>::pointer_to(*static_cast<typename std::pointer_traits<target_pointer_t>::element_type *>(std::to_address(source_pointer)));
            }

            template<typename target_pointer_t, typename source_pointer_t>
            target_pointer_t pointer_traits_reinterpret_cast(source_pointer_t source_pointer) { return std::pointer_traits<target_pointer_t>::pointer_to(*reinterpret_cast<typename std::pointer_traits<target_pointer_t>::element_type *>(std::to_address(source_pointer))); }
#endif // AUGMENTED_CONTAINERS_LANGUAGE_POINTER_TRAITS_CAST

#ifndef AUGMENTED_CONTAINERS_LANGUAGE_TAGGED_PTR_BIT0
    #define AUGMENTED_CONTAINERS_LANGUAGE_TAGGED_PTR_BIT0
            template<typename pointer_t>
            bool tagged_ptr_bit0_is_setted(pointer_t p)
            {
                return (reinterpret_cast<uintptr_t>(std::to_address(p)) & 0b1) != 0;
            };
            template<typename pointer_t>
            auto tagged_ptr_bit0_unsetted_relaxed(pointer_t p) { return std::pointer_traits<pointer_t>::pointer_to(*reinterpret_cast<typename std::pointer_traits<pointer_t>::element_type *>(reinterpret_cast<uintptr_t>(std::to_address(p)) & ~0b1)); }
            template<typename pointer_t>
            auto tagged_ptr_bit0_unsetted(pointer_t p) { return assert(tagged_ptr_bit0_is_setted(p)), tagged_ptr_bit0_unsetted_relaxed(p); }
            template<typename pointer_t>
            auto tagged_ptr_bit0_setted_relaxed(pointer_t p) { return std::pointer_traits<pointer_t>::pointer_to(*reinterpret_cast<typename std::pointer_traits<pointer_t>::element_type *>(reinterpret_cast<uintptr_t>(std::to_address(p)) | 0b1)); }
            template<typename pointer_t>
            auto tagged_ptr_bit0_setted(pointer_t p) { return assert(!tagged_ptr_bit0_is_setted(p)), tagged_ptr_bit0_setted_relaxed(p); }
#endif // AUGMENTED_CONTAINERS_LANGUAGE_TAGGED_PTR_BIT0

#ifndef AUGMENTED_CONTAINERS_LANGUAGE_TAGGED_PTR_BIT1
    #define AUGMENTED_CONTAINERS_LANGUAGE_TAGGED_PTR_BIT1
            template<typename pointer_t>
            bool tagged_ptr_bit1_is_setted(pointer_t p)
            {
                return (reinterpret_cast<uintptr_t>(std::to_address(p)) & 0b10) != 0;
            };
            template<typename pointer_t>
            auto tagged_ptr_bit1_unsetted_relaxed(pointer_t p) { return std::pointer_traits<pointer_t>::pointer_to(*reinterpret_cast<typename std::pointer_traits<pointer_t>::element_type *>(reinterpret_cast<uintptr_t>(std::to_address(p)) & ~0b10)); }
            template<typename pointer_t>
            auto tagged_ptr_bit1_unsetted(pointer_t p) { return assert(tagged_ptr_bit1_is_setted(p)), tagged_ptr_bit1_unsetted_relaxed(p); }
            template<typename pointer_t>
            auto tagged_ptr_bit1_setted_relaxed(pointer_t p) { return std::pointer_traits<pointer_t>::pointer_to(*reinterpret_cast<typename std::pointer_traits<pointer_t>::element_type *>(reinterpret_cast<uintptr_t>(std::to_address(p)) | 0b10)); }
            template<typename pointer_t>
            auto tagged_ptr_bit1_setted(pointer_t p) { return assert(!tagged_ptr_bit1_is_setted(p)), tagged_ptr_bit1_setted_relaxed(p); }
#endif // AUGMENTED_CONTAINERS_LANGUAGE_TAGGED_PTR_BIT1

#ifndef AUGMENTED_CONTAINERS_LANGUAGE_TAGGED_PTR_BIT2
    #define AUGMENTED_CONTAINERS_LANGUAGE_TAGGED_PTR_BIT2
            template<typename pointer_t>
            bool tagged_ptr_bit2_is_setted(pointer_t p)
            {
                return (reinterpret_cast<uintptr_t>(std::to_address(p)) & 0b100) != 0;
            };
            template<typename pointer_t>
            auto tagged_ptr_bit2_unsetted_relaxed(pointer_t p) { return std::pointer_traits<pointer_t>::pointer_to(*reinterpret_cast<typename std::pointer_traits<pointer_t>::element_type *>(reinterpret_cast<uintptr_t>(std::to_address(p)) & ~0b100)); }
            template<typename pointer_t>
            auto tagged_ptr_bit2_unsetted(pointer_t p) { return assert(tagged_ptr_bit2_is_setted(p)), tagged_ptr_bit2_unsetted_relaxed(p); }
            template<typename pointer_t>
            auto tagged_ptr_bit2_setted_relaxed(pointer_t p) { return std::pointer_traits<pointer_t>::pointer_to(*reinterpret_cast<typename std::pointer_traits<pointer_t>::element_type *>(reinterpret_cast<uintptr_t>(std::to_address(p)) | 0b100)); }
            template<typename pointer_t>
            auto tagged_ptr_bit2_setted(pointer_t p) { return assert(!tagged_ptr_bit2_is_setted(p)), tagged_ptr_bit2_setted_relaxed(p); }

            template<typename pointer_t>
            uintptr_t tagged_ptr_bit210_get(pointer_t p) { return reinterpret_cast<uintptr_t>(std::to_address(p)) & 0b111; };
            template<typename pointer_t>
            auto tagged_ptr_bit210_set(pointer_t p, uintptr_t bit210) { return std::pointer_traits<pointer_t>::pointer_to(*reinterpret_cast<typename std::pointer_traits<pointer_t>::element_type *>((reinterpret_cast<uintptr_t>(std::to_address(p)) & ~0b111) | bit210)); };
#endif // AUGMENTED_CONTAINERS_LANGUAGE_TAGGED_PTR_BIT2

#ifndef AUGMENTED_CONTAINERS_LANGUAGE_LITERALS
    #define AUGMENTED_CONTAINERS_LANGUAGE_LITERALS
            constexpr std::size_t zu(std::size_t v)
            {
                return v;
            }
            constexpr std::ptrdiff_t z(std::ptrdiff_t v) { return v; }
#endif // AUGMENTED_CONTAINERS_LANGUAGE_LITERALS
        } // namespace language

#ifndef AUGMENTED_CONTAINERS_CONCEPTS
    #define AUGMENTED_CONTAINERS_CONCEPTS
        namespace concepts
        {
            template<typename F, typename Ret, typename... Args>
            concept invocable_r = std::invocable<F, Args...> && (std::same_as<Ret, void> || std::convertible_to<std::invoke_result_t<F, Args...>, Ret>)/*&& !
            reference_converts_from_temporary_v<Ret, std::invoke_result_t<F, Args...>>*/
                ; // https://stackoverflow.com/questions/61932900/c-template-function-specify-argument-type-of-callback-functor-lambda-while-st#comment109544863_61933163
        } // namespace concepts
#endif // AUGMENTED_CONTAINERS_CONCEPTS

#ifndef AUGMENTED_CONTAINERS_UTILITY
    #define AUGMENTED_CONTAINERS_UTILITY
        namespace utility
        {
            template<typename T>
            constexpr T &unmove(T &&t) { return static_cast<T &>(t); } //https://stackoverflow.com/a/67059296/8343353

            template<bool is_const = true, typename T = void>
            using conditional_const_t = std::conditional_t<is_const, const T, T>;

            template<bool is_const = true, typename T = void>
            constexpr conditional_const_t<is_const, T> &conditional_as_const(T &_Val) noexcept { return _Val; }
            template<bool is_const = true, typename T = void>
            void conditional_as_const(T const &&) = delete;

            template<typename list_t, typename element_t>
            struct list_find_first_index
            {
                template<std::size_t I>
                struct iteration: std::conditional_t<std::is_same_v<typename std::tuple_element_t<I, list_t>, element_t>, std::type_identity<std::integral_constant<std::size_t, I>>, iteration<I + 1>>::type
                {
                };
                template<>
                struct iteration<std::tuple_size_v<list_t>>: std::integral_constant<std::size_t, std::tuple_size_v<list_t>>
                {
                };
                using type = typename iteration<0>::type;
            };
            template<typename list_t, typename element_t>
            using list_find_first_index_t = typename list_find_first_index<list_t, element_t>::type;

            template<typename list_t, template<typename element_t> typename transformer_t>
            struct list_transform
            {
                template<typename list_t_>
                struct iterations;
                template<typename... elements_t>
                struct iterations<std::tuple<elements_t...>>: std::type_identity<std::tuple<typename transformer_t<elements_t>::type...>>
                {
                };
                using type = typename iterations<list_t>::type;
            };
            template<typename list_t, template<typename element_t> typename transformer_t>
            using list_transform_t = typename list_transform<list_t, transformer_t>::type;


            template<typename... tuples_t>
            using tuple_cat_t = decltype(std::tuple_cat(std::declval<tuples_t>()...));


            template<typename list_t>
            struct list_without_first
            {
                template<typename index_sequence_t>
                struct impl;
                template<std::size_t... I>
                struct impl<std::index_sequence<I...>>: std::type_identity<std::tuple<std::tuple_element_t<1 + I, list_t>...>>
                {};
                using type = typename impl<std::make_index_sequence<std::tuple_size_v<list_t> - 1>>::type;
            };
            template<typename list_t>
            using list_without_first_t = typename list_without_first<list_t>::type;

            template<typename map_t, template<std::size_t index, typename item_t> typename transformer_t>
            struct map_transform
            {
                template<typename map_t_, typename index_sequence_t>
                struct iterations;
                template<typename... elements_t, std::size_t... I>
                struct iterations<std::tuple<elements_t...>, std::index_sequence<I...>>: std::type_identity<std::tuple<typename transformer_t<I, elements_t>::type...>>
                {
                };
                using type = typename iterations<map_t, std::make_index_sequence<std::tuple_size_v<map_t>>>::type;
            };
            template<typename map_t, template<std::size_t index, typename item_t> typename transformer_t>
            using map_transform_t = typename map_transform<map_t, transformer_t>::type;
        } // namespace utility
#endif // AUGMENTED_CONTAINERS_UTILITY

#ifndef AUGMENTED_CONTAINERS_ITERATOR
    #define AUGMENTED_CONTAINERS_ITERATOR
        namespace iterator
        {
            template<typename wrapped_iterator_t, typename transformer_t>
            struct transform_output_iterator_t
            {
                wrapped_iterator_t wrapped_iterator;
                transformer_t transformer;

                using difference_type = std::ptrdiff_t;
                transform_output_iterator_t &operator++() &
                {
                    ++wrapped_iterator;
                    return *this;
                }
                transform_output_iterator_t operator++(int) &
                {
                    transform_output_iterator_t temp = *this;
                    operator++();
                    return temp;
                }

                template<typename wrapped_iterator_cvref_t>
                struct dereference_proxy_t
                {
                    transform_output_iterator_t const &transform_output_iterator;
                    decltype(auto) operator=(auto &&value) const & { return *const_cast<wrapped_iterator_cvref_t>(transform_output_iterator.wrapped_iterator) = transform_output_iterator.transformer(std::forward<decltype(value)>(value)); }
                };
                dereference_proxy_t<wrapped_iterator_t const &> operator*() const & { return dereference_proxy_t<wrapped_iterator_t const &>{.transform_output_iterator = *this}; }
                dereference_proxy_t<wrapped_iterator_t &> operator*() & { return dereference_proxy_t<wrapped_iterator_t &>{.transform_output_iterator = *this}; }
                dereference_proxy_t<wrapped_iterator_t const &&> operator*() const && { return dereference_proxy_t<wrapped_iterator_t const &&>{.transform_output_iterator = *this}; }
                dereference_proxy_t<wrapped_iterator_t &&> operator*() && { return dereference_proxy_t<wrapped_iterator_t &&>{.transform_output_iterator = *this}; }
            };
    #if __cpp_deduction_guides >= 201907L
    #else
            template<typename wrapped_iterator_t, typename transformer_t>
            transform_output_iterator_t(wrapped_iterator_t, transformer_t) -> transform_output_iterator_t<wrapped_iterator_t, transformer_t>;
    #endif
        } // namespace iterator
#endif // AUGMENTED_CONTAINERS_ITERATOR

#ifndef AUGMENTED_CONTAINERS_COROUTINE
    #define AUGMENTED_CONTAINERS_COROUTINE
        namespace coroutine
        {
            template<typename element_t>
            struct generator_t // https://github.com/cor3ntin/coro_benchmark/blob/main/generator.hpp
            {
                struct promise_t
                {
                    element_t *element = nullptr;
                    std::coroutine_handle<promise_t> continuation = nullptr;
                    std::coroutine_handle<promise_t> root_or_current;
                    std::exception_ptr *exception = nullptr;

                    promise_t()
                        : root_or_current(std::coroutine_handle<promise_t>::from_promise(*this))
                    {}
                    generator_t get_return_object() { return {std::coroutine_handle<promise_t>::from_promise(*this)}; }
                    std::suspend_never initial_suspend() { return {}; }
                    auto final_suspend() noexcept
                    {
                        struct final_awaitable_t
                        {
                            bool await_ready() noexcept { return false; }
                            std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_t> continuation) noexcept
                            {
                                if(continuation.promise().continuation)
                                {
                                    continuation.promise().root_or_current.promise().root_or_current = continuation.promise().continuation;
                                    return continuation.promise().continuation;
                                }
                                else return std::noop_coroutine();
                            }
                            void await_resume() noexcept {}
                        } final_awaitable;
                        return final_awaitable;
                    }
                    void return_void() {}
                    void unhandled_exception()
                    {
                        if(exception == nullptr) throw;
                        else *exception = std::current_exception();
                    }

                    std::suspend_always yield_value(element_t &element)
                    {
                        this->element = std::addressof(element);
                        return {};
                    }
                    std::suspend_always yield_value(element_t &&element)
                    {
                        this->element = std::addressof(element);
                        return {};
                    }
                    auto yield_value(generator_t<element_t> generator)
                    {
                        struct yield_awaitable_t
                        {
                            generator_t<element_t> generator;
                            std::exception_ptr exception;
                            bool await_ready() noexcept { return generator.handle.done(); }
                            std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_t> continuation) noexcept
                            {
                                generator.handle.promise().continuation = continuation;
                                std::coroutine_handle<promise_t> root = continuation.promise().continuation == nullptr ? continuation : continuation.promise().root_or_current;
                                root.promise().root_or_current = generator.handle.promise().root_or_current;
                                for(std::coroutine_handle<promise_t> state = generator.handle.promise().root_or_current; state != continuation; state = state.promise().continuation)
                                    state.promise().root_or_current = root;
                                generator.handle.promise().exception = &exception;
                                return std::noop_coroutine();
                            }
                            void await_resume() noexcept
                            {
                                if(exception)
                                    std::rethrow_exception(std::move(exception));
                            }
                        } yield_awaitable{.generator = std::move(generator)};
                        return yield_awaitable;
                    }
                };
                using promise_type = promise_t;
                std::coroutine_handle<promise_t> handle;
                generator_t()
                    : handle(nullptr)
                {}
                generator_t(std::coroutine_handle<promise_t> handle)
                    : handle(handle)
                {}
                generator_t(generator_t &&other)
                    : handle(std::exchange(other.handle, nullptr))
                {}
                generator_t &operator=(generator_t &&other) &
                {
                    if(this == &other)
                        return;
                    if(handle) handle.destroy();
                    handle = std::exchange(other.handle, nullptr);
                    return *this;
                }
                ~generator_t()
                {
                    if(handle)
                    {
                        //            if(handle.promise().continuation != nullptr)
                        //                handle.promise().root_or_current.promise().root_or_current = handle.promise().continuation;
                        handle.destroy();
                    }
                }
                bool empty() const { return handle.done(); }

                struct iterator_t
                {
                    std::coroutine_handle<promise_t> handle = nullptr;

                    using difference_type = std::ptrdiff_t;
                    iterator_t &operator++()
                    {
                        assert(handle != nullptr);
                        assert(handle.promise().continuation == nullptr);
                        assert(!handle.done());
                        handle.promise().root_or_current.resume();
                        return *this;
                    }
                    iterator_t operator++(int)
                    {
                        iterator_t temp = *this;
                        operator++();
                        return temp;
                    }

                    using value_type = element_t;
                    element_t &operator*() const &
                    {
                        assert(handle != nullptr);
                        assert(handle.promise().continuation == nullptr);
                        return *handle.promise().root_or_current.promise().element;
                    }
                    bool is_end() const
                    {
                        assert(handle != nullptr);
                        assert(handle.promise().continuation == nullptr);
                        return handle.done();
                    }
                    friend bool operator==(iterator_t const &iterator, std::default_sentinel_t const &) { return iterator.is_end(); }

                    using iterator_concept = std::input_iterator_tag;
                };
                iterator_t begin() { return iterator_t{.handle = handle}; }
                std::default_sentinel_t end() { return {}; }
            };
        } // namespace coroutine
#endif // AUGMENTED_CONTAINERS_COROUTINE

#ifndef AUGMENTED_CONTAINERS_FUNCTIONAL
    #define AUGMENTED_CONTAINERS_FUNCTIONAL
        namespace functional
        {

    #ifdef __EMSCRIPTEN__
        }
    }
}

namespace std
{
    template<class R, class F, class... Args>
        requires std::is_invocable_r_v<R, F, Args...>
    constexpr R invoke_r(F &&f, Args &&...args) noexcept(std::is_nothrow_invocable_r_v<R, F, Args...>)
    {
        if constexpr(std::is_void_v<R>)
            std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
        else
            return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    }
} // namespace std

namespace augmented_containers
{
    namespace detail
    {
        namespace functional
        {
    #endif
            template<typename Sig, bool is_no_except = false> // https://www.reddit.com/r/cpp/comments/7svbj7/is_stdfunction_really_the_best_we_can_do_lukas/
            class function_view;
            template<typename R, typename... Args, bool is_no_except>
            class function_view<R(Args...), is_no_except>
            {
              private:
                R(*f)
                (void *, Args &&...) noexcept(is_no_except);
                void *d;

              public:
                function_view() noexcept = default;
                function_view(std::nullptr_t) noexcept
                    : f(nullptr),
                      d(nullptr)
                {}
                function_view &operator=(std::nullptr_t) &noexcept
                {
                    f = nullptr;
                    d = nullptr;
                    return *this;
                };
                function_view(function_view const &) noexcept = default;
                function_view &operator=(function_view const &) &noexcept = default;

                template<typename T>
                    requires(!std::same_as<std::decay_t<T>, function_view> && ((!is_no_except && std::is_invocable_r_v<R, T, Args...>) || (is_no_except && std::is_nothrow_invocable_r_v<R, T, Args...>)) && std::is_object_v<std::remove_reference_t<T>>)
                function_view(T &&t) noexcept
                    : f([](void *d, Args &&...args) noexcept(is_no_except) -> R
                          { return std::invoke_r<R>(std::forward<T>(*static_cast<std::remove_reference_t<T> *>(d)), std::forward<Args>(args)...); }),
                      d(std::addressof(t))
                {}
                template<typename T>
                    requires(!std::same_as<std::decay_t<T>, function_view> && ((!is_no_except && std::is_invocable_r_v<R, T, Args...>) || (is_no_except && std::is_nothrow_invocable_r_v<R, T, Args...>)) && std::is_object_v<std::remove_reference_t<T>>)
                function_view &operator=(T &&t) &noexcept
                {
                    *this = function_view(std::forward<T>(t));
                    return *this;
                }

                function_view(R (&t)(Args...) noexcept(is_no_except)) noexcept
                    : f(reinterpret_cast<R (*)(void *, Args...) noexcept(is_no_except)>(&t)),
                      d(nullptr)
                {}
                function_view &operator=(R (&t)(Args...) noexcept(is_no_except)) &noexcept
                {
                    *this = function_view(t);
                    return *this;
                }

                explicit operator bool() const noexcept { return f != nullptr; }
                R operator()(Args... args) const noexcept(is_no_except)
                {
                    assert(f != nullptr);
                    if(d != nullptr)
                        return f(d, std::forward<Args>(args)...);
                    else
                        return (*reinterpret_cast<R (*)(Args...) noexcept(is_no_except)>(f))(std::forward<Args>(args)...);
                }
            };
            template<typename R, typename... Args>
            function_view(R (*)(Args...)) -> function_view<R(Args...), false>;
            template<typename R, typename... Args>
            function_view(R (*)(Args...) noexcept) -> function_view<R(Args...), true>;
            template<typename R, typename... Args>
            function_view(R (&)(Args...)) -> function_view<R(Args...), false>;
            template<typename R, typename... Args>
            function_view(R (&)(Args...) noexcept) -> function_view<R(Args...), true>;
            template<typename R, typename... Args>
            function_view(R (&&)(Args...)) -> function_view<R(Args...), false>;
            template<typename R, typename... Args>
            function_view(R (&&)(Args...) noexcept) -> function_view<R(Args...), true>;
            template<typename F>
            struct get_signature_from_non_static_function_member;
            template<typename R, typename C, typename... Args>
            struct get_signature_from_non_static_function_member<R (C::*)(Args...)>
            {
                using type = R(Args...);
                static constexpr bool is_no_except = false;
            };
            template<typename R, typename C, typename... Args>
            struct get_signature_from_non_static_function_member<R (C::*)(Args...) noexcept>
            {
                using type = R(Args...);
                static constexpr bool is_no_except = true;
            };
            template<typename R, typename C, typename... Args>
            struct get_signature_from_non_static_function_member<R (C::*)(Args...) const>
            {
                using type = R(Args...);
                static constexpr bool is_no_except = false;
            };
            template<typename R, typename C, typename... Args>
            struct get_signature_from_non_static_function_member<R (C::*)(Args...) const noexcept>
            {
                using type = R(Args...);
                static constexpr bool is_no_except = true;
            };
            template<typename R, typename C, typename... Args>
            struct get_signature_from_non_static_function_member<R (C::*)(Args...) &>
            {
                using type = R(Args...);
                static constexpr bool is_no_except = false;
            };
            template<typename R, typename C, typename... Args>
            struct get_signature_from_non_static_function_member<R (C::*)(Args...) & noexcept>
            {
                using type = R(Args...);
                static constexpr bool is_no_except = true;
            };
            template<typename R, typename C, typename... Args>
            struct get_signature_from_non_static_function_member<R (C::*)(Args...) const &>
            {
                using type = R(Args...);
                static constexpr bool is_no_except = false;
            };
            template<typename R, typename C, typename... Args>
            struct get_signature_from_non_static_function_member<R (C::*)(Args...) const & noexcept>
            {
                using type = R(Args...);
                static constexpr bool is_no_except = true;
            };
            template<typename R, typename C, typename... Args>
            struct get_signature_from_non_static_function_member<R (C::*)(Args...) &&>
            {
                using type = R(Args...);
                static constexpr bool is_no_except = false;
            };
            template<typename R, typename C, typename... Args>
            struct get_signature_from_non_static_function_member<R (C::*)(Args...) && noexcept>
            {
                using type = R(Args...);
                static constexpr bool is_no_except = true;
            };
            template<typename R, typename C, typename... Args>
            struct get_signature_from_non_static_function_member<R (C::*)(Args...) const &&>
            {
                using type = R(Args...);
                static constexpr bool is_no_except = false;
            };
            template<typename R, typename C, typename... Args>
            struct get_signature_from_non_static_function_member<R (C::*)(Args...) const && noexcept>
            {
                using type = R(Args...);
                static constexpr bool is_no_except = true;
            };
            template<typename F>
            function_view(F) -> function_view<typename get_signature_from_non_static_function_member<decltype(&F::operator())>::type, get_signature_from_non_static_function_member<decltype(&F::operator())>::is_no_except>;
            template<typename F>
            struct get_signature_from_static_function_member;
            template<typename R, typename... Args>
            struct get_signature_from_static_function_member<R(Args...)>
            {
                using type = R(Args...);
                static constexpr bool is_no_except = false;
            };
            template<typename R, typename... Args>
            struct get_signature_from_static_function_member<R(Args...) noexcept>
            {
                using type = R(Args...);
                static constexpr bool is_no_except = true;
            };
            template<typename F>
            function_view(F) -> function_view<typename get_signature_from_static_function_member<decltype(F::operator())>::type, get_signature_from_static_function_member<decltype(F::operator())>::is_no_except>;
        } // namespace functional
#endif // AUGMENTED_CONTAINERS_FUNCTIONAL

        namespace augmented_sequence_rb2p
        {
            using namespace language;
            using namespace concepts;
            using namespace utility;
            using namespace coroutine;
            using namespace functional;

            template<typename pointer_element_t, typename accumulated_storage_t, typename derived_t>
            struct add_accumulated_storage_member_t
            {
                alignas(accumulated_storage_t) std::byte accumulated_storage_buffer[sizeof(accumulated_storage_t)]; // accumulated_storage_t accumulated_storage;
                typename std::pointer_traits<pointer_element_t>::template rebind<accumulated_storage_t> p_accumulated_storage() { return std::pointer_traits<typename std::pointer_traits<pointer_element_t>::template rebind<accumulated_storage_t>>::pointer_to(*reinterpret_cast<accumulated_storage_t *>(&accumulated_storage_buffer)); }

                static typename std::pointer_traits<pointer_element_t>::template rebind<derived_t const> from_accumulated_storage_pointer(typename std::pointer_traits<pointer_element_t>::template rebind<accumulated_storage_t const> pointer) { return std::pointer_traits<typename std::pointer_traits<pointer_element_t>::template rebind<derived_t const>>::pointer_to(*reinterpret_cast<derived_t const *>(reinterpret_cast<std::byte const *>(std::to_address(pointer)) - offsetof(derived_t, accumulated_storage_buffer))); }
                static typename std::pointer_traits<pointer_element_t>::template rebind<derived_t> from_accumulated_storage_pointer(typename std::pointer_traits<pointer_element_t>::template rebind<accumulated_storage_t> pointer) { return std::pointer_traits<typename std::pointer_traits<pointer_element_t>::template rebind<derived_t>>::pointer_to(*reinterpret_cast<derived_t *>(reinterpret_cast<std::byte *>(std::to_address(pointer)) - offsetof(derived_t, accumulated_storage_buffer))); }
            };
            template<typename pointer_element_t, typename derived_t>
            struct add_accumulated_storage_member_t<pointer_element_t, void, derived_t>
            {
            };
            template<bool add>
            struct add_node_count_member_t
            {
            };
            template<>
            struct add_node_count_member_t<true>
            {
                std::size_t node_count = 0;
            };

            enum class rb2p_node_role_e { //                                loop_end    loop_ancestor   loop_parent loop_my_list
                end = 0b100, //                                         △▲ *
                root = 0b000, //                                        ▽▼ *                                       *
                child_left_not_a_leftmost_descendent = 0b001, //        ◸◤                            *           *
                child_left_leftmost_descendent_of_non_root = 0b010, //  ◿◢            *               *
                child_left_leftmost_descendent_of_root = 0b011, //      ◁◀  *                           *
                child_right_not_a_rightmost_descendent = 0b101, //      ◹◥                            *           *
                child_right_rightmost_descendent_of_non_root = 0b110, //◺◣            *               *
                child_right_rightmost_descendent_of_root = 0b111, //    ▷▶  *                           *
            };

            template<typename allocator_element_t>
            struct alignas(std::max({alignof(void *), static_cast<std::size_t>(0b1000)})) rb2p_node_navigator_all_t;
            template<typename allocator_element_t>
            struct rb2p_node_navigator_except_node_end_t;

            template<typename allocator_element_t>
            struct proxy_role_t
            {
                rb2p_node_navigator_all_t<allocator_element_t> *this_;

                operator rb2p_node_role_e() const { return static_cast<rb2p_node_role_e>(tagged_ptr_bit210_get(this_->my_list_begin_)); }
                proxy_role_t &operator=(rb2p_node_role_e other)
                {
                    this_->my_list_begin_ = tagged_ptr_bit210_set(this_->my_list_begin_, static_cast<uintptr_t>(other));
                    return *this;
                }
                proxy_role_t &operator=(proxy_role_t const &other) { return this->operator=(other.operator rb2p_node_role_e()); }
                friend void swap(proxy_role_t &lhs, proxy_role_t &rhs)
                {
                    rb2p_node_role_e temp = lhs;
                    lhs = rhs;
                    rhs = temp;
                }
                friend void swap(proxy_role_t &lhs, proxy_role_t &&rhs) { std::ranges::swap(lhs, rhs); }
                friend void swap(proxy_role_t &&lhs, proxy_role_t &rhs) { std::ranges::swap(lhs, rhs); }
                friend void swap(proxy_role_t &&lhs, proxy_role_t &&rhs) { std::ranges::swap(lhs, rhs); }
            };

            template<typename allocator_element_t>
            struct proxy_color_t
            {
                rb2p_node_navigator_all_t<allocator_element_t> *this_;

                operator bool() const
                {
                    switch(this_->role())
                    {
                    case rb2p_node_role_e::end:
                        //                        return false;
                        std::unreachable();
                        break;
                    default:
                        return tagged_ptr_bit2_is_setted(static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(this_)->next_);
                        break;
                    }
                }
                proxy_color_t &operator=(bool other)
                {
                    switch(this_->role())
                    {
                    case rb2p_node_role_e::end:
                        std::unreachable();
                        break;
                    default:
                        static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(this_)->next_ = !other ? tagged_ptr_bit2_unsetted_relaxed(static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(this_)->next_) : tagged_ptr_bit2_setted_relaxed(static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(this_)->next_);
                        break;
                    }
                    return *this;
                }
                proxy_color_t &operator=(proxy_color_t const &other) { return this->operator=(other.operator bool()); }
                friend void swap(proxy_color_t &lhs, proxy_color_t &rhs)
                {
                    bool temp = lhs;
                    lhs = rhs;
                    rhs = temp;
                }
                friend void swap(proxy_color_t &lhs, proxy_color_t &&rhs) { std::ranges::swap(lhs, rhs); }
                friend void swap(proxy_color_t &&lhs, proxy_color_t &rhs) { std::ranges::swap(lhs, rhs); }
                friend void swap(proxy_color_t &&lhs, proxy_color_t &&rhs) { std::ranges::swap(lhs, rhs); }
            };

            template<typename allocator_element_t>
            struct proxy_my_list_begin_t
            {
                rb2p_node_navigator_all_t<allocator_element_t> *this_;

                operator rb2p_node_navigator_all_t<allocator_element_t> *() const
                {
                    auto my_list_begin_bit210_unsetted = tagged_ptr_bit210_set(this_->my_list_begin_, 0);
                    switch(this_->role())
                    {
                    case rb2p_node_role_e::end:
                        return my_list_begin_bit210_unsetted == this_ ? tagged_ptr_bit0_setted(my_list_begin_bit210_unsetted) : my_list_begin_bit210_unsetted;
                        break;
                    default:
                        return tagged_ptr_bit1_is_setted(static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(this_)->next_) ? tagged_ptr_bit0_setted(my_list_begin_bit210_unsetted) : my_list_begin_bit210_unsetted;
                        break;
                    }
                }
                rb2p_node_navigator_all_t<allocator_element_t> *operator->() const { return this->operator rb2p_node_navigator_all_t<allocator_element_t> *(); }
                proxy_my_list_begin_t &operator=(rb2p_node_navigator_all_t<allocator_element_t> *other)
                {
                    switch(this_->role())
                    {
                    case rb2p_node_role_e::end:
                        if(tagged_ptr_bit0_is_setted(other))
                            assert(tagged_ptr_bit0_unsetted(other) == this_);
                        else
                            assert(other != this_);
                        this_->my_list_begin_ = tagged_ptr_bit210_set(other, tagged_ptr_bit210_get(this_->my_list_begin_));
                        break;
                    default:
                        static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(this_)->next_ = !tagged_ptr_bit0_is_setted(other) ? tagged_ptr_bit1_unsetted_relaxed(static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(this_)->next_) : tagged_ptr_bit1_setted_relaxed(static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(this_)->next_);
                        this_->my_list_begin_ = tagged_ptr_bit210_set(other, tagged_ptr_bit210_get(this_->my_list_begin_));
                        break;
                    }
                    return *this;
                }
                proxy_my_list_begin_t &operator=(proxy_my_list_begin_t const &other) { return this->operator=(other.operator rb2p_node_navigator_all_t<allocator_element_t> *()); }
            };

            template<typename allocator_element_t>
            struct proxy_next_t
            {
                rb2p_node_navigator_all_t<allocator_element_t> *this_;

                operator rb2p_node_navigator_all_t<allocator_element_t> *() const
                {
                    switch(this_->role())
                    {
                    case rb2p_node_role_e::end:
                        std::unreachable();
                        break;
                    default:
                    {
                        auto next_bit210_unsetted = tagged_ptr_bit210_set(static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(this_)->next_, 0);
                        return tagged_ptr_bit0_is_setted(static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(this_)->next_) ? tagged_ptr_bit0_setted(next_bit210_unsetted) : next_bit210_unsetted;
                    } break;
                    }
                }
                rb2p_node_navigator_all_t<allocator_element_t> *operator->() const { return this->operator rb2p_node_navigator_all_t<allocator_element_t> *(); }
                proxy_next_t &operator=(rb2p_node_navigator_all_t<allocator_element_t> *other)
                {
                    switch(this_->role())
                    {
                    case rb2p_node_role_e::end:
                        std::unreachable();
                        break;
                    default:
                        static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(this_)->next_ = !tagged_ptr_bit0_is_setted(other) ? tagged_ptr_bit0_unsetted_relaxed(tagged_ptr_bit210_set(other, tagged_ptr_bit210_get(static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(this_)->next_))) : tagged_ptr_bit0_setted_relaxed(tagged_ptr_bit210_set(other, tagged_ptr_bit210_get(static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(this_)->next_)));
                        break;
                    }
                    return *this;
                }
                proxy_next_t &operator=(proxy_next_t const &other) { return this->operator=(other.operator rb2p_node_navigator_all_t<allocator_element_t> *()); }
            };

            template<typename allocator_element_t>
            struct proxy_parent_all_t
            {
                rb2p_node_navigator_all_t<allocator_element_t> *this_;

                operator rb2p_node_navigator_all_t<allocator_element_t> *() const
                {
                    switch(this_->role())
                    {
                    case rb2p_node_role_e::end:
                        if(tagged_ptr_bit0_is_setted(this_->my_list_begin())) // return tagged end
                        {
                            assert(tagged_ptr_bit0_unsetted(this_->my_list_begin()) == this_);
                            return this_->my_list_begin();
                        }
                        else // return tagged root
                        {
                            rb2p_node_navigator_all_t<allocator_element_t> *current = this_->my_list_begin();
                            switch(current->role())
                            {
                            case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                                assert(!tagged_ptr_bit0_is_setted(current->my_list_begin()));
                                current = current->my_list_begin();
                                break;
                            case rb2p_node_role_e::root:
                                break;
                            default:
                                std::unreachable();
                            }
                            assert(current->role() == rb2p_node_role_e::root);
                            return tagged_ptr_bit0_setted(current);
                        }
                        break;
                    case rb2p_node_role_e::root: // return tagged end
                    {
                        rb2p_node_navigator_all_t<allocator_element_t> *current = this_;
                        if(!tagged_ptr_bit0_is_setted(current->next()))
                        {
                            current = current->next();
                            assert(current->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root);
                            assert(tagged_ptr_bit0_is_setted(current->my_list_begin()));
                            assert(tagged_ptr_bit0_unsetted(current->my_list_begin())->role() == rb2p_node_role_e::end);
                            return current->my_list_begin();
                        }
                        else
                        {
                            assert(tagged_ptr_bit0_unsetted(current->next())->role() == rb2p_node_role_e::end);
                            return current->next();
                        }
                    } break;
                    case rb2p_node_role_e::child_left_not_a_leftmost_descendent:
                    case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                    case rb2p_node_role_e::child_left_leftmost_descendent_of_non_root:
                        assert(tagged_ptr_bit0_is_setted(this_->next()));
                        return tagged_ptr_bit0_unsetted(this_->next());
                        break;
                    case rb2p_node_role_e::child_right_not_a_rightmost_descendent:
                    case rb2p_node_role_e::child_right_rightmost_descendent_of_non_root:
                    case rb2p_node_role_e::child_right_rightmost_descendent_of_root:
                        if(tagged_ptr_bit0_is_setted(this_->next()))
                        {
                            assert(tagged_ptr_bit0_unsetted(this_->next())->my_list_begin() == this_);
                            return tagged_ptr_bit0_unsetted(this_->next());
                        }
                        else
                        {
                            rb2p_node_navigator_all_t<allocator_element_t> *current = this_->next();
                            switch(current->role())
                            {
                            case rb2p_node_role_e::child_right_rightmost_descendent_of_non_root:
                                assert(!tagged_ptr_bit0_is_setted(current->my_list_begin()));
                                current = current->my_list_begin();
                                assert(current->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent);
                                assert(tagged_ptr_bit0_is_setted(current->next()));
                                current = tagged_ptr_bit0_unsetted(current->next());
                                assert(current->my_list_begin() == this_);
                                return current;
                                break;
                            case rb2p_node_role_e::child_left_not_a_leftmost_descendent:
                            case rb2p_node_role_e::child_left_leftmost_descendent_of_non_root:
                            case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                                assert(current->next() == nullptr || tagged_ptr_bit0_is_setted(current->next()));
                                if(current->next() != nullptr && tagged_ptr_bit0_unsetted(current->next())->my_list_begin() == this_)
                                    return tagged_ptr_bit0_unsetted(current->next());
                                else
                                {
                                    assert(!tagged_ptr_bit0_is_setted(current->my_list_begin()));
                                    current = current->my_list_begin();
                                    switch(current->role())
                                    {
                                    case rb2p_node_role_e::child_left_not_a_leftmost_descendent:
                                    case rb2p_node_role_e::child_left_leftmost_descendent_of_non_root:
                                    case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                                        assert(tagged_ptr_bit0_is_setted(current->next()));
                                        current = tagged_ptr_bit0_unsetted(current->next());
                                        assert(current->my_list_begin() == this_);
                                        return current;
                                        break;
                                    case rb2p_node_role_e::child_right_rightmost_descendent_of_non_root:
                                        assert(!tagged_ptr_bit0_is_setted(current->my_list_begin()));
                                        current = current->my_list_begin();
                                        assert(current->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent);
                                        assert(tagged_ptr_bit0_is_setted(current->next()));
                                        current = tagged_ptr_bit0_unsetted(current->next());
                                        assert(current->my_list_begin() == this_);
                                        return current;
                                        break;
                                    default:
                                        std::unreachable();
                                        break;
                                    }
                                }
                                break;
                            default:
                                std::unreachable();
                                break;
                            }
                        }
                        break;
                    default:
                        std::unreachable();
                        break;
                    }
                }
                rb2p_node_navigator_all_t<allocator_element_t> *operator->() const { return this->operator rb2p_node_navigator_all_t<allocator_element_t> *(); }
            };

            template<typename allocator_element_t>
            struct proxy_parent_except_node_end_t
            {
                rb2p_node_navigator_except_node_end_t<allocator_element_t> *this_;

                operator rb2p_node_navigator_except_node_end_t<allocator_element_t> *() const
                {
                    switch(this_->role())
                    {
                    case rb2p_node_role_e::end:
                        std::unreachable();
                        break;
                    case rb2p_node_role_e::root: // return tagged end
                        return nullptr;
                        break;
                    case rb2p_node_role_e::child_left_not_a_leftmost_descendent:
                    case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                    case rb2p_node_role_e::child_left_leftmost_descendent_of_non_root:
                        assert(tagged_ptr_bit0_is_setted(this_->next()));
                        return static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(tagged_ptr_bit0_unsetted(this_->next()));
                        break;
                    case rb2p_node_role_e::child_right_not_a_rightmost_descendent:
                    case rb2p_node_role_e::child_right_rightmost_descendent_of_non_root:
                    case rb2p_node_role_e::child_right_rightmost_descendent_of_root:
                        if(tagged_ptr_bit0_is_setted(this_->next()))
                        {
                            assert(tagged_ptr_bit0_unsetted(this_->next())->my_list_begin() == this_);
                            return static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(tagged_ptr_bit0_unsetted(this_->next()));
                        }
                        else
                        {
                            rb2p_node_navigator_all_t<allocator_element_t> *current = this_->next();
                            switch(current->role())
                            {
                            case rb2p_node_role_e::child_right_rightmost_descendent_of_non_root:
                                assert(!tagged_ptr_bit0_is_setted(current->my_list_begin()));
                                current = current->my_list_begin();
                                assert(current->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent);
                                assert(tagged_ptr_bit0_is_setted(current->next()));
                                current = tagged_ptr_bit0_unsetted(current->next());
                                assert(current->my_list_begin() == this_);
                                return static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(current);
                                break;
                            case rb2p_node_role_e::child_left_not_a_leftmost_descendent:
                            case rb2p_node_role_e::child_left_leftmost_descendent_of_non_root:
                            case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                                assert(current->next() == nullptr || tagged_ptr_bit0_is_setted(current->next()));
                                if(current->next() != nullptr && tagged_ptr_bit0_unsetted(current->next())->my_list_begin() == this_)
                                    return static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(tagged_ptr_bit0_unsetted(current->next()));
                                else
                                {
                                    assert(!tagged_ptr_bit0_is_setted(current->my_list_begin()));
                                    current = current->my_list_begin();
                                    switch(current->role())
                                    {
                                    case rb2p_node_role_e::child_left_not_a_leftmost_descendent:
                                    case rb2p_node_role_e::child_left_leftmost_descendent_of_non_root:
                                    case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                                        assert(tagged_ptr_bit0_is_setted(current->next()));
                                        current = tagged_ptr_bit0_unsetted(current->next());
                                        assert(current->my_list_begin() == this_);
                                        return static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(current);
                                        break;
                                    case rb2p_node_role_e::child_right_rightmost_descendent_of_non_root:
                                        assert(!tagged_ptr_bit0_is_setted(current->my_list_begin()));
                                        current = current->my_list_begin();
                                        assert(current->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent);
                                        assert(tagged_ptr_bit0_is_setted(current->next()));
                                        current = tagged_ptr_bit0_unsetted(current->next());
                                        assert(current->my_list_begin() == this_);
                                        return static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(current);
                                        break;
                                    default:
                                        std::unreachable();
                                        break;
                                    }
                                }
                                break;
                            default:
                                std::unreachable();
                                break;
                            }
                        }
                        break;
                    default:
                        std::unreachable();
                        break;
                    }
                }
                rb2p_node_navigator_except_node_end_t<allocator_element_t> *operator->() const { return this->operator rb2p_node_navigator_except_node_end_t<allocator_element_t> *(); }
            };

            template<typename allocator_element_t>
            struct proxy_child_all_t
            {
                rb2p_node_navigator_all_t<allocator_element_t> *this_;
                bool child_left_or_right;
                operator rb2p_node_navigator_all_t<allocator_element_t> *() const
                {
                    if(!child_left_or_right)
                    {
                        switch(this_->role())
                        {
                        case rb2p_node_role_e::end:
                            if(tagged_ptr_bit0_is_setted(this_->my_list_begin())) // return tagged end
                            {
                                assert(tagged_ptr_bit0_unsetted(this_->my_list_begin()) == this_);
                                return this_->my_list_begin();
                            }
                            else // return tagged rightmost descendent of root (or tagged root)
                            {
                                rb2p_node_navigator_all_t<allocator_element_t> *current = this_->my_list_begin();
                                switch(current->role())
                                {
                                case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                                    assert(!tagged_ptr_bit0_is_setted(current->my_list_begin()));
                                    current = current->my_list_begin();
                                    break;
                                case rb2p_node_role_e::root:
                                    break;
                                default:
                                    std::unreachable();
                                }
                                assert(current->role() == rb2p_node_role_e::root);
                                if(!tagged_ptr_bit0_is_setted(current->next()))
                                {
                                    current = current->next();
                                    assert(current->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root);
                                    assert(tagged_ptr_bit0_is_setted(current->my_list_begin()));
                                    assert(tagged_ptr_bit0_unsetted(current->my_list_begin())->role() == rb2p_node_role_e::end);
                                    return tagged_ptr_bit0_setted(current);
                                }
                                else
                                {
                                    assert(tagged_ptr_bit0_unsetted(current->next())->role() == rb2p_node_role_e::end);
                                    return tagged_ptr_bit0_setted(current);
                                }
                            }
                            break;
                        case rb2p_node_role_e::child_left_leftmost_descendent_of_root: // return tagged end
                        {
                            rb2p_node_navigator_all_t<allocator_element_t> *current = this_;
                            assert(!tagged_ptr_bit0_is_setted(current->my_list_begin()));
                            current = current->my_list_begin();
                            assert(current->role() == rb2p_node_role_e::root);
                            if(!tagged_ptr_bit0_is_setted(current->next()))
                            {
                                current = current->next();
                                assert(current->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root);
                                assert(tagged_ptr_bit0_is_setted(current->my_list_begin()));
                                assert(tagged_ptr_bit0_unsetted(current->my_list_begin())->role() == rb2p_node_role_e::end);
                                return current->my_list_begin();
                            }
                            else
                            {
                                assert(tagged_ptr_bit0_unsetted(current->next())->role() == rb2p_node_role_e::end);
                                return current->next();
                            }
                        } break;
                        case rb2p_node_role_e::root:
                        case rb2p_node_role_e::child_left_not_a_leftmost_descendent:
                        case rb2p_node_role_e::child_right_not_a_rightmost_descendent:
                        {
                            if(this_->role() == rb2p_node_role_e::root && tagged_ptr_bit0_is_setted(this_->my_list_begin())) // list is empty
                            {
                                assert(tagged_ptr_bit0_unsetted(this_->my_list_begin()) == this_);
                                return this_->parent_all();
                            }
                            else // list is not empty
                            {
                                assert(!tagged_ptr_bit0_is_setted(this_->my_list_begin()));
                                rb2p_node_navigator_all_t<allocator_element_t> *current = this_->my_list_begin();
                                if(tagged_ptr_bit0_is_setted(current->next()))
                                {
                                    assert(tagged_ptr_bit0_unsetted(current->next()) == this_);
                                    switch(current->role())
                                    {
                                    case rb2p_node_role_e::child_left_leftmost_descendent_of_non_root:
                                    case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                                        return current;
                                        break;
                                    case rb2p_node_role_e::child_right_rightmost_descendent_of_non_root:
                                    case rb2p_node_role_e::child_right_rightmost_descendent_of_root:
                                        if(this_->role() == rb2p_node_role_e::root)
                                            return this_->parent_all();
                                        else
                                            return nullptr;
                                        break;
                                    default:
                                        std::unreachable();
                                        break;
                                    }
                                }
                                else
                                {
                                    current = current->next();
                                    switch(current->role())
                                    {
                                    case rb2p_node_role_e::child_right_rightmost_descendent_of_non_root:
                                        assert(!tagged_ptr_bit0_is_setted(current->my_list_begin()));
                                        current = current->my_list_begin();
                                        assert(current->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent);
                                        assert(tagged_ptr_bit0_is_setted(current->next()));
                                        assert(tagged_ptr_bit0_unsetted(current->next()) == this_);
                                        return current;
                                        break;
                                    case rb2p_node_role_e::child_left_not_a_leftmost_descendent:
                                    case rb2p_node_role_e::child_left_leftmost_descendent_of_non_root:
                                    case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                                        assert(current->next() == nullptr || tagged_ptr_bit0_is_setted(current->next()));
                                        if(current->next() != nullptr && tagged_ptr_bit0_unsetted(current->next()) == this_)
                                            return current;
                                        else
                                        {
                                            assert(!tagged_ptr_bit0_is_setted(current->my_list_begin()));
                                            current = current->my_list_begin();
                                            switch(current->role())
                                            {
                                            case rb2p_node_role_e::child_left_not_a_leftmost_descendent:
                                            case rb2p_node_role_e::child_left_leftmost_descendent_of_non_root:
                                            case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                                                assert(tagged_ptr_bit0_is_setted(current->next()));
                                                assert(tagged_ptr_bit0_unsetted(current->next()) == this_);
                                                return current;
                                                break;
                                            case rb2p_node_role_e::child_right_rightmost_descendent_of_non_root:
                                                assert(!tagged_ptr_bit0_is_setted(current->my_list_begin()));
                                                current = current->my_list_begin();
                                                assert(current->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent);
                                                assert(tagged_ptr_bit0_is_setted(current->next()));
                                                assert(tagged_ptr_bit0_unsetted(current->next()) == this_);
                                                return current;
                                                break;
                                            default:
                                                std::unreachable();
                                                break;
                                            }
                                        }
                                        break;
                                    default:
                                        std::unreachable();
                                        break;
                                    }
                                }
                            }
                        } break;
                        case rb2p_node_role_e::child_left_leftmost_descendent_of_non_root:
                        case rb2p_node_role_e::child_right_rightmost_descendent_of_non_root:
                        case rb2p_node_role_e::child_right_rightmost_descendent_of_root:
                            return nullptr;
                            break;
                        default:
                            std::unreachable();
                            break;
                        }
                    }
                    else
                    {
                        switch(this_->role())
                        {
                        case rb2p_node_role_e::end:
                            if(tagged_ptr_bit0_is_setted(this_->my_list_begin())) // return tagged end
                            {
                                assert(tagged_ptr_bit0_unsetted(this_->my_list_begin()) == this_);
                                return this_->my_list_begin();
                            }
                            else // return tagged leftmost descendent of root (or tagged root)
                            {
                                rb2p_node_navigator_all_t<allocator_element_t> *current = this_->my_list_begin();
                                switch(current->role())
                                {
                                case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                                case rb2p_node_role_e::root:
                                    return tagged_ptr_bit0_setted(current);
                                    break;
                                default:
                                    std::unreachable();
                                }
                            }
                            break;
                        case rb2p_node_role_e::child_right_rightmost_descendent_of_root: // return tagged end
                            assert(tagged_ptr_bit0_is_setted(this_->my_list_begin()));
                            assert(tagged_ptr_bit0_unsetted(this_->my_list_begin())->role() == rb2p_node_role_e::end);
                            return this_->my_list_begin();
                            break;
                        case rb2p_node_role_e::root:
                        case rb2p_node_role_e::child_left_not_a_leftmost_descendent:
                        case rb2p_node_role_e::child_right_not_a_rightmost_descendent:
                        {
                            if(this_->role() == rb2p_node_role_e::root && tagged_ptr_bit0_is_setted(this_->my_list_begin())) // list is empty
                            {
                                assert(tagged_ptr_bit0_unsetted(this_->my_list_begin()) == this_);
                                return this_->parent_all();
                            }
                            else // list is not empty
                            {
                                assert(!tagged_ptr_bit0_is_setted(this_->my_list_begin()));
                                rb2p_node_navigator_all_t<allocator_element_t> *current = this_->my_list_begin();
                                switch(current->role())
                                {
                                case rb2p_node_role_e::child_right_not_a_rightmost_descendent:
                                case rb2p_node_role_e::child_right_rightmost_descendent_of_non_root:
                                case rb2p_node_role_e::child_right_rightmost_descendent_of_root:
                                    return current;
                                    break;
                                case rb2p_node_role_e::child_left_leftmost_descendent_of_non_root:
                                case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                                    if(this_->role() == rb2p_node_role_e::root)
                                        return this_->parent_all();
                                    else
                                        return nullptr;
                                    break;
                                default:
                                    std::unreachable();
                                    break;
                                }
                            }
                        } break;
                        case rb2p_node_role_e::child_left_leftmost_descendent_of_non_root:
                        case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                        case rb2p_node_role_e::child_right_rightmost_descendent_of_non_root:
                            return nullptr;
                            break;
                        default:
                            std::unreachable();
                            break;
                        }
                    }
                }
                rb2p_node_navigator_all_t<allocator_element_t> *operator->() const { return this->operator rb2p_node_navigator_all_t<allocator_element_t> *(); }
            };

            template<typename allocator_element_t>
            struct proxy_child_except_node_end_t
            {
                rb2p_node_navigator_except_node_end_t<allocator_element_t> *this_;
                bool child_left_or_right;

                operator rb2p_node_navigator_except_node_end_t<allocator_element_t> *() const
                {
                    if(!child_left_or_right)
                    {
                        switch(this_->role())
                        {
                        case rb2p_node_role_e::end:
                            std::unreachable();
                            break;
                        case rb2p_node_role_e::root:
                        case rb2p_node_role_e::child_left_not_a_leftmost_descendent:
                        case rb2p_node_role_e::child_right_not_a_rightmost_descendent:
                        {
                            if(this_->role() == rb2p_node_role_e::root && tagged_ptr_bit0_is_setted(this_->my_list_begin())) // list is empty
                            {
                                assert(tagged_ptr_bit0_unsetted(this_->my_list_begin()) == this_);
                                return nullptr;
                            }
                            else // list is not empty
                            {
                                assert(!tagged_ptr_bit0_is_setted(this_->my_list_begin()));
                                rb2p_node_navigator_except_node_end_t<allocator_element_t> *current = static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(static_cast<rb2p_node_navigator_all_t<allocator_element_t> *>(this_->my_list_begin()));
                                if(tagged_ptr_bit0_is_setted(current->next()))
                                {
                                    assert(tagged_ptr_bit0_unsetted(current->next()) == this_);
                                    switch(current->role())
                                    {
                                    case rb2p_node_role_e::child_left_leftmost_descendent_of_non_root:
                                    case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                                        return current;
                                        break;
                                    case rb2p_node_role_e::child_right_rightmost_descendent_of_non_root:
                                    case rb2p_node_role_e::child_right_rightmost_descendent_of_root:
                                        return nullptr;
                                        break;
                                    default:
                                        std::unreachable();
                                        break;
                                    }
                                }
                                else
                                {
                                    current = static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(static_cast<rb2p_node_navigator_all_t<allocator_element_t> *>(current->next()));
                                    switch(current->role())
                                    {
                                    case rb2p_node_role_e::child_right_rightmost_descendent_of_non_root:
                                        assert(!tagged_ptr_bit0_is_setted(current->my_list_begin()));
                                        current = static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(static_cast<rb2p_node_navigator_all_t<allocator_element_t> *>(current->my_list_begin()));
                                        assert(current->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent);
                                        assert(tagged_ptr_bit0_is_setted(current->next()));
                                        assert(tagged_ptr_bit0_unsetted(current->next()) == this_);
                                        return current;
                                        break;
                                    case rb2p_node_role_e::child_left_not_a_leftmost_descendent:
                                    case rb2p_node_role_e::child_left_leftmost_descendent_of_non_root:
                                    case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                                        assert(current->next() == nullptr || tagged_ptr_bit0_is_setted(current->next()));
                                        if(current->next() != nullptr && tagged_ptr_bit0_unsetted(current->next()) == this_)
                                            return current;
                                        else
                                        {
                                            assert(!tagged_ptr_bit0_is_setted(current->my_list_begin()));
                                            current = static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(static_cast<rb2p_node_navigator_all_t<allocator_element_t> *>(current->my_list_begin()));
                                            switch(current->role())
                                            {
                                            case rb2p_node_role_e::child_left_not_a_leftmost_descendent:
                                            case rb2p_node_role_e::child_left_leftmost_descendent_of_non_root:
                                            case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                                                assert(tagged_ptr_bit0_is_setted(current->next()));
                                                assert(tagged_ptr_bit0_unsetted(current->next()) == this_);
                                                return current;
                                                break;
                                            case rb2p_node_role_e::child_right_rightmost_descendent_of_non_root:
                                                assert(!tagged_ptr_bit0_is_setted(current->my_list_begin()));
                                                current = static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(static_cast<rb2p_node_navigator_all_t<allocator_element_t> *>(current->my_list_begin()));
                                                assert(current->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent);
                                                assert(tagged_ptr_bit0_is_setted(current->next()));
                                                assert(tagged_ptr_bit0_unsetted(current->next()) == this_);
                                                return current;
                                                break;
                                            default:
                                                std::unreachable();
                                                break;
                                            }
                                        }
                                        break;
                                    default:
                                        std::unreachable();
                                        break;
                                    }
                                }
                            }
                        } break;
                        case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                        case rb2p_node_role_e::child_left_leftmost_descendent_of_non_root:
                        case rb2p_node_role_e::child_right_rightmost_descendent_of_non_root:
                        case rb2p_node_role_e::child_right_rightmost_descendent_of_root:
                            return nullptr;
                            break;
                        default:
                            std::unreachable();
                            break;
                        }
                    }
                    else
                    {
                        switch(this_->role())
                        {
                        case rb2p_node_role_e::end:
                            std::unreachable();
                            break;
                        case rb2p_node_role_e::root:
                        case rb2p_node_role_e::child_left_not_a_leftmost_descendent:
                        case rb2p_node_role_e::child_right_not_a_rightmost_descendent:
                        {
                            if(this_->role() == rb2p_node_role_e::root && tagged_ptr_bit0_is_setted(this_->my_list_begin())) // list is empty
                            {
                                assert(tagged_ptr_bit0_unsetted(this_->my_list_begin()) == this_);
                                return nullptr;
                            }
                            else // list is not empty
                            {
                                assert(!tagged_ptr_bit0_is_setted(this_->my_list_begin()));
                                rb2p_node_navigator_except_node_end_t<allocator_element_t> *current = static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(static_cast<rb2p_node_navigator_all_t<allocator_element_t> *>(this_->my_list_begin()));
                                switch(current->role())
                                {
                                case rb2p_node_role_e::child_right_not_a_rightmost_descendent:
                                case rb2p_node_role_e::child_right_rightmost_descendent_of_non_root:
                                case rb2p_node_role_e::child_right_rightmost_descendent_of_root:
                                    return current;
                                    break;
                                case rb2p_node_role_e::child_left_leftmost_descendent_of_non_root:
                                case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                                    return nullptr;
                                    break;
                                default:
                                    std::unreachable();
                                    break;
                                }
                            }
                        } break;
                        case rb2p_node_role_e::child_right_rightmost_descendent_of_root:
                        case rb2p_node_role_e::child_left_leftmost_descendent_of_non_root:
                        case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                        case rb2p_node_role_e::child_right_rightmost_descendent_of_non_root:
                            return nullptr;
                            break;
                        default:
                            std::unreachable();
                            break;
                        }
                    }
                }
                rb2p_node_navigator_except_node_end_t<allocator_element_t> *operator->() const { return this->operator rb2p_node_navigator_except_node_end_t<allocator_element_t> *(); }
            };
        } // namespace augmented_sequence_rb2p
    } // namespace detail
} // namespace augmented_containers
namespace std
{
    template<typename allocator_element_t>
    struct std::pointer_traits<augmented_containers::detail::augmented_sequence_rb2p::proxy_my_list_begin_t<allocator_element_t>>
    {
        using pointer = augmented_containers::detail::augmented_sequence_rb2p::proxy_my_list_begin_t<allocator_element_t>;
        using element_type = augmented_containers::detail::augmented_sequence_rb2p::rb2p_node_navigator_all_t<allocator_element_t>;
        using difference_type = std::ptrdiff_t;
        template<class U>
        using rebind = U *;
        static element_type *pointer_to(element_type &r) { return &r; }
        static element_type *to_address(pointer p) noexcept { return p.operator element_type *(); }
    };
    template<typename allocator_element_t>
    struct std::pointer_traits<augmented_containers::detail::augmented_sequence_rb2p::proxy_next_t<allocator_element_t>>
    {
        using pointer = augmented_containers::detail::augmented_sequence_rb2p::proxy_next_t<allocator_element_t>;
        using element_type = augmented_containers::detail::augmented_sequence_rb2p::rb2p_node_navigator_all_t<allocator_element_t>;
        using difference_type = std::ptrdiff_t;
        template<class U>
        using rebind = U *;
        static element_type *pointer_to(element_type &r) { return &r; }
        static element_type *to_address(pointer p) noexcept { return p.operator element_type *(); }
    };

    template<typename allocator_element_t>
    struct std::pointer_traits<augmented_containers::detail::augmented_sequence_rb2p::proxy_parent_all_t<allocator_element_t>>
    {
        using pointer = augmented_containers::detail::augmented_sequence_rb2p::proxy_parent_all_t<allocator_element_t>;
        using element_type = augmented_containers::detail::augmented_sequence_rb2p::rb2p_node_navigator_all_t<allocator_element_t>;
        using difference_type = std::ptrdiff_t;
        template<class U>
        using rebind = U *;
        static element_type *pointer_to(element_type &r) { return &r; }
        static element_type *to_address(pointer p) noexcept { return p.operator element_type *(); }
    };
    template<typename allocator_element_t>
    struct std::pointer_traits<augmented_containers::detail::augmented_sequence_rb2p::proxy_parent_except_node_end_t<allocator_element_t>>
    {
        using pointer = augmented_containers::detail::augmented_sequence_rb2p::proxy_parent_except_node_end_t<allocator_element_t>;
        using element_type = augmented_containers::detail::augmented_sequence_rb2p::rb2p_node_navigator_except_node_end_t<allocator_element_t>;
        using difference_type = std::ptrdiff_t;
        template<class U>
        using rebind = U *;
        static element_type *pointer_to(element_type &r) { return &r; }
        static element_type *to_address(pointer p) noexcept { return p.operator element_type *(); }
    };
    template<typename allocator_element_t>
    struct std::pointer_traits<augmented_containers::detail::augmented_sequence_rb2p::proxy_child_all_t<allocator_element_t>>
    {
        using pointer = augmented_containers::detail::augmented_sequence_rb2p::proxy_child_all_t<allocator_element_t>;
        using element_type = augmented_containers::detail::augmented_sequence_rb2p::rb2p_node_navigator_all_t<allocator_element_t>;
        using difference_type = std::ptrdiff_t;
        template<class U>
        using rebind = U *;
        static element_type *pointer_to(element_type &r) { return &r; }
        static element_type *to_address(pointer p) noexcept { return p.operator element_type *(); }
    };
    template<typename allocator_element_t>
    struct std::pointer_traits<augmented_containers::detail::augmented_sequence_rb2p::proxy_child_except_node_end_t<allocator_element_t>>
    {
        using pointer = augmented_containers::detail::augmented_sequence_rb2p::proxy_child_except_node_end_t<allocator_element_t>;
        using element_type = augmented_containers::detail::augmented_sequence_rb2p::rb2p_node_navigator_except_node_end_t<allocator_element_t>;
        using difference_type = std::ptrdiff_t;
        template<class U>
        using rebind = U *;
        static element_type *pointer_to(element_type &r) { return &r; }
        static element_type *to_address(pointer p) noexcept { return p.operator element_type *(); }
    };
} // namespace std
namespace augmented_containers
{
    namespace detail
    {
        namespace augmented_sequence_rb2p
        {
            template<typename allocator_element_t>
            struct alignas(std::max({alignof(void *), static_cast<std::size_t>(0b1000)})) rb2p_node_navigator_all_t
            {
                rb2p_node_navigator_all_t *my_list_begin_ = nullptr;

                proxy_role_t<allocator_element_t> role() { return {this}; }
                proxy_color_t<allocator_element_t> color() { return {this}; }
                proxy_my_list_begin_t<allocator_element_t> my_list_begin() { return {this}; }
                proxy_next_t<allocator_element_t> next() { return {this}; }
                proxy_parent_all_t<allocator_element_t> parent_all() { return {this}; }
                proxy_parent_except_node_end_t<allocator_element_t> parent()
                {
                    assert(this->role() != rb2p_node_role_e::end);
                    return {static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(this)};
                }
                proxy_child_all_t<allocator_element_t> child_left_all() { return {this, false}; }
                proxy_child_all_t<allocator_element_t> child_right_all() { return {this, true}; }
                proxy_child_except_node_end_t<allocator_element_t> child_left()
                {
                    assert(this->role() != rb2p_node_role_e::end);
                    return {static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(this), false};
                }
                proxy_child_except_node_end_t<allocator_element_t> child_right()
                {
                    assert(this->role() != rb2p_node_role_e::end);
                    return {static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(this), true};
                }

                struct p_child_left_or_right_all_t
                {
                    bool child_left_or_right;
                    friend decltype(auto) operator->*(rb2p_node_navigator_all_t *navigator, p_child_left_or_right_all_t const &p_child_left_or_right) { return !p_child_left_or_right.child_left_or_right ? navigator->child_left_all() : navigator->child_right_all(); }

                    template<bool is_reversed>
                    static constexpr p_child_left_or_right_all_t make_p_child_left_or_right(bool child_left_or_right) { return {.child_left_or_right = !is_reversed ? child_left_or_right : !child_left_or_right}; }
                };
                struct p_child_left_or_right_except_node_end_t
                {
                    bool child_left_or_right;
                    friend decltype(auto) operator->*(rb2p_node_navigator_all_t *navigator, p_child_left_or_right_except_node_end_t const &p_child_left_or_right) { return !p_child_left_or_right.child_left_or_right ? navigator->child_left() : navigator->child_right(); }

                    template<bool is_reversed>
                    static constexpr p_child_left_or_right_except_node_end_t make_p_child_left_or_right(bool child_left_or_right) { return {.child_left_or_right = !is_reversed ? child_left_or_right : !child_left_or_right}; }
                };

                template<bool is_reversed_>
                struct parent_info_t
                {
                    static constexpr bool is_reversed = is_reversed_;
                    static constexpr p_child_left_or_right_except_node_end_t p_child_left = p_child_left_or_right_except_node_end_t::template make_p_child_left_or_right<is_reversed>(false), p_child_right = p_child_left_or_right_except_node_end_t::template make_p_child_left_or_right<is_reversed>(true);

                    rb2p_node_navigator_all_t *parent;
                    bool is_left_or_right_child_of_parent;
                    bool is_end() const { return tagged_ptr_bit0_is_setted(parent); }

                    parent_info_t()
                        : parent(nullptr)
                    {}
                    parent_info_t(rb2p_node_navigator_all_t *this_)
                        : parent(this_->parent_all())
                    {
                        if(!is_end())
                        {
                            if(parent->*p_child_left == this_) is_left_or_right_child_of_parent = false;
                            else if(parent->*p_child_right == this_) is_left_or_right_child_of_parent = true;
                            else std::unreachable();
                        }
                        switch(this_->role())
                        {
                        case rb2p_node_role_e::root:
                            assert(tagged_ptr_bit0_is_setted(parent));
                            break;
                        case rb2p_node_role_e::child_left_not_a_leftmost_descendent:
                        case rb2p_node_role_e::child_left_leftmost_descendent_of_non_root:
                        case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                            assert(!tagged_ptr_bit0_is_setted(parent));
                            assert(!is_reversed ? !is_left_or_right_child_of_parent : is_left_or_right_child_of_parent);
                            break;
                        case rb2p_node_role_e::child_right_not_a_rightmost_descendent:
                        case rb2p_node_role_e::child_right_rightmost_descendent_of_non_root:
                        case rb2p_node_role_e::child_right_rightmost_descendent_of_root:
                            assert(!tagged_ptr_bit0_is_setted(parent));
                            assert(!is_reversed ? is_left_or_right_child_of_parent : !is_left_or_right_child_of_parent);
                            break;
                        default:
                            std::unreachable();
                            break;
                        }
                    };
                };

                template<typename node_end_t>
                struct node_end_functions_t
                {
                    static node_end_t *create_node_end(/*allocator_element_t const &allocator_element*/)
                    {
                        node_end_t *result = new node_end_t();
                        return result;
                    }
                };

                struct loop_end_t
                {
                    rb2p_node_navigator_all_t *end = nullptr;
                    rb2p_node_navigator_except_node_end_t<allocator_element_t> *leftmost_descendent_of_root = nullptr, *root = nullptr, *rightmost_descendent_of_root = nullptr;
                    int index = -1;
                    template<typename ostream_t>
                    friend ostream_t &&operator<<(ostream_t &&ostream, loop_end_t const &loop)
                    {
                        return std::forward<ostream_t>(ostream << *loop.end << '\n'
                                                               << *loop.leftmost_descendent_of_root << '\n'
                                                               << *loop.root << '\n'
                                                               << *loop.rightmost_descendent_of_root << '\n');
                    }
                    bool leftmost_descendent_of_root_is_nullptr() const { return leftmost_descendent_of_root == nullptr || leftmost_descendent_of_root == root; }
                    bool rightmost_descendent_of_root_is_nullptr() const { return rightmost_descendent_of_root == nullptr || rightmost_descendent_of_root == root; }
                    void assert_unique()
                    {
                        std::unordered_set<rb2p_node_navigator_all_t *> pointers;
                        assert(end != nullptr);
                        assert(pointers.insert(end).second);
                        if(!leftmost_descendent_of_root_is_nullptr())
                            assert(pointers.insert(leftmost_descendent_of_root).second);
                        if(root != nullptr)
                            assert(pointers.insert(root).second);
                        if(!rightmost_descendent_of_root_is_nullptr())
                            assert(pointers.insert(rightmost_descendent_of_root).second);
                    }
                    void link()
                    {
                        assert(index == -1);
                        assert(end != nullptr);
                        assert_unique();
                        if(root == nullptr)
                        {
                            assert(leftmost_descendent_of_root == nullptr);
                            assert(rightmost_descendent_of_root == nullptr);
                            end->my_list_begin() = tagged_ptr_bit0_setted(end);
                        }
                        else
                        {
                            if(!leftmost_descendent_of_root_is_nullptr())
                            {
                                end->my_list_begin() = leftmost_descendent_of_root;
                                leftmost_descendent_of_root->my_list_begin() = root;
                            }
                            else
                                end->my_list_begin() = root;
                            if(!rightmost_descendent_of_root_is_nullptr())
                            {
                                root->next() = rightmost_descendent_of_root;
                                rightmost_descendent_of_root->my_list_begin() = tagged_ptr_bit0_setted(end);
                            }
                            else
                                root->next() = tagged_ptr_bit0_setted(end);
                        }
                    }
                    rb2p_node_navigator_except_node_end_t<allocator_element_t> *exchange(rb2p_node_navigator_except_node_end_t<allocator_element_t> *node_new)
                    {
                        assert(node_new != nullptr);
                        assert(end != nullptr);
                        assert_unique();
                        rb2p_node_navigator_except_node_end_t<allocator_element_t> *node_old;
                        switch(index)
                        {
                        case 1:
                            node_old = std::exchange(leftmost_descendent_of_root, node_new);
                            assert(node_old != nullptr);
                            leftmost_descendent_of_root->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_root;
                            end->my_list_begin() = leftmost_descendent_of_root;
                            leftmost_descendent_of_root->my_list_begin = root;
                            break;
                        case 2:
                            node_old = std::exchange(root, node_new);
                            assert(node_old != nullptr);
                            root->role() = rb2p_node_role_e::root;
                            if(leftmost_descendent_of_root != nullptr)
                                leftmost_descendent_of_root->my_list_begin() = root;
                            else
                                end->my_list_begin() = root;
                            if(rightmost_descendent_of_root != nullptr)
                                root->next() = rightmost_descendent_of_root;
                            else
                                root->next() = tagged_ptr_bit0_setted(end);
                            break;
                        case 3:
                            node_old = std::exchange(rightmost_descendent_of_root, node_new);
                            assert(node_old != nullptr);
                            rightmost_descendent_of_root->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_root;
                            root->next() = rightmost_descendent_of_root;
                            rightmost_descendent_of_root->my_list_begin() = tagged_ptr_bit0_setted(end);
                            break;
                        default:
                            std::unreachable();
                            break;
                        }
                        assert_unique();
                        return node_old;
                    }
                    void set_roles()
                    {
                        if(!leftmost_descendent_of_root_is_nullptr())
                            leftmost_descendent_of_root->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_root;
                        if(root != nullptr)
                            root->role() = rb2p_node_role_e::root;
                        if(!rightmost_descendent_of_root_is_nullptr())
                            rightmost_descendent_of_root->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_root;
                    }
                    void link_if_one()
                    {
                        if(leftmost_descendent_of_root_is_nullptr() && root != nullptr && rightmost_descendent_of_root_is_nullptr())
                            loop_t{
                                .this_ = root,
                            }
                                .link();
                    }
                };
                struct loop_t
                {
                    rb2p_node_navigator_except_node_end_t<allocator_element_t> *this_ = nullptr, *child_right = nullptr, *leftmost_descendent_of_child_right = nullptr, *rightmost_descendent_of_child_left = nullptr, *child_left = nullptr;
                    int index = -1;
                    //                    friend bool operator==(loop_t const &lhs, loop_t const &rhs) = default;
                    template<typename ostream_t>
                    friend ostream_t &&operator<<(ostream_t &&ostream, loop_t const &loop)
                    {
                        return std::forward<ostream_t>(ostream << *loop.this_ << '\n'
                                                               << *loop.child_right << '\n'
                                                               << *loop.leftmost_descendent_of_child_right << '\n'
                                                               << *loop.rightmost_descendent_of_child_left << '\n'
                                                               << *loop.child_left << '\n');
                    }
                    bool leftmost_descendent_of_child_right_is_nullptr() const { return leftmost_descendent_of_child_right == nullptr || leftmost_descendent_of_child_right == child_right; }
                    bool rightmost_descendent_of_child_left_is_nullptr() const { return rightmost_descendent_of_child_left == nullptr || rightmost_descendent_of_child_left == child_left; }
                    void assert_unique() const
                    {
                        std::unordered_set<rb2p_node_navigator_except_node_end_t<allocator_element_t> *> pointers;

                        assert(this_ != nullptr);
                        assert(pointers.insert(this_).second);

                        if(child_right != nullptr)
                            assert(pointers.insert(child_right).second);

                        if(!leftmost_descendent_of_child_right_is_nullptr())
                            assert(pointers.insert(leftmost_descendent_of_child_right).second);

                        if(!rightmost_descendent_of_child_left_is_nullptr())
                            assert(pointers.insert(rightmost_descendent_of_child_left).second);

                        if(child_left != nullptr)
                            assert(pointers.insert(child_left).second);
                    }
                    void link() const
                    {
                        assert(index == -1);
                        assert(this_ != nullptr);
                        assert_unique();
                        if(child_right == nullptr || child_left == nullptr)
                        {
                            assert(leftmost_descendent_of_child_right == nullptr);
                            assert(rightmost_descendent_of_child_left == nullptr);
                            if(child_right == nullptr && child_left == nullptr)
                                this_->my_list_begin() = tagged_ptr_bit0_setted(this_);
                            else if(child_right != nullptr && child_left == nullptr)
                            {
                                this_->my_list_begin() = child_right;
                                child_right->next() = tagged_ptr_bit0_setted(this_);
                            }
                            else if(child_right == nullptr && child_left != nullptr)
                            {
                                this_->my_list_begin() = child_left;
                                child_left->next() = tagged_ptr_bit0_setted(this_);
                            }
                            else std::unreachable();
                        }
                        else
                        {
                            this_->my_list_begin() = child_right;
                            if(leftmost_descendent_of_child_right_is_nullptr() && rightmost_descendent_of_child_left_is_nullptr())
                                child_right->next() = child_left;
                            else if(!leftmost_descendent_of_child_right_is_nullptr() && rightmost_descendent_of_child_left_is_nullptr())
                            {
                                child_right->next() = leftmost_descendent_of_child_right;
                                leftmost_descendent_of_child_right->my_list_begin() = child_left;
                            }
                            else if(leftmost_descendent_of_child_right_is_nullptr() && !rightmost_descendent_of_child_left_is_nullptr())
                            {
                                child_right->next() = rightmost_descendent_of_child_left;
                                rightmost_descendent_of_child_left->my_list_begin() = child_left;
                            }
                            else if(!leftmost_descendent_of_child_right_is_nullptr() && !rightmost_descendent_of_child_left_is_nullptr())
                            {
                                child_right->next() = leftmost_descendent_of_child_right;
                                leftmost_descendent_of_child_right->my_list_begin() = rightmost_descendent_of_child_left;
                                rightmost_descendent_of_child_left->my_list_begin() = child_left;
                            }
                            else std::unreachable();
                            child_left->next() = tagged_ptr_bit0_setted(this_);
                        }
                    }
                    rb2p_node_navigator_except_node_end_t<allocator_element_t> *exchange(rb2p_node_navigator_except_node_end_t<allocator_element_t> *node_new)
                    {
                        assert(node_new != nullptr);
                        assert(this_ != nullptr);
                        assert_unique();
                        rb2p_node_navigator_except_node_end_t<allocator_element_t> *node_old;
                        switch(index)
                        {
                        case 0:
                            node_old = std::exchange(this_, node_new);
                            assert(node_old != nullptr);
                            if(child_right == nullptr)
                            {
                                if(child_left == nullptr)
                                    this_->my_list_begin() = tagged_ptr_bit0_setted(this_);
                                else
                                {
                                    this_->my_list_begin() = child_left;
                                    child_left->next() = tagged_ptr_bit0_setted(this_);
                                }
                            }
                            else
                            {
                                if(child_left == nullptr)
                                {
                                    this_->my_list_begin() = child_right;
                                    child_right->next() = tagged_ptr_bit0_setted(this_);
                                }
                                else
                                {
                                    this_->my_list_begin() = child_right;
                                    child_left->next() = tagged_ptr_bit0_setted(this_);
                                }
                            }
                            break;
                        case 1:
                            node_old = std::exchange(child_right, node_new);
                            assert(node_old != nullptr);
                            this_->my_list_begin() = child_right;
                            if(!leftmost_descendent_of_child_right_is_nullptr())
                                child_right->next() = leftmost_descendent_of_child_right;
                            else if(!rightmost_descendent_of_child_left_is_nullptr())
                                child_right->next() = rightmost_descendent_of_child_left;
                            else if(child_left != nullptr)
                                child_right->next() = child_left;
                            else
                                child_right->next() = tagged_ptr_bit0_setted(this_);
                            break;
                        case 2:
                            node_old = std::exchange(leftmost_descendent_of_child_right, node_new);
                            assert(node_old != nullptr);
                            assert(child_right != nullptr);
                            child_right->next() = leftmost_descendent_of_child_right;
                            if(!rightmost_descendent_of_child_left_is_nullptr())
                                leftmost_descendent_of_child_right->my_list_begin() = rightmost_descendent_of_child_left;
                            else
                            {
                                assert(child_left != nullptr);
                                leftmost_descendent_of_child_right->my_list_begin() = child_left;
                            }
                            break;
                        case 3:
                            node_old = std::exchange(rightmost_descendent_of_child_left, node_new);
                            assert(node_old != nullptr);
                            assert(child_left != nullptr);
                            rightmost_descendent_of_child_left->my_list_begin() = child_left;
                            if(!leftmost_descendent_of_child_right_is_nullptr())
                                leftmost_descendent_of_child_right->my_list_begin() = rightmost_descendent_of_child_left;
                            else
                            {
                                assert(child_right != nullptr);
                                child_right->next() = rightmost_descendent_of_child_left;
                            }
                            break;
                        case 4:
                            node_old = std::exchange(child_left, node_new);
                            assert(node_old != nullptr);
                            child_left->next() = tagged_ptr_bit0_setted(this_);
                            if(!rightmost_descendent_of_child_left_is_nullptr())
                                rightmost_descendent_of_child_left->my_list_begin() = child_left;
                            else if(!leftmost_descendent_of_child_right_is_nullptr())
                                leftmost_descendent_of_child_right->my_list_begin() = child_left;
                            else if(child_right != nullptr)
                                child_right->next() = child_left;
                            else
                                this_->my_list_begin() = child_left;
                            break;
                        default:
                            std::unreachable();
                            break;
                        }
                        assert_unique();
                        return node_old;
                    }
                };
                static rb2p_node_navigator_all_t *get_loop_end_next(rb2p_node_navigator_all_t *this_) // return untagged next
                {
                    switch(this_->role())
                    {
                    case rb2p_node_role_e::end:
                        if(tagged_ptr_bit0_is_setted(this_->my_list_begin()))
                        {
                            assert(tagged_ptr_bit0_unsetted(this_->my_list_begin()) == this_);
                            return this_;
                        }
                        else
                        {
                            rb2p_node_navigator_all_t *current = this_->my_list_begin();
                            switch(current->role())
                            {
                            case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                            case rb2p_node_role_e::root:
                                return current;
                                break;
                            default:
                                std::unreachable();
                                break;
                            }
                        }
                        break;
                    case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                    {
                        assert(!tagged_ptr_bit0_is_setted(this_->my_list_begin()));
                        rb2p_node_navigator_all_t *current = this_->my_list_begin();
                        assert(current->role() == rb2p_node_role_e::root);
                        return current;
                    } break;
                    case rb2p_node_role_e::root:
                        if(tagged_ptr_bit0_is_setted(this_->next()))
                        {
                            rb2p_node_navigator_all_t *current = tagged_ptr_bit0_unsetted(this_->next());
                            assert(current->role() == rb2p_node_role_e::end);
                            return current;
                        }
                        else
                        {
                            rb2p_node_navigator_all_t *current = this_->next();
                            assert(current->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root);
                            return current;
                        }
                        break;
                    case rb2p_node_role_e::child_right_rightmost_descendent_of_root:
                    {
                        assert(tagged_ptr_bit0_is_setted(this_->my_list_begin()));
                        rb2p_node_navigator_all_t *current = tagged_ptr_bit0_unsetted(this_->my_list_begin());
                        assert(current->role() == rb2p_node_role_e::end);
                        return current;
                    } break;
                    default:
                        std::unreachable();
                        break;
                    }
                }
                static loop_end_t get_loop_end(rb2p_node_navigator_all_t *this_)
                {
                    loop_end_t result;
                    switch(this_->role())
                    {
                    case rb2p_node_role_e::end:
                        result.index = 0;
                        break;
                    case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                        result.index = 1;
                        break;
                    case rb2p_node_role_e::root:
                        result.index = 2;
                        break;
                    case rb2p_node_role_e::child_right_rightmost_descendent_of_root:
                        result.index = 3;
                        break;
                    default:
                        std::unreachable();
                        break;
                    }
                    rb2p_node_navigator_all_t *current = this_;
                    do
                    {
                        switch(current->role())
                        {
                        case rb2p_node_role_e::end:
                            result.end = current;
                            break;
                        case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                            result.leftmost_descendent_of_root = static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(current);
                            break;
                        case rb2p_node_role_e::root:
                            result.root = static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(current);
                            break;
                        case rb2p_node_role_e::child_right_rightmost_descendent_of_root:
                            result.rightmost_descendent_of_root = static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(current);
                            break;
                        default:
                            std::unreachable();
                            break;
                        }
                        current = get_loop_end_next(current);
                    }
                    while(current != this_);
                    return result;
                }
                static std::tuple<int, rb2p_node_navigator_except_node_end_t<allocator_element_t> *> get_loop_next(int index, rb2p_node_navigator_except_node_end_t<allocator_element_t> *this_) // return untagged next
                {
                    switch(index)
                    {
                    case 0:
                        assert(this_->role() == rb2p_node_role_e::root || this_->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent || this_->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent);
                        if(tagged_ptr_bit0_is_setted(this_->my_list_begin())) // list is empty
                        {
                            assert(tagged_ptr_bit0_unsetted(this_->my_list_begin()) == this_);
                            return std::make_tuple(0, this_);
                        }
                        else // list is not empty
                        {
                            rb2p_node_navigator_except_node_end_t<allocator_element_t> *current = static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(static_cast<rb2p_node_navigator_all_t *>(this_->my_list_begin()));
                            switch(current->role())
                            {
                            case rb2p_node_role_e::child_left_leftmost_descendent_of_non_root:
                            case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                                return std::make_tuple(4, current);
                                break;
                            case rb2p_node_role_e::child_right_not_a_rightmost_descendent:
                            case rb2p_node_role_e::child_right_rightmost_descendent_of_non_root:
                            case rb2p_node_role_e::child_right_rightmost_descendent_of_root:
                                return std::make_tuple(1, current);
                                break;
                            case rb2p_node_role_e::child_left_not_a_leftmost_descendent:
                                assert(current->my_list_begin() != nullptr && !tagged_ptr_bit0_is_setted(current->my_list_begin()) && current->my_list_begin()->next() == tagged_ptr_bit0_setted(current));
                                return std::make_tuple(1, current);
                                break;
                            default:
                                std::unreachable();
                                break;
                            }
                        }
                        break;
                    case 1:
                        assert(this_->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent || this_->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_non_root || this_->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root || (this_->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent && this_->my_list_begin() != nullptr && !tagged_ptr_bit0_is_setted(this_->my_list_begin()) && this_->my_list_begin()->next() == tagged_ptr_bit0_setted(this_)));
                        if(tagged_ptr_bit0_is_setted(this_->next()))
                        {
                            assert(tagged_ptr_bit0_unsetted(this_->next())->my_list_begin() == this_);
                            return std::make_tuple(0, tagged_ptr_bit0_unsetted(static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(static_cast<rb2p_node_navigator_all_t *>(this_->next()))));
                        }
                        else
                        {
                            rb2p_node_navigator_except_node_end_t<allocator_element_t> *current = static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(static_cast<rb2p_node_navigator_all_t *>(this_->next()));
                            switch(current->role())
                            {
                            case rb2p_node_role_e::child_right_rightmost_descendent_of_non_root:
                                return std::make_tuple(3, current);
                                break;
                            case rb2p_node_role_e::child_left_not_a_leftmost_descendent:
                            case rb2p_node_role_e::child_left_leftmost_descendent_of_non_root:
                            case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                                assert(current->next() == nullptr || tagged_ptr_bit0_is_setted(current->next()));
                                if(current->next() != nullptr && tagged_ptr_bit0_unsetted(current->next())->my_list_begin() == this_)
                                    return std::make_tuple(4, current);
                                else
                                    return std::make_tuple(2, current);
                                break;
                            default:
                                std::unreachable();
                                break;
                            }
                        }
                        break;
                    case 2:
                        assert(this_->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_non_root);
                        {
                            assert(!tagged_ptr_bit0_is_setted(this_->my_list_begin()));
                            rb2p_node_navigator_except_node_end_t<allocator_element_t> *current = static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(static_cast<rb2p_node_navigator_all_t *>(this_->my_list_begin()));
                            switch(current->role())
                            {
                            case rb2p_node_role_e::child_left_not_a_leftmost_descendent:
                            case rb2p_node_role_e::child_left_leftmost_descendent_of_non_root:
                            case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                                return std::make_tuple(4, current);
                                break;
                            case rb2p_node_role_e::child_right_rightmost_descendent_of_non_root:
                                return std::make_tuple(3, current);
                                break;
                            default:
                                std::unreachable();
                                break;
                            }
                        }
                        break;
                    case 3:
                        assert(this_->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_non_root);
                        assert(!tagged_ptr_bit0_is_setted(this_->my_list_begin()));
                        return std::make_tuple(4, static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(static_cast<rb2p_node_navigator_all_t *>(this_->my_list_begin())));
                        break;
                    case 4:
                        assert(this_->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent || this_->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_non_root || this_->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root || (this_->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent && this_->my_list_begin() != nullptr && !tagged_ptr_bit0_is_setted(this_->my_list_begin()) && this_->my_list_begin()->next() == tagged_ptr_bit0_setted(this_)));
                        assert(tagged_ptr_bit0_is_setted(this_->next()));
                        return std::make_tuple(0, tagged_ptr_bit0_unsetted(static_cast<rb2p_node_navigator_except_node_end_t<allocator_element_t> *>(static_cast<rb2p_node_navigator_all_t *>(this_->next()))));
                        break;
                    default:
                        std::unreachable();
                        break;
                    }
                }
                enum class loop_type_e {
                    end,
                    ancestor,
                    parent,
                    my_list
                };
                template<loop_type_e loop_type>
                static loop_t get_loop(rb2p_node_navigator_except_node_end_t<allocator_element_t> *this_)
                {
                    loop_t result;
                    std::array<int, 5> indexes;
                    if constexpr(loop_type == loop_type_e::ancestor)
                    {
                        switch(this_->role())
                        {
                        case rb2p_node_role_e::child_left_leftmost_descendent_of_non_root:
                            indexes = {{2, 3, 4, 0, 1}};
                            break;
                        case rb2p_node_role_e::child_right_rightmost_descendent_of_non_root:
                            indexes = {{3, 4, 0, 1, 2}};
                            break;
                        default:
                            std::unreachable();
                            break;
                        }
                    }
                    else if constexpr(loop_type == loop_type_e::parent)
                    {
                        switch(this_->role())
                        {
                        case rb2p_node_role_e::child_left_not_a_leftmost_descendent:
                        case rb2p_node_role_e::child_left_leftmost_descendent_of_non_root:
                        case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                            indexes = {{4, 0, 1, 2, 3}};
                            break;
                        case rb2p_node_role_e::child_right_not_a_rightmost_descendent:
                        case rb2p_node_role_e::child_right_rightmost_descendent_of_non_root:
                        case rb2p_node_role_e::child_right_rightmost_descendent_of_root:
                            indexes = {{1, 2, 3, 4, 0}};
                            break;
                        default:
                            std::unreachable();
                            break;
                        }
                    }
                    else if constexpr(loop_type == loop_type_e::my_list)
                    {
                        switch(this_->role())
                        {
                        case rb2p_node_role_e::root:
                        case rb2p_node_role_e::child_left_not_a_leftmost_descendent:
                        case rb2p_node_role_e::child_right_not_a_rightmost_descendent:
                            indexes = {{0, 1, 2, 3, 4}};
                            break;
                        default:
                            std::unreachable();
                            break;
                        }
                    }
                    else std::unreachable();
                    std::tuple<int, rb2p_node_navigator_except_node_end_t<allocator_element_t> *> current_index_and_node = std::make_tuple(indexes[0], this_);
                    result.index = indexes[0];
                    for(int index : indexes)
                    {
                        if(index == std::get<0>(current_index_and_node))
                        {
                            switch(std::get<0>(current_index_and_node))
                            {
                            case 0:
                                result.this_ = std::get<1>(current_index_and_node);
                                break;
                            case 1:
                                result.child_right = std::get<1>(current_index_and_node);
                                break;
                            case 2:
                                result.leftmost_descendent_of_child_right = std::get<1>(current_index_and_node);
                                break;
                            case 3:
                                result.rightmost_descendent_of_child_left = std::get<1>(current_index_and_node);
                                break;
                            case 4:
                                result.child_left = std::get<1>(current_index_and_node);
                                break;
                            default:
                                std::unreachable();
                                break;
                            }
                            current_index_and_node = std::apply(get_loop_next, current_index_and_node);
                            index = std::get<0>(current_index_and_node);
                        }
                    }
                    assert(std::get<0>(current_index_and_node) == indexes[0]);
                    assert(std::get<1>(current_index_and_node) == this_);
                    return result;
                }
            };
            template<typename allocator_element_t>
            struct rb2p_node_navigator_except_node_end_t: rb2p_node_navigator_all_t<allocator_element_t>
            {
                rb2p_node_navigator_all_t<allocator_element_t> *next_ = nullptr;
            };
            template<typename config_t>
            struct rb2p_node_end_t: rb2p_node_navigator_all_t<typename config_t::allocator_element_t>, rb2p_node_navigator_all_t<typename config_t::allocator_element_t>::template node_end_functions_t<rb2p_node_end_t<config_t>>, add_node_count_member_t<static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_node_end>
            {
                rb2p_node_end_t()
                {
                    this->role() = rb2p_node_role_e::end;
                    typename rb2p_node_navigator_all_t<typename config_t::allocator_element_t>::loop_end_t{
                        .end = this,
                    }
                        .link();
                }
                typename config_t::accumulator_t accumulator;
            };
            template<typename config_t>
            struct rb2p_node_t: rb2p_node_navigator_except_node_end_t<typename config_t::allocator_element_t>, add_accumulated_storage_member_t<typename config_t::pointer_element_t, typename config_t::accumulated_storage_t, rb2p_node_t<config_t>>, add_node_count_member_t<(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_each_node_except_node_end)>
            {
                alignas(typename config_t::element_t) std::byte element_buffer[sizeof(typename config_t::element_t)]; // element_t element;
                typename config_t::const_pointer_element_t p_element() const { return std::pointer_traits<typename config_t::const_pointer_element_t>::pointer_to(*reinterpret_cast<typename config_t::element_t const *>(&element_buffer)); }
                typename config_t::pointer_element_t p_element() { return std::pointer_traits<typename config_t::pointer_element_t>::pointer_to(*reinterpret_cast<typename config_t::element_t *>(&element_buffer)); }
            };
            template<typename ostream_t, typename allocator_element_t_>
            ostream_t &&operator<<(ostream_t &&ostream, rb2p_node_navigator_all_t<allocator_element_t_> &rb2p_node_navigator_all)
            {
                //                struct config_t
                //                {
                //                    using element_t = int;
                //                    using allocator_element_t = std::allocator<int>;
                //                    using accumulator_t = void;
                //                    using augmented_sequence_physical_representation_t = std::integral_constant<augmented_sequence_physical_representation_e, augmented_sequence_physical_representation_e::rb2p>;
                //                    using augmented_sequence_size_management_t = std::integral_constant<augmented_sequence_size_management_e, augmented_sequence_size_management_e::at_each_node_except_node_end>;

                //                    using pointer_element_t = int *;
                //                    using const_pointer_element_t = int const *;
                //                    using accumulated_storage_t = void;
                //                };
                //                rb2p_node_t<config_t> &rb2p_node = static_cast<rb2p_node_t<config_t> &>(rb2p_node_navigator_all);
                //                if(std::addressof(rb2p_node) == nullptr)
                //                    return std::forward<ostream_t>(ostream << std::addressof(rb2p_node));
                //                else
                //                    return std::forward<ostream_t>(ostream << std::addressof(rb2p_node) << rb2p_node.color() << std::map<rb2p_node_role_e, std::string>{
                //                                                                                                                    {rb2p_node_role_e::end, "end"},
                //                                                                                                                    {rb2p_node_role_e::root, "root"},
                //                                                                                                                    {rb2p_node_role_e::child_left_not_a_leftmost_descendent, "child_left_not_a_leftmost_descendent"},
                //                                                                                                                    {rb2p_node_role_e::child_left_leftmost_descendent_of_non_root, "child_left_leftmost_descendent_of_non_root"},
                //                                                                                                                    {rb2p_node_role_e::child_left_leftmost_descendent_of_root, "child_left_leftmost_descendent_of_root"},
                //                                                                                                                    {rb2p_node_role_e::child_right_not_a_rightmost_descendent, "child_right_not_a_rightmost_descendent"},
                //                                                                                                                    {rb2p_node_role_e::child_right_rightmost_descendent_of_non_root, "child_right_rightmost_descendent_of_non_root"},
                //                                                                                                                    {rb2p_node_role_e::child_right_rightmost_descendent_of_root, "child_right_rightmost_descendent_of_root"},
                //                                                                                                                }
                //                                                                                                                    .at(rb2p_node.role())
                //                                                           << rb2p_node.my_list_begin_ << rb2p_node.next_ << *rb2p_node.p_element());
                return std::forward<ostream_t>(ostream);
            }

            template<bool is_const_, bool is_reversed_, typename config_t>
            struct rb2p_iterator_t
            {
                static constexpr bool is_const = is_const_;
                static constexpr bool is_reversed = is_reversed_;

                using element_t = typename config_t::element_t;
                using pointer_element_t = typename config_t::pointer_element_t;

                using accumulated_storage_t = typename config_t::accumulated_storage_t;
                using pointer_accumulated_storage_t = typename std::pointer_traits<pointer_element_t>::template rebind<conditional_const_t<is_const, accumulated_storage_t>>;

                using navigator_t = rb2p_node_navigator_all_t<typename config_t::allocator_element_t>;
                using pointer_navigator_t = typename std::pointer_traits<pointer_element_t>::template rebind<navigator_t>;

                using navigator_except_node_end_t = rb2p_node_navigator_except_node_end_t<typename config_t::allocator_element_t>;
                using node_end_t = rb2p_node_end_t<config_t>;
                using node_t = rb2p_node_t<config_t>;

                static constexpr typename navigator_t::p_child_left_or_right_all_t p_child_left_all = navigator_t::p_child_left_or_right_all_t::template make_p_child_left_or_right<is_reversed>(false), p_child_right_all = navigator_t::p_child_left_or_right_all_t::template make_p_child_left_or_right<is_reversed>(true);
                static constexpr typename navigator_t::p_child_left_or_right_except_node_end_t p_child_left = navigator_t::p_child_left_or_right_except_node_end_t::template make_p_child_left_or_right<is_reversed>(false), p_child_right = navigator_t::p_child_left_or_right_except_node_end_t::template make_p_child_left_or_right<is_reversed>(true);

                template<bool is_reversed_predecessor>
                static navigator_t *predecessor(navigator_t *node)
                {
                    constexpr bool is_reversed = rb2p_iterator_t::is_reversed ^ is_reversed_predecessor;

                    if constexpr(!is_reversed) // predecessor
                    {
                        switch(node->role())
                        {
                        case rb2p_node_role_e::end:
                            return tagged_ptr_bit0_unsetted(node->*p_child_left_all);
                            break;
                        case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                            return tagged_ptr_bit0_unsetted(node->*p_child_left_all);
                            break;
                        case rb2p_node_role_e::child_right_rightmost_descendent_of_non_root:
                        case rb2p_node_role_e::child_right_rightmost_descendent_of_root:
                            return node->parent();
                            break;
                        case rb2p_node_role_e::root:
                        {
                            std::tuple<int, navigator_except_node_end_t *> current_index_and_node = std::make_tuple(0, static_cast<navigator_except_node_end_t *>(node));
                            do
                                current_index_and_node = std::apply(navigator_t::get_loop_next, current_index_and_node);
                            while(!(std::get<0>(current_index_and_node) == 3 || std::get<0>(current_index_and_node) == 4 || std::get<0>(current_index_and_node) == 0));
                            if(std::get<0>(current_index_and_node) != 0)
                                return std::get<1>(current_index_and_node);
                            else
                                return tagged_ptr_bit0_unsetted(node->*p_child_left_all);
                        } break;
                        case rb2p_node_role_e::child_left_not_a_leftmost_descendent:
                        {
                            std::tuple<int, navigator_except_node_end_t *> current_index_and_node = std::make_tuple(0, static_cast<navigator_except_node_end_t *>(node));
                            do
                                current_index_and_node = std::apply(navigator_t::get_loop_next, current_index_and_node);
                            while(!(std::get<0>(current_index_and_node) == 3 || std::get<0>(current_index_and_node) == 4));
                            return std::get<1>(current_index_and_node);
                        } break;
                        case rb2p_node_role_e::child_right_not_a_rightmost_descendent:
                        {
                            std::tuple<int, navigator_except_node_end_t *> current_index_and_node = std::make_tuple(0, static_cast<navigator_except_node_end_t *>(node));
                            do
                                current_index_and_node = std::apply(navigator_t::get_loop_next, current_index_and_node);
                            while(!(std::get<0>(current_index_and_node) == 3 || std::get<0>(current_index_and_node) == 4 || std::get<0>(current_index_and_node) == 0));
                            if(std::get<0>(current_index_and_node) != 0)
                                return std::get<1>(current_index_and_node);
                            else
                                return node->parent();
                        } break;
                        case rb2p_node_role_e::child_left_leftmost_descendent_of_non_root:
                        {
                            std::tuple<int, navigator_except_node_end_t *> current_index_and_node = std::make_tuple(2, static_cast<navigator_except_node_end_t *>(node));
                            do
                                current_index_and_node = std::apply(navigator_t::get_loop_next, current_index_and_node);
                            while(std::get<0>(current_index_and_node) != 0);
                            return std::get<1>(current_index_and_node);
                        } break;
                        default:
                            std::unreachable();
                            break;
                        }
                    }
                    else // successor
                    {
                        switch(node->role())
                        {
                        case rb2p_node_role_e::end:
                            return tagged_ptr_bit0_unsetted(node->*p_child_right_all);
                            break;
                        case rb2p_node_role_e::child_right_rightmost_descendent_of_root:
                            return tagged_ptr_bit0_unsetted(node->*p_child_right_all);
                            break;
                        case rb2p_node_role_e::child_left_leftmost_descendent_of_non_root:
                        case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                            return node->parent();
                            break;
                        case rb2p_node_role_e::root:
                        {
                            std::tuple<int, navigator_except_node_end_t *> current_index_and_node = std::make_tuple(0, static_cast<navigator_except_node_end_t *>(node));
                            current_index_and_node = std::apply(navigator_t::get_loop_next, current_index_and_node);
                            switch(std::get<0>(current_index_and_node))
                            {
                            case 1:
                            {
                                std::tuple<int, navigator_except_node_end_t *> current_index_and_node2 = std::apply(navigator_t::get_loop_next, current_index_and_node);
                                if(std::get<0>(current_index_and_node2) == 2)
                                    return std::get<1>(current_index_and_node2);
                                else
                                    return std::get<1>(current_index_and_node);
                            } break;
                            case 4:
                            case 0:
                                return tagged_ptr_bit0_unsetted(node->*p_child_right_all);
                                break;
                            default:
                                std::unreachable();
                                break;
                            }
                        } break;
                        case rb2p_node_role_e::child_right_not_a_rightmost_descendent:
                        {
                            std::tuple<int, navigator_except_node_end_t *> current_index_and_node = std::make_tuple(0, static_cast<navigator_except_node_end_t *>(node));
                            current_index_and_node = std::apply(navigator_t::get_loop_next, current_index_and_node);
                            switch(std::get<0>(current_index_and_node))
                            {
                            case 1:
                            {
                                std::tuple<int, navigator_t *> current_index_and_node2 = std::apply(navigator_t::get_loop_next, current_index_and_node);
                                if(std::get<0>(current_index_and_node2) == 2)
                                    return std::get<1>(current_index_and_node2);
                                else
                                    return std::get<1>(current_index_and_node);
                            } break;
                            default:
                                std::unreachable();
                                break;
                            }
                        } break;
                        case rb2p_node_role_e::child_left_not_a_leftmost_descendent:
                        {
                            std::tuple<int, navigator_except_node_end_t *> current_index_and_node = std::make_tuple(0, static_cast<navigator_except_node_end_t *>(node));
                            current_index_and_node = std::apply(navigator_t::get_loop_next, current_index_and_node);
                            switch(std::get<0>(current_index_and_node))
                            {
                            case 1:
                            {
                                std::tuple<int, navigator_t *> current_index_and_node2 = std::apply(navigator_t::get_loop_next, current_index_and_node);
                                if(std::get<0>(current_index_and_node2) == 2)
                                    return std::get<1>(current_index_and_node2);
                                else
                                    return std::get<1>(current_index_and_node);
                            } break;
                            case 4:
                            case 0:
                                return node->parent();
                                break;
                            default:
                                std::unreachable();
                                break;
                            }
                        } break;
                        case rb2p_node_role_e::child_right_rightmost_descendent_of_non_root:
                        {
                            std::tuple<int, navigator_except_node_end_t *> current_index_and_node = std::make_tuple(3, static_cast<navigator_except_node_end_t *>(node));
                            do
                                current_index_and_node = std::apply(navigator_t::get_loop_next, current_index_and_node);
                            while(std::get<0>(current_index_and_node) != 0);
                            return std::get<1>(current_index_and_node);
                        } break;
                        default:
                            std::unreachable();
                            break;
                        }
                    }
                }

                navigator_t *current_node = nullptr;
                rb2p_iterator_t(navigator_t *current_node)
                    : current_node(current_node)
                {}
                bool is_end() const
                {
                    assert(current_node != nullptr);
                    return current_node->role() == rb2p_node_role_e::end;
                }

                using non_const_iterator_t = rb2p_iterator_t<false, is_reversed, config_t>;
                using const_iterator_t = rb2p_iterator_t<true, is_reversed, config_t>;
                non_const_iterator_t to_non_const() const { return {current_node}; }
                const_iterator_t to_const() const { return {current_node}; }
                rb2p_iterator_t(non_const_iterator_t const &rhs)
                    requires(is_const) // https://quuxplusone.github.io/blog/2018/12/01/const-iterator-antipatterns/
                    : current_node(rhs.current_node)
                {}
                const_iterator_t &operator=(non_const_iterator_t const &rhs) &
                        requires(is_const)
                {
                    current_node = rhs.current_node;
                    return *this;
                }

                // std::input_or_output_iterator / std::weakly_incrementable
                rb2p_iterator_t(rb2p_iterator_t const &) = default;
                rb2p_iterator_t &operator=(rb2p_iterator_t const &) & = default;
                using difference_type = std::ptrdiff_t;
                rb2p_iterator_t &operator++() &
                {
                    assert(current_node != nullptr);
                    current_node = predecessor<true>(current_node);
                    return *this;
                }
                rb2p_iterator_t operator++(int) &
                {
                    rb2p_iterator_t temp = *this;
                    operator++();
                    return temp;
                }

                // std::forward_iterator / std::sentinel_for / std::semiregular, std::forward_iterator / std::incrementable / std::regular
                rb2p_iterator_t() = default;

                // std::input_iterator / std::indirectly_readable
                using value_type = element_t;
                using pointer = typename std::pointer_traits<pointer_element_t>::template rebind<conditional_const_t<is_const, value_type>>;
                using reference = conditional_const_t<is_const, value_type> &;
                reference operator*() const &
                {
                    assert(current_node != nullptr);
                    assert(current_node->role() != rb2p_node_role_e::end);
                    return conditional_as_const<is_const>(*static_cast<node_t *>(std::to_address(current_node))->p_element());
                }
                pointer to_pointer_element() const & { return std::pointer_traits<pointer>::pointer_to(operator*()); }
                pointer operator->() const & { return to_pointer_element(); }
                static rb2p_iterator_t from_element_pointer(pointer ptr) { return {std::pointer_traits<pointer_navigator_t>::pointer_to(*reinterpret_cast<node_t *>(const_cast<std::byte *>(reinterpret_cast<conditional_const_t<is_const, std::byte> *>(std::to_address(ptr))) - offsetof(node_t, element_buffer)))}; }

                pointer_accumulated_storage_t to_pointer_accumulated_storage() const &
                    requires(!std::is_same_v<accumulated_storage_t, void>)
                {
                    assert(current_node != nullptr);
                    assert(current_node->role() != rb2p_node_role_e::end);
                    return std::pointer_traits<pointer_accumulated_storage_t>::pointer_to(conditional_as_const<is_const>(*static_cast<node_t *>(std::to_address(current_node))->p_accumulated_storage()));
                }
                static rb2p_iterator_t from_accumulated_storage_pointer(pointer_accumulated_storage_t ptr)
                    requires(!std::is_same_v<accumulated_storage_t, void>)
                {
                    return {std::pointer_traits<pointer_navigator_t>::pointer_to(*reinterpret_cast<node_t *>(const_cast<std::byte *>(reinterpret_cast<conditional_const_t<is_const, std::byte> *>(std::to_address(ptr))) - offsetof(node_t, accumulated_storage_buffer)))};
                }

                // std::forward_iterator / std::sentinel_for / __WeaklyEqualityComparableWith, std::forward_iterator / std::incrementable / std::regular
                template<std::bool_constant<is_const> * = nullptr>
                    requires(is_const)
                friend bool operator==(const_iterator_t const &lhs, const_iterator_t const &rhs)
                {
                    assert((lhs.current_node != nullptr) == (rhs.current_node != nullptr));
                    return lhs.current_node == rhs.current_node;
                }
                template<std::bool_constant<is_const> * = nullptr>
                    requires(!is_const)
                friend bool operator==(const_iterator_t const &lhs, non_const_iterator_t const &rhs)
                {
                    return lhs == rhs.to_const();
                }
                template<std::bool_constant<is_const> * = nullptr>
                    requires(!is_const)
                friend bool operator==(non_const_iterator_t const &lhs, const_iterator_t const &rhs)
                {
                    return lhs.to_const() == rhs;
                }
                template<std::bool_constant<is_const> * = nullptr>
                    requires(!is_const)
                friend bool operator==(non_const_iterator_t const &lhs, non_const_iterator_t const &rhs)
                {
                    return lhs.to_const() == rhs.to_const();
                }
                friend bool operator==(rb2p_iterator_t const &lhs, [[maybe_unused]] std::default_sentinel_t const &rhs)
                {
                    assert(lhs.current_node != nullptr);
                    return lhs.current_node->role() == rb2p_node_role_e::end;
                }

                // std::bidirectional_iterator
                rb2p_iterator_t &operator--() &
                {
                    assert(current_node != nullptr);
                    current_node = predecessor<false>(current_node);
                    return *this;
                }
                rb2p_iterator_t operator--(int) &
                {
                    rb2p_iterator_t temp = *this;
                    operator--();
                    return temp;
                }

                static constexpr bool support_random_access = static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_each_node_except_node_end;
                static std::size_t size_from_node_end(node_end_t *node_end)
                    requires(support_random_access)
                {
                    if(node_end->my_list_begin() == tagged_ptr_bit0_setted(node_end))
                        return 0;
                    else
                        return static_cast<node_t *>(tagged_ptr_bit0_unsetted(node_end->parent_all()))->node_count;
                }

                // std::random_access_iterator / std::totally_ordered / __PartiallyOrderedWith
                std::tuple<std::size_t, std::size_t> index_impl() const &
                    requires(support_random_access)
                {
                    assert(current_node != nullptr);
                    if(current_node->role() == rb2p_node_role_e::end)
                        return std::make_tuple(size_from_node_end(static_cast<node_end_t *>(current_node)), size_from_node_end(static_cast<node_end_t *>(current_node)));
                    else
                    {
                        navigator_t *current_node = this->current_node;
                        std::size_t current_index = 0;
                        while(true)
                        {
                            if(current_node->*p_child_left != nullptr)
                                current_index += static_cast<node_t *>(static_cast<navigator_t *>(current_node->*p_child_left))->node_count;
                            while(true)
                            {
                                typename navigator_t::template parent_info_t<is_reversed> parent_info(current_node);
                                if(parent_info.is_end())
                                    goto reached_root;
                                else
                                {
                                    current_node = parent_info.parent;
                                    if(parent_info.is_left_or_right_child_of_parent)
                                    {
                                        ++current_index;
                                        break;
                                    }
                                }
                            }
                        }
                    reached_root:;
                        return std::make_tuple(current_index, static_cast<node_t *>(current_node)->node_count);
                    }
                }
                template<std::bool_constant<is_const> * = nullptr>
                    requires(support_random_access && is_const)
                friend std::strong_ordering operator<=>(const_iterator_t const &lhs, const_iterator_t const &rhs)
                {
                    return std::get<0>(lhs.index_impl()) <=> std::get<0>(rhs.index_impl());
                }
                template<std::bool_constant<is_const> * = nullptr>
                    requires(support_random_access && !is_const)
                friend std::strong_ordering operator<=>(const_iterator_t const &lhs, non_const_iterator_t const &rhs)
                {
                    return lhs <=> rhs.to_const();
                }
                template<std::bool_constant<is_const> * = nullptr>
                    requires(support_random_access && !is_const)
                friend std::strong_ordering operator<=>(non_const_iterator_t const &lhs, const_iterator_t const &rhs)
                {
                    return lhs.to_const() <=> rhs;
                }
                template<std::bool_constant<is_const> * = nullptr>
                    requires(support_random_access && !is_const)
                friend std::strong_ordering operator<=>(non_const_iterator_t const &lhs, non_const_iterator_t const &rhs)
                {
                    return lhs.to_const() <=> rhs.to_const();
                }
                friend std::strong_ordering operator<=>(rb2p_iterator_t const &lhs, [[maybe_unused]] std::default_sentinel_t const &rhs)
                    requires(support_random_access)
                {
                    assert(lhs.current_node != nullptr);
                    if(lhs.current_node->role() == rb2p_node_role_e::end)
                        return std::strong_ordering::equal;
                    else
                        return std::strong_ordering::less;
                }
                std::size_t index() const &
                    requires(support_random_access)
                {
                    assert(current_node != nullptr);
                    return std::get<0>(index_impl());
                }
                template<std::bool_constant<is_const> * = nullptr>
                    requires(support_random_access && is_const)
                friend std::ptrdiff_t operator-(const_iterator_t const &lhs, const_iterator_t const &rhs)
                {
                    assert(lhs.current_node != nullptr);
                    assert(rhs.current_node != nullptr);
                    return static_cast<std::ptrdiff_t>(std::get<0>(lhs.index_impl())) - static_cast<std::ptrdiff_t>(std::get<0>(rhs.index_impl()));
                }
                template<std::bool_constant<is_const> * = nullptr>
                    requires(support_random_access && !is_const)
                friend std::ptrdiff_t operator-(const_iterator_t const &lhs, non_const_iterator_t const &rhs)
                {
                    return lhs - rhs.to_const();
                }
                template<std::bool_constant<is_const> * = nullptr>
                    requires(support_random_access && !is_const)
                friend std::ptrdiff_t operator-(non_const_iterator_t const &lhs, const_iterator_t const &rhs)
                {
                    return lhs.to_const() - rhs;
                }
                template<std::bool_constant<is_const> * = nullptr>
                    requires(support_random_access && !is_const)
                friend std::ptrdiff_t operator-(non_const_iterator_t const &lhs, non_const_iterator_t const &rhs)
                {
                    return lhs.to_const() - rhs.to_const();
                }
                friend std::ptrdiff_t operator-([[maybe_unused]] std::default_sentinel_t const &lhs, rb2p_iterator_t const &rhs)
                    requires(support_random_access)
                {
                    assert(rhs.current_list_node != nullptr);
                    auto [index, size] = rhs.index_impl();
                    return static_cast<std::ptrdiff_t>(size) - static_cast<std::ptrdiff_t>(index);
                }
                friend std::ptrdiff_t operator-(rb2p_iterator_t const &lhs, [[maybe_unused]] std::default_sentinel_t const &rhs)
                    requires(support_random_access)
                {
                    return -(rhs - lhs);
                }

                // std::random_access_iterator
                template<bool is_reversed_move_impl>
                    void move_impl(std::size_t distance) &
                        requires(support_random_access)
                {
                    constexpr bool is_reversed = rb2p_iterator_t::is_reversed ^ is_reversed_move_impl;

                    static constexpr typename navigator_t::p_child_left_or_right_except_node_end_t p_child_left = navigator_t::p_child_left_or_right_except_node_end_t::template make_p_child_left_or_right<is_reversed>(false), p_child_right = navigator_t::p_child_left_or_right_except_node_end_t::template make_p_child_left_or_right<is_reversed>(true);

                    assert(distance != 0);
                    auto find_in_tree = [](auto &this_, navigator_t *root, std::size_t index) -> navigator_t *
                    {
                        if(root->*p_child_left != nullptr)
                        {
                            if(index < static_cast<node_t *>(static_cast<navigator_t *>(root->*p_child_left))->node_count)
                                return this_(this_, root->*p_child_left, index);
                            else
                                index -= static_cast<node_t *>(static_cast<navigator_t *>(root->*p_child_left))->node_count;

                            if(index == 0)
                                return root;
                            else
                                --index;

                            assert(root->*p_child_right != nullptr);
                            return this_(this_, root->*p_child_right, index);
                        }
                        else
                        {
                            if(index == 0)
                                return root;
                            else
                                --index;

                            assert(root->*p_child_right != nullptr);
                            return this_(this_, root->*p_child_right, index);
                        }
                    };
                    while(true)
                    {
                        --distance;
                        if(current_node->*p_child_right != nullptr)
                        {
                            if(distance < static_cast<node_t *>(static_cast<navigator_t *>(current_node->*p_child_right))->node_count)
                            {
                                current_node = find_in_tree(find_in_tree, current_node->*p_child_right, distance);
                                return;
                            }
                            else
                                distance -= static_cast<node_t *>(static_cast<navigator_t *>(current_node->*p_child_right))->node_count;
                        }
                        while(true)
                        {
                            typename navigator_t::template parent_info_t<is_reversed> parent_info(current_node);
                            if(parent_info.is_end())
                            {
                                distance %= size_from_node_end(static_cast<node_end_t *>(tagged_ptr_bit0_unsetted(parent_info.parent))) + 1;
                                if(distance == 0)
                                    current_node = tagged_ptr_bit0_unsetted(parent_info.parent);
                                else
                                    current_node = find_in_tree(find_in_tree, current_node, distance - 1);
                                return;
                            }
                            else
                            {
                                current_node = parent_info.parent;
                                if(!parent_info.is_left_or_right_child_of_parent)
                                    break;
                            }
                        }
                        if(distance == 0)
                            return;
                    }
                }
                rb2p_iterator_t &operator+=(std::ptrdiff_t offset) &
                        requires(support_random_access)
                {
                    assert(current_node != nullptr);
                    if(offset == 0)
                        ;
                    else
                    {
                        if(current_node->role() == rb2p_node_role_e::end)
                        {
                            offset %= static_cast<std::ptrdiff_t>(size_from_node_end(static_cast<node_end_t *>(current_node))) + 1;
                            auto find_in_tree = [](auto &this_, navigator_t *root, std::size_t index) -> navigator_t *
                            {
                                if(root->*p_child_left != nullptr)
                                {
                                    if(index < static_cast<node_t *>(static_cast<navigator_t *>(root->*p_child_left))->node_count)
                                        return this_(this_, root->*p_child_left, index);
                                    else
                                        index -= static_cast<node_t *>(static_cast<navigator_t *>(root->*p_child_left))->node_count;

                                    if(index == 0)
                                        return root;
                                    else
                                        --index;

                                    assert(root->*p_child_right != nullptr);
                                    return this_(this_, root->*p_child_right, index);
                                }
                                else
                                {
                                    if(index == 0)
                                        return root;
                                    else
                                        --index;

                                    assert(root->*p_child_right != nullptr);
                                    return this_(this_, root->*p_child_right, index);
                                }
                            };
                            if(offset == 0)
                                ;
                            else if(offset > 0)
                            {
                                current_node = find_in_tree(find_in_tree, tagged_ptr_bit0_unsetted(current_node->parent_all()), offset - 1);
                            }
                            else if(offset < 0)
                            {
                                current_node = find_in_tree(find_in_tree, tagged_ptr_bit0_unsetted(current_node->parent_all()), offset + (static_cast<std::ptrdiff_t>(size_from_node_end(static_cast<node_end_t *>(current_node))) + 1) - 1);
                            }
                            else std::unreachable();
                        }
                        else
                        {
                            if(offset == 0)
                                ;
                            else if(offset > 0)
                                move_impl<false>(offset);
                            else if(offset < 0)
                                move_impl<true>(-offset);
                            else std::unreachable();
                        }
                    }
                    return *this;
                }
                rb2p_iterator_t &operator-=(std::ptrdiff_t offset) &
                        requires(support_random_access)
                {
                    return operator+=(-offset);
                }
                rb2p_iterator_t operator+(std::ptrdiff_t offset) const &
                    requires(support_random_access)
                {
                    rb2p_iterator_t temp = *this;
                    temp += offset;
                    return temp;
                }
                rb2p_iterator_t operator-(std::ptrdiff_t offset) const &
                    requires(support_random_access)
                {
                    return operator+(-offset);
                }
                friend rb2p_iterator_t operator+(std::ptrdiff_t offset, rb2p_iterator_t const &this_)
                    requires(support_random_access)
                {
                    return *this_ + offset;
                }
                reference operator[](std::ptrdiff_t offset) const &
                    requires(support_random_access)
                {
                    return *(*this + offset);
                }

                using iterator_concept = std::bidirectional_iterator_tag;
            };

            template<bool is_reversed_, typename config_t>
            struct rb2p_functor_t
            {
                static constexpr bool is_reversed = is_reversed_;

                using element_t = typename config_t::element_t;
                using allocator_element_t = typename config_t::allocator_element_t;
                using accumulated_storage_t = typename config_t::accumulated_storage_t;
                using accumulator_t = typename config_t::accumulator_t;

                using navigator_t = rb2p_node_navigator_all_t<typename config_t::allocator_element_t>;
                using navigator_except_node_end_t = rb2p_node_navigator_except_node_end_t<typename config_t::allocator_element_t>;
                using node_end_t = rb2p_node_end_t<config_t>;
                using node_t = rb2p_node_t<config_t>;

                static constexpr bool uses_siblings = []() consteval->bool
                {
                    if constexpr(requires { std::convertible_to<decltype(accumulator_t::uses_siblings), bool>; })
                        return accumulator_t::uses_siblings;
                    return false;
                }
                ();

                static constexpr typename navigator_t::p_child_left_or_right_all_t p_child_left_all = navigator_t::p_child_left_or_right_all_t::template make_p_child_left_or_right<is_reversed>(false), p_child_right_all = navigator_t::p_child_left_or_right_all_t::template make_p_child_left_or_right<is_reversed>(true);
                static constexpr typename navigator_t::p_child_left_or_right_except_node_end_t p_child_left = navigator_t::p_child_left_or_right_except_node_end_t::template make_p_child_left_or_right<is_reversed>(false), p_child_right = navigator_t::p_child_left_or_right_except_node_end_t::template make_p_child_left_or_right<is_reversed>(true);

                static bool empty(node_end_t *node_end)
                {
                    if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::no_size)
                        return node_end->my_list_begin() == tagged_ptr_bit0_setted(node_end);
                    else if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_node_end)
                        return node_end->my_list_begin() == tagged_ptr_bit0_setted(node_end);
                    //                        return node_end->node_count == 0;
                    else if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_each_node_except_node_end)
                        return node_end->my_list_begin() == tagged_ptr_bit0_setted(node_end);
                    else
                        std::unreachable();
                }
                static std::size_t size(node_end_t *node_end)
                {
                    if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::no_size)
                        return std::ranges::distance(std::ranges::next(rb2p_iterator_t<false, false, config_t>(node_end)), rb2p_iterator_t<false, false, config_t>(node_end));
                    else if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_node_end)
                        return node_end->node_count;
                    else if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_each_node_except_node_end)
                    {
                        if(node_end->my_list_begin() == tagged_ptr_bit0_setted(node_end))
                            return 0;
                        else
                        {
                            node_t *root = static_cast<node_t *>(tagged_ptr_bit0_unsetted(node_end->parent_all()));
                            return root->node_count;
                        }
                    }
                    else
                        std::unreachable();
                }
                static bool one_provided_not_empty(node_end_t *node_end)
                {
                    if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::no_size)
                    {
                        assert(node_end->my_list_begin() != tagged_ptr_bit0_setted(node_end));
                        navigator_t *root = tagged_ptr_bit0_unsetted(node_end->parent_all());
                        return root->my_list_begin() == tagged_ptr_bit0_setted(root);
                    }
                    else if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_node_end)
                    {
                        assert(node_end->node_count != 0);
                        return node_end->node_count == 1;
                    }
                    else if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_each_node_except_node_end)
                    {
                        assert(node_end->my_list_begin() != tagged_ptr_bit0_setted(node_end));
                        node_t *root = static_cast<node_t *>(tagged_ptr_bit0_unsetted(node_end->parent_all()));
                        return root->node_count == 1;
                    }
                    else
                        std::unreachable();
                }

                struct refresh_node_count_and_accumulated_storage_schedule_t
                {
                    navigator_except_node_end_t *node;
                };
                struct refresh_node_count_and_accumulated_storage_and_above_schedule_t
                {
                    navigator_except_node_end_t *node;
                };
                struct refresh_node_count_and_accumulated_storage_and_above_until_schedule_t
                {
                    navigator_except_node_end_t *node;
                    navigator_except_node_end_t *node_end;
                };
                template<bool from_left_to_right_or_from_right_to_left>
                struct move_node_count_and_accumulated_storage_schedule_t
                {
                    navigator_except_node_end_t *node_left;
                    navigator_except_node_end_t *node_right;
                };
                using schedules_t = std::conditional_t<uses_siblings, std::vector<std::variant<refresh_node_count_and_accumulated_storage_schedule_t, refresh_node_count_and_accumulated_storage_and_above_schedule_t, refresh_node_count_and_accumulated_storage_and_above_until_schedule_t, move_node_count_and_accumulated_storage_schedule_t<false>, move_node_count_and_accumulated_storage_schedule_t<true>>>, std::nullptr_t>;

                static void refresh_node_count(navigator_except_node_end_t *node)
                {
                    if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_each_node_except_node_end)
                        static_cast<node_t *>(node)->node_count = (node->child_left() != nullptr ? static_cast<node_t *>(static_cast<navigator_except_node_end_t *>(node->child_left()))->node_count : 0) + 1 + (node->child_right() != nullptr ? static_cast<node_t *>(static_cast<navigator_except_node_end_t *>(node->child_right()))->node_count : 0);
                }
                static void refresh_accumulated_storage(accumulator_t const &accumulator, navigator_except_node_end_t *node)
                {
                    if constexpr(!std::is_same_v<accumulated_storage_t, void>)
                    {
                        auto get_left_operand = [&](auto return_accumulated_tuple)
                        { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> void
                          {
                              if(node->child_left() == nullptr)
                                  return_accumulated_tuple(accumulated_tuple_so_far);
                              else
                                  return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*static_cast<node_t *>(static_cast<navigator_except_node_end_t *>(node->child_left()))->p_accumulated_storage()))));
                          }; };
                        auto get_middle_operand = [&](auto return_accumulated_tuple)
                        { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> void
                          {
                              return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::cref(*static_cast<node_t *>(node)->p_element()))));
                          }; };
                        auto get_right_operand = [&](auto return_accumulated_tuple)
                        { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> void
                          {
                              if(node->child_right() == nullptr)
                                  return_accumulated_tuple(accumulated_tuple_so_far);
                              else
                                  return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*static_cast<node_t *>(static_cast<navigator_except_node_end_t *>(node->child_right()))->p_accumulated_storage()))));
                          }; };
                        auto return_accumulated_tuple = [&](auto accumulated_tuple_so_far)
                        {
                            accumulator.update_accumulated_storage(*static_cast<node_t *>(node)->p_accumulated_storage(), accumulated_tuple_so_far);
                        };
                        get_left_operand(get_middle_operand(get_right_operand(return_accumulated_tuple)))(std::make_tuple());
                    }
                }
                static void refresh_node_count_and_accumulated_storage_impl(accumulator_t const &accumulator, navigator_except_node_end_t *node)
                {
                    refresh_node_count(node);
                    refresh_accumulated_storage(accumulator, node);
                }
                static void refresh_node_count_and_accumulated_storage(schedules_t &schedules, accumulator_t const &accumulator, navigator_except_node_end_t *node)
                {
                    if constexpr(uses_siblings)
                        schedules.push_back(refresh_node_count_and_accumulated_storage_schedule_t{.node = node});
                    else
                        refresh_node_count_and_accumulated_storage_impl(accumulator, node);
                }
                static void refresh_accumulated_storage_and_above(accumulator_t const &accumulator, navigator_except_node_end_t *node)
                {
                    if constexpr(!std::is_same_v<accumulated_storage_t, void>)
                    {
                        refresh_accumulated_storage(accumulator, node);
                        while(node->role() != rb2p_node_role_e::root)
                        {
                            node = node->parent();
                            refresh_accumulated_storage(accumulator, node);
                        }
                    }
                }
                static void refresh_node_count_and_accumulated_storage_and_above_impl(accumulator_t const &accumulator, navigator_except_node_end_t *node)
                {
                    if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_each_node_except_node_end || !std::is_same_v<accumulated_storage_t, void>)
                    {
                        refresh_node_count_and_accumulated_storage_impl(accumulator, node);
                        while(node->role() != rb2p_node_role_e::root)
                        {
                            node = node->parent();
                            refresh_node_count_and_accumulated_storage_impl(accumulator, node);
                        }
                    }
                }
                static void refresh_node_count_and_accumulated_storage_and_above(schedules_t &schedules, accumulator_t const &accumulator, navigator_except_node_end_t *node)
                {
                    if constexpr(uses_siblings)
                        schedules.push_back(refresh_node_count_and_accumulated_storage_and_above_schedule_t{.node = node});
                    else
                        refresh_node_count_and_accumulated_storage_and_above_impl(accumulator, node);
                }
                static void refresh_node_count_and_accumulated_storage_and_above_until_impl(accumulator_t const &accumulator, navigator_except_node_end_t *node, navigator_except_node_end_t *node_end)
                {
                    if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_each_node_except_node_end || !std::is_same_v<accumulated_storage_t, void>)
                    {
                        refresh_node_count_and_accumulated_storage_impl(accumulator, node);
                        while(node->role() != rb2p_node_role_e::root)
                        {
                            node = node->parent();
                            if(node == node_end)
                                break;
                            refresh_node_count_and_accumulated_storage_impl(accumulator, node);
                        }
                    }
                }
                static void refresh_node_count_and_accumulated_storage_and_above_until(schedules_t &schedules, accumulator_t const &accumulator, navigator_except_node_end_t *node, navigator_except_node_end_t *node_end)
                {
                    if constexpr(uses_siblings)
                        schedules.push_back(refresh_node_count_and_accumulated_storage_and_above_until_schedule_t{.node = node, .node_end = node_end});
                    else
                        refresh_node_count_and_accumulated_storage_and_above_until_impl(accumulator, node, node_end);
                }
                template<bool from_left_to_right_or_from_right_to_left>
                static void move_node_count_and_accumulated_storage_impl(accumulator_t const &accumulator, navigator_except_node_end_t *node_left, navigator_except_node_end_t *node_right)
                {
                    if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_each_node_except_node_end)
                    {
                        if constexpr(!from_left_to_right_or_from_right_to_left)
                        {
                            static_cast<node_t *>(node_left)->node_count = 1;
                            static_cast<node_t *>(node_right)->node_count = 2;
                        }
                        else
                        {
                            static_cast<node_t *>(node_right)->node_count = 1;
                            static_cast<node_t *>(node_left)->node_count = 2;
                        }
                    }
                    if constexpr(!std::is_same_v<accumulated_storage_t, void>)
                    {
                        if constexpr(!from_left_to_right_or_from_right_to_left)
                        {
                            refresh_accumulated_storage(accumulator, node_left);
                            refresh_accumulated_storage(accumulator, node_right);
                        }
                        else
                        {
                            refresh_accumulated_storage(accumulator, node_right);
                            refresh_accumulated_storage(accumulator, node_left);
                        }
                    }
                }
                template<bool from_left_to_right_or_from_right_to_left>
                static void move_node_count_and_accumulated_storage(schedules_t &schedules, accumulator_t const &accumulator, navigator_except_node_end_t *node_left, navigator_except_node_end_t *node_right)
                {
                    if constexpr(uses_siblings)
                        schedules.push_back(move_node_count_and_accumulated_storage_schedule_t<from_left_to_right_or_from_right_to_left>{.node_left = node_left, .node_right = node_right});
                    else
                        move_node_count_and_accumulated_storage_impl<from_left_to_right_or_from_right_to_left>(accumulator, node_left, node_right);
                }

                static void run_schedules(schedules_t &schedules, accumulator_t const &accumulator)
                {
                    if constexpr(uses_siblings)
                    {
                        for(auto &schedule : schedules)
                        {
                            std::visit([&]<typename schedule_t>(schedule_t &schedule) -> void
                                {
                                   if constexpr(std::is_same_v<schedule_t,refresh_node_count_and_accumulated_storage_schedule_t>)
                                       refresh_node_count_and_accumulated_storage_impl(accumulator,schedule.node);
                                   else if constexpr(std::is_same_v<schedule_t,refresh_node_count_and_accumulated_storage_and_above_schedule_t>)
                                       refresh_node_count_and_accumulated_storage_and_above_impl(accumulator,schedule.node);
                                   else if constexpr(std::is_same_v<schedule_t,refresh_node_count_and_accumulated_storage_and_above_until_schedule_t>)
                                       refresh_node_count_and_accumulated_storage_and_above_until_impl(accumulator,schedule.node,schedule.node_end);
                                   else if constexpr(std::is_same_v<schedule_t,move_node_count_and_accumulated_storage_schedule_t<false>>)
                                       move_node_count_and_accumulated_storage_impl<false>(accumulator,schedule.node_left,schedule.node_right);
                                   else if constexpr(std::is_same_v<schedule_t,move_node_count_and_accumulated_storage_schedule_t<true>>)
                                       move_node_count_and_accumulated_storage_impl<true>(accumulator,schedule.node_left,schedule.node_right); },
                                schedule);
                        }
                    }
                }

                struct bnode_t
                {
                    std::tuple<std::optional<navigator_except_node_end_t *>, navigator_except_node_end_t *, std::optional<navigator_except_node_end_t *>> keys;
                    int key_count;
                    typename navigator_t::template parent_info_t<is_reversed> center_key_parent_info;
                    std::optional<navigator_except_node_end_t *> opt_child_at(int index /* -3 / -1 / 1 / 3 */)
                    {
                        if(index == -3)
                        {
                            if(!std::get<0>(keys).has_value()) return std::nullopt;
                            else
                            {
                                if(/*std::get<0>(keys).value()->*p_child_left == nullptr*/ std::get<0>(keys).value()->*p_child_left_all == nullptr)
                                {
                                    assert(!tagged_ptr_bit0_is_setted(std::get<0>(keys).value()->*p_child_left_all));
                                    return std::nullopt;
                                }
                                else return std::get<0>(keys).value()->*p_child_left;
                            }
                        }
                        else if(index == -1)
                        {
                            if(!std::get<0>(keys).has_value())
                                return std::get<1>(keys)->*p_child_left;
                            else
                                return std::get<0>(keys).value()->*p_child_right;
                        }
                        else if(index == 1)
                        {
                            if(!std::get<2>(keys).has_value())
                                return std::get<1>(keys)->*p_child_right;
                            else
                                return std::get<2>(keys).value()->*p_child_left;
                        }
                        else if(index == 3)
                        {
                            if(!std::get<2>(keys).has_value()) return std::nullopt;
                            else
                            {
                                if(/*std::get<2>(keys).value()->*p_child_right == nullptr*/ std::get<2>(keys).value()->*p_child_right_all == nullptr)
                                {
                                    assert(!tagged_ptr_bit0_is_setted(std::get<2>(keys).value()->*p_child_right_all));
                                    return std::nullopt;
                                }
                                else return std::get<2>(keys).value()->*p_child_right;
                            }
                        }
                        else return std::nullopt;
                    }
                    navigator_except_node_end_t *key_at(int index /* -2 / 0 / 2 */)
                    {
                        if(index == -2)
                            return std::get<0>(keys).value();
                        else if(index == 0)
                            return std::get<1>(keys);
                        else if(index == 2)
                            return std::get<2>(keys).value();
                        else std::unreachable();
                    }
                };
                struct bnode_up_t: bnode_t
                {
                    int child_index; // inserting, -3 / -1 / 1 / 3
                    static bnode_up_t get_bnode_from_key(navigator_except_node_end_t *key, bool child_is_left_or_right_child_of_key)
                    {
                        bnode_up_t bnode;
                        if(!key->color()) // black
                        {
                            if(key->*p_child_left == nullptr || !(key->*p_child_left)->color()) std::get<0>(bnode.keys).reset();
                            else std::get<0>(bnode.keys) = key->*p_child_left;
                            std::get<1>(bnode.keys) = key;
                            if(key->*p_child_right == nullptr || !(key->*p_child_right)->color()) std::get<2>(bnode.keys).reset();
                            else std::get<2>(bnode.keys) = key->*p_child_right;

                            bnode.child_index = !child_is_left_or_right_child_of_key ? -1 : 1;
                        }
                        else // red
                        {
                            typename navigator_t::template parent_info_t<is_reversed> key_red_parent_info(key);
                            navigator_except_node_end_t *key_black = std::get<1>(bnode.keys) = static_cast<navigator_except_node_end_t *>(key_red_parent_info.parent);
                            if(!key_red_parent_info.is_left_or_right_child_of_parent)
                            {
                                std::get<0>(bnode.keys).emplace(key);
                                if(key_black->*p_child_right == nullptr || !(key_black->*p_child_right)->color()) std::get<2>(bnode.keys).reset();
                                else std::get<2>(bnode.keys) = key_black->*p_child_right;

                                bnode.child_index = !child_is_left_or_right_child_of_key ? -3 : -1;
                            }
                            else
                            {
                                if(key_black->*p_child_left == nullptr || !(key_black->*p_child_left)->color()) std::get<0>(bnode.keys).reset();
                                else std::get<0>(bnode.keys) = key_black->*p_child_left;
                                std::get<2>(bnode.keys).emplace(key);

                                bnode.child_index = !child_is_left_or_right_child_of_key ? 1 : 3;
                            }
                        }
                        bnode.center_key_parent_info = typename navigator_t::template parent_info_t<is_reversed>(std::get<1>(bnode.keys));
                        bnode.key_count = std::get<0>(bnode.keys).has_value() + 1 + std::get<2>(bnode.keys).has_value();
                        return bnode;
                    };
                    static std::tuple<bnode_up_t, navigator_except_node_end_t *> inserting_get_bnode_from_key_black_sibling(navigator_except_node_end_t *key_black_sibling, bool key_black_sibling_is_left_or_right_sibling)
                    {
                        assert(!key_black_sibling->color());

                        bnode_up_t bnode;

                        if(key_black_sibling->*p_child_left == nullptr || !(key_black_sibling->*p_child_left)->color()) std::get<0>(bnode.keys).reset();
                        else std::get<0>(bnode.keys) = key_black_sibling->*p_child_left;
                        std::get<1>(bnode.keys) = key_black_sibling;
                        if(key_black_sibling->*p_child_right == nullptr || !(key_black_sibling->*p_child_right)->color()) std::get<2>(bnode.keys).reset();
                        else std::get<2>(bnode.keys) = key_black_sibling->*p_child_right;

                        bnode.child_index = !key_black_sibling_is_left_or_right_sibling //
                            ? (std::get<2>(bnode.keys).has_value() ? 3 : 1) //
                            : (std::get<0>(bnode.keys).has_value() ? -3 : -1);
                        bnode.center_key_parent_info = typename navigator_t::template parent_info_t<is_reversed>(std::get<1>(bnode.keys));
                        bnode.key_count = std::get<0>(bnode.keys).has_value() + 1 + std::get<2>(bnode.keys).has_value();

                        navigator_except_node_end_t *bnode_rightmost_or_leftmost_child = !key_black_sibling_is_left_or_right_sibling //
                            ? (std::get<2>(bnode.keys).has_value() ? std::get<2>(bnode.keys).value()->*p_child_right : std::get<1>(bnode.keys)->*p_child_right) //
                            : (std::get<0>(bnode.keys).has_value() ? std::get<0>(bnode.keys).value()->*p_child_left : std::get<1>(bnode.keys)->*p_child_left);
                        return std::make_tuple(bnode, bnode_rightmost_or_leftmost_child);
                    };
                };
                struct bnode_erase_t: bnode_t
                {
                    int key_to_be_erased_index; // erasing, -2 / 0 / 2
                    static bnode_erase_t erasing_get_bnode(navigator_except_node_end_t *node_key_to_be_erased)
                    {
                        bnode_erase_t bnode;
                        if(!node_key_to_be_erased->color()) // black
                        {
                            if(node_key_to_be_erased->*p_child_left == nullptr || !(node_key_to_be_erased->*p_child_left)->color()) std::get<0>(bnode.keys).reset();
                            else std::get<0>(bnode.keys) = node_key_to_be_erased->*p_child_left;
                            std::get<1>(bnode.keys) = node_key_to_be_erased;
                            if(node_key_to_be_erased->*p_child_right == nullptr || !(node_key_to_be_erased->*p_child_right)->color()) std::get<2>(bnode.keys).reset();
                            else std::get<2>(bnode.keys) = node_key_to_be_erased->*p_child_right;

                            bnode.key_to_be_erased_index = 0;
                        }
                        else // red
                        {
                            typename navigator_t::template parent_info_t<is_reversed> key_red_parent_info(node_key_to_be_erased);
                            navigator_except_node_end_t *key_black = std::get<1>(bnode.keys) = static_cast<navigator_except_node_end_t *>(key_red_parent_info.parent);
                            if(!key_red_parent_info.is_left_or_right_child_of_parent)
                            {
                                std::get<0>(bnode.keys).emplace(node_key_to_be_erased);
                                if(key_black->*p_child_right == nullptr || !(key_black->*p_child_right)->color()) std::get<2>(bnode.keys).reset();
                                else std::get<2>(bnode.keys) = key_black->*p_child_right;

                                bnode.key_to_be_erased_index = -2;
                            }
                            else
                            {
                                if(key_black->*p_child_left == nullptr || !(key_black->*p_child_left)->color()) std::get<0>(bnode.keys).reset();
                                else std::get<0>(bnode.keys) = key_black->*p_child_left;
                                std::get<2>(bnode.keys).emplace(node_key_to_be_erased);

                                bnode.key_to_be_erased_index = 2;
                            }
                        }
                        bnode.center_key_parent_info = typename navigator_t::template parent_info_t<is_reversed>(std::get<1>(bnode.keys));
                        bnode.key_count = std::get<0>(bnode.keys).has_value() + 1 + std::get<2>(bnode.keys).has_value();
                        return bnode;
                    }
                    static std::tuple<bnode_erase_t, navigator_except_node_end_t *, navigator_except_node_end_t *> erasing_get_bnode_from_key_black_sibling(navigator_except_node_end_t *key_black_sibling, bool key_black_sibling_is_left_or_right_sibling)
                    {
                        assert(!key_black_sibling->color());

                        bnode_erase_t bnode;

                        if(key_black_sibling->*p_child_left == nullptr || !(key_black_sibling->*p_child_left)->color()) std::get<0>(bnode.keys).reset();
                        else std::get<0>(bnode.keys) = key_black_sibling->*p_child_left;
                        std::get<1>(bnode.keys) = key_black_sibling;
                        if(key_black_sibling->*p_child_right == nullptr || !(key_black_sibling->*p_child_right)->color()) std::get<2>(bnode.keys).reset();
                        else std::get<2>(bnode.keys) = key_black_sibling->*p_child_right;

                        bnode.key_to_be_erased_index = !key_black_sibling_is_left_or_right_sibling //
                            ? (std::get<2>(bnode.keys).has_value() ? 2 : 0) //
                            : (std::get<0>(bnode.keys).has_value() ? -2 : 0);
                        bnode.center_key_parent_info = typename navigator_t::template parent_info_t<is_reversed>(std::get<1>(bnode.keys));
                        bnode.key_count = std::get<0>(bnode.keys).has_value() + 1 + std::get<2>(bnode.keys).has_value();

                        navigator_except_node_end_t *bnode_rightmost_or_leftmost_child = !key_black_sibling_is_left_or_right_sibling //
                            ? (std::get<2>(bnode.keys).has_value() ? std::get<2>(bnode.keys).value()->*p_child_right : std::get<1>(bnode.keys)->*p_child_right) //
                            : (std::get<0>(bnode.keys).has_value() ? std::get<0>(bnode.keys).value()->*p_child_left : std::get<1>(bnode.keys)->*p_child_left);
                        navigator_except_node_end_t *bnode_second_rightmost_or_second_leftmost_child = !key_black_sibling_is_left_or_right_sibling //
                            ? (std::get<2>(bnode.keys).has_value() ? std::get<2>(bnode.keys).value()->*p_child_left : (!std::get<0>(bnode.keys).has_value() ? std::get<1>(bnode.keys)->*p_child_left : std::get<0>(bnode.keys).value()->*p_child_right)) //
                            : (std::get<0>(bnode.keys).has_value() ? std::get<0>(bnode.keys).value()->*p_child_right : (!std::get<2>(bnode.keys).has_value() ? std::get<1>(bnode.keys)->*p_child_right : std::get<2>(bnode.keys).value()->*p_child_left));
                        return std::make_tuple(bnode, bnode_rightmost_or_leftmost_child, bnode_second_rightmost_or_second_leftmost_child);
                    }
                };

                static bool erase(schedules_t &schedules, node_end_t *node_end, navigator_t * const node_)
                {
                    accumulator_t const &accumulator = node_end->accumulator;
                    assert(node_ != nullptr);
                    assert(node_ != node_end); // node_end
                    bool is_empty = empty(node_end);
                    assert(!is_empty);
                    navigator_except_node_end_t *node = static_cast<navigator_except_node_end_t *>(node_);
                    bool will_be_empty = one_provided_not_empty(node_end);
                    if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::no_size)
                        ;
                    else if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_node_end)
                        --node_end->node_count;
                    else if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_each_node_except_node_end)
                        ;
                    else
                        std::unreachable();
                    bool height_changed;
                    if(will_be_empty) // --count==0
                    {
                        typename navigator_t::loop_end_t{.end = node_end}.link(); //node_end->my_list_begin() = tagged_ptr_bit0_setted(node_end);
                        height_changed = true;
                    }
                    else // --count!=0
                    {
                        bool should_link_loop_end = false;
                        typename navigator_t::loop_end_t loop_end = navigator_t::get_loop_end(node_end);

                        if((!node->color() && ((node->*p_child_left_all == nullptr || tagged_ptr_bit0_is_setted(node->*p_child_left_all)) || ((node->*p_child_left_all)->color() && (node->*p_child_left_all->*p_child_left_all == nullptr || tagged_ptr_bit0_is_setted(node->*p_child_left_all->*p_child_left_all))))) ||
                            (node->color() && (node->*p_child_left_all == nullptr || tagged_ptr_bit0_is_setted(node->*p_child_left_all)))) // leaf bnode
                        {
                        }
                        else // not leaf bnode
                        {
                            navigator_except_node_end_t *node_current = static_cast<navigator_except_node_end_t *>(rb2p_iterator_t<false, is_reversed, config_t>::template predecessor<false>(node));
                            typename navigator_t::loop_t parent_outer_loop;
                            if(node->role() != rb2p_node_role_e::root)
                                parent_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(node);
                            typename navigator_t::loop_t parent_inner_loop = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(node);
                            if(node_current == parent_inner_loop.child_left)
                            {
                                assert(!node_current->color() && ((node_current->*p_child_left_all == nullptr || tagged_ptr_bit0_is_setted(node_current->*p_child_left_all)) || ((node_current->*p_child_left_all)->color() && (node_current->*p_child_left_all->*p_child_left_all == nullptr || tagged_ptr_bit0_is_setted(node_current->*p_child_left_all->*p_child_left_all)))));
                                assert(node_current->*p_child_right_all == nullptr);
                                if(node_current->*p_child_left_all == nullptr || tagged_ptr_bit0_is_setted(node_current->*p_child_left_all)) //node_current is a single black node
                                {
                                    if(node->role() == rb2p_node_role_e::root && node_current->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root)
                                    {
                                        assert(loop_end.root == node);
                                        assert(loop_end.leftmost_descendent_of_root == node_current);
                                        loop_end.root = node_current;
                                        loop_end.leftmost_descendent_of_root = node;
                                        //                                        should_link_loop_end = true;
                                        loop_end.index = -1, loop_end.link();
                                    }
                                    else if(node->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent && node_current->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root)
                                    {
                                        assert(loop_end.leftmost_descendent_of_root == node_current);
                                        loop_end.leftmost_descendent_of_root = node;
                                        //                                        should_link_loop_end = true;
                                        loop_end.index = -1, loop_end.link();

                                        parent_outer_loop.exchange(node_current);
                                    }
                                    else if(node->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent || (node->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent && node_current->role() != rb2p_node_role_e::child_left_leftmost_descendent_of_root))
                                    {
                                        assert(node_current->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_non_root);
                                        typename navigator_t::loop_t child_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::ancestor>(node_current);
                                        if(child_outer_loop.this_ == parent_outer_loop.this_)
                                        {
                                            assert(parent_outer_loop.child_right == node);
                                            assert(parent_outer_loop.leftmost_descendent_of_child_right == node_current);
                                            parent_outer_loop.child_right = node_current;
                                            parent_outer_loop.leftmost_descendent_of_child_right = node;
                                            parent_outer_loop.index = -1, parent_outer_loop.link();
                                        }
                                        else
                                        {
                                            child_outer_loop.exchange(node);
                                            parent_outer_loop.exchange(node_current);
                                        }
                                    }
                                    else std::unreachable();
                                }
                                else //node_current is a black node with red left child
                                {
                                    assert(node_current->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent);
                                    typename navigator_t::loop_t child_inner_loop = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(node_current);
                                    assert(child_inner_loop.child_right == nullptr);
                                    if(node->role() == rb2p_node_role_e::root)
                                    {
                                        assert(loop_end.root == node);
                                        loop_end.root = node_current;
                                        //                                        should_link_loop_end = true;
                                        loop_end.index = -1, loop_end.link();
                                    }
                                    else
                                        parent_outer_loop.exchange(node_current);
                                    child_inner_loop.exchange(node);
                                }
                                parent_inner_loop.this_ = node_current;
                                parent_inner_loop.child_left = node;
                                parent_inner_loop.index = -1, parent_inner_loop.link();
                            }
                            else if(node_current == parent_inner_loop.rightmost_descendent_of_child_left)
                            {
                                typename navigator_t::loop_t child_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::ancestor>(node_current);
                                assert(child_outer_loop.this_ == parent_inner_loop.this_);

                                typename navigator_t::loop_t child_parent_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(node_current);
                                if(node->role() == rb2p_node_role_e::root)
                                {
                                    assert(loop_end.root == node);
                                    loop_end.root = node_current;
                                    //                                    should_link_loop_end = true;
                                    loop_end.index = -1, loop_end.link();
                                }
                                else
                                    parent_outer_loop.exchange(node_current);
                                parent_inner_loop.this_ = node_current;
                                parent_inner_loop.rightmost_descendent_of_child_left = node;
                                parent_inner_loop.index = -1, parent_inner_loop.link();
                                child_parent_loop.exchange(node);
                            }
                            else std::unreachable();
                            std::ranges::swap(node->color(), node_current->color());
                            std::ranges::swap(node->role(), node_current->role());
                        }
                        auto erase_impl = [&schedules, &accumulator, &node, &loop_end, &should_link_loop_end, &height_changed](auto &this_, bnode_erase_t bnode_to_have_key_erased, function_view<navigator_except_node_end_t *(rb2p_node_role_e)> merge_and_return_child_after_merge, std::size_t child_after_merge_height) -> void
                        {
                            if(bnode_to_have_key_erased.key_count != 1)
                            {
                                assert(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::root || std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent || std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent);
                                if(bnode_to_have_key_erased.key_to_be_erased_index == -2)
                                {
                                    if(child_after_merge_height == 0)
                                    {
                                        if(!std::get<2>(bnode_to_have_key_erased.keys).has_value())
                                        {
                                            if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::root)
                                            {
                                                assert(std::get<0>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root);

                                                assert(!should_link_loop_end);
                                                assert(loop_end.leftmost_descendent_of_root == std::get<0>(bnode_to_have_key_erased.keys).value());
                                                assert(loop_end.root == std::get<1>(bnode_to_have_key_erased.keys));
                                                assert(loop_end.rightmost_descendent_of_root == nullptr);
                                                loop_end.leftmost_descendent_of_root = nullptr;
                                                should_link_loop_end = true;

                                                typename navigator_t::loop_t{
                                                    .this_ = std::get<1>(bnode_to_have_key_erased.keys),
                                                }
                                                    .link();
                                            }
                                            else if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent)
                                            {
                                                assert(std::get<0>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root || std::get<0>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_non_root);
                                                if(std::get<0>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root)
                                                {
                                                    assert(!should_link_loop_end);
                                                    assert(loop_end.leftmost_descendent_of_root == std::get<0>(bnode_to_have_key_erased.keys).value());
                                                    loop_end.leftmost_descendent_of_root = std::get<1>(bnode_to_have_key_erased.keys);
                                                    should_link_loop_end = true;
                                                }
                                                else if(std::get<0>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_non_root)
                                                {
                                                    typename navigator_t::loop_t child_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::ancestor>(std::get<0>(bnode_to_have_key_erased.keys).value());
                                                    child_outer_loop.exchange(std::get<1>(bnode_to_have_key_erased.keys));
                                                }
                                                else std::unreachable();
                                                std::get<1>(bnode_to_have_key_erased.keys)->role() = std::get<0>(bnode_to_have_key_erased.keys).value()->role();
                                            }
                                            else std::unreachable();
                                        }
                                        else
                                        {
                                            if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::root)
                                            {
                                                assert(std::get<0>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root);
                                                assert(std::get<2>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root);

                                                assert(!should_link_loop_end);
                                                assert(loop_end.leftmost_descendent_of_root == std::get<0>(bnode_to_have_key_erased.keys).value());
                                                assert(loop_end.root == std::get<1>(bnode_to_have_key_erased.keys));
                                                assert(loop_end.rightmost_descendent_of_root == std::get<2>(bnode_to_have_key_erased.keys).value());
                                                loop_end.leftmost_descendent_of_root = nullptr;
                                                should_link_loop_end = true;

                                                typename navigator_t::loop_t{
                                                    .this_ = std::get<1>(bnode_to_have_key_erased.keys),
                                                    .child_right = std::get<2>(bnode_to_have_key_erased.keys).value(),
                                                }
                                                    .link();
                                            }
                                            else if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent)
                                            {
                                                assert(std::get<0>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root || std::get<0>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_non_root);
                                                assert(std::get<2>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_non_root);

                                                typename navigator_t::loop_t parent_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_to_have_key_erased.keys));
                                                typename navigator_t::loop_t child_left_outer_loop;
                                                if(std::get<0>(bnode_to_have_key_erased.keys).value()->role() != rb2p_node_role_e::child_left_leftmost_descendent_of_root)
                                                    child_left_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::ancestor>(std::get<0>(bnode_to_have_key_erased.keys).value());

                                                if(std::get<0>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root)
                                                {
                                                    assert(!should_link_loop_end);
                                                    assert(loop_end.leftmost_descendent_of_root == std::get<0>(bnode_to_have_key_erased.keys).value());
                                                    loop_end.leftmost_descendent_of_root = std::get<1>(bnode_to_have_key_erased.keys);
                                                    should_link_loop_end = true;
                                                }
                                                else if(std::get<0>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_non_root)
                                                    child_left_outer_loop.exchange(std::get<1>(bnode_to_have_key_erased.keys));
                                                else std::unreachable();

                                                assert(parent_outer_loop.child_left == std::get<1>(bnode_to_have_key_erased.keys));
                                                assert(parent_outer_loop.rightmost_descendent_of_child_left == std::get<2>(bnode_to_have_key_erased.keys).value());
                                                parent_outer_loop.child_left = std::get<2>(bnode_to_have_key_erased.keys).value();
                                                parent_outer_loop.rightmost_descendent_of_child_left = nullptr;
                                                parent_outer_loop.index = -1, parent_outer_loop.link();

                                                typename navigator_t::loop_t{
                                                    .this_ = std::get<2>(bnode_to_have_key_erased.keys).value(),
                                                    .child_left = std::get<1>(bnode_to_have_key_erased.keys),
                                                }
                                                    .link();

                                                std::get<2>(bnode_to_have_key_erased.keys).value()->role() = std::get<1>(bnode_to_have_key_erased.keys)->role();
                                                std::get<2>(bnode_to_have_key_erased.keys).value()->color() = std::get<1>(bnode_to_have_key_erased.keys)->color();
                                                std::get<1>(bnode_to_have_key_erased.keys)->role() = std::get<0>(bnode_to_have_key_erased.keys).value()->role();
                                                std::get<1>(bnode_to_have_key_erased.keys)->color() = std::get<0>(bnode_to_have_key_erased.keys).value()->color();
                                            }
                                            else if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent)
                                            {
                                                assert(std::get<0>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_non_root);
                                                assert(std::get<2>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root || std::get<2>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_non_root);

                                                typename navigator_t::loop_t parent_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_to_have_key_erased.keys));
                                                typename navigator_t::loop_t child_left_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::ancestor>(std::get<0>(bnode_to_have_key_erased.keys).value());
                                                assert(parent_outer_loop.this_ == child_left_outer_loop.this_);

                                                assert(parent_outer_loop.child_right == std::get<1>(bnode_to_have_key_erased.keys));
                                                assert(parent_outer_loop.leftmost_descendent_of_child_right == std::get<0>(bnode_to_have_key_erased.keys).value());
                                                parent_outer_loop.leftmost_descendent_of_child_right = nullptr;
                                                parent_outer_loop.index = -1, parent_outer_loop.link();

                                                typename navigator_t::loop_t{
                                                    .this_ = std::get<1>(bnode_to_have_key_erased.keys),
                                                    .child_right = std::get<2>(bnode_to_have_key_erased.keys).value(),
                                                }
                                                    .link();
                                            }
                                            else std::unreachable();
                                        }
                                    }
                                    else
                                    {
                                        typename navigator_t::loop_t parent_inner_loop = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<1>(bnode_to_have_key_erased.keys));
                                        if(parent_inner_loop.rightmost_descendent_of_child_left == node)
                                            parent_inner_loop.rightmost_descendent_of_child_left = static_cast<navigator_except_node_end_t *>(rb2p_iterator_t<false, is_reversed, config_t>::template predecessor<false>(parent_inner_loop.rightmost_descendent_of_child_left));

                                        navigator_except_node_end_t *child_after_merge = merge_and_return_child_after_merge(rb2p_node_role_e::child_left_not_a_leftmost_descendent);

                                        parent_inner_loop.child_left = child_after_merge;
                                        parent_inner_loop.index = -1, parent_inner_loop.link();
                                    }

                                    refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<1>(bnode_to_have_key_erased.keys));
                                }
                                else if(bnode_to_have_key_erased.key_to_be_erased_index == 0)
                                {
                                    if(child_after_merge_height == 0)
                                    {
                                        if(std::get<0>(bnode_to_have_key_erased.keys).has_value() && !std::get<2>(bnode_to_have_key_erased.keys).has_value())
                                        {
                                            if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::root)
                                            {
                                                assert(std::get<0>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root);

                                                assert(!should_link_loop_end);
                                                assert(loop_end.leftmost_descendent_of_root == std::get<0>(bnode_to_have_key_erased.keys).value());
                                                assert(loop_end.root == std::get<1>(bnode_to_have_key_erased.keys));
                                                assert(loop_end.rightmost_descendent_of_root == nullptr);
                                                loop_end.leftmost_descendent_of_root = nullptr;
                                                loop_end.root = std::get<0>(bnode_to_have_key_erased.keys).value();
                                                should_link_loop_end = true;

                                                typename navigator_t::loop_t{
                                                    .this_ = std::get<0>(bnode_to_have_key_erased.keys).value(),
                                                }
                                                    .link();
                                                std::get<0>(bnode_to_have_key_erased.keys).value()->role() = std::get<1>(bnode_to_have_key_erased.keys)->role();
                                            }
                                            else if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent)
                                            {
                                                assert(std::get<0>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root || std::get<0>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_non_root);

                                                typename navigator_t::loop_t parent_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_to_have_key_erased.keys));
                                                parent_outer_loop.exchange(std::get<0>(bnode_to_have_key_erased.keys).value());
                                            }
                                            else std::unreachable();
                                            std::get<0>(bnode_to_have_key_erased.keys).value()->color() = std::get<1>(bnode_to_have_key_erased.keys)->color();

                                            refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<0>(bnode_to_have_key_erased.keys).value());
                                        }
                                        else if(!std::get<0>(bnode_to_have_key_erased.keys).has_value() && std::get<2>(bnode_to_have_key_erased.keys).has_value())
                                        {
                                            if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::root)
                                            {
                                                assert(std::get<2>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root);

                                                assert(!should_link_loop_end);
                                                assert(loop_end.rightmost_descendent_of_root == std::get<2>(bnode_to_have_key_erased.keys).value());
                                                assert(loop_end.root == std::get<1>(bnode_to_have_key_erased.keys));
                                                assert(loop_end.leftmost_descendent_of_root == nullptr);
                                                loop_end.rightmost_descendent_of_root = nullptr;
                                                loop_end.root = std::get<2>(bnode_to_have_key_erased.keys).value();
                                                should_link_loop_end = true;

                                                typename navigator_t::loop_t{
                                                    .this_ = std::get<2>(bnode_to_have_key_erased.keys).value(),
                                                }
                                                    .link();
                                                std::get<2>(bnode_to_have_key_erased.keys).value()->role() = std::get<1>(bnode_to_have_key_erased.keys)->role();
                                            }
                                            else if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent)
                                            {
                                                assert(std::get<2>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root || std::get<2>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_non_root);

                                                typename navigator_t::loop_t parent_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_to_have_key_erased.keys));
                                                parent_outer_loop.exchange(std::get<2>(bnode_to_have_key_erased.keys).value());
                                            }
                                            else std::unreachable();
                                            std::get<2>(bnode_to_have_key_erased.keys).value()->color() = std::get<1>(bnode_to_have_key_erased.keys)->color();

                                            refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<2>(bnode_to_have_key_erased.keys).value());
                                        }
                                        else if(std::get<0>(bnode_to_have_key_erased.keys).has_value() && std::get<2>(bnode_to_have_key_erased.keys).has_value())
                                        {
                                            if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::root)
                                            {
                                                assert(std::get<0>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root);
                                                assert(std::get<2>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root);

                                                assert(!should_link_loop_end);
                                                assert(loop_end.leftmost_descendent_of_root == std::get<0>(bnode_to_have_key_erased.keys).value());
                                                assert(loop_end.root == std::get<1>(bnode_to_have_key_erased.keys));
                                                assert(loop_end.rightmost_descendent_of_root == std::get<2>(bnode_to_have_key_erased.keys).value());
                                                loop_end.leftmost_descendent_of_root = nullptr;
                                                loop_end.root = std::get<0>(bnode_to_have_key_erased.keys).value();
                                                should_link_loop_end = true;

                                                typename navigator_t::loop_t{
                                                    .this_ = std::get<0>(bnode_to_have_key_erased.keys).value(),
                                                    .child_right = std::get<2>(bnode_to_have_key_erased.keys).value(),
                                                }
                                                    .link();
                                                std::get<0>(bnode_to_have_key_erased.keys).value()->role() = std::get<1>(bnode_to_have_key_erased.keys)->role();
                                                std::get<0>(bnode_to_have_key_erased.keys).value()->color() = std::get<1>(bnode_to_have_key_erased.keys)->color();

                                                refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<2>(bnode_to_have_key_erased.keys).value());
                                            }
                                            else if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent)
                                            {
                                                assert(std::get<0>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root || std::get<0>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_non_root);
                                                assert(std::get<2>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_non_root);

                                                typename navigator_t::loop_t parent_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_to_have_key_erased.keys));
                                                typename navigator_t::loop_t child_right_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::ancestor>(std::get<2>(bnode_to_have_key_erased.keys).value());
                                                assert(parent_outer_loop.this_ == child_right_outer_loop.this_);

                                                parent_outer_loop.rightmost_descendent_of_child_left = nullptr;
                                                parent_outer_loop.child_left = std::get<2>(bnode_to_have_key_erased.keys).value();
                                                parent_outer_loop.index = -1, parent_outer_loop.link();

                                                typename navigator_t::loop_t{
                                                    .this_ = std::get<2>(bnode_to_have_key_erased.keys).value(),
                                                    .child_left = std::get<0>(bnode_to_have_key_erased.keys).value(),
                                                }
                                                    .link();
                                                std::get<2>(bnode_to_have_key_erased.keys).value()->role() = std::get<1>(bnode_to_have_key_erased.keys)->role();
                                                std::get<2>(bnode_to_have_key_erased.keys).value()->color() = std::get<1>(bnode_to_have_key_erased.keys)->color();

                                                refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<0>(bnode_to_have_key_erased.keys).value());
                                            }
                                            else if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent)
                                            {
                                                assert(std::get<0>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_non_root);
                                                assert(std::get<2>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root || std::get<2>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_non_root);

                                                typename navigator_t::loop_t parent_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_to_have_key_erased.keys));
                                                typename navigator_t::loop_t child_left_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::ancestor>(std::get<0>(bnode_to_have_key_erased.keys).value());
                                                assert(parent_outer_loop.this_ == child_left_outer_loop.this_);

                                                parent_outer_loop.leftmost_descendent_of_child_right = nullptr;
                                                parent_outer_loop.child_right = std::get<0>(bnode_to_have_key_erased.keys).value();
                                                parent_outer_loop.index = -1, parent_outer_loop.link();

                                                typename navigator_t::loop_t{
                                                    .this_ = std::get<0>(bnode_to_have_key_erased.keys).value(),
                                                    .child_right = std::get<2>(bnode_to_have_key_erased.keys).value(),
                                                }
                                                    .link();
                                                std::get<0>(bnode_to_have_key_erased.keys).value()->role() = std::get<1>(bnode_to_have_key_erased.keys)->role();
                                                std::get<0>(bnode_to_have_key_erased.keys).value()->color() = std::get<1>(bnode_to_have_key_erased.keys)->color();

                                                refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<2>(bnode_to_have_key_erased.keys).value());
                                            }
                                            else std::unreachable();
                                        }
                                        else std::unreachable();
                                    }
                                    else
                                    {
                                        if(std::get<0>(bnode_to_have_key_erased.keys).has_value() && !std::get<2>(bnode_to_have_key_erased.keys).has_value())
                                        {
                                            typename navigator_t::loop_t parent_outer_loop;
                                            if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent || std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent)
                                            {
                                                parent_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_to_have_key_erased.keys));
                                                if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent)
                                                {
                                                    assert(parent_outer_loop.child_left == std::get<1>(bnode_to_have_key_erased.keys));
                                                    parent_outer_loop.child_left = std::get<0>(bnode_to_have_key_erased.keys).value();
                                                    if(parent_outer_loop.rightmost_descendent_of_child_left == node)
                                                        parent_outer_loop.rightmost_descendent_of_child_left = static_cast<navigator_except_node_end_t *>(rb2p_iterator_t<false, is_reversed, config_t>::template predecessor<false>(parent_outer_loop.rightmost_descendent_of_child_left));
                                                }
                                                else if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent)
                                                {
                                                    assert(parent_outer_loop.child_right == std::get<1>(bnode_to_have_key_erased.keys));
                                                    parent_outer_loop.child_right = std::get<0>(bnode_to_have_key_erased.keys).value();
                                                }
                                                else std::unreachable();
                                            }
                                            typename navigator_t::loop_t child_left_inner_loop = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<0>(bnode_to_have_key_erased.keys).value());
                                            if(child_left_inner_loop.leftmost_descendent_of_child_right == node)
                                                child_left_inner_loop.leftmost_descendent_of_child_right = static_cast<navigator_except_node_end_t *>(rb2p_iterator_t<false, is_reversed, config_t>::template predecessor<true>(child_left_inner_loop.leftmost_descendent_of_child_right));
                                            rb2p_node_role_e center_key_role = std::get<1>(bnode_to_have_key_erased.keys)->role();

                                            navigator_except_node_end_t *child_after_merge = merge_and_return_child_after_merge(rb2p_node_role_e::child_right_not_a_rightmost_descendent);

                                            if(parent_outer_loop.this_ != nullptr)
                                                parent_outer_loop.index = -1, parent_outer_loop.link();
                                            else
                                            {
                                                assert(loop_end.root == std::get<1>(bnode_to_have_key_erased.keys));
                                                loop_end.root = std::get<0>(bnode_to_have_key_erased.keys).value();
                                                should_link_loop_end = true;
                                            }

                                            child_left_inner_loop.child_right = child_after_merge;
                                            child_left_inner_loop.index = -1, child_left_inner_loop.link();

                                            std::get<0>(bnode_to_have_key_erased.keys).value()->role() = center_key_role;
                                            std::get<0>(bnode_to_have_key_erased.keys).value()->color() = false;

                                            refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<0>(bnode_to_have_key_erased.keys).value());
                                        }
                                        else if(!std::get<0>(bnode_to_have_key_erased.keys).has_value() && std::get<2>(bnode_to_have_key_erased.keys).has_value())
                                        {
                                            typename navigator_t::loop_t parent_outer_loop;
                                            if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent || std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent)
                                            {
                                                parent_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_to_have_key_erased.keys));
                                                if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent)
                                                {
                                                    assert(parent_outer_loop.child_right == std::get<1>(bnode_to_have_key_erased.keys));
                                                    parent_outer_loop.child_right = std::get<2>(bnode_to_have_key_erased.keys).value();
                                                    if(parent_outer_loop.leftmost_descendent_of_child_right == node)
                                                        parent_outer_loop.leftmost_descendent_of_child_right = static_cast<navigator_except_node_end_t *>(rb2p_iterator_t<false, is_reversed, config_t>::template predecessor<true>(parent_outer_loop.leftmost_descendent_of_child_right));
                                                }
                                                else if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent)
                                                {
                                                    assert(parent_outer_loop.child_left == std::get<1>(bnode_to_have_key_erased.keys));
                                                    parent_outer_loop.child_left = std::get<2>(bnode_to_have_key_erased.keys).value();
                                                }
                                                else std::unreachable();
                                            }
                                            typename navigator_t::loop_t child_right_inner_loop = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<2>(bnode_to_have_key_erased.keys).value());
                                            if(child_right_inner_loop.rightmost_descendent_of_child_left == node)
                                                child_right_inner_loop.rightmost_descendent_of_child_left = static_cast<navigator_except_node_end_t *>(rb2p_iterator_t<false, is_reversed, config_t>::template predecessor<false>(child_right_inner_loop.rightmost_descendent_of_child_left));
                                            rb2p_node_role_e center_key_role = std::get<1>(bnode_to_have_key_erased.keys)->role();

                                            navigator_except_node_end_t *child_after_merge = merge_and_return_child_after_merge(rb2p_node_role_e::child_left_not_a_leftmost_descendent);

                                            if(parent_outer_loop.this_ != nullptr)
                                                parent_outer_loop.index = -1, parent_outer_loop.link();
                                            else
                                            {
                                                assert(loop_end.root == std::get<1>(bnode_to_have_key_erased.keys));
                                                loop_end.root = std::get<2>(bnode_to_have_key_erased.keys).value();
                                                should_link_loop_end = true;
                                            }

                                            child_right_inner_loop.child_left = child_after_merge;
                                            child_right_inner_loop.index = -1, child_right_inner_loop.link();

                                            std::get<2>(bnode_to_have_key_erased.keys).value()->role() = center_key_role;
                                            std::get<2>(bnode_to_have_key_erased.keys).value()->color() = false;

                                            refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<2>(bnode_to_have_key_erased.keys).value());
                                        }
                                        else if(std::get<0>(bnode_to_have_key_erased.keys).has_value() && std::get<2>(bnode_to_have_key_erased.keys).has_value())
                                        {
                                            typename navigator_t::loop_t parent_outer_loop;
                                            if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent || std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent)
                                                parent_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_to_have_key_erased.keys));

                                            typename navigator_t::loop_t child_left_inner_loop = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<0>(bnode_to_have_key_erased.keys).value());
                                            if(child_left_inner_loop.leftmost_descendent_of_child_right == nullptr)
                                                child_left_inner_loop.leftmost_descendent_of_child_right = child_left_inner_loop.child_right;
                                            if(child_left_inner_loop.leftmost_descendent_of_child_right == node)
                                                child_left_inner_loop.leftmost_descendent_of_child_right = static_cast<navigator_except_node_end_t *>(rb2p_iterator_t<false, is_reversed, config_t>::template predecessor<true>(child_left_inner_loop.leftmost_descendent_of_child_right));

                                            typename navigator_t::loop_t child_right_inner_loop = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<2>(bnode_to_have_key_erased.keys).value());
                                            if(child_right_inner_loop.rightmost_descendent_of_child_left == node)
                                                child_right_inner_loop.rightmost_descendent_of_child_left = static_cast<navigator_except_node_end_t *>(rb2p_iterator_t<false, is_reversed, config_t>::template predecessor<false>(child_right_inner_loop.rightmost_descendent_of_child_left));

                                            rb2p_node_role_e center_key_role = std::get<1>(bnode_to_have_key_erased.keys)->role();
                                            navigator_except_node_end_t *child_after_merge = merge_and_return_child_after_merge(rb2p_node_role_e::child_left_not_a_leftmost_descendent);

                                            if(parent_outer_loop.this_ != nullptr)
                                                parent_outer_loop.exchange(std::get<0>(bnode_to_have_key_erased.keys).value());
                                            else
                                            {
                                                assert(!should_link_loop_end);
                                                assert(loop_end.root == std::get<1>(bnode_to_have_key_erased.keys));
                                                loop_end.root = std::get<0>(bnode_to_have_key_erased.keys).value();
                                                should_link_loop_end = true;
                                            }

                                            child_left_inner_loop.child_right = std::get<2>(bnode_to_have_key_erased.keys).value();
                                            child_left_inner_loop.index = -1, child_left_inner_loop.link();

                                            child_right_inner_loop.child_left = child_after_merge;
                                            child_right_inner_loop.index = -1, child_right_inner_loop.link();

                                            std::get<0>(bnode_to_have_key_erased.keys).value()->role() = center_key_role;
                                            std::get<0>(bnode_to_have_key_erased.keys).value()->color() = false;

                                            refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<2>(bnode_to_have_key_erased.keys).value());
                                        }
                                        else std::unreachable();
                                    }
                                }
                                else if(bnode_to_have_key_erased.key_to_be_erased_index == 2)
                                {
                                    if(child_after_merge_height == 0)
                                    {
                                        if(!std::get<0>(bnode_to_have_key_erased.keys).has_value())
                                        {
                                            if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::root)
                                            {
                                                assert(std::get<2>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root);

                                                assert(!should_link_loop_end);
                                                assert(loop_end.rightmost_descendent_of_root == std::get<2>(bnode_to_have_key_erased.keys).value());
                                                assert(loop_end.root == std::get<1>(bnode_to_have_key_erased.keys));
                                                assert(loop_end.leftmost_descendent_of_root == nullptr);
                                                loop_end.rightmost_descendent_of_root = nullptr;
                                                should_link_loop_end = true;

                                                typename navigator_t::loop_t{
                                                    .this_ = std::get<1>(bnode_to_have_key_erased.keys),
                                                }
                                                    .link();
                                            }
                                            else if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent)
                                            {
                                                assert(std::get<2>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root || std::get<2>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_non_root);
                                                if(std::get<2>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root)
                                                {
                                                    assert(!should_link_loop_end);
                                                    assert(loop_end.rightmost_descendent_of_root == std::get<2>(bnode_to_have_key_erased.keys).value());
                                                    loop_end.rightmost_descendent_of_root = std::get<1>(bnode_to_have_key_erased.keys);
                                                    should_link_loop_end = true;
                                                }
                                                else if(std::get<2>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_non_root)
                                                {
                                                    typename navigator_t::loop_t child_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::ancestor>(std::get<2>(bnode_to_have_key_erased.keys).value());
                                                    child_outer_loop.exchange(std::get<1>(bnode_to_have_key_erased.keys));
                                                }
                                                else std::unreachable();
                                                std::get<1>(bnode_to_have_key_erased.keys)->role() = std::get<2>(bnode_to_have_key_erased.keys).value()->role();
                                            }
                                            else std::unreachable();
                                        }
                                        else
                                        {
                                            if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::root)
                                            {
                                                assert(std::get<2>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root);
                                                assert(std::get<0>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root);

                                                assert(!should_link_loop_end);
                                                assert(loop_end.rightmost_descendent_of_root == std::get<2>(bnode_to_have_key_erased.keys).value());
                                                assert(loop_end.root == std::get<1>(bnode_to_have_key_erased.keys));
                                                assert(loop_end.leftmost_descendent_of_root == std::get<0>(bnode_to_have_key_erased.keys).value());
                                                loop_end.rightmost_descendent_of_root = nullptr;
                                                should_link_loop_end = true;

                                                typename navigator_t::loop_t{
                                                    .this_ = std::get<1>(bnode_to_have_key_erased.keys),
                                                    .child_left = std::get<0>(bnode_to_have_key_erased.keys).value(),
                                                }
                                                    .link();
                                            }
                                            else if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent)
                                            {
                                                assert(std::get<2>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root || std::get<2>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_non_root);
                                                assert(std::get<0>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_non_root);

                                                typename navigator_t::loop_t parent_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_to_have_key_erased.keys));
                                                typename navigator_t::loop_t child_right_outer_loop;
                                                if(std::get<2>(bnode_to_have_key_erased.keys).value()->role() != rb2p_node_role_e::child_right_rightmost_descendent_of_root)
                                                    child_right_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::ancestor>(std::get<2>(bnode_to_have_key_erased.keys).value());

                                                if(std::get<2>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root)
                                                {
                                                    assert(!should_link_loop_end);
                                                    assert(loop_end.rightmost_descendent_of_root == std::get<2>(bnode_to_have_key_erased.keys).value());
                                                    loop_end.rightmost_descendent_of_root = std::get<1>(bnode_to_have_key_erased.keys);
                                                    should_link_loop_end = true;
                                                }
                                                else if(std::get<2>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_non_root)
                                                    child_right_outer_loop.exchange(std::get<1>(bnode_to_have_key_erased.keys));
                                                else std::unreachable();

                                                assert(parent_outer_loop.child_right == std::get<1>(bnode_to_have_key_erased.keys));
                                                assert(parent_outer_loop.leftmost_descendent_of_child_right == std::get<0>(bnode_to_have_key_erased.keys).value());
                                                parent_outer_loop.child_right = std::get<0>(bnode_to_have_key_erased.keys).value();
                                                parent_outer_loop.leftmost_descendent_of_child_right = nullptr;
                                                parent_outer_loop.index = -1, parent_outer_loop.link();

                                                typename navigator_t::loop_t{
                                                    .this_ = std::get<0>(bnode_to_have_key_erased.keys).value(),
                                                    .child_right = std::get<1>(bnode_to_have_key_erased.keys),
                                                }
                                                    .link();

                                                std::get<0>(bnode_to_have_key_erased.keys).value()->role() = std::get<1>(bnode_to_have_key_erased.keys)->role();
                                                std::get<0>(bnode_to_have_key_erased.keys).value()->color() = std::get<1>(bnode_to_have_key_erased.keys)->color();
                                                std::get<1>(bnode_to_have_key_erased.keys)->role() = std::get<2>(bnode_to_have_key_erased.keys).value()->role();
                                                std::get<1>(bnode_to_have_key_erased.keys)->color() = std::get<2>(bnode_to_have_key_erased.keys).value()->color();
                                            }
                                            else if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent)
                                            {
                                                assert(std::get<2>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_non_root);
                                                assert(std::get<0>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root || std::get<0>(bnode_to_have_key_erased.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_non_root);

                                                typename navigator_t::loop_t parent_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_to_have_key_erased.keys));
                                                typename navigator_t::loop_t child_right_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::ancestor>(std::get<2>(bnode_to_have_key_erased.keys).value());
                                                assert(parent_outer_loop.this_ == child_right_outer_loop.this_);

                                                assert(parent_outer_loop.child_left == std::get<1>(bnode_to_have_key_erased.keys));
                                                assert(parent_outer_loop.rightmost_descendent_of_child_left == std::get<2>(bnode_to_have_key_erased.keys).value());
                                                parent_outer_loop.rightmost_descendent_of_child_left = nullptr;
                                                parent_outer_loop.index = -1, parent_outer_loop.link();

                                                typename navigator_t::loop_t{
                                                    .this_ = std::get<1>(bnode_to_have_key_erased.keys),
                                                    .child_left = std::get<0>(bnode_to_have_key_erased.keys).value(),
                                                }
                                                    .link();
                                            }
                                            else std::unreachable();
                                        }
                                    }
                                    else
                                    {
                                        typename navigator_t::loop_t parent_inner_loop = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<1>(bnode_to_have_key_erased.keys));
                                        if(parent_inner_loop.leftmost_descendent_of_child_right == node)
                                            parent_inner_loop.leftmost_descendent_of_child_right = static_cast<navigator_except_node_end_t *>(rb2p_iterator_t<false, is_reversed, config_t>::template predecessor<true>(parent_inner_loop.leftmost_descendent_of_child_right));

                                        navigator_except_node_end_t *child_after_merge = merge_and_return_child_after_merge(rb2p_node_role_e::child_right_not_a_rightmost_descendent);

                                        parent_inner_loop.child_right = child_after_merge;
                                        parent_inner_loop.index = -1, parent_inner_loop.link();
                                    }

                                    refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<1>(bnode_to_have_key_erased.keys));
                                }
                                else std::unreachable();
                                height_changed = false;
                            }
                            else
                            {
                                if(bnode_to_have_key_erased.center_key_parent_info.is_end())
                                {
                                    assert(child_after_merge_height != 0);

                                    navigator_except_node_end_t *child_after_merge = merge_and_return_child_after_merge(rb2p_node_role_e::root);

                                    assert(loop_end.root == std::get<1>(bnode_to_have_key_erased.keys));
                                    loop_end.root = child_after_merge;
                                    should_link_loop_end = true;
                                    height_changed = true;
                                }
                                else
                                {
                                    bnode_up_t bnode_parent = bnode_up_t::get_bnode_from_key(static_cast<navigator_except_node_end_t *>(bnode_to_have_key_erased.center_key_parent_info.parent), bnode_to_have_key_erased.center_key_parent_info.is_left_or_right_child_of_parent);

                                    std::optional<navigator_except_node_end_t *> opt_node_left_sibling_black;
                                    bnode_erase_t bnode_left_sibling;
                                    navigator_t *bnode_left_sibling_rightmost_child, *bnode_left_sibling_second_rightmost_child;

                                    std::optional<navigator_except_node_end_t *> opt_node_right_sibling_black;
                                    bnode_erase_t bnode_right_sibling;
                                    navigator_t *bnode_right_sibling_leftmost_child, *bnode_right_sibling_second_leftmost_child;

                                    auto try_grab_from_left_sibling = [&](auto fallback)
                                    {
                                        return [&, fallback]()
                                        {
                                            if(opt_node_left_sibling_black = bnode_parent.opt_child_at(bnode_parent.child_index - 2); opt_node_left_sibling_black.has_value())
                                            {
                                                std::tie(bnode_left_sibling, bnode_left_sibling_rightmost_child, bnode_left_sibling_second_rightmost_child) = bnode_erase_t::erasing_get_bnode_from_key_black_sibling(opt_node_left_sibling_black.value(), false);
                                                if(bnode_left_sibling.key_count != 1)
                                                {
                                                    navigator_except_node_end_t *parent = bnode_parent.key_at(bnode_parent.child_index - 1);
                                                    assert(parent->role() == rb2p_node_role_e::root || parent->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent || parent->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent);
                                                    typename navigator_t::loop_t parent_outer_loop;
                                                    if(parent->role() != rb2p_node_role_e::root)
                                                        parent_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(parent);
                                                    typename navigator_t::loop_t parent_inner_loop = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(parent);

                                                    if(child_after_merge_height == 0)
                                                    {
                                                        typename navigator_t::loop_t child_right_outer_loop;
                                                        if(!std::get<0>(bnode_left_sibling.keys).has_value() && std::get<2>(bnode_left_sibling.keys).has_value())
                                                        {
                                                            if(parent->role() == rb2p_node_role_e::root)
                                                            {
                                                                assert(!should_link_loop_end);
                                                                assert(loop_end.root == parent);
                                                                loop_end.root = std::get<2>(bnode_left_sibling.keys).value();
                                                                if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root)
                                                                {
                                                                    assert(loop_end.rightmost_descendent_of_root == std::get<1>(bnode_to_have_key_erased.keys));
                                                                    loop_end.rightmost_descendent_of_root = parent;
                                                                }
                                                                should_link_loop_end = true;
                                                            }
                                                            else if(parent->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent)
                                                            {
                                                                assert(parent_outer_loop.child_left == parent);
                                                                if(parent_outer_loop.rightmost_descendent_of_child_left == std::get<1>(bnode_to_have_key_erased.keys))
                                                                    parent_outer_loop.rightmost_descendent_of_child_left = parent;
                                                                parent_outer_loop.child_left = std::get<2>(bnode_left_sibling.keys).value();
                                                            }
                                                            else if(parent->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent)
                                                            {
                                                                assert(parent_outer_loop.child_right == parent);
                                                                parent_outer_loop.child_right = std::get<2>(bnode_left_sibling.keys).value();
                                                                if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root)
                                                                {
                                                                    assert(!should_link_loop_end);
                                                                    assert(loop_end.rightmost_descendent_of_root == std::get<1>(bnode_to_have_key_erased.keys));
                                                                    loop_end.rightmost_descendent_of_root = parent;
                                                                    should_link_loop_end = true;
                                                                }
                                                            }
                                                            else std::unreachable();

                                                            if(parent_inner_loop.child_right == std::get<1>(bnode_to_have_key_erased.keys))
                                                            {
                                                                parent_inner_loop.child_right = parent;
                                                                if(parent->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent && std::get<1>(bnode_to_have_key_erased.keys)->role() != rb2p_node_role_e::child_right_rightmost_descendent_of_root)
                                                                    child_right_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::ancestor>(std::get<1>(bnode_to_have_key_erased.keys));
                                                            }
                                                            else if(parent_inner_loop.leftmost_descendent_of_child_right == std::get<1>(bnode_to_have_key_erased.keys))
                                                            {
                                                                parent_inner_loop.leftmost_descendent_of_child_right = parent;
                                                                child_right_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_to_have_key_erased.keys));
                                                            }
                                                            else std::unreachable();
                                                            parent_inner_loop.this_ = std::get<2>(bnode_left_sibling.keys).value();
                                                            parent_inner_loop.rightmost_descendent_of_child_left = std::get<1>(bnode_left_sibling.keys);
                                                            parent_inner_loop.index = -1, parent_inner_loop.link();
                                                            if(child_right_outer_loop.this_ != nullptr)
                                                                child_right_outer_loop.exchange(parent);

                                                            if(parent->role() != rb2p_node_role_e::root)
                                                                parent_outer_loop.index = -1, parent_outer_loop.link();

                                                            std::get<1>(bnode_left_sibling.keys)->role() = std::get<2>(bnode_left_sibling.keys).value()->role();
                                                            std::get<2>(bnode_left_sibling.keys).value()->role() = parent->role();
                                                            std::get<2>(bnode_left_sibling.keys).value()->color() = parent->color();
                                                            parent->role() = std::get<1>(bnode_to_have_key_erased.keys)->role();
                                                            parent->color() = std::get<1>(bnode_to_have_key_erased.keys)->color();

                                                            refresh_node_count_and_accumulated_storage_and_above_until(schedules, accumulator, parent, std::get<2>(bnode_left_sibling.keys).value());
                                                            refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<1>(bnode_left_sibling.keys));
                                                        }
                                                        else if(std::get<0>(bnode_left_sibling.keys).has_value() && !std::get<2>(bnode_left_sibling.keys).has_value())
                                                        {
                                                            if(parent->role() == rb2p_node_role_e::root)
                                                            {
                                                                assert(!should_link_loop_end);
                                                                assert(loop_end.root == parent);
                                                                assert(loop_end.leftmost_descendent_of_root == std::get<0>(bnode_left_sibling.keys).value());
                                                                loop_end.root = std::get<1>(bnode_left_sibling.keys);
                                                                if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root)
                                                                {
                                                                    assert(loop_end.rightmost_descendent_of_root == std::get<1>(bnode_to_have_key_erased.keys));
                                                                    loop_end.rightmost_descendent_of_root = parent;
                                                                }
                                                                should_link_loop_end = true;
                                                            }
                                                            else if(parent->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent)
                                                            {
                                                                assert(parent_outer_loop.child_left == parent);
                                                                if(parent_outer_loop.rightmost_descendent_of_child_left == std::get<1>(bnode_to_have_key_erased.keys))
                                                                    parent_outer_loop.rightmost_descendent_of_child_left = parent;
                                                                parent_outer_loop.child_left = std::get<1>(bnode_left_sibling.keys);
                                                            }
                                                            else if(parent->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent)
                                                            {
                                                                assert(parent_outer_loop.child_right == parent);
                                                                parent_outer_loop.child_right = std::get<1>(bnode_left_sibling.keys);
                                                                if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root)
                                                                {
                                                                    assert(!should_link_loop_end);
                                                                    assert(loop_end.rightmost_descendent_of_root == std::get<1>(bnode_to_have_key_erased.keys));
                                                                    loop_end.rightmost_descendent_of_root = parent;
                                                                    should_link_loop_end = true;
                                                                }
                                                            }
                                                            else std::unreachable();

                                                            if(parent_inner_loop.child_right == std::get<1>(bnode_to_have_key_erased.keys))
                                                            {
                                                                parent_inner_loop.child_right = parent;
                                                                if(parent->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent && std::get<1>(bnode_to_have_key_erased.keys)->role() != rb2p_node_role_e::child_right_rightmost_descendent_of_root)
                                                                    child_right_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::ancestor>(std::get<1>(bnode_to_have_key_erased.keys));
                                                            }
                                                            else if(parent_inner_loop.leftmost_descendent_of_child_right == std::get<1>(bnode_to_have_key_erased.keys))
                                                            {
                                                                parent_inner_loop.leftmost_descendent_of_child_right = parent;
                                                                child_right_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_to_have_key_erased.keys));
                                                            }
                                                            else std::unreachable();
                                                            parent_inner_loop.this_ = std::get<1>(bnode_left_sibling.keys);
                                                            parent_inner_loop.child_left = std::get<0>(bnode_left_sibling.keys).value();
                                                            parent_inner_loop.index = -1, parent_inner_loop.link();
                                                            if(child_right_outer_loop.this_ != nullptr)
                                                                child_right_outer_loop.exchange(parent);

                                                            if(parent->role() != rb2p_node_role_e::root)
                                                                parent_outer_loop.index = -1, parent_outer_loop.link();

                                                            std::get<0>(bnode_left_sibling.keys).value()->color() = std::get<1>(bnode_left_sibling.keys)->color();
                                                            std::get<1>(bnode_left_sibling.keys)->role() = parent->role();
                                                            std::get<1>(bnode_left_sibling.keys)->color() = parent->color();
                                                            parent->role() = std::get<1>(bnode_to_have_key_erased.keys)->role();
                                                            parent->color() = std::get<1>(bnode_to_have_key_erased.keys)->color();

                                                            refresh_node_count_and_accumulated_storage_and_above_until(schedules, accumulator, parent, std::get<1>(bnode_left_sibling.keys));
                                                            refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<0>(bnode_left_sibling.keys).value());
                                                        }
                                                        else if(std::get<0>(bnode_left_sibling.keys).has_value() && std::get<2>(bnode_left_sibling.keys).has_value())
                                                        {
                                                            assert(std::get<1>(bnode_left_sibling.keys)->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent || std::get<1>(bnode_left_sibling.keys)->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent);
                                                            if(parent->role() == rb2p_node_role_e::root)
                                                            {
                                                                assert(!should_link_loop_end);
                                                                assert(loop_end.root == parent);
                                                                loop_end.root = std::get<2>(bnode_left_sibling.keys).value();
                                                                if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root)
                                                                {
                                                                    assert(loop_end.rightmost_descendent_of_root == std::get<1>(bnode_to_have_key_erased.keys));
                                                                    loop_end.rightmost_descendent_of_root = parent;
                                                                }
                                                                should_link_loop_end = true;
                                                            }
                                                            else if(parent->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent)
                                                            {
                                                                assert(parent_outer_loop.child_left == parent);
                                                                if(parent_outer_loop.rightmost_descendent_of_child_left == std::get<1>(bnode_to_have_key_erased.keys))
                                                                    parent_outer_loop.rightmost_descendent_of_child_left = parent;
                                                                parent_outer_loop.child_left = std::get<2>(bnode_left_sibling.keys).value();
                                                            }
                                                            else if(parent->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent)
                                                            {
                                                                assert(parent_outer_loop.child_right == parent);
                                                                parent_outer_loop.child_right = std::get<2>(bnode_left_sibling.keys).value();
                                                                if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root)
                                                                {
                                                                    assert(!should_link_loop_end);
                                                                    assert(loop_end.rightmost_descendent_of_root == std::get<1>(bnode_to_have_key_erased.keys));
                                                                    loop_end.rightmost_descendent_of_root = parent;
                                                                    should_link_loop_end = true;
                                                                }
                                                            }
                                                            else std::unreachable();

                                                            if(parent_inner_loop.child_right == std::get<1>(bnode_to_have_key_erased.keys))
                                                            {
                                                                parent_inner_loop.child_right = parent;
                                                                if(parent->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent && std::get<1>(bnode_to_have_key_erased.keys)->role() != rb2p_node_role_e::child_right_rightmost_descendent_of_root)
                                                                    child_right_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::ancestor>(std::get<1>(bnode_to_have_key_erased.keys));
                                                            }
                                                            else if(parent_inner_loop.leftmost_descendent_of_child_right == std::get<1>(bnode_to_have_key_erased.keys))
                                                            {
                                                                parent_inner_loop.leftmost_descendent_of_child_right = parent;
                                                                child_right_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_to_have_key_erased.keys));
                                                            }
                                                            else std::unreachable();
                                                            parent_inner_loop.this_ = std::get<2>(bnode_left_sibling.keys).value();
                                                            assert(parent_inner_loop.rightmost_descendent_of_child_left == std::get<2>(bnode_left_sibling.keys).value());
                                                            if(parent_inner_loop.child_left == std::get<1>(bnode_left_sibling.keys))
                                                            {
                                                                parent_inner_loop.rightmost_descendent_of_child_left = nullptr;

                                                                typename navigator_t::loop_t{
                                                                    .this_ = std::get<1>(bnode_left_sibling.keys),
                                                                    .child_left = std::get<0>(bnode_left_sibling.keys).value(),
                                                                }
                                                                    .link();
                                                            }
                                                            else
                                                            {
                                                                parent_inner_loop.rightmost_descendent_of_child_left = std::get<1>(bnode_left_sibling.keys);

                                                                typename navigator_t::loop_t child_left_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_left_sibling.keys));
                                                                assert(child_left_outer_loop.child_right == std::get<1>(bnode_left_sibling.keys));
                                                                assert(child_left_outer_loop.leftmost_descendent_of_child_right == std::get<0>(bnode_left_sibling.keys).value());
                                                                child_left_outer_loop.child_right = std::get<0>(bnode_left_sibling.keys).value();
                                                                child_left_outer_loop.leftmost_descendent_of_child_right = nullptr;
                                                                child_left_outer_loop.index = -1, child_left_outer_loop.link();

                                                                typename navigator_t::loop_t{
                                                                    .this_ = std::get<0>(bnode_left_sibling.keys).value(),
                                                                    .child_right = std::get<1>(bnode_left_sibling.keys),
                                                                }
                                                                    .link();
                                                                std::get<0>(bnode_left_sibling.keys).value()->color() = false;
                                                                std::get<0>(bnode_left_sibling.keys).value()->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                                std::get<1>(bnode_left_sibling.keys)->color() = true;
                                                                std::get<1>(bnode_left_sibling.keys)->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                            }
                                                            parent_inner_loop.index = -1, parent_inner_loop.link();
                                                            if(child_right_outer_loop.this_ != nullptr)
                                                                child_right_outer_loop.exchange(parent);

                                                            if(parent->role() != rb2p_node_role_e::root)
                                                                parent_outer_loop.index = -1, parent_outer_loop.link();

                                                            std::get<2>(bnode_left_sibling.keys).value()->role() = parent->role();
                                                            std::get<2>(bnode_left_sibling.keys).value()->color() = parent->color();
                                                            parent->role() = std::get<1>(bnode_to_have_key_erased.keys)->role();
                                                            parent->color() = std::get<1>(bnode_to_have_key_erased.keys)->color();

                                                            refresh_node_count_and_accumulated_storage_and_above_until(schedules, accumulator, parent, std::get<2>(bnode_left_sibling.keys).value());
                                                            refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<1>(bnode_left_sibling.keys));
                                                        }
                                                        else std::unreachable();
                                                    }
                                                    else
                                                    {
                                                        if(std::get<2>(bnode_left_sibling.keys).has_value())
                                                        {
                                                            bool child_after_merge_is_child_right_of_parent = std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                            typename navigator_t::loop_t child_right_outer_loop;
                                                            if(!child_after_merge_is_child_right_of_parent)
                                                            {
                                                                child_right_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_to_have_key_erased.keys));
                                                                if(child_right_outer_loop.rightmost_descendent_of_child_left == node)
                                                                    child_right_outer_loop.rightmost_descendent_of_child_left = static_cast<navigator_except_node_end_t *>(rb2p_iterator_t<false, is_reversed, config_t>::template predecessor<false>(child_right_outer_loop.rightmost_descendent_of_child_left));
                                                            }

                                                            typename navigator_t::loop_t child_right_inner_loop{
                                                                .this_ = parent,
                                                                .leftmost_descendent_of_child_right = parent_inner_loop.leftmost_descendent_of_child_right,
                                                            };
                                                            if(child_right_inner_loop.leftmost_descendent_of_child_right == node)
                                                                child_right_inner_loop.leftmost_descendent_of_child_right = static_cast<navigator_except_node_end_t *>(rb2p_iterator_t<false, is_reversed, config_t>::template predecessor<true>(child_right_inner_loop.leftmost_descendent_of_child_right));

                                                            typename navigator_t::loop_t child_left_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<2>(bnode_left_sibling.keys).value());
                                                            typename navigator_t::loop_t child_left_inner_loop = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<2>(bnode_left_sibling.keys).value());
                                                            navigator_except_node_end_t *key_right_child_left = std::get<2>(bnode_left_sibling.keys).value()->child_left(), *key_right_child_left_child_left, *key_right_child_left_child_right;
                                                            navigator_except_node_end_t *key_right_child_right = std::get<2>(bnode_left_sibling.keys).value()->child_right(), *key_right_child_right_child_left, *key_right_child_right_child_right;
                                                            if(child_after_merge_height == 1)
                                                            {
                                                                key_right_child_left_child_left = key_right_child_left->child_left();
                                                                key_right_child_left_child_right = key_right_child_left->child_right();
                                                                key_right_child_right_child_left = key_right_child_right->child_left();
                                                                key_right_child_right_child_right = key_right_child_right->child_right();
                                                            }

                                                            if(child_after_merge_height == 1 && key_right_child_left_child_left != nullptr && key_right_child_left_child_right == nullptr)
                                                            {
                                                                navigator_except_node_end_t *node_left = key_right_child_left_child_left;
                                                                navigator_except_node_end_t *node_right = key_right_child_left;
                                                                typename navigator_t::loop_t{
                                                                    .this_ = node_left,
                                                                    .child_right = node_right,
                                                                }
                                                                    .link();
                                                                node_left->color() = false;
                                                                node_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                                node_right->color() = true;
                                                                node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                                std::tie(child_left_inner_loop.child_left, child_left_inner_loop.rightmost_descendent_of_child_left) = std::make_tuple(node_left, node_right);
                                                                std::tie(child_left_outer_loop.child_right, child_left_outer_loop.leftmost_descendent_of_child_right) = std::make_tuple(node_left, nullptr);
                                                                move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                                            }
                                                            else
                                                            {
                                                                if(child_after_merge_height == 1 && key_right_child_left_child_left == nullptr && key_right_child_left_child_right == nullptr)
                                                                    key_right_child_left->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                                else
                                                                    key_right_child_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                                child_left_outer_loop.child_right = child_left_inner_loop.child_left;
                                                            }

                                                            if(child_after_merge_height == 1 && key_right_child_right_child_left == nullptr && key_right_child_right_child_right != nullptr)
                                                            {
                                                                navigator_except_node_end_t *node_left = key_right_child_right;
                                                                navigator_except_node_end_t *node_right = key_right_child_right_child_right;
                                                                typename navigator_t::loop_t{
                                                                    .this_ = node_right,
                                                                    .child_left = node_left,
                                                                }
                                                                    .link();
                                                                node_right->color() = false;
                                                                node_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                                node_left->color() = true;
                                                                node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                                std::tie(child_left_inner_loop.child_right, child_left_inner_loop.leftmost_descendent_of_child_right) = std::make_tuple(node_right, node_left);
                                                                std::tie(child_right_inner_loop.child_left, child_right_inner_loop.rightmost_descendent_of_child_left) = std::make_tuple(node_right, nullptr);
                                                                move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                                            }
                                                            else
                                                            {
                                                                if(child_after_merge_height == 1 && key_right_child_right_child_left == nullptr && key_right_child_right_child_right == nullptr)
                                                                    key_right_child_right->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                                else
                                                                    key_right_child_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                                std::tie(child_right_inner_loop.child_left, child_right_inner_loop.rightmost_descendent_of_child_left) = std::make_tuple(child_left_inner_loop.child_right, parent_inner_loop.rightmost_descendent_of_child_left);
                                                            }

                                                            parent_inner_loop.this_ = std::get<2>(bnode_left_sibling.keys).value();
                                                            parent_inner_loop.rightmost_descendent_of_child_left = child_left_inner_loop.rightmost_descendent_of_child_left != nullptr ? child_left_inner_loop.rightmost_descendent_of_child_left : child_left_inner_loop.child_left;
                                                            parent_inner_loop.leftmost_descendent_of_child_right = child_left_inner_loop.leftmost_descendent_of_child_right != nullptr ? child_left_inner_loop.leftmost_descendent_of_child_right : child_left_inner_loop.child_right;

                                                            if(parent->role() == rb2p_node_role_e::root)
                                                            {
                                                                assert(loop_end.root == parent);
                                                                loop_end.root = std::get<2>(bnode_left_sibling.keys).value();
                                                                should_link_loop_end = true;
                                                            }
                                                            else if(parent->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent)
                                                            {
                                                                assert(parent_outer_loop.child_left == parent);
                                                                parent_outer_loop.child_left = std::get<2>(bnode_left_sibling.keys).value();
                                                                if(parent_outer_loop.rightmost_descendent_of_child_left == node)
                                                                    parent_outer_loop.rightmost_descendent_of_child_left = static_cast<navigator_except_node_end_t *>(rb2p_iterator_t<false, is_reversed, config_t>::template predecessor<false>(parent_outer_loop.rightmost_descendent_of_child_left));
                                                            }
                                                            else if(parent->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent)
                                                            {
                                                                assert(parent_outer_loop.child_right == parent);
                                                                parent_outer_loop.child_right = std::get<2>(bnode_left_sibling.keys).value();
                                                            }
                                                            else std::unreachable();

                                                            rb2p_node_role_e child_after_merge_role = std::get<1>(bnode_to_have_key_erased.keys)->role();
                                                            navigator_except_node_end_t *child_after_merge = merge_and_return_child_after_merge(rb2p_node_role_e::child_right_not_a_rightmost_descendent);

                                                            if(!child_after_merge_is_child_right_of_parent)
                                                            {
                                                                child_right_outer_loop.child_left = parent;
                                                                child_right_outer_loop.index = -1, child_right_outer_loop.link();
                                                            }
                                                            else
                                                                parent_inner_loop.child_right = parent;
                                                            child_right_inner_loop.child_right = child_after_merge;
                                                            child_right_inner_loop.link();
                                                            parent_inner_loop.index = -1, parent_inner_loop.link();
                                                            child_left_outer_loop.index = -1, child_left_outer_loop.link();
                                                            if(parent->role() != rb2p_node_role_e::root)
                                                                parent_outer_loop.index = -1, parent_outer_loop.link();

                                                            std::get<2>(bnode_left_sibling.keys).value()->role() = parent->role();
                                                            std::get<2>(bnode_left_sibling.keys).value()->color() = parent->color();
                                                            parent->role() = child_after_merge_role;
                                                            parent->color() = false;

                                                            refresh_node_count_and_accumulated_storage_and_above_until(schedules, accumulator, parent, std::get<2>(bnode_left_sibling.keys).value());
                                                            refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<1>(bnode_left_sibling.keys));
                                                        }
                                                        else if(!std::get<2>(bnode_left_sibling.keys).has_value())
                                                        {
                                                            assert(std::get<0>(bnode_left_sibling.keys).has_value());

                                                            bool child_after_merge_is_child_right_of_parent = std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                            typename navigator_t::loop_t child_right_outer_loop;
                                                            if(!child_after_merge_is_child_right_of_parent)
                                                            {
                                                                child_right_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_to_have_key_erased.keys));
                                                                if(child_right_outer_loop.rightmost_descendent_of_child_left == node)
                                                                    child_right_outer_loop.rightmost_descendent_of_child_left = static_cast<navigator_except_node_end_t *>(rb2p_iterator_t<false, is_reversed, config_t>::template predecessor<false>(child_right_outer_loop.rightmost_descendent_of_child_left));
                                                            }

                                                            typename navigator_t::loop_t child_right_inner_loop{
                                                                .this_ = parent,
                                                                .leftmost_descendent_of_child_right = parent_inner_loop.leftmost_descendent_of_child_right,
                                                            };
                                                            if(child_right_inner_loop.leftmost_descendent_of_child_right == node)
                                                                child_right_inner_loop.leftmost_descendent_of_child_right = static_cast<navigator_except_node_end_t *>(rb2p_iterator_t<false, is_reversed, config_t>::template predecessor<true>(child_right_inner_loop.leftmost_descendent_of_child_right));

                                                            typename navigator_t::loop_t child_left_outer_loop;
                                                            typename navigator_t::loop_t child_left_inner_loop = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<1>(bnode_left_sibling.keys));
                                                            navigator_except_node_end_t *key_right_child_right = std::get<1>(bnode_left_sibling.keys)->child_right(), *key_right_child_right_child_left, *key_right_child_right_child_right;
                                                            if(child_after_merge_height == 1)
                                                            {
                                                                key_right_child_right_child_left = key_right_child_right->child_left();
                                                                key_right_child_right_child_right = key_right_child_right->child_right();
                                                            }

                                                            if(std::get<1>(bnode_left_sibling.keys)->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent)
                                                            {
                                                                assert(parent_inner_loop.child_left == std::get<1>(bnode_left_sibling.keys));
                                                                parent_inner_loop.child_left = std::get<0>(bnode_left_sibling.keys).value();
                                                            }
                                                            else if(std::get<1>(bnode_left_sibling.keys)->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent)
                                                            {
                                                                child_left_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_left_sibling.keys));
                                                                assert(child_left_outer_loop.child_right == std::get<1>(bnode_left_sibling.keys));
                                                                child_left_outer_loop.child_right = std::get<0>(bnode_left_sibling.keys).value();
                                                            }
                                                            else std::unreachable();

                                                            if(child_after_merge_height == 1 && key_right_child_right_child_left == nullptr && key_right_child_right_child_right != nullptr)
                                                            {
                                                                navigator_except_node_end_t *node_left = key_right_child_right;
                                                                navigator_except_node_end_t *node_right = key_right_child_right_child_right;
                                                                typename navigator_t::loop_t{
                                                                    .this_ = node_right,
                                                                    .child_left = node_left,
                                                                }
                                                                    .link();
                                                                node_right->color() = false;
                                                                node_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                                node_left->color() = true;
                                                                node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                                std::tie(child_left_inner_loop.child_right, child_left_inner_loop.leftmost_descendent_of_child_right) = std::make_tuple(node_right, node_left);
                                                                std::tie(child_right_inner_loop.child_left, child_right_inner_loop.rightmost_descendent_of_child_left) = std::make_tuple(node_right, nullptr);
                                                                move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                                            }
                                                            else
                                                            {
                                                                if(child_after_merge_height == 1 && key_right_child_right_child_left == nullptr && key_right_child_right_child_right == nullptr)
                                                                    key_right_child_right->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                                else
                                                                    key_right_child_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                                std::tie(child_right_inner_loop.child_left, child_right_inner_loop.rightmost_descendent_of_child_left) = std::make_tuple(child_left_inner_loop.child_right, parent_inner_loop.rightmost_descendent_of_child_left);
                                                            }

                                                            parent_inner_loop.this_ = std::get<1>(bnode_left_sibling.keys);
                                                            parent_inner_loop.rightmost_descendent_of_child_left = child_left_inner_loop.rightmost_descendent_of_child_left != nullptr ? child_left_inner_loop.rightmost_descendent_of_child_left : child_left_inner_loop.child_left;
                                                            parent_inner_loop.leftmost_descendent_of_child_right = child_left_inner_loop.leftmost_descendent_of_child_right != nullptr ? child_left_inner_loop.leftmost_descendent_of_child_right : child_left_inner_loop.child_right;

                                                            if(parent->role() == rb2p_node_role_e::root)
                                                            {
                                                                assert(loop_end.root == parent);
                                                                loop_end.root = std::get<1>(bnode_left_sibling.keys);
                                                                should_link_loop_end = true;
                                                            }
                                                            else if(parent->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent)
                                                            {
                                                                assert(parent_outer_loop.child_left == parent);
                                                                parent_outer_loop.child_left = std::get<1>(bnode_left_sibling.keys);
                                                                if(parent_outer_loop.rightmost_descendent_of_child_left == node)
                                                                    parent_outer_loop.rightmost_descendent_of_child_left = static_cast<navigator_except_node_end_t *>(rb2p_iterator_t<false, is_reversed, config_t>::template predecessor<false>(parent_outer_loop.rightmost_descendent_of_child_left));
                                                            }
                                                            else if(parent->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent)
                                                            {
                                                                assert(parent_outer_loop.child_right == parent);
                                                                parent_outer_loop.child_right = std::get<1>(bnode_left_sibling.keys);
                                                            }
                                                            else std::unreachable();

                                                            rb2p_node_role_e child_after_merge_role = std::get<1>(bnode_to_have_key_erased.keys)->role();
                                                            navigator_except_node_end_t *child_after_merge = merge_and_return_child_after_merge(rb2p_node_role_e::child_right_not_a_rightmost_descendent);

                                                            if(!child_after_merge_is_child_right_of_parent)
                                                            {
                                                                child_right_outer_loop.child_left = parent;
                                                                child_right_outer_loop.index = -1, child_right_outer_loop.link();
                                                            }
                                                            else
                                                                parent_inner_loop.child_right = parent;
                                                            child_right_inner_loop.child_right = child_after_merge;
                                                            child_right_inner_loop.link();
                                                            parent_inner_loop.index = -1, parent_inner_loop.link();
                                                            if(child_left_outer_loop.this_ != nullptr)
                                                                child_left_outer_loop.index = -1, child_left_outer_loop.link();
                                                            if(parent->role() != rb2p_node_role_e::root)
                                                                parent_outer_loop.index = -1, parent_outer_loop.link();

                                                            std::get<0>(bnode_left_sibling.keys).value()->role() = std::get<1>(bnode_left_sibling.keys)->role();
                                                            std::get<0>(bnode_left_sibling.keys).value()->color() = std::get<1>(bnode_left_sibling.keys)->color();
                                                            std::get<1>(bnode_left_sibling.keys)->role() = parent->role();
                                                            std::get<1>(bnode_left_sibling.keys)->color() = parent->color();
                                                            parent->role() = child_after_merge_role;
                                                            parent->color() = false;

                                                            refresh_node_count_and_accumulated_storage_and_above_until(schedules, accumulator, parent, std::get<1>(bnode_left_sibling.keys));
                                                            refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<0>(bnode_left_sibling.keys).value());
                                                        }
                                                        else std::unreachable();
                                                    }
                                                }
                                                else
                                                    fallback();
                                            }
                                            else
                                                fallback();
                                        };
                                    };
                                    auto try_grab_from_right_sibling = [&](auto fallback)
                                    {
                                        return [&, fallback]()
                                        {
                                            if(opt_node_right_sibling_black = bnode_parent.opt_child_at(bnode_parent.child_index + 2); opt_node_right_sibling_black.has_value())
                                            {
                                                std::tie(bnode_right_sibling, bnode_right_sibling_leftmost_child, bnode_right_sibling_second_leftmost_child) = bnode_erase_t::erasing_get_bnode_from_key_black_sibling(opt_node_right_sibling_black.value(), true);
                                                if(bnode_right_sibling.key_count != 1)
                                                {
                                                    navigator_except_node_end_t *parent = bnode_parent.key_at(bnode_parent.child_index + 1);
                                                    assert(parent->role() == rb2p_node_role_e::root || parent->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent || parent->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent);
                                                    typename navigator_t::loop_t parent_outer_loop;
                                                    if(parent->role() != rb2p_node_role_e::root)
                                                        parent_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(parent);
                                                    typename navigator_t::loop_t parent_inner_loop = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(parent);
                                                    if(child_after_merge_height == 0)
                                                    {
                                                        typename navigator_t::loop_t child_left_outer_loop;
                                                        if(!std::get<2>(bnode_right_sibling.keys).has_value() && std::get<0>(bnode_right_sibling.keys).has_value())
                                                        {
                                                            if(parent->role() == rb2p_node_role_e::root)
                                                            {
                                                                assert(!should_link_loop_end);
                                                                assert(loop_end.root == parent);
                                                                loop_end.root = std::get<0>(bnode_right_sibling.keys).value();
                                                                if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root)
                                                                {
                                                                    assert(loop_end.leftmost_descendent_of_root == std::get<1>(bnode_to_have_key_erased.keys));
                                                                    loop_end.leftmost_descendent_of_root = parent;
                                                                }
                                                                should_link_loop_end = true;
                                                            }
                                                            else if(parent->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent)
                                                            {
                                                                assert(parent_outer_loop.child_right == parent);
                                                                if(parent_outer_loop.leftmost_descendent_of_child_right == std::get<1>(bnode_to_have_key_erased.keys))
                                                                    parent_outer_loop.leftmost_descendent_of_child_right = parent;
                                                                parent_outer_loop.child_right = std::get<0>(bnode_right_sibling.keys).value();
                                                            }
                                                            else if(parent->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent)
                                                            {
                                                                assert(parent_outer_loop.child_left == parent);
                                                                parent_outer_loop.child_left = std::get<0>(bnode_right_sibling.keys).value();
                                                                if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root)
                                                                {
                                                                    assert(!should_link_loop_end);
                                                                    assert(loop_end.leftmost_descendent_of_root == std::get<1>(bnode_to_have_key_erased.keys));
                                                                    loop_end.leftmost_descendent_of_root = parent;
                                                                    should_link_loop_end = true;
                                                                }
                                                            }
                                                            else std::unreachable();

                                                            if(parent_inner_loop.child_left == std::get<1>(bnode_to_have_key_erased.keys))
                                                            {
                                                                parent_inner_loop.child_left = parent;
                                                                if(parent->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent && std::get<1>(bnode_to_have_key_erased.keys)->role() != rb2p_node_role_e::child_left_leftmost_descendent_of_root)
                                                                    child_left_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::ancestor>(std::get<1>(bnode_to_have_key_erased.keys));
                                                            }
                                                            else if(parent_inner_loop.rightmost_descendent_of_child_left == std::get<1>(bnode_to_have_key_erased.keys))
                                                            {
                                                                parent_inner_loop.rightmost_descendent_of_child_left = parent;
                                                                child_left_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_to_have_key_erased.keys));
                                                            }
                                                            else std::unreachable();
                                                            parent_inner_loop.this_ = std::get<0>(bnode_right_sibling.keys).value();
                                                            parent_inner_loop.leftmost_descendent_of_child_right = std::get<1>(bnode_right_sibling.keys);
                                                            parent_inner_loop.index = -1, parent_inner_loop.link();
                                                            if(child_left_outer_loop.this_ != nullptr)
                                                                child_left_outer_loop.exchange(parent);

                                                            if(parent->role() != rb2p_node_role_e::root)
                                                                parent_outer_loop.index = -1, parent_outer_loop.link();

                                                            std::get<1>(bnode_right_sibling.keys)->role() = std::get<0>(bnode_right_sibling.keys).value()->role();
                                                            std::get<0>(bnode_right_sibling.keys).value()->role() = parent->role();
                                                            std::get<0>(bnode_right_sibling.keys).value()->color() = parent->color();
                                                            parent->role() = std::get<1>(bnode_to_have_key_erased.keys)->role();
                                                            parent->color() = std::get<1>(bnode_to_have_key_erased.keys)->color();

                                                            refresh_node_count_and_accumulated_storage_and_above_until(schedules, accumulator, parent, std::get<0>(bnode_right_sibling.keys).value());
                                                            refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<1>(bnode_right_sibling.keys));
                                                        }
                                                        else if(std::get<2>(bnode_right_sibling.keys).has_value() && !std::get<0>(bnode_right_sibling.keys).has_value())
                                                        {
                                                            if(parent->role() == rb2p_node_role_e::root)
                                                            {
                                                                assert(!should_link_loop_end);
                                                                assert(loop_end.root == parent);
                                                                assert(loop_end.rightmost_descendent_of_root == std::get<2>(bnode_right_sibling.keys).value());
                                                                loop_end.root = std::get<1>(bnode_right_sibling.keys);
                                                                if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root)
                                                                {
                                                                    assert(loop_end.leftmost_descendent_of_root == std::get<1>(bnode_to_have_key_erased.keys));
                                                                    loop_end.leftmost_descendent_of_root = parent;
                                                                }
                                                                should_link_loop_end = true;
                                                            }
                                                            else if(parent->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent)
                                                            {
                                                                assert(parent_outer_loop.child_right == parent);
                                                                if(parent_outer_loop.leftmost_descendent_of_child_right == std::get<1>(bnode_to_have_key_erased.keys))
                                                                    parent_outer_loop.leftmost_descendent_of_child_right = parent;
                                                                parent_outer_loop.child_right = std::get<1>(bnode_right_sibling.keys);
                                                            }
                                                            else if(parent->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent)
                                                            {
                                                                assert(parent_outer_loop.child_left == parent);
                                                                parent_outer_loop.child_left = std::get<1>(bnode_right_sibling.keys);
                                                                if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root)
                                                                {
                                                                    assert(!should_link_loop_end);
                                                                    assert(loop_end.leftmost_descendent_of_root == std::get<1>(bnode_to_have_key_erased.keys));
                                                                    loop_end.leftmost_descendent_of_root = parent;
                                                                    should_link_loop_end = true;
                                                                }
                                                            }
                                                            else std::unreachable();

                                                            if(parent_inner_loop.child_left == std::get<1>(bnode_to_have_key_erased.keys))
                                                            {
                                                                parent_inner_loop.child_left = parent;
                                                                if(parent->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent && std::get<1>(bnode_to_have_key_erased.keys)->role() != rb2p_node_role_e::child_left_leftmost_descendent_of_root)
                                                                    child_left_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::ancestor>(std::get<1>(bnode_to_have_key_erased.keys));
                                                            }
                                                            else if(parent_inner_loop.rightmost_descendent_of_child_left == std::get<1>(bnode_to_have_key_erased.keys))
                                                            {
                                                                parent_inner_loop.rightmost_descendent_of_child_left = parent;
                                                                child_left_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_to_have_key_erased.keys));
                                                            }
                                                            else std::unreachable();
                                                            parent_inner_loop.this_ = std::get<1>(bnode_right_sibling.keys);
                                                            parent_inner_loop.child_right = std::get<2>(bnode_right_sibling.keys).value();
                                                            parent_inner_loop.index = -1, parent_inner_loop.link();
                                                            if(child_left_outer_loop.this_ != nullptr)
                                                                child_left_outer_loop.exchange(parent);

                                                            if(parent->role() != rb2p_node_role_e::root)
                                                                parent_outer_loop.index = -1, parent_outer_loop.link();

                                                            std::get<2>(bnode_right_sibling.keys).value()->color() = std::get<1>(bnode_right_sibling.keys)->color();
                                                            std::get<1>(bnode_right_sibling.keys)->role() = parent->role();
                                                            std::get<1>(bnode_right_sibling.keys)->color() = parent->color();
                                                            parent->role() = std::get<1>(bnode_to_have_key_erased.keys)->role();
                                                            parent->color() = std::get<1>(bnode_to_have_key_erased.keys)->color();

                                                            refresh_node_count_and_accumulated_storage_and_above_until(schedules, accumulator, parent, std::get<1>(bnode_right_sibling.keys));
                                                            refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<2>(bnode_right_sibling.keys).value());
                                                        }
                                                        else if(std::get<2>(bnode_right_sibling.keys).has_value() && std::get<0>(bnode_right_sibling.keys).has_value())
                                                        {
                                                            assert(std::get<1>(bnode_right_sibling.keys)->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent || std::get<1>(bnode_right_sibling.keys)->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent);
                                                            if(parent->role() == rb2p_node_role_e::root)
                                                            {
                                                                assert(!should_link_loop_end);
                                                                assert(loop_end.root == parent);
                                                                loop_end.root = std::get<0>(bnode_right_sibling.keys).value();
                                                                if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root)
                                                                {
                                                                    assert(loop_end.leftmost_descendent_of_root == std::get<1>(bnode_to_have_key_erased.keys));
                                                                    loop_end.leftmost_descendent_of_root = parent;
                                                                }
                                                                should_link_loop_end = true;
                                                            }
                                                            else if(parent->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent)
                                                            {
                                                                assert(parent_outer_loop.child_right == parent);
                                                                if(parent_outer_loop.leftmost_descendent_of_child_right == std::get<1>(bnode_to_have_key_erased.keys))
                                                                    parent_outer_loop.leftmost_descendent_of_child_right = parent;
                                                                parent_outer_loop.child_right = std::get<0>(bnode_right_sibling.keys).value();
                                                            }
                                                            else if(parent->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent)
                                                            {
                                                                assert(parent_outer_loop.child_left == parent);
                                                                parent_outer_loop.child_left = std::get<0>(bnode_right_sibling.keys).value();
                                                                if(std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root)
                                                                {
                                                                    assert(!should_link_loop_end);
                                                                    assert(loop_end.leftmost_descendent_of_root == std::get<1>(bnode_to_have_key_erased.keys));
                                                                    loop_end.leftmost_descendent_of_root = parent;
                                                                    should_link_loop_end = true;
                                                                }
                                                            }
                                                            else std::unreachable();

                                                            if(parent_inner_loop.child_left == std::get<1>(bnode_to_have_key_erased.keys))
                                                            {
                                                                parent_inner_loop.child_left = parent;
                                                                if(parent->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent && std::get<1>(bnode_to_have_key_erased.keys)->role() != rb2p_node_role_e::child_left_leftmost_descendent_of_root)
                                                                    child_left_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::ancestor>(std::get<1>(bnode_to_have_key_erased.keys));
                                                            }
                                                            else if(parent_inner_loop.rightmost_descendent_of_child_left == std::get<1>(bnode_to_have_key_erased.keys))
                                                            {
                                                                parent_inner_loop.rightmost_descendent_of_child_left = parent;
                                                                child_left_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_to_have_key_erased.keys));
                                                            }
                                                            else std::unreachable();
                                                            parent_inner_loop.this_ = std::get<0>(bnode_right_sibling.keys).value();
                                                            assert(parent_inner_loop.leftmost_descendent_of_child_right == std::get<0>(bnode_right_sibling.keys).value());
                                                            if(parent_inner_loop.child_right == std::get<1>(bnode_right_sibling.keys))
                                                            {
                                                                parent_inner_loop.leftmost_descendent_of_child_right = nullptr;

                                                                typename navigator_t::loop_t{
                                                                    .this_ = std::get<1>(bnode_right_sibling.keys),
                                                                    .child_right = std::get<2>(bnode_right_sibling.keys).value(),
                                                                }
                                                                    .link();
                                                            }
                                                            else
                                                            {
                                                                parent_inner_loop.leftmost_descendent_of_child_right = std::get<1>(bnode_right_sibling.keys);

                                                                typename navigator_t::loop_t child_right_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_right_sibling.keys));
                                                                assert(child_right_outer_loop.child_left == std::get<1>(bnode_right_sibling.keys));
                                                                assert(child_right_outer_loop.rightmost_descendent_of_child_left == std::get<2>(bnode_right_sibling.keys).value());
                                                                child_right_outer_loop.child_left = std::get<2>(bnode_right_sibling.keys).value();
                                                                child_right_outer_loop.rightmost_descendent_of_child_left = nullptr;
                                                                child_right_outer_loop.index = -1, child_right_outer_loop.link();

                                                                typename navigator_t::loop_t{
                                                                    .this_ = std::get<2>(bnode_right_sibling.keys).value(),
                                                                    .child_left = std::get<1>(bnode_right_sibling.keys),
                                                                }
                                                                    .link();
                                                                std::get<2>(bnode_right_sibling.keys).value()->color() = false;
                                                                std::get<2>(bnode_right_sibling.keys).value()->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                                std::get<1>(bnode_right_sibling.keys)->color() = true;
                                                                std::get<1>(bnode_right_sibling.keys)->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                            }
                                                            parent_inner_loop.index = -1, parent_inner_loop.link();
                                                            if(child_left_outer_loop.this_ != nullptr)
                                                                child_left_outer_loop.exchange(parent);

                                                            if(parent->role() != rb2p_node_role_e::root)
                                                                parent_outer_loop.index = -1, parent_outer_loop.link();

                                                            std::get<0>(bnode_right_sibling.keys).value()->role() = parent->role();
                                                            std::get<0>(bnode_right_sibling.keys).value()->color() = parent->color();
                                                            parent->role() = std::get<1>(bnode_to_have_key_erased.keys)->role();
                                                            parent->color() = std::get<1>(bnode_to_have_key_erased.keys)->color();

                                                            refresh_node_count_and_accumulated_storage_and_above_until(schedules, accumulator, parent, std::get<0>(bnode_right_sibling.keys).value());
                                                            refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<1>(bnode_right_sibling.keys));
                                                        }
                                                        else std::unreachable();
                                                    }
                                                    else
                                                    {
                                                        if(std::get<0>(bnode_right_sibling.keys).has_value())
                                                        {
                                                            bool child_after_merge_is_child_left_of_parent = std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                            typename navigator_t::loop_t child_left_outer_loop;
                                                            if(!child_after_merge_is_child_left_of_parent)
                                                            {
                                                                child_left_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_to_have_key_erased.keys));
                                                                if(child_left_outer_loop.leftmost_descendent_of_child_right == node)
                                                                    child_left_outer_loop.leftmost_descendent_of_child_right = static_cast<navigator_except_node_end_t *>(rb2p_iterator_t<false, is_reversed, config_t>::template predecessor<true>(child_left_outer_loop.leftmost_descendent_of_child_right));
                                                            }

                                                            typename navigator_t::loop_t child_left_inner_loop{
                                                                .this_ = parent,
                                                                .rightmost_descendent_of_child_left = parent_inner_loop.rightmost_descendent_of_child_left,
                                                            };
                                                            if(child_left_inner_loop.rightmost_descendent_of_child_left == node)
                                                                child_left_inner_loop.rightmost_descendent_of_child_left = static_cast<navigator_except_node_end_t *>(rb2p_iterator_t<false, is_reversed, config_t>::template predecessor<false>(child_left_inner_loop.rightmost_descendent_of_child_left));

                                                            typename navigator_t::loop_t child_right_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<0>(bnode_right_sibling.keys).value());
                                                            typename navigator_t::loop_t child_right_inner_loop = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<0>(bnode_right_sibling.keys).value());
                                                            navigator_except_node_end_t *key_left_child_right = std::get<0>(bnode_right_sibling.keys).value()->child_right(), *key_left_child_right_child_right, *key_left_child_right_child_left;
                                                            navigator_except_node_end_t *key_left_child_left = std::get<0>(bnode_right_sibling.keys).value()->child_left(), *key_left_child_left_child_right, *key_left_child_left_child_left;
                                                            if(child_after_merge_height == 1)
                                                            {
                                                                key_left_child_right_child_right = key_left_child_right->child_right();
                                                                key_left_child_right_child_left = key_left_child_right->child_left();
                                                                key_left_child_left_child_right = key_left_child_left->child_right();
                                                                key_left_child_left_child_left = key_left_child_left->child_left();
                                                            }

                                                            if(child_after_merge_height == 1 && key_left_child_right_child_right != nullptr && key_left_child_right_child_left == nullptr)
                                                            {
                                                                navigator_except_node_end_t *node_right = key_left_child_right_child_right;
                                                                navigator_except_node_end_t *node_left = key_left_child_right;
                                                                typename navigator_t::loop_t{
                                                                    .this_ = node_right,
                                                                    .child_left = node_left,
                                                                }
                                                                    .link();
                                                                node_right->color() = false;
                                                                node_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                                node_left->color() = true;
                                                                node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                                std::tie(child_right_inner_loop.child_right, child_right_inner_loop.leftmost_descendent_of_child_right) = std::make_tuple(node_right, node_left);
                                                                std::tie(child_right_outer_loop.child_left, child_right_outer_loop.rightmost_descendent_of_child_left) = std::make_tuple(node_right, nullptr);
                                                                move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                                            }
                                                            else
                                                            {
                                                                if(child_after_merge_height == 1 && key_left_child_right_child_right == nullptr && key_left_child_right_child_left == nullptr)
                                                                    key_left_child_right->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                                else
                                                                    key_left_child_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                                child_right_outer_loop.child_left = child_right_inner_loop.child_right;
                                                            }

                                                            if(child_after_merge_height == 1 && key_left_child_left_child_right == nullptr && key_left_child_left_child_left != nullptr)
                                                            {
                                                                navigator_except_node_end_t *node_right = key_left_child_left;
                                                                navigator_except_node_end_t *node_left = key_left_child_left_child_left;
                                                                typename navigator_t::loop_t{
                                                                    .this_ = node_left,
                                                                    .child_right = node_right,
                                                                }
                                                                    .link();
                                                                node_left->color() = false;
                                                                node_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                                node_right->color() = true;
                                                                node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                                std::tie(child_right_inner_loop.child_left, child_right_inner_loop.rightmost_descendent_of_child_left) = std::make_tuple(node_left, node_right);
                                                                std::tie(child_left_inner_loop.child_right, child_left_inner_loop.leftmost_descendent_of_child_right) = std::make_tuple(node_left, nullptr);
                                                                move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                                            }
                                                            else
                                                            {
                                                                if(child_after_merge_height == 1 && key_left_child_left_child_right == nullptr && key_left_child_left_child_left == nullptr)
                                                                    key_left_child_left->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                                else
                                                                    key_left_child_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                                std::tie(child_left_inner_loop.child_right, child_left_inner_loop.leftmost_descendent_of_child_right) = std::make_tuple(child_right_inner_loop.child_left, parent_inner_loop.leftmost_descendent_of_child_right);
                                                            }

                                                            parent_inner_loop.this_ = std::get<0>(bnode_right_sibling.keys).value();
                                                            parent_inner_loop.leftmost_descendent_of_child_right = child_right_inner_loop.leftmost_descendent_of_child_right != nullptr ? child_right_inner_loop.leftmost_descendent_of_child_right : child_right_inner_loop.child_right;
                                                            parent_inner_loop.rightmost_descendent_of_child_left = child_right_inner_loop.rightmost_descendent_of_child_left != nullptr ? child_right_inner_loop.rightmost_descendent_of_child_left : child_right_inner_loop.child_left;

                                                            if(parent->role() == rb2p_node_role_e::root)
                                                            {
                                                                assert(loop_end.root == parent);
                                                                loop_end.root = std::get<0>(bnode_right_sibling.keys).value();
                                                                should_link_loop_end = true;
                                                            }
                                                            else if(parent->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent)
                                                            {
                                                                assert(parent_outer_loop.child_right == parent);
                                                                parent_outer_loop.child_right = std::get<0>(bnode_right_sibling.keys).value();
                                                                if(parent_outer_loop.leftmost_descendent_of_child_right == node)
                                                                    parent_outer_loop.leftmost_descendent_of_child_right = static_cast<navigator_except_node_end_t *>(rb2p_iterator_t<false, is_reversed, config_t>::template predecessor<true>(parent_outer_loop.leftmost_descendent_of_child_right));
                                                            }
                                                            else if(parent->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent)
                                                            {
                                                                assert(parent_outer_loop.child_left == parent);
                                                                parent_outer_loop.child_left = std::get<0>(bnode_right_sibling.keys).value();
                                                            }
                                                            else std::unreachable();

                                                            rb2p_node_role_e child_after_merge_role = std::get<1>(bnode_to_have_key_erased.keys)->role();
                                                            navigator_except_node_end_t *child_after_merge = merge_and_return_child_after_merge(rb2p_node_role_e::child_left_not_a_leftmost_descendent);

                                                            if(!child_after_merge_is_child_left_of_parent)
                                                            {
                                                                child_left_outer_loop.child_right = parent;
                                                                child_left_outer_loop.index = -1, child_left_outer_loop.link();
                                                            }
                                                            else
                                                                parent_inner_loop.child_left = parent;
                                                            child_left_inner_loop.child_left = child_after_merge;
                                                            child_left_inner_loop.link();
                                                            parent_inner_loop.index = -1, parent_inner_loop.link();
                                                            child_right_outer_loop.index = -1, child_right_outer_loop.link();
                                                            if(parent->role() != rb2p_node_role_e::root)
                                                                parent_outer_loop.index = -1, parent_outer_loop.link();

                                                            std::get<0>(bnode_right_sibling.keys).value()->role() = parent->role();
                                                            std::get<0>(bnode_right_sibling.keys).value()->color() = parent->color();
                                                            parent->role() = child_after_merge_role;
                                                            parent->color() = false;

                                                            refresh_node_count_and_accumulated_storage_and_above_until(schedules, accumulator, parent, std::get<0>(bnode_right_sibling.keys).value());
                                                            refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<1>(bnode_right_sibling.keys));
                                                        }
                                                        else if(!std::get<0>(bnode_right_sibling.keys).has_value())
                                                        {
                                                            assert(std::get<2>(bnode_right_sibling.keys).has_value());

                                                            bool child_after_merge_is_child_left_of_parent = std::get<1>(bnode_to_have_key_erased.keys)->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                            typename navigator_t::loop_t child_left_outer_loop;
                                                            if(!child_after_merge_is_child_left_of_parent)
                                                            {
                                                                child_left_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_to_have_key_erased.keys));
                                                                if(child_left_outer_loop.leftmost_descendent_of_child_right == node)
                                                                    child_left_outer_loop.leftmost_descendent_of_child_right = static_cast<navigator_except_node_end_t *>(rb2p_iterator_t<false, is_reversed, config_t>::template predecessor<true>(child_left_outer_loop.leftmost_descendent_of_child_right));
                                                            }

                                                            typename navigator_t::loop_t child_left_inner_loop{
                                                                .this_ = parent,
                                                                .rightmost_descendent_of_child_left = parent_inner_loop.rightmost_descendent_of_child_left,
                                                            };
                                                            if(child_left_inner_loop.rightmost_descendent_of_child_left == node)
                                                                child_left_inner_loop.rightmost_descendent_of_child_left = static_cast<navigator_except_node_end_t *>(rb2p_iterator_t<false, is_reversed, config_t>::template predecessor<false>(child_left_inner_loop.rightmost_descendent_of_child_left));

                                                            typename navigator_t::loop_t child_right_outer_loop;
                                                            typename navigator_t::loop_t child_right_inner_loop = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<1>(bnode_right_sibling.keys));
                                                            navigator_except_node_end_t *key_left_child_left = std::get<1>(bnode_right_sibling.keys)->child_left(), *key_left_child_left_child_right, *key_left_child_left_child_left;
                                                            if(child_after_merge_height == 1)
                                                            {
                                                                key_left_child_left_child_right = key_left_child_left->child_right();
                                                                key_left_child_left_child_left = key_left_child_left->child_left();
                                                            }

                                                            if(std::get<1>(bnode_right_sibling.keys)->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent)
                                                            {
                                                                assert(parent_inner_loop.child_right == std::get<1>(bnode_right_sibling.keys));
                                                                parent_inner_loop.child_right = std::get<2>(bnode_right_sibling.keys).value();
                                                            }
                                                            else if(std::get<1>(bnode_right_sibling.keys)->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent)
                                                            {
                                                                child_right_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_right_sibling.keys));
                                                                assert(child_right_outer_loop.child_left == std::get<1>(bnode_right_sibling.keys));
                                                                child_right_outer_loop.child_left = std::get<2>(bnode_right_sibling.keys).value();
                                                            }
                                                            else std::unreachable();

                                                            if(child_after_merge_height == 1 && key_left_child_left_child_right == nullptr && key_left_child_left_child_left != nullptr)
                                                            {
                                                                navigator_except_node_end_t *node_right = key_left_child_left;
                                                                navigator_except_node_end_t *node_left = key_left_child_left_child_left;
                                                                typename navigator_t::loop_t{
                                                                    .this_ = node_left,
                                                                    .child_right = node_right,
                                                                }
                                                                    .link();
                                                                node_left->color() = false;
                                                                node_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                                node_right->color() = true;
                                                                node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                                std::tie(child_right_inner_loop.child_left, child_right_inner_loop.rightmost_descendent_of_child_left) = std::make_tuple(node_left, node_right);
                                                                std::tie(child_left_inner_loop.child_right, child_left_inner_loop.leftmost_descendent_of_child_right) = std::make_tuple(node_left, nullptr);
                                                                move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                                            }
                                                            else
                                                            {
                                                                if(child_after_merge_height == 1 && key_left_child_left_child_right == nullptr && key_left_child_left_child_left == nullptr)
                                                                    key_left_child_left->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                                else
                                                                    key_left_child_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                                std::tie(child_left_inner_loop.child_right, child_left_inner_loop.leftmost_descendent_of_child_right) = std::make_tuple(child_right_inner_loop.child_left, parent_inner_loop.leftmost_descendent_of_child_right);
                                                            }

                                                            parent_inner_loop.this_ = std::get<1>(bnode_right_sibling.keys);
                                                            parent_inner_loop.leftmost_descendent_of_child_right = child_right_inner_loop.leftmost_descendent_of_child_right != nullptr ? child_right_inner_loop.leftmost_descendent_of_child_right : child_right_inner_loop.child_right;
                                                            parent_inner_loop.rightmost_descendent_of_child_left = child_right_inner_loop.rightmost_descendent_of_child_left != nullptr ? child_right_inner_loop.rightmost_descendent_of_child_left : child_right_inner_loop.child_left;

                                                            if(parent->role() == rb2p_node_role_e::root)
                                                            {
                                                                assert(loop_end.root == parent);
                                                                loop_end.root = std::get<1>(bnode_right_sibling.keys);
                                                                should_link_loop_end = true;
                                                            }
                                                            else if(parent->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent)
                                                            {
                                                                assert(parent_outer_loop.child_right == parent);
                                                                parent_outer_loop.child_right = std::get<1>(bnode_right_sibling.keys);
                                                                if(parent_outer_loop.leftmost_descendent_of_child_right == node)
                                                                    parent_outer_loop.leftmost_descendent_of_child_right = static_cast<navigator_except_node_end_t *>(rb2p_iterator_t<false, is_reversed, config_t>::template predecessor<true>(parent_outer_loop.leftmost_descendent_of_child_right));
                                                            }
                                                            else if(parent->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent)
                                                            {
                                                                assert(parent_outer_loop.child_left == parent);
                                                                parent_outer_loop.child_left = std::get<1>(bnode_right_sibling.keys);
                                                            }
                                                            else std::unreachable();

                                                            rb2p_node_role_e child_after_merge_role = std::get<1>(bnode_to_have_key_erased.keys)->role();
                                                            navigator_except_node_end_t *child_after_merge = merge_and_return_child_after_merge(rb2p_node_role_e::child_left_not_a_leftmost_descendent);

                                                            if(!child_after_merge_is_child_left_of_parent)
                                                            {
                                                                child_left_outer_loop.child_right = parent;
                                                                child_left_outer_loop.index = -1, child_left_outer_loop.link();
                                                            }
                                                            else
                                                                parent_inner_loop.child_left = parent;
                                                            child_left_inner_loop.child_left = child_after_merge;
                                                            child_left_inner_loop.link();
                                                            parent_inner_loop.index = -1, parent_inner_loop.link();
                                                            if(child_right_outer_loop.this_ != nullptr)
                                                                child_right_outer_loop.index = -1, child_right_outer_loop.link();
                                                            if(parent->role() != rb2p_node_role_e::root)
                                                                parent_outer_loop.index = -1, parent_outer_loop.link();

                                                            std::get<2>(bnode_right_sibling.keys).value()->role() = std::get<1>(bnode_right_sibling.keys)->role();
                                                            std::get<2>(bnode_right_sibling.keys).value()->color() = std::get<1>(bnode_right_sibling.keys)->color();
                                                            std::get<1>(bnode_right_sibling.keys)->role() = parent->role();
                                                            std::get<1>(bnode_right_sibling.keys)->color() = parent->color();
                                                            parent->role() = child_after_merge_role;
                                                            parent->color() = false;

                                                            refresh_node_count_and_accumulated_storage_and_above_until(schedules, accumulator, parent, std::get<1>(bnode_right_sibling.keys));
                                                            refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<2>(bnode_right_sibling.keys).value());
                                                        }
                                                        else std::unreachable();
                                                    }
                                                }
                                                else
                                                    fallback();
                                            }
                                            else
                                                fallback();
                                        };
                                    };
                                    auto try_merge_with_left_sibling = [&](auto fallback)
                                    {
                                        return [&, fallback]()
                                        {
                                            if(opt_node_left_sibling_black.has_value())
                                            {
                                                navigator_except_node_end_t *key_from_parent = bnode_parent.key_at(bnode_parent.child_index - 1);
                                                navigator_except_node_end_t *key_left = opt_node_left_sibling_black.value();
                                                navigator_except_node_end_t *key_right = std::get<1>(bnode_to_have_key_erased.keys);

                                                bnode_erase_t bnode_to_have_key_erased_next;
                                                static_cast<bnode_t &>(bnode_to_have_key_erased_next) = static_cast<bnode_t &>(bnode_parent);
                                                bnode_to_have_key_erased_next.key_to_be_erased_index = bnode_parent.child_index - 1;

                                                if(child_after_merge_height == 0)
                                                {
                                                    assert(key_left->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root || key_left->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_non_root || key_left->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_non_root);
                                                    assert(key_right->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root || key_right->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_non_root || key_right->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_non_root);
                                                    typename navigator_t::loop_t key_left_outer_loop;
                                                    if(key_left->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_non_root)
                                                        key_left_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::ancestor>(key_left);
                                                    typename navigator_t::loop_t key_right_outer_loop;
                                                    if(key_right->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_non_root)
                                                        key_right_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::ancestor>(key_right);

                                                    this_ /*erase_impl*/ (
                                                        this_, bnode_to_have_key_erased_next, [&schedules, &accumulator, &should_link_loop_end, &loop_end, key_left_outer_loop, key_left, key_from_parent, key_right, key_right_outer_loop](rb2p_node_role_e child_after_merge_role) mutable -> navigator_except_node_end_t *
                                                        {
                                                            if(child_after_merge_role == rb2p_node_role_e::root)
                                                            {
                                                                assert(key_left->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root);
                                                                assert(key_from_parent->role() == rb2p_node_role_e::root);
                                                                assert(key_right->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root);

                                                                assert(!should_link_loop_end);
                                                                assert(loop_end.leftmost_descendent_of_root == key_left);
                                                                assert(loop_end.root == key_from_parent);
                                                                assert(loop_end.rightmost_descendent_of_root == key_right);
                                                                loop_end.rightmost_descendent_of_root = nullptr;
                                                                should_link_loop_end = true;

                                                                typename navigator_t::loop_t{
                                                                    .this_ = key_from_parent,
                                                                    .child_left = key_left,
                                                                }
                                                                    .link();

                                                                assert(key_from_parent->color() == false);
                                                                key_left->color() = true;

                                                                refresh_node_count_and_accumulated_storage(schedules,accumulator,key_from_parent);

                                                                return key_from_parent;
                                                            }
                                                            else if(child_after_merge_role == rb2p_node_role_e::child_left_not_a_leftmost_descendent)
                                                            {
                                                                typename navigator_t::loop_t{
                                                                    .this_ = key_from_parent,
                                                                    .child_left = key_left,
                                                                }
                                                                    .link();

                                                                key_from_parent->role() = child_after_merge_role;
                                                                key_from_parent->color() = false;
                                                                if(key_left->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_non_root)
                                                                    key_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                                key_left->color() = true;

                                                                refresh_node_count_and_accumulated_storage(schedules,accumulator,key_from_parent);

                                                                return key_from_parent;
                                                            }
                                                            else if(child_after_merge_role == rb2p_node_role_e::child_right_not_a_rightmost_descendent)
                                                            {
                                                                if(key_right->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root)
                                                                {
                                                                    // assert(!should_link_loop_end);
                                                                    assert(loop_end.rightmost_descendent_of_root == key_right);
                                                                    loop_end.rightmost_descendent_of_root = key_from_parent;
                                                                    should_link_loop_end = true;
                                                                }
                                                                else if(key_right->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_non_root)
                                                                {
                                                                    assert(key_right_outer_loop.rightmost_descendent_of_child_left == key_right);
                                                                    if(key_right_outer_loop.child_left == key_from_parent)
                                                                        key_right_outer_loop.child_left = key_left;
                                                                    key_right_outer_loop.rightmost_descendent_of_child_left = key_from_parent;
                                                                    key_right_outer_loop.index = -1, key_right_outer_loop.link();
                                                                }

                                                                typename navigator_t::loop_t{
                                                                    .this_ = key_left,
                                                                    .child_right = key_from_parent,
                                                                }
                                                                    .link();

                                                                key_left->role() = child_after_merge_role;
                                                                key_left->color() = false;
                                                                key_from_parent->role() = key_right->role();
                                                                if(key_from_parent->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_non_root)
                                                                    key_from_parent->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                                key_from_parent->color() = true;

                                                                refresh_node_count_and_accumulated_storage(schedules,accumulator,key_from_parent);
                                                                refresh_node_count_and_accumulated_storage(schedules,accumulator,key_left);

                                                                return key_left;
                                                            }
                                                            else std::unreachable(); },
                                                        child_after_merge_height + 1);
                                                }
                                                else
                                                {
                                                    typename navigator_t::loop_t parent_inner_loop = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(key_from_parent);
                                                    if(parent_inner_loop.leftmost_descendent_of_child_right == node)
                                                        parent_inner_loop.leftmost_descendent_of_child_right = static_cast<navigator_except_node_end_t *>(rb2p_iterator_t<false, is_reversed, config_t>::template predecessor<true>(parent_inner_loop.leftmost_descendent_of_child_right));
                                                    parent_inner_loop.child_left = key_left;

                                                    this_ /*erase_impl*/ (
                                                        this_, bnode_to_have_key_erased_next, [&schedules, &accumulator, &merge_and_return_child_after_merge, key_left, key_from_parent, key_right, parent_inner_loop](rb2p_node_role_e child_after_merge_role) mutable -> navigator_except_node_end_t *
                                                        {
                                                            navigator_except_node_end_t *child_after_merge = merge_and_return_child_after_merge(rb2p_node_role_e::child_right_not_a_rightmost_descendent);

                                                            parent_inner_loop.child_right = child_after_merge;
                                                            parent_inner_loop.index = -1, parent_inner_loop.link();

                                                            key_from_parent->role() = child_after_merge_role;
                                                            key_from_parent->color() = false;
                                                            key_left->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                            key_left->color() = true;

                                                            refresh_node_count_and_accumulated_storage(schedules,accumulator,key_from_parent);

                                                            return key_from_parent; },
                                                        child_after_merge_height + 1);
                                                }
                                            }
                                            else fallback();
                                        };
                                    };
                                    auto try_merge_with_right_sibling = [&](auto fallback)
                                    {
                                        return [&, fallback]()
                                        {
                                            if(opt_node_right_sibling_black.has_value())
                                            {
                                                navigator_except_node_end_t *key_from_parent = bnode_parent.key_at(bnode_parent.child_index + 1);
                                                navigator_except_node_end_t *key_right = opt_node_right_sibling_black.value();
                                                navigator_except_node_end_t *key_left = std::get<1>(bnode_to_have_key_erased.keys);

                                                bnode_erase_t bnode_to_have_key_erased_next;
                                                static_cast<bnode_t &>(bnode_to_have_key_erased_next) = static_cast<bnode_t &>(bnode_parent);
                                                bnode_to_have_key_erased_next.key_to_be_erased_index = bnode_parent.child_index + 1;

                                                if(child_after_merge_height == 0)
                                                {
                                                    assert(key_right->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root || key_right->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_non_root || key_right->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_non_root);
                                                    assert(key_left->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root || key_left->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_non_root || key_left->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_non_root);
                                                    typename navigator_t::loop_t key_right_outer_loop;
                                                    if(key_right->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_non_root)
                                                        key_right_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::ancestor>(key_right);
                                                    typename navigator_t::loop_t key_left_outer_loop;
                                                    if(key_left->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_non_root)
                                                        key_left_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::ancestor>(key_left);

                                                    this_ /*erase_impl*/ (
                                                        this_, bnode_to_have_key_erased_next, [&schedules, &accumulator, &should_link_loop_end, &loop_end, key_right_outer_loop, key_right, key_from_parent, key_left, key_left_outer_loop](rb2p_node_role_e child_after_merge_role) mutable -> navigator_except_node_end_t *
                                                        {
                                                            if(child_after_merge_role == rb2p_node_role_e::root)
                                                            {
                                                                assert(key_right->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root);
                                                                assert(key_from_parent->role() == rb2p_node_role_e::root);
                                                                assert(key_left->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root);

                                                                typename navigator_t::loop_t{
                                                                    .this_ = key_from_parent,
                                                                    .child_right = key_right,
                                                                }
                                                                    .link();

                                                                assert(!should_link_loop_end);
                                                                assert(loop_end.rightmost_descendent_of_root == key_right);
                                                                assert(loop_end.root == key_from_parent);
                                                                assert(loop_end.leftmost_descendent_of_root == key_left);
                                                                loop_end.leftmost_descendent_of_root = nullptr;
                                                                should_link_loop_end = true;

                                                                assert(key_from_parent->color() == false);
                                                                key_right->color() = true;

                                                                refresh_node_count_and_accumulated_storage(schedules,accumulator,key_from_parent);

                                                                return key_from_parent;
                                                            }
                                                            else if(child_after_merge_role == rb2p_node_role_e::child_right_not_a_rightmost_descendent)
                                                            {
                                                                typename navigator_t::loop_t{
                                                                    .this_ = key_from_parent,
                                                                    .child_right = key_right,
                                                                }
                                                                    .link();

                                                                key_from_parent->role() = child_after_merge_role;
                                                                key_from_parent->color() = false;
                                                                if(key_right->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_non_root)
                                                                    key_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                                key_right->color() = true;

                                                                refresh_node_count_and_accumulated_storage(schedules,accumulator,key_from_parent);

                                                                return key_from_parent;
                                                            }
                                                            else if(child_after_merge_role == rb2p_node_role_e::child_left_not_a_leftmost_descendent)
                                                            {
                                                                if(key_left->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root)
                                                                {
                                                                    // assert(!should_link_loop_end);
                                                                    assert(loop_end.leftmost_descendent_of_root == key_left);
                                                                    loop_end.leftmost_descendent_of_root = key_from_parent;
                                                                    should_link_loop_end = true;
                                                                }
                                                                else if(key_left->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_non_root)
                                                                {
                                                                    assert(key_left_outer_loop.leftmost_descendent_of_child_right == key_left);
                                                                    if(key_left_outer_loop.child_right == key_from_parent)
                                                                        key_left_outer_loop.child_right = key_right;
                                                                    key_left_outer_loop.leftmost_descendent_of_child_right = key_from_parent;
                                                                    key_left_outer_loop.index = -1, key_left_outer_loop.link();
                                                                }

                                                                typename navigator_t::loop_t{
                                                                    .this_ = key_right,
                                                                    .child_left = key_from_parent,
                                                                }
                                                                    .link();

                                                                key_right->role() = child_after_merge_role;
                                                                key_right->color() = false;
                                                                key_from_parent->role() = key_left->role();
                                                                if(key_from_parent->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_non_root)
                                                                    key_from_parent->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                                key_from_parent->color() = true;

                                                                refresh_node_count_and_accumulated_storage(schedules,accumulator,key_from_parent);
                                                                refresh_node_count_and_accumulated_storage(schedules,accumulator,key_right);

                                                                return key_right;
                                                            }
                                                            else std::unreachable(); },
                                                        child_after_merge_height + 1);
                                                }
                                                else
                                                {
                                                    typename navigator_t::loop_t parent_inner_loop = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(key_from_parent);
                                                    if(parent_inner_loop.rightmost_descendent_of_child_left == node)
                                                        parent_inner_loop.rightmost_descendent_of_child_left = static_cast<navigator_except_node_end_t *>(rb2p_iterator_t<false, is_reversed, config_t>::template predecessor<false>(parent_inner_loop.rightmost_descendent_of_child_left));
                                                    parent_inner_loop.child_right = key_right;

                                                    this_ /*erase_impl*/ (
                                                        this_, bnode_to_have_key_erased_next, [&schedules, &accumulator, &merge_and_return_child_after_merge, key_right, key_from_parent, key_left, parent_inner_loop](rb2p_node_role_e child_after_merge_role) mutable -> navigator_except_node_end_t *
                                                        {
                                                            navigator_except_node_end_t *child_after_merge = merge_and_return_child_after_merge(rb2p_node_role_e::child_left_not_a_leftmost_descendent);

                                                            parent_inner_loop.child_left = child_after_merge;
                                                            parent_inner_loop.index = -1, parent_inner_loop.link();

                                                            key_from_parent->role() = child_after_merge_role;
                                                            key_from_parent->color() = false;
                                                            key_right->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                            key_right->color() = true;

                                                            refresh_node_count_and_accumulated_storage(schedules,accumulator,key_from_parent);

                                                            return key_from_parent; },
                                                        child_after_merge_height + 1);
                                                }
                                            }
                                            else fallback();
                                        };
                                    };
                                    try_grab_from_left_sibling(try_grab_from_right_sibling(try_merge_with_left_sibling(try_merge_with_right_sibling([]()
                                        { std::unreachable(); }))))();
                                }
                            }
                        };
                        erase_impl(
                            erase_impl, bnode_erase_t::erasing_get_bnode(node), []([[maybe_unused]] rb2p_node_role_e child_after_merge_role) -> navigator_except_node_end_t *
                            { return nullptr; },
                            0);
                        if(should_link_loop_end)
                            loop_end.index = -1, loop_end.link();
                    }
                    return height_changed;
                }

                static bool insert_impl(schedules_t &schedules, [[maybe_unused]] node_end_t *node_end, bnode_up_t bnode_before_split, navigator_except_node_end_t *child_left_after_split, navigator_except_node_end_t *child_left_after_split_rightmost_descendent, navigator_except_node_end_t *child_after_split, navigator_except_node_end_t *child_right_after_split_leftmost_descendent, navigator_except_node_end_t *child_right_after_split, std::size_t child_left_or_right_after_split_height, bool &should_link_loop_end, typename navigator_t::loop_end_t &loop_end)
                {
                    accumulator_t const &accumulator = node_end->accumulator;
                    bool height_changed;
                    while(true)
                    {
                        if(bnode_before_split.key_count != 3)
                        {
                            if(bnode_before_split.child_index == -3)
                            {
                                rb2p_node_role_e role_right;
                                if(child_left_or_right_after_split_height == 0)
                                {
                                    if(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::root)
                                    {
                                        assert(std::get<0>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root);
                                        assert(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::root);

                                        loop_end.root = std::get<0>(bnode_before_split.keys).value();
                                        loop_end.rightmost_descendent_of_root = std::get<1>(bnode_before_split.keys);
                                        assert(should_link_loop_end);

                                        role_right = rb2p_node_role_e::child_right_rightmost_descendent_of_root;
                                    }
                                    else
                                    {
                                        assert(std::get<0>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_non_root || std::get<0>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root);
                                        assert(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent);

                                        if(std::get<0>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_non_root)
                                        {
                                            typename navigator_t::loop_t loop_outer_left = navigator_t::template get_loop<navigator_t::loop_type_e::ancestor>(std::get<0>(bnode_before_split.keys).value());
                                            loop_outer_left.exchange(child_after_split);
                                        }

                                        typename navigator_t::loop_t loop_outer_right = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_before_split.keys));
                                        loop_outer_right.rightmost_descendent_of_child_left = std::get<1>(bnode_before_split.keys);
                                        loop_outer_right.child_left = std::get<0>(bnode_before_split.keys).value();
                                        loop_outer_right.index = -1, loop_outer_right.link();

                                        role_right = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                    }
                                    typename navigator_t::loop_t{
                                        .this_ = std::get<0>(bnode_before_split.keys).value(),
                                        .child_right = std::get<1>(bnode_before_split.keys),
                                        .child_left = child_after_split,
                                    }
                                        .link();
                                }
                                else
                                {
                                    assert(std::get<0>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent);
                                    assert(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::root || std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent || std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent);
                                    typename navigator_t::loop_t loop_outer_right_or_left;
                                    if(!bnode_before_split.center_key_parent_info.is_end())
                                        loop_outer_right_or_left = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_before_split.keys));
                                    typename navigator_t::loop_t loop_inner_left = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<0>(bnode_before_split.keys).value());
                                    typename navigator_t::loop_t loop_inner_right = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<0>(bnode_before_split.keys).value());
                                    if(!bnode_before_split.center_key_parent_info.is_end())
                                        loop_outer_right_or_left.exchange(std::get<0>(bnode_before_split.keys).value());
                                    else
                                    {
                                        loop_end.root = std::get<0>(bnode_before_split.keys).value();
                                        should_link_loop_end = true;
                                    }
                                    if(child_left_or_right_after_split_height == 1 && std::get<0>(bnode_before_split.keys).value()->child_right()->child_left() == nullptr && std::get<0>(bnode_before_split.keys).value()->child_right()->child_right() != nullptr)
                                    {
                                        navigator_except_node_end_t *node_left = std::get<0>(bnode_before_split.keys).value()->child_right();
                                        navigator_except_node_end_t *node_right = node_left->child_right();
                                        typename navigator_t::loop_t{
                                            .this_ = node_right,
                                            .child_left = node_left,
                                        }
                                            .link();
                                        loop_inner_right.rightmost_descendent_of_child_left = nullptr;
                                        loop_inner_right.child_left = node_right;
                                        loop_inner_right.index = -1, loop_inner_right.link();
                                        loop_inner_left.child_right = std::get<1>(bnode_before_split.keys);
                                        loop_inner_left.leftmost_descendent_of_child_right = node_left;
                                        if(loop_inner_left.rightmost_descendent_of_child_left == nullptr)
                                            loop_inner_left.rightmost_descendent_of_child_left = child_right_after_split;
                                        loop_inner_left.child_left = child_after_split;
                                        loop_inner_left.index = -1, loop_inner_left.link();
                                        node_right->color() = false;
                                        node_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                        node_left->color() = true;
                                        node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                        move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                    }
                                    else
                                    {
                                        if(child_left_or_right_after_split_height == 1 && std::get<0>(bnode_before_split.keys).value()->child_right()->child_left() == nullptr && std::get<0>(bnode_before_split.keys).value()->child_right()->child_right() == nullptr)
                                        {
                                            assert(loop_inner_left.child_right == std::get<0>(bnode_before_split.keys).value()->child_right());
                                            assert(loop_inner_left.leftmost_descendent_of_child_right == nullptr);
                                            loop_inner_left.leftmost_descendent_of_child_right = std::get<0>(bnode_before_split.keys).value()->child_right();
                                            std::get<0>(bnode_before_split.keys).value()->child_right()->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                        }
                                        else
                                        {
                                            std::get<0>(bnode_before_split.keys).value()->child_right()->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                        }
                                        loop_inner_right.exchange(loop_inner_left.child_right);
                                        loop_inner_left.child_right = std::get<1>(bnode_before_split.keys);
                                        if(loop_inner_left.rightmost_descendent_of_child_left == nullptr)
                                            loop_inner_left.rightmost_descendent_of_child_left = child_right_after_split;
                                        loop_inner_left.child_left = child_after_split;
                                        loop_inner_left.index = -1, loop_inner_left.link();
                                    }

                                    role_right = rb2p_node_role_e::child_right_not_a_rightmost_descendent;

                                    typename navigator_t::loop_t{
                                        .this_ = child_after_split,
                                        .child_right = child_right_after_split,
                                        .leftmost_descendent_of_child_right = child_right_after_split_leftmost_descendent,
                                        .rightmost_descendent_of_child_left = child_left_after_split_rightmost_descendent,
                                        .child_left = child_left_after_split,
                                    }
                                        .link();
                                }
                                child_after_split->color() = true;
                                child_after_split->role() = std::get<0>(bnode_before_split.keys).value()->role();
                                std::get<0>(bnode_before_split.keys).value()->color() = false;
                                std::get<0>(bnode_before_split.keys).value()->role() = std::get<1>(bnode_before_split.keys)->role();
                                std::get<1>(bnode_before_split.keys)->color() = true;
                                std::get<1>(bnode_before_split.keys)->role() = role_right;
                                if(child_left_or_right_after_split_height == 1)
                                {
                                    if(child_left_after_split->role() != rb2p_node_role_e::child_left_leftmost_descendent_of_root)
                                        child_left_after_split->role() = child_left_after_split->child_left() == nullptr ? rb2p_node_role_e::child_left_leftmost_descendent_of_non_root : rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                    child_right_after_split->role() = child_right_after_split->child_right() == nullptr ? rb2p_node_role_e::child_right_rightmost_descendent_of_non_root : rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                }
                                else if(child_left_or_right_after_split_height > 1)
                                {
                                    child_left_after_split->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                    child_right_after_split->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                }

                                refresh_node_count_and_accumulated_storage(schedules, accumulator, child_after_split);
                                refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<0>(bnode_before_split.keys).value());
                            }
                            else if(bnode_before_split.child_index == -1)
                            {
                                if(!std::get<0>(bnode_before_split.keys).has_value())
                                {
                                    if(child_left_or_right_after_split_height == 0)
                                    {
                                        if(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::root)
                                        {
                                            typename navigator_t::loop_t{
                                                .this_ = std::get<1>(bnode_before_split.keys),
                                                .child_right = std::get<2>(bnode_before_split.keys).value_or(nullptr),
                                                .child_left = child_after_split,
                                            }
                                                .link();
                                            child_after_split->color() = true;
                                            child_after_split->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_root;

                                            refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, child_after_split);
                                        }
                                        else
                                        {
                                            switch(std::get<1>(bnode_before_split.keys)->role())
                                            {
                                            case rb2p_node_role_e::child_left_leftmost_descendent_of_non_root:
                                            case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                                                if(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_non_root)
                                                {
                                                    typename navigator_t::loop_t loop_outer_left = navigator_t::template get_loop<navigator_t::loop_type_e::ancestor>(std::get<1>(bnode_before_split.keys));
                                                    loop_outer_left.exchange(child_after_split);
                                                }
                                                typename navigator_t::loop_t{
                                                    .this_ = std::get<1>(bnode_before_split.keys),
                                                    .child_left = child_after_split,
                                                }
                                                    .link();
                                                child_after_split->color() = true;
                                                child_after_split->role() = std::get<1>(bnode_before_split.keys)->role();
                                                std::get<1>(bnode_before_split.keys)->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;

                                                refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, child_after_split);
                                                break;
                                            case rb2p_node_role_e::child_right_rightmost_descendent_of_non_root:
                                            case rb2p_node_role_e::child_right_rightmost_descendent_of_root:
                                            {
                                                typename navigator_t::loop_t loop_outer_left = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_before_split.keys));
                                                loop_outer_left.exchange(child_after_split);
                                            }
                                                typename navigator_t::loop_t{
                                                    .this_ = child_after_split,
                                                    .child_right = std::get<1>(bnode_before_split.keys),
                                                }
                                                    .link();
                                                child_after_split->color() = false;
                                                child_after_split->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                std::get<1>(bnode_before_split.keys)->color() = true;

                                                refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                                break;
                                            case rb2p_node_role_e::child_right_not_a_rightmost_descendent:
                                            {
                                                typename navigator_t::loop_t loop_outer_left = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_before_split.keys));
                                                loop_outer_left.leftmost_descendent_of_child_right = child_after_split;
                                                loop_outer_left.index = -1, loop_outer_left.link();
                                            }
                                                typename navigator_t::loop_t{
                                                    .this_ = std::get<1>(bnode_before_split.keys),
                                                    .child_right = std::get<2>(bnode_before_split.keys).value(),
                                                    .child_left = child_after_split,
                                                }
                                                    .link();
                                                child_after_split->color() = true;
                                                child_after_split->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;

                                                refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, child_after_split);
                                                break;
                                            default:
                                                std::unreachable();
                                                break;
                                            }
                                        }
                                    }
                                    else
                                    {
                                        assert(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::root || std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent || std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent);

                                        typename navigator_t::loop_t loop_inner = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<1>(bnode_before_split.keys));
                                        loop_inner.child_left = child_after_split;
                                        if(loop_inner.rightmost_descendent_of_child_left == nullptr)
                                            loop_inner.rightmost_descendent_of_child_left = child_right_after_split;
                                        loop_inner.index = -1, loop_inner.link();

                                        typename navigator_t::loop_t{
                                            .this_ = child_after_split,
                                            .child_right = child_right_after_split,
                                            .leftmost_descendent_of_child_right = child_right_after_split_leftmost_descendent,
                                            .rightmost_descendent_of_child_left = child_left_after_split_rightmost_descendent,
                                            .child_left = child_left_after_split,
                                        }
                                            .link();

                                        child_after_split->color() = true;
                                        child_after_split->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                        if(child_left_or_right_after_split_height == 1)
                                        {
                                            if(child_left_after_split->role() != rb2p_node_role_e::child_left_leftmost_descendent_of_root)
                                                child_left_after_split->role() = child_left_after_split->child_left() == nullptr ? rb2p_node_role_e::child_left_leftmost_descendent_of_non_root : rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                            child_right_after_split->role() = child_right_after_split->child_right() == nullptr ? rb2p_node_role_e::child_right_rightmost_descendent_of_non_root : rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                        }
                                        else if(child_left_or_right_after_split_height > 1)
                                        {
                                            child_left_after_split->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                            child_right_after_split->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                        }

                                        refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, child_after_split);
                                    }
                                }
                                else
                                {
                                    rb2p_node_role_e role_right;
                                    if(child_left_or_right_after_split_height == 0)
                                    {
                                        if(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::root)
                                        {
                                            assert(std::get<0>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root);
                                            assert(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::root);

                                            loop_end.root = child_after_split;
                                            loop_end.rightmost_descendent_of_root = std::get<1>(bnode_before_split.keys);
                                            assert(!should_link_loop_end);
                                            should_link_loop_end = true;

                                            role_right = rb2p_node_role_e::child_right_rightmost_descendent_of_root;
                                        }
                                        else
                                        {
                                            assert(std::get<0>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_non_root || std::get<0>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root);
                                            assert(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent);

                                            typename navigator_t::loop_t loop_outer_right = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_before_split.keys));
                                            loop_outer_right.rightmost_descendent_of_child_left = std::get<1>(bnode_before_split.keys);
                                            loop_outer_right.child_left = child_after_split;
                                            loop_outer_right.index = -1, loop_outer_right.link();

                                            role_right = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                        }
                                        typename navigator_t::loop_t{
                                            .this_ = child_after_split,
                                            .child_right = std::get<1>(bnode_before_split.keys),
                                            .child_left = std::get<0>(bnode_before_split.keys).value(),
                                        }
                                            .link();
                                    }
                                    else
                                    {
                                        assert(std::get<0>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent);
                                        assert(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::root || std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent || std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent);

                                        typename navigator_t::loop_t loop_outer_right_or_left;
                                        if(!bnode_before_split.center_key_parent_info.is_end())
                                            loop_outer_right_or_left = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_before_split.keys));
                                        typename navigator_t::loop_t loop_inner_left = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<0>(bnode_before_split.keys).value());
                                        typename navigator_t::loop_t loop_inner_right = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<0>(bnode_before_split.keys).value());
                                        if(!bnode_before_split.center_key_parent_info.is_end())
                                            loop_outer_right_or_left.exchange(child_after_split);
                                        else
                                        {
                                            loop_end.root = child_after_split;
                                            assert(!should_link_loop_end);
                                            should_link_loop_end = true;
                                        }
                                        if(child_left_or_right_after_split_height == 1 && child_right_after_split->child_right() != nullptr && child_right_after_split->child_left() == nullptr)
                                        {
                                            navigator_except_node_end_t *node_left = child_right_after_split;
                                            navigator_except_node_end_t *node_right = node_left->child_right();
                                            loop_inner_right.rightmost_descendent_of_child_left = nullptr;
                                            loop_inner_right.child_left = node_right;
                                            loop_inner_right.index = -1, loop_inner_right.link();
                                            typename navigator_t::loop_t{
                                                .this_ = node_right,
                                                .child_left = node_left,
                                            }
                                                .link();
                                            node_right->color() = false;
                                            node_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                            node_left->color() = true;
                                            node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                            std::tie(child_right_after_split, child_right_after_split_leftmost_descendent) = std::make_tuple(node_right, node_left);
                                            move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                        }
                                        else
                                            loop_inner_right.exchange(child_right_after_split);
                                        if(child_left_or_right_after_split_height == 1 && child_left_after_split->child_left() != nullptr && child_left_after_split->child_right() == nullptr)
                                        {
                                            navigator_except_node_end_t *node_right = child_left_after_split;
                                            navigator_except_node_end_t *node_left = node_right->child_left();
                                            loop_inner_left.child_right = node_left;
                                            loop_inner_left.leftmost_descendent_of_child_right = nullptr;
                                            loop_inner_left.index = -1, loop_inner_left.link();
                                            typename navigator_t::loop_t{
                                                .this_ = node_left,
                                                .child_right = node_right,
                                            }
                                                .link();
                                            node_left->color() = false;
                                            node_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                            node_right->color() = true;
                                            node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                            std::tie(child_left_after_split, child_left_after_split_rightmost_descendent) = std::make_tuple(node_left, node_right);
                                            move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                        }
                                        else
                                        {
                                            loop_inner_left.child_right = child_left_after_split;
                                            loop_inner_left.index = -1, loop_inner_left.link();
                                        }

                                        role_right = rb2p_node_role_e::child_right_not_a_rightmost_descendent;

                                        typename navigator_t::loop_t{
                                            .this_ = child_after_split,
                                            .child_right = std::get<1>(bnode_before_split.keys),
                                            .leftmost_descendent_of_child_right = child_right_after_split_leftmost_descendent != nullptr ? child_right_after_split_leftmost_descendent : child_right_after_split,
                                            .rightmost_descendent_of_child_left = child_left_after_split_rightmost_descendent != nullptr ? child_left_after_split_rightmost_descendent : child_left_after_split,
                                            .child_left = std::get<0>(bnode_before_split.keys).value(),
                                        }
                                            .link();
                                    }
                                    child_after_split->color() = false;
                                    child_after_split->role() = std::get<1>(bnode_before_split.keys)->role();
                                    std::get<1>(bnode_before_split.keys)->color() = true;
                                    std::get<1>(bnode_before_split.keys)->role() = role_right;
                                    if(child_left_or_right_after_split_height == 1)
                                    {
                                        child_left_after_split->role() = child_left_after_split->child_right() == nullptr ? rb2p_node_role_e::child_right_rightmost_descendent_of_non_root : rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                        child_right_after_split->role() = child_right_after_split->child_left() == nullptr ? rb2p_node_role_e::child_left_leftmost_descendent_of_non_root : rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                    }
                                    else if(child_left_or_right_after_split_height > 1)
                                    {
                                        child_left_after_split->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                        child_right_after_split->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                    }

                                    if(child_left_or_right_after_split_height == 0)
                                        refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                    else
                                    {
                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<0>(bnode_before_split.keys).value());
                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                        refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, child_after_split);
                                    }
                                }
                            }
                            else if(bnode_before_split.child_index == 1)
                            {
                                if(!std::get<2>(bnode_before_split.keys).has_value())
                                {
                                    if(child_left_or_right_after_split_height == 0)
                                    {
                                        if(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::root)
                                        {
                                            typename navigator_t::loop_t{
                                                .this_ = std::get<1>(bnode_before_split.keys),
                                                .child_right = child_after_split,
                                                .child_left = std::get<0>(bnode_before_split.keys).value_or(nullptr),
                                            }
                                                .link();
                                            child_after_split->color() = true;
                                            child_after_split->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_root;

                                            refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, child_after_split);
                                        }
                                        else
                                        {
                                            switch(std::get<1>(bnode_before_split.keys)->role())
                                            {
                                            case rb2p_node_role_e::child_right_rightmost_descendent_of_non_root:
                                            case rb2p_node_role_e::child_right_rightmost_descendent_of_root:
                                                if(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_non_root)
                                                {
                                                    typename navigator_t::loop_t loop_outer_right = navigator_t::template get_loop<navigator_t::loop_type_e::ancestor>(std::get<1>(bnode_before_split.keys));
                                                    loop_outer_right.exchange(child_after_split);
                                                }
                                                typename navigator_t::loop_t{
                                                    .this_ = std::get<1>(bnode_before_split.keys),
                                                    .child_right = child_after_split,
                                                }
                                                    .link();
                                                child_after_split->color() = true;
                                                child_after_split->role() = std::get<1>(bnode_before_split.keys)->role();
                                                std::get<1>(bnode_before_split.keys)->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;

                                                refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, child_after_split);
                                                break;
                                            case rb2p_node_role_e::child_left_leftmost_descendent_of_non_root:
                                            case rb2p_node_role_e::child_left_leftmost_descendent_of_root:
                                            {
                                                typename navigator_t::loop_t loop_outer_right = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_before_split.keys));
                                                loop_outer_right.exchange(child_after_split);
                                            }
                                                typename navigator_t::loop_t{
                                                    .this_ = child_after_split,
                                                    .child_left = std::get<1>(bnode_before_split.keys),
                                                }
                                                    .link();
                                                child_after_split->color() = false;
                                                child_after_split->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                std::get<1>(bnode_before_split.keys)->color() = true;

                                                refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                                break;
                                            case rb2p_node_role_e::child_left_not_a_leftmost_descendent:
                                            {
                                                typename navigator_t::loop_t loop_outer_right = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_before_split.keys));
                                                loop_outer_right.rightmost_descendent_of_child_left = child_after_split;
                                                loop_outer_right.index = -1, loop_outer_right.link();
                                            }
                                                typename navigator_t::loop_t{
                                                    .this_ = std::get<1>(bnode_before_split.keys),
                                                    .child_right = child_after_split,
                                                    .child_left = std::get<0>(bnode_before_split.keys).value(),
                                                }
                                                    .link();
                                                child_after_split->color() = true;
                                                child_after_split->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;

                                                refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, child_after_split);
                                                break;
                                            default:
                                                std::unreachable();
                                                break;
                                            }
                                        }
                                    }
                                    else
                                    {
                                        assert(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::root || std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent || std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent);

                                        typename navigator_t::loop_t loop_inner = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<1>(bnode_before_split.keys));
                                        loop_inner.child_right = child_after_split;
                                        if(loop_inner.leftmost_descendent_of_child_right == nullptr)
                                            loop_inner.leftmost_descendent_of_child_right = child_left_after_split;
                                        loop_inner.index = -1, loop_inner.link();

                                        typename navigator_t::loop_t{
                                            .this_ = child_after_split,
                                            .child_right = child_right_after_split,
                                            .leftmost_descendent_of_child_right = child_right_after_split_leftmost_descendent,
                                            .rightmost_descendent_of_child_left = child_left_after_split_rightmost_descendent,
                                            .child_left = child_left_after_split,
                                        }
                                            .link();

                                        child_after_split->color() = true;
                                        child_after_split->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                        if(child_left_or_right_after_split_height == 1)
                                        {
                                            if(child_right_after_split->role() != rb2p_node_role_e::child_right_rightmost_descendent_of_root)
                                                child_right_after_split->role() = child_right_after_split->child_right() == nullptr ? rb2p_node_role_e::child_right_rightmost_descendent_of_non_root : rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                            child_left_after_split->role() = child_left_after_split->child_left() == nullptr ? rb2p_node_role_e::child_left_leftmost_descendent_of_non_root : rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                        }
                                        else if(child_left_or_right_after_split_height > 1)
                                        {
                                            child_right_after_split->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                            child_left_after_split->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                        }

                                        refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, child_after_split);
                                    }
                                }
                                else
                                {
                                    rb2p_node_role_e role_left;
                                    if(child_left_or_right_after_split_height == 0)
                                    {
                                        if(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::root)
                                        {
                                            assert(std::get<2>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root);
                                            assert(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::root);

                                            loop_end.root = child_after_split;
                                            loop_end.leftmost_descendent_of_root = std::get<1>(bnode_before_split.keys);
                                            assert(!should_link_loop_end);
                                            should_link_loop_end = true;

                                            role_left = rb2p_node_role_e::child_left_leftmost_descendent_of_root;
                                        }
                                        else
                                        {
                                            assert(std::get<2>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_non_root || std::get<2>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root);
                                            assert(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent);

                                            typename navigator_t::loop_t loop_outer_left = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_before_split.keys));
                                            loop_outer_left.leftmost_descendent_of_child_right = std::get<1>(bnode_before_split.keys);
                                            loop_outer_left.child_right = child_after_split;
                                            loop_outer_left.index = -1, loop_outer_left.link();

                                            role_left = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                        }
                                        typename navigator_t::loop_t{
                                            .this_ = child_after_split,
                                            .child_right = std::get<2>(bnode_before_split.keys).value(),
                                            .child_left = std::get<1>(bnode_before_split.keys),
                                        }
                                            .link();
                                    }
                                    else
                                    {
                                        assert(std::get<2>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent);
                                        assert(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::root || std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent || std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent);

                                        typename navigator_t::loop_t loop_outer_left_or_right;
                                        if(!bnode_before_split.center_key_parent_info.is_end())
                                            loop_outer_left_or_right = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_before_split.keys));
                                        typename navigator_t::loop_t loop_inner_right = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<2>(bnode_before_split.keys).value());
                                        typename navigator_t::loop_t loop_inner_left = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<2>(bnode_before_split.keys).value());
                                        if(!bnode_before_split.center_key_parent_info.is_end())
                                            loop_outer_left_or_right.exchange(child_after_split);
                                        else
                                        {
                                            loop_end.root = child_after_split;
                                            assert(!should_link_loop_end);
                                            should_link_loop_end = true;
                                        }
                                        if(child_left_or_right_after_split_height == 1 && child_left_after_split->child_left() != nullptr && child_right_after_split->child_right() == nullptr)
                                        {
                                            navigator_except_node_end_t *node_right = child_left_after_split;
                                            navigator_except_node_end_t *node_left = node_right->child_left();
                                            loop_inner_left.leftmost_descendent_of_child_right = nullptr;
                                            loop_inner_left.child_right = node_left;
                                            loop_inner_left.index = -1, loop_inner_left.link();
                                            typename navigator_t::loop_t{
                                                .this_ = node_left,
                                                .child_right = node_right,
                                            }
                                                .link();
                                            node_left->color() = false;
                                            node_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                            node_right->color() = true;
                                            node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                            std::tie(child_left_after_split, child_left_after_split_rightmost_descendent) = std::make_tuple(node_left, node_right);
                                            move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                        }
                                        else
                                            loop_inner_left.exchange(child_left_after_split);
                                        if(child_left_or_right_after_split_height == 1 && child_right_after_split->child_right() != nullptr && child_right_after_split->child_left() == nullptr)
                                        {
                                            navigator_except_node_end_t *node_left = child_right_after_split;
                                            navigator_except_node_end_t *node_right = node_left->child_right();
                                            loop_inner_right.child_left = node_right;
                                            loop_inner_right.rightmost_descendent_of_child_left = nullptr;
                                            loop_inner_right.index = -1, loop_inner_right.link();
                                            typename navigator_t::loop_t{
                                                .this_ = node_right,
                                                .child_left = node_left,
                                            }
                                                .link();
                                            node_right->color() = false;
                                            node_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                            node_left->color() = true;
                                            node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                            std::tie(child_right_after_split, child_right_after_split_leftmost_descendent) = std::make_tuple(node_right, node_left);
                                            move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                        }
                                        else
                                        {
                                            loop_inner_right.child_left = child_right_after_split;
                                            loop_inner_right.index = -1, loop_inner_right.link();
                                        }

                                        role_left = rb2p_node_role_e::child_left_not_a_leftmost_descendent;

                                        typename navigator_t::loop_t{
                                            .this_ = child_after_split,
                                            .child_right = std::get<2>(bnode_before_split.keys).value(),
                                            .leftmost_descendent_of_child_right = child_right_after_split_leftmost_descendent != nullptr ? child_right_after_split_leftmost_descendent : child_right_after_split,
                                            .rightmost_descendent_of_child_left = child_left_after_split_rightmost_descendent != nullptr ? child_left_after_split_rightmost_descendent : child_left_after_split,
                                            .child_left = std::get<1>(bnode_before_split.keys),
                                        }
                                            .link();
                                    }
                                    child_after_split->color() = false;
                                    child_after_split->role() = std::get<1>(bnode_before_split.keys)->role();
                                    std::get<1>(bnode_before_split.keys)->color() = true;
                                    std::get<1>(bnode_before_split.keys)->role() = role_left;
                                    if(child_left_or_right_after_split_height == 1)
                                    {
                                        child_right_after_split->role() = child_right_after_split->child_left() == nullptr ? rb2p_node_role_e::child_left_leftmost_descendent_of_non_root : rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                        child_left_after_split->role() = child_left_after_split->child_right() == nullptr ? rb2p_node_role_e::child_right_rightmost_descendent_of_non_root : rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                    }
                                    else if(child_left_or_right_after_split_height > 1)
                                    {
                                        child_right_after_split->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                        child_left_after_split->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                    }

                                    if(child_left_or_right_after_split_height == 0)
                                        refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                    else
                                    {
                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<2>(bnode_before_split.keys).value());
                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                        refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, child_after_split);
                                    }
                                }
                            }
                            else if(bnode_before_split.child_index == 3)
                            {
                                rb2p_node_role_e role_left;
                                if(child_left_or_right_after_split_height == 0)
                                {
                                    if(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::root)
                                    {
                                        assert(std::get<2>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root);
                                        assert(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::root);

                                        loop_end.root = std::get<2>(bnode_before_split.keys).value();
                                        loop_end.leftmost_descendent_of_root = std::get<1>(bnode_before_split.keys);
                                        assert(should_link_loop_end);

                                        role_left = rb2p_node_role_e::child_left_leftmost_descendent_of_root;
                                    }
                                    else
                                    {
                                        assert(std::get<2>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_non_root || std::get<2>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root);
                                        assert(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent);

                                        if(std::get<2>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_non_root)
                                        {
                                            typename navigator_t::loop_t loop_outer_right = navigator_t::template get_loop<navigator_t::loop_type_e::ancestor>(std::get<2>(bnode_before_split.keys).value());
                                            loop_outer_right.exchange(child_after_split);
                                        }

                                        typename navigator_t::loop_t loop_outer_left = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_before_split.keys));
                                        loop_outer_left.leftmost_descendent_of_child_right = std::get<1>(bnode_before_split.keys);
                                        loop_outer_left.child_right = std::get<2>(bnode_before_split.keys).value();
                                        loop_outer_left.index = -1, loop_outer_left.link();

                                        role_left = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                    }
                                    typename navigator_t::loop_t{
                                        .this_ = std::get<2>(bnode_before_split.keys).value(),
                                        .child_right = child_after_split,
                                        .child_left = std::get<1>(bnode_before_split.keys),
                                    }
                                        .link();
                                }
                                else
                                {
                                    assert(std::get<2>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent);
                                    assert(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::root || std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent || std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent);
                                    typename navigator_t::loop_t loop_outer_left_or_right;
                                    if(!bnode_before_split.center_key_parent_info.is_end())
                                        loop_outer_left_or_right = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_before_split.keys));
                                    typename navigator_t::loop_t loop_inner_right = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<2>(bnode_before_split.keys).value());
                                    typename navigator_t::loop_t loop_inner_left = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<2>(bnode_before_split.keys).value());
                                    if(!bnode_before_split.center_key_parent_info.is_end())
                                        loop_outer_left_or_right.exchange(std::get<2>(bnode_before_split.keys).value());
                                    else
                                    {
                                        loop_end.root = std::get<2>(bnode_before_split.keys).value();
                                        should_link_loop_end = true;
                                    }
                                    if(child_left_or_right_after_split_height == 1 && std::get<2>(bnode_before_split.keys).value()->child_left()->child_right() == nullptr && std::get<2>(bnode_before_split.keys).value()->child_left()->child_left() != nullptr)
                                    {
                                        navigator_except_node_end_t *node_right = std::get<2>(bnode_before_split.keys).value()->child_left();
                                        navigator_except_node_end_t *node_left = node_right->child_left();
                                        typename navigator_t::loop_t{
                                            .this_ = node_left,
                                            .child_right = node_right,
                                        }
                                            .link();
                                        loop_inner_left.leftmost_descendent_of_child_right = nullptr;
                                        loop_inner_left.child_right = node_left;
                                        loop_inner_left.index = -1, loop_inner_left.link();
                                        loop_inner_right.child_left = std::get<1>(bnode_before_split.keys);
                                        loop_inner_right.rightmost_descendent_of_child_left = node_right;
                                        if(loop_inner_right.leftmost_descendent_of_child_right == nullptr)
                                            loop_inner_right.leftmost_descendent_of_child_right = child_left_after_split;
                                        loop_inner_right.child_right = child_after_split;
                                        loop_inner_right.index = -1, loop_inner_right.link();
                                        node_left->color() = false;
                                        node_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                        node_right->color() = true;
                                        node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                        move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                    }
                                    else
                                    {
                                        if(child_left_or_right_after_split_height == 1 && std::get<2>(bnode_before_split.keys).value()->child_left()->child_right() == nullptr && std::get<2>(bnode_before_split.keys).value()->child_left()->child_left() == nullptr)
                                        {
                                            assert(loop_inner_right.child_left == std::get<2>(bnode_before_split.keys).value()->child_left());
                                            assert(loop_inner_right.rightmost_descendent_of_child_left == nullptr);
                                            loop_inner_right.rightmost_descendent_of_child_left = std::get<2>(bnode_before_split.keys).value()->child_left();
                                            std::get<2>(bnode_before_split.keys).value()->child_left()->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                        }
                                        else
                                        {
                                            std::get<2>(bnode_before_split.keys).value()->child_left()->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                        }
                                        loop_inner_left.exchange(loop_inner_right.child_left);
                                        loop_inner_right.child_left = std::get<1>(bnode_before_split.keys);
                                        if(loop_inner_right.leftmost_descendent_of_child_right == nullptr)
                                            loop_inner_right.leftmost_descendent_of_child_right = child_left_after_split;
                                        loop_inner_right.child_right = child_after_split;
                                        loop_inner_right.index = -1, loop_inner_right.link();
                                    }

                                    role_left = rb2p_node_role_e::child_left_not_a_leftmost_descendent;

                                    typename navigator_t::loop_t{
                                        .this_ = child_after_split,
                                        .child_right = child_right_after_split,
                                        .leftmost_descendent_of_child_right = child_right_after_split_leftmost_descendent,
                                        .rightmost_descendent_of_child_left = child_left_after_split_rightmost_descendent,
                                        .child_left = child_left_after_split,
                                    }
                                        .link();
                                }
                                child_after_split->color() = true;
                                child_after_split->role() = std::get<2>(bnode_before_split.keys).value()->role();
                                std::get<2>(bnode_before_split.keys).value()->color() = false;
                                std::get<2>(bnode_before_split.keys).value()->role() = std::get<1>(bnode_before_split.keys)->role();
                                std::get<1>(bnode_before_split.keys)->color() = true;
                                std::get<1>(bnode_before_split.keys)->role() = role_left;
                                if(child_left_or_right_after_split_height == 1)
                                {
                                    if(child_right_after_split->role() != rb2p_node_role_e::child_right_rightmost_descendent_of_root)
                                        child_right_after_split->role() = child_right_after_split->child_right() == nullptr ? rb2p_node_role_e::child_right_rightmost_descendent_of_non_root : rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                    child_left_after_split->role() = child_left_after_split->child_left() == nullptr ? rb2p_node_role_e::child_left_leftmost_descendent_of_non_root : rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                }
                                else if(child_left_or_right_after_split_height > 1)
                                {
                                    child_right_after_split->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                    child_left_after_split->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                }

                                refresh_node_count_and_accumulated_storage(schedules, accumulator, child_after_split);
                                refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<2>(bnode_before_split.keys).value());
                            }
                            else std::unreachable();
                            height_changed = false;
                            break;
                        }
                        else
                        {
                            if(bnode_before_split.center_key_parent_info.is_end())
                            {
                                assert(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::root);
                                if(bnode_before_split.child_index == -3)
                                {
                                    if(child_left_or_right_after_split_height == 0)
                                    {
                                        assert(std::get<0>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root);
                                        assert(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::root);
                                        assert(std::get<2>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root);

                                        loop_end.root = std::get<0>(bnode_before_split.keys).value();
                                        assert(should_link_loop_end);

                                        typename navigator_t::loop_t{
                                            .this_ = std::get<0>(bnode_before_split.keys).value(),
                                            .child_right = std::get<1>(bnode_before_split.keys),
                                            .child_left = child_after_split,
                                        }
                                            .link();
                                        typename navigator_t::loop_t{
                                            .this_ = std::get<1>(bnode_before_split.keys),
                                            .child_right = std::get<2>(bnode_before_split.keys).value(),
                                        }
                                            .link();
                                    }
                                    else
                                    {
                                        assert(std::get<0>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent);
                                        assert(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::root);
                                        assert(std::get<2>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent);

                                        loop_end.root = std::get<0>(bnode_before_split.keys).value();
                                        should_link_loop_end = true;

                                        typename navigator_t::loop_t loop_inner_left = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<0>(bnode_before_split.keys).value());
                                        typename navigator_t::loop_t loop_inner_right = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<0>(bnode_before_split.keys).value());
                                        if(child_left_or_right_after_split_height == 1 && std::get<0>(bnode_before_split.keys).value()->child_right()->child_right() != nullptr && std::get<0>(bnode_before_split.keys).value()->child_right()->child_left() == nullptr)
                                        {
                                            navigator_except_node_end_t *node_left = std::get<0>(bnode_before_split.keys).value()->child_right();
                                            navigator_except_node_end_t *node_right = node_left->child_right();
                                            typename navigator_t::loop_t{
                                                .this_ = node_right,
                                                .child_left = node_left,
                                            }
                                                .link();
                                            loop_inner_right.child_left = node_right;
                                            loop_inner_right.rightmost_descendent_of_child_left = nullptr;
                                            loop_inner_right.index = -1, loop_inner_right.link();
                                            loop_inner_left.child_right = std::get<1>(bnode_before_split.keys);
                                            loop_inner_left.leftmost_descendent_of_child_right = node_left;
                                            loop_inner_left.rightmost_descendent_of_child_left = loop_inner_left.rightmost_descendent_of_child_left != nullptr ? loop_inner_left.rightmost_descendent_of_child_left : child_right_after_split;
                                            loop_inner_left.child_left = child_after_split;
                                            loop_inner_left.index = -1, loop_inner_left.link();
                                            node_right->color() = false;
                                            node_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                            node_left->color() = true;
                                            node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                            move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                        }
                                        else
                                        {
                                            if(std::get<0>(bnode_before_split.keys).value()->child_right()->child_right() == nullptr && std::get<0>(bnode_before_split.keys).value()->child_right()->child_left() == nullptr)
                                                std::get<0>(bnode_before_split.keys).value()->child_right()->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                            else
                                                std::get<0>(bnode_before_split.keys).value()->child_right()->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                            loop_inner_right.exchange(loop_inner_left.child_right);
                                            loop_inner_left.leftmost_descendent_of_child_right = loop_inner_left.leftmost_descendent_of_child_right != nullptr ? loop_inner_left.leftmost_descendent_of_child_right : loop_inner_left.child_right;
                                            loop_inner_left.rightmost_descendent_of_child_left = loop_inner_left.rightmost_descendent_of_child_left != nullptr ? loop_inner_left.rightmost_descendent_of_child_left : child_right_after_split;
                                            loop_inner_left.child_right = std::get<1>(bnode_before_split.keys);
                                            loop_inner_left.child_left = child_after_split;
                                            loop_inner_left.index = -1, loop_inner_left.link();
                                        }

                                        typename navigator_t::loop_t{
                                            .this_ = child_after_split,
                                            .child_right = child_right_after_split,
                                            .leftmost_descendent_of_child_right = child_right_after_split_leftmost_descendent,
                                            .rightmost_descendent_of_child_left = child_left_after_split_rightmost_descendent,
                                            .child_left = child_left_after_split,
                                        }
                                            .link();
                                    }
                                    child_after_split->color() = false;
                                    child_after_split->role() = std::get<0>(bnode_before_split.keys).value()->role();
                                    std::get<0>(bnode_before_split.keys).value()->color() = false;
                                    std::get<0>(bnode_before_split.keys).value()->role() = std::get<1>(bnode_before_split.keys)->role();
                                    std::get<1>(bnode_before_split.keys)->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                    if(child_left_or_right_after_split_height == 1)
                                    {
                                        if(child_left_after_split->role() != rb2p_node_role_e::child_left_leftmost_descendent_of_root)
                                            child_left_after_split->role() = child_left_after_split->child_left() == nullptr ? rb2p_node_role_e::child_left_leftmost_descendent_of_non_root : rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                        child_right_after_split->role() = child_right_after_split->child_right() == nullptr ? rb2p_node_role_e::child_right_rightmost_descendent_of_non_root : rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                    }
                                    else if(child_left_or_right_after_split_height > 1)
                                    {
                                        child_left_after_split->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                        child_right_after_split->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                    }

                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, child_after_split);
                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                    refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<0>(bnode_before_split.keys).value());
                                }
                                else if(bnode_before_split.child_index == -1)
                                {
                                    if(child_left_or_right_after_split_height == 0)
                                    {
                                        assert(std::get<0>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root);
                                        assert(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::root);
                                        assert(std::get<2>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root);

                                        loop_end.root = child_after_split;
                                        assert(!should_link_loop_end);
                                        should_link_loop_end = true;

                                        typename navigator_t::loop_t{
                                            .this_ = child_after_split,
                                            .child_right = std::get<1>(bnode_before_split.keys),
                                            .child_left = std::get<0>(bnode_before_split.keys).value(),
                                        }
                                            .link();
                                        typename navigator_t::loop_t{
                                            .this_ = std::get<1>(bnode_before_split.keys),
                                            .child_right = std::get<2>(bnode_before_split.keys).value(),
                                        }
                                            .link();
                                    }
                                    else
                                    {
                                        assert(std::get<0>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent);
                                        assert(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::root);
                                        assert(std::get<2>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent);

                                        loop_end.root = child_after_split;
                                        assert(!should_link_loop_end);
                                        should_link_loop_end = true;

                                        typename navigator_t::loop_t loop_inner_left = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<0>(bnode_before_split.keys).value());
                                        typename navigator_t::loop_t loop_inner_right = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<0>(bnode_before_split.keys).value());
                                        if(child_left_or_right_after_split_height == 1 && child_right_after_split->child_right() != nullptr && child_right_after_split->child_left() == nullptr)
                                        {
                                            navigator_except_node_end_t *node_left = child_right_after_split;
                                            navigator_except_node_end_t *node_right = node_left->child_right();
                                            loop_inner_right.rightmost_descendent_of_child_left = nullptr;
                                            loop_inner_right.child_left = node_right;
                                            loop_inner_right.index = -1, loop_inner_right.link();
                                            typename navigator_t::loop_t{
                                                .this_ = node_right,
                                                .child_left = node_left,
                                            }
                                                .link();
                                            node_right->color() = false;
                                            node_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                            node_left->color() = true;
                                            node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                            std::tie(child_right_after_split, child_right_after_split_leftmost_descendent) = std::make_tuple(node_right, node_left);
                                            move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                        }
                                        else
                                            loop_inner_right.exchange(child_right_after_split);
                                        if(child_left_or_right_after_split_height == 1 && child_left_after_split->child_left() != nullptr && child_left_after_split->child_right() == nullptr)
                                        {
                                            navigator_except_node_end_t *node_right = child_left_after_split;
                                            navigator_except_node_end_t *node_left = node_right->child_left();
                                            loop_inner_left.child_right = node_left;
                                            loop_inner_left.leftmost_descendent_of_child_right = nullptr;
                                            loop_inner_left.index = -1, loop_inner_left.link();
                                            typename navigator_t::loop_t{
                                                .this_ = node_left,
                                                .child_right = node_right,
                                            }
                                                .link();
                                            node_left->color() = false;
                                            node_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                            node_right->color() = true;
                                            node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                            std::tie(child_left_after_split, child_left_after_split_rightmost_descendent) = std::make_tuple(node_left, node_right);
                                            move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                        }
                                        else
                                        {
                                            loop_inner_left.child_right = child_left_after_split;
                                            loop_inner_left.index = -1, loop_inner_left.link();
                                        }

                                        typename navigator_t::loop_t{
                                            .this_ = child_after_split,
                                            .child_right = std::get<1>(bnode_before_split.keys),
                                            .leftmost_descendent_of_child_right = child_right_after_split_leftmost_descendent != nullptr ? child_right_after_split_leftmost_descendent : child_right_after_split,
                                            .rightmost_descendent_of_child_left = child_left_after_split_rightmost_descendent != nullptr ? child_left_after_split_rightmost_descendent : child_left_after_split,
                                            .child_left = std::get<0>(bnode_before_split.keys).value(),
                                        }
                                            .link();
                                    }
                                    std::get<0>(bnode_before_split.keys).value()->color() = false;
                                    child_after_split->color() = false;
                                    child_after_split->role() = std::get<1>(bnode_before_split.keys)->role();
                                    std::get<1>(bnode_before_split.keys)->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                    if(child_left_or_right_after_split_height == 1)
                                    {
                                        child_left_after_split->role() = child_left_after_split->child_right() == nullptr ? rb2p_node_role_e::child_right_rightmost_descendent_of_non_root : rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                        child_right_after_split->role() = child_right_after_split->child_left() == nullptr ? rb2p_node_role_e::child_left_leftmost_descendent_of_non_root : rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                    }
                                    else if(child_left_or_right_after_split_height > 1)
                                    {
                                        child_left_after_split->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                        child_right_after_split->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                    }

                                    if(child_left_or_right_after_split_height == 0)
                                        refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                    else
                                    {
                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<0>(bnode_before_split.keys).value());
                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                        refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, child_after_split);
                                    }
                                }
                                else if(bnode_before_split.child_index == 1)
                                {
                                    if(child_left_or_right_after_split_height == 0)
                                    {
                                        assert(std::get<0>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root);
                                        assert(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::root);
                                        assert(std::get<2>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root);

                                        loop_end.root = child_after_split;
                                        assert(!should_link_loop_end);
                                        should_link_loop_end = true;

                                        typename navigator_t::loop_t{
                                            .this_ = child_after_split,
                                            .child_right = std::get<2>(bnode_before_split.keys).value(),
                                            .child_left = std::get<1>(bnode_before_split.keys),
                                        }
                                            .link();
                                        typename navigator_t::loop_t{
                                            .this_ = std::get<1>(bnode_before_split.keys),
                                            .child_left = std::get<0>(bnode_before_split.keys).value(),
                                        }
                                            .link();
                                    }
                                    else
                                    {
                                        assert(std::get<0>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent);
                                        assert(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::root);
                                        assert(std::get<2>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent);

                                        loop_end.root = child_after_split;
                                        assert(!should_link_loop_end);
                                        should_link_loop_end = true;

                                        typename navigator_t::loop_t loop_inner_right = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<2>(bnode_before_split.keys).value());
                                        typename navigator_t::loop_t loop_inner_left = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<2>(bnode_before_split.keys).value());
                                        if(child_left_or_right_after_split_height == 1 && child_left_after_split->child_left() != nullptr && child_left_after_split->child_right() == nullptr)
                                        {
                                            navigator_except_node_end_t *node_right = child_left_after_split;
                                            navigator_except_node_end_t *node_left = node_right->child_left();
                                            loop_inner_left.leftmost_descendent_of_child_right = nullptr;
                                            loop_inner_left.child_right = node_left;
                                            loop_inner_left.index = -1, loop_inner_left.link();
                                            typename navigator_t::loop_t{
                                                .this_ = node_left,
                                                .child_right = node_right,
                                            }
                                                .link();
                                            node_left->color() = false;
                                            node_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                            node_right->color() = true;
                                            node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                            std::tie(child_left_after_split, child_left_after_split_rightmost_descendent) = std::make_tuple(node_left, node_right);
                                            move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                        }
                                        else
                                            loop_inner_left.exchange(child_left_after_split);
                                        if(child_left_or_right_after_split_height == 1 && child_right_after_split->child_right() != nullptr && child_right_after_split->child_left() == nullptr)
                                        {
                                            navigator_except_node_end_t *node_left = child_right_after_split;
                                            navigator_except_node_end_t *node_right = node_left->child_right();
                                            loop_inner_right.child_left = node_right;
                                            loop_inner_right.rightmost_descendent_of_child_left = nullptr;
                                            loop_inner_right.index = -1, loop_inner_right.link();
                                            typename navigator_t::loop_t{
                                                .this_ = node_right,
                                                .child_left = node_left,
                                            }
                                                .link();
                                            node_right->color() = false;
                                            node_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                            node_left->color() = true;
                                            node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                            std::tie(child_right_after_split, child_right_after_split_leftmost_descendent) = std::make_tuple(node_right, node_left);
                                            move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                        }
                                        else
                                        {
                                            loop_inner_right.child_left = child_right_after_split;
                                            loop_inner_right.index = -1, loop_inner_right.link();
                                        }

                                        typename navigator_t::loop_t{
                                            .this_ = child_after_split,
                                            .child_right = std::get<2>(bnode_before_split.keys).value(),
                                            .leftmost_descendent_of_child_right = child_right_after_split_leftmost_descendent != nullptr ? child_right_after_split_leftmost_descendent : child_right_after_split,
                                            .rightmost_descendent_of_child_left = child_left_after_split_rightmost_descendent != nullptr ? child_left_after_split_rightmost_descendent : child_left_after_split,
                                            .child_left = std::get<1>(bnode_before_split.keys),
                                        }
                                            .link();
                                    }
                                    std::get<2>(bnode_before_split.keys).value()->color() = false;
                                    child_after_split->color() = false;
                                    child_after_split->role() = std::get<1>(bnode_before_split.keys)->role();
                                    std::get<1>(bnode_before_split.keys)->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                    if(child_left_or_right_after_split_height == 1)
                                    {
                                        child_right_after_split->role() = child_right_after_split->child_left() == nullptr ? rb2p_node_role_e::child_left_leftmost_descendent_of_non_root : rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                        child_left_after_split->role() = child_left_after_split->child_right() == nullptr ? rb2p_node_role_e::child_right_rightmost_descendent_of_non_root : rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                    }
                                    else if(child_left_or_right_after_split_height > 1)
                                    {
                                        child_right_after_split->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                        child_left_after_split->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                    }

                                    if(child_left_or_right_after_split_height == 0)
                                        refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                    else
                                    {
                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<2>(bnode_before_split.keys).value());
                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                        refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, child_after_split);
                                    }
                                }
                                else if(bnode_before_split.child_index == 3)
                                {
                                    if(child_left_or_right_after_split_height == 0)
                                    {
                                        assert(std::get<0>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root);
                                        assert(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::root);
                                        assert(std::get<2>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root);

                                        loop_end.root = std::get<2>(bnode_before_split.keys).value();
                                        assert(should_link_loop_end);

                                        typename navigator_t::loop_t{
                                            .this_ = std::get<2>(bnode_before_split.keys).value(),
                                            .child_right = child_after_split,
                                            .child_left = std::get<1>(bnode_before_split.keys),
                                        }
                                            .link();
                                        typename navigator_t::loop_t{
                                            .this_ = std::get<1>(bnode_before_split.keys),
                                            .child_left = std::get<0>(bnode_before_split.keys).value(),
                                        }
                                            .link();
                                    }
                                    else
                                    {
                                        assert(std::get<0>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent);
                                        assert(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::root);
                                        assert(std::get<2>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent);

                                        loop_end.root = std::get<2>(bnode_before_split.keys).value();
                                        should_link_loop_end = true;

                                        typename navigator_t::loop_t loop_inner_right = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<2>(bnode_before_split.keys).value());
                                        typename navigator_t::loop_t loop_inner_left = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<2>(bnode_before_split.keys).value());
                                        if(child_left_or_right_after_split_height == 1 && std::get<2>(bnode_before_split.keys).value()->child_left()->child_left() != nullptr && std::get<2>(bnode_before_split.keys).value()->child_left()->child_right() == nullptr)
                                        {
                                            navigator_except_node_end_t *node_right = std::get<2>(bnode_before_split.keys).value()->child_left();
                                            navigator_except_node_end_t *node_left = node_right->child_left();
                                            typename navigator_t::loop_t{
                                                .this_ = node_left,
                                                .child_right = node_right,
                                            }
                                                .link();
                                            loop_inner_left.child_right = node_left;
                                            loop_inner_left.leftmost_descendent_of_child_right = nullptr;
                                            loop_inner_left.index = -1, loop_inner_left.link();
                                            loop_inner_right.child_left = std::get<1>(bnode_before_split.keys);
                                            loop_inner_right.rightmost_descendent_of_child_left = node_right;
                                            loop_inner_right.leftmost_descendent_of_child_right = loop_inner_right.leftmost_descendent_of_child_right != nullptr ? loop_inner_right.leftmost_descendent_of_child_right : child_left_after_split;
                                            loop_inner_right.child_right = child_after_split;
                                            loop_inner_right.index = -1, loop_inner_right.link();
                                            node_left->color() = false;
                                            node_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                            node_right->color() = true;
                                            node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                            move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                        }
                                        else
                                        {
                                            if(std::get<2>(bnode_before_split.keys).value()->child_left()->child_left() == nullptr && std::get<2>(bnode_before_split.keys).value()->child_left()->child_right() == nullptr)
                                                std::get<2>(bnode_before_split.keys).value()->child_left()->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                            else
                                                std::get<2>(bnode_before_split.keys).value()->child_left()->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                            loop_inner_left.exchange(loop_inner_right.child_left);
                                            loop_inner_right.rightmost_descendent_of_child_left = loop_inner_right.rightmost_descendent_of_child_left != nullptr ? loop_inner_right.rightmost_descendent_of_child_left : loop_inner_right.child_left;
                                            loop_inner_right.leftmost_descendent_of_child_right = loop_inner_right.leftmost_descendent_of_child_right != nullptr ? loop_inner_right.leftmost_descendent_of_child_right : child_left_after_split;
                                            loop_inner_right.child_left = std::get<1>(bnode_before_split.keys);
                                            loop_inner_right.child_right = child_after_split;
                                            loop_inner_right.index = -1, loop_inner_right.link();
                                        }

                                        typename navigator_t::loop_t{
                                            .this_ = child_after_split,
                                            .child_right = child_right_after_split,
                                            .leftmost_descendent_of_child_right = child_right_after_split_leftmost_descendent,
                                            .rightmost_descendent_of_child_left = child_left_after_split_rightmost_descendent,
                                            .child_left = child_left_after_split,
                                        }
                                            .link();
                                    }
                                    child_after_split->color() = false;
                                    child_after_split->role() = std::get<2>(bnode_before_split.keys).value()->role();
                                    std::get<2>(bnode_before_split.keys).value()->color() = false;
                                    std::get<2>(bnode_before_split.keys).value()->role() = std::get<1>(bnode_before_split.keys)->role();
                                    std::get<1>(bnode_before_split.keys)->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                    if(child_left_or_right_after_split_height == 1)
                                    {
                                        if(child_right_after_split->role() != rb2p_node_role_e::child_right_rightmost_descendent_of_root)
                                            child_right_after_split->role() = child_right_after_split->child_right() == nullptr ? rb2p_node_role_e::child_right_rightmost_descendent_of_non_root : rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                        child_left_after_split->role() = child_left_after_split->child_left() == nullptr ? rb2p_node_role_e::child_left_leftmost_descendent_of_non_root : rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                    }
                                    else if(child_left_or_right_after_split_height > 1)
                                    {
                                        child_right_after_split->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                        child_left_after_split->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                    }

                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, child_after_split);
                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                    refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<2>(bnode_before_split.keys).value());
                                }
                                else std::unreachable();
                                height_changed = true;
                                break;
                            }
                            else
                            {
                                navigator_except_node_end_t *child_after_split_old = child_after_split;

                                bnode_up_t bnode_parent = bnode_up_t::get_bnode_from_key(static_cast<navigator_except_node_end_t *>(bnode_before_split.center_key_parent_info.parent), bnode_before_split.center_key_parent_info.is_left_or_right_child_of_parent);
                                auto try_spill_to_left_sibling = [&](auto fallback)
                                {
                                    return [&, fallback]()
                                    {
                                        if(std::optional<navigator_except_node_end_t *> opt_key_black_left_sibling = bnode_parent.opt_child_at(bnode_parent.child_index - 2); opt_key_black_left_sibling.has_value())
                                        {
                                            auto [bnode_left_sibling, bnode_left_sibling_rightmost_child] = bnode_up_t::inserting_get_bnode_from_key_black_sibling(opt_key_black_left_sibling.value(), false);
                                            if(bnode_left_sibling.key_count != 3)
                                            {
                                                navigator_except_node_end_t *parent = bnode_parent.key_at(bnode_parent.child_index - 1);
                                                if(bnode_before_split.child_index == -3)
                                                {
                                                    typename navigator_t::loop_t loop_outer_parent;
                                                    if(parent->role() != rb2p_node_role_e::root)
                                                        loop_outer_parent = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(parent);
                                                    typename navigator_t::loop_t loop_inner_parent = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(parent);
                                                    if(child_left_or_right_after_split_height == 0)
                                                        loop_inner_parent.exchange(child_after_split);
                                                    else
                                                    {
                                                        loop_inner_parent.this_ = child_after_split;

                                                        typename navigator_t::loop_t loop_inner_right = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<0>(bnode_before_split.keys).value());
                                                        if(child_left_or_right_after_split_height == 1 && child_right_after_split->child_right() != nullptr && child_right_after_split->child_left() == nullptr)
                                                        {
                                                            navigator_except_node_end_t *node_left = child_right_after_split;
                                                            navigator_except_node_end_t *node_right = node_left->child_right();
                                                            typename navigator_t::loop_t{
                                                                .this_ = node_right,
                                                                .child_left = node_left,
                                                            }
                                                                .link();
                                                            loop_inner_right.child_left = node_right;
                                                            loop_inner_right.rightmost_descendent_of_child_left = nullptr;
                                                            loop_inner_right.index = -1, loop_inner_right.link();
                                                            node_left->color() = true;
                                                            node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                            node_right->color() = false;
                                                            node_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                            std::tie(child_right_after_split, child_right_after_split_leftmost_descendent) = std::make_tuple(node_right, node_left);
                                                            move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                                        }
                                                        else
                                                        {
                                                            loop_inner_right.child_left = child_right_after_split;
                                                            loop_inner_right.index = -1, loop_inner_right.link();
                                                        }
                                                        if(child_left_or_right_after_split_height == 1)
                                                        {
                                                            child_right_after_split->role() = child_right_after_split->child_left() == nullptr ? rb2p_node_role_e::child_left_leftmost_descendent_of_non_root : rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                        }
                                                        else if(child_left_or_right_after_split_height > 1)
                                                        {
                                                            child_right_after_split->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                        }
                                                        if(child_right_after_split_leftmost_descendent != nullptr)
                                                            std::ranges::swap(loop_inner_parent.leftmost_descendent_of_child_right, child_right_after_split_leftmost_descendent);
                                                        else
                                                            child_right_after_split_leftmost_descendent = std::exchange(loop_inner_parent.leftmost_descendent_of_child_right, child_right_after_split);

                                                        if(child_left_or_right_after_split_height == 1 && child_left_after_split->child_left() != nullptr && child_left_after_split->child_right() == nullptr)
                                                        {
                                                            navigator_except_node_end_t *node_right = child_left_after_split;
                                                            navigator_except_node_end_t *node_left = node_right->child_left();
                                                            typename navigator_t::loop_t{
                                                                .this_ = node_left,
                                                                .child_right = node_right,
                                                            }
                                                                .link();
                                                            node_right->color() = true;
                                                            node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                            node_left->color() = false;
                                                            node_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                            std::tie(child_left_after_split, child_left_after_split_rightmost_descendent) = std::make_tuple(node_left, node_right);
                                                            move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                                        }
                                                        else if(child_left_or_right_after_split_height == 1 && child_left_after_split->child_left() == nullptr && child_left_after_split->child_right() == nullptr)
                                                            child_left_after_split->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                        child_right_after_split = child_left_after_split;
                                                        if(child_left_after_split_rightmost_descendent != nullptr)
                                                            std::ranges::swap(loop_inner_parent.rightmost_descendent_of_child_left, child_left_after_split_rightmost_descendent);
                                                        else
                                                            child_left_after_split_rightmost_descendent = std::exchange(loop_inner_parent.rightmost_descendent_of_child_left, child_left_after_split);

                                                        loop_inner_parent.index = -1, loop_inner_parent.link();

                                                        if(child_left_or_right_after_split_height == 1 && bnode_left_sibling_rightmost_child->child_right() != nullptr && bnode_left_sibling_rightmost_child->child_left() == nullptr)
                                                        {
                                                            navigator_except_node_end_t *node_left = bnode_left_sibling_rightmost_child;
                                                            navigator_except_node_end_t *node_right = node_left->child_right();
                                                            typename navigator_t::loop_t loop_outer_sibling = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(node_left);
                                                            loop_outer_sibling.child_right = node_right;
                                                            loop_outer_sibling.leftmost_descendent_of_child_right = node_left;
                                                            loop_outer_sibling.index = -1, loop_outer_sibling.link();
                                                            typename navigator_t::loop_t{
                                                                .this_ = node_right,
                                                                .child_left = node_left,
                                                            }
                                                                .link();
                                                            node_left->color() = true;
                                                            node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                            node_right->color() = false;
                                                            node_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                            child_left_after_split = node_right;
                                                            move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                                        }
                                                        else
                                                            child_left_after_split = bnode_left_sibling_rightmost_child;
                                                    }
                                                    if(parent->role() != rb2p_node_role_e::root)
                                                        loop_outer_parent.exchange(child_after_split);
                                                    else
                                                    {
                                                        loop_end.root = child_after_split;
                                                        assert(!should_link_loop_end);
                                                        should_link_loop_end = true;
                                                    }
                                                    child_after_split->color() = parent->color();
                                                    child_after_split->role() = parent->role();
                                                    child_after_split = parent;

                                                    if(child_left_or_right_after_split_height == 0)
                                                        ;
                                                    else
                                                        refresh_node_count_and_accumulated_storage_and_above_until(schedules, accumulator, std::get<0>(bnode_before_split.keys).value(), child_after_split_old);
                                                }
                                                else if(bnode_before_split.child_index == -1)
                                                {
                                                    typename navigator_t::loop_t loop_outer_parent;
                                                    if(parent->role() != rb2p_node_role_e::root)
                                                        loop_outer_parent = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(parent);
                                                    typename navigator_t::loop_t loop_inner_parent = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(parent);
                                                    if(child_left_or_right_after_split_height == 0)
                                                    {
                                                        typename navigator_t::loop_t{
                                                            .this_ = std::get<1>(bnode_before_split.keys),
                                                            .child_right = std::get<2>(bnode_before_split.keys).value(),
                                                            .child_left = child_after_split,
                                                        }
                                                            .link();
                                                        loop_inner_parent.this_ = std::get<0>(bnode_before_split.keys).value();
                                                        loop_inner_parent.leftmost_descendent_of_child_right = child_after_split;
                                                        loop_inner_parent.index = -1, loop_inner_parent.link();
                                                    }
                                                    else
                                                    {
                                                        loop_inner_parent.this_ = std::get<0>(bnode_before_split.keys).value();

                                                        typename navigator_t::loop_t loop_inner_left = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<0>(bnode_before_split.keys).value());
                                                        typename navigator_t::loop_t loop_inner_right = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<0>(bnode_before_split.keys).value());
                                                        navigator_except_node_end_t *key_left_child_left, *key_left_child_left_child_left, *key_left_child_left_child_right;
                                                        if(child_left_or_right_after_split_height == 1)
                                                        {
                                                            key_left_child_left = std::get<0>(bnode_before_split.keys).value()->child_left();
                                                            key_left_child_left_child_left = key_left_child_left->child_left();
                                                            key_left_child_left_child_right = key_left_child_left->child_right();
                                                        }
                                                        loop_inner_right.exchange(child_after_split);
                                                        typename navigator_t::loop_t{
                                                            .this_ = child_after_split,
                                                            .child_right = child_right_after_split,
                                                            .leftmost_descendent_of_child_right = child_right_after_split_leftmost_descendent,
                                                            .rightmost_descendent_of_child_left = child_left_after_split_rightmost_descendent,
                                                            .child_left = child_left_after_split,
                                                        }
                                                            .link();
                                                        if(child_left_or_right_after_split_height == 1)
                                                        {
                                                            child_left_after_split->role() = child_left_after_split->child_left() == nullptr ? rb2p_node_role_e::child_left_leftmost_descendent_of_non_root : rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                            child_right_after_split->role() = child_right_after_split->child_right() == nullptr ? rb2p_node_role_e::child_right_rightmost_descendent_of_non_root : rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                        }
                                                        else if(child_left_or_right_after_split_height > 1)
                                                        {
                                                            child_left_after_split->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                            child_right_after_split->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                        }
                                                        child_right_after_split_leftmost_descendent = loop_inner_parent.leftmost_descendent_of_child_right;
                                                        loop_inner_parent.leftmost_descendent_of_child_right = loop_inner_left.leftmost_descendent_of_child_right != nullptr ? loop_inner_left.leftmost_descendent_of_child_right : loop_inner_left.child_right;

                                                        if(child_left_or_right_after_split_height == 1 && key_left_child_left_child_left != nullptr && key_left_child_left_child_right == nullptr)
                                                        {
                                                            navigator_except_node_end_t *node_right = key_left_child_left;
                                                            navigator_except_node_end_t *node_left = key_left_child_left_child_left;
                                                            typename navigator_t::loop_t{
                                                                .this_ = node_left,
                                                                .child_right = node_right,
                                                            }
                                                                .link();
                                                            node_right->color() = true;
                                                            node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                            node_left->color() = false;
                                                            node_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                            loop_inner_left.rightmost_descendent_of_child_left = node_right;
                                                            loop_inner_left.child_left = node_left;
                                                            move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                                        }
                                                        else if(child_left_or_right_after_split_height == 1 && key_left_child_left_child_left == nullptr && key_left_child_left_child_right == nullptr)
                                                            key_left_child_left->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                        child_left_after_split_rightmost_descendent = loop_inner_parent.rightmost_descendent_of_child_left;
                                                        loop_inner_parent.rightmost_descendent_of_child_left = loop_inner_left.rightmost_descendent_of_child_left != nullptr ? loop_inner_left.rightmost_descendent_of_child_left : loop_inner_left.child_left;
                                                        child_right_after_split = loop_inner_left.child_left;

                                                        loop_inner_parent.index = -1, loop_inner_parent.link();

                                                        if(child_left_or_right_after_split_height == 1 && bnode_left_sibling_rightmost_child->child_right() != nullptr && bnode_left_sibling_rightmost_child->child_left() == nullptr)
                                                        {
                                                            navigator_except_node_end_t *node_left = bnode_left_sibling_rightmost_child;
                                                            navigator_except_node_end_t *node_right = node_left->child_right();
                                                            typename navigator_t::loop_t loop_outer_sibling = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(node_left);
                                                            loop_outer_sibling.child_right = node_right;
                                                            loop_outer_sibling.leftmost_descendent_of_child_right = node_left;
                                                            loop_outer_sibling.index = -1, loop_outer_sibling.link();
                                                            typename navigator_t::loop_t{
                                                                .this_ = node_right,
                                                                .child_left = node_left,
                                                            }
                                                                .link();
                                                            node_left->color() = true;
                                                            node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                            node_right->color() = false;
                                                            node_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                            child_left_after_split = node_right;
                                                            move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                                        }
                                                        else
                                                            child_left_after_split = bnode_left_sibling_rightmost_child;
                                                    }
                                                    if(parent->role() != rb2p_node_role_e::root)
                                                        loop_outer_parent.exchange(std::get<0>(bnode_before_split.keys).value());
                                                    else
                                                    {
                                                        loop_end.root = std::get<0>(bnode_before_split.keys).value();
                                                        assert(!should_link_loop_end);
                                                        should_link_loop_end = true;
                                                    }
                                                    child_after_split->color() = std::get<0>(bnode_before_split.keys).value()->color();
                                                    child_after_split->role() = std::get<0>(bnode_before_split.keys).value()->role();
                                                    std::get<0>(bnode_before_split.keys).value()->color() = parent->color();
                                                    std::get<0>(bnode_before_split.keys).value()->role() = parent->role();
                                                    child_after_split = parent;

                                                    if(child_left_or_right_after_split_height == 0)
                                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, child_after_split_old);
                                                    else
                                                        refresh_node_count_and_accumulated_storage_and_above_until(schedules, accumulator, child_after_split_old, std::get<0>(bnode_before_split.keys).value());
                                                }
                                                else if(bnode_before_split.child_index == 1)
                                                {
                                                    typename navigator_t::loop_t loop_outer_parent;
                                                    if(parent->role() != rb2p_node_role_e::root)
                                                        loop_outer_parent = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(parent);
                                                    typename navigator_t::loop_t loop_inner_parent = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(parent);
                                                    if(child_left_or_right_after_split_height == 0)
                                                    {
                                                        typename navigator_t::loop_t loop_inner_parent2;
                                                        if(!bnode_before_split.center_key_parent_info.is_left_or_right_child_of_parent)
                                                            loop_inner_parent2 = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_before_split.keys));
                                                        typename navigator_t::loop_t{
                                                            .this_ = child_after_split,
                                                            .child_right = std::get<2>(bnode_before_split.keys).value(),
                                                            .child_left = std::get<1>(bnode_before_split.keys),
                                                        }
                                                            .link();
                                                        if(!bnode_before_split.center_key_parent_info.is_left_or_right_child_of_parent)
                                                            loop_inner_parent2.exchange(child_after_split);
                                                        else
                                                            loop_inner_parent.child_right = child_after_split;
                                                        loop_inner_parent.this_ = std::get<0>(bnode_before_split.keys).value();
                                                        loop_inner_parent.leftmost_descendent_of_child_right = std::get<1>(bnode_before_split.keys);
                                                        loop_inner_parent.index = -1, loop_inner_parent.link();
                                                    }
                                                    else
                                                    {
                                                        loop_inner_parent.this_ = std::get<0>(bnode_before_split.keys).value();

                                                        typename navigator_t::loop_t loop_inner_parent2;
                                                        if(!bnode_before_split.center_key_parent_info.is_left_or_right_child_of_parent)
                                                            loop_inner_parent2 = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_before_split.keys));
                                                        typename navigator_t::loop_t loop_inner_left = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<0>(bnode_before_split.keys).value());
                                                        typename navigator_t::loop_t loop_inner_middle = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<1>(bnode_before_split.keys));
                                                        typename navigator_t::loop_t loop_inner_right = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<2>(bnode_before_split.keys).value());
                                                        navigator_except_node_end_t *key_left_child_left, *key_left_child_left_child_left, *key_left_child_left_child_right;
                                                        navigator_except_node_end_t *key_left_child_right, *key_left_child_right_child_left, *key_left_child_right_child_right;
                                                        if(child_left_or_right_after_split_height == 1)
                                                        {
                                                            key_left_child_left = std::get<0>(bnode_before_split.keys).value()->child_left();
                                                            key_left_child_left_child_left = key_left_child_left->child_left();
                                                            key_left_child_left_child_right = key_left_child_left->child_right();
                                                            key_left_child_right = std::get<0>(bnode_before_split.keys).value()->child_right();
                                                            key_left_child_right_child_left = key_left_child_right->child_left();
                                                            key_left_child_right_child_right = key_left_child_right->child_right();
                                                        }
                                                        if(!bnode_before_split.center_key_parent_info.is_left_or_right_child_of_parent)
                                                            loop_inner_parent2.exchange(child_after_split);
                                                        else
                                                            loop_inner_parent.child_right = child_after_split;

                                                        if(child_left_or_right_after_split_height == 1 && child_right_after_split->child_right() != nullptr && child_right_after_split->child_left() == nullptr)
                                                        {
                                                            navigator_except_node_end_t *node_left = child_right_after_split;
                                                            navigator_except_node_end_t *node_right = node_left->child_right();
                                                            typename navigator_t::loop_t{
                                                                .this_ = node_right,
                                                                .child_left = node_left,
                                                            }
                                                                .link();
                                                            node_left->color() = true;
                                                            node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                            node_right->color() = false;
                                                            node_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                            std::tie(child_right_after_split, child_right_after_split_leftmost_descendent) = std::make_tuple(node_right, node_left);
                                                            move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                                        }
                                                        loop_inner_right.child_left = child_right_after_split;
                                                        loop_inner_right.index = -1, loop_inner_right.link();
                                                        if(child_left_or_right_after_split_height == 1)
                                                        {
                                                            child_right_after_split->role() = child_right_after_split->child_left() == nullptr ? rb2p_node_role_e::child_left_leftmost_descendent_of_non_root : rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                        }
                                                        else if(child_left_or_right_after_split_height > 1)
                                                        {
                                                            child_right_after_split->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                        }

                                                        if(child_left_or_right_after_split_height == 1 && child_left_after_split->child_left() != nullptr && child_left_after_split->child_right() == nullptr)
                                                        {
                                                            navigator_except_node_end_t *node_right = child_left_after_split;
                                                            navigator_except_node_end_t *node_left = node_right->child_left();
                                                            typename navigator_t::loop_t{
                                                                .this_ = node_left,
                                                                .child_right = node_right,
                                                            }
                                                                .link();
                                                            node_right->color() = true;
                                                            node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                            node_left->color() = false;
                                                            node_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                            std::tie(child_left_after_split, child_left_after_split_rightmost_descendent) = std::make_tuple(node_left, node_right);
                                                            move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                                        }
                                                        typename navigator_t::loop_t{
                                                            .this_ = child_after_split,
                                                            .child_right = std::get<2>(bnode_before_split.keys).value(),
                                                            .leftmost_descendent_of_child_right = child_right_after_split_leftmost_descendent != nullptr ? child_right_after_split_leftmost_descendent : child_right_after_split,
                                                            .rightmost_descendent_of_child_left = child_left_after_split_rightmost_descendent != nullptr ? child_left_after_split_rightmost_descendent : child_left_after_split,
                                                            .child_left = std::get<1>(bnode_before_split.keys),
                                                        }
                                                            .link();
                                                        if(child_left_or_right_after_split_height == 1)
                                                        {
                                                            child_left_after_split->role() = child_left_after_split->child_right() == nullptr ? rb2p_node_role_e::child_right_rightmost_descendent_of_non_root : rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                        }
                                                        else if(child_left_or_right_after_split_height > 1)
                                                        {
                                                            child_left_after_split->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                        }

                                                        if(child_left_or_right_after_split_height == 1 && key_left_child_right_child_right != nullptr && key_left_child_right_child_left == nullptr)
                                                        {
                                                            navigator_except_node_end_t *node_left = key_left_child_right;
                                                            navigator_except_node_end_t *node_right = key_left_child_right_child_right;
                                                            typename navigator_t::loop_t{
                                                                .this_ = node_right,
                                                                .child_left = node_left,
                                                            }
                                                                .link();
                                                            node_left->color() = true;
                                                            node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                            node_right->color() = false;
                                                            node_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                            loop_inner_left.child_right = node_right;
                                                            loop_inner_left.leftmost_descendent_of_child_right = node_left;
                                                            move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                                        }
                                                        loop_inner_middle.child_right = child_left_after_split;
                                                        loop_inner_middle.child_left = loop_inner_left.child_right;
                                                        loop_inner_middle.index = -1, loop_inner_middle.link();
                                                        if(child_left_or_right_after_split_height == 1)
                                                        {
                                                            loop_inner_left.child_right->role() = loop_inner_left.child_right->child_left() == nullptr ? rb2p_node_role_e::child_left_leftmost_descendent_of_non_root : rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                        }
                                                        else if(child_left_or_right_after_split_height > 1)
                                                        {
                                                            loop_inner_left.child_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                        }

                                                        if(child_left_or_right_after_split_height == 1 && key_left_child_left_child_left != nullptr && key_left_child_left_child_right == nullptr)
                                                        {
                                                            navigator_except_node_end_t *node_right = key_left_child_left;
                                                            navigator_except_node_end_t *node_left = key_left_child_left_child_left;
                                                            typename navigator_t::loop_t{
                                                                .this_ = node_left,
                                                                .child_right = node_right,
                                                            }
                                                                .link();
                                                            node_right->color() = true;
                                                            node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                            node_left->color() = false;
                                                            node_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                            loop_inner_left.rightmost_descendent_of_child_left = node_right;
                                                            loop_inner_left.child_left = node_left;
                                                            move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                                        }
                                                        else if(child_left_or_right_after_split_height == 1 && key_left_child_left_child_left == nullptr && key_left_child_left_child_right == nullptr)
                                                            key_left_child_left->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                        child_right_after_split = loop_inner_left.child_left;
                                                        child_right_after_split_leftmost_descendent = loop_inner_parent.leftmost_descendent_of_child_right;
                                                        loop_inner_parent.leftmost_descendent_of_child_right = loop_inner_left.leftmost_descendent_of_child_right != nullptr ? loop_inner_left.leftmost_descendent_of_child_right : loop_inner_left.child_right;
                                                        child_left_after_split_rightmost_descendent = loop_inner_parent.rightmost_descendent_of_child_left;
                                                        loop_inner_parent.rightmost_descendent_of_child_left = loop_inner_left.rightmost_descendent_of_child_left != nullptr ? loop_inner_left.rightmost_descendent_of_child_left : loop_inner_left.child_left;

                                                        loop_inner_parent.index = -1, loop_inner_parent.link();

                                                        if(child_left_or_right_after_split_height == 1 && bnode_left_sibling_rightmost_child->child_right() != nullptr && bnode_left_sibling_rightmost_child->child_left() == nullptr)
                                                        {
                                                            navigator_except_node_end_t *node_left = bnode_left_sibling_rightmost_child;
                                                            navigator_except_node_end_t *node_right = node_left->child_right();
                                                            typename navigator_t::loop_t loop_outer_sibling = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(node_left);
                                                            loop_outer_sibling.child_right = node_right;
                                                            loop_outer_sibling.leftmost_descendent_of_child_right = node_left;
                                                            loop_outer_sibling.index = -1, loop_outer_sibling.link();
                                                            typename navigator_t::loop_t{
                                                                .this_ = node_right,
                                                                .child_left = node_left,
                                                            }
                                                                .link();
                                                            node_left->color() = true;
                                                            node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                            node_right->color() = false;
                                                            node_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                            child_left_after_split = node_right;
                                                            move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                                        }
                                                        else
                                                            child_left_after_split = bnode_left_sibling_rightmost_child;
                                                    }
                                                    if(parent->role() != rb2p_node_role_e::root)
                                                        loop_outer_parent.exchange(std::get<0>(bnode_before_split.keys).value());
                                                    else
                                                    {
                                                        loop_end.root = std::get<0>(bnode_before_split.keys).value();
                                                        assert(!should_link_loop_end);
                                                        should_link_loop_end = true;
                                                    }
                                                    child_after_split->color() = std::get<1>(bnode_before_split.keys)->color();
                                                    child_after_split->role() = std::get<1>(bnode_before_split.keys)->role();
                                                    std::get<1>(bnode_before_split.keys)->color() = std::get<0>(bnode_before_split.keys).value()->color();
                                                    std::get<1>(bnode_before_split.keys)->role() = std::get<0>(bnode_before_split.keys).value()->role();
                                                    std::get<0>(bnode_before_split.keys).value()->color() = parent->color();
                                                    std::get<0>(bnode_before_split.keys).value()->role() = parent->role();
                                                    child_after_split = parent;

                                                    if(child_left_or_right_after_split_height == 0)
                                                    {
                                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, child_after_split_old);
                                                    }
                                                    else
                                                    {
                                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<2>(bnode_before_split.keys).value());
                                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                                        refresh_node_count_and_accumulated_storage_and_above_until(schedules, accumulator, child_after_split_old, std::get<0>(bnode_before_split.keys).value());
                                                    }
                                                }
                                                else if(bnode_before_split.child_index == 3)
                                                {
                                                    if(child_left_or_right_after_split_height == 0 && std::get<2>(bnode_before_split.keys).value()->role() != rb2p_node_role_e::child_right_rightmost_descendent_of_root)
                                                    {
                                                        typename navigator_t::loop_t loop_outer_parent = navigator_t::template get_loop<navigator_t::loop_type_e::ancestor>(std::get<2>(bnode_before_split.keys).value());
                                                        loop_outer_parent.exchange(child_after_split);
                                                        child_after_split->role() = std::get<2>(bnode_before_split.keys).value()->role();
                                                    }
                                                    typename navigator_t::loop_t loop_outer_parent;
                                                    if(parent->role() != rb2p_node_role_e::root)
                                                        loop_outer_parent = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(parent);
                                                    typename navigator_t::loop_t loop_inner_parent = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(parent);
                                                    if(child_left_or_right_after_split_height == 0)
                                                    {
                                                        typename navigator_t::loop_t loop_inner_parent2;
                                                        if(!bnode_before_split.center_key_parent_info.is_left_or_right_child_of_parent)
                                                            loop_inner_parent2 = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_before_split.keys));
                                                        typename navigator_t::loop_t{
                                                            .this_ = std::get<2>(bnode_before_split.keys).value(),
                                                            .child_right = child_after_split,
                                                            .child_left = std::get<1>(bnode_before_split.keys),
                                                        }
                                                            .link();
                                                        if(!bnode_before_split.center_key_parent_info.is_left_or_right_child_of_parent)
                                                            loop_inner_parent2.exchange(std::get<2>(bnode_before_split.keys).value());
                                                        else
                                                            loop_inner_parent.child_right = std::get<2>(bnode_before_split.keys).value();
                                                        loop_inner_parent.this_ = std::get<0>(bnode_before_split.keys).value();
                                                        loop_inner_parent.leftmost_descendent_of_child_right = std::get<1>(bnode_before_split.keys);
                                                        loop_inner_parent.index = -1, loop_inner_parent.link();
                                                    }
                                                    else
                                                    {
                                                        loop_inner_parent.this_ = std::get<0>(bnode_before_split.keys).value();

                                                        typename navigator_t::loop_t loop_inner_parent2;
                                                        if(!bnode_before_split.center_key_parent_info.is_left_or_right_child_of_parent)
                                                            loop_inner_parent2 = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_before_split.keys));
                                                        typename navigator_t::loop_t loop_inner_left = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<0>(bnode_before_split.keys).value());
                                                        typename navigator_t::loop_t loop_inner_middle = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<1>(bnode_before_split.keys));
                                                        typename navigator_t::loop_t loop_inner_right = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<2>(bnode_before_split.keys).value());
                                                        navigator_except_node_end_t *key_left_child_left, *key_left_child_left_child_left, *key_left_child_left_child_right;
                                                        navigator_except_node_end_t *key_left_child_right, *key_left_child_right_child_left, *key_left_child_right_child_right;
                                                        navigator_except_node_end_t *key_right_child_left, *key_right_child_left_child_left, *key_right_child_left_child_right;
                                                        if(child_left_or_right_after_split_height == 1)
                                                        {
                                                            key_left_child_left = std::get<0>(bnode_before_split.keys).value()->child_left();
                                                            key_left_child_left_child_left = key_left_child_left->child_left();
                                                            key_left_child_left_child_right = key_left_child_left->child_right();
                                                            key_left_child_right = std::get<0>(bnode_before_split.keys).value()->child_right();
                                                            key_left_child_right_child_left = key_left_child_right->child_left();
                                                            key_left_child_right_child_right = key_left_child_right->child_right();
                                                            key_right_child_left = std::get<2>(bnode_before_split.keys).value()->child_left();
                                                            key_right_child_left_child_left = key_right_child_left->child_left();
                                                            key_right_child_left_child_right = key_right_child_left->child_right();
                                                        }
                                                        if(!bnode_before_split.center_key_parent_info.is_left_or_right_child_of_parent)
                                                            loop_inner_parent2.exchange(std::get<2>(bnode_before_split.keys).value());
                                                        else
                                                            loop_inner_parent.child_right = std::get<2>(bnode_before_split.keys).value();

                                                        typename navigator_t::loop_t{
                                                            .this_ = child_after_split,
                                                            .child_right = child_right_after_split,
                                                            .leftmost_descendent_of_child_right = child_right_after_split_leftmost_descendent,
                                                            .rightmost_descendent_of_child_left = child_left_after_split_rightmost_descendent,
                                                            .child_left = child_left_after_split,
                                                        }
                                                            .link();
                                                        if(child_left_or_right_after_split_height == 1)
                                                        {
                                                            child_left_after_split->role() = child_left_after_split->child_left() == nullptr ? rb2p_node_role_e::child_left_leftmost_descendent_of_non_root : rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                            if(child_right_after_split->role() != rb2p_node_role_e::child_right_rightmost_descendent_of_root)
                                                                child_right_after_split->role() = child_right_after_split->child_right() == nullptr ? rb2p_node_role_e::child_right_rightmost_descendent_of_non_root : rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                        }
                                                        else if(child_left_or_right_after_split_height > 1)
                                                        {
                                                            child_left_after_split->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                            child_right_after_split->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                        }

                                                        if(child_left_or_right_after_split_height == 1 && key_left_child_right_child_right != nullptr && key_left_child_right_child_left == nullptr)
                                                        {
                                                            navigator_except_node_end_t *node_left = key_left_child_right;
                                                            navigator_except_node_end_t *node_right = key_left_child_right_child_right;
                                                            typename navigator_t::loop_t{
                                                                .this_ = node_right,
                                                                .child_left = node_left,
                                                            }
                                                                .link();
                                                            node_left->color() = true;
                                                            node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                            node_right->color() = false;
                                                            node_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                            loop_inner_left.child_right = node_right;
                                                            loop_inner_left.leftmost_descendent_of_child_right = node_left;
                                                            move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                                        }
                                                        if(child_left_or_right_after_split_height == 1)
                                                            loop_inner_left.child_right->role() = loop_inner_left.child_right->child_left() == nullptr ? rb2p_node_role_e::child_left_leftmost_descendent_of_non_root : rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                        else if(child_left_or_right_after_split_height > 1)
                                                            loop_inner_left.child_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                        if(child_left_or_right_after_split_height == 1 && key_right_child_left_child_left != nullptr && key_right_child_left_child_right == nullptr)
                                                        {
                                                            navigator_except_node_end_t *node_right = key_right_child_left;
                                                            navigator_except_node_end_t *node_left = key_right_child_left_child_left;
                                                            typename navigator_t::loop_t{
                                                                .this_ = node_left,
                                                                .child_right = node_right,
                                                            }
                                                                .link();
                                                            node_right->color() = true;
                                                            node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                            node_left->color() = false;
                                                            node_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                            loop_inner_right.child_left = node_left;
                                                            loop_inner_right.rightmost_descendent_of_child_left = node_right;
                                                            move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                                        }
                                                        if(child_left_or_right_after_split_height == 1)
                                                            loop_inner_right.child_left->role() = loop_inner_right.child_left->child_right() == nullptr ? rb2p_node_role_e::child_right_rightmost_descendent_of_non_root : rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                        else if(child_left_or_right_after_split_height > 1)
                                                            loop_inner_right.child_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                        loop_inner_middle.child_right = loop_inner_right.child_left;
                                                        loop_inner_middle.child_left = loop_inner_left.child_right;
                                                        loop_inner_middle.index = -1, loop_inner_middle.link();

                                                        loop_inner_right.rightmost_descendent_of_child_left = loop_inner_right.rightmost_descendent_of_child_left != nullptr ? loop_inner_right.rightmost_descendent_of_child_left : loop_inner_right.child_left;
                                                        loop_inner_right.leftmost_descendent_of_child_right = loop_inner_right.leftmost_descendent_of_child_right != nullptr ? loop_inner_right.leftmost_descendent_of_child_right : child_left_after_split;
                                                        loop_inner_right.child_right = child_after_split;
                                                        loop_inner_right.child_left = std::get<1>(bnode_before_split.keys);
                                                        loop_inner_right.index = -1, loop_inner_right.link();

                                                        if(child_left_or_right_after_split_height == 1 && key_left_child_left_child_left != nullptr && key_left_child_left_child_right == nullptr)
                                                        {
                                                            navigator_except_node_end_t *node_right = key_left_child_left;
                                                            navigator_except_node_end_t *node_left = key_left_child_left_child_left;
                                                            typename navigator_t::loop_t{
                                                                .this_ = node_left,
                                                                .child_right = node_right,
                                                            }
                                                                .link();
                                                            node_right->color() = true;
                                                            node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                            node_left->color() = false;
                                                            node_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                            loop_inner_left.rightmost_descendent_of_child_left = node_right;
                                                            loop_inner_left.child_left = node_left;
                                                            move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                                        }
                                                        else if(child_left_or_right_after_split_height == 1 && key_left_child_left_child_left == nullptr && key_left_child_left_child_right == nullptr)
                                                            key_left_child_left->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                        if(child_left_or_right_after_split_height == 1)
                                                            loop_inner_left.child_left->role() = loop_inner_left.child_left->child_right() == nullptr ? rb2p_node_role_e::child_right_rightmost_descendent_of_non_root : rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                        else if(child_left_or_right_after_split_height > 1)
                                                            loop_inner_left.child_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                        child_right_after_split = loop_inner_left.child_left;
                                                        child_right_after_split_leftmost_descendent = loop_inner_parent.leftmost_descendent_of_child_right;
                                                        loop_inner_parent.leftmost_descendent_of_child_right = loop_inner_left.leftmost_descendent_of_child_right != nullptr ? loop_inner_left.leftmost_descendent_of_child_right : loop_inner_left.child_right;
                                                        child_left_after_split_rightmost_descendent = loop_inner_parent.rightmost_descendent_of_child_left;
                                                        loop_inner_parent.rightmost_descendent_of_child_left = loop_inner_left.rightmost_descendent_of_child_left != nullptr ? loop_inner_left.rightmost_descendent_of_child_left : loop_inner_left.child_left;

                                                        loop_inner_parent.index = -1, loop_inner_parent.link();

                                                        if(child_left_or_right_after_split_height == 1 && bnode_left_sibling_rightmost_child->child_right() != nullptr && bnode_left_sibling_rightmost_child->child_left() == nullptr)
                                                        {
                                                            navigator_except_node_end_t *node_left = bnode_left_sibling_rightmost_child;
                                                            navigator_except_node_end_t *node_right = node_left->child_right();
                                                            typename navigator_t::loop_t loop_outer_sibling = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(node_left);
                                                            loop_outer_sibling.child_right = node_right;
                                                            loop_outer_sibling.leftmost_descendent_of_child_right = node_left;
                                                            loop_outer_sibling.index = -1, loop_outer_sibling.link();
                                                            typename navigator_t::loop_t{
                                                                .this_ = node_right,
                                                                .child_left = node_left,
                                                            }
                                                                .link();
                                                            node_left->color() = true;
                                                            node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                            node_right->color() = false;
                                                            node_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                            child_left_after_split = node_right;
                                                            move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                                        }
                                                        else
                                                            child_left_after_split = bnode_left_sibling_rightmost_child;
                                                    }
                                                    if(parent->role() != rb2p_node_role_e::root)
                                                        loop_outer_parent.exchange(std::get<0>(bnode_before_split.keys).value());
                                                    else
                                                    {
                                                        loop_end.root = std::get<0>(bnode_before_split.keys).value();
                                                        should_link_loop_end = true;
                                                    }
                                                    child_after_split->color() = std::get<2>(bnode_before_split.keys).value()->color();
                                                    child_after_split->role() = std::get<2>(bnode_before_split.keys).value()->role();
                                                    std::get<2>(bnode_before_split.keys).value()->color() = std::get<1>(bnode_before_split.keys)->color();
                                                    std::get<2>(bnode_before_split.keys).value()->role() = std::get<1>(bnode_before_split.keys)->role();
                                                    std::get<1>(bnode_before_split.keys)->color() = std::get<0>(bnode_before_split.keys).value()->color();
                                                    std::get<1>(bnode_before_split.keys)->role() = std::get<0>(bnode_before_split.keys).value()->role();
                                                    std::get<0>(bnode_before_split.keys).value()->color() = parent->color();
                                                    std::get<0>(bnode_before_split.keys).value()->role() = parent->role();
                                                    child_after_split = parent;

                                                    if(child_left_or_right_after_split_height == 0)
                                                    {
                                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, child_after_split_old);
                                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<2>(bnode_before_split.keys).value());
                                                    }
                                                    else
                                                    {
                                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, child_after_split_old);
                                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                                        refresh_node_count_and_accumulated_storage_and_above_until(schedules, accumulator, std::get<2>(bnode_before_split.keys).value(), std::get<0>(bnode_before_split.keys).value());
                                                    }
                                                }
                                                else std::unreachable();
                                                bnode_before_split = bnode_left_sibling;
                                            }
                                            else
                                                fallback();
                                        }
                                        else
                                            fallback();
                                    };
                                };
                                auto try_spill_to_right_sibling = [&](auto fallback)
                                {
                                    return [&, fallback]()
                                    {
                                        if(std::optional<navigator_except_node_end_t *> opt_key_black_right_sibling = bnode_parent.opt_child_at(bnode_parent.child_index + 2); opt_key_black_right_sibling.has_value())
                                        {
                                            auto [bnode_right_sibling, bnode_right_sibling_leftmost_child] = bnode_up_t::inserting_get_bnode_from_key_black_sibling(opt_key_black_right_sibling.value(), true);
                                            if(bnode_right_sibling.key_count != 3)
                                            {
                                                navigator_except_node_end_t *parent = bnode_parent.key_at(bnode_parent.child_index + 1);
                                                if(bnode_before_split.child_index == 3)
                                                {
                                                    typename navigator_t::loop_t loop_outer_parent;
                                                    if(parent->role() != rb2p_node_role_e::root)
                                                        loop_outer_parent = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(parent);
                                                    typename navigator_t::loop_t loop_inner_parent = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(parent);
                                                    if(child_left_or_right_after_split_height == 0)
                                                        loop_inner_parent.exchange(child_after_split);
                                                    else
                                                    {
                                                        loop_inner_parent.this_ = child_after_split;

                                                        typename navigator_t::loop_t loop_inner_left = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<2>(bnode_before_split.keys).value());
                                                        if(child_left_or_right_after_split_height == 1 && child_left_after_split->child_left() != nullptr && child_left_after_split->child_right() == nullptr)
                                                        {
                                                            navigator_except_node_end_t *node_right = child_left_after_split;
                                                            navigator_except_node_end_t *node_left = node_right->child_left();
                                                            typename navigator_t::loop_t{
                                                                .this_ = node_left,
                                                                .child_right = node_right,
                                                            }
                                                                .link();
                                                            loop_inner_left.child_right = node_left;
                                                            loop_inner_left.leftmost_descendent_of_child_right = nullptr;
                                                            loop_inner_left.index = -1, loop_inner_left.link();
                                                            node_right->color() = true;
                                                            node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                            node_left->color() = false;
                                                            node_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                            std::tie(child_left_after_split, child_left_after_split_rightmost_descendent) = std::make_tuple(node_left, node_right);
                                                            move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                                        }
                                                        else
                                                        {
                                                            loop_inner_left.child_right = child_left_after_split;
                                                            loop_inner_left.index = -1, loop_inner_left.link();
                                                        }
                                                        if(child_left_or_right_after_split_height == 1)
                                                        {
                                                            child_left_after_split->role() = child_left_after_split->child_right() == nullptr ? rb2p_node_role_e::child_right_rightmost_descendent_of_non_root : rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                        }
                                                        else if(child_left_or_right_after_split_height > 1)
                                                        {
                                                            child_left_after_split->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                        }
                                                        if(child_left_after_split_rightmost_descendent != nullptr)
                                                            std::ranges::swap(loop_inner_parent.rightmost_descendent_of_child_left, child_left_after_split_rightmost_descendent);
                                                        else
                                                            child_left_after_split_rightmost_descendent = std::exchange(loop_inner_parent.rightmost_descendent_of_child_left, child_left_after_split);

                                                        if(child_left_or_right_after_split_height == 1 && child_right_after_split->child_right() != nullptr && child_right_after_split->child_left() == nullptr)
                                                        {
                                                            navigator_except_node_end_t *node_left = child_right_after_split;
                                                            navigator_except_node_end_t *node_right = node_left->child_right();
                                                            typename navigator_t::loop_t{
                                                                .this_ = node_right,
                                                                .child_left = node_left,
                                                            }
                                                                .link();
                                                            node_left->color() = true;
                                                            node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                            node_right->color() = false;
                                                            node_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                            std::tie(child_right_after_split, child_right_after_split_leftmost_descendent) = std::make_tuple(node_right, node_left);
                                                            move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                                        }
                                                        else if(child_left_or_right_after_split_height == 1 && child_right_after_split->child_right() == nullptr && child_right_after_split->child_left() == nullptr)
                                                        {
                                                            child_right_after_split->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                            child_right_after_split->next() = nullptr;
                                                        }
                                                        child_left_after_split = child_right_after_split;
                                                        if(child_right_after_split_leftmost_descendent != nullptr)
                                                            std::ranges::swap(loop_inner_parent.leftmost_descendent_of_child_right, child_right_after_split_leftmost_descendent);
                                                        else
                                                            child_right_after_split_leftmost_descendent = std::exchange(loop_inner_parent.leftmost_descendent_of_child_right, child_right_after_split);

                                                        loop_inner_parent.index = -1, loop_inner_parent.link();

                                                        if(child_left_or_right_after_split_height == 1 && bnode_right_sibling_leftmost_child->child_left() != nullptr && bnode_right_sibling_leftmost_child->child_right() == nullptr)
                                                        {
                                                            navigator_except_node_end_t *node_right = bnode_right_sibling_leftmost_child;
                                                            navigator_except_node_end_t *node_left = node_right->child_left();
                                                            typename navigator_t::loop_t loop_outer_sibling = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(node_right);
                                                            loop_outer_sibling.child_left = node_left;
                                                            loop_outer_sibling.rightmost_descendent_of_child_left = node_right;
                                                            loop_outer_sibling.index = -1, loop_outer_sibling.link();
                                                            typename navigator_t::loop_t{
                                                                .this_ = node_left,
                                                                .child_right = node_right,
                                                            }
                                                                .link();
                                                            node_right->color() = true;
                                                            node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                            node_left->color() = false;
                                                            node_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                            child_right_after_split = node_left;
                                                            move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                                        }
                                                        else
                                                            child_right_after_split = bnode_right_sibling_leftmost_child;
                                                    }
                                                    if(parent->role() != rb2p_node_role_e::root)
                                                        loop_outer_parent.exchange(child_after_split);
                                                    else
                                                    {
                                                        loop_end.root = child_after_split;
                                                        assert(!should_link_loop_end);
                                                        should_link_loop_end = true;
                                                    }
                                                    child_after_split->color() = parent->color();
                                                    child_after_split->role() = parent->role();
                                                    child_after_split = parent;

                                                    if(child_left_or_right_after_split_height == 0)
                                                        ;
                                                    else
                                                        refresh_node_count_and_accumulated_storage_and_above_until(schedules, accumulator, std::get<2>(bnode_before_split.keys).value(), child_after_split_old);
                                                }
                                                else if(bnode_before_split.child_index == 1)
                                                {
                                                    typename navigator_t::loop_t loop_outer_parent;
                                                    if(parent->role() != rb2p_node_role_e::root)
                                                        loop_outer_parent = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(parent);
                                                    typename navigator_t::loop_t loop_inner_parent = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(parent);
                                                    if(child_left_or_right_after_split_height == 0)
                                                    {
                                                        typename navigator_t::loop_t{
                                                            .this_ = std::get<1>(bnode_before_split.keys),
                                                            .child_right = child_after_split,
                                                            .child_left = std::get<0>(bnode_before_split.keys).value(),
                                                        }
                                                            .link();
                                                        loop_inner_parent.this_ = std::get<2>(bnode_before_split.keys).value();
                                                        loop_inner_parent.rightmost_descendent_of_child_left = child_after_split;
                                                        loop_inner_parent.index = -1, loop_inner_parent.link();
                                                    }
                                                    else
                                                    {
                                                        loop_inner_parent.this_ = std::get<2>(bnode_before_split.keys).value();

                                                        typename navigator_t::loop_t loop_inner_right = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<2>(bnode_before_split.keys).value());
                                                        typename navigator_t::loop_t loop_inner_left = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<2>(bnode_before_split.keys).value());
                                                        navigator_except_node_end_t *key_right_child_right, *key_right_child_right_child_right, *key_right_child_right_child_left;
                                                        if(child_left_or_right_after_split_height == 1)
                                                        {
                                                            key_right_child_right = std::get<2>(bnode_before_split.keys).value()->child_right();
                                                            key_right_child_right_child_right = key_right_child_right->child_right();
                                                            key_right_child_right_child_left = key_right_child_right->child_left();
                                                        }
                                                        loop_inner_left.exchange(child_after_split);
                                                        typename navigator_t::loop_t{
                                                            .this_ = child_after_split,
                                                            .child_right = child_right_after_split,
                                                            .leftmost_descendent_of_child_right = child_right_after_split_leftmost_descendent,
                                                            .rightmost_descendent_of_child_left = child_left_after_split_rightmost_descendent,
                                                            .child_left = child_left_after_split,
                                                        }
                                                            .link();
                                                        if(child_left_or_right_after_split_height == 1)
                                                        {
                                                            child_right_after_split->role() = child_right_after_split->child_right() == nullptr ? rb2p_node_role_e::child_right_rightmost_descendent_of_non_root : rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                            child_left_after_split->role() = child_left_after_split->child_left() == nullptr ? rb2p_node_role_e::child_left_leftmost_descendent_of_non_root : rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                        }
                                                        else if(child_left_or_right_after_split_height > 1)
                                                        {
                                                            child_right_after_split->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                            child_left_after_split->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                        }
                                                        child_left_after_split_rightmost_descendent = loop_inner_parent.rightmost_descendent_of_child_left;
                                                        loop_inner_parent.rightmost_descendent_of_child_left = loop_inner_right.rightmost_descendent_of_child_left != nullptr ? loop_inner_right.rightmost_descendent_of_child_left : loop_inner_right.child_left;

                                                        if(child_left_or_right_after_split_height == 1 && key_right_child_right_child_right != nullptr && key_right_child_right_child_left == nullptr)
                                                        {
                                                            navigator_except_node_end_t *node_left = key_right_child_right;
                                                            navigator_except_node_end_t *node_right = key_right_child_right_child_right;
                                                            typename navigator_t::loop_t{
                                                                .this_ = node_right,
                                                                .child_left = node_left,
                                                            }
                                                                .link();
                                                            node_left->color() = true;
                                                            node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                            node_right->color() = false;
                                                            node_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                            loop_inner_right.leftmost_descendent_of_child_right = node_left;
                                                            loop_inner_right.child_right = node_right;
                                                            move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                                        }
                                                        else if(child_left_or_right_after_split_height == 1 && key_right_child_right_child_right == nullptr && key_right_child_right_child_left == nullptr)
                                                        {
                                                            key_right_child_right->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                            key_right_child_right->next() = nullptr;
                                                        }
                                                        child_right_after_split_leftmost_descendent = loop_inner_parent.leftmost_descendent_of_child_right;
                                                        loop_inner_parent.leftmost_descendent_of_child_right = loop_inner_left.leftmost_descendent_of_child_right != nullptr ? loop_inner_right.leftmost_descendent_of_child_right : loop_inner_right.child_right;
                                                        child_left_after_split = loop_inner_right.child_right;

                                                        loop_inner_parent.index = -1, loop_inner_parent.link();

                                                        if(child_left_or_right_after_split_height == 1 && bnode_right_sibling_leftmost_child->child_left() != nullptr && bnode_right_sibling_leftmost_child->child_right() == nullptr)
                                                        {
                                                            navigator_except_node_end_t *node_right = bnode_right_sibling_leftmost_child;
                                                            navigator_except_node_end_t *node_left = node_right->child_left();
                                                            typename navigator_t::loop_t loop_outer_sibling = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(node_right);
                                                            loop_outer_sibling.child_left = node_left;
                                                            loop_outer_sibling.rightmost_descendent_of_child_left = node_right;
                                                            loop_outer_sibling.index = -1, loop_outer_sibling.link();
                                                            typename navigator_t::loop_t{
                                                                .this_ = node_left,
                                                                .child_right = node_right,
                                                            }
                                                                .link();
                                                            node_right->color() = true;
                                                            node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                            node_left->color() = false;
                                                            node_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                            child_right_after_split = node_left;
                                                            move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                                        }
                                                        else
                                                            child_right_after_split = bnode_right_sibling_leftmost_child;
                                                    }
                                                    if(parent->role() != rb2p_node_role_e::root)
                                                        loop_outer_parent.exchange(std::get<2>(bnode_before_split.keys).value());
                                                    else
                                                    {
                                                        loop_end.root = std::get<2>(bnode_before_split.keys).value();
                                                        assert(!should_link_loop_end);
                                                        should_link_loop_end = true;
                                                    }
                                                    child_after_split->color() = std::get<2>(bnode_before_split.keys).value()->color();
                                                    child_after_split->role() = std::get<2>(bnode_before_split.keys).value()->role();
                                                    std::get<2>(bnode_before_split.keys).value()->color() = parent->color();
                                                    std::get<2>(bnode_before_split.keys).value()->role() = parent->role();
                                                    child_after_split = parent;

                                                    if(child_left_or_right_after_split_height == 0)
                                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, child_after_split_old);
                                                    else
                                                        refresh_node_count_and_accumulated_storage_and_above_until(schedules, accumulator, child_after_split_old, std::get<2>(bnode_before_split.keys).value());
                                                }
                                                else if(bnode_before_split.child_index == -1)
                                                {
                                                    typename navigator_t::loop_t loop_outer_parent;
                                                    if(parent->role() != rb2p_node_role_e::root)
                                                        loop_outer_parent = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(parent);
                                                    typename navigator_t::loop_t loop_inner_parent = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(parent);
                                                    if(child_left_or_right_after_split_height == 0)
                                                    {
                                                        typename navigator_t::loop_t loop_inner_parent2;
                                                        if(bnode_before_split.center_key_parent_info.is_left_or_right_child_of_parent)
                                                            loop_inner_parent2 = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_before_split.keys));
                                                        typename navigator_t::loop_t{
                                                            .this_ = child_after_split,
                                                            .child_right = std::get<1>(bnode_before_split.keys),
                                                            .child_left = std::get<0>(bnode_before_split.keys).value(),
                                                        }
                                                            .link();
                                                        if(bnode_before_split.center_key_parent_info.is_left_or_right_child_of_parent)
                                                            loop_inner_parent2.exchange(child_after_split);
                                                        else
                                                            loop_inner_parent.child_left = child_after_split;
                                                        loop_inner_parent.this_ = std::get<2>(bnode_before_split.keys).value();
                                                        loop_inner_parent.rightmost_descendent_of_child_left = std::get<1>(bnode_before_split.keys);
                                                        loop_inner_parent.index = -1, loop_inner_parent.link();
                                                    }
                                                    else
                                                    {
                                                        loop_inner_parent.this_ = std::get<2>(bnode_before_split.keys).value();

                                                        typename navigator_t::loop_t loop_inner_parent2;
                                                        if(bnode_before_split.center_key_parent_info.is_left_or_right_child_of_parent)
                                                            loop_inner_parent2 = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_before_split.keys));
                                                        typename navigator_t::loop_t loop_inner_right = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<2>(bnode_before_split.keys).value());
                                                        typename navigator_t::loop_t loop_inner_middle = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<1>(bnode_before_split.keys));
                                                        typename navigator_t::loop_t loop_inner_left = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<0>(bnode_before_split.keys).value());
                                                        navigator_except_node_end_t *key_right_child_right, *key_right_child_right_child_right, *key_right_child_right_child_left;
                                                        navigator_except_node_end_t *key_right_child_left, *key_right_child_left_child_right, *key_right_child_left_child_left;
                                                        if(child_left_or_right_after_split_height == 1)
                                                        {
                                                            key_right_child_right = std::get<2>(bnode_before_split.keys).value()->child_right();
                                                            key_right_child_right_child_right = key_right_child_right->child_right();
                                                            key_right_child_right_child_left = key_right_child_right->child_left();
                                                            key_right_child_left = std::get<2>(bnode_before_split.keys).value()->child_left();
                                                            key_right_child_left_child_right = key_right_child_left->child_right();
                                                            key_right_child_left_child_left = key_right_child_left->child_left();
                                                        }
                                                        if(bnode_before_split.center_key_parent_info.is_left_or_right_child_of_parent)
                                                            loop_inner_parent2.exchange(child_after_split);
                                                        else
                                                            loop_inner_parent.child_left = child_after_split;

                                                        if(child_left_or_right_after_split_height == 1 && child_left_after_split->child_left() != nullptr && child_left_after_split->child_right() == nullptr)
                                                        {
                                                            navigator_except_node_end_t *node_right = child_left_after_split;
                                                            navigator_except_node_end_t *node_left = node_right->child_left();
                                                            typename navigator_t::loop_t{
                                                                .this_ = node_left,
                                                                .child_right = node_right,
                                                            }
                                                                .link();
                                                            node_right->color() = true;
                                                            node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                            node_left->color() = false;
                                                            node_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                            std::tie(child_left_after_split, child_left_after_split_rightmost_descendent) = std::make_tuple(node_left, node_right);
                                                            move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                                        }
                                                        loop_inner_left.child_right = child_left_after_split;
                                                        loop_inner_left.index = -1, loop_inner_left.link();
                                                        if(child_left_or_right_after_split_height == 1)
                                                        {
                                                            child_left_after_split->role() = child_left_after_split->child_right() == nullptr ? rb2p_node_role_e::child_right_rightmost_descendent_of_non_root : rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                        }
                                                        else if(child_left_or_right_after_split_height > 1)
                                                        {
                                                            child_left_after_split->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                        }

                                                        if(child_left_or_right_after_split_height == 1 && child_right_after_split->child_right() != nullptr && child_right_after_split->child_left() == nullptr)
                                                        {
                                                            navigator_except_node_end_t *node_left = child_right_after_split;
                                                            navigator_except_node_end_t *node_right = node_left->child_right();
                                                            typename navigator_t::loop_t{
                                                                .this_ = node_right,
                                                                .child_left = node_left,
                                                            }
                                                                .link();
                                                            node_left->color() = true;
                                                            node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                            node_right->color() = false;
                                                            node_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                            std::tie(child_right_after_split, child_right_after_split_leftmost_descendent) = std::make_tuple(node_right, node_left);
                                                            move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                                        }
                                                        typename navigator_t::loop_t{
                                                            .this_ = child_after_split,
                                                            .child_right = std::get<1>(bnode_before_split.keys),
                                                            .leftmost_descendent_of_child_right = child_right_after_split_leftmost_descendent != nullptr ? child_right_after_split_leftmost_descendent : child_right_after_split,
                                                            .rightmost_descendent_of_child_left = child_left_after_split_rightmost_descendent != nullptr ? child_left_after_split_rightmost_descendent : child_left_after_split,
                                                            .child_left = std::get<0>(bnode_before_split.keys).value(),
                                                        }
                                                            .link();
                                                        if(child_left_or_right_after_split_height == 1)
                                                        {
                                                            child_right_after_split->role() = child_right_after_split->child_left() == nullptr ? rb2p_node_role_e::child_left_leftmost_descendent_of_non_root : rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                        }
                                                        else if(child_left_or_right_after_split_height > 1)
                                                        {
                                                            child_right_after_split->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                        }

                                                        if(child_left_or_right_after_split_height == 1 && key_right_child_left_child_left != nullptr && key_right_child_left_child_right == nullptr)
                                                        {
                                                            navigator_except_node_end_t *node_right = key_right_child_left;
                                                            navigator_except_node_end_t *node_left = key_right_child_left_child_left;
                                                            typename navigator_t::loop_t{
                                                                .this_ = node_left,
                                                                .child_right = node_right,
                                                            }
                                                                .link();
                                                            node_right->color() = true;
                                                            node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                            node_left->color() = false;
                                                            node_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                            loop_inner_right.child_left = node_left;
                                                            loop_inner_right.rightmost_descendent_of_child_left = node_right;
                                                            move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                                        }
                                                        loop_inner_middle.child_left = child_right_after_split;
                                                        loop_inner_middle.child_right = loop_inner_right.child_left;
                                                        loop_inner_middle.index = -1, loop_inner_middle.link();
                                                        if(child_left_or_right_after_split_height == 1)
                                                        {
                                                            loop_inner_right.child_left->role() = loop_inner_right.child_left->child_right() == nullptr ? rb2p_node_role_e::child_right_rightmost_descendent_of_non_root : rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                        }
                                                        else if(child_left_or_right_after_split_height > 1)
                                                        {
                                                            loop_inner_right.child_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                        }

                                                        if(child_left_or_right_after_split_height == 1 && key_right_child_right_child_right != nullptr && key_right_child_right_child_left == nullptr)
                                                        {
                                                            navigator_except_node_end_t *node_left = key_right_child_right;
                                                            navigator_except_node_end_t *node_right = key_right_child_right_child_right;
                                                            typename navigator_t::loop_t{
                                                                .this_ = node_right,
                                                                .child_left = node_left,
                                                            }
                                                                .link();
                                                            node_left->color() = true;
                                                            node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                            node_right->color() = false;
                                                            node_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                            loop_inner_right.leftmost_descendent_of_child_right = node_left;
                                                            loop_inner_right.child_right = node_right;
                                                            move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                                        }
                                                        else if(child_left_or_right_after_split_height == 1 && key_right_child_right_child_right == nullptr && key_right_child_right_child_left == nullptr)
                                                        {
                                                            key_right_child_right->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                            key_right_child_right->next() = nullptr;
                                                        }
                                                        child_left_after_split = loop_inner_right.child_right;
                                                        child_left_after_split_rightmost_descendent = loop_inner_parent.rightmost_descendent_of_child_left;
                                                        loop_inner_parent.rightmost_descendent_of_child_left = loop_inner_right.rightmost_descendent_of_child_left != nullptr ? loop_inner_right.rightmost_descendent_of_child_left : loop_inner_right.child_left;
                                                        child_right_after_split_leftmost_descendent = loop_inner_parent.leftmost_descendent_of_child_right;
                                                        loop_inner_parent.leftmost_descendent_of_child_right = loop_inner_right.leftmost_descendent_of_child_right != nullptr ? loop_inner_right.leftmost_descendent_of_child_right : loop_inner_right.child_right;

                                                        loop_inner_parent.index = -1, loop_inner_parent.link();

                                                        if(child_left_or_right_after_split_height == 1 && bnode_right_sibling_leftmost_child->child_left() != nullptr && bnode_right_sibling_leftmost_child->child_right() == nullptr)
                                                        {
                                                            navigator_except_node_end_t *node_right = bnode_right_sibling_leftmost_child;
                                                            navigator_except_node_end_t *node_left = node_right->child_left();
                                                            typename navigator_t::loop_t loop_outer_sibling = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(node_right);
                                                            loop_outer_sibling.child_left = node_left;
                                                            loop_outer_sibling.rightmost_descendent_of_child_left = node_right;
                                                            loop_outer_sibling.index = -1, loop_outer_sibling.link();
                                                            typename navigator_t::loop_t{
                                                                .this_ = node_left,
                                                                .child_right = node_right,
                                                            }
                                                                .link();
                                                            node_right->color() = true;
                                                            node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                            node_left->color() = false;
                                                            node_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                            child_right_after_split = node_left;
                                                            move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                                        }
                                                        else
                                                            child_right_after_split = bnode_right_sibling_leftmost_child;
                                                    }
                                                    if(parent->role() != rb2p_node_role_e::root)
                                                        loop_outer_parent.exchange(std::get<2>(bnode_before_split.keys).value());
                                                    else
                                                    {
                                                        loop_end.root = std::get<2>(bnode_before_split.keys).value();
                                                        assert(!should_link_loop_end);
                                                        should_link_loop_end = true;
                                                    }
                                                    child_after_split->color() = std::get<1>(bnode_before_split.keys)->color();
                                                    child_after_split->role() = std::get<1>(bnode_before_split.keys)->role();
                                                    std::get<1>(bnode_before_split.keys)->color() = std::get<2>(bnode_before_split.keys).value()->color();
                                                    std::get<1>(bnode_before_split.keys)->role() = std::get<2>(bnode_before_split.keys).value()->role();
                                                    std::get<2>(bnode_before_split.keys).value()->color() = parent->color();
                                                    std::get<2>(bnode_before_split.keys).value()->role() = parent->role();
                                                    child_after_split = parent;

                                                    if(child_left_or_right_after_split_height == 0)
                                                    {
                                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, child_after_split_old);
                                                    }
                                                    else
                                                    {
                                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<0>(bnode_before_split.keys).value());
                                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                                        refresh_node_count_and_accumulated_storage_and_above_until(schedules, accumulator, child_after_split_old, std::get<2>(bnode_before_split.keys).value());
                                                    }
                                                }
                                                else if(bnode_before_split.child_index == -3)
                                                {
                                                    if(child_left_or_right_after_split_height == 0 && std::get<0>(bnode_before_split.keys).value()->role() != rb2p_node_role_e::child_left_leftmost_descendent_of_root)
                                                    {
                                                        typename navigator_t::loop_t loop_outer_parent = navigator_t::template get_loop<navigator_t::loop_type_e::ancestor>(std::get<0>(bnode_before_split.keys).value());
                                                        loop_outer_parent.exchange(child_after_split);
                                                        child_after_split->role() = std::get<0>(bnode_before_split.keys).value()->role();
                                                    }
                                                    typename navigator_t::loop_t loop_outer_parent;
                                                    if(parent->role() != rb2p_node_role_e::root)
                                                        loop_outer_parent = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(parent);
                                                    typename navigator_t::loop_t loop_inner_parent = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(parent);
                                                    if(child_left_or_right_after_split_height == 0)
                                                    {
                                                        typename navigator_t::loop_t loop_inner_parent2;
                                                        if(bnode_before_split.center_key_parent_info.is_left_or_right_child_of_parent)
                                                            loop_inner_parent2 = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_before_split.keys));
                                                        typename navigator_t::loop_t{
                                                            .this_ = std::get<0>(bnode_before_split.keys).value(),
                                                            .child_right = std::get<1>(bnode_before_split.keys),
                                                            .child_left = child_after_split,
                                                        }
                                                            .link();
                                                        if(bnode_before_split.center_key_parent_info.is_left_or_right_child_of_parent)
                                                            loop_inner_parent2.exchange(std::get<0>(bnode_before_split.keys).value());
                                                        else
                                                            loop_inner_parent.child_left = std::get<0>(bnode_before_split.keys).value();
                                                        loop_inner_parent.this_ = std::get<2>(bnode_before_split.keys).value();
                                                        loop_inner_parent.rightmost_descendent_of_child_left = std::get<1>(bnode_before_split.keys);
                                                        loop_inner_parent.index = -1, loop_inner_parent.link();
                                                    }
                                                    else
                                                    {
                                                        loop_inner_parent.this_ = std::get<2>(bnode_before_split.keys).value();

                                                        typename navigator_t::loop_t loop_inner_parent2;
                                                        if(bnode_before_split.center_key_parent_info.is_left_or_right_child_of_parent)
                                                            loop_inner_parent2 = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<1>(bnode_before_split.keys));
                                                        typename navigator_t::loop_t loop_inner_right = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<2>(bnode_before_split.keys).value());
                                                        typename navigator_t::loop_t loop_inner_middle = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<1>(bnode_before_split.keys));
                                                        typename navigator_t::loop_t loop_inner_left = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<0>(bnode_before_split.keys).value());
                                                        navigator_except_node_end_t *key_right_child_right, *key_right_child_right_child_right, *key_right_child_right_child_left;
                                                        navigator_except_node_end_t *key_right_child_left, *key_right_child_left_child_right, *key_right_child_left_child_left;
                                                        navigator_except_node_end_t *key_left_child_right, *key_left_child_right_child_right, *key_left_child_right_child_left;
                                                        if(child_left_or_right_after_split_height == 1)
                                                        {
                                                            key_right_child_right = std::get<2>(bnode_before_split.keys).value()->child_right();
                                                            key_right_child_right_child_right = key_right_child_right->child_right();
                                                            key_right_child_right_child_left = key_right_child_right->child_left();
                                                            key_right_child_left = std::get<2>(bnode_before_split.keys).value()->child_left();
                                                            key_right_child_left_child_right = key_right_child_left->child_right();
                                                            key_right_child_left_child_left = key_right_child_left->child_left();
                                                            key_left_child_right = std::get<0>(bnode_before_split.keys).value()->child_right();
                                                            key_left_child_right_child_right = key_left_child_right->child_right();
                                                            key_left_child_right_child_left = key_left_child_right->child_left();
                                                        }
                                                        if(bnode_before_split.center_key_parent_info.is_left_or_right_child_of_parent)
                                                            loop_inner_parent2.exchange(std::get<0>(bnode_before_split.keys).value());
                                                        else
                                                            loop_inner_parent.child_left = std::get<0>(bnode_before_split.keys).value();

                                                        typename navigator_t::loop_t{
                                                            .this_ = child_after_split,
                                                            .child_right = child_right_after_split,
                                                            .leftmost_descendent_of_child_right = child_right_after_split_leftmost_descendent,
                                                            .rightmost_descendent_of_child_left = child_left_after_split_rightmost_descendent,
                                                            .child_left = child_left_after_split,
                                                        }
                                                            .link();
                                                        if(child_left_or_right_after_split_height == 1)
                                                        {
                                                            child_right_after_split->role() = child_right_after_split->child_right() == nullptr ? rb2p_node_role_e::child_right_rightmost_descendent_of_non_root : rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                            if(child_left_after_split->role() != rb2p_node_role_e::child_left_leftmost_descendent_of_root)
                                                                child_left_after_split->role() = child_left_after_split->child_left() == nullptr ? rb2p_node_role_e::child_left_leftmost_descendent_of_non_root : rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                        }
                                                        else if(child_left_or_right_after_split_height > 1)
                                                        {
                                                            child_right_after_split->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                            child_left_after_split->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                        }

                                                        if(child_left_or_right_after_split_height == 1 && key_right_child_left_child_left != nullptr && key_right_child_left_child_right == nullptr)
                                                        {
                                                            navigator_except_node_end_t *node_right = key_right_child_left;
                                                            navigator_except_node_end_t *node_left = key_right_child_left_child_left;
                                                            typename navigator_t::loop_t{
                                                                .this_ = node_left,
                                                                .child_right = node_right,
                                                            }
                                                                .link();
                                                            node_right->color() = true;
                                                            node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                            node_left->color() = false;
                                                            node_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                            loop_inner_right.child_left = node_left;
                                                            loop_inner_right.rightmost_descendent_of_child_left = node_right;
                                                            move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                                        }
                                                        if(child_left_or_right_after_split_height == 1)
                                                            loop_inner_right.child_left->role() = loop_inner_right.child_left->child_right() == nullptr ? rb2p_node_role_e::child_right_rightmost_descendent_of_non_root : rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                        else if(child_left_or_right_after_split_height > 1)
                                                            loop_inner_right.child_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                        if(child_left_or_right_after_split_height == 1 && key_left_child_right_child_right != nullptr && key_left_child_right_child_left == nullptr)
                                                        {
                                                            navigator_except_node_end_t *node_left = key_left_child_right;
                                                            navigator_except_node_end_t *node_right = key_left_child_right_child_right;
                                                            typename navigator_t::loop_t{
                                                                .this_ = node_right,
                                                                .child_left = node_left,
                                                            }
                                                                .link();
                                                            node_left->color() = true;
                                                            node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                            node_right->color() = false;
                                                            node_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                            loop_inner_left.child_right = node_right;
                                                            loop_inner_left.leftmost_descendent_of_child_right = node_left;
                                                            move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                                        }
                                                        if(child_left_or_right_after_split_height == 1)
                                                            loop_inner_left.child_right->role() = loop_inner_left.child_right->child_left() == nullptr ? rb2p_node_role_e::child_left_leftmost_descendent_of_non_root : rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                        else if(child_left_or_right_after_split_height > 1)
                                                            loop_inner_left.child_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                        loop_inner_middle.child_left = loop_inner_left.child_right;
                                                        loop_inner_middle.child_right = loop_inner_right.child_left;
                                                        loop_inner_middle.index = -1, loop_inner_middle.link();

                                                        loop_inner_left.leftmost_descendent_of_child_right = loop_inner_left.leftmost_descendent_of_child_right != nullptr ? loop_inner_left.leftmost_descendent_of_child_right : loop_inner_left.child_right;
                                                        loop_inner_left.rightmost_descendent_of_child_left = loop_inner_left.rightmost_descendent_of_child_left != nullptr ? loop_inner_left.rightmost_descendent_of_child_left : child_right_after_split;
                                                        loop_inner_left.child_left = child_after_split;
                                                        loop_inner_left.child_right = std::get<1>(bnode_before_split.keys);
                                                        loop_inner_left.index = -1, loop_inner_left.link();

                                                        if(child_left_or_right_after_split_height == 1 && key_right_child_right_child_right != nullptr && key_right_child_right_child_left == nullptr)
                                                        {
                                                            navigator_except_node_end_t *node_left = key_right_child_right;
                                                            navigator_except_node_end_t *node_right = key_right_child_right_child_right;
                                                            typename navigator_t::loop_t{
                                                                .this_ = node_right,
                                                                .child_left = node_left,
                                                            }
                                                                .link();
                                                            node_left->color() = true;
                                                            node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                            node_right->color() = false;
                                                            node_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                            loop_inner_right.leftmost_descendent_of_child_right = node_left;
                                                            loop_inner_right.child_right = node_right;
                                                            move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                                        }
                                                        else if(child_left_or_right_after_split_height == 1 && key_right_child_right_child_right == nullptr && key_right_child_right_child_left == nullptr)
                                                        {
                                                            key_right_child_right->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                            key_right_child_right->next() = nullptr;
                                                        }
                                                        if(child_left_or_right_after_split_height == 1)
                                                            loop_inner_right.child_right->role() = loop_inner_right.child_right->child_left() == nullptr ? rb2p_node_role_e::child_left_leftmost_descendent_of_non_root : rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                        else if(child_left_or_right_after_split_height > 1)
                                                            loop_inner_right.child_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                        child_left_after_split = loop_inner_right.child_right;
                                                        child_left_after_split_rightmost_descendent = loop_inner_parent.rightmost_descendent_of_child_left;
                                                        loop_inner_parent.rightmost_descendent_of_child_left = loop_inner_right.rightmost_descendent_of_child_left != nullptr ? loop_inner_right.rightmost_descendent_of_child_left : loop_inner_right.child_left;
                                                        child_right_after_split_leftmost_descendent = loop_inner_parent.leftmost_descendent_of_child_right;
                                                        loop_inner_parent.leftmost_descendent_of_child_right = loop_inner_right.leftmost_descendent_of_child_right != nullptr ? loop_inner_right.leftmost_descendent_of_child_right : loop_inner_right.child_right;

                                                        loop_inner_parent.index = -1, loop_inner_parent.link();

                                                        if(child_left_or_right_after_split_height == 1 && bnode_right_sibling_leftmost_child->child_left() != nullptr && bnode_right_sibling_leftmost_child->child_right() == nullptr)
                                                        {
                                                            navigator_except_node_end_t *node_right = bnode_right_sibling_leftmost_child;
                                                            navigator_except_node_end_t *node_left = node_right->child_left();
                                                            typename navigator_t::loop_t loop_outer_sibling = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(node_right);
                                                            loop_outer_sibling.child_left = node_left;
                                                            loop_outer_sibling.rightmost_descendent_of_child_left = node_right;
                                                            loop_outer_sibling.index = -1, loop_outer_sibling.link();
                                                            typename navigator_t::loop_t{
                                                                .this_ = node_left,
                                                                .child_right = node_right,
                                                            }
                                                                .link();
                                                            node_right->color() = true;
                                                            node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                            node_left->color() = false;
                                                            node_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                            child_right_after_split = node_left;
                                                            move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                                        }
                                                        else
                                                            child_right_after_split = bnode_right_sibling_leftmost_child;
                                                    }
                                                    if(parent->role() != rb2p_node_role_e::root)
                                                        loop_outer_parent.exchange(std::get<2>(bnode_before_split.keys).value());
                                                    else
                                                    {
                                                        loop_end.root = std::get<2>(bnode_before_split.keys).value();
                                                        should_link_loop_end = true;
                                                    }
                                                    child_after_split->color() = std::get<0>(bnode_before_split.keys).value()->color();
                                                    child_after_split->role() = std::get<0>(bnode_before_split.keys).value()->role();
                                                    std::get<0>(bnode_before_split.keys).value()->color() = std::get<1>(bnode_before_split.keys)->color();
                                                    std::get<0>(bnode_before_split.keys).value()->role() = std::get<1>(bnode_before_split.keys)->role();
                                                    std::get<1>(bnode_before_split.keys)->color() = std::get<2>(bnode_before_split.keys).value()->color();
                                                    std::get<1>(bnode_before_split.keys)->role() = std::get<2>(bnode_before_split.keys).value()->role();
                                                    std::get<2>(bnode_before_split.keys).value()->color() = parent->color();
                                                    std::get<2>(bnode_before_split.keys).value()->role() = parent->role();
                                                    child_after_split = parent;

                                                    if(child_left_or_right_after_split_height == 0)
                                                    {
                                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, child_after_split_old);
                                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<0>(bnode_before_split.keys).value());
                                                    }
                                                    else
                                                    {
                                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, child_after_split_old);
                                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                                        refresh_node_count_and_accumulated_storage_and_above_until(schedules, accumulator, std::get<0>(bnode_before_split.keys).value(), std::get<2>(bnode_before_split.keys).value());
                                                    }
                                                }
                                                else std::unreachable();
                                                bnode_before_split = bnode_right_sibling;
                                            }
                                            else
                                                fallback();
                                        }
                                        else
                                            fallback();
                                    };
                                };
                                auto spill_to_parent = [&]()
                                {
                                    if(child_left_or_right_after_split_height == 0)
                                    {
                                        assert(std::get<0>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root || std::get<0>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_non_root);
                                        assert(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent || std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent);
                                        assert(std::get<2>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root || std::get<2>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_non_root);
                                        assert(std::get<0>(bnode_before_split.keys).value()->role() != rb2p_node_role_e::child_left_leftmost_descendent_of_root || std::get<2>(bnode_before_split.keys).value()->role() != rb2p_node_role_e::child_right_rightmost_descendent_of_root);
                                    }
                                    else
                                    {
                                        assert(std::get<0>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent);
                                        assert(std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::child_left_not_a_leftmost_descendent || std::get<1>(bnode_before_split.keys)->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent);
                                        assert(std::get<2>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_right_not_a_rightmost_descendent);
                                    }
                                    if(bnode_before_split.child_index == -3)
                                    {
                                        if(child_left_or_right_after_split_height == 0)
                                        {
                                            if(std::get<0>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_non_root)
                                            {
                                                typename navigator_t::loop_t loop_outer_left = navigator_t::template get_loop<navigator_t::loop_type_e::ancestor>(std::get<0>(bnode_before_split.keys).value());
                                                loop_outer_left.exchange(child_after_split);
                                            }
                                            typename navigator_t::loop_t{
                                                .this_ = std::get<1>(bnode_before_split.keys),
                                                .child_right = std::get<2>(bnode_before_split.keys).value(),
                                            }
                                                .link();
                                        }
                                        else
                                        {
                                            typename navigator_t::loop_t loop_inner_left = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<0>(bnode_before_split.keys).value());
                                            typename navigator_t::loop_t loop_inner_right = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<0>(bnode_before_split.keys).value());
                                            navigator_except_node_end_t *key_left_child_right, *key_left_child_right_child_left, *key_left_child_right_child_right;
                                            if(child_left_or_right_after_split_height == 1)
                                            {
                                                key_left_child_right = std::get<0>(bnode_before_split.keys).value()->child_right();
                                                key_left_child_right_child_left = key_left_child_right->child_left();
                                                key_left_child_right_child_right = key_left_child_right->child_right();
                                            }
                                            typename navigator_t::loop_t{
                                                .this_ = child_after_split,
                                                .child_right = child_right_after_split,
                                                .leftmost_descendent_of_child_right = child_right_after_split_leftmost_descendent,
                                                .rightmost_descendent_of_child_left = child_left_after_split_rightmost_descendent,
                                                .child_left = child_left_after_split,
                                            }
                                                .link();
                                            if(child_left_or_right_after_split_height == 1)
                                            {
                                                if(child_left_after_split->role() != rb2p_node_role_e::child_left_leftmost_descendent_of_root)
                                                    child_left_after_split->role() = child_left_after_split->child_left() == nullptr ? rb2p_node_role_e::child_left_leftmost_descendent_of_non_root : rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                child_right_after_split->role() = child_right_after_split->child_right() == nullptr ? rb2p_node_role_e::child_right_rightmost_descendent_of_non_root : rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                            }
                                            else if(child_left_or_right_after_split_height > 1)
                                            {
                                                child_left_after_split->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                child_right_after_split->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                            }

                                            if(child_left_or_right_after_split_height == 1 && key_left_child_right_child_right != nullptr && key_left_child_right_child_left == nullptr)
                                            {
                                                navigator_except_node_end_t *node_left = key_left_child_right;
                                                navigator_except_node_end_t *node_right = key_left_child_right_child_right;
                                                typename navigator_t::loop_t{
                                                    .this_ = node_right,
                                                    .child_left = node_left,
                                                }
                                                    .link();
                                                node_left->color() = true;
                                                node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                node_right->color() = false;
                                                node_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                loop_inner_left.child_right = node_right;
                                                loop_inner_left.leftmost_descendent_of_child_right = node_left;
                                                move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                            }
                                            loop_inner_right.exchange(loop_inner_left.child_right);
                                            if(child_left_or_right_after_split_height == 1)
                                                loop_inner_left.child_right->role() = loop_inner_left.child_right->child_left() == nullptr ? rb2p_node_role_e::child_left_leftmost_descendent_of_non_root : rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                            else if(child_left_or_right_after_split_height > 1)
                                                loop_inner_left.child_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;

                                            child_right_after_split_leftmost_descendent = loop_inner_left.leftmost_descendent_of_child_right != nullptr ? loop_inner_left.leftmost_descendent_of_child_right : loop_inner_left.child_right;
                                            child_left_after_split_rightmost_descendent = loop_inner_left.rightmost_descendent_of_child_left != nullptr ? loop_inner_left.rightmost_descendent_of_child_left : child_right_after_split;
                                        }
                                        child_after_split->color() = false;
                                        child_after_split->role() = std::get<0>(bnode_before_split.keys).value()->role();
                                        child_left_after_split = child_after_split;
                                        child_right_after_split = std::get<1>(bnode_before_split.keys);
                                        child_after_split = std::get<0>(bnode_before_split.keys).value();

                                        if(child_left_or_right_after_split_height == 0)
                                        {
                                            refresh_node_count_and_accumulated_storage(schedules, accumulator, child_after_split_old);
                                            refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                        }
                                        else
                                        {
                                            refresh_node_count_and_accumulated_storage(schedules, accumulator, child_after_split_old);
                                            refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                        }
                                    }
                                    else if(bnode_before_split.child_index == -1)
                                    {
                                        if(child_left_or_right_after_split_height == 0)
                                        {
                                            typename navigator_t::loop_t{
                                                .this_ = std::get<1>(bnode_before_split.keys),
                                                .child_right = std::get<2>(bnode_before_split.keys).value(),
                                            }
                                                .link();
                                        }
                                        else
                                        {
                                            typename navigator_t::loop_t loop_inner_left = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<0>(bnode_before_split.keys).value());
                                            typename navigator_t::loop_t loop_inner_right = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<0>(bnode_before_split.keys).value());
                                            if(child_left_or_right_after_split_height == 1 && child_left_after_split->child_left() != nullptr && child_left_after_split->child_right() == nullptr)
                                            {
                                                navigator_except_node_end_t *node_right = child_left_after_split;
                                                navigator_except_node_end_t *node_left = node_right->child_left();
                                                typename navigator_t::loop_t{
                                                    .this_ = node_left,
                                                    .child_right = node_right,
                                                }
                                                    .link();
                                                node_right->color() = true;
                                                node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                node_left->color() = false;
                                                node_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                std::tie(child_left_after_split, child_left_after_split_rightmost_descendent) = std::make_tuple(node_left, node_right);
                                                move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                            }
                                            if(child_left_or_right_after_split_height == 1 && child_right_after_split->child_right() != nullptr && child_right_after_split->child_left() == nullptr)
                                            {
                                                navigator_except_node_end_t *node_left = child_right_after_split;
                                                navigator_except_node_end_t *node_right = node_left->child_right();
                                                typename navigator_t::loop_t{
                                                    .this_ = node_right,
                                                    .child_left = node_left,
                                                }
                                                    .link();
                                                node_left->color() = true;
                                                node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                node_right->color() = false;
                                                node_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                std::tie(child_right_after_split, child_right_after_split_leftmost_descendent) = std::make_tuple(node_right, node_left);
                                                move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                            }
                                            loop_inner_right.exchange(child_right_after_split);
                                            loop_inner_left.child_right = child_left_after_split;
                                            loop_inner_left.index = -1, loop_inner_left.link();
                                            if(child_left_or_right_after_split_height == 1)
                                            {
                                                child_left_after_split->role() = child_left_after_split->child_right() == nullptr ? rb2p_node_role_e::child_right_rightmost_descendent_of_non_root : rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                child_right_after_split->role() = child_right_after_split->child_left() == nullptr ? rb2p_node_role_e::child_left_leftmost_descendent_of_non_root : rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                            }
                                            else if(child_left_or_right_after_split_height > 1)
                                            {
                                                child_left_after_split->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                child_right_after_split->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                            }
                                        }
                                        std::get<0>(bnode_before_split.keys).value()->color() = false;
                                        if(child_left_after_split_rightmost_descendent == nullptr)
                                            child_left_after_split_rightmost_descendent = child_left_after_split;
                                        child_left_after_split = std::get<0>(bnode_before_split.keys).value();
                                        if(child_right_after_split_leftmost_descendent == nullptr)
                                            child_right_after_split_leftmost_descendent = child_right_after_split;
                                        child_right_after_split = std::get<1>(bnode_before_split.keys);

                                        if(child_left_or_right_after_split_height == 0)
                                            refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                        else
                                        {
                                            refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<0>(bnode_before_split.keys).value());
                                            refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                        }
                                    }
                                    else if(bnode_before_split.child_index == 1)
                                    {
                                        if(child_left_or_right_after_split_height == 0)
                                        {
                                            typename navigator_t::loop_t{
                                                .this_ = std::get<1>(bnode_before_split.keys),
                                                .child_left = std::get<0>(bnode_before_split.keys).value(),
                                            }
                                                .link();
                                        }
                                        else
                                        {
                                            typename navigator_t::loop_t loop_inner_right = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<2>(bnode_before_split.keys).value());
                                            typename navigator_t::loop_t loop_inner_left = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<2>(bnode_before_split.keys).value());
                                            if(child_left_or_right_after_split_height == 1 && child_right_after_split->child_right() != nullptr && child_right_after_split->child_left() == nullptr)
                                            {
                                                navigator_except_node_end_t *node_left = child_right_after_split;
                                                navigator_except_node_end_t *node_right = node_left->child_right();
                                                typename navigator_t::loop_t{
                                                    .this_ = node_right,
                                                    .child_left = node_left,
                                                }
                                                    .link();
                                                node_left->color() = true;
                                                node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                                node_right->color() = false;
                                                node_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                std::tie(child_right_after_split, child_right_after_split_leftmost_descendent) = std::make_tuple(node_right, node_left);
                                                move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                            }
                                            if(child_left_or_right_after_split_height == 1 && child_left_after_split->child_left() != nullptr && child_left_after_split->child_right() == nullptr)
                                            {
                                                navigator_except_node_end_t *node_right = child_left_after_split;
                                                navigator_except_node_end_t *node_left = node_right->child_left();
                                                typename navigator_t::loop_t{
                                                    .this_ = node_left,
                                                    .child_right = node_right,
                                                }
                                                    .link();
                                                node_right->color() = true;
                                                node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                node_left->color() = false;
                                                node_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                std::tie(child_left_after_split, child_left_after_split_rightmost_descendent) = std::make_tuple(node_left, node_right);
                                                move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                            }
                                            loop_inner_left.exchange(child_left_after_split);
                                            loop_inner_right.child_left = child_right_after_split;
                                            loop_inner_right.index = -1, loop_inner_right.link();
                                            if(child_left_or_right_after_split_height == 1)
                                            {
                                                child_right_after_split->role() = child_right_after_split->child_left() == nullptr ? rb2p_node_role_e::child_left_leftmost_descendent_of_non_root : rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                child_left_after_split->role() = child_left_after_split->child_right() == nullptr ? rb2p_node_role_e::child_right_rightmost_descendent_of_non_root : rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                            }
                                            else if(child_left_or_right_after_split_height > 1)
                                            {
                                                child_right_after_split->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                                child_left_after_split->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                            }
                                        }
                                        std::get<2>(bnode_before_split.keys).value()->color() = false;
                                        if(child_right_after_split_leftmost_descendent == nullptr)
                                            child_right_after_split_leftmost_descendent = child_right_after_split;
                                        child_right_after_split = std::get<2>(bnode_before_split.keys).value();
                                        if(child_left_after_split_rightmost_descendent == nullptr)
                                            child_left_after_split_rightmost_descendent = child_left_after_split;
                                        child_left_after_split = std::get<1>(bnode_before_split.keys);

                                        if(child_left_or_right_after_split_height == 0)
                                            refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                        else
                                        {
                                            refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<2>(bnode_before_split.keys).value());
                                            refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                        }
                                    }
                                    else if(bnode_before_split.child_index == 3)
                                    {
                                        if(child_left_or_right_after_split_height == 0)
                                        {
                                            if(std::get<2>(bnode_before_split.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_non_root)
                                            {
                                                typename navigator_t::loop_t loop_outer_right = navigator_t::template get_loop<navigator_t::loop_type_e::ancestor>(std::get<2>(bnode_before_split.keys).value());
                                                loop_outer_right.exchange(child_after_split);
                                            }
                                            typename navigator_t::loop_t{
                                                .this_ = std::get<1>(bnode_before_split.keys),
                                                .child_left = std::get<0>(bnode_before_split.keys).value(),
                                            }
                                                .link();
                                        }
                                        else
                                        {
                                            typename navigator_t::loop_t loop_inner_right = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<2>(bnode_before_split.keys).value());
                                            typename navigator_t::loop_t loop_inner_left = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<2>(bnode_before_split.keys).value());
                                            navigator_except_node_end_t *key_right_child_left, *key_right_child_left_child_right, *key_right_child_left_child_left;
                                            if(child_left_or_right_after_split_height == 1)
                                            {
                                                key_right_child_left = std::get<2>(bnode_before_split.keys).value()->child_left();
                                                key_right_child_left_child_right = key_right_child_left->child_right();
                                                key_right_child_left_child_left = key_right_child_left->child_left();
                                            }
                                            typename navigator_t::loop_t{
                                                .this_ = child_after_split,
                                                .child_right = child_right_after_split,
                                                .leftmost_descendent_of_child_right = child_right_after_split_leftmost_descendent,
                                                .rightmost_descendent_of_child_left = child_left_after_split_rightmost_descendent,
                                                .child_left = child_left_after_split,
                                            }
                                                .link();
                                            if(child_left_or_right_after_split_height == 1)
                                            {
                                                if(child_right_after_split->role() != rb2p_node_role_e::child_right_rightmost_descendent_of_root)
                                                    child_right_after_split->role() = child_right_after_split->child_right() == nullptr ? rb2p_node_role_e::child_right_rightmost_descendent_of_non_root : rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                child_left_after_split->role() = child_left_after_split->child_left() == nullptr ? rb2p_node_role_e::child_left_leftmost_descendent_of_non_root : rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                            }
                                            else if(child_left_or_right_after_split_height > 1)
                                            {
                                                child_right_after_split->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                child_left_after_split->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                            }

                                            if(child_left_or_right_after_split_height == 1 && key_right_child_left_child_left != nullptr && key_right_child_left_child_right == nullptr)
                                            {
                                                navigator_except_node_end_t *node_right = key_right_child_left;
                                                navigator_except_node_end_t *node_left = key_right_child_left_child_left;
                                                typename navigator_t::loop_t{
                                                    .this_ = node_left,
                                                    .child_right = node_right,
                                                }
                                                    .link();
                                                node_right->color() = true;
                                                node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                                node_left->color() = false;
                                                node_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                                loop_inner_right.child_left = node_left;
                                                loop_inner_right.rightmost_descendent_of_child_left = node_right;
                                                move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                            }
                                            loop_inner_left.exchange(loop_inner_right.child_left);
                                            if(child_left_or_right_after_split_height == 1)
                                                loop_inner_right.child_left->role() = loop_inner_right.child_left->child_right() == nullptr ? rb2p_node_role_e::child_right_rightmost_descendent_of_non_root : rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                            else if(child_left_or_right_after_split_height > 1)
                                                loop_inner_right.child_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;

                                            child_left_after_split_rightmost_descendent = loop_inner_right.rightmost_descendent_of_child_left != nullptr ? loop_inner_right.rightmost_descendent_of_child_left : loop_inner_right.child_left;
                                            child_right_after_split_leftmost_descendent = loop_inner_right.leftmost_descendent_of_child_right != nullptr ? loop_inner_right.leftmost_descendent_of_child_right : child_left_after_split;
                                        }
                                        child_after_split->color() = false;
                                        child_after_split->role() = std::get<2>(bnode_before_split.keys).value()->role();
                                        if(child_after_split->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root)
                                            loop_end.index = -1, loop_end.link();
                                        child_right_after_split = child_after_split;
                                        child_left_after_split = std::get<1>(bnode_before_split.keys);
                                        child_after_split = std::get<2>(bnode_before_split.keys).value();

                                        if(child_left_or_right_after_split_height == 0)
                                        {
                                            refresh_node_count_and_accumulated_storage(schedules, accumulator, child_after_split_old);
                                            refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                        }
                                        else
                                        {
                                            refresh_node_count_and_accumulated_storage(schedules, accumulator, child_after_split_old);
                                            refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                        }
                                    }
                                    else std::unreachable();
                                    bnode_before_split = bnode_parent;
                                    ++child_left_or_right_after_split_height;
                                };
                                if(bnode_parent.child_index == -3 || bnode_parent.child_index == -1)
                                    try_spill_to_left_sibling(try_spill_to_right_sibling(spill_to_parent))();
                                else if(bnode_parent.child_index == 1 || bnode_parent.child_index == 3)
                                    try_spill_to_right_sibling(try_spill_to_left_sibling(spill_to_parent))();
                                else std::unreachable();
                            }
                        }
                    }
                    if(should_link_loop_end)
                        loop_end.index = -1, loop_end.link();
                    return height_changed;
                }

                static bool insert(schedules_t &schedules, node_end_t *node_end, navigator_t * const node, navigator_except_node_end_t * const node_new)
                {
                    accumulator_t const &accumulator = node_end->accumulator;
                    assert(node != nullptr);
                    assert(node_new != nullptr);
                    bool is_empty = empty(node_end);
                    if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::no_size)
                        ;
                    else if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_node_end)
                        ++node_end->node_count;
                    else if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_each_node_except_node_end)
                        ;
                    else
                        std::unreachable();

                    bnode_up_t bnode_before_split;

                    navigator_except_node_end_t *child_left_after_split, *child_left_after_split_rightmost_descendent, *child_after_split, *child_right_after_split_leftmost_descendent, *child_right_after_split;
                    std::size_t child_left_or_right_after_split_height;

                    bool should_link_loop_end = false;
                    typename navigator_t::loop_end_t loop_end = navigator_t::get_loop_end(node_end);

                    bool height_changed;
                    if(node == node_end) // node_end
                    {
                        if(is_empty) // ++count==1
                        {
                            node_new->color() = false;
                            node_new->role() = rb2p_node_role_e::root;
                            typename navigator_t::loop_t{
                                .this_ = node_new,
                            }
                                .link();
                            refresh_node_count_and_accumulated_storage(schedules, accumulator, node_new);

                            loop_end.root = node_new;
                            loop_end.index = -1, loop_end.link();
                            height_changed = true;
                            goto skip_insert_impl;
                        }
                        else // ++count!=1
                        {
                            bnode_before_split = bnode_up_t::get_bnode_from_key(static_cast<navigator_except_node_end_t *>(tagged_ptr_bit0_unsetted(node->*p_child_left_all)), true);
                            std::tie(child_left_after_split, child_left_after_split_rightmost_descendent, child_after_split, child_right_after_split_leftmost_descendent, child_right_after_split, child_left_or_right_after_split_height) = std::make_tuple(nullptr, nullptr, node_new, nullptr, nullptr, 0);

                            loop_end.rightmost_descendent_of_root = node_new;
                            should_link_loop_end = true;
                        }
                    }
                    else if(node->*p_child_left_all == nullptr || tagged_ptr_bit0_is_setted(node->*p_child_left_all)) // doesn't have left tree (not root's leftmost descendent) / doesn't have left tree (root's leftmost descendent)
                    {
                        bnode_before_split = bnode_up_t::get_bnode_from_key(static_cast<navigator_except_node_end_t *>(node), false);
                        std::tie(child_left_after_split, child_left_after_split_rightmost_descendent, child_after_split, child_right_after_split_leftmost_descendent, child_right_after_split, child_left_or_right_after_split_height) = std::make_tuple(nullptr, nullptr, node_new, nullptr, nullptr, 0);
                        if(node->*p_child_left_all == nullptr) // doesn't have left tree (not root's leftmost descendent)
                            ;
                        else // doesn't have left tree (root's leftmost descendent)
                        {
                            loop_end.leftmost_descendent_of_root = node_new;
                            should_link_loop_end = true;
                        }
                    }
                    else // has left tree
                    {
                        navigator_except_node_end_t *node_current = static_cast<navigator_except_node_end_t *>(rb2p_iterator_t<false, is_reversed, config_t>::template predecessor<false>(node)); // find rightmost descendent of left tree
                        bnode_before_split = bnode_up_t::get_bnode_from_key(node_current, true);
                        std::tie(child_left_after_split, child_left_after_split_rightmost_descendent, child_after_split, child_right_after_split_leftmost_descendent, child_right_after_split, child_left_or_right_after_split_height) = std::make_tuple(nullptr, nullptr, node_new, nullptr, nullptr, 0);
                    }
                    height_changed = insert_impl(schedules, node_end, bnode_before_split, child_left_after_split, child_left_after_split_rightmost_descendent, child_after_split, child_right_after_split_leftmost_descendent, child_right_after_split, child_left_or_right_after_split_height, should_link_loop_end, loop_end);
                skip_insert_impl:;
                    return height_changed;
                }

                static void swap_root(node_end_t *node_end_lhs, node_end_t *node_end_rhs)
                {
                    if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::no_size)
                        ;
                    else if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_node_end)
                        std::ranges::swap(node_end_lhs->node_count, node_end_rhs->node_count);
                    else if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_each_node_except_node_end)
                        ;
                    else
                        std::unreachable();
                    typename navigator_t::loop_end_t loop_end_lhs = navigator_t::get_loop_end(node_end_lhs);
                    typename navigator_t::loop_end_t loop_end_rhs = navigator_t::get_loop_end(node_end_rhs);
                    std::ranges::swap(loop_end_lhs.end, loop_end_rhs.end);
                    loop_end_lhs.index = -1, loop_end_lhs.link();
                    loop_end_rhs.index = -1, loop_end_rhs.link();
                };

                template<bool goto_left_or_right, bool invoked_by_concat_or_split>
                static bool concat_with_middle_key(schedules_t &schedules, node_end_t *node_end_lhs, navigator_except_node_end_t * const node_middle_key, node_end_t *node_end_rhs, std::conditional_t<!invoked_by_concat_or_split, std::nullptr_t, int> height_difference)
                {
                    accumulator_t const &accumulator = !goto_left_or_right ? node_end_lhs->accumulator : node_end_rhs->accumulator;
                    bool height_changed;
                    bool is_empty_lhs = empty(node_end_lhs), is_empty_rhs = empty(node_end_rhs);
                    if(is_empty_lhs || is_empty_rhs)
                    {
                        if constexpr(!goto_left_or_right)
                        {
                            if(is_empty_rhs)
                                height_changed = insert(schedules, node_end_lhs, node_end_lhs, node_middle_key);
                            else if(is_empty_lhs)
                            {
                                height_changed = insert(schedules, node_end_rhs, tagged_ptr_bit0_unsetted(node_end_rhs->child_right_all()), node_middle_key);
                                swap_root(node_end_lhs, node_end_rhs);
                            }
                            else std::unreachable();
                        }
                        else
                        {
                            if(is_empty_lhs)
                                height_changed = insert(schedules, node_end_rhs, tagged_ptr_bit0_unsetted(node_end_rhs->child_right_all()), node_middle_key);
                            else if(is_empty_rhs)
                            {
                                height_changed = insert(schedules, node_end_lhs, node_end_lhs, node_middle_key);
                                swap_root(node_end_lhs, node_end_rhs);
                            }
                            else std::unreachable();
                        }
                    }
                    else
                    {
                        navigator_except_node_end_t *child_left_after_split_rightmost_descendent = static_cast<navigator_except_node_end_t *>(tagged_ptr_bit0_unsetted(node_end_lhs->child_left_all())),
                                                    *current_rightmost_black_descendent_of_lhs,
                                                    *child_right_after_split_leftmost_descendent = static_cast<navigator_except_node_end_t *>(tagged_ptr_bit0_unsetted(node_end_rhs->child_right_all())),
                                                    *current_leftmost_black_descendent_of_rhs;
                        std::size_t child_left_or_right_after_split_height = 1;
                        typename navigator_t::loop_end_t loop_end_lhs = navigator_t::get_loop_end(node_end_lhs);
                        typename navigator_t::loop_end_t loop_end_rhs = navigator_t::get_loop_end(node_end_rhs);
                        if constexpr(!invoked_by_concat_or_split)
                        {
                            current_rightmost_black_descendent_of_lhs = child_left_after_split_rightmost_descendent;
                            if(current_rightmost_black_descendent_of_lhs->color() == true)
                            {
                                current_rightmost_black_descendent_of_lhs = current_rightmost_black_descendent_of_lhs->parent();
                                assert(current_rightmost_black_descendent_of_lhs->color() == false);
                            }
                            current_leftmost_black_descendent_of_rhs = child_right_after_split_leftmost_descendent;
                            if(current_leftmost_black_descendent_of_rhs->color() == true)
                            {
                                current_leftmost_black_descendent_of_rhs = current_leftmost_black_descendent_of_rhs->parent();
                                assert(current_leftmost_black_descendent_of_rhs->color() == false);
                            }
                            while(current_rightmost_black_descendent_of_lhs->role() != rb2p_node_role_e::root && current_leftmost_black_descendent_of_rhs->role() != rb2p_node_role_e::root)
                            {
                                current_rightmost_black_descendent_of_lhs = current_rightmost_black_descendent_of_lhs->parent();
                                if(current_rightmost_black_descendent_of_lhs->color() == true)
                                {
                                    current_rightmost_black_descendent_of_lhs = current_rightmost_black_descendent_of_lhs->parent();
                                    assert(current_rightmost_black_descendent_of_lhs->color() == false);
                                }
                                current_leftmost_black_descendent_of_rhs = current_leftmost_black_descendent_of_rhs->parent();
                                if(current_leftmost_black_descendent_of_rhs->color() == true)
                                {
                                    current_leftmost_black_descendent_of_rhs = current_leftmost_black_descendent_of_rhs->parent();
                                    assert(current_leftmost_black_descendent_of_rhs->color() == false);
                                }
                                ++child_left_or_right_after_split_height;
                            }
                        }
                        else
                        {
                            current_rightmost_black_descendent_of_lhs = static_cast<navigator_except_node_end_t *>(tagged_ptr_bit0_unsetted(node_end_lhs->parent_all()));
                            current_leftmost_black_descendent_of_rhs = static_cast<navigator_except_node_end_t *>(tagged_ptr_bit0_unsetted(node_end_rhs->parent_all()));
                            if(height_difference < 0)
                            {
                                for(int current_height_difference = 0; current_height_difference != -height_difference; ++current_height_difference)
                                {
                                    current_leftmost_black_descendent_of_rhs = current_leftmost_black_descendent_of_rhs->child_left();
                                    if(current_leftmost_black_descendent_of_rhs->color() == true)
                                    {
                                        current_leftmost_black_descendent_of_rhs = current_leftmost_black_descendent_of_rhs->child_left();
                                        assert(current_leftmost_black_descendent_of_rhs->color() == false);
                                    }
                                }
                            }
                            else if(height_difference > 0)
                            {
                                for(int current_height_difference = 0; current_height_difference != height_difference; ++current_height_difference)
                                {
                                    current_rightmost_black_descendent_of_lhs = current_rightmost_black_descendent_of_lhs->child_right();
                                    if(current_rightmost_black_descendent_of_lhs->color() == true)
                                    {
                                        current_rightmost_black_descendent_of_lhs = current_rightmost_black_descendent_of_lhs->child_right();
                                        assert(current_rightmost_black_descendent_of_lhs->color() == false);
                                    }
                                }
                            }
                            if(current_rightmost_black_descendent_of_lhs->child_left() == nullptr || (current_rightmost_black_descendent_of_lhs->child_left()->color() == true && current_rightmost_black_descendent_of_lhs->child_left()->child_left() == nullptr))
                                child_left_or_right_after_split_height = 1;
                            else
                                child_left_or_right_after_split_height = 2;
                        }
                        if(current_rightmost_black_descendent_of_lhs->role() == rb2p_node_role_e::root && current_leftmost_black_descendent_of_rhs->role() == rb2p_node_role_e::root)
                        {
                            node_middle_key->color() = false;
                            node_middle_key->role() = rb2p_node_role_e::root;
                            if(current_rightmost_black_descendent_of_lhs->child_left() == nullptr)
                            {
                                if(current_rightmost_black_descendent_of_lhs->child_right() == nullptr)
                                    current_rightmost_black_descendent_of_lhs->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_root;
                                else
                                {
                                    navigator_except_node_end_t *node_left = current_rightmost_black_descendent_of_lhs;
                                    navigator_except_node_end_t *node_right = current_rightmost_black_descendent_of_lhs->child_right();
                                    typename navigator_t::loop_t{
                                        .this_ = node_right,
                                        .child_left = node_left,
                                    }
                                        .link();
                                    node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_root;
                                    node_left->color() = true;
                                    node_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                    node_right->color() = false;
                                    loop_end_lhs.leftmost_descendent_of_root = node_left;
                                    loop_end_lhs.root = node_right;
                                    loop_end_lhs.rightmost_descendent_of_root = nullptr;
                                    current_rightmost_black_descendent_of_lhs = node_right;
                                    move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                }
                            }
                            else
                            {
                                current_rightmost_black_descendent_of_lhs->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                if(loop_end_lhs.rightmost_descendent_of_root != nullptr)
                                    loop_end_lhs.rightmost_descendent_of_root->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                            }
                            if(current_leftmost_black_descendent_of_rhs->child_right() == nullptr)
                            {
                                if(current_leftmost_black_descendent_of_rhs->child_left() == nullptr)
                                    current_leftmost_black_descendent_of_rhs->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_root;
                                else
                                {
                                    navigator_except_node_end_t *node_right = current_leftmost_black_descendent_of_rhs;
                                    navigator_except_node_end_t *node_left = current_leftmost_black_descendent_of_rhs->child_left();
                                    typename navigator_t::loop_t{
                                        .this_ = node_left,
                                        .child_right = node_right,
                                    }
                                        .link();
                                    node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_root;
                                    node_right->color() = true;
                                    node_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                    node_left->color() = false;
                                    loop_end_rhs.rightmost_descendent_of_root = node_right;
                                    loop_end_rhs.root = node_left;
                                    loop_end_rhs.leftmost_descendent_of_root = nullptr;
                                    current_leftmost_black_descendent_of_rhs = node_left;
                                    move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                }
                            }
                            else
                            {
                                current_leftmost_black_descendent_of_rhs->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                if(loop_end_rhs.leftmost_descendent_of_root != nullptr)
                                    loop_end_rhs.leftmost_descendent_of_root->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                            }
                            typename navigator_t::loop_t{
                                .this_ = node_middle_key,
                                .child_right = loop_end_rhs.root,
                                .leftmost_descendent_of_child_right = child_right_after_split_leftmost_descendent,
                                .rightmost_descendent_of_child_left = child_left_after_split_rightmost_descendent,
                                .child_left = loop_end_lhs.root,
                            }
                                .link();
                            if constexpr(!goto_left_or_right)
                            {
                                loop_end_lhs.leftmost_descendent_of_root = loop_end_lhs.leftmost_descendent_of_root != nullptr ? loop_end_lhs.leftmost_descendent_of_root : loop_end_lhs.root;
                                loop_end_lhs.root = node_middle_key;
                                loop_end_lhs.rightmost_descendent_of_root = loop_end_rhs.rightmost_descendent_of_root != nullptr ? loop_end_rhs.rightmost_descendent_of_root : loop_end_rhs.root;
                                loop_end_lhs.index = -1, loop_end_lhs.link();
                                loop_end_rhs.leftmost_descendent_of_root = loop_end_rhs.root = loop_end_rhs.rightmost_descendent_of_root = nullptr;
                                loop_end_rhs.index = -1, loop_end_rhs.link();
                            }
                            else
                            {
                                loop_end_rhs.rightmost_descendent_of_root = loop_end_rhs.rightmost_descendent_of_root != nullptr ? loop_end_rhs.rightmost_descendent_of_root : loop_end_rhs.root;
                                loop_end_rhs.root = node_middle_key;
                                loop_end_rhs.leftmost_descendent_of_root = loop_end_lhs.leftmost_descendent_of_root != nullptr ? loop_end_lhs.leftmost_descendent_of_root : loop_end_lhs.root;
                                loop_end_rhs.index = -1, loop_end_rhs.link();
                                loop_end_lhs.rightmost_descendent_of_root = loop_end_lhs.root = loop_end_lhs.leftmost_descendent_of_root = nullptr;
                                loop_end_lhs.index = -1, loop_end_lhs.link();
                            }
                            refresh_node_count_and_accumulated_storage(schedules, accumulator, node_middle_key);
                            height_changed = true;
                        }
                        else if(current_leftmost_black_descendent_of_rhs->role() == rb2p_node_role_e::root)
                        {
                            auto current_rightmost_black_descendent_of_lhs_center_key_parent_info = typename navigator_t::template parent_info_t<is_reversed>(current_rightmost_black_descendent_of_lhs);
                            bnode_up_t bnode_parent_of_current_rightmost_black_descendent_of_lhs = bnode_up_t::get_bnode_from_key(static_cast<navigator_except_node_end_t *>(current_rightmost_black_descendent_of_lhs_center_key_parent_info.parent), current_rightmost_black_descendent_of_lhs_center_key_parent_info.is_left_or_right_child_of_parent);
                            if(current_rightmost_black_descendent_of_lhs->child_left() == nullptr)
                            {
                                if(current_rightmost_black_descendent_of_lhs->child_right() == nullptr)
                                    /*current_rightmost_black_descendent_of_lhs->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root*/ typename navigator_t::loop_t{.this_ = current_rightmost_black_descendent_of_lhs}.link();
                                else
                                {
                                    typename navigator_t::loop_t child_left_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(current_rightmost_black_descendent_of_lhs);
                                    navigator_except_node_end_t *node_left = current_rightmost_black_descendent_of_lhs;
                                    navigator_except_node_end_t *node_right = current_rightmost_black_descendent_of_lhs->child_right();
                                    typename navigator_t::loop_t{
                                        .this_ = node_right,
                                        .child_left = node_left,
                                    }
                                        .link();
                                    node_right->role() = node_left->role();
                                    node_right->color() = false;
                                    node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                    node_left->color() = true;
                                    child_left_outer_loop.child_right = node_right;
                                    child_left_outer_loop.leftmost_descendent_of_child_right = node_left;
                                    child_left_outer_loop.index = -1, child_left_outer_loop.link();
                                    current_rightmost_black_descendent_of_lhs = node_right;
                                    move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                }
                            }
                            else
                            {
                                /*current_rightmost_black_descendent_of_lhs->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent*/;
                                assert(loop_end_lhs.rightmost_descendent_of_root != nullptr);
                                loop_end_lhs.rightmost_descendent_of_root->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                            }
                            if(current_leftmost_black_descendent_of_rhs->child_right() == nullptr)
                            {
                                if(current_leftmost_black_descendent_of_rhs->child_left() == nullptr)
                                    current_leftmost_black_descendent_of_rhs->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_root;
                                else
                                {
                                    navigator_except_node_end_t *node_right = current_leftmost_black_descendent_of_rhs;
                                    navigator_except_node_end_t *node_left = current_leftmost_black_descendent_of_rhs->child_left();
                                    typename navigator_t::loop_t{
                                        .this_ = node_left,
                                        .child_right = node_right,
                                    }
                                        .link();
                                    node_left->role() = node_right->role();
                                    node_left->color() = false;
                                    node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_root;
                                    node_right->color() = true;
                                    loop_end_rhs.rightmost_descendent_of_root = node_right;
                                    loop_end_rhs.root = node_left;
                                    loop_end_rhs.leftmost_descendent_of_root = nullptr;
                                    current_leftmost_black_descendent_of_rhs = node_left;
                                    move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                }
                            }
                            else
                            {
                                /*current_leftmost_black_descendent_of_rhs->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent*/;
                                if(loop_end_rhs.leftmost_descendent_of_root != nullptr)
                                    loop_end_rhs.leftmost_descendent_of_root->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                            }

                            bool should_link_loop_end = false;
                            if constexpr(!goto_left_or_right)
                            {
                                loop_end_lhs.rightmost_descendent_of_root = loop_end_rhs.rightmost_descendent_of_root != nullptr ? loop_end_rhs.rightmost_descendent_of_root : loop_end_rhs.root;
                                loop_end_lhs.index = -1, loop_end_lhs.link();
                                loop_end_rhs.leftmost_descendent_of_root = loop_end_rhs.root = loop_end_rhs.rightmost_descendent_of_root = nullptr;
                                loop_end_rhs.index = -1, loop_end_rhs.link();
                                height_changed = insert_impl(schedules, node_end_lhs, bnode_parent_of_current_rightmost_black_descendent_of_lhs, current_rightmost_black_descendent_of_lhs, child_left_after_split_rightmost_descendent, node_middle_key, child_right_after_split_leftmost_descendent, current_leftmost_black_descendent_of_rhs, child_left_or_right_after_split_height, should_link_loop_end, loop_end_lhs);
                            }
                            else
                            {
                                loop_end_rhs.rightmost_descendent_of_root = loop_end_rhs.rightmost_descendent_of_root != nullptr ? loop_end_rhs.rightmost_descendent_of_root : loop_end_rhs.root;
                                loop_end_rhs.root = loop_end_lhs.root;
                                loop_end_rhs.leftmost_descendent_of_root = loop_end_lhs.leftmost_descendent_of_root;
                                loop_end_rhs.index = -1, loop_end_rhs.link();
                                loop_end_lhs.rightmost_descendent_of_root = loop_end_lhs.root = loop_end_lhs.leftmost_descendent_of_root = nullptr;
                                loop_end_lhs.index = -1, loop_end_lhs.link();
                                height_changed = insert_impl(schedules, node_end_rhs, bnode_parent_of_current_rightmost_black_descendent_of_lhs, current_rightmost_black_descendent_of_lhs, child_left_after_split_rightmost_descendent, node_middle_key, child_right_after_split_leftmost_descendent, current_leftmost_black_descendent_of_rhs, child_left_or_right_after_split_height, should_link_loop_end, loop_end_rhs);
                            }
                        }
                        else if(current_rightmost_black_descendent_of_lhs->role() == rb2p_node_role_e::root)
                        {
                            auto current_leftmost_black_descendent_of_rhs_center_key_parent_info = typename navigator_t::template parent_info_t<is_reversed>(current_leftmost_black_descendent_of_rhs);
                            bnode_up_t bnode_parent_of_current_leftmost_black_descendent_of_rhs = bnode_up_t::get_bnode_from_key(static_cast<navigator_except_node_end_t *>(current_leftmost_black_descendent_of_rhs_center_key_parent_info.parent), current_leftmost_black_descendent_of_rhs_center_key_parent_info.is_left_or_right_child_of_parent);
                            if(current_leftmost_black_descendent_of_rhs->child_right() == nullptr)
                            {
                                if(current_leftmost_black_descendent_of_rhs->child_left() == nullptr)
                                    /*current_leftmost_black_descendent_of_rhs->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root*/ typename navigator_t::loop_t{.this_ = current_leftmost_black_descendent_of_rhs}.link();
                                else
                                {
                                    typename navigator_t::loop_t child_right_outer_loop = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(current_leftmost_black_descendent_of_rhs);
                                    navigator_except_node_end_t *node_right = current_leftmost_black_descendent_of_rhs;
                                    navigator_except_node_end_t *node_left = current_leftmost_black_descendent_of_rhs->child_left();
                                    typename navigator_t::loop_t{
                                        .this_ = node_left,
                                        .child_right = node_right,
                                    }
                                        .link();
                                    node_left->role() = node_right->role();
                                    node_left->color() = false;
                                    node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                    node_right->color() = true;
                                    child_right_outer_loop.child_left = node_left;
                                    child_right_outer_loop.rightmost_descendent_of_child_left = node_right;
                                    child_right_outer_loop.index = -1, child_right_outer_loop.link();
                                    current_leftmost_black_descendent_of_rhs = node_left;
                                    move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                }
                            }
                            else
                            {
                                /*current_leftmost_black_descendent_of_rhs->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent*/;
                                assert(loop_end_rhs.leftmost_descendent_of_root != nullptr);
                                loop_end_rhs.leftmost_descendent_of_root->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                            }
                            if(current_rightmost_black_descendent_of_lhs->child_left() == nullptr)
                            {
                                if(current_rightmost_black_descendent_of_lhs->child_right() == nullptr)
                                    current_rightmost_black_descendent_of_lhs->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_root;
                                else
                                {
                                    navigator_except_node_end_t *node_left = current_rightmost_black_descendent_of_lhs;
                                    navigator_except_node_end_t *node_right = current_rightmost_black_descendent_of_lhs->child_right();
                                    typename navigator_t::loop_t{
                                        .this_ = node_right,
                                        .child_left = node_left,
                                    }
                                        .link();
                                    node_right->role() = node_left->role();
                                    node_right->color() = false;
                                    node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_root;
                                    node_left->color() = true;
                                    loop_end_lhs.leftmost_descendent_of_root = node_left;
                                    loop_end_lhs.root = node_right;
                                    loop_end_lhs.rightmost_descendent_of_root = nullptr;
                                    current_rightmost_black_descendent_of_lhs = node_right;
                                    move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                }
                            }
                            else
                            {
                                /*current_rightmost_black_descendent_of_lhs->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent*/;
                                if(loop_end_lhs.rightmost_descendent_of_root != nullptr)
                                    loop_end_lhs.rightmost_descendent_of_root->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                            }

                            bool should_link_loop_end = false;
                            if constexpr(!goto_left_or_right)
                            {
                                loop_end_lhs.leftmost_descendent_of_root = loop_end_lhs.leftmost_descendent_of_root != nullptr ? loop_end_lhs.leftmost_descendent_of_root : loop_end_lhs.root;
                                loop_end_lhs.root = loop_end_rhs.root;
                                loop_end_lhs.rightmost_descendent_of_root = loop_end_rhs.rightmost_descendent_of_root;
                                loop_end_lhs.index = -1, loop_end_lhs.link();
                                loop_end_rhs.leftmost_descendent_of_root = loop_end_rhs.root = loop_end_rhs.rightmost_descendent_of_root = nullptr;
                                loop_end_rhs.index = -1, loop_end_rhs.link();
                                height_changed = insert_impl(schedules, node_end_lhs, bnode_parent_of_current_leftmost_black_descendent_of_rhs, current_rightmost_black_descendent_of_lhs, child_left_after_split_rightmost_descendent, node_middle_key, child_right_after_split_leftmost_descendent, current_leftmost_black_descendent_of_rhs, child_left_or_right_after_split_height, should_link_loop_end, loop_end_lhs);
                            }
                            else
                            {
                                loop_end_rhs.leftmost_descendent_of_root = loop_end_lhs.leftmost_descendent_of_root != nullptr ? loop_end_lhs.leftmost_descendent_of_root : loop_end_lhs.root;
                                loop_end_rhs.index = -1, loop_end_rhs.link();
                                loop_end_lhs.rightmost_descendent_of_root = loop_end_lhs.root = loop_end_lhs.leftmost_descendent_of_root = nullptr;
                                loop_end_lhs.index = -1, loop_end_lhs.link();
                                height_changed = insert_impl(schedules, node_end_rhs, bnode_parent_of_current_leftmost_black_descendent_of_rhs, current_rightmost_black_descendent_of_lhs, child_left_after_split_rightmost_descendent, node_middle_key, child_right_after_split_leftmost_descendent, current_leftmost_black_descendent_of_rhs, child_left_or_right_after_split_height, should_link_loop_end, loop_end_rhs);
                            }
                        }
                        else std::unreachable();

                        if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::no_size)
                            ;
                        else if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_node_end)
                        {
                            if constexpr(!goto_left_or_right)
                                std::tie(node_end_lhs->node_count, node_end_rhs->node_count) = std::make_tuple(node_end_lhs->node_count + 1 + node_end_rhs->node_count, 0);
                            else
                                std::tie(node_end_rhs->node_count, node_end_lhs->node_count) = std::make_tuple(node_end_rhs->node_count + 1 + node_end_lhs->node_count, 0);
                        }
                        else if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_each_node_except_node_end)
                            ;
                        else
                            std::unreachable();
                    }
                    return height_changed;
                }

                template<bool goto_left_or_right>
                static void concat_without_middle_key(schedules_t &schedules, node_end_t *node_end_lhs, node_end_t *node_end_rhs)
                {
                    bool is_empty_lhs = empty(node_end_lhs), is_empty_rhs = empty(node_end_rhs);
                    if constexpr(!goto_left_or_right)
                    {
                        if(is_empty_rhs)
                            ;
                        else if(is_empty_lhs)
                            swap_root(node_end_lhs, node_end_rhs);
                        else
                        {
                            navigator_except_node_end_t *node_middle_key = static_cast<navigator_except_node_end_t *>(tagged_ptr_bit0_unsetted(node_end_rhs->child_right_all()));
                            erase(schedules, node_end_rhs, node_middle_key);
                            concat_with_middle_key<goto_left_or_right, false>(schedules, node_end_lhs, node_middle_key, node_end_rhs, nullptr);
                        }
                    }
                    else
                    {
                        if(is_empty_lhs)
                            ;
                        else if(is_empty_rhs)
                            swap_root(node_end_lhs, node_end_rhs);
                        else
                        {
                            navigator_except_node_end_t *node_middle_key = static_cast<navigator_except_node_end_t *>(tagged_ptr_bit0_unsetted(node_end_lhs->child_left_all()));
                            erase(schedules, node_end_lhs, node_middle_key);
                            concat_with_middle_key<goto_left_or_right, false>(schedules, node_end_lhs, node_middle_key, node_end_rhs, nullptr);
                        }
                    }
                }

                template<bool emit_left_or_right>
                static void split(schedules_t &schedules, node_end_t *node_end_emit, node_end_t *node_end, navigator_t * const node)
                {
                    accumulator_t const &accumulator = node_end->accumulator;
                    assert(node != nullptr);
                    assert(empty(node_end_emit));
                    std::size_t size_sum;
                    if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_node_end)
                        size_sum = node_end->node_count;
                    if(node == node_end)
                    {
                        if constexpr(!emit_left_or_right)
                            swap_root(node_end_emit, node_end);
                        else
                            ;
                    }
                    else if(node == tagged_ptr_bit0_unsetted(node_end->child_right_all()))
                    {
                        if constexpr(!emit_left_or_right)
                            ;
                        else
                            swap_root(node_end_emit, node_end);
                    }
                    else
                    {
                        auto split_impl = [&schedules, &accumulator, &node_end_emit, &node_end](auto &this_, bnode_t bnode_to_be_splitted, int split_position, function_view<std::tuple<int, int>(node_end_t *, navigator_except_node_end_t *, node_end_t *, navigator_except_node_end_t *)> split_and_emit_to_node_end_emit) -> void
                        {
                            if(split_position == -2)
                            {
                                auto split_when_all_information_are_ready = [&schedules, &accumulator, &bnode_to_be_splitted](node_end_t *node_end_emit, navigator_except_node_end_t *leftmost_descendent_of_root, node_end_t *node_end, navigator_except_node_end_t *rightmost_descendent_of_root) -> std::tuple<int, int>
                                {
                                    assert(empty(node_end_emit));

                                    if(std::get<0>(bnode_to_be_splitted.keys).value()->child_left() == nullptr)
                                        return std::make_tuple(-1, 0);
                                    else
                                    {
                                        typename navigator_t::loop_t inner_loop_left = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<0>(bnode_to_be_splitted.keys).value());
                                        typename navigator_t::loop_t inner_loop_right = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<0>(bnode_to_be_splitted.keys).value());

                                        navigator_except_node_end_t *key_left_child_right = std::get<0>(bnode_to_be_splitted.keys).value()->child_right(),
                                                                    *key_left_child_right_child_left = key_left_child_right->child_left(),
                                                                    *key_left_child_right_child_right = key_left_child_right->child_right();
                                        if(key_left_child_right_child_left == nullptr && key_left_child_right_child_right != nullptr)
                                        {
                                            navigator_except_node_end_t *node_left = key_left_child_right;
                                            navigator_except_node_end_t *node_right = key_left_child_right_child_right;
                                            typename navigator_t::loop_t{
                                                .this_ = node_right,
                                                .child_left = node_left,
                                            }
                                                .link();
                                            node_right->color() = false;
                                            node_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                            node_left->color() = true;
                                            node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                            std::tie(inner_loop_left.child_right, inner_loop_left.leftmost_descendent_of_child_right) = std::make_tuple(node_right, node_left);
                                            std::tie(inner_loop_right.child_left, inner_loop_right.rightmost_descendent_of_child_left) = std::make_tuple(node_right, nullptr);
                                            move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                            inner_loop_right.index = -1, inner_loop_right.link();
                                        }
                                        else
                                        {
                                            if(key_left_child_right_child_left == nullptr && key_left_child_right_child_right == nullptr)
                                                key_left_child_right->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                            else
                                                key_left_child_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                            inner_loop_right.exchange(key_left_child_right);
                                        }

                                        typename navigator_t::loop_end_t loop_end_left{
                                            .end = node_end_emit,
                                            .leftmost_descendent_of_root = leftmost_descendent_of_root,
                                            .root = inner_loop_left.child_left,
                                            .rightmost_descendent_of_root = inner_loop_left.rightmost_descendent_of_child_left != nullptr ? inner_loop_left.rightmost_descendent_of_child_left : inner_loop_left.child_left,
                                        };
                                        loop_end_left.link();
                                        loop_end_left.link_if_one();
                                        loop_end_left.set_roles();
                                        assert(loop_end_left.root->color() == false);

                                        typename navigator_t::loop_end_t loop_end_right{
                                            .end = node_end,
                                            .leftmost_descendent_of_root = inner_loop_left.leftmost_descendent_of_child_right != nullptr ? inner_loop_left.leftmost_descendent_of_child_right : inner_loop_left.child_right,
                                            .root = inner_loop_right.this_,
                                            .rightmost_descendent_of_root = rightmost_descendent_of_root,
                                        };
                                        loop_end_right.link();
                                        loop_end_right.set_roles();
                                        assert(loop_end_right.root->color() == false);
                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, loop_end_right.root);

                                        bool height_changed = insert(schedules, static_cast<node_end_t *>(loop_end_right.end), loop_end_right.leftmost_descendent_of_root, std::get<0>(bnode_to_be_splitted.keys).value());
                                        return std::make_tuple(-1, (!height_changed ? 0 : 1));
                                    }
                                };
                                if(bnode_to_be_splitted.center_key_parent_info.is_end())
                                    split_when_all_information_are_ready(node_end_emit, static_cast<navigator_except_node_end_t *>(tagged_ptr_bit0_unsetted(node_end->child_right_all())), node_end, static_cast<navigator_except_node_end_t *>(tagged_ptr_bit0_unsetted(node_end->child_left_all())));
                                else
                                {
                                    bnode_up_t bnode_parent = bnode_up_t::get_bnode_from_key(static_cast<navigator_except_node_end_t *>(bnode_to_be_splitted.center_key_parent_info.parent), bnode_to_be_splitted.center_key_parent_info.is_left_or_right_child_of_parent);
                                    this_(this_, static_cast<bnode_t &>(bnode_parent), bnode_parent.child_index, split_when_all_information_are_ready);
                                }
                            }
                            else if(split_position == 0)
                            {
                                auto split_when_all_information_are_ready = [&schedules, &accumulator, &bnode_to_be_splitted](node_end_t *node_end_emit, navigator_except_node_end_t *leftmost_descendent_of_root, node_end_t *node_end, navigator_except_node_end_t *rightmost_descendent_of_root) -> std::tuple<int, int>
                                {
                                    assert(empty(node_end_emit));

                                    if(std::get<1>(bnode_to_be_splitted.keys)->child_left() == nullptr || (std::get<1>(bnode_to_be_splitted.keys)->child_left()->color() == true && std::get<1>(bnode_to_be_splitted.keys)->child_left()->child_left() == nullptr))
                                    {
                                        int height_left = !std::get<0>(bnode_to_be_splitted.keys).has_value() ? -1 : 0;
                                        if(!std::get<0>(bnode_to_be_splitted.keys).has_value())
                                            ;
                                        else
                                        {
                                            typename navigator_t::loop_end_t{
                                                .end = node_end_emit,
                                                .root = std::get<0>(bnode_to_be_splitted.keys).value(),
                                            }
                                                .link();
                                            typename navigator_t::loop_t{
                                                .this_ = std::get<0>(bnode_to_be_splitted.keys).value(),
                                            }
                                                .link();
                                            std::get<0>(bnode_to_be_splitted.keys).value()->color() = false;
                                            std::get<0>(bnode_to_be_splitted.keys).value()->role() = rb2p_node_role_e::root;
                                            refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<0>(bnode_to_be_splitted.keys).value());
                                        }
                                        if(!std::get<2>(bnode_to_be_splitted.keys).has_value())
                                        {
                                            typename navigator_t::loop_end_t{
                                                .end = node_end,
                                                .root = std::get<1>(bnode_to_be_splitted.keys),
                                            }
                                                .link();
                                            typename navigator_t::loop_t{
                                                .this_ = std::get<1>(bnode_to_be_splitted.keys),
                                            }
                                                .link();
                                            assert(std::get<1>(bnode_to_be_splitted.keys)->color() == false);
                                            assert(std::get<1>(bnode_to_be_splitted.keys)->role() == rb2p_node_role_e::root);
                                            refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_to_be_splitted.keys));
                                        }
                                        else
                                        {
                                            typename navigator_t::loop_end_t{
                                                .end = node_end,
                                                .root = std::get<1>(bnode_to_be_splitted.keys),
                                                .rightmost_descendent_of_root = std::get<2>(bnode_to_be_splitted.keys).value(),
                                            }
                                                .link();
                                            typename navigator_t::loop_t{
                                                .this_ = std::get<1>(bnode_to_be_splitted.keys),
                                                .child_right = std::get<2>(bnode_to_be_splitted.keys).value(),
                                            }
                                                .link();
                                            assert(std::get<1>(bnode_to_be_splitted.keys)->color() == false);
                                            assert(std::get<1>(bnode_to_be_splitted.keys)->role() == rb2p_node_role_e::root);
                                            assert(std::get<2>(bnode_to_be_splitted.keys).value()->color() == true);
                                            assert(std::get<2>(bnode_to_be_splitted.keys).value()->role() == rb2p_node_role_e::child_right_rightmost_descendent_of_root);
                                            refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<2>(bnode_to_be_splitted.keys).value());
                                            refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_to_be_splitted.keys));
                                        }
                                        return std::make_tuple(height_left, 0);
                                    }
                                    else
                                    {
                                        int height_left = !std::get<0>(bnode_to_be_splitted.keys).has_value() ? -1 : 0;
                                        int height_right = !std::get<2>(bnode_to_be_splitted.keys).has_value() ? -1 : 0;
                                        typename navigator_t::loop_t inner_loop = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<1>(bnode_to_be_splitted.keys));

                                        typename navigator_t::loop_end_t loop_end_left{
                                            .end = node_end_emit,
                                            .leftmost_descendent_of_root = leftmost_descendent_of_root,
                                            .root = inner_loop.child_left,
                                            .rightmost_descendent_of_root = inner_loop.rightmost_descendent_of_child_left != nullptr ? inner_loop.rightmost_descendent_of_child_left : inner_loop.child_left,
                                        };
                                        loop_end_left.link();
                                        loop_end_left.link_if_one();
                                        loop_end_left.set_roles();
                                        loop_end_left.root->color() = false;

                                        typename navigator_t::loop_end_t loop_end_right{
                                            .end = node_end,
                                            .leftmost_descendent_of_root = inner_loop.leftmost_descendent_of_child_right != nullptr ? inner_loop.leftmost_descendent_of_child_right : inner_loop.child_right,
                                            .root = inner_loop.child_right,
                                            .rightmost_descendent_of_root = rightmost_descendent_of_root,
                                        };
                                        loop_end_right.link();
                                        loop_end_right.link_if_one();
                                        loop_end_right.set_roles();
                                        loop_end_right.root->color() = false;

                                        bool height_changed = insert(schedules, static_cast<node_end_t *>(loop_end_right.end), loop_end_right.leftmost_descendent_of_root, std::get<1>(bnode_to_be_splitted.keys));
                                        return std::make_tuple(height_left, height_right + (!height_changed ? 0 : 1));
                                    }
                                };
                                if(bnode_to_be_splitted.center_key_parent_info.is_end())
                                    split_when_all_information_are_ready(node_end_emit, static_cast<navigator_except_node_end_t *>(tagged_ptr_bit0_unsetted(node_end->child_right_all())), node_end, static_cast<navigator_except_node_end_t *>(tagged_ptr_bit0_unsetted(node_end->child_left_all())));
                                else
                                {
                                    bnode_up_t bnode_parent = bnode_up_t::get_bnode_from_key(static_cast<navigator_except_node_end_t *>(bnode_to_be_splitted.center_key_parent_info.parent), bnode_to_be_splitted.center_key_parent_info.is_left_or_right_child_of_parent);
                                    this_(this_, static_cast<bnode_t &>(bnode_parent), bnode_parent.child_index, split_when_all_information_are_ready);
                                }
                            }
                            else if(split_position == 2)
                            {
                                auto split_when_all_information_are_ready = [&schedules, &accumulator, &bnode_to_be_splitted](node_end_t *node_end_emit, navigator_except_node_end_t *leftmost_descendent_of_root, node_end_t *node_end, navigator_except_node_end_t *rightmost_descendent_of_root) -> std::tuple<int, int>
                                {
                                    assert(empty(node_end_emit));

                                    if(std::get<2>(bnode_to_be_splitted.keys).value()->child_right() == nullptr)
                                    {
                                        if(!std::get<0>(bnode_to_be_splitted.keys).has_value())
                                        {
                                            typename navigator_t::loop_end_t{
                                                .end = node_end_emit,
                                                .root = std::get<1>(bnode_to_be_splitted.keys),
                                            }
                                                .link();
                                            typename navigator_t::loop_t{
                                                .this_ = std::get<1>(bnode_to_be_splitted.keys),
                                            }
                                                .link();
                                            assert(std::get<1>(bnode_to_be_splitted.keys)->color() == false);
                                            assert(std::get<1>(bnode_to_be_splitted.keys)->role() == rb2p_node_role_e::root);
                                            refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_to_be_splitted.keys));
                                        }
                                        else
                                        {
                                            typename navigator_t::loop_end_t{
                                                .end = node_end_emit,
                                                .leftmost_descendent_of_root = std::get<0>(bnode_to_be_splitted.keys).value(),
                                                .root = std::get<1>(bnode_to_be_splitted.keys),
                                            }
                                                .link();
                                            typename navigator_t::loop_t{
                                                .this_ = std::get<1>(bnode_to_be_splitted.keys),
                                                .child_left = std::get<0>(bnode_to_be_splitted.keys).value(),
                                            }
                                                .link();
                                            assert(std::get<0>(bnode_to_be_splitted.keys).value()->color() == true);
                                            assert(std::get<0>(bnode_to_be_splitted.keys).value()->role() == rb2p_node_role_e::child_left_leftmost_descendent_of_root);
                                            assert(std::get<1>(bnode_to_be_splitted.keys)->color() == false);
                                            assert(std::get<1>(bnode_to_be_splitted.keys)->role() == rb2p_node_role_e::root);
                                            refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<0>(bnode_to_be_splitted.keys).value());
                                            refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_to_be_splitted.keys));
                                        }
                                        typename navigator_t::loop_end_t{
                                            .end = node_end,
                                            .root = std::get<2>(bnode_to_be_splitted.keys).value(),
                                        }
                                            .link();
                                        typename navigator_t::loop_t{
                                            .this_ = std::get<2>(bnode_to_be_splitted.keys).value(),
                                        }
                                            .link();
                                        std::get<2>(bnode_to_be_splitted.keys).value()->color() = false;
                                        std::get<2>(bnode_to_be_splitted.keys).value()->role() = rb2p_node_role_e::root;
                                        return std::make_tuple(0, 0);
                                    }
                                    else
                                    {
                                        typename navigator_t::loop_t inner_loop_right = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<2>(bnode_to_be_splitted.keys).value());
                                        typename navigator_t::loop_t inner_loop_left = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<2>(bnode_to_be_splitted.keys).value());

                                        navigator_except_node_end_t *key_right_child_left = std::get<2>(bnode_to_be_splitted.keys).value()->child_left(),
                                                                    *key_right_child_left_child_right = key_right_child_left->child_right(),
                                                                    *key_right_child_left_child_left = key_right_child_left->child_left();
                                        if(key_right_child_left_child_right == nullptr && key_right_child_left_child_left != nullptr)
                                        {
                                            navigator_except_node_end_t *node_right = key_right_child_left;
                                            navigator_except_node_end_t *node_left = key_right_child_left_child_left;
                                            typename navigator_t::loop_t{
                                                .this_ = node_left,
                                                .child_right = node_right,
                                            }
                                                .link();
                                            node_left->color() = false;
                                            node_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                            node_right->color() = true;
                                            node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                            std::tie(inner_loop_right.child_left, inner_loop_right.rightmost_descendent_of_child_left) = std::make_tuple(node_left, node_right);
                                            std::tie(inner_loop_left.child_right, inner_loop_left.leftmost_descendent_of_child_right) = std::make_tuple(node_left, nullptr);
                                            move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                            inner_loop_left.index = -1, inner_loop_left.link();
                                        }
                                        else
                                        {
                                            if(key_right_child_left_child_right == nullptr && key_right_child_left_child_left == nullptr)
                                                key_right_child_left->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                            else
                                                key_right_child_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                            inner_loop_left.exchange(key_right_child_left);
                                        }

                                        typename navigator_t::loop_end_t loop_end_right{
                                            .end = node_end,
                                            .leftmost_descendent_of_root = inner_loop_right.leftmost_descendent_of_child_right != nullptr ? inner_loop_right.leftmost_descendent_of_child_right : inner_loop_right.child_right,
                                            .root = inner_loop_right.child_right,
                                            .rightmost_descendent_of_root = rightmost_descendent_of_root,
                                        };
                                        loop_end_right.link();
                                        loop_end_right.link_if_one();
                                        loop_end_right.set_roles();
                                        assert(loop_end_right.root->color() == false);

                                        typename navigator_t::loop_end_t loop_end_left{
                                            .end = node_end_emit,
                                            .leftmost_descendent_of_root = leftmost_descendent_of_root,
                                            .root = inner_loop_left.this_,
                                            .rightmost_descendent_of_root = inner_loop_right.rightmost_descendent_of_child_left != nullptr ? inner_loop_right.rightmost_descendent_of_child_left : inner_loop_right.child_left,
                                        };
                                        loop_end_left.link();
                                        loop_end_left.set_roles();
                                        assert(loop_end_left.root->color() == false);
                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, loop_end_left.root);

                                        bool height_changed = insert(schedules, static_cast<node_end_t *>(loop_end_right.end), loop_end_right.leftmost_descendent_of_root, std::get<2>(bnode_to_be_splitted.keys).value());
                                        return std::make_tuple(0, (-1) + (!height_changed ? 0 : 1));
                                    }
                                };
                                if(bnode_to_be_splitted.center_key_parent_info.is_end())
                                    split_when_all_information_are_ready(node_end_emit, static_cast<navigator_except_node_end_t *>(tagged_ptr_bit0_unsetted(node_end->child_right_all())), node_end, static_cast<navigator_except_node_end_t *>(tagged_ptr_bit0_unsetted(node_end->child_left_all())));
                                else
                                {
                                    bnode_up_t bnode_parent = bnode_up_t::get_bnode_from_key(static_cast<navigator_except_node_end_t *>(bnode_to_be_splitted.center_key_parent_info.parent), bnode_to_be_splitted.center_key_parent_info.is_left_or_right_child_of_parent);
                                    this_(this_, static_cast<bnode_t &>(bnode_parent), bnode_parent.child_index, split_when_all_information_are_ready);
                                }
                            }
                            else if(split_position == -3)
                            {
                                auto split_when_all_information_are_ready = [&schedules, &accumulator, &bnode_to_be_splitted, &split_and_emit_to_node_end_emit](node_end_t *node_end_emit, navigator_except_node_end_t *leftmost_descendent_of_root, node_end_t *node_end, navigator_except_node_end_t *rightmost_descendent_of_root) -> std::tuple<int, int>
                                {
                                    assert(empty(node_end_emit));

                                    typename navigator_t::loop_t inner_loop_left = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<0>(bnode_to_be_splitted.keys).value());
                                    typename navigator_t::loop_t inner_loop_right = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<0>(bnode_to_be_splitted.keys).value());

                                    navigator_except_node_end_t *key_left_child_right = std::get<0>(bnode_to_be_splitted.keys).value()->child_right(),
                                                                *key_left_child_right_child_left = key_left_child_right->child_left(),
                                                                *key_left_child_right_child_right = key_left_child_right->child_right();
                                    if(key_left_child_right_child_left == nullptr && key_left_child_right_child_right != nullptr)
                                    {
                                        navigator_except_node_end_t *node_left = key_left_child_right;
                                        navigator_except_node_end_t *node_right = key_left_child_right_child_right;
                                        typename navigator_t::loop_t{
                                            .this_ = node_right,
                                            .child_left = node_left,
                                        }
                                            .link();
                                        node_right->color() = false;
                                        node_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                        node_left->color() = true;
                                        node_left->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                        std::tie(inner_loop_left.child_right, inner_loop_left.leftmost_descendent_of_child_right) = std::make_tuple(node_right, node_left);
                                        std::tie(inner_loop_right.child_left, inner_loop_right.rightmost_descendent_of_child_left) = std::make_tuple(node_right, nullptr);
                                        move_node_count_and_accumulated_storage<false>(schedules, accumulator, node_left, node_right);
                                        inner_loop_right.index = -1, inner_loop_right.link();
                                    }
                                    else
                                    {
                                        if(key_left_child_right_child_left == nullptr && key_left_child_right_child_right == nullptr)
                                            key_left_child_right->role() = rb2p_node_role_e::child_left_leftmost_descendent_of_non_root;
                                        else
                                            key_left_child_right->role() = rb2p_node_role_e::child_left_not_a_leftmost_descendent;
                                        inner_loop_right.exchange(key_left_child_right);
                                    }

                                    node_end_t node_end_middle;
                                    typename navigator_t::loop_end_t loop_end_left{
                                        .end = &node_end_middle,
                                        .leftmost_descendent_of_root = leftmost_descendent_of_root,
                                        .root = inner_loop_left.child_left,
                                        .rightmost_descendent_of_root = inner_loop_left.rightmost_descendent_of_child_left != nullptr ? inner_loop_left.rightmost_descendent_of_child_left : inner_loop_left.child_left,
                                    };
                                    loop_end_left.link();
                                    loop_end_left.link_if_one();
                                    loop_end_left.set_roles();
                                    assert(loop_end_left.root->color() == false);

                                    typename navigator_t::loop_end_t loop_end_right{
                                        .end = node_end,
                                        .leftmost_descendent_of_root = inner_loop_left.leftmost_descendent_of_child_right != nullptr ? inner_loop_left.leftmost_descendent_of_child_right : inner_loop_left.child_right,
                                        .root = inner_loop_right.this_,
                                        .rightmost_descendent_of_root = rightmost_descendent_of_root,
                                    };
                                    loop_end_right.link();
                                    loop_end_right.link_if_one();
                                    loop_end_right.set_roles();
                                    assert(loop_end_right.root->color() == false);

                                    std::tuple<int, int> height_changed_s = split_and_emit_to_node_end_emit(node_end_emit, loop_end_left.leftmost_descendent_of_root, static_cast<node_end_t *>(loop_end_left.end), loop_end_left.rightmost_descendent_of_root);
                                    bool height_changed = concat_with_middle_key<true, true>(schedules, static_cast<node_end_t *>(loop_end_left.end), std::get<0>(bnode_to_be_splitted.keys).value(), static_cast<node_end_t *>(loop_end_right.end), std::get<1>(height_changed_s) - 1);
                                    return std::make_tuple(std::get<0>(height_changed_s) - 1, (!height_changed ? 0 : 1));
                                };
                                if(bnode_to_be_splitted.center_key_parent_info.is_end())
                                    split_when_all_information_are_ready(node_end_emit, static_cast<navigator_except_node_end_t *>(tagged_ptr_bit0_unsetted(node_end->child_right_all())), node_end, static_cast<navigator_except_node_end_t *>(tagged_ptr_bit0_unsetted(node_end->child_left_all())));
                                else
                                {
                                    bnode_up_t bnode_parent = bnode_up_t::get_bnode_from_key(static_cast<navigator_except_node_end_t *>(bnode_to_be_splitted.center_key_parent_info.parent), bnode_to_be_splitted.center_key_parent_info.is_left_or_right_child_of_parent);
                                    this_(this_, static_cast<bnode_t &>(bnode_parent), bnode_parent.child_index, split_when_all_information_are_ready);
                                }
                            }
                            else if(split_position == -1)
                            {
                                auto split_when_all_information_are_ready = [&schedules, &bnode_to_be_splitted, &split_and_emit_to_node_end_emit](node_end_t *node_end_emit, navigator_except_node_end_t *leftmost_descendent_of_root, node_end_t *node_end, navigator_except_node_end_t *rightmost_descendent_of_root) -> std::tuple<int, int>
                                {
                                    assert(empty(node_end_emit));

                                    int height_right = !std::get<2>(bnode_to_be_splitted.keys).has_value() ? -1 : 0;

                                    if(!std::get<0>(bnode_to_be_splitted.keys).has_value())
                                    {
                                        typename navigator_t::loop_t inner_loop = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<1>(bnode_to_be_splitted.keys));

                                        node_end_t node_end_middle;
                                        typename navigator_t::loop_end_t loop_end_left{
                                            .end = &node_end_middle,
                                            .leftmost_descendent_of_root = leftmost_descendent_of_root,
                                            .root = inner_loop.child_left,
                                            .rightmost_descendent_of_root = inner_loop.rightmost_descendent_of_child_left != nullptr ? inner_loop.rightmost_descendent_of_child_left : inner_loop.child_left,
                                        };
                                        loop_end_left.link();
                                        loop_end_left.link_if_one();
                                        loop_end_left.set_roles();
                                        assert(loop_end_left.root->color() == false);

                                        typename navigator_t::loop_end_t loop_end_right{
                                            .end = node_end,
                                            .leftmost_descendent_of_root = inner_loop.leftmost_descendent_of_child_right != nullptr ? inner_loop.leftmost_descendent_of_child_right : inner_loop.child_right,
                                            .root = inner_loop.child_right,
                                            .rightmost_descendent_of_root = rightmost_descendent_of_root,
                                        };
                                        loop_end_right.link();
                                        loop_end_right.link_if_one();
                                        loop_end_right.set_roles();
                                        loop_end_right.root->color() = false;

                                        std::tuple<int, int> height_changed_s = split_and_emit_to_node_end_emit(node_end_emit, loop_end_left.leftmost_descendent_of_root, static_cast<node_end_t *>(loop_end_left.end), loop_end_left.rightmost_descendent_of_root);
                                        bool height_changed = concat_with_middle_key<true, true>(schedules, static_cast<node_end_t *>(loop_end_left.end), std::get<1>(bnode_to_be_splitted.keys), static_cast<node_end_t *>(loop_end_right.end), (std::get<1>(height_changed_s) - 1) - height_right);
                                        return std::make_tuple(std::get<0>(height_changed_s) - 1, (!height_changed ? 0 : 1) + height_right);
                                    }
                                    else
                                    {
                                        typename navigator_t::loop_t inner_loop_left = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<0>(bnode_to_be_splitted.keys).value());
                                        typename navigator_t::loop_t inner_loop_right = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<1>(bnode_to_be_splitted.keys));

                                        typename navigator_t::loop_end_t loop_end_left{
                                            .end = node_end_emit,
                                            .leftmost_descendent_of_root = leftmost_descendent_of_root,
                                            .root = inner_loop_left.child_left,
                                            .rightmost_descendent_of_root = inner_loop_left.rightmost_descendent_of_child_left != nullptr ? inner_loop_left.rightmost_descendent_of_child_left : inner_loop_left.child_left,
                                        };
                                        loop_end_left.link();
                                        loop_end_left.link_if_one();
                                        loop_end_left.set_roles();
                                        assert(loop_end_left.root->color() == false);

                                        node_end_t node_end_middle;
                                        typename navigator_t::loop_end_t loop_end_middle{
                                            .end = &node_end_middle,
                                            .leftmost_descendent_of_root = inner_loop_left.leftmost_descendent_of_child_right != nullptr ? inner_loop_left.leftmost_descendent_of_child_right : inner_loop_left.child_right,
                                            .root = inner_loop_left.child_right,
                                            .rightmost_descendent_of_root = inner_loop_right.rightmost_descendent_of_child_left != nullptr ? inner_loop_right.rightmost_descendent_of_child_left : inner_loop_right.child_left,
                                        };
                                        loop_end_middle.link();
                                        loop_end_middle.link_if_one();
                                        loop_end_middle.set_roles();
                                        assert(loop_end_middle.root->color() == false);

                                        typename navigator_t::loop_end_t loop_end_right{
                                            .end = node_end,
                                            .leftmost_descendent_of_root = inner_loop_right.leftmost_descendent_of_child_right != nullptr ? inner_loop_right.leftmost_descendent_of_child_right : inner_loop_right.child_right,
                                            .root = inner_loop_right.child_right,
                                            .rightmost_descendent_of_root = rightmost_descendent_of_root,
                                        };
                                        loop_end_right.link();
                                        loop_end_right.link_if_one();
                                        loop_end_right.set_roles();
                                        loop_end_right.root->color() = false;

                                        node_end_t node_end_left_middle;
                                        std::tuple<int, int> height_changed_s = split_and_emit_to_node_end_emit(&node_end_left_middle, loop_end_middle.leftmost_descendent_of_root, static_cast<node_end_t *>(loop_end_middle.end), loop_end_middle.rightmost_descendent_of_root);
                                        bool height_changed_lhs = concat_with_middle_key<false, true>(schedules, static_cast<node_end_t *>(loop_end_left.end), std::get<0>(bnode_to_be_splitted.keys).value(), &node_end_left_middle, -std::get<0>(height_changed_s));
                                        bool height_changed_rhs = concat_with_middle_key<true, true>(schedules, static_cast<node_end_t *>(loop_end_middle.end), std::get<1>(bnode_to_be_splitted.keys), static_cast<node_end_t *>(loop_end_right.end), (std::get<1>(height_changed_s) - 1) - height_right);
                                        return std::make_tuple((!height_changed_lhs ? 0 : 1) - 1, (!height_changed_rhs ? 0 : 1) + height_right);
                                    }
                                };
                                if(bnode_to_be_splitted.center_key_parent_info.is_end())
                                    split_when_all_information_are_ready(node_end_emit, static_cast<navigator_except_node_end_t *>(tagged_ptr_bit0_unsetted(node_end->child_right_all())), node_end, static_cast<navigator_except_node_end_t *>(tagged_ptr_bit0_unsetted(node_end->child_left_all())));
                                else
                                {
                                    bnode_up_t bnode_parent = bnode_up_t::get_bnode_from_key(static_cast<navigator_except_node_end_t *>(bnode_to_be_splitted.center_key_parent_info.parent), bnode_to_be_splitted.center_key_parent_info.is_left_or_right_child_of_parent);
                                    this_(this_, static_cast<bnode_t &>(bnode_parent), bnode_parent.child_index, split_when_all_information_are_ready);
                                }
                            }
                            else if(split_position == 1)
                            {
                                auto split_when_all_information_are_ready = [&schedules, &bnode_to_be_splitted, &split_and_emit_to_node_end_emit](node_end_t *node_end_emit, navigator_except_node_end_t *leftmost_descendent_of_root, node_end_t *node_end, navigator_except_node_end_t *rightmost_descendent_of_root) -> std::tuple<int, int>
                                {
                                    assert(empty(node_end_emit));

                                    int height_left = !std::get<0>(bnode_to_be_splitted.keys).has_value() ? -1 : 0;

                                    if(!std::get<2>(bnode_to_be_splitted.keys).has_value())
                                    {
                                        typename navigator_t::loop_t inner_loop = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<1>(bnode_to_be_splitted.keys));

                                        typename navigator_t::loop_end_t loop_end_right{
                                            .end = node_end,
                                            .leftmost_descendent_of_root = inner_loop.leftmost_descendent_of_child_right != nullptr ? inner_loop.leftmost_descendent_of_child_right : inner_loop.child_right,
                                            .root = inner_loop.child_right,
                                            .rightmost_descendent_of_root = rightmost_descendent_of_root,
                                        };
                                        loop_end_right.link();
                                        loop_end_right.link_if_one();
                                        loop_end_right.set_roles();
                                        assert(loop_end_right.root->color() == false);

                                        typename navigator_t::loop_end_t loop_end_left{
                                            .end = node_end_emit,
                                            .leftmost_descendent_of_root = leftmost_descendent_of_root,
                                            .root = inner_loop.child_left,
                                            .rightmost_descendent_of_root = inner_loop.rightmost_descendent_of_child_left != nullptr ? inner_loop.rightmost_descendent_of_child_left : inner_loop.child_left,
                                        };
                                        loop_end_left.link();
                                        loop_end_left.link_if_one();
                                        loop_end_left.set_roles();
                                        loop_end_left.root->color() = false;

                                        node_end_t node_end_middle;
                                        std::tuple<int, int> height_changed_s = split_and_emit_to_node_end_emit(&node_end_middle, loop_end_right.leftmost_descendent_of_root, static_cast<node_end_t *>(loop_end_right.end), loop_end_right.rightmost_descendent_of_root);
                                        bool height_changed = concat_with_middle_key<false, true>(schedules, static_cast<node_end_t *>(loop_end_left.end), std::get<1>(bnode_to_be_splitted.keys), &node_end_middle, height_left - (std::get<0>(height_changed_s) - 1));
                                        return std::make_tuple((!height_changed ? 0 : 1) + height_left, std::get<1>(height_changed_s) - 1);
                                    }
                                    else
                                    {
                                        typename navigator_t::loop_t inner_loop_right = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<2>(bnode_to_be_splitted.keys).value());
                                        typename navigator_t::loop_t inner_loop_left = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<1>(bnode_to_be_splitted.keys));

                                        typename navigator_t::loop_end_t loop_end_right{
                                            .end = node_end,
                                            .leftmost_descendent_of_root = inner_loop_right.leftmost_descendent_of_child_right != nullptr ? inner_loop_right.leftmost_descendent_of_child_right : inner_loop_right.child_right,
                                            .root = inner_loop_right.child_right,
                                            .rightmost_descendent_of_root = rightmost_descendent_of_root,
                                        };
                                        loop_end_right.link();
                                        loop_end_right.link_if_one();
                                        loop_end_right.set_roles();
                                        assert(loop_end_right.root->color() == false);

                                        node_end_t node_end_middle;
                                        typename navigator_t::loop_end_t loop_end_middle{
                                            .end = &node_end_middle,
                                            .leftmost_descendent_of_root = inner_loop_left.leftmost_descendent_of_child_right != nullptr ? inner_loop_left.leftmost_descendent_of_child_right : inner_loop_left.child_right,
                                            .root = inner_loop_right.child_left,
                                            .rightmost_descendent_of_root = inner_loop_right.rightmost_descendent_of_child_left != nullptr ? inner_loop_right.rightmost_descendent_of_child_left : inner_loop_right.child_left,
                                        };
                                        loop_end_middle.link();
                                        loop_end_middle.link_if_one();
                                        loop_end_middle.set_roles();
                                        assert(loop_end_middle.root->color() == false);

                                        typename navigator_t::loop_end_t loop_end_left{
                                            .end = node_end_emit,
                                            .leftmost_descendent_of_root = leftmost_descendent_of_root,
                                            .root = inner_loop_left.child_left,
                                            .rightmost_descendent_of_root = inner_loop_left.rightmost_descendent_of_child_left != nullptr ? inner_loop_left.rightmost_descendent_of_child_left : inner_loop_left.child_left,
                                        };
                                        loop_end_left.link();
                                        loop_end_left.link_if_one();
                                        loop_end_left.set_roles();
                                        loop_end_left.root->color() = false;

                                        node_end_t node_end_left_middle;
                                        std::tuple<int, int> height_changed_s = split_and_emit_to_node_end_emit(&node_end_left_middle, loop_end_middle.leftmost_descendent_of_root, static_cast<node_end_t *>(loop_end_middle.end), loop_end_middle.rightmost_descendent_of_root);
                                        bool height_changed_rhs = concat_with_middle_key<true, true>(schedules, static_cast<node_end_t *>(loop_end_middle.end), std::get<2>(bnode_to_be_splitted.keys).value(), static_cast<node_end_t *>(loop_end_right.end), std::get<1>(height_changed_s));
                                        bool height_changed_lhs = concat_with_middle_key<false, true>(schedules, static_cast<node_end_t *>(loop_end_left.end), std::get<1>(bnode_to_be_splitted.keys), &node_end_left_middle, height_left - (std::get<0>(height_changed_s) - 1));
                                        return std::make_tuple((!height_changed_lhs ? 0 : 1) + height_left, (!height_changed_rhs ? 0 : 1) - 1);
                                    }
                                };
                                if(bnode_to_be_splitted.center_key_parent_info.is_end())
                                    split_when_all_information_are_ready(node_end_emit, static_cast<navigator_except_node_end_t *>(tagged_ptr_bit0_unsetted(node_end->child_right_all())), node_end, static_cast<navigator_except_node_end_t *>(tagged_ptr_bit0_unsetted(node_end->child_left_all())));
                                else
                                {
                                    bnode_up_t bnode_parent = bnode_up_t::get_bnode_from_key(static_cast<navigator_except_node_end_t *>(bnode_to_be_splitted.center_key_parent_info.parent), bnode_to_be_splitted.center_key_parent_info.is_left_or_right_child_of_parent);
                                    this_(this_, static_cast<bnode_t &>(bnode_parent), bnode_parent.child_index, split_when_all_information_are_ready);
                                }
                            }
                            else if(split_position == 3)
                            {
                                auto split_when_all_information_are_ready = [&schedules, &accumulator, &bnode_to_be_splitted, &split_and_emit_to_node_end_emit](node_end_t *node_end_emit, navigator_except_node_end_t *leftmost_descendent_of_root, node_end_t *node_end, navigator_except_node_end_t *rightmost_descendent_of_root) -> std::tuple<int, int>
                                {
                                    assert(empty(node_end_emit));

                                    typename navigator_t::loop_t inner_loop_right = navigator_t::template get_loop<navigator_t::loop_type_e::my_list>(std::get<2>(bnode_to_be_splitted.keys).value());
                                    typename navigator_t::loop_t inner_loop_left = navigator_t::template get_loop<navigator_t::loop_type_e::parent>(std::get<2>(bnode_to_be_splitted.keys).value());

                                    navigator_except_node_end_t *key_right_child_left = std::get<2>(bnode_to_be_splitted.keys).value()->child_left(),
                                                                *key_right_child_left_child_right = key_right_child_left->child_right(),
                                                                *key_right_child_left_child_left = key_right_child_left->child_left();
                                    if(key_right_child_left_child_right == nullptr && key_right_child_left_child_left != nullptr)
                                    {
                                        navigator_except_node_end_t *node_right = key_right_child_left;
                                        navigator_except_node_end_t *node_left = key_right_child_left_child_left;
                                        typename navigator_t::loop_t{
                                            .this_ = node_left,
                                            .child_right = node_right,
                                        }
                                            .link();
                                        node_left->color() = false;
                                        node_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                        node_right->color() = true;
                                        node_right->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                        std::tie(inner_loop_right.child_left, inner_loop_right.rightmost_descendent_of_child_left) = std::make_tuple(node_left, node_right);
                                        std::tie(inner_loop_left.child_right, inner_loop_left.leftmost_descendent_of_child_right) = std::make_tuple(node_left, nullptr);
                                        move_node_count_and_accumulated_storage<true>(schedules, accumulator, node_left, node_right);
                                        inner_loop_left.index = -1, inner_loop_left.link();
                                    }
                                    else
                                    {
                                        if(key_right_child_left_child_right == nullptr && key_right_child_left_child_left == nullptr)
                                            key_right_child_left->role() = rb2p_node_role_e::child_right_rightmost_descendent_of_non_root;
                                        else
                                            key_right_child_left->role() = rb2p_node_role_e::child_right_not_a_rightmost_descendent;
                                        inner_loop_left.exchange(key_right_child_left);
                                    }

                                    typename navigator_t::loop_end_t loop_end_right{
                                        .end = node_end,
                                        .leftmost_descendent_of_root = inner_loop_right.leftmost_descendent_of_child_right != nullptr ? inner_loop_right.leftmost_descendent_of_child_right : inner_loop_right.child_right,
                                        .root = inner_loop_right.child_right,
                                        .rightmost_descendent_of_root = rightmost_descendent_of_root,
                                    };
                                    loop_end_right.link();
                                    loop_end_right.link_if_one();
                                    loop_end_right.set_roles();
                                    assert(loop_end_right.root->color() == false);

                                    typename navigator_t::loop_end_t loop_end_left{
                                        .end = node_end_emit,
                                        .leftmost_descendent_of_root = leftmost_descendent_of_root,
                                        .root = inner_loop_left.this_,
                                        .rightmost_descendent_of_root = inner_loop_right.rightmost_descendent_of_child_left != nullptr ? inner_loop_right.rightmost_descendent_of_child_left : inner_loop_right.child_left,
                                    };
                                    loop_end_left.link();
                                    loop_end_left.link_if_one();
                                    loop_end_left.set_roles();
                                    assert(loop_end_left.root->color() == false);

                                    node_end_t node_end_middle;
                                    std::tuple<int, int> height_changed_s = split_and_emit_to_node_end_emit(&node_end_middle, loop_end_right.leftmost_descendent_of_root, static_cast<node_end_t *>(loop_end_right.end), loop_end_right.rightmost_descendent_of_root);
                                    bool height_changed = concat_with_middle_key<false, true>(schedules, static_cast<node_end_t *>(loop_end_left.end), std::get<2>(bnode_to_be_splitted.keys).value(), &node_end_middle, -(std::get<0>(height_changed_s) - 1));
                                    return std::make_tuple((!height_changed ? 0 : 1), std::get<1>(height_changed_s) - 1);
                                };
                                if(bnode_to_be_splitted.center_key_parent_info.is_end())
                                    split_when_all_information_are_ready(node_end_emit, static_cast<navigator_except_node_end_t *>(tagged_ptr_bit0_unsetted(node_end->child_right_all())), node_end, static_cast<navigator_except_node_end_t *>(tagged_ptr_bit0_unsetted(node_end->child_left_all())));
                                else
                                {
                                    bnode_up_t bnode_parent = bnode_up_t::get_bnode_from_key(static_cast<navigator_except_node_end_t *>(bnode_to_be_splitted.center_key_parent_info.parent), bnode_to_be_splitted.center_key_parent_info.is_left_or_right_child_of_parent);
                                    this_(this_, static_cast<bnode_t &>(bnode_parent), bnode_parent.child_index, split_when_all_information_are_ready);
                                }
                            }
                            else std::unreachable();
                        };
                        bnode_erase_t bnode_to_be_splitted = bnode_erase_t::erasing_get_bnode(static_cast<navigator_except_node_end_t *>(node));
                        split_impl(split_impl, static_cast<bnode_t>(bnode_to_be_splitted), bnode_to_be_splitted.key_to_be_erased_index, [](node_end_t *, navigator_except_node_end_t *, node_end_t *, navigator_except_node_end_t *) -> std::tuple<int, int>
                            { return {}; });
                        if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_node_end)
                        {
                            std::size_t size_emitted = std::ranges::distance(std::ranges::next(rb2p_iterator_t<false, false, config_t>(node_end_emit)), rb2p_iterator_t<false, false, config_t>(node_end_emit));
                            std::tie(node_end_emit->node_count, node_end->node_count) = std::make_tuple(size_emitted, size_sum - size_emitted);
                        }
                        if constexpr(!emit_left_or_right)
                            ;
                        else
                            swap_root(node_end_emit, node_end);
                    }
                }

                static accumulated_storage_t read_range_impl(allocator_element_t const &allocator_element, node_end_t *node_end, navigator_except_node_end_t *node_front, navigator_except_node_end_t *node_back)
                {
                    if(node_front == node_back)
                        return node_end->accumulator.construct_accumulated_storage(allocator_element, std::make_tuple(std::cref(*static_cast<node_t *>(node_front)->p_element())));

                    struct path_vertex_t
                    {
                        path_vertex_t *next;
                        bool child_left_or_child_right;
                        navigator_except_node_end_t *node;
                    } path_vertex_front{
                        .next = nullptr,
                        .child_left_or_child_right = false,
                        .node = node_front,
                    },
                        *path_front = &path_vertex_front,
                        path_vertex_back{
                            .next = nullptr,
                            .child_left_or_child_right = true,
                            .node = node_back,
                        },
                        *path_back = &path_vertex_back;
                    auto build_path = [&accumulator = std::as_const(node_end->accumulator), &allocator_element](auto &this_, path_vertex_t *path_front, path_vertex_t *path_back) -> accumulated_storage_t
                    {
                        typename navigator_t::template parent_info_t<is_reversed> parent_info_front(path_front->node), parent_info_back(path_back->node);
                        if(!parent_info_front.is_end() || !parent_info_back.is_end())
                        {
                            path_vertex_t path_vertex_front, path_vertex_back;
                            if(!parent_info_front.is_end())
                            {
                                path_vertex_front = {
                                    .next = path_front,
                                    .child_left_or_child_right = parent_info_front.is_left_or_right_child_of_parent,
                                    .node = static_cast<navigator_except_node_end_t *>(parent_info_front.parent),
                                };
                                path_front = &path_vertex_front;
                            }
                            if(!parent_info_back.is_end())
                            {
                                path_vertex_back = {
                                    .next = path_back,
                                    .child_left_or_child_right = parent_info_back.is_left_or_right_child_of_parent,
                                    .node = static_cast<navigator_except_node_end_t *>(parent_info_back.parent),
                                };
                                path_back = &path_vertex_back;
                            }
                            return this_(this_, path_front, path_back);
                        }
                        else
                        {
                            assert(path_front->node == path_back->node);
                            while(path_front->next != nullptr && path_back->next != nullptr && path_front->next->node == path_back->next->node)
                            {
                                path_front = path_front->next;
                                path_back = path_back->next;
                            }
                            auto accumulate_left = [&accumulator, &allocator_element](auto &this_, path_vertex_t *path_vertex_front) -> accumulated_storage_t
                            {
                                if(path_vertex_front->next == nullptr)
                                {
                                    if(path_vertex_front->node->child_right() == nullptr)
                                        return accumulator.construct_accumulated_storage(allocator_element, std::make_tuple(std::cref(*static_cast<node_t *>(path_vertex_front->node)->p_element())));
                                    else
                                        return accumulator.construct_accumulated_storage(allocator_element, std::make_tuple(std::cref(*static_cast<node_t *>(path_vertex_front->node)->p_element()), std::ref(*static_cast<node_t *>(static_cast<navigator_except_node_end_t *>(path_vertex_front->node->child_right()))->p_accumulated_storage())));
                                }
                                else
                                {
                                    if(path_vertex_front->child_left_or_child_right == false)
                                    {
                                        accumulated_storage_t intermediate_accumulated_storage_left(this_(this_, path_vertex_front->next));
                                        if(path_vertex_front->node->child_right() == nullptr)
                                            return accumulator.construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(intermediate_accumulated_storage_left), std::cref(*static_cast<node_t *>(path_vertex_front->node)->p_element())));
                                        else
                                            return accumulator.construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(intermediate_accumulated_storage_left), std::cref(*static_cast<node_t *>(path_vertex_front->node)->p_element()), std::ref(*static_cast<node_t *>(static_cast<navigator_except_node_end_t *>(path_vertex_front->node->child_right()))->p_accumulated_storage())));
                                    }
                                    else
                                        return this_(this_, path_vertex_front->next);
                                }
                            };
                            auto accumulate_right = [&accumulator, &allocator_element](auto &this_, path_vertex_t *path_vertex_back) -> accumulated_storage_t
                            {
                                if(path_vertex_back->next == nullptr)
                                {
                                    if(path_vertex_back->node->child_left() == nullptr)
                                        return accumulator.construct_accumulated_storage(allocator_element, std::make_tuple(std::cref(*static_cast<node_t *>(path_vertex_back->node)->p_element())));
                                    else
                                        return accumulator.construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(*static_cast<node_t *>(static_cast<navigator_except_node_end_t *>(path_vertex_back->node->child_left()))->p_accumulated_storage()), std::cref(*static_cast<node_t *>(path_vertex_back->node)->p_element())));
                                }
                                else
                                {
                                    if(path_vertex_back->child_left_or_child_right == true)
                                    {
                                        accumulated_storage_t intermediate_accumulated_storage_right(this_(this_, path_vertex_back->next));
                                        if(path_vertex_back->node->child_left() == nullptr)
                                            return accumulator.construct_accumulated_storage(allocator_element, std::make_tuple(std::cref(*static_cast<node_t *>(path_vertex_back->node)->p_element()), std::ref(intermediate_accumulated_storage_right)));
                                        else
                                            return accumulator.construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(*static_cast<node_t *>(static_cast<navigator_except_node_end_t *>(path_vertex_back->node->child_left()))->p_accumulated_storage()), std::cref(*static_cast<node_t *>(path_vertex_back->node)->p_element()), std::ref(intermediate_accumulated_storage_right)));
                                    }
                                    else
                                        return this_(this_, path_vertex_back->next);
                                }
                            };

                            auto get_left_operand = [&](auto return_accumulated_tuple)
                            { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> accumulated_storage_t
                              {
                                  if(path_front->next == nullptr)
                                      return return_accumulated_tuple(accumulated_tuple_so_far);
                                  else
                                  {
                                      accumulated_storage_t intermediate_accumulated_storage_left(accumulate_left(accumulate_left, path_front->next));
                                      return return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(intermediate_accumulated_storage_left))));
                                  }
                              }; };
                            auto get_middle_operand = [&](auto return_accumulated_tuple)
                            { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> accumulated_storage_t
                              {
                                  return return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::cref(*static_cast<node_t *>(path_front->node)->p_element()))));
                              }; };
                            auto get_right_operand = [&](auto return_accumulated_tuple)
                            { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> accumulated_storage_t
                              {
                                  if(path_back->next == nullptr)
                                      return return_accumulated_tuple(accumulated_tuple_so_far);
                                  else
                                  {
                                      accumulated_storage_t intermediate_accumulated_storage_right(accumulate_right(accumulate_right, path_back->next));
                                      return return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(intermediate_accumulated_storage_right))));
                                  }
                              }; };
                            auto return_accumulated_tuple = [&](auto accumulated_tuple_so_far) -> accumulated_storage_t
                            {
                                return accumulator.construct_accumulated_storage(allocator_element, accumulated_tuple_so_far);
                            };
                            return get_left_operand(get_middle_operand(get_right_operand(return_accumulated_tuple)))(std::make_tuple());
                        }
                    };
                    return build_path(build_path, path_front, path_back);
                }

                static void update_range_impl(node_end_t *node_end, navigator_except_node_end_t *node_front, navigator_except_node_end_t *node_back)
                {
                    accumulator_t const &accumulator = node_end->accumulator;
                    auto refresh_tree_and_find_back = [&accumulator, &node_back](auto &this_, navigator_except_node_end_t *root) -> bool
                    {
                        bool found = false;
                        if(root->child_left() != nullptr)
                            found = found || this_(this_, root->child_left());
                        found = found || root == node_back;
                        if(!found && root->child_right() != nullptr)
                            found = found || this_(this_, root->child_right());
                        refresh_accumulated_storage(accumulator, root);
                        return found;
                    };
                    if(node_front == node_back)
                        refresh_accumulated_storage_and_above(accumulator, node_front);
                    else if(node_front->child_right() != nullptr && refresh_tree_and_find_back(refresh_tree_and_find_back, node_front->child_right()))
                        refresh_accumulated_storage_and_above(accumulator, node_front);
                    else
                    {
                        refresh_accumulated_storage(accumulator, node_front);
                        while(true)
                        {
                            typename navigator_t::template parent_info_t<is_reversed> parent_info(node_front);
                            assert(!parent_info.is_end());
                            node_front = static_cast<navigator_except_node_end_t *>(parent_info.parent);
                            if(!parent_info.is_left_or_right_child_of_parent)
                            {
                                if(node_front == node_back)
                                {
                                    refresh_accumulated_storage_and_above(accumulator, node_front);
                                    break;
                                }
                                else if(node_front->child_right() != nullptr && refresh_tree_and_find_back(refresh_tree_and_find_back, node_front->child_right()))
                                {
                                    refresh_accumulated_storage_and_above(accumulator, node_front);
                                    break;
                                }
                                else
                                    refresh_accumulated_storage(accumulator, node_front);
                            }
                            else
                                refresh_accumulated_storage(accumulator, node_front);
                        }
                    }
                }


                template<detail::concepts::invocable_r<bool, accumulated_storage_t const &> monotonic_predicate_t>
                static navigator_t *find_by_monotonic_predicate(allocator_element_t const &allocator_element, node_end_t *node_end, monotonic_predicate_t const &monotonic_predicate)
                {
                    if(empty(node_end))
                        return node_end;
                    navigator_except_node_end_t *root = static_cast<navigator_except_node_end_t *>(tagged_ptr_bit0_unsetted(node_end->parent_all()));
                    if(!monotonic_predicate(*static_cast<node_t *>(root)->p_accumulated_storage()))
                        return node_end;

                    accumulator_t const &accumulator = node_end->accumulator;
                    auto search_tree = [&accumulator, &allocator_element, &monotonic_predicate](auto &this_, auto accumulated_storage_tuple_so_far, navigator_except_node_end_t *node) -> navigator_except_node_end_t *
                    {
                        if(node->child_left() == nullptr)
                        {
                            accumulated_storage_t intermediate_accumulated_storage(accumulator.construct_accumulated_storage(allocator_element, std::tuple_cat(accumulated_storage_tuple_so_far, std::make_tuple(std::cref(*static_cast<node_t *>(node)->p_element())))));
                            if(monotonic_predicate(intermediate_accumulated_storage))
                                return node;
                            else
                            {
                                assert(node->child_right() != nullptr);
                                return this_(this_, std::make_tuple(std::ref(intermediate_accumulated_storage)), node->child_right());
                            }
                        }
                        else
                        {
                            accumulated_storage_t intermediate_accumulated_storage_left(accumulator.construct_accumulated_storage(allocator_element, std::tuple_cat(accumulated_storage_tuple_so_far, std::make_tuple(std::ref(*static_cast<node_t *>(static_cast<navigator_except_node_end_t *>(node->child_left()))->p_accumulated_storage())))));
                            if(monotonic_predicate(intermediate_accumulated_storage_left))
                                return this_(this_, accumulated_storage_tuple_so_far, node->child_left());
                            else
                            {
                                accumulated_storage_t intermediate_accumulated_storage_right(accumulator.construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(intermediate_accumulated_storage_left), std::cref(*static_cast<node_t *>(node)->p_element()))));
                                if(monotonic_predicate(intermediate_accumulated_storage_right))
                                    return node;
                                else
                                {
                                    assert(node->child_right() != nullptr);
                                    return this_(this_, std::make_tuple(std::ref(intermediate_accumulated_storage_right)), node->child_right());
                                }
                            }
                        }
                    };
                    return search_tree(search_tree, std::make_tuple(), root);
                }

                template</*std::output_iterator<navigator_except_node_end_t*>*/ typename iterator_output_pointer_node_t, typename heap_predicate_t>
                    requires(invocable_r<heap_predicate_t, bool, accumulated_storage_t &> && invocable_r<heap_predicate_t, bool, element_t const &>)
                static void find_by_heap_predicate(node_end_t *node_end, iterator_output_pointer_node_t iterator_output_pointer_node, heap_predicate_t const &heap_predicate)
                {
                    if(empty(node_end))
                        return;
                    navigator_except_node_end_t *root = static_cast<navigator_except_node_end_t *>(tagged_ptr_bit0_unsetted(node_end->parent_all()));
                    auto search_tree = [&heap_predicate, &iterator_output_pointer_node](auto &this_, navigator_except_node_end_t *node) -> void
                    {
                        if(node->child_left() != nullptr)
                        {
                            if(heap_predicate(*static_cast<node_t *>(static_cast<navigator_except_node_end_t *>(node->child_left()))->p_accumulated_storage()))
                                this_(this_, node->child_left());
                        }
                        if(heap_predicate(std::as_const(*static_cast<node_t *>(node)->p_element())))
                            *iterator_output_pointer_node++ = node;
                        if(node->child_right() != nullptr)
                        {
                            if(heap_predicate(*static_cast<node_t *>(static_cast<navigator_except_node_end_t *>(node->child_right()))->p_accumulated_storage()))
                                this_(this_, node->child_right());
                        }
                    };
                    if(heap_predicate(*static_cast<node_t *>(root)->p_accumulated_storage()))
                        search_tree(search_tree, root);
                }

                template<typename heap_predicate_t>
                    requires(invocable_r<heap_predicate_t, bool, accumulated_storage_t &> && invocable_r<heap_predicate_t, bool, element_t const &>)
                static generator_t<navigator_except_node_end_t *> find_by_heap_predicate_generator(node_end_t *node_end, heap_predicate_t const &heap_predicate)
                {
                    if(empty(node_end))
                        co_return;
                    navigator_except_node_end_t *root = static_cast<navigator_except_node_end_t *>(tagged_ptr_bit0_unsetted(node_end->parent_all()));
                    auto search_tree = [](heap_predicate_t const &heap_predicate, auto &this_, navigator_except_node_end_t *node) -> generator_t<navigator_except_node_end_t *>
                    {
                        if(node->child_left() != nullptr)
                        {
                            if(heap_predicate(*static_cast<node_t *>(static_cast<navigator_except_node_end_t *>(node->child_left()))->p_accumulated_storage()))
                                co_yield this_(heap_predicate, this_, node->child_left());
                        }
                        if(heap_predicate(std::as_const(*static_cast<node_t *>(node)->p_element())))
                            co_yield node;
                        if(node->child_right() != nullptr)
                        {
                            if(heap_predicate(*static_cast<node_t *>(static_cast<navigator_except_node_end_t *>(node->child_right()))->p_accumulated_storage()))
                                co_yield this_(heap_predicate, this_, node->child_right());
                        }
                    };
                    if(heap_predicate(*static_cast<node_t *>(root)->p_accumulated_storage()))
                        co_yield search_tree(heap_predicate, search_tree, root);
                }
            };
        } // namespace augmented_sequence_rb2p
    } // namespace detail

#ifndef AUGMENTED_CONTAINERS_AUGMENTED_SEQUENCE_HELPERS
    #define AUGMENTED_CONTAINERS_AUGMENTED_SEQUENCE_HELPERS
    namespace augmented_sequence_helpers
    {
        template<typename accumulating_binary_functor_t>
        struct extract_builtin_accumulated_storage_from_accumulating_binary_functor: std::type_identity<void>
        {
        };
        template<typename accumulating_binary_functor_t>
            requires requires { typename accumulating_binary_functor_t::accumulated_storage_t; }
        struct extract_builtin_accumulated_storage_from_accumulating_binary_functor<accumulating_binary_functor_t>: std::type_identity<typename accumulating_binary_functor_t::accumulated_storage_t>
        {
        };
        template<typename accumulating_binary_functor_t>
        using extract_builtin_accumulated_storage_from_accumulating_binary_functor_t = typename extract_builtin_accumulated_storage_from_accumulating_binary_functor<accumulating_binary_functor_t>::type;

        template<std::size_t index, typename accumulating_binary_functor_t, typename Tuple>
        decltype(auto) tuple_fold(accumulating_binary_functor_t const &accumulate_binary_functor, Tuple const &t) // https://stackoverflow.com/questions/47216057/how-to-write-a-fold-sum-function-for-c-tuple
        {
            if constexpr(index == 0)
            {
                if constexpr(requires { accumulate_binary_functor.identity_element(); })
                    return accumulate_binary_functor(detail::utility::unmove(accumulate_binary_functor.identity_element()), std::get<index>(t));
                else
                    return std::get<index>(t);
            }
            else
                return accumulate_binary_functor(detail::utility::unmove(tuple_fold<index - 1>(accumulate_binary_functor, t)), std::get<index>(t));
        }

        template<typename accumulating_binary_functor_t_ = void>
        struct accumulator_wrapping_accumulating_binary_functor_t
        {
            using accumulated_storage_t = void; // void means to skip the accumulate operation
        };
        template<typename accumulating_binary_functor_t_>
            requires(!std::is_same_v<extract_builtin_accumulated_storage_from_accumulating_binary_functor_t<accumulating_binary_functor_t_>, void>)
        struct accumulator_wrapping_accumulating_binary_functor_t<accumulating_binary_functor_t_>
        {
            using accumulated_storage_t = extract_builtin_accumulated_storage_from_accumulating_binary_functor_t<accumulating_binary_functor_t_>;
            accumulating_binary_functor_t_ accumulate_binary_functor;

            template<typename allocator_element_t, typename... Args>
            accumulated_storage_t construct_accumulated_storage([[maybe_unused]] allocator_element_t const &allocator_element, std::tuple<Args &...> const &values) const
            {
                if constexpr(sizeof...(Args) == 0)
                {
                    if constexpr(requires { accumulate_binary_functor.identity_element(); })
                        return accumulated_storage_t(accumulate_binary_functor.identity_element());
                    else
                        return accumulated_storage_t();
                }
                else
                    return accumulated_storage_t(tuple_fold<sizeof...(Args) - 1>(accumulate_binary_functor, values));
            }
            template<typename allocator_element_t, typename... Args>
            void construct_accumulated_storage(allocator_element_t const &allocator_element, typename std::pointer_traits<typename std::allocator_traits<allocator_element_t>::pointer>::template rebind<accumulated_storage_t> pointer_accumulated_storage, std::tuple<Args &...> const &values) const
            {
                using allocator_accumulated_storage_t = typename std::allocator_traits<allocator_element_t>::template rebind_alloc<accumulated_storage_t>;
                if constexpr(sizeof...(Args) == 0)
                {
                    if constexpr(requires { accumulate_binary_functor.identity_element(); })
                        std::allocator_traits<allocator_accumulated_storage_t>::construct(detail::utility::unmove(allocator_accumulated_storage_t(allocator_element)), std::to_address(pointer_accumulated_storage), accumulate_binary_functor.identity_element());
                    else
                        std::allocator_traits<allocator_accumulated_storage_t>::construct(detail::utility::unmove(allocator_accumulated_storage_t(allocator_element)), std::to_address(pointer_accumulated_storage));
                }
                else
                    std::allocator_traits<allocator_accumulated_storage_t>::construct(detail::utility::unmove(allocator_accumulated_storage_t(allocator_element)), std::to_address(pointer_accumulated_storage), tuple_fold<sizeof...(Args) - 1>(accumulate_binary_functor, values));
            }

            template<typename allocator_element_t>
            void destroy_accumulated_storage(allocator_element_t const &allocator_element, typename std::pointer_traits<typename std::allocator_traits<allocator_element_t>::pointer>::template rebind<accumulated_storage_t> pointer_accumulated_storage) const
            {
                using allocator_accumulated_storage_t = typename std::allocator_traits<allocator_element_t>::template rebind_alloc<accumulated_storage_t>;
                std::allocator_traits<allocator_accumulated_storage_t>::destroy(detail::utility::unmove(allocator_accumulated_storage_t(allocator_element)), std::to_address(pointer_accumulated_storage));
            }

            template<typename... Args>
            bool update_accumulated_storage(accumulated_storage_t &value, std::tuple<Args &...> const &values) const
            {
                auto compare_and_update = [&value](accumulated_storage_t const &value_new)
                {
                    if constexpr(requires { value == value_new; })
                    {
                        if(value == value_new)
                            return false;
                        else
                        {
                            value = value_new;
                            return true;
                        }
                    }
                    else
                    {
                        value = value_new;
                        return true;
                    }
                };
                if constexpr(sizeof...(Args) == 0)
                {
                    if constexpr(requires { accumulate_binary_functor.identity_element(); })
                        return compare_and_update(accumulated_storage_t(accumulate_binary_functor.identity_element()));
                    else
                        return compare_and_update(accumulated_storage_t());
                }
                else
                    return compare_and_update(accumulated_storage_t(tuple_fold<sizeof...(Args) - 1>(accumulate_binary_functor, values)));
            }
        };

        //more homogeneous binary functors
        template<typename element_t>
        struct min_t
        {
            element_t operator()(element_t const &lhs, element_t const &rhs) const
            {
                return std::ranges::min(lhs, rhs);
            }
        };
        template<typename element_t>
        struct max_t
        {
            element_t operator()(element_t const &lhs, element_t const &rhs) const
            {
                return std::ranges::max(lhs, rhs);
            }
        };

        template<typename identity_element_c>
        struct add_identity_element_member
        {
            static constexpr typename identity_element_c::value_type identity_element()
            {
                return identity_element_c::value;
            }
        };
        template<>
        struct add_identity_element_member<void>
        {
        };
        template<typename element_t_, typename homogeneous_binary_functor_t, typename identity_element_c = void>
        struct accumulating_binary_functor_wrapping_homogeneous_binary_functor_t: homogeneous_binary_functor_t, add_identity_element_member<identity_element_c>
        {
            using accumulated_storage_t = element_t_;
        };
        template<typename element_t_, typename sequence_t_>
        struct accumulating_sequence_binary_functor_t
        {
            using accumulated_storage_t = sequence_t_;

            sequence_t_ identity_element() const { return sequence_t_(); }
            sequence_t_ operator()(sequence_t_ lhs, element_t_ const &rhs) const
            {
                lhs.push_back(rhs);
                return lhs;
            }
            sequence_t_ operator()(sequence_t_ lhs, sequence_t_ &rhs) const
            {
                std::ranges::copy(rhs, std::back_inserter(lhs));
                return lhs;
            }
        };

        using empty_accumulator_t = accumulator_wrapping_accumulating_binary_functor_t<void>;
        template<typename element_t>
        using accumulator_plus_t = accumulator_wrapping_accumulating_binary_functor_t<accumulating_binary_functor_wrapping_homogeneous_binary_functor_t<element_t, std::plus<element_t>>>;
        template<typename element_t>
        using accumulator_logical_and_t = accumulator_wrapping_accumulating_binary_functor_t<accumulating_binary_functor_wrapping_homogeneous_binary_functor_t<element_t, std::logical_and<element_t>, std::true_type>>;
        template<typename element_t>
        using accumulator_logical_or_t = accumulator_wrapping_accumulating_binary_functor_t<accumulating_binary_functor_wrapping_homogeneous_binary_functor_t<element_t, std::logical_or<element_t>, std::true_type>>;
        template<typename element_t, typename sequence_t>
        using accumulator_sequence_t = accumulator_wrapping_accumulating_binary_functor_t<accumulating_sequence_binary_functor_t<element_t, sequence_t>>;
        template<typename element_t>
        using accumulator_max_t = accumulator_wrapping_accumulating_binary_functor_t<accumulating_binary_functor_wrapping_homogeneous_binary_functor_t<element_t, max_t<element_t>>>;
        template<typename element_t>
        using accumulator_min_t = accumulator_wrapping_accumulating_binary_functor_t<accumulating_binary_functor_wrapping_homogeneous_binary_functor_t<element_t, min_t<element_t>>>;
    } // namespace augmented_sequence_helpers
#endif // AUGMENTED_CONTAINERS_AUGMENTED_SEQUENCE_HELPERS

#ifndef AUGMENTED_CONTAINERS_AUGMENTED_SEQUENCE_DEFAULT_ARGUMENT
    #define AUGMENTED_CONTAINERS_AUGMENTED_SEQUENCE_DEFAULT_ARGUMENT
    template<
        typename element_t_,
        typename allocator_element_t_ = std::allocator<element_t_>,
        typename accumulator_t_ = augmented_sequence_helpers::empty_accumulator_t,
        typename augmented_sequence_physical_representation_t = std::integral_constant<augmented_sequence_physical_representation_e, augmented_sequence_physical_representation_e::rb3p>,
        typename augmented_sequence_size_management_t = std::integral_constant<augmented_sequence_size_management_e, augmented_sequence_size_management_e::at_each_node_except_node_end> //
        >
    struct augmented_sequence_t;
#endif // AUGMENTED_CONTAINERS_AUGMENTED_SEQUENCE_DEFAULT_ARGUMENT

    template<
        typename element_t_,
        typename allocator_element_t_,
        typename accumulator_t_,
        typename augmented_sequence_physical_representation_t_,
        typename augmented_sequence_size_management_t_>
        requires(static_cast<augmented_sequence_physical_representation_e>(augmented_sequence_physical_representation_t_{}) == augmented_sequence_physical_representation_e::rb2p)
    struct augmented_sequence_t<element_t_, allocator_element_t_, accumulator_t_, augmented_sequence_physical_representation_t_, augmented_sequence_size_management_t_>
    {
        using element_t = element_t_;
        using allocator_element_t = allocator_element_t_;
        using accumulator_t = accumulator_t_;
        using augmented_sequence_physical_representation_t = augmented_sequence_physical_representation_t_;
        using augmented_sequence_size_management_t = augmented_sequence_size_management_t_;

        using pointer_element_t = typename std::allocator_traits<allocator_element_t_>::pointer;
        using const_pointer_element_t = typename std::allocator_traits<allocator_element_t_>::const_pointer;
        using accumulated_storage_t = typename accumulator_t::accumulated_storage_t;

        using value_type = element_t;
        using allocator_type = allocator_element_t;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type &;
        using const_reference = value_type const &;
        using pointer = typename std::allocator_traits<allocator_type>::pointer;
        using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;

        struct config_t
        {
            using element_t = augmented_sequence_t::element_t;
            using allocator_element_t = augmented_sequence_t::allocator_element_t;
            using accumulator_t = augmented_sequence_t::accumulator_t;
            using augmented_sequence_physical_representation_t = augmented_sequence_t::augmented_sequence_physical_representation_t;
            using augmented_sequence_size_management_t = augmented_sequence_t::augmented_sequence_size_management_t;

            using pointer_element_t = augmented_sequence_t::pointer_element_t;
            using const_pointer_element_t = augmented_sequence_t::const_pointer_element_t;
            using accumulated_storage_t = augmented_sequence_t::accumulated_storage_t;
        };

        using navigator_t = detail::augmented_sequence_rb2p::rb2p_node_navigator_all_t<allocator_element_t>;
        using navigator_except_node_end_t = detail::augmented_sequence_rb2p::rb2p_node_navigator_except_node_end_t<allocator_element_t>;
        using node_t = detail::augmented_sequence_rb2p::rb2p_node_t<config_t>;
        using node_end_t = detail::augmented_sequence_rb2p::rb2p_node_end_t<config_t>;

        using iterator_t = detail::augmented_sequence_rb2p::rb2p_iterator_t<false, false, config_t>;
        static_assert(std::input_or_output_iterator<iterator_t>);
        static_assert(std::input_iterator<iterator_t>);
        static_assert(std::sentinel_for<iterator_t, iterator_t>);
        static_assert(std::sentinel_for<std::default_sentinel_t, iterator_t>);
        static_assert(std::forward_iterator<iterator_t>);
        static_assert(std::bidirectional_iterator<iterator_t>);
        using const_iterator_t = detail::augmented_sequence_rb2p::rb2p_iterator_t<true, false, config_t>;
        static_assert(std::input_or_output_iterator<const_iterator_t>);
        static_assert(std::input_iterator<const_iterator_t>);
        static_assert(std::sentinel_for<const_iterator_t, const_iterator_t>);
        static_assert(std::sentinel_for<std::default_sentinel_t, const_iterator_t>);
        static_assert(std::forward_iterator<const_iterator_t>);
        static_assert(std::bidirectional_iterator<const_iterator_t>);

        allocator_element_t allocator_element;
        node_end_t *node_end;
        allocator_type get_allocator() const noexcept { return allocator_element; }

        iterator_t begin()
        {
            iterator_t result(node_end);
            ++result;
            return result;
        }
        iterator_t end() { return {node_end}; }
        const_iterator_t begin() const
        {
            const_iterator_t result(node_end);
            ++result;
            return result;
        }
        const_iterator_t end() const { return {node_end}; }
        const_iterator_t cbegin() const
        {
            const_iterator_t result(node_end);
            ++result;
            return result;
        }
        const_iterator_t cend() const { return {node_end}; }

        void create_end_node() { node_end = node_end_t::create_node_end(); }
        void destroy_end_node() { delete node_end; }
        void swap_end_node(augmented_sequence_t &other)
        {
            std::ranges::swap(this->node_end, other.node_end);
        }

        augmented_sequence_t() { create_end_node(); }
        void clear() &
        {
            auto erase_tree = [](auto &this_, navigator_except_node_end_t *node) -> void
            {
                navigator_except_node_end_t *child_left = node->child_left();
                navigator_except_node_end_t *child_right = node->child_right();
                if(child_left != nullptr)
                    this_(this_, child_left);
                if(child_right != nullptr)
                    this_(this_, child_right);
                static_cast<node_t *>(node)->p_element()->~element_t();
                delete static_cast<node_t *>(node);
            };
            if(!detail::augmented_sequence_rb2p::rb2p_functor_t<false, config_t>::empty(node_end))
                erase_tree(erase_tree, static_cast<navigator_except_node_end_t *>(detail::language::tagged_ptr_bit0_unsetted(node_end->parent_all())));
            typename navigator_t::loop_end_t{.end = node_end}.link();

            if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::no_size)
                ;
            else if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_node_end)
                node_end->node_count = 0;
            else if constexpr(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_each_node_except_node_end)
                ;
            else
                std::unreachable();
        }
        explicit augmented_sequence_t(allocator_element_t const &allocator_element) // default constructor with allocator
            : allocator_element(allocator_element)
        {
            create_end_node();
        }
        explicit augmented_sequence_t(size_type count, allocator_element_t const &allocator_element = allocator_element_t()) // count default-inserted constructor (with allocator)?
            : augmented_sequence_t(allocator_element)
        {
            std::ranges::for_each(std::views::iota(static_cast<size_type>(0), count), [this]([[maybe_unused]] size_type index)
                { this->emplace_back(); });
        }
        explicit augmented_sequence_t(size_type count, element_t const &value, allocator_element_t const &allocator_element = allocator_element_t()) // count copy-inserted constructor (with allocator)?
            : augmented_sequence_t(allocator_element)
        {
            std::ranges::for_each(std::views::iota(static_cast<size_type>(0), count), [this, &value]([[maybe_unused]] size_type index)
                { this->emplace_back(value); });
        }
        void assign(size_type count, element_t const &value) &
        {
            this->clear();
            std::ranges::for_each(std::views::iota(static_cast<size_type>(0), count), [this, &value]([[maybe_unused]] size_type index)
                { this->emplace_back(value); });
        }
        template<std::input_iterator iterator_t, std::sentinel_for<iterator_t> sentinel_t>
        augmented_sequence_t(iterator_t iterator, sentinel_t sentinel, allocator_element_t const &allocator_element = allocator_element_t()) // comparable range constructor (with allocator)?
            : augmented_sequence_t(allocator_element)
        {
            std::ranges::for_each(std::ranges::subrange(iterator, sentinel), [this]<typename other_element_t>(other_element_t &&other_element)
                { this->emplace_back(std::forward<other_element_t>(other_element)); });
        }
        template<std::input_iterator iterator_t, std::sentinel_for<iterator_t> sentinel_t>
        void assign(iterator_t iterator, sentinel_t sentinel) &
        {
            this->clear();
            std::ranges::for_each(std::ranges::subrange(iterator, sentinel), [this]<typename other_element_t>(other_element_t &&other_element)
                { this->emplace_back(std::forward<other_element_t>(other_element)); });
        }
        augmented_sequence_t(std::initializer_list<element_t> initializer_list, allocator_element_t const &allocator_element = allocator_element_t()) // std::initializer_list constructor (with allocator)?
            : augmented_sequence_t(allocator_element)
        {
            std::ranges::for_each(initializer_list, [this](element_t const &other_element)
                { this->emplace_back(other_element); });
        }
        augmented_sequence_t &operator=(std::initializer_list<element_t> initializer_list) & // std::initializer_list assignment operator
        {
            this->clear();
            std::ranges::for_each(initializer_list, [this](element_t const &other_element)
                { this->emplace_back(other_element); });
            return *this;
        }
        void assign(std::initializer_list<element_t> initializer_list) &
        {
            this->clear();
            std::ranges::for_each(initializer_list, [this](element_t const &other_element)
                { this->emplace_back(other_element); });
        }
        augmented_sequence_t(augmented_sequence_t const &other, std::type_identity_t<allocator_element_t> const &allocator_element) // copy constructor with allocator
            : augmented_sequence_t(allocator_element)
        {
            std::ranges::for_each(std::ranges::subrange(other.cbegin(), other.cend()), [this](element_t const &other_element)
                { this->emplace_back(other_element); });
        }
        augmented_sequence_t(augmented_sequence_t const &other) // copy constructor
            : augmented_sequence_t(other, std::allocator_traits<allocator_type>::select_on_container_copy_construction(other.allocator_element))
        {}
        augmented_sequence_t &operator=(augmented_sequence_t const &other) & // copy assignment operator
        {
            if(this == &other)
                return *this;
            this->clear();
            if(this->allocator_element != other.allocator_element)
            {
                if constexpr(std::allocator_traits<allocator_type>::propagate_on_container_copy_assignment::value)
                {
                    destroy_end_node();
                    this->allocator_element = other.allocator_element;
                    create_end_node();
                }
            }
            std::ranges::for_each(std::ranges::subrange(other.cbegin(), other.cend()), [this](element_t const &other_element)
                { this->emplace_back(other_element); });
            return *this;
        }

        augmented_sequence_t(augmented_sequence_t &&other) // move constructor
            : allocator_element(([&]()
                  {
                                     this->node_end=other.node_end;
                                     other.create_end_node(); }(),
                  std::move(other.allocator_element)))
        {}
        augmented_sequence_t(augmented_sequence_t &&other, std::type_identity_t<allocator_element_t> const &allocator_element) // move constructor with allocator
            : allocator_element(allocator_element)
        {
            if(allocator_element == other.allocator_element)
            {
                this->node_end = other.node_end;
                other.create_end_node();
            }
            else
            {
                create_end_node();
                std::ranges::for_each(std::ranges::subrange(other.begin(), other.end()), [this](element_t &other_element)
                    { this->emplace_back(std::move(other_element)); });
            }
        }
        augmented_sequence_t &operator=(augmented_sequence_t &&other) & // move assignment operator
        {
            if(this == &other)
                return *this;
            this->clear();
            if(this->allocator_element == other.allocator_element)
                swap_end_node(other);
            else
            {
                if constexpr(std::allocator_traits<allocator_type>::propagate_on_container_move_assignment::value)
                {
                    destroy_end_node();
                    this->allocator_element = std::move(other.allocator_element);
                    create_end_node();
                    swap_end_node(other);
                }
                else
                    std::ranges::for_each(std::ranges::subrange(other.begin(), other.end()), [this](element_t &other_element)
                        { this->emplace_back(std::move(other_element)); });
            }
            return *this;
        }
        void swap(augmented_sequence_t &other)
        {
            if(this->allocator_element == other.allocator_element)
                swap_end_node(other);
            else
            {
                if constexpr(std::allocator_traits<allocator_type>::propagate_on_container_swap::value)
                {
                    std::ranges::swap(this->allocator_element, other.allocator_element);
                    swap_end_node(other);
                }
                else
                {
                    augmented_sequence_t temp_this(other, this->allocator_element);
                    augmented_sequence_t temp_other(*this, other.allocator_element);
                    swap_end_node(temp_this);
                    other.swap_end_node(temp_other);
                }
            }
        }
        ~augmented_sequence_t()
        {
            clear();
            destroy_end_node();
        }

        bool empty() const
        {
            return detail::augmented_sequence_rb2p::rb2p_functor_t<false, config_t>::empty(node_end);
        }
        size_type size() const
            requires(static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_node_end || static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_each_node_except_node_end)
        {
            return detail::augmented_sequence_rb2p::rb2p_functor_t<false, config_t>::size(node_end);
        }

        reference front() { return *begin(); }
        const_reference front() const { return *begin(); }
        reference back() { return *std::ranges::prev(end()); }
        const_reference back() const { return *std::ranges::prev(end()); }

        template<typename... args_t>
        iterator_t emplace(const_iterator_t pos, args_t &&...args)
        {
            iterator_t result = std::ranges::prev(iterator_t(pos.current_node));
            node_t *node_new = new node_t();
            new(node_new->p_element()) element_t(std::forward<args_t>(args)...);
            if constexpr(!std::is_same_v<accumulated_storage_t, void>)
                node_end->accumulator.construct_accumulated_storage(this->allocator_element, node_new->p_accumulated_storage(), std::make_tuple());
            typename detail::augmented_sequence_rb2p::rb2p_functor_t<false, config_t>::schedules_t schedules;
            detail::augmented_sequence_rb2p::rb2p_functor_t<false, config_t>::insert(schedules, node_end, pos.current_node, node_new);
            detail::augmented_sequence_rb2p::rb2p_functor_t<false, config_t>::run_schedules(schedules, node_end->accumulator);
            return std::ranges::next(result);
        }
        iterator_t insert(const_iterator_t pos, element_t const &element) { return this->emplace(pos, element); }
        iterator_t insert(const_iterator_t pos, element_t &&element) { return this->emplace(pos, std::move(element)); }
        iterator_t insert(const_iterator_t pos, size_type count, element_t const &value)
        {
            iterator_t result = std::ranges::prev(iterator_t(pos.current_node));
            std::ranges::for_each(std::views::iota(static_cast<size_type>(0), count), [this, &pos, &value]([[maybe_unused]] size_type index)
                { this->emplace(pos, value); });
            return std::ranges::next(result);
        }
        template<std::input_iterator iterator_t_, std::sentinel_for<iterator_t_> sentinel_t_>
        iterator_t insert(const_iterator_t pos, iterator_t_ iterator, sentinel_t_ sentinel)
        {
            iterator_t result = std::ranges::prev(iterator_t(pos.current_node));
            std::ranges::for_each(std::ranges::subrange(iterator, sentinel), [this, &pos]<typename other_element_t>(other_element_t &&other_element)
                { this->emplace(pos, std::forward<other_element_t>(other_element)); });
            return std::ranges::next(result);
        }
        iterator_t insert(const_iterator_t pos, std::initializer_list<element_t> initializer_list)
        {
            iterator_t result = std::ranges::prev(iterator_t(pos.current_node));
            std::ranges::for_each(initializer_list, [this, &pos](element_t const &element)
                { this->emplace(pos, element); });
            return std::ranges::next(result);
        }
        iterator_t erase(const_iterator_t pos)
        {
            iterator_t result = std::ranges::prev(iterator_t(pos.current_node));
            typename detail::augmented_sequence_rb2p::rb2p_functor_t<false, config_t>::schedules_t schedules;
            detail::augmented_sequence_rb2p::rb2p_functor_t<false, config_t>::erase(schedules, node_end, pos.current_node);
            detail::augmented_sequence_rb2p::rb2p_functor_t<false, config_t>::run_schedules(schedules, node_end->accumulator);
            if constexpr(!std::is_same_v<accumulated_storage_t, void>)
                node_end->accumulator.destroy_accumulated_storage(this->allocator_element, static_cast<node_t *>(pos.current_node)->p_accumulated_storage());
            static_cast<node_t *>(pos.current_node)->p_element()->~element_t();
            delete static_cast<node_t *>(pos.current_node);
            return std::ranges::next(result);
        }
        iterator_t erase(const_iterator_t pos_begin, const_iterator_t pos_end)
        {
            iterator_t result = std::ranges::prev(iterator_t(pos_begin.current_node));
            for(; pos_begin != pos_end;)
            {
                navigator_t *pos_begin_current_node = pos_begin.current_node;
                ++pos_begin;
                detail::augmented_sequence_rb2p::rb2p_functor_t<false, config_t>::erase(node_end, pos_begin_current_node);
                static_cast<node_t *>(pos_begin_current_node)->p_element()->~element_t();
                delete static_cast<node_t *>(pos_begin_current_node);
            }
            return std::ranges::next(result);
        }

        void pop_back() { this->erase(std::ranges::prev(end())); }
        template<typename... args_t>
        reference emplace_back(args_t &&...args) { return *this->emplace(end(), std::forward<args_t>(args)...); }
        void push_back(element_t const &element) { this->emplace_back(element); }
        void push_back(element_t &&element) { this->emplace_back(std::move(element)); }

        void pop_front() { this->erase(begin()); }
        template<typename... args_t>
        reference emplace_front(args_t &&...args) { return *this->emplace(begin(), std::forward<args_t>(args)...); }
        void push_front(element_t const &element) { this->emplace_front(element); }
        void push_front(element_t &&element) { this->emplace_front(std::move(element)); }


        friend void swap(augmented_sequence_t &lhs, augmented_sequence_t &rhs) { lhs.swap(rhs); }
        friend bool operator==(augmented_sequence_t const &lhs, augmented_sequence_t const &rhs)
        {
            return std::ranges::equal(std::ranges::subrange(lhs.begin(), lhs.end()), std::ranges::subrange(rhs.begin(), rhs.end()));
        }
        friend auto operator<=>(augmented_sequence_t const &lhs, augmented_sequence_t const &rhs)
        {
#ifdef __EMSCRIPTEN__
            auto f1 = lhs.begin(), l1 = lhs.end(), f2 = rhs.begin(), l2 = rhs.end();
            bool exhaust1 = (f1 == l1);
            bool exhaust2 = (f2 == l2);
            for(; !exhaust1 && !exhaust2; exhaust1 = (++f1 == l1), exhaust2 = (++f2 == l2))
                if(auto c = comp(*f1, *f2); c != 0)
                    return c;
            return !exhaust1 ? std::strong_ordering::greater : !exhaust2 ? std::strong_ordering::less
                                                                         : std::strong_ordering::equal;
#else
            return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
#endif
        }


        friend augmented_sequence_t &operator<<(augmented_sequence_t &lhs, augmented_sequence_t &&rhs)
        {
            if(&lhs == &rhs)
                return lhs;
            typename detail::augmented_sequence_rb2p::rb2p_functor_t<false, config_t>::schedules_t schedules;
            detail::augmented_sequence_rb2p::rb2p_functor_t<false, config_t>::template concat_without_middle_key<false>(schedules, lhs.node_end, rhs.node_end);
            detail::augmented_sequence_rb2p::rb2p_functor_t<false, config_t>::run_schedules(schedules, lhs.node_end->accumulator);
            return lhs;
        }
        friend augmented_sequence_t &&operator<<(augmented_sequence_t &&lhs, augmented_sequence_t &&rhs)
        {
            if(&lhs == &rhs)
                return std::move(lhs);
            typename detail::augmented_sequence_rb2p::rb2p_functor_t<false, config_t>::schedules_t schedules;
            detail::augmented_sequence_rb2p::rb2p_functor_t<false, config_t>::template concat_without_middle_key<false>(schedules, lhs.node_end, rhs.node_end);
            detail::augmented_sequence_rb2p::rb2p_functor_t<false, config_t>::run_schedules(schedules, lhs.node_end->accumulator);
            return std::move(lhs);
        }
        friend augmented_sequence_t &operator>>(augmented_sequence_t &&lhs, augmented_sequence_t &rhs)
        {
            if(&lhs == &rhs)
                return rhs;
            typename detail::augmented_sequence_rb2p::rb2p_functor_t<false, config_t>::schedules_t schedules;
            detail::augmented_sequence_rb2p::rb2p_functor_t<false, config_t>::template concat_without_middle_key<true>(schedules, lhs.node_end, rhs.node_end);
            detail::augmented_sequence_rb2p::rb2p_functor_t<false, config_t>::run_schedules(schedules, rhs.node_end->accumulator);
            return rhs;
        }
        friend augmented_sequence_t &&operator>>(augmented_sequence_t &&lhs, augmented_sequence_t &&rhs)
        {
            if(&lhs == &rhs)
                return std::move(rhs);
            typename detail::augmented_sequence_rb2p::rb2p_functor_t<false, config_t>::schedules_t schedules;
            detail::augmented_sequence_rb2p::rb2p_functor_t<false, config_t>::template concat_without_middle_key<true>(schedules, lhs.node_end, rhs.node_end);
            detail::augmented_sequence_rb2p::rb2p_functor_t<false, config_t>::run_schedules(schedules, rhs.node_end->accumulator);
            return std::move(rhs);
        }

        augmented_sequence_t split_emit_left(const_iterator_t pos)
        {
            augmented_sequence_t result;
            typename detail::augmented_sequence_rb2p::rb2p_functor_t<false, config_t>::schedules_t schedules;
            detail::augmented_sequence_rb2p::rb2p_functor_t<false, config_t>::template split<false>(schedules, result.node_end, node_end, pos.current_node);
            detail::augmented_sequence_rb2p::rb2p_functor_t<false, config_t>::run_schedules(schedules, node_end->accumulator);
            return result;
        }
        augmented_sequence_t split_emit_right(const_iterator_t pos)
        {
            augmented_sequence_t result;
            typename detail::augmented_sequence_rb2p::rb2p_functor_t<false, config_t>::schedules_t schedules;
            detail::augmented_sequence_rb2p::rb2p_functor_t<false, config_t>::template split<true>(schedules, result.node_end, node_end, pos.current_node);
            detail::augmented_sequence_rb2p::rb2p_functor_t<false, config_t>::run_schedules(schedules, node_end->accumulator);
            return result;
        }

        template<bool is_reversed = false, bool is_const_iterator_begin, bool is_const_iterator_end>
        accumulated_storage_t read_range(detail::augmented_sequence_rb2p::rb2p_iterator_t<is_const_iterator_begin, is_reversed, config_t> const &iterator_begin, detail::augmented_sequence_rb2p::rb2p_iterator_t<is_const_iterator_end, is_reversed, config_t> const &iterator_end) const
        {
            if(iterator_begin == iterator_end)
                return node_end->accumulator.construct_accumulated_storage(allocator_element, std::make_tuple());

            navigator_except_node_end_t *node_front = static_cast<navigator_except_node_end_t *>(iterator_begin.current_node);
            navigator_except_node_end_t *node_back = static_cast<navigator_except_node_end_t *>(std::ranges::prev(iterator_end).current_node);

            return detail::augmented_sequence_rb2p::rb2p_functor_t<is_reversed, config_t>::read_range_impl(allocator_element, node_end, node_front, node_back);
        }

        template<bool is_reversed = false, bool is_const_iterator_begin, bool is_const_iterator_end>
        void update_range(detail::augmented_sequence_rb2p::rb2p_iterator_t<is_const_iterator_begin, is_reversed, config_t> const &iterator_begin, detail::augmented_sequence_rb2p::rb2p_iterator_t<is_const_iterator_end, is_reversed, config_t> const &iterator_end)
        {
            if(iterator_begin == iterator_end)
                return;

            navigator_except_node_end_t *node_front = static_cast<navigator_except_node_end_t *>(iterator_begin.current_node);
            navigator_except_node_end_t *node_back = static_cast<navigator_except_node_end_t *>(std::ranges::prev(iterator_end).current_node);

            detail::augmented_sequence_rb2p::rb2p_functor_t<is_reversed, config_t>::update_range_impl(node_end, node_front, node_back);
        }

        template<bool is_reversed = false, detail::concepts::invocable_r<bool, accumulated_storage_t const &> monotonic_predicate_t>
        const_iterator_t find_by_monotonic_predicate(monotonic_predicate_t const &monotonic_predicate) const
        {
            return {detail::augmented_sequence_rb2p::rb2p_functor_t<is_reversed, config_t>::find_by_monotonic_predicate(allocator_element, node_end, monotonic_predicate)};
        }
        template<bool is_reversed = false, detail::concepts::invocable_r<bool, accumulated_storage_t const &> monotonic_predicate_t>
        iterator_t find_by_monotonic_predicate(monotonic_predicate_t const &monotonic_predicate)
        {
            return {detail::augmented_sequence_rb2p::rb2p_functor_t<is_reversed, config_t>::find_by_monotonic_predicate(allocator_element, node_end, monotonic_predicate)};
        }

        template<bool is_reversed = false, /*std::output_iterator<const_iterator_t>*/ typename iterator_output_const_iterator_t, typename heap_predicate_t>
            requires(detail::concepts::invocable_r<heap_predicate_t, bool, accumulated_storage_t &> && detail::concepts::invocable_r<heap_predicate_t, bool, element_t const &>)
        void find_by_heap_predicate(iterator_output_const_iterator_t iterator_output_const_iterator, heap_predicate_t const &heap_predicate) const
        {
            detail::augmented_sequence_rb2p::rb2p_functor_t<is_reversed, config_t>::find_by_heap_predicate(node_end,
                detail::iterator::transform_output_iterator_t{
#ifndef __EMSCRIPTEN__
                    .wrapped_iterator =
#endif
                        iterator_output_const_iterator,
#ifndef __EMSCRIPTEN__
                    .transformer =
#endif
                        [](navigator_t *value)
                    { return const_iterator_t{value}; },
                },
                heap_predicate);
        }
        template<bool is_reversed = false, /*std::output_iterator<iterator_t>*/ typename iterator_output_iterator_t, typename heap_predicate_t>
            requires(detail::concepts::invocable_r<heap_predicate_t, bool, accumulated_storage_t &> && detail::concepts::invocable_r<heap_predicate_t, bool, element_t const &>)
        void find_by_heap_predicate(iterator_output_iterator_t iterator_output_iterator, heap_predicate_t const &heap_predicate)
        {
            detail::augmented_sequence_rb2p::rb2p_functor_t<is_reversed, config_t>::find_by_heap_predicate(node_end,
                detail::iterator::transform_output_iterator_t{
#ifndef __EMSCRIPTEN__
                    .wrapped_iterator =
#endif
                        iterator_output_iterator,
#ifndef __EMSCRIPTEN__
                    .transformer =
#endif
                        [](navigator_t *value)
                    { return iterator_t{value}; },
                },
                heap_predicate);
        }

        template<bool is_reversed = false, typename heap_predicate_t>
            requires(detail::concepts::invocable_r<heap_predicate_t, bool, accumulated_storage_t &> && detail::concepts::invocable_r<heap_predicate_t, bool, element_t const &>)
        detail::coroutine::generator_t<const_iterator_t> find_by_heap_predicate_generator(heap_predicate_t const &heap_predicate) const
        {
            for(navigator_t *value : detail::augmented_sequence_rb2p::rb2p_functor_t<is_reversed, config_t>::find_by_heap_predicate_generator(node_end, heap_predicate))
                co_yield const_iterator_t{value};
        }
        template<bool is_reversed = false, typename heap_predicate_t>
            requires(detail::concepts::invocable_r<heap_predicate_t, bool, accumulated_storage_t &> && detail::concepts::invocable_r<heap_predicate_t, bool, element_t const &>)
        detail::coroutine::generator_t<iterator_t> find_by_heap_predicate_generator(heap_predicate_t const &heap_predicate)
        {
            for(navigator_t *value : detail::augmented_sequence_rb2p::rb2p_functor_t<is_reversed, config_t>::find_by_heap_predicate_generator(node_end, heap_predicate))
                co_yield iterator_t{value};
        }
    };
} // namespace augmented_containers

#endif // AUGMENTED_RB2P_HPP
