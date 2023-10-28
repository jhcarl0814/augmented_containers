#ifndef AUGMENTED_GRAPH_HPP
#define AUGMENTED_GRAPH_HPP

#include <tuple>
#include <functional>
#include <cassert>
#include <ranges>
#include <iterator>
#include <algorithm>
#include <cstddef>
#include <memory>
#include <concepts>
#include <coroutine>
#include <utility>

#include <set>
#include <map>

namespace augmented_containers
{
    enum class augmented_graph_part_data_structure_e {
        top_tree,
    };

    struct augmented_graph_part_base
    {
        void vertex_inserting(auto &&...) {}
        void vertex_inserted(auto &&...) {}
        void vertex_erasing(auto &&...) {}
        void vertex_erased(auto &&...) {}
        void vertex_updating(auto &&...) {}
        void vertex_updated(auto &&...) {}
        void edge_inserting(auto &&...) {}
        void edge_inserted(auto &&...) {}
        void edge_erasing(auto &&...) {}
        void edge_erased(auto &&...) {}
        void edge_updating(auto &&...) {}
        void edge_updated(auto &&...) {}
    };

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
            target_pointer_t pointer_traits_reinterpret_cast(source_pointer_t source_pointer)
            {
                return std::pointer_traits<target_pointer_t>::pointer_to(*reinterpret_cast<typename std::pointer_traits<target_pointer_t>::element_type *>(std::to_address(source_pointer)));
            }
#endif // AUGMENTED_CONTAINERS_LANGUAGE_POINTER_TRAITS_CAST

#ifndef AUGMENTED_CONTAINERS_LANGUAGE_TAGGED_PTR_BIT0
    #define AUGMENTED_CONTAINERS_LANGUAGE_TAGGED_PTR_BIT0
            template<typename pointer_t>
            bool tagged_ptr_bit0_is_setted(pointer_t p)
            {
                return (reinterpret_cast<uintptr_t>(std::to_address(p)) & 0b1) != 0;
            };
            template<typename pointer_t>
            pointer_t tagged_ptr_bit0_unsetted_relaxed(pointer_t p) { return std::pointer_traits<pointer_t>::pointer_to(*reinterpret_cast<typename std::pointer_traits<pointer_t>::element_type *>(reinterpret_cast<uintptr_t>(std::to_address(p)) & ~0b1)); }
            template<typename pointer_t>
            pointer_t tagged_ptr_bit0_unsetted(pointer_t p) { return assert(tagged_ptr_bit0_is_setted(p)), tagged_ptr_bit0_unsetted_relaxed(p); }
            template<typename pointer_t>
            pointer_t tagged_ptr_bit0_setted_relaxed(pointer_t p) { return std::pointer_traits<pointer_t>::pointer_to(*reinterpret_cast<typename std::pointer_traits<pointer_t>::element_type *>(reinterpret_cast<uintptr_t>(std::to_address(p)) | 0b1)); }
            template<typename pointer_t>
            pointer_t tagged_ptr_bit0_setted(pointer_t p) { return assert(!tagged_ptr_bit0_is_setted(p)), tagged_ptr_bit0_setted_relaxed(p); }
#endif // AUGMENTED_CONTAINERS_LANGUAGE_TAGGED_PTR_BIT0

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
        }
#endif // AUGMENTED_CONTAINERS_CONCEPTS

#ifndef AUGMENTED_CONTAINERS_UTILITY
    #define AUGMENTED_CONTAINERS_UTILITY
        namespace utility
        {
    #ifdef __EMSCRIPTEN__
        }
    }
}

namespace std
{
    template<typename T>
    initializer_list(initializer_list<T>) -> initializer_list<T>;
}

namespace augmented_containers
{
    namespace detail
    {
        namespace utility
        {
    #else
    #endif

            template<typename T>
            constexpr T &unmove(T &&t)
            {
                return static_cast<T &>(t);
            } //https://stackoverflow.com/a/67059296/8343353

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

#ifndef AUGMENTED_CONTAINERS_TUPLE
    #define AUGMENTED_CONTAINERS_TUPLE
        namespace tuple
        {
            //            template<std::size_t I, typename tuple_t>
            //            constexpr std::size_t tuple_element_offset() // https://stackoverflow.com/questions/70647441/how-to-determine-the-offset-of-an-element-of-a-tuple-at-compile-time
            //            {
            //                static_assert(!std::is_reference_v<std::tuple_element_t<I, tuple_t>>);
            //                union
            //                {
            //                    std::byte a[sizeof(tuple_t)]{};
            //                    tuple_t t;
            //                };
            //                void *p_element = std::addressof(std::get<I>(t));
            //                for(std::size_t i = 0;; ++i) {
            //                    if(a + i == p_element) return i;
            //                }
            //            }
            template<size_t I, typename tuple_t>
            constexpr std::size_t tuple_element_offset() // https://stackoverflow.com/questions/5358684/get-the-offset-of-a-tuple-element
            {
    #ifdef __clang__
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wnull-dereference"
        #pragma clang diagnostic ignored "-Wnull-pointer-subtraction"
    #endif
                return static_cast<std::size_t>(static_cast<std::byte *>(static_cast<void *>(&std::get<I>(*static_cast<tuple_t *>(nullptr)))) - static_cast<std::byte *>(nullptr));
    #ifdef __clang__
        #pragma clang diagnostic pop
    #endif
            }
        } // namespace tuple
#endif // AUGMENTED_CONTAINERS_TUPLE

#ifndef AUGMENTED_CONTAINERS_MEMORY
    #define AUGMENTED_CONTAINERS_MEMORY
        namespace memory
        {
            template<typename target_t, typename allocator_element_t, typename... args_t>
            typename std::allocator_traits<typename std::allocator_traits<allocator_element_t>::template rebind_alloc<target_t>>::pointer new_expression(allocator_element_t const &allocator_element, args_t &&...args)
            {
                using allocator_target_t = typename std::allocator_traits<allocator_element_t>::template rebind_alloc<target_t>;
                allocator_target_t allocator_target(allocator_element);
                typename std::allocator_traits<allocator_target_t>::pointer p_target = std::allocator_traits<allocator_target_t>::allocate(allocator_target, 1);
                std::allocator_traits<allocator_target_t>::construct(allocator_target, std::to_address(p_target), std::forward<args_t>(args)...);
                return p_target;
            }

            template<typename target_t, typename allocator_element_t>
            void delete_expression(allocator_element_t const &allocator_element, typename std::allocator_traits<typename std::allocator_traits<allocator_element_t>::template rebind_alloc<target_t>>::pointer p_target)
            {
                using allocator_target_t = typename std::allocator_traits<allocator_element_t>::template rebind_alloc<target_t>;
                allocator_target_t allocator_target(allocator_element);
                std::allocator_traits<allocator_target_t>::destroy(allocator_target, std::to_address(p_target));
                std::allocator_traits<allocator_target_t>::deallocate(allocator_target, p_target, 1);
            }
        } // namespace memory
#endif // AUGMENTED_CONTAINERS_MEMORY

        //        namespace functional
        //        {
        //            template<typename... Ts>
        //            struct overloaded: Ts... //https://en.cppreference.com/w/cpp/utility/variant/visit
        //            {
        //                using Ts::operator()...;
        //            };
        //#if __cpp_deduction_guides >= 201907L
        //#else
        //            template<typename... Ts>
        //            overloaded(Ts...) -> overloaded<Ts...>;
        //#endif
        //        } // namespace functional

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
    #ifdef __clang__
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wmissing-field-initializers"
    #endif
                        } yield_awaitable{.generator = std::move(generator)};
    #ifdef __clang__
        #pragma clang diagnostic pop
    #endif
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
    } // namespace detail

    namespace augmented_graph_helpers
    {
        template<typename cluster_t_ = void>
        struct get_top_tree_internal_operations_empty_t
        {
            template<typename graph_t>
            struct type
            {
                using cluster_t = cluster_t_;
                void create([[maybe_unused]] graph_t *graph, [[maybe_unused]] graph_t::it_vertex_t it_vertex1, cluster_t *cluster, [[maybe_unused]] graph_t::it_vertex_t it_vertex2, [[maybe_unused]] graph_t::it_edge_it_vertexes_t it_edge_it_vertexes) const
                {
                    new(cluster) cluster_t();
                }
                void destroy([[maybe_unused]] graph_t *graph, [[maybe_unused]] graph_t::it_vertex_t it_vertex1, cluster_t *cluster, [[maybe_unused]] graph_t::it_vertex_t it_vertex2) const
                {
                    cluster->~cluster_t();
                }
                void join([[maybe_unused]] graph_t *graph, cluster_t *cluster_parent, [[maybe_unused]] graph_t::it_vertex_t it_vertex1, [[maybe_unused]] cluster_t *cluster_child_left, [[maybe_unused]] graph_t::it_vertex_t it_vertex2, [[maybe_unused]] cluster_t *cluster_child_right, [[maybe_unused]] graph_t::it_vertex_t it_vertex3, [[maybe_unused]] int dummy_or_rake_or_compress) const
                {
                    new(cluster_parent) cluster_t();
                }
                void split([[maybe_unused]] graph_t *graph, cluster_t *cluster_parent, [[maybe_unused]] graph_t::it_vertex_t it_vertex1, [[maybe_unused]] cluster_t *cluster_child_left, [[maybe_unused]] graph_t::it_vertex_t it_vertex2, [[maybe_unused]] cluster_t *cluster_child_right, [[maybe_unused]] graph_t::it_vertex_t it_vertex3, [[maybe_unused]] int dummy_or_rake_or_compress) const
                {
                    cluster_parent->~cluster_t();
                }
            };
        };
        template<typename cluster_t_>
            requires(std::is_same_v<cluster_t_, void>)
        struct get_top_tree_internal_operations_empty_t<cluster_t_>
        {
            template<typename graph_t>
            struct type
            {
                using cluster_t = cluster_t_;
            };
        };

        template<typename cluster_t_, typename homogeneous_binary_functor_t>
        struct get_top_tree_internal_operations_accumulating_tree_t
        {
            template<typename graph_t>
            struct type
            {
                using cluster_t = cluster_t_;
                homogeneous_binary_functor_t homogeneous_binary_functor;
                void create([[maybe_unused]] graph_t *graph, [[maybe_unused]] graph_t::it_vertex_t it_vertex1, cluster_t *cluster, [[maybe_unused]] graph_t::it_vertex_t it_vertex2, graph_t::it_edge_it_vertexes_t it_edge_it_vertexes) const
                {
                    new(cluster) cluster_t(std::as_const(it_edge_it_vertexes->first));
                }
                void destroy([[maybe_unused]] graph_t *graph, [[maybe_unused]] graph_t::it_vertex_t it_vertex1, cluster_t *cluster, [[maybe_unused]] graph_t::it_vertex_t it_vertex2) const
                {
                    cluster->~cluster_t();
                }
                void join([[maybe_unused]] graph_t *graph, cluster_t *cluster_parent, [[maybe_unused]] graph_t::it_vertex_t it_vertex1, cluster_t *cluster_child_left, [[maybe_unused]] graph_t::it_vertex_t it_vertex2, cluster_t *cluster_child_right, [[maybe_unused]] graph_t::it_vertex_t it_vertex3, int dummy_or_rake_or_compress) const
                {
                    if(dummy_or_rake_or_compress == 0)
                        new(cluster_parent) cluster_t(std::as_const(*cluster_child_left));
                    else if(dummy_or_rake_or_compress == 1)
                        new(cluster_parent) cluster_t(homogeneous_binary_functor(std::as_const(*cluster_child_left), std::as_const(*cluster_child_right)));
                    else if(dummy_or_rake_or_compress == 2)
                        new(cluster_parent) cluster_t(homogeneous_binary_functor(std::as_const(*cluster_child_left), std::as_const(*cluster_child_right)));
                    else std::unreachable();
                }
                void split([[maybe_unused]] graph_t *graph, cluster_t *cluster_parent, [[maybe_unused]] graph_t::it_vertex_t it_vertex1, [[maybe_unused]] cluster_t *cluster_child_left, [[maybe_unused]] graph_t::it_vertex_t it_vertex2, [[maybe_unused]] cluster_t *cluster_child_right, [[maybe_unused]] graph_t::it_vertex_t it_vertex3, [[maybe_unused]] int dummy_or_rake_or_compress) const
                {
                    cluster_parent->~cluster_t();
                }
            };
        };

        template<typename cluster_t_, typename homogeneous_binary_functor_t>
        struct get_top_tree_internal_operations_accumulating_path_t
        {
            template<typename graph_t>
            struct type
            {
                using cluster_t = cluster_t_;
                homogeneous_binary_functor_t homogeneous_binary_functor;
                void create([[maybe_unused]] graph_t *graph, [[maybe_unused]] graph_t::it_vertex_t it_vertex1, cluster_t *cluster, [[maybe_unused]] graph_t::it_vertex_t it_vertex2, graph_t::it_edge_it_vertexes_t it_edge_it_vertexes) const
                {
                    new(cluster) cluster_t(std::as_const(it_edge_it_vertexes->first));
                }
                void destroy([[maybe_unused]] graph_t *graph, [[maybe_unused]] graph_t::it_vertex_t it_vertex1, cluster_t *cluster, [[maybe_unused]] graph_t::it_vertex_t it_vertex2) const
                {
                    cluster->~cluster_t();
                }
                void join([[maybe_unused]] graph_t *graph, cluster_t *cluster_parent, [[maybe_unused]] graph_t::it_vertex_t it_vertex1, cluster_t *cluster_child_left, [[maybe_unused]] graph_t::it_vertex_t it_vertex2, cluster_t *cluster_child_right, [[maybe_unused]] graph_t::it_vertex_t it_vertex3, int dummy_or_rake_or_compress) const
                {
                    if(dummy_or_rake_or_compress == 0)
                        new(cluster_parent) cluster_t(std::as_const(*cluster_child_left));
                    else if(dummy_or_rake_or_compress == 1)
                        new(cluster_parent) cluster_t(std::as_const(*cluster_child_right));
                    else if(dummy_or_rake_or_compress == 2)
                        new(cluster_parent) cluster_t(homogeneous_binary_functor(std::as_const(*cluster_child_left), std::as_const(*cluster_child_right)));
                    else std::unreachable();
                }
                void split([[maybe_unused]] graph_t *graph, cluster_t *cluster_parent, [[maybe_unused]] graph_t::it_vertex_t it_vertex1, [[maybe_unused]] cluster_t *cluster_child_left, [[maybe_unused]] graph_t::it_vertex_t it_vertex2, [[maybe_unused]] cluster_t *cluster_child_right, [[maybe_unused]] graph_t::it_vertex_t it_vertex3, [[maybe_unused]] int dummy_or_rake_or_compress) const
                {
                    cluster_parent->~cluster_t();
                }
            };
        };

        template<typename comparator_edge_t>
        struct get_top_tree_internal_operations_selecting_tree_t
        {
            template<typename graph_t>
            struct type
            {
                using cluster_t = graph_t::it_edge_it_vertexes_t;
                comparator_edge_t comparator_edge;
                void create([[maybe_unused]] graph_t *graph, [[maybe_unused]] graph_t::it_vertex_t it_vertex1, cluster_t *cluster, [[maybe_unused]] graph_t::it_vertex_t it_vertex2, graph_t::it_edge_it_vertexes_t it_edge_it_vertexes) const
                {
                    new(cluster) cluster_t(it_edge_it_vertexes);
                }
                void destroy([[maybe_unused]] graph_t *graph, [[maybe_unused]] graph_t::it_vertex_t it_vertex1, cluster_t *cluster, [[maybe_unused]] graph_t::it_vertex_t it_vertex2) const
                {
                    cluster->~cluster_t();
                }
                void join([[maybe_unused]] graph_t *graph, cluster_t *cluster_parent, [[maybe_unused]] graph_t::it_vertex_t it_vertex1, cluster_t *cluster_child_left, [[maybe_unused]] graph_t::it_vertex_t it_vertex2, cluster_t *cluster_child_right, [[maybe_unused]] graph_t::it_vertex_t it_vertex3, int dummy_or_rake_or_compress) const
                {
                    if(dummy_or_rake_or_compress == 0)
                        new(cluster_parent) cluster_t(std::as_const(*cluster_child_left));
                    else if(dummy_or_rake_or_compress == 1)
                        new(cluster_parent) cluster_t(!comparator_edge(std::as_const(*cluster_child_right), std::as_const(*cluster_child_left)) ? std::as_const(*cluster_child_right) : std::as_const(*cluster_child_left));
                    else if(dummy_or_rake_or_compress == 2)
                        new(cluster_parent) cluster_t(!comparator_edge(std::as_const(*cluster_child_right), std::as_const(*cluster_child_left)) ? std::as_const(*cluster_child_right) : std::as_const(*cluster_child_left));
                    else std::unreachable();
                }
                void split([[maybe_unused]] graph_t *graph, cluster_t *cluster_parent, [[maybe_unused]] graph_t::it_vertex_t it_vertex1, [[maybe_unused]] cluster_t *cluster_child_left, [[maybe_unused]] graph_t::it_vertex_t it_vertex2, [[maybe_unused]] cluster_t *cluster_child_right, [[maybe_unused]] graph_t::it_vertex_t it_vertex3, [[maybe_unused]] int dummy_or_rake_or_compress) const
                {
                    cluster_parent->~cluster_t();
                }
            };
        };

        template<typename comparator_edge_t>
        struct get_top_tree_internal_operations_selecting_path_t
        {
            template<typename graph_t>
            struct type
            {
                using cluster_t = graph_t::it_edge_it_vertexes_t;
                struct comparator_it_edge_it_vertexes_t
                {
                    comparator_edge_t comparator_edge;
                    bool operator()(typename graph_t::it_edge_it_vertexes_t const &lhs, typename graph_t::it_edge_it_vertexes_t const &rhs) const
                    {
                        if(comparator_edge(lhs->first, rhs->first))
                            return true;
                        else if(comparator_edge(rhs->first, lhs->first))
                            return false;
                        else
                            return std::as_const(detail::utility::unmove(std::less<typename graph_t::edges_it_vertexes_t::key_type const *>{}))(&lhs->first, &rhs->first);
                    }
                } comparator_it_edge_it_vertexes;
                void create([[maybe_unused]] graph_t *graph, [[maybe_unused]] graph_t::it_vertex_t it_vertex1, cluster_t *cluster, [[maybe_unused]] graph_t::it_vertex_t it_vertex2, graph_t::it_edge_it_vertexes_t it_edge_it_vertexes) const
                {
                    new(cluster) cluster_t(it_edge_it_vertexes);
                }
                void destroy([[maybe_unused]] graph_t *graph, [[maybe_unused]] graph_t::it_vertex_t it_vertex1, cluster_t *cluster, [[maybe_unused]] graph_t::it_vertex_t it_vertex2) const
                {
                    cluster->~cluster_t();
                }
                void join([[maybe_unused]] graph_t *graph, cluster_t *cluster_parent, [[maybe_unused]] graph_t::it_vertex_t it_vertex1, cluster_t *cluster_child_left, [[maybe_unused]] graph_t::it_vertex_t it_vertex2, cluster_t *cluster_child_right, [[maybe_unused]] graph_t::it_vertex_t it_vertex3, int dummy_or_rake_or_compress) const
                {
                    if(dummy_or_rake_or_compress == 0)
                        new(cluster_parent) cluster_t(std::as_const(*cluster_child_left));
                    else if(dummy_or_rake_or_compress == 1)
                        new(cluster_parent) cluster_t(std::as_const(*cluster_child_right));
                    else if(dummy_or_rake_or_compress == 2)
                        new(cluster_parent) cluster_t(!comparator_it_edge_it_vertexes(*cluster_child_right, *cluster_child_left) ? std::as_const(*cluster_child_right) : std::as_const(*cluster_child_left));
                    else std::unreachable();
                }
                void split([[maybe_unused]] graph_t *graph, cluster_t *cluster_parent, [[maybe_unused]] graph_t::it_vertex_t it_vertex1, [[maybe_unused]] cluster_t *cluster_child_left, [[maybe_unused]] graph_t::it_vertex_t it_vertex2, [[maybe_unused]] cluster_t *cluster_child_right, [[maybe_unused]] graph_t::it_vertex_t it_vertex3, [[maybe_unused]] int dummy_or_rake_or_compress) const
                {
                    cluster_parent->~cluster_t();
                }
                std::optional<typename graph_t::it_edge_it_vertexes_t> edge_to_be_replaced([[maybe_unused]] graph_t *graph, cluster_t const &cluster, graph_t::it_edge_it_vertexes_t it_edge_it_vertexes) const
                {
                    if(comparator_it_edge_it_vertexes(it_edge_it_vertexes, cluster))
                        return std::make_optional(cluster);
                    else
                        return std::nullopt;
                }
            };
        };
    } // namespace augmented_graph_helpers

    namespace detail
    {
        namespace augmented_graph
        {
            using namespace language;
            using namespace concepts;
            using namespace utility;
            using namespace memory;
            using namespace iterator;
            using namespace coroutine;
            using namespace functional;

            template<typename graph_t, std::size_t index, typename part_data_structure_t, typename part_parameters_t>
            struct augmented_graph_part_t;

            template<typename graph_t, std::size_t index, typename part_data_structure_t, typename part_parameters_t>
                requires(static_cast<augmented_graph_part_data_structure_e>(part_data_structure_t{}) == augmented_graph_part_data_structure_e::top_tree)
            struct augmented_graph_part_t<graph_t, index, part_data_structure_t, part_parameters_t>: public augmented_graph_part_base
            {
                graph_t *graph() { return reinterpret_cast<graph_t *>(reinterpret_cast<std::byte *>(this) - graph_t::template part_offset<index>()); }
                graph_t const *graph() const { return reinterpret_cast<graph_t const *>(reinterpret_cast<std::byte const *>(this) - graph_t::template part_offset<index>()); }

                using allocator_vertex_t = typename graph_t::allocator_vertex_t;

                using it_vertex_t = typename graph_t::it_vertex_t;
                using allocator_it_vertex_t = std::allocator_traits<allocator_vertex_t>::template rebind_alloc<it_vertex_t>;
                using comparator_it_vertex_t = typename graph_t::comparator_it_vertex_t;
                using comparator_it_vertex_address_t = typename graph_t::comparator_it_vertex_address_t;

                using it_edge_it_vertexes_t = typename graph_t::it_edge_it_vertexes_t;
                using allocator_it_edge_it_vertexes_t = typename graph_t::allocator_it_edge_it_vertexes_t;
                using comparator_it_edge_it_vertexes_t = typename graph_t::comparator_it_edge_it_vertexes_t;
                using comparator_it_edge_it_vertexes_address_t = typename graph_t::comparator_it_edge_it_vertexes_address_t;

                using cluster_t = part_parameters_t::cluster_t;
                using internal_operations_t = part_parameters_t;

                template<typename pointer_vertex_t, typename cluster_t>
                struct add_cluster_member_t
                {
                    alignas(cluster_t) std::byte cluster_buffer[sizeof(cluster_t)]; // cluster_t cluster;
                    typename std::pointer_traits<pointer_vertex_t>::template rebind<cluster_t> p_cluster() { return std::pointer_traits<typename std::pointer_traits<pointer_vertex_t>::template rebind<cluster_t>>::pointer_to(*reinterpret_cast<cluster_t *>(&cluster_buffer)); }
                };
                template<typename pointer_element_t>
                struct add_cluster_member_t<pointer_element_t, void>
                {
                };

                template<typename internal_operations_t>
                struct add_internal_operations_member_t
                {
                    internal_operations_t internal_operations;
                };
                template<>
                struct add_internal_operations_member_t<void>
                {
                };

                struct cluster_tree_node_t;
                struct arc_t
                {
                    arc_t *prev_, *next_;
                    it_vertex_t head;

                    struct proxy_prev_t
                    {
                        arc_t *this_;
                        operator arc_t *() const { return tagged_ptr_bit0_unsetted_relaxed(this_->prev_); }
                        arc_t *operator->() { return this->operator arc_t *(); }
                        proxy_prev_t &operator=(arc_t *other)
                        {
                            this_->prev_ = tagged_ptr_bit0_is_setted(this_->prev_) ? tagged_ptr_bit0_setted(other) : other;
                            return *this;
                        }
                        proxy_prev_t &operator=(proxy_prev_t const &other) { return this->operator=(other.operator arc_t *()); }
                    };
                    proxy_prev_t prev() { return proxy_prev_t{.this_ = this}; }

                    struct proxy_next_t
                    {
                        arc_t *this_;
                        operator arc_t *() const { return tagged_ptr_bit0_unsetted_relaxed(this_->next_); }
                        arc_t *operator->() { return this->operator arc_t *(); }
                        proxy_next_t &operator=(arc_t *other)
                        {
                            this_->next_ = tagged_ptr_bit0_is_setted(this_->next_) ? tagged_ptr_bit0_setted(other) : other;
                            return *this;
                        }
                        proxy_next_t &operator=(proxy_next_t const &other) { return this->operator=(other.operator arc_t *()); }
                    };
                    proxy_next_t next() { return proxy_next_t{.this_ = this}; }

                    cluster_tree_node_t *cluster_tree_node()
                    {
                        if(!tagged_ptr_bit0_is_setted(this->prev_))
#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Winvalid-offsetof"
#endif
                            return reinterpret_cast<cluster_tree_node_t *>(reinterpret_cast<std::byte *>(this) - offsetof(cluster_tree_node_t, arc_backward));
#ifdef __clang__
    #pragma clang diagnostic pop
#endif
                        else
#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Winvalid-offsetof"
#endif
                            return reinterpret_cast<cluster_tree_node_t *>(reinterpret_cast<std::byte *>(this) - offsetof(cluster_tree_node_t, arc_forward));
#ifdef __clang__
    #pragma clang diagnostic pop
#endif
                    }
                    arc_t *twin()
                    {
                        if(!tagged_ptr_bit0_is_setted(this->prev_))
                            return &this->cluster_tree_node()->arc_forward;
                        else
                            return &this->cluster_tree_node()->arc_backward;
                    }
                };
                struct cluster_tree_node_t: public add_cluster_member_t<typename std::allocator_traits<allocator_vertex_t>::pointer, cluster_t>
                {
                    cluster_tree_node_t *parent = nullptr, *child_left_ = nullptr, *child_right_ = nullptr;
                    arc_t arc_backward, arc_forward;

                    cluster_tree_node_t(std::nullptr_t) // not initialized
                    {}

                    cluster_tree_node_t(it_vertex_t it_vertex1, it_vertex_t it_vertex2)
                        : arc_backward{.prev_ = nullptr, .next_ = nullptr, .head = it_vertex1},
                          arc_forward{.prev_ = tagged_ptr_bit0_setted(static_cast<arc_t *>(nullptr)), .next_ = nullptr, .head = it_vertex2}
                    {}

                    arc_t *arc_with_head(it_vertex_t head)
                    {
                        if(arc_backward.head == head)
                            return &arc_backward;
                        else if(arc_forward.head == head)
                            return &arc_forward;
                        else
                            std::unreachable();
                    }
                    arc_t *arc_with_tail(it_vertex_t tail)
                    {
                        if(arc_backward.head == tail)
                            return &arc_forward;
                        else if(arc_forward.head == tail)
                            return &arc_backward;
                        else
                            std::unreachable();
                    }

                    struct proxy_child_left_t
                    {
                        cluster_tree_node_t *this_;
                        operator cluster_tree_node_t *() const { return tagged_ptr_bit0_unsetted_relaxed(this_->child_left_); }
                        cluster_tree_node_t *operator->() { return this->operator cluster_tree_node_t *(); }
                        proxy_child_left_t &operator=(cluster_tree_node_t *other)
                        {
                            this_->child_left_ = tagged_ptr_bit0_is_setted(this_->child_left_) ? tagged_ptr_bit0_setted(other) : other;
                            return *this;
                        }
                        proxy_child_left_t(cluster_tree_node_t *this_)
                            : this_(this_)
                        {}
                        proxy_child_left_t(proxy_child_left_t const &other) = default;
                        proxy_child_left_t &operator=(proxy_child_left_t const &other) { return this->operator=(other.operator cluster_tree_node_t *()); }
                    };
                    proxy_child_left_t child_left() { return {this}; }
                    struct proxy_is_marked_t
                    {
                        cluster_tree_node_t *this_;
                        operator bool() const { return tagged_ptr_bit0_is_setted(this_->child_left_); }
                        proxy_is_marked_t &operator=(bool other)
                        {
                            this_->child_left_ = other ? tagged_ptr_bit0_setted_relaxed(this_->child_left_) : tagged_ptr_bit0_unsetted_relaxed(this_->child_left_);
                            return *this;
                        }
                        proxy_is_marked_t &operator=(proxy_is_marked_t const &other) { return this->operator=(other.operator bool()); }
                    };
                    proxy_is_marked_t is_marked() { return proxy_is_marked_t{.this_ = this}; }

                    struct proxy_child_right_t
                    {
                        cluster_tree_node_t *this_;
                        operator cluster_tree_node_t *() const { return tagged_ptr_bit0_unsetted_relaxed(this_->child_right_); }
                        cluster_tree_node_t *operator->() { return this->operator cluster_tree_node_t *(); }
                        proxy_child_right_t &operator=(cluster_tree_node_t *other)
                        {
                            this_->child_right_ = tagged_ptr_bit0_is_setted(this_->child_right_) ? tagged_ptr_bit0_setted(other) : other;
                            return *this;
                        }
                        proxy_child_right_t(cluster_tree_node_t *this_)
                            : this_(this_)
                        {}
                        proxy_child_right_t(proxy_child_right_t const &other) = default;
                        proxy_child_right_t &operator=(proxy_child_right_t const &other) { return this->operator=(other.operator cluster_tree_node_t *()); }
                    };
                    proxy_child_right_t child_right() { return {this}; }
                    struct proxy_contraction_type_t
                    {
                        cluster_tree_node_t *this_;
                        operator bool() const { return tagged_ptr_bit0_is_setted(this_->child_right_); }
                        proxy_contraction_type_t &operator=(bool other)
                        {
                            this_->child_right_ = other ? tagged_ptr_bit0_setted_relaxed(this_->child_right_) : tagged_ptr_bit0_unsetted_relaxed(this_->child_right_);
                            return *this;
                        }
                        proxy_contraction_type_t &operator=(proxy_contraction_type_t const &other) { return this->operator=(other.operator bool()); }
                    };
                    proxy_contraction_type_t contraction_type() { return proxy_contraction_type_t{.this_ = this}; }

                    int dummy_or_rake_or_compress()
                    {
                        if(this->child_left() != nullptr && this->child_right() == nullptr)
                            return 0;
                        else if(this->child_left() != nullptr && this->child_right() != nullptr)
                        {
                            if(!this->contraction_type())
                                return 1;
                            else
                                return 2;
                        }
                        else std::unreachable();
                    }
                    std::tuple<it_vertex_t, it_vertex_t> dummy_it_vertexes()
                    {
                        assert(this->child_left() != nullptr);
                        assert(this->child_right() == nullptr);
                        return std::make_tuple(this->arc_backward.head, this->arc_forward.head);
                    }
                    std::tuple<it_vertex_t, it_vertex_t, it_vertex_t> rake_it_vertexes()
                    {
                        assert(this->child_left() != nullptr);
                        assert(this->child_right() != nullptr);
                        assert(this->contraction_type() == false);
                        it_vertex_t it_vertex1, it_vertex2, it_vertex3;
                        if(this->child_left()->arc_backward.head == this->arc_backward.head || this->child_left()->arc_backward.head == this->arc_forward.head)
                            std::tie(it_vertex1, it_vertex2) = std::make_tuple(this->child_left()->arc_forward.head, this->child_left()->arc_backward.head);
                        else if(this->child_left()->arc_forward.head == this->arc_backward.head || this->child_left()->arc_forward.head == this->arc_forward.head)
                            std::tie(it_vertex1, it_vertex2) = std::make_tuple(this->child_left()->arc_backward.head, this->child_left()->arc_forward.head);
                        else std::unreachable();
                        if(this->arc_backward.head == this->child_left()->arc_backward.head || this->arc_backward.head == this->child_left()->arc_forward.head)
                            it_vertex3 = this->arc_forward.head;
                        else if(this->arc_forward.head == this->child_left()->arc_backward.head || this->arc_forward.head == this->child_left()->arc_forward.head)
                            it_vertex3 = this->arc_backward.head;
                        else std::unreachable();
                        return std::make_tuple(it_vertex1, it_vertex2, it_vertex3);
                    }
                    std::tuple<arc_t *, arc_t *> rake_arc_in_arc_out()
                    {
                        assert(this->child_left() != nullptr);
                        assert(this->child_right() != nullptr);
                        assert(this->contraction_type() == false);
                        arc_t *arc_in, *arc_out;
                        if(this->child_left()->arc_backward.head == this->arc_backward.head || this->child_left()->arc_backward.head == this->arc_forward.head)
                            arc_in = &this->child_left()->arc_backward;
                        else if(this->child_left()->arc_forward.head == this->arc_backward.head || this->child_left()->arc_forward.head == this->arc_forward.head)
                            arc_in = &this->child_left()->arc_forward;
                        else std::unreachable();
                        if(this->child_left()->arc_backward.head == this->arc_backward.head || this->child_left()->arc_forward.head == this->arc_backward.head)
                            arc_out = &this->child_right()->arc_forward;
                        else if(this->child_left()->arc_backward.head == this->arc_forward.head || this->child_left()->arc_forward.head == this->arc_forward.head)
                            arc_out = &this->child_right()->arc_backward;
                        else std::unreachable();
                        return std::make_tuple(arc_in, arc_out);
                    }
                    std::tuple<it_vertex_t, it_vertex_t, it_vertex_t> compress_it_vertexes()
                    {
                        assert(this->child_left() != nullptr);
                        assert(this->child_right() != nullptr);
                        assert(this->contraction_type() == true);
                        it_vertex_t it_vertex1, it_vertex2, it_vertex3;
                        if(this->child_left()->arc_backward.head == this->arc_backward.head)
                            std::tie(it_vertex1, it_vertex2) = std::make_tuple(this->child_left()->arc_backward.head, this->child_left()->arc_forward.head);
                        else if(this->child_left()->arc_forward.head == this->arc_backward.head)
                            std::tie(it_vertex1, it_vertex2) = std::make_tuple(this->child_left()->arc_forward.head, this->child_left()->arc_backward.head);
                        else std::unreachable();
                        if(this->child_right()->arc_forward.head == this->arc_forward.head)
                            it_vertex3 = this->child_right()->arc_forward.head;
                        else if(this->child_right()->arc_backward.head == this->arc_forward.head)
                            it_vertex3 = this->child_right()->arc_backward.head;
                        else std::unreachable();
                        return std::make_tuple(it_vertex1, it_vertex2, it_vertex3);
                    }
                    std::tuple<arc_t *, arc_t *> compress_arc_in_from_vertex1_arc_in_from_vertex2()
                    {
                        assert(this->child_left() != nullptr);
                        assert(this->child_right() != nullptr);
                        assert(this->contraction_type() == true);
                        arc_t *arc_in_from_vertex1, *arc_in_from_vertex2;
                        if(this->child_left()->arc_backward.head == this->arc_backward.head)
                            arc_in_from_vertex1 = &this->child_left()->arc_forward;
                        else if(this->child_left()->arc_forward.head == this->arc_backward.head)
                            arc_in_from_vertex1 = &this->child_left()->arc_backward;
                        else std::unreachable();
                        if(this->child_right()->arc_forward.head == this->arc_forward.head)
                            arc_in_from_vertex2 = &this->child_right()->arc_backward;
                        else if(this->child_right()->arc_backward.head == this->arc_forward.head)
                            arc_in_from_vertex2 = &this->child_right()->arc_forward;
                        else std::unreachable();
                        return std::make_tuple(arc_in_from_vertex1, arc_in_from_vertex2);
                    }
                };

                struct cluster_list_node_navigator_t
                {
                    cluster_list_node_navigator_t *prev, *next;
                    cluster_list_node_navigator_t() // initialize to node_end
                        : prev(tagged_ptr_bit0_setted(this)),
                          next(tagged_ptr_bit0_setted(this))
                    {}
                    cluster_list_node_navigator_t(std::nullptr_t) // not initialized
                        : prev(nullptr),
                          next(nullptr)
                    {}

                    static void push_impl(cluster_list_node_navigator_t *tagged_end, cluster_list_node_navigator_t *node_new)
                    {
                        if(tagged_ptr_bit0_unsetted(tagged_end)->prev == tagged_end && tagged_ptr_bit0_unsetted(tagged_end)->next == tagged_end)
                        {
                            node_new->prev = node_new->next = tagged_end;
                            tagged_ptr_bit0_unsetted(tagged_end)->prev = tagged_ptr_bit0_unsetted(tagged_end)->next = tagged_ptr_bit0_setted(node_new);
                        }
                        else if(tagged_ptr_bit0_unsetted(tagged_end)->prev != tagged_end && tagged_ptr_bit0_unsetted(tagged_end)->next != tagged_end)
                        {
                            node_new->prev = tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(tagged_end)->prev);
                            node_new->prev->next = node_new;
                            node_new->next = tagged_end;
                            tagged_ptr_bit0_unsetted(tagged_end)->prev = tagged_ptr_bit0_setted(node_new);
                        }
                        else std::unreachable();
                    }

                    static void extract_impl(cluster_list_node_navigator_t *node)
                    {
                        if(tagged_ptr_bit0_is_setted(node->prev))
                            tagged_ptr_bit0_unsetted(node->prev)->next = tagged_ptr_bit0_setted_relaxed(node->next);
                        else
                            node->prev->next = node->next;
                        if(tagged_ptr_bit0_is_setted(node->next))
                            tagged_ptr_bit0_unsetted(node->next)->prev = tagged_ptr_bit0_setted_relaxed(node->prev);
                        else
                            node->next->prev = node->prev;
                    }

                    static cluster_list_node_navigator_t *untagged_prev_or_tagged_end(cluster_list_node_navigator_t *tagged_end)
                    {
                        if(cluster_list_node_navigator_t *prev = tagged_ptr_bit0_unsetted(tagged_end)->prev; prev == tagged_end)
                            return prev;
                        else
                            return tagged_ptr_bit0_unsetted(prev);
                    }

                    static cluster_list_node_navigator_t *untagged_next_or_tagged_end(cluster_list_node_navigator_t *tagged_end)
                    {
                        if(cluster_list_node_navigator_t *next = tagged_ptr_bit0_unsetted(tagged_end)->next; next == tagged_end)
                            return next;
                        else
                            return tagged_ptr_bit0_unsetted(next);
                    }
                };
                struct cluster_list_node_t: public cluster_list_node_navigator_t
                {
                    cluster_tree_node_t *tree_root;
                };
                struct cluster_list_node_end_t: public cluster_list_node_navigator_t, public add_internal_operations_member_t<internal_operations_t>
                {
                    std::size_t node_count = 0;
                };
                cluster_list_node_end_t *cluster_list_node_end;

                using allocator_it_vertex_arc_and_it_edge_it_vertexes_base_level_t = std::allocator_traits<allocator_vertex_t>::template rebind_alloc<std::pair<it_vertex_t const, std::pair<arc_t *, it_edge_it_vertexes_t>>>;
                using allocator_it_vertex_it_vertexes_arc_and_it_edge_it_vertexes_base_level_t = std::allocator_traits<allocator_vertex_t>::template rebind_alloc<std::pair< //
                    it_vertex_t const, //
                    std::map< //
                        it_vertex_t, //
                        std::pair<arc_t *, it_edge_it_vertexes_t>, //
                        std::reference_wrapper<comparator_it_vertex_address_t const>, //
                        allocator_it_vertex_arc_and_it_edge_it_vertexes_base_level_t //
                        > //
                    >>;
                std::map< //
                    it_vertex_t, //
                    std::map< //
                        it_vertex_t, //
                        std::pair<arc_t *, it_edge_it_vertexes_t>, //
                        std::reference_wrapper<comparator_it_vertex_address_t const>, //
                        allocator_it_vertex_arc_and_it_edge_it_vertexes_base_level_t //
                        >, //
                    std::reference_wrapper<comparator_it_vertex_address_t const>, //
                    allocator_it_vertex_it_vertexes_arc_and_it_edge_it_vertexes_base_level_t>
                    it_vertexes_it_vertexes_arc_and_it_edge_it_vertexes_base_level;

                std::set< //
                    it_vertex_t, //
                    std::reference_wrapper<comparator_it_vertex_address_t const>, //
                    allocator_it_vertex_t //
                    >
                    it_vertexes_trivial_component;

                template<typename internal_operations_t>
                struct extract_comparator_it_edge_it_vertexes_t_from_internal_operations_t_t: std::type_identity<void>
                {};
                template<typename internal_operations_t>
                    requires(std::strict_weak_order<typename internal_operations_t::comparator_it_edge_it_vertexes_t, it_edge_it_vertexes_t, it_edge_it_vertexes_t>)
                struct extract_comparator_it_edge_it_vertexes_t_from_internal_operations_t_t<internal_operations_t>: std::type_identity<typename internal_operations_t::comparator_it_edge_it_vertexes_t>
                {};
                template<typename internal_operations_t>
                using extract_comparator_it_edge_it_vertexes_t_from_internal_operations_t = extract_comparator_it_edge_it_vertexes_t_from_internal_operations_t_t<internal_operations_t>::type;

                using comparator_it_edge_it_vertexes_internal_operations_t = extract_comparator_it_edge_it_vertexes_t_from_internal_operations_t<internal_operations_t>;
                using comparator_it_edge_fundamental_t = std::conditional_t<std::is_same_v<comparator_it_edge_it_vertexes_internal_operations_t, void>, comparator_it_edge_it_vertexes_t, comparator_it_edge_it_vertexes_internal_operations_t>;
                std::set< //
                    it_edge_it_vertexes_t, //
                    std::reference_wrapper<comparator_it_edge_fundamental_t const>, //
                    allocator_it_edge_it_vertexes_t //
                    >
                    it_edges_fundamental;

                std::optional<it_vertex_t> it_vertex_exposed1, it_vertex_exposed2;

                bool belongs_to_same_component(it_vertex_t it_vertex1, it_vertex_t it_vertex2) const
                {
                    if(it_vertex1 == it_vertex2)
                        return true;
                    else
                    {
                        if(it_vertexes_trivial_component.find(it_vertex1) != it_vertexes_trivial_component.end())
                            return false;
                        else if(it_vertexes_trivial_component.find(it_vertex2) != it_vertexes_trivial_component.end())
                            return false;
                        else
                        {
                            cluster_tree_node_t *tree_node1 = (*it_vertexes_it_vertexes_arc_and_it_edge_it_vertexes_base_level.at(it_vertex1).begin()).second.first->cluster_tree_node();
                            cluster_tree_node_t *tree_node2 = (*it_vertexes_it_vertexes_arc_and_it_edge_it_vertexes_base_level.at(it_vertex2).begin()).second.first->cluster_tree_node();
                            while(true)
                            {
                                if(tree_node1 == tree_node2)
                                    return true;
                                else
                                {
                                    if(tagged_ptr_bit0_is_setted(tree_node1->parent))
                                        return false;
                                    else if(tagged_ptr_bit0_is_setted(tree_node2->parent))
                                        return false;
                                    else
                                    {
                                        tree_node1 = tree_node1->parent;
                                        tree_node2 = tree_node2->parent;
                                    }
                                }
                            }
                        }
                    }
                }

                void update(std::size_t current_level, std::vector<cluster_tree_node_t *> clusters_to_be_erased, std::vector<cluster_tree_node_t *> clusters_affected, std::vector<cluster_tree_node_t *> clusters_to_be_inserted, std::vector<it_edge_it_vertexes_t> it_edges_to_be_inserted, function_view<void()> split_clusters_to_be_erased_and_join_clusters_to_be_inserted)
                {
                    auto connect_arc = [](arc_t *predecessor, arc_t *successor)
                    {
                        predecessor->next() = successor;
                        successor->prev() = predecessor;
                    };

                    auto if_not_marked_then_mark_and_add_to_clusters_affected = [&clusters_affected](cluster_tree_node_t *cluster_tree_node)
                    {
                        assert(cluster_tree_node != nullptr);
                        if(!cluster_tree_node->is_marked())
                        {
                            cluster_tree_node->is_marked() = true;
                            clusters_affected.push_back(cluster_tree_node);
                        }
                    };

                    std::vector<cluster_tree_node_t *> clusters_to_be_erased_new;
                    auto if_not_null_and_not_list_node_and_not_marked_then_mark_and_add_to_clusters_to_be_erased_new = [&clusters_to_be_erased_new](cluster_tree_node_t *cluster_tree_node)
                    {
                        if(cluster_tree_node != nullptr && !tagged_ptr_bit0_is_setted(cluster_tree_node) && !cluster_tree_node->is_marked())
                        {
                            cluster_tree_node->is_marked() = true;
                            clusters_to_be_erased_new.push_back(cluster_tree_node);
                        }
                    };
                    auto if_not_null_and_list_node_delete_list_node = [this](cluster_tree_node_t *cluster_tree_node)
                    {
                        if(cluster_tree_node != nullptr && tagged_ptr_bit0_is_setted(cluster_tree_node))
                        {
                            cluster_list_node_t *cluster_list_node = reinterpret_cast<cluster_list_node_t *>(tagged_ptr_bit0_unsetted(cluster_tree_node));
                            cluster_list_node_navigator_t::extract_impl(cluster_list_node);
                            delete cluster_list_node;
                            --tagged_ptr_bit0_unsetted(cluster_list_node_end)->node_count;
                        }
                    };

                    std::vector<cluster_tree_node_t *> clusters_to_be_inserted_new;

                    // removal
                    for(cluster_tree_node_t *cluster_to_be_erased : clusters_to_be_erased)
                    {
                        assert(cluster_to_be_erased->is_marked());

                        connect_arc(cluster_to_be_erased->arc_forward.prev(), cluster_to_be_erased->arc_backward.next());
                        connect_arc(cluster_to_be_erased->arc_backward.prev(), cluster_to_be_erased->arc_forward.next());

                        if_not_null_and_not_list_node_and_not_marked_then_mark_and_add_to_clusters_to_be_erased_new(cluster_to_be_erased->parent);
                        if_not_null_and_list_node_delete_list_node(cluster_to_be_erased->parent);

                        if_not_marked_then_mark_and_add_to_clusters_affected(cluster_to_be_erased->arc_backward.prev()->cluster_tree_node());
                        if_not_marked_then_mark_and_add_to_clusters_affected(cluster_to_be_erased->arc_backward.next()->cluster_tree_node());
                        if_not_marked_then_mark_and_add_to_clusters_affected(cluster_to_be_erased->arc_forward.prev()->cluster_tree_node());
                        if_not_marked_then_mark_and_add_to_clusters_affected(cluster_to_be_erased->arc_forward.next()->cluster_tree_node());

                        if(current_level == 0)
                        {
                            auto it_vertex1_vertexes_arc_base_level = it_vertexes_it_vertexes_arc_and_it_edge_it_vertexes_base_level.find(cluster_to_be_erased->arc_backward.head);
                            assert(it_vertex1_vertexes_arc_base_level != it_vertexes_it_vertexes_arc_and_it_edge_it_vertexes_base_level.end());
                            auto it_vertex1_vertex2_arc_base_level = it_vertex1_vertexes_arc_base_level->second.find(cluster_to_be_erased->arc_forward.head);
                            assert(it_vertex1_vertex2_arc_base_level != it_vertex1_vertexes_arc_base_level->second.end());
                            it_vertex1_vertexes_arc_base_level->second.erase(it_vertex1_vertex2_arc_base_level);
                            if(it_vertex1_vertexes_arc_base_level->second.empty())
                            {
                                it_vertexes_it_vertexes_arc_and_it_edge_it_vertexes_base_level.erase(it_vertex1_vertexes_arc_base_level);
                                [[maybe_unused]] bool assert_result_of_expression_with_side_effect = it_vertexes_trivial_component.insert(cluster_to_be_erased->arc_backward.head).second;
                                assert((static_cast<void>("it_vertexes_trivial_component.insert(cluster_to_be_erased->arc_backward.head).second"), assert_result_of_expression_with_side_effect));
                            }

                            auto it_vertex2_vertexes_arc_base_level = it_vertexes_it_vertexes_arc_and_it_edge_it_vertexes_base_level.find(cluster_to_be_erased->arc_forward.head);
                            assert(it_vertex2_vertexes_arc_base_level != it_vertexes_it_vertexes_arc_and_it_edge_it_vertexes_base_level.end());
                            auto it_vertex2_vertex1_arc_base_level = it_vertex2_vertexes_arc_base_level->second.find(cluster_to_be_erased->arc_backward.head);
                            assert(it_vertex2_vertex1_arc_base_level != it_vertex2_vertexes_arc_base_level->second.end());
                            it_vertex2_vertexes_arc_base_level->second.erase(it_vertex2_vertex1_arc_base_level);
                            if(it_vertex2_vertexes_arc_base_level->second.empty())
                            {
                                it_vertexes_it_vertexes_arc_and_it_edge_it_vertexes_base_level.erase(it_vertex2_vertexes_arc_base_level);
                                [[maybe_unused]] bool assert_result_of_expression_with_side_effect = it_vertexes_trivial_component.insert(cluster_to_be_erased->arc_forward.head).second;
                                assert((static_cast<void>("it_vertexes_trivial_component.insert(cluster_to_be_erased->arc_forward.head).second"), assert_result_of_expression_with_side_effect));
                            }
                        }
                    }

                    // insertion
                    if(current_level == 0)
                    {
                        assert(clusters_to_be_inserted.size() == it_edges_to_be_inserted.size());

                        std::size_t cluster_to_be_inserted_index = 0;
                        for(cluster_tree_node_t *cluster_to_be_inserted : clusters_to_be_inserted)
                        {
                            assert(!cluster_to_be_inserted->is_marked());
                            cluster_to_be_inserted->is_marked() = true;

                            auto it_vertex1_vertexes_arc_base_level = it_vertexes_it_vertexes_arc_and_it_edge_it_vertexes_base_level.find(cluster_to_be_inserted->arc_backward.head);
                            if(it_vertex1_vertexes_arc_base_level == it_vertexes_it_vertexes_arc_and_it_edge_it_vertexes_base_level.end())
                            {
                                it_vertex1_vertexes_arc_base_level = it_vertexes_it_vertexes_arc_and_it_edge_it_vertexes_base_level.emplace(std::piecewise_construct, std::forward_as_tuple(cluster_to_be_inserted->arc_backward.head), std::forward_as_tuple(std::cref(graph()->comparator_it_vertex_address), allocator_it_vertex_arc_and_it_edge_it_vertexes_base_level_t(graph()->vertexes.get_allocator()))).first;
                                [[maybe_unused]] bool assert_result_of_expression_with_side_effect = it_vertexes_trivial_component.erase(cluster_to_be_inserted->arc_backward.head) == 1;
                                assert((static_cast<void>("it_vertexes_trivial_component.erase(cluster_to_be_inserted->arc_backward.head) == 1"), assert_result_of_expression_with_side_effect));
                            }
                            if(it_vertex1_vertexes_arc_base_level->second.empty())
                                connect_arc(&cluster_to_be_inserted->arc_backward, &cluster_to_be_inserted->arc_forward);
                            else
                            {
                                auto it_vertex1_vertex2_arc_base_level_successor = it_vertex1_vertexes_arc_base_level->second.lower_bound(cluster_to_be_inserted->arc_forward.head);
                                assert(!(it_vertex1_vertex2_arc_base_level_successor != it_vertex1_vertexes_arc_base_level->second.end()) || it_vertex1_vertex2_arc_base_level_successor->second.first->head != cluster_to_be_inserted->arc_forward.head);
                                auto it_vertex1_vertex2_arc_base_level_predecessor = std::ranges::prev(it_vertex1_vertex2_arc_base_level_successor == it_vertex1_vertexes_arc_base_level->second.begin() ? it_vertex1_vertexes_arc_base_level->second.end() : it_vertex1_vertex2_arc_base_level_successor);
                                it_vertex1_vertex2_arc_base_level_successor = it_vertex1_vertex2_arc_base_level_successor == it_vertex1_vertexes_arc_base_level->second.end() ? it_vertex1_vertexes_arc_base_level->second.begin() : it_vertex1_vertex2_arc_base_level_successor;
                                assert(it_vertex1_vertex2_arc_base_level_successor->second.first->prev() == it_vertex1_vertex2_arc_base_level_predecessor->second.first->twin());
                                assert(it_vertex1_vertex2_arc_base_level_predecessor->second.first->twin()->next() == it_vertex1_vertex2_arc_base_level_successor->second.first);
                                connect_arc(&cluster_to_be_inserted->arc_backward, it_vertex1_vertex2_arc_base_level_successor->second.first);
                                connect_arc(it_vertex1_vertex2_arc_base_level_predecessor->second.first->twin(), &cluster_to_be_inserted->arc_forward);
                                if_not_marked_then_mark_and_add_to_clusters_affected(it_vertex1_vertex2_arc_base_level_predecessor->second.first->cluster_tree_node());
                                if_not_marked_then_mark_and_add_to_clusters_affected(it_vertex1_vertex2_arc_base_level_successor->second.first->cluster_tree_node());
                            }
                            it_vertex1_vertexes_arc_base_level->second.emplace(std::piecewise_construct, std::forward_as_tuple(cluster_to_be_inserted->arc_forward.head), std::forward_as_tuple(std::piecewise_construct, std::forward_as_tuple(&cluster_to_be_inserted->arc_forward), std::forward_as_tuple(it_edges_to_be_inserted[cluster_to_be_inserted_index])));

                            auto it_vertex2_vertexes_arc_base_level = it_vertexes_it_vertexes_arc_and_it_edge_it_vertexes_base_level.find(cluster_to_be_inserted->arc_forward.head);
                            if(it_vertex2_vertexes_arc_base_level == it_vertexes_it_vertexes_arc_and_it_edge_it_vertexes_base_level.end())
                            {
                                it_vertex2_vertexes_arc_base_level = it_vertexes_it_vertexes_arc_and_it_edge_it_vertexes_base_level.emplace(std::piecewise_construct, std::forward_as_tuple(cluster_to_be_inserted->arc_forward.head), std::forward_as_tuple(std::cref(graph()->comparator_it_vertex_address), allocator_it_vertex_arc_and_it_edge_it_vertexes_base_level_t(graph()->vertexes.get_allocator()))).first;
                                [[maybe_unused]] bool assert_result_of_expression_with_side_effect = it_vertexes_trivial_component.erase(cluster_to_be_inserted->arc_forward.head) == 1;
                                assert((static_cast<void>("it_vertexes_trivial_component.erase(cluster_to_be_inserted->arc_forward.head) == 1"), assert_result_of_expression_with_side_effect));
                            }
                            if(it_vertex2_vertexes_arc_base_level->second.empty())
                                connect_arc(&cluster_to_be_inserted->arc_forward, &cluster_to_be_inserted->arc_backward);
                            else
                            {
                                auto it_vertex2_vertex1_arc_base_level_successor = it_vertex2_vertexes_arc_base_level->second.lower_bound(cluster_to_be_inserted->arc_backward.head);
                                assert(!(it_vertex2_vertex1_arc_base_level_successor != it_vertex2_vertexes_arc_base_level->second.end()) || it_vertex2_vertex1_arc_base_level_successor->second.first->head != cluster_to_be_inserted->arc_backward.head);
                                auto it_vertex2_vertex1_arc_base_level_predecessor = std::ranges::prev(it_vertex2_vertex1_arc_base_level_successor == it_vertex2_vertexes_arc_base_level->second.begin() ? it_vertex2_vertexes_arc_base_level->second.end() : it_vertex2_vertex1_arc_base_level_successor);
                                it_vertex2_vertex1_arc_base_level_successor = it_vertex2_vertex1_arc_base_level_successor == it_vertex2_vertexes_arc_base_level->second.end() ? it_vertex2_vertexes_arc_base_level->second.begin() : it_vertex2_vertex1_arc_base_level_successor;
                                assert(it_vertex2_vertex1_arc_base_level_successor->second.first->prev() == it_vertex2_vertex1_arc_base_level_predecessor->second.first->twin());
                                assert(it_vertex2_vertex1_arc_base_level_predecessor->second.first->twin()->next() == it_vertex2_vertex1_arc_base_level_successor->second.first);
                                connect_arc(it_vertex2_vertex1_arc_base_level_predecessor->second.first->twin(), &cluster_to_be_inserted->arc_backward);
                                connect_arc(&cluster_to_be_inserted->arc_forward, it_vertex2_vertex1_arc_base_level_successor->second.first);
                                if_not_marked_then_mark_and_add_to_clusters_affected(it_vertex2_vertex1_arc_base_level_predecessor->second.first->cluster_tree_node());
                                if_not_marked_then_mark_and_add_to_clusters_affected(it_vertex2_vertex1_arc_base_level_successor->second.first->cluster_tree_node());
                            }
                            it_vertex2_vertexes_arc_base_level->second.emplace(std::piecewise_construct, std::forward_as_tuple(cluster_to_be_inserted->arc_backward.head), std::forward_as_tuple(std::piecewise_construct, std::forward_as_tuple(&cluster_to_be_inserted->arc_backward), std::forward_as_tuple(it_edges_to_be_inserted[cluster_to_be_inserted_index])));

                            ++cluster_to_be_inserted_index;
                        }
                    }
                    else
                    {
                        for(cluster_tree_node_t *cluster_to_be_inserted : clusters_to_be_inserted)
                        {
                            assert(cluster_to_be_inserted->arc_backward.prev() == nullptr);
                            assert(cluster_to_be_inserted->arc_backward.next() == nullptr);
                            assert(cluster_to_be_inserted->arc_forward.prev() == nullptr);
                            assert(cluster_to_be_inserted->arc_forward.next() == nullptr);

                            assert(!cluster_to_be_inserted->is_marked());
                            cluster_to_be_inserted->is_marked() = true;
                        }
                        for(cluster_tree_node_t *cluster_to_be_inserted : clusters_to_be_inserted)
                        {
                            cluster_tree_node_t *child_cluster_vertex1, *child_cluster_vertex2;
                            if(cluster_to_be_inserted->child_left() != nullptr && cluster_to_be_inserted->child_right() == nullptr) // dummy
                                child_cluster_vertex1 = child_cluster_vertex2 = cluster_to_be_inserted->child_left();
                            else if(cluster_to_be_inserted->child_left() != nullptr && cluster_to_be_inserted->child_right() != nullptr)
                            {
                                if(!cluster_to_be_inserted->contraction_type()) // rake
                                    child_cluster_vertex1 = child_cluster_vertex2 = cluster_to_be_inserted->child_right();
                                else // compress
                                    std::tie(child_cluster_vertex1, child_cluster_vertex2) = std::make_tuple(cluster_to_be_inserted->child_left(), cluster_to_be_inserted->child_right());
                            }
                            else std::unreachable();
                            if(cluster_to_be_inserted->arc_forward.prev() == nullptr)
                            {
                                arc_t *child_arc_vertex1_as_tail = child_cluster_vertex1->arc_with_tail(cluster_to_be_inserted->arc_backward.head);
                                if(cluster_to_be_inserted->child_left() != nullptr && cluster_to_be_inserted->child_right() != nullptr && !cluster_to_be_inserted->contraction_type() &&
                                    child_arc_vertex1_as_tail->prev()->cluster_tree_node() == cluster_to_be_inserted->child_left())
                                    child_arc_vertex1_as_tail = child_arc_vertex1_as_tail->prev()->twin();
                                cluster_tree_node_t *neighboor_cluster_vertex1_as_head = child_arc_vertex1_as_tail->prev()->cluster_tree_node()->parent;
                                assert(neighboor_cluster_vertex1_as_head != nullptr && !tagged_ptr_bit0_is_setted(neighboor_cluster_vertex1_as_head));
                                connect_arc(neighboor_cluster_vertex1_as_head->arc_with_head(cluster_to_be_inserted->arc_backward.head), &cluster_to_be_inserted->arc_forward);
                                if_not_marked_then_mark_and_add_to_clusters_affected(neighboor_cluster_vertex1_as_head);
                            }
                            if(cluster_to_be_inserted->arc_backward.next() == nullptr)
                            {
                                arc_t *child_arc_vertex1_as_head = child_cluster_vertex1->arc_with_head(cluster_to_be_inserted->arc_backward.head);
                                cluster_tree_node_t *neighboor_cluster_vertex1_as_tail = child_arc_vertex1_as_head->next()->cluster_tree_node()->parent;
                                assert(neighboor_cluster_vertex1_as_tail != nullptr && !tagged_ptr_bit0_is_setted(neighboor_cluster_vertex1_as_tail));
                                connect_arc(&cluster_to_be_inserted->arc_backward, neighboor_cluster_vertex1_as_tail->arc_with_tail(cluster_to_be_inserted->arc_backward.head));
                                if_not_marked_then_mark_and_add_to_clusters_affected(neighboor_cluster_vertex1_as_tail);
                            }
                            if(cluster_to_be_inserted->arc_backward.prev() == nullptr)
                            {
                                arc_t *child_arc_vertex2_as_tail = child_cluster_vertex2->arc_with_tail(cluster_to_be_inserted->arc_forward.head);
                                if(cluster_to_be_inserted->child_left() != nullptr && cluster_to_be_inserted->child_right() != nullptr && !cluster_to_be_inserted->contraction_type() &&
                                    child_arc_vertex2_as_tail->prev()->cluster_tree_node() == cluster_to_be_inserted->child_left())
                                    child_arc_vertex2_as_tail = child_arc_vertex2_as_tail->prev()->twin();
                                cluster_tree_node_t *neighboor_cluster_vertex2_as_head = child_arc_vertex2_as_tail->prev()->cluster_tree_node()->parent;
                                assert(neighboor_cluster_vertex2_as_head != nullptr && !tagged_ptr_bit0_is_setted(neighboor_cluster_vertex2_as_head));
                                connect_arc(neighboor_cluster_vertex2_as_head->arc_with_head(cluster_to_be_inserted->arc_forward.head), &cluster_to_be_inserted->arc_backward);
                                if_not_marked_then_mark_and_add_to_clusters_affected(neighboor_cluster_vertex2_as_head);
                            }
                            if(cluster_to_be_inserted->arc_forward.next() == nullptr)
                            {
                                arc_t *child_arc_vertex2_as_head = child_cluster_vertex2->arc_with_head(cluster_to_be_inserted->arc_forward.head);
                                cluster_tree_node_t *neighboor_cluster_vertex2_as_tail = child_arc_vertex2_as_head->next()->cluster_tree_node()->parent;
                                assert(neighboor_cluster_vertex2_as_tail != nullptr && !tagged_ptr_bit0_is_setted(neighboor_cluster_vertex2_as_tail));
                                connect_arc(&cluster_to_be_inserted->arc_forward, neighboor_cluster_vertex2_as_tail->arc_with_tail(cluster_to_be_inserted->arc_forward.head));
                                if_not_marked_then_mark_and_add_to_clusters_affected(neighboor_cluster_vertex2_as_tail);
                            }
                        }
                    }

                    // detecting invalid moves
                    decltype(clusters_affected) clusters_affected_extra; // rake targets only
                    auto if_not_marked_then_mark_and_add_to_clusters_affected_extra = [&clusters_affected_extra](cluster_tree_node_t *cluster_tree_node)
                    {
                        if(!cluster_tree_node->is_marked())
                        {
                            cluster_tree_node->is_marked() = true;
                            clusters_affected_extra.push_back(cluster_tree_node);
                        }
                    };
                    for(cluster_tree_node_t *cluster_affected : clusters_affected)
                    {
                        cluster_tree_node_t *cluster_affected_parent = cluster_affected->parent;
                        assert(cluster_affected_parent != nullptr);

                        if(tagged_ptr_bit0_is_setted(cluster_affected_parent)) // root, i.e. not participating in any move
                            ;
                        else if(cluster_affected_parent->child_left() != nullptr && cluster_affected_parent->child_right() == nullptr) // dummy
                            ;
                        else if(cluster_affected_parent->child_left() != nullptr && cluster_affected_parent->child_right() != nullptr)
                        {
                            if(!cluster_affected_parent->contraction_type()) // rake
                            {
                                if(!cluster_affected_parent->is_marked())
                                {
                                    auto [arc_in, arc_out] = cluster_affected_parent->rake_arc_in_arc_out();
                                    if(!(arc_in->twin() == arc_in->prev() && arc_in->next() == arc_out &&
                                           it_vertex_exposed1 != arc_in->twin()->head &&
                                           it_vertex_exposed2 != arc_in->twin()->head))
                                    {
                                        if_not_null_and_not_list_node_and_not_marked_then_mark_and_add_to_clusters_to_be_erased_new(cluster_affected_parent);
                                        assert(cluster_affected_parent->child_left()->is_marked());
                                        if_not_marked_then_mark_and_add_to_clusters_affected_extra(cluster_affected_parent->child_right());
                                    }
                                }
                            }
                            else // compress
                            {
                                if(!cluster_affected_parent->is_marked())
                                {
                                    auto [arc_in_from_vertex1, arc_in_from_vertex2] = cluster_affected_parent->compress_arc_in_from_vertex1_arc_in_from_vertex2();
                                    if(!(arc_in_from_vertex1->twin() == arc_in_from_vertex2->next() && arc_in_from_vertex2->twin() == arc_in_from_vertex1->next() &&
                                           it_vertex_exposed1 != arc_in_from_vertex1->head &&
                                           it_vertex_exposed2 != arc_in_from_vertex1->head))
                                    {
                                        if_not_null_and_not_list_node_and_not_marked_then_mark_and_add_to_clusters_to_be_erased_new(cluster_affected_parent);
                                        assert(cluster_affected_parent->child_left()->is_marked());
                                        assert(cluster_affected_parent->child_right()->is_marked());
                                    }
                                }
                            }
                        }
                        else
                            std::unreachable();
                    }
#ifdef __EMSCRIPTEN__
                    clusters_affected.insert(clusters_affected.end(), clusters_affected_extra.begin(), clusters_affected_extra.end());
#else
                    clusters_affected.append_range(clusters_affected_extra);
#endif

                    // new moves
                    auto cluster_is_free = [](cluster_tree_node_t *cluster_tree_node)
                    {
                        return cluster_tree_node->parent == nullptr ||
                            tagged_ptr_bit0_is_setted(cluster_tree_node->parent) ||
                            cluster_tree_node->parent->is_marked() ||
                            (cluster_tree_node->parent->child_left() != nullptr && cluster_tree_node->parent->child_right() == nullptr);
                    };
                    auto new_move = [this, &cluster_is_free, &clusters_to_be_inserted_new, &if_not_null_and_not_list_node_and_not_marked_then_mark_and_add_to_clusters_to_be_erased_new, &if_not_null_and_list_node_delete_list_node](cluster_tree_node_t *cluster_to_be_inserted_or_cluster_affected)
                    {
                        if(cluster_to_be_inserted_or_cluster_affected->is_marked())
                        {
                            cluster_to_be_inserted_or_cluster_affected->is_marked() = false;
                            if(cluster_is_free(cluster_to_be_inserted_or_cluster_affected))
                            {
                                if(cluster_tree_node_t *cluster_rake_source1 = cluster_to_be_inserted_or_cluster_affected->arc_forward.prev()->cluster_tree_node();
                                    cluster_rake_source1 != cluster_to_be_inserted_or_cluster_affected &&
                                    cluster_to_be_inserted_or_cluster_affected->arc_forward.prev()->twin() == cluster_to_be_inserted_or_cluster_affected->arc_forward.prev()->prev() &&
                                    cluster_is_free(cluster_rake_source1) &&
                                    it_vertex_exposed1 != cluster_to_be_inserted_or_cluster_affected->arc_forward.prev()->twin()->head &&
                                    it_vertex_exposed2 != cluster_to_be_inserted_or_cluster_affected->arc_forward.prev()->twin()->head) // rake target
                                {
                                    cluster_rake_source1->is_marked() = false;

                                    if_not_null_and_not_list_node_and_not_marked_then_mark_and_add_to_clusters_to_be_erased_new(cluster_to_be_inserted_or_cluster_affected->parent);
                                    if_not_null_and_list_node_delete_list_node(cluster_to_be_inserted_or_cluster_affected->parent);
                                    if_not_null_and_not_list_node_and_not_marked_then_mark_and_add_to_clusters_to_be_erased_new(cluster_rake_source1->parent);
                                    if_not_null_and_list_node_delete_list_node(cluster_rake_source1->parent);

                                    cluster_tree_node_t *parent = new cluster_tree_node_t(cluster_to_be_inserted_or_cluster_affected->arc_backward.head, cluster_to_be_inserted_or_cluster_affected->arc_forward.head);
                                    parent->is_marked() = false;
                                    parent->contraction_type() = false;
                                    parent->child_left() = cluster_rake_source1;
                                    cluster_rake_source1->parent = parent;
                                    parent->child_right() = cluster_to_be_inserted_or_cluster_affected;
                                    cluster_to_be_inserted_or_cluster_affected->parent = parent;
                                    clusters_to_be_inserted_new.push_back(parent);
                                }
                                else if(cluster_tree_node_t *cluster_rake_source2 = cluster_to_be_inserted_or_cluster_affected->arc_backward.prev()->cluster_tree_node();
                                        cluster_rake_source2 != cluster_to_be_inserted_or_cluster_affected &&
                                        cluster_to_be_inserted_or_cluster_affected->arc_backward.prev()->twin() == cluster_to_be_inserted_or_cluster_affected->arc_backward.prev()->prev() &&
                                        cluster_is_free(cluster_rake_source2) &&
                                        it_vertex_exposed1 != cluster_to_be_inserted_or_cluster_affected->arc_backward.prev()->twin()->head &&
                                        it_vertex_exposed2 != cluster_to_be_inserted_or_cluster_affected->arc_backward.prev()->twin()->head) // rake target
                                {
                                    cluster_rake_source2->is_marked() = false;

                                    if_not_null_and_not_list_node_and_not_marked_then_mark_and_add_to_clusters_to_be_erased_new(cluster_to_be_inserted_or_cluster_affected->parent);
                                    if_not_null_and_list_node_delete_list_node(cluster_to_be_inserted_or_cluster_affected->parent);
                                    if_not_null_and_not_list_node_and_not_marked_then_mark_and_add_to_clusters_to_be_erased_new(cluster_rake_source2->parent);
                                    if_not_null_and_list_node_delete_list_node(cluster_rake_source2->parent);

                                    cluster_tree_node_t *parent = new cluster_tree_node_t(cluster_to_be_inserted_or_cluster_affected->arc_backward.head, cluster_to_be_inserted_or_cluster_affected->arc_forward.head);
                                    parent->is_marked() = false;
                                    parent->contraction_type() = false;
                                    parent->child_left() = cluster_rake_source2;
                                    cluster_rake_source2->parent = parent;
                                    parent->child_right() = cluster_to_be_inserted_or_cluster_affected;
                                    cluster_to_be_inserted_or_cluster_affected->parent = parent;
                                    clusters_to_be_inserted_new.push_back(parent);
                                }
                                else if(cluster_tree_node_t *cluster_rake_target1 = cluster_to_be_inserted_or_cluster_affected->arc_forward.next()->cluster_tree_node();
                                        cluster_rake_target1 != cluster_to_be_inserted_or_cluster_affected &&
                                        cluster_to_be_inserted_or_cluster_affected->arc_backward.next() == &cluster_to_be_inserted_or_cluster_affected->arc_forward &&
                                        cluster_is_free(cluster_rake_target1) &&
                                        it_vertex_exposed1 != cluster_to_be_inserted_or_cluster_affected->arc_backward.head &&
                                        it_vertex_exposed2 != cluster_to_be_inserted_or_cluster_affected->arc_backward.head) // rake source
                                {
                                    cluster_rake_target1->is_marked() = false;

                                    if_not_null_and_not_list_node_and_not_marked_then_mark_and_add_to_clusters_to_be_erased_new(cluster_to_be_inserted_or_cluster_affected->parent);
                                    if_not_null_and_list_node_delete_list_node(cluster_to_be_inserted_or_cluster_affected->parent);
                                    if_not_null_and_not_list_node_and_not_marked_then_mark_and_add_to_clusters_to_be_erased_new(cluster_rake_target1->parent);
                                    if_not_null_and_list_node_delete_list_node(cluster_rake_target1->parent);

                                    cluster_tree_node_t *parent = new cluster_tree_node_t(cluster_rake_target1->arc_backward.head, cluster_rake_target1->arc_forward.head);
                                    parent->is_marked() = false;
                                    parent->contraction_type() = false;
                                    parent->child_left() = cluster_to_be_inserted_or_cluster_affected;
                                    cluster_to_be_inserted_or_cluster_affected->parent = parent;
                                    parent->child_right() = cluster_rake_target1;
                                    cluster_rake_target1->parent = parent;
                                    clusters_to_be_inserted_new.push_back(parent);
                                }
                                else if(cluster_tree_node_t *cluster_rake_target2 = cluster_to_be_inserted_or_cluster_affected->arc_backward.next()->cluster_tree_node();
                                        cluster_rake_target2 != cluster_to_be_inserted_or_cluster_affected &&
                                        cluster_to_be_inserted_or_cluster_affected->arc_forward.next() == &cluster_to_be_inserted_or_cluster_affected->arc_backward &&
                                        cluster_is_free(cluster_rake_target2) &&
                                        it_vertex_exposed1 != cluster_to_be_inserted_or_cluster_affected->arc_forward.head &&
                                        it_vertex_exposed2 != cluster_to_be_inserted_or_cluster_affected->arc_forward.head) // rake source
                                {
                                    cluster_rake_target2->is_marked() = false;

                                    if_not_null_and_not_list_node_and_not_marked_then_mark_and_add_to_clusters_to_be_erased_new(cluster_to_be_inserted_or_cluster_affected->parent);
                                    if_not_null_and_list_node_delete_list_node(cluster_to_be_inserted_or_cluster_affected->parent);
                                    if_not_null_and_not_list_node_and_not_marked_then_mark_and_add_to_clusters_to_be_erased_new(cluster_rake_target2->parent);
                                    if_not_null_and_list_node_delete_list_node(cluster_rake_target2->parent);

                                    cluster_tree_node_t *parent = new cluster_tree_node_t(cluster_rake_target2->arc_backward.head, cluster_rake_target2->arc_forward.head);
                                    parent->is_marked() = false;
                                    parent->contraction_type() = false;
                                    parent->child_left() = cluster_to_be_inserted_or_cluster_affected;
                                    cluster_to_be_inserted_or_cluster_affected->parent = parent;
                                    parent->child_right() = cluster_rake_target2;
                                    cluster_rake_target2->parent = parent;
                                    clusters_to_be_inserted_new.push_back(parent);
                                }
                                else if(cluster_tree_node_t *cluster_other1 = cluster_to_be_inserted_or_cluster_affected->arc_backward.next()->cluster_tree_node();
                                        cluster_other1 != cluster_to_be_inserted_or_cluster_affected &&
                                        cluster_to_be_inserted_or_cluster_affected->arc_backward.next()->twin() == cluster_to_be_inserted_or_cluster_affected->arc_forward.prev() &&
                                        cluster_is_free(cluster_other1) &&
                                        it_vertex_exposed1 != cluster_to_be_inserted_or_cluster_affected->arc_backward.head &&
                                        it_vertex_exposed2 != cluster_to_be_inserted_or_cluster_affected->arc_backward.head) // compress
                                {
                                    cluster_other1->is_marked() = false;

                                    if_not_null_and_not_list_node_and_not_marked_then_mark_and_add_to_clusters_to_be_erased_new(cluster_to_be_inserted_or_cluster_affected->parent);
                                    if_not_null_and_list_node_delete_list_node(cluster_to_be_inserted_or_cluster_affected->parent);
                                    if_not_null_and_not_list_node_and_not_marked_then_mark_and_add_to_clusters_to_be_erased_new(cluster_other1->parent);
                                    if_not_null_and_list_node_delete_list_node(cluster_other1->parent);

                                    it_vertex_t it_vertex_this = cluster_to_be_inserted_or_cluster_affected->arc_forward.head;
                                    it_vertex_t it_vertex_other = cluster_to_be_inserted_or_cluster_affected->arc_backward.next()->head;
                                    it_vertex_t it_vertex1, it_vertex2;
                                    cluster_tree_node_t *cluster_vertex1, *cluster_vertex2;
                                    if(std::as_const(graph()->comparator_it_vertex_address)(it_vertex_this, it_vertex_other))
                                    {
                                        std::tie(it_vertex1, it_vertex2) = std::make_tuple(it_vertex_this, it_vertex_other);
                                        std::tie(cluster_vertex1, cluster_vertex2) = std::make_tuple(cluster_to_be_inserted_or_cluster_affected, cluster_other1);
                                    }
                                    else if(std::as_const(graph()->comparator_it_vertex_address)(it_vertex_other, it_vertex_this))
                                    {
                                        std::tie(it_vertex1, it_vertex2) = std::make_tuple(it_vertex_other, it_vertex_this);
                                        std::tie(cluster_vertex1, cluster_vertex2) = std::make_tuple(cluster_other1, cluster_to_be_inserted_or_cluster_affected);
                                    }
                                    else std::unreachable();
                                    cluster_tree_node_t *parent = new cluster_tree_node_t(it_vertex1, it_vertex2);
                                    parent->is_marked() = false;
                                    parent->contraction_type() = true;
                                    parent->child_left() = cluster_vertex1;
                                    cluster_vertex1->parent = parent;
                                    parent->child_right() = cluster_vertex2;
                                    cluster_vertex2->parent = parent;
                                    clusters_to_be_inserted_new.push_back(parent);
                                }
                                else if(cluster_tree_node_t *cluster_other2 = cluster_to_be_inserted_or_cluster_affected->arc_forward.next()->cluster_tree_node();
                                        cluster_other2 != cluster_to_be_inserted_or_cluster_affected &&
                                        cluster_to_be_inserted_or_cluster_affected->arc_forward.next()->twin() == cluster_to_be_inserted_or_cluster_affected->arc_backward.prev() &&
                                        cluster_is_free(cluster_other2) &&
                                        it_vertex_exposed1 != cluster_to_be_inserted_or_cluster_affected->arc_forward.head &&
                                        it_vertex_exposed2 != cluster_to_be_inserted_or_cluster_affected->arc_forward.head) // compress
                                {
                                    cluster_other2->is_marked() = false;

                                    if_not_null_and_not_list_node_and_not_marked_then_mark_and_add_to_clusters_to_be_erased_new(cluster_to_be_inserted_or_cluster_affected->parent);
                                    if_not_null_and_list_node_delete_list_node(cluster_to_be_inserted_or_cluster_affected->parent);
                                    if_not_null_and_not_list_node_and_not_marked_then_mark_and_add_to_clusters_to_be_erased_new(cluster_other2->parent);
                                    if_not_null_and_list_node_delete_list_node(cluster_other2->parent);

                                    it_vertex_t it_vertex_this = cluster_to_be_inserted_or_cluster_affected->arc_backward.head;
                                    it_vertex_t it_vertex_other = cluster_to_be_inserted_or_cluster_affected->arc_forward.next()->head;
                                    it_vertex_t it_vertex1, it_vertex2;
                                    cluster_tree_node_t *cluster_vertex1, *cluster_vertex2;
                                    if(std::as_const(graph()->comparator_it_vertex_address)(it_vertex_this, it_vertex_other))
                                    {
                                        std::tie(it_vertex1, it_vertex2) = std::make_tuple(it_vertex_this, it_vertex_other);
                                        std::tie(cluster_vertex1, cluster_vertex2) = std::make_tuple(cluster_to_be_inserted_or_cluster_affected, cluster_other2);
                                    }
                                    else if(std::as_const(graph()->comparator_it_vertex_address)(it_vertex_other, it_vertex_this))
                                    {
                                        std::tie(it_vertex1, it_vertex2) = std::make_tuple(it_vertex_other, it_vertex_this);
                                        std::tie(cluster_vertex1, cluster_vertex2) = std::make_tuple(cluster_other2, cluster_to_be_inserted_or_cluster_affected);
                                    }
                                    else std::unreachable();
                                    cluster_tree_node_t *parent = new cluster_tree_node_t(it_vertex1, it_vertex2);
                                    parent->is_marked() = false;
                                    parent->contraction_type() = true;
                                    parent->child_left() = cluster_vertex1;
                                    cluster_vertex1->parent = parent;
                                    parent->child_right() = cluster_vertex2;
                                    cluster_vertex2->parent = parent;
                                    clusters_to_be_inserted_new.push_back(parent);
                                }
                                else if(cluster_to_be_inserted_or_cluster_affected->arc_backward.next() == &cluster_to_be_inserted_or_cluster_affected->arc_forward &&
                                    cluster_to_be_inserted_or_cluster_affected->arc_forward.next() == &cluster_to_be_inserted_or_cluster_affected->arc_backward) // no match, is root
                                {
                                    if_not_null_and_not_list_node_and_not_marked_then_mark_and_add_to_clusters_to_be_erased_new(cluster_to_be_inserted_or_cluster_affected->parent);
                                    if_not_null_and_list_node_delete_list_node(cluster_to_be_inserted_or_cluster_affected->parent);

                                    cluster_list_node_t *cluster_list_node = new cluster_list_node_t();
                                    cluster_list_node_navigator_t::push_impl(cluster_list_node_end, cluster_list_node);
                                    ++tagged_ptr_bit0_unsetted(cluster_list_node_end)->node_count;
                                    cluster_list_node->tree_root = cluster_to_be_inserted_or_cluster_affected;
                                    cluster_to_be_inserted_or_cluster_affected->parent = tagged_ptr_bit0_setted(reinterpret_cast<cluster_tree_node_t *>(cluster_list_node));
                                }
                                else // no match, is not root
                                {
                                    if_not_null_and_not_list_node_and_not_marked_then_mark_and_add_to_clusters_to_be_erased_new(cluster_to_be_inserted_or_cluster_affected->parent);
                                    if_not_null_and_list_node_delete_list_node(cluster_to_be_inserted_or_cluster_affected->parent);

                                    cluster_tree_node_t *parent = new cluster_tree_node_t(cluster_to_be_inserted_or_cluster_affected->arc_backward.head, cluster_to_be_inserted_or_cluster_affected->arc_forward.head);
                                    parent->is_marked() = false;
                                    parent->contraction_type() = false;
                                    parent->child_left() = cluster_to_be_inserted_or_cluster_affected;
                                    cluster_to_be_inserted_or_cluster_affected->parent = parent;
                                    parent->child_right() = nullptr;
                                    clusters_to_be_inserted_new.push_back(parent);
                                }
                            }
                        }
                    };
                    for(cluster_tree_node_t *cluster_to_be_inserted : clusters_to_be_inserted)
                        new_move(cluster_to_be_inserted);
                    for(cluster_tree_node_t *cluster_affected : clusters_affected)
                        new_move(cluster_affected);

                    auto split_clusters_to_be_erased_and_join_clusters_to_be_inserted_current_level = [&]()
                    {
                        if(current_level == 0)
                        {
                            for(cluster_tree_node_t *cluster_to_be_erased : clusters_to_be_erased)
                            {
                                if constexpr(!std::is_same_v<cluster_t, void>)
                                    std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).destroy(graph(), cluster_to_be_erased->arc_backward.head, cluster_to_be_erased->p_cluster(), cluster_to_be_erased->arc_forward.head);
                                delete cluster_to_be_erased;
                            }
                        }
                        else
                        {
                            for(cluster_tree_node_t *cluster_to_be_erased : clusters_to_be_erased)
                            {
                                if constexpr(!std::is_same_v<cluster_t, void>)
                                {
                                    it_vertex_t it_vertex1, it_vertex2, it_vertex3;
                                    if(cluster_to_be_erased->child_left() != nullptr && cluster_to_be_erased->child_right() == nullptr)
                                        std::tie(it_vertex1, it_vertex2) = cluster_to_be_erased->dummy_it_vertexes();
                                    else if(cluster_to_be_erased->child_left() != nullptr && cluster_to_be_erased->child_right() != nullptr)
                                    {
                                        if(!cluster_to_be_erased->contraction_type())
                                            std::tie(it_vertex1, it_vertex2, it_vertex3) = cluster_to_be_erased->rake_it_vertexes();
                                        else
                                            std::tie(it_vertex1, it_vertex2, it_vertex3) = cluster_to_be_erased->compress_it_vertexes();
                                    }
                                    else std::unreachable();
                                    std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).split(graph(), cluster_to_be_erased->p_cluster(), it_vertex1, (assert(cluster_to_be_erased->child_left() != nullptr), cluster_to_be_erased->child_left()->p_cluster()), it_vertex2, cluster_to_be_erased->child_right() == nullptr ? nullptr : cluster_to_be_erased->child_right()->p_cluster(), it_vertex3, cluster_to_be_erased->dummy_or_rake_or_compress());
                                }
                                delete cluster_to_be_erased;
                            }
                        }

                        split_clusters_to_be_erased_and_join_clusters_to_be_inserted();

                        if(current_level == 0)
                        {
                            std::size_t cluster_to_be_inserted_index = 0;
                            for(cluster_tree_node_t *cluster_to_be_inserted : clusters_to_be_inserted)
                            {
                                if constexpr(!std::is_same_v<cluster_t, void>)
                                    std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).create(graph(), cluster_to_be_inserted->arc_backward.head, cluster_to_be_inserted->p_cluster(), cluster_to_be_inserted->arc_forward.head, it_edges_to_be_inserted[cluster_to_be_inserted_index]);
                                ++cluster_to_be_inserted_index;
                            }
                        }
                        else
                        {
                            for(cluster_tree_node_t *cluster_to_be_inserted : clusters_to_be_inserted)
                            {
                                if constexpr(!std::is_same_v<cluster_t, void>)
                                {
                                    it_vertex_t it_vertex1, it_vertex2, it_vertex3;
                                    if(cluster_to_be_inserted->child_left() != nullptr && cluster_to_be_inserted->child_right() == nullptr)
                                        std::tie(it_vertex1, it_vertex2) = cluster_to_be_inserted->dummy_it_vertexes();
                                    else if(cluster_to_be_inserted->child_left() != nullptr && cluster_to_be_inserted->child_right() != nullptr)
                                    {
                                        if(!cluster_to_be_inserted->contraction_type())
                                            std::tie(it_vertex1, it_vertex2, it_vertex3) = cluster_to_be_inserted->rake_it_vertexes();
                                        else
                                            std::tie(it_vertex1, it_vertex2, it_vertex3) = cluster_to_be_inserted->compress_it_vertexes();
                                    }
                                    else std::unreachable();
                                    std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).join(graph(), cluster_to_be_inserted->p_cluster(), it_vertex1, cluster_to_be_inserted->child_left()->p_cluster(), it_vertex2, cluster_to_be_inserted->child_right()->p_cluster(), it_vertex3, cluster_to_be_inserted->dummy_or_rake_or_compress());
                                }
                            }
                        }
                    };

                    if(!(clusters_to_be_erased_new.empty() && clusters_to_be_inserted_new.empty()))
                        update(current_level + 1, clusters_to_be_erased_new, {}, clusters_to_be_inserted_new, {}, split_clusters_to_be_erased_and_join_clusters_to_be_inserted_current_level);
                    else
                        split_clusters_to_be_erased_and_join_clusters_to_be_inserted_current_level();
                }
                void unexpose_impl(it_vertex_t it_vertex)
                {
                    if(it_vertexes_trivial_component.find(it_vertex) != it_vertexes_trivial_component.end())
                        ;
                    else
                    {
                        auto cluster_is_free = [](cluster_tree_node_t *cluster_tree_node)
                        {
                            assert(cluster_tree_node->parent != nullptr);
                            assert(!tagged_ptr_bit0_is_setted(cluster_tree_node->parent));
                            assert(!cluster_tree_node->parent->is_marked());
                            return cluster_tree_node->parent->child_left() != nullptr && cluster_tree_node->parent->child_right() == nullptr;
                        };
                        cluster_tree_node_t *cluster_tree_node = (*it_vertexes_it_vertexes_arc_and_it_edge_it_vertexes_base_level.at(it_vertex).begin()).second.first->cluster_tree_node();
                        std::size_t current_level = 0;
                        while(true)
                        {
                            if(tagged_ptr_bit0_is_setted(cluster_tree_node->parent))
                                break;
                            else
                            {
                                if(cluster_is_free(cluster_tree_node))
                                {
                                    if(cluster_tree_node_t *cluster_rake_source = cluster_tree_node->arc_forward.prev()->cluster_tree_node();
                                        cluster_rake_source != cluster_tree_node &&
                                        cluster_tree_node->arc_forward.prev()->twin() == cluster_tree_node->arc_forward.prev()->prev() &&
                                        cluster_is_free(cluster_rake_source) &&
                                        it_vertex_exposed1 != cluster_tree_node->arc_forward.prev()->twin()->head &&
                                        it_vertex_exposed2 != cluster_tree_node->arc_forward.prev()->twin()->head) // rake target
                                    {
                                        cluster_tree_node->is_marked() = true;
                                        update(current_level, {}, {cluster_tree_node}, {}, {}, []() {});
                                        break;
                                    }
                                    else if(cluster_tree_node_t *cluster_rake_source2 = cluster_tree_node->arc_backward.prev()->cluster_tree_node();
                                            cluster_rake_source2 != cluster_tree_node &&
                                            cluster_tree_node->arc_backward.prev()->twin() == cluster_tree_node->arc_backward.prev()->prev() &&
                                            cluster_is_free(cluster_rake_source2) &&
                                            it_vertex_exposed1 != cluster_tree_node->arc_backward.prev()->twin()->head &&
                                            it_vertex_exposed2 != cluster_tree_node->arc_backward.prev()->twin()->head) // rake target
                                    {
                                        cluster_tree_node->is_marked() = true;
                                        update(current_level, {}, {cluster_tree_node}, {}, {}, []() {});
                                        break;
                                    }
                                    else if(cluster_tree_node_t *cluster_rake_target = cluster_tree_node->arc_forward.next()->cluster_tree_node();
                                            cluster_rake_target != cluster_tree_node &&
                                            cluster_tree_node->arc_backward.next() == &cluster_tree_node->arc_forward &&
                                            cluster_is_free(cluster_rake_target) &&
                                            it_vertex_exposed1 != cluster_tree_node->arc_backward.head &&
                                            it_vertex_exposed2 != cluster_tree_node->arc_backward.head) // rake source
                                    {
                                        cluster_tree_node->is_marked() = true;
                                        update(current_level, {}, {cluster_tree_node}, {}, {}, []() {});
                                        break;
                                    }
                                    else if(cluster_tree_node_t *cluster_rake_target2 = cluster_tree_node->arc_backward.next()->cluster_tree_node();
                                            cluster_rake_target2 != cluster_tree_node &&
                                            cluster_tree_node->arc_forward.next() == &cluster_tree_node->arc_backward &&
                                            cluster_is_free(cluster_rake_target2) &&
                                            it_vertex_exposed1 != cluster_tree_node->arc_forward.head &&
                                            it_vertex_exposed2 != cluster_tree_node->arc_forward.head) // rake source
                                    {
                                        cluster_tree_node->is_marked() = true;
                                        update(current_level, {}, {cluster_tree_node}, {}, {}, []() {});
                                        break;
                                    }
                                    else if(cluster_tree_node_t *cluster_other = cluster_tree_node->arc_backward.next()->cluster_tree_node();
                                            cluster_other != cluster_tree_node &&
                                            cluster_tree_node->arc_backward.next()->twin() == cluster_tree_node->arc_forward.prev() &&
                                            cluster_is_free(cluster_other) &&
                                            it_vertex_exposed1 != cluster_tree_node->arc_backward.head &&
                                            it_vertex_exposed2 != cluster_tree_node->arc_backward.head) // compress
                                    {
                                        cluster_tree_node->is_marked() = true;
                                        update(current_level, {}, {cluster_tree_node}, {}, {}, []() {});
                                        break;
                                    }
                                    else if(cluster_tree_node_t *cluster_other2 = cluster_tree_node->arc_forward.next()->cluster_tree_node();
                                            cluster_other2 != cluster_tree_node &&
                                            cluster_tree_node->arc_forward.next()->twin() == cluster_tree_node->arc_backward.prev() &&
                                            cluster_is_free(cluster_other2) &&
                                            it_vertex_exposed1 != cluster_tree_node->arc_forward.head &&
                                            it_vertex_exposed2 != cluster_tree_node->arc_forward.head) // compress
                                    {
                                        cluster_tree_node->is_marked() = true;
                                        update(current_level, {}, {cluster_tree_node}, {}, {}, []() {});
                                        break;
                                    }
                                    else
                                    {
                                        cluster_tree_node = cluster_tree_node->parent;
                                        ++current_level;
                                    }
                                }
                                else
                                {
                                    cluster_tree_node = cluster_tree_node->parent;
                                    ++current_level;
                                }
                            }
                        }
                    }
                }
                void expose_impl(it_vertex_t it_vertex)
                {
                    assert(it_vertex != graph()->vertexes.end());

                    if(it_vertexes_trivial_component.find(it_vertex) != it_vertexes_trivial_component.end())
                        ;
                    else
                    {
                        cluster_tree_node_t *cluster_tree_node = (*it_vertexes_it_vertexes_arc_and_it_edge_it_vertexes_base_level.at(it_vertex).begin()).second.first->cluster_tree_node();
                        std::size_t current_level = 0;
                        while(true)
                        {
                            if(cluster_tree_node->arc_backward.head != it_vertex && cluster_tree_node->arc_forward.head != it_vertex)
                            {
                                assert(cluster_tree_node->child_left() != nullptr && cluster_tree_node->child_right() != nullptr);
                                cluster_tree_node->child_left()->is_marked() = true;
                                cluster_tree_node->child_right()->is_marked() = true;
                                update(current_level - 1, {}, {cluster_tree_node->child_left(), cluster_tree_node->child_right()}, {}, {}, []() {});
                                break;
                            }
                            else
                            {
                                if(tagged_ptr_bit0_is_setted(cluster_tree_node->parent))
                                    break;
                                else
                                {
                                    cluster_tree_node = cluster_tree_node->parent;
                                    ++current_level;
                                }
                            }
                        }
                    }
                }

                void unexpose()
                {
                    if(it_vertex_exposed1.has_value())
                    {
                        it_vertex_t it_vertex_exposed1_value = it_vertex_exposed1.value();
                        it_vertex_exposed1.reset();
                        unexpose_impl(it_vertex_exposed1_value);
                    }
                    if(it_vertex_exposed2.has_value())
                    {
                        it_vertex_t it_vertex_exposed2_value = it_vertex_exposed2.value();
                        it_vertex_exposed2.reset();
                        unexpose_impl(it_vertex_exposed2_value);
                    }
                }
                void unexpose(it_vertex_t it_vertex)
                {
                    if(it_vertex_exposed1 == it_vertex)
                    {
                        it_vertex_t it_vertex_exposed1_value = it_vertex_exposed1.value();
                        it_vertex_exposed1.reset();
                        unexpose_impl(it_vertex_exposed1_value);
                    }
                    else if(it_vertex_exposed2 == it_vertex)
                    {
                        it_vertex_t it_vertex_exposed2_value = it_vertex_exposed2.value();
                        it_vertex_exposed2.reset();
                        unexpose_impl(it_vertex_exposed2_value);
                    }
                    else std::unreachable();
                }
                void unexpose_relaxed(it_vertex_t it_vertex)
                {
                    if(it_vertex_exposed1 == it_vertex)
                    {
                        it_vertex_t it_vertex_exposed1_value = it_vertex_exposed1.value();
                        it_vertex_exposed1.reset();
                        unexpose_impl(it_vertex_exposed1_value);
                    }
                    else if(it_vertex_exposed2 == it_vertex)
                    {
                        it_vertex_t it_vertex_exposed2_value = it_vertex_exposed2.value();
                        it_vertex_exposed2.reset();
                        unexpose_impl(it_vertex_exposed2_value);
                    }
                }
                void expose(it_vertex_t it_vertex1)
                {
                    if(it_vertex_exposed1 == it_vertex1)
                        return;
                    if(it_vertex_exposed2 == it_vertex1)
                        return;
                    if(!it_vertex_exposed1.has_value())
                        it_vertex_exposed1.emplace(it_vertex1);
                    else if(!it_vertex_exposed2.has_value())
                        it_vertex_exposed2.emplace(it_vertex1);
                    else std::unreachable();
                    expose_impl(it_vertex1);
                }
                void expose(it_vertex_t it_vertex1, it_vertex_t it_vertex2)
                {
                    expose(it_vertex1);
                    expose(it_vertex2);
                }
                void expose_replace(it_vertex_t it_vertex1)
                {
                    if(it_vertex_exposed1.has_value() && it_vertex_exposed1.value() != it_vertex1)
                    {
                        it_vertex_t it_vertex_exposed1_value = it_vertex_exposed1.value();
                        it_vertex_exposed1.reset();
                        unexpose_impl(it_vertex_exposed1_value);
                    }
                    if(it_vertex_exposed2.has_value() && it_vertex_exposed2.value() != it_vertex1)
                    {
                        it_vertex_t it_vertex_exposed2_value = it_vertex_exposed2.value();
                        it_vertex_exposed2.reset();
                        unexpose_impl(it_vertex_exposed2_value);
                    }
                    expose(it_vertex1);
                }
                void expose_replace(it_vertex_t it_vertex1, it_vertex_t it_vertex2)
                {
                    if(it_vertex_exposed1.has_value() && it_vertex_exposed1.value() != it_vertex1 && it_vertex_exposed1.value() != it_vertex2)
                    {
                        it_vertex_t it_vertex_exposed1_value = it_vertex_exposed1.value();
                        it_vertex_exposed1.reset();
                        unexpose_impl(it_vertex_exposed1_value);
                    }
                    if(it_vertex_exposed2.has_value() && it_vertex_exposed2.value() != it_vertex1 && it_vertex_exposed2.value() != it_vertex2)
                    {
                        it_vertex_t it_vertex_exposed2_value = it_vertex_exposed2.value();
                        it_vertex_exposed2.reset();
                        unexpose_impl(it_vertex_exposed2_value);
                    }
                    expose(it_vertex1, it_vertex2);
                }

                void link(it_edge_it_vertexes_t it_edge)
                {
                    auto link_impl = [&]()
                    {
                        cluster_tree_node_t *parent = new cluster_tree_node_t(it_edge->second.first, it_edge->second.second);
                        parent->is_marked() = false;
                        parent->contraction_type() = false;
                        parent->child_left() = nullptr;
                        parent->child_right() = nullptr;
                        update(0, {}, {}, {parent}, {it_edge}, []() {});
                    };
                    if(it_edge->second.first == it_edge->second.second)
                        it_edges_fundamental.insert(it_edge);
                    else if(belongs_to_same_component(it_edge->second.first, it_edge->second.second))
                    {
                        if constexpr(std::is_same_v<cluster_t, void>)
                            it_edges_fundamental.insert(it_edge); // no cluster_t to be compared
                        else
                        {
                            if constexpr(!requires(cluster_tree_node_t * root) { std::optional<it_edge_it_vertexes_t>(std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).edge_to_be_replaced(graph(), std::as_const(*root->p_cluster()), it_edge)); })
                                it_edges_fundamental.insert(it_edge); // internal_operations_t does not have ability to compare cluster_t
                            else
                            {
                                expose_replace(it_edge->second.first, it_edge->second.second);
                                cluster_tree_node_t *root_cluster = (*it_vertexes_it_vertexes_arc_and_it_edge_it_vertexes_base_level.at(it_edge->second.first).begin()).second.first->cluster_tree_node();
                                while(!tagged_ptr_bit0_is_setted(root_cluster->parent))
                                    root_cluster = root_cluster->parent;
                                std::optional<it_edge_it_vertexes_t> edge_to_be_replaced(std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).edge_to_be_replaced(graph(), std::as_const(*root_cluster->p_cluster()), it_edge));
                                if(!edge_to_be_replaced.has_value())
                                    it_edges_fundamental.insert(it_edge);
                                else
                                {
                                    it_edges_fundamental.insert(edge_to_be_replaced.value());

                                    cluster_tree_node_t *edge_to_be_replaced_cluster_base_level = it_vertexes_it_vertexes_arc_and_it_edge_it_vertexes_base_level.at(edge_to_be_replaced.value()->second.first).at(edge_to_be_replaced.value()->second.second).first->cluster_tree_node();
                                    edge_to_be_replaced_cluster_base_level->is_marked() = true;
                                    update(0, {edge_to_be_replaced_cluster_base_level}, {}, {}, {}, []() {});

                                    link_impl();
                                }
                            }
                        }
                    }
                    else
                        link_impl();
                }
                void cut(it_edge_it_vertexes_t it_edge)
                {
                    auto it_edge_not_in_forest = it_edges_fundamental.find(it_edge);
                    if(it_edge_not_in_forest != it_edges_fundamental.end())
                        it_edges_fundamental.erase(it_edge_not_in_forest);
                    else
                    {
                        cluster_tree_node_t *cluster_tree_node = it_vertexes_it_vertexes_arc_and_it_edge_it_vertexes_base_level.at(it_edge->second.first).at(it_edge->second.second).first->cluster_tree_node();
                        cluster_tree_node->is_marked() = true;
                        update(0, {cluster_tree_node}, {}, {}, {}, []() {});

                        if(auto it_it_edge_fundamental = std::ranges::find_if(it_edges_fundamental, [&](it_edge_it_vertexes_t const &it_edge_fundamental)
                               { return !belongs_to_same_component(it_edge_fundamental->second.first, it_edge_fundamental->second.second); });
                            it_it_edge_fundamental != it_edges_fundamental.end())
                        {
                            it_edge_it_vertexes_t it_edge_fundamental = *it_it_edge_fundamental;
                            it_edges_fundamental.erase(it_it_edge_fundamental);

                            cluster_tree_node_t *parent = new cluster_tree_node_t(it_edge_fundamental->second.first, it_edge_fundamental->second.second);
                            parent->is_marked() = false;
                            parent->contraction_type() = false;
                            parent->child_left() = nullptr;
                            parent->child_right() = nullptr;
                            update(0, {}, {}, {parent}, {it_edge_fundamental}, []() {});
                        }
                    }
                }

                std::optional<cluster_tree_node_t *> root_cluster_tree_node(it_vertex_t it_vertex)
                {
                    if(it_vertexes_trivial_component.find(it_vertex) != it_vertexes_trivial_component.end())
                        return std::nullopt;
                    else
                    {
                        cluster_tree_node_t *root_cluster_tree_node = (*it_vertexes_it_vertexes_arc_and_it_edge_it_vertexes_base_level.at(it_vertex).begin()).second.first->cluster_tree_node();
                        while(!tagged_ptr_bit0_is_setted(root_cluster_tree_node->parent))
                            root_cluster_tree_node = root_cluster_tree_node->parent;
                        return root_cluster_tree_node;
                    }
                }
                template<typename selector_t>
                std::optional<it_edge_it_vertexes_t> search(selector_t selector, cluster_tree_node_t *root_cluster_tree_node)
                {
                    assert(root_cluster_tree_node != nullptr);
                    auto non_local_search_impl = [this, &selector](auto &this_, cluster_tree_node_t *cluster_tree_node) -> std::optional<it_edge_it_vertexes_t>
                    {
                        if(cluster_tree_node->child_left() == nullptr && cluster_tree_node->child_right() == nullptr) // leaf
                            return it_vertexes_it_vertexes_arc_and_it_edge_it_vertexes_base_level.at(cluster_tree_node->arc_backward.head).at(cluster_tree_node->arc_forward.head).second;
                        else if(cluster_tree_node->child_left() != nullptr && cluster_tree_node->child_right() == nullptr) // dummy
                            return this_(this_, cluster_tree_node->child_left());
                        else if(cluster_tree_node->child_left() != nullptr && cluster_tree_node->child_right() != nullptr)
                        {
                            if(!cluster_tree_node->contraction_type()) // rake
                            {
                                it_vertex_t it_vertex1, it_vertex2, it_vertex3;
                                std::tie(it_vertex1, it_vertex2, it_vertex3) = cluster_tree_node->rake_it_vertexes();
                                std::optional<bool> selector_result(selector(graph(), cluster_tree_node->p_cluster(), it_vertex1, cluster_tree_node->child_left()->p_cluster(), it_vertex2, cluster_tree_node->child_right()->p_cluster(), it_vertex3, 1));
                                if(!selector_result.has_value())
                                    return std::nullopt;
                                else
                                {
                                    if(!selector_result.value())
                                        return this_(this_, cluster_tree_node->child_left());
                                    else
                                        return this_(this_, cluster_tree_node->child_right());
                                }
                            }
                            else // compress
                            {
                                it_vertex_t it_vertex1, it_vertex2, it_vertex3;
                                std::tie(it_vertex1, it_vertex2, it_vertex3) = cluster_tree_node->compress_it_vertexes();
                                std::optional<bool> selector_result(selector(graph(), cluster_tree_node->p_cluster(), it_vertex1, cluster_tree_node->child_left()->p_cluster(), it_vertex2, cluster_tree_node->child_right()->p_cluster(), it_vertex3, 2));
                                if(!selector_result.has_value())
                                    return std::nullopt;
                                else
                                {
                                    if(!selector_result.value())
                                        return this_(this_, cluster_tree_node->child_left());
                                    else
                                        return this_(this_, cluster_tree_node->child_right());
                                }
                            }
                        }
                        else std::unreachable();
                    };
                    return non_local_search_impl(non_local_search_impl, root_cluster_tree_node);
                }
                template<typename selector_t>
                std::optional<it_edge_it_vertexes_t> non_local_search(selector_t selector, cluster_tree_node_t *root_cluster_tree_node)
                {
                    assert(root_cluster_tree_node != nullptr);
                    auto non_local_search_impl = [this, &selector](auto &this_, cluster_tree_node_t *cluster_tree_node_external1, cluster_tree_node_t *cluster_tree_node, cluster_tree_node_t *cluster_tree_node_external2) -> std::optional<it_edge_it_vertexes_t>
                    {
                        if(cluster_tree_node->child_left() == nullptr && cluster_tree_node->child_right() == nullptr) // leaf
                            return it_vertexes_it_vertexes_arc_and_it_edge_it_vertexes_base_level.at(cluster_tree_node->arc_backward.head).at(cluster_tree_node->arc_forward.head).second;
                        else if(cluster_tree_node->child_left() != nullptr && cluster_tree_node->child_right() == nullptr) // dummy
                        {
                            it_vertex_t it_vertex1, it_vertex2, it_vertex3;
                            std::tie(it_vertex1, it_vertex2) = cluster_tree_node->dummy_it_vertexes();
                            std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).split(graph(), cluster_tree_node->p_cluster(), it_vertex1, cluster_tree_node->child_left()->p_cluster(), it_vertex2, nullptr, it_vertex3, 0);
                            std::optional<it_edge_it_vertexes_t> result(this_(this_, cluster_tree_node_external1, cluster_tree_node->child_left(), cluster_tree_node_external2));
                            std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).join(graph(), cluster_tree_node->p_cluster(), it_vertex1, cluster_tree_node->child_left()->p_cluster(), it_vertex2, nullptr, it_vertex3, 0);
                            return result;
                        }
                        else if(cluster_tree_node->child_left() != nullptr && cluster_tree_node->child_right() != nullptr)
                        {
                            if(!cluster_tree_node->contraction_type()) // rake
                            {
                                it_vertex_t it_vertex1, it_vertex2, it_vertex3;
                                std::tie(it_vertex1, it_vertex2, it_vertex3) = cluster_tree_node->rake_it_vertexes();
                                cluster_tree_node_t *p_cluster_tree_node_mixed_rake_center, cluster_tree_node_mixed_rake_center(nullptr), *p_cluster_tree_node_mixed_rake_target, cluster_tree_node_mixed_rake_target(nullptr), *p_cluster_tree_node_fake_root, cluster_tree_node_fake_root(nullptr);
                                it_vertex_t it_vertex1_mixed_rake_center, it_vertex2_mixed_rake_center, it_vertex3_mixed_rake_center;
                                it_vertex_t it_vertex1_mixed_rake_target, it_vertex2_mixed_rake_target, it_vertex3_mixed_rake_target;
                                it_vertex_t it_vertex1_fake_root, it_vertex2_fake_root, it_vertex3_fake_root;
                                cluster_tree_node_t *cluster_tree_node_external_rake_center, *cluster_tree_node_external_rake_target;
                                if(cluster_tree_node_external1 == nullptr && cluster_tree_node_external2 == nullptr)
                                    ;
                                else std::tie(cluster_tree_node_external_rake_center, cluster_tree_node_external_rake_target) = [&]()
                                {
                                    if(cluster_tree_node->child_left()->arc_backward.head == cluster_tree_node->arc_backward.head || cluster_tree_node->child_left()->arc_forward.head == cluster_tree_node->arc_backward.head)
                                        return std::make_tuple(cluster_tree_node_external1, cluster_tree_node_external2);
                                    else if(cluster_tree_node->child_left()->arc_backward.head == cluster_tree_node->arc_forward.head || cluster_tree_node->child_left()->arc_forward.head == cluster_tree_node->arc_forward.head)
                                        return std::make_tuple(cluster_tree_node_external2, cluster_tree_node_external1);
                                    else std::unreachable();
                                }();
                                if(cluster_tree_node_external1 == nullptr && cluster_tree_node_external2 == nullptr)
                                {
                                    p_cluster_tree_node_fake_root = cluster_tree_node;
                                    std::tie(it_vertex1_fake_root, it_vertex2_fake_root, it_vertex3_fake_root) = std::make_tuple(it_vertex1, it_vertex2, it_vertex3);
                                }
                                else
                                {
                                    std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).split(graph(), cluster_tree_node->p_cluster(), it_vertex1, cluster_tree_node->child_left()->p_cluster(), it_vertex2, cluster_tree_node->child_right()->p_cluster(), it_vertex3, 1);
                                    if(cluster_tree_node_external_rake_center == nullptr)
                                        p_cluster_tree_node_mixed_rake_center = cluster_tree_node->child_left();
                                    else
                                    {
                                        p_cluster_tree_node_mixed_rake_center = &cluster_tree_node_mixed_rake_center;
                                        std::tie(cluster_tree_node_mixed_rake_center.arc_backward.head, cluster_tree_node_mixed_rake_center.arc_forward.head) = std::make_tuple(cluster_tree_node->child_left()->arc_backward.head, cluster_tree_node->child_left()->arc_forward.head);
                                        cluster_tree_node_mixed_rake_center.contraction_type() = false;
                                        cluster_tree_node_mixed_rake_center.child_left() = cluster_tree_node_external_rake_center;
                                        cluster_tree_node_mixed_rake_center.child_right() = cluster_tree_node->child_left();
                                        std::tie(it_vertex1_mixed_rake_center, it_vertex2_mixed_rake_center, it_vertex3_mixed_rake_center) = cluster_tree_node_mixed_rake_center.rake_it_vertexes();
                                        std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).join(graph(), cluster_tree_node_mixed_rake_center.p_cluster(), it_vertex1_mixed_rake_center, cluster_tree_node_mixed_rake_center.child_left()->p_cluster(), it_vertex2_mixed_rake_center, cluster_tree_node_mixed_rake_center.child_right()->p_cluster(), it_vertex3_mixed_rake_center, 1);
                                    }
                                    if(cluster_tree_node_external_rake_target == nullptr)
                                        p_cluster_tree_node_mixed_rake_target = cluster_tree_node->child_right();
                                    else
                                    {
                                        p_cluster_tree_node_mixed_rake_target = &cluster_tree_node_mixed_rake_target;
                                        it_vertex_t it_vertex_external_rake_target = [&]()
                                        {
                                            if(it_vertex3 == cluster_tree_node_external_rake_target->arc_backward.head)
                                                return cluster_tree_node_external_rake_target->arc_forward.head;
                                            else if(it_vertex3 == cluster_tree_node_external_rake_target->arc_forward.head)
                                                return cluster_tree_node_external_rake_target->arc_backward.head;
                                            else std::unreachable();
                                        }();
                                        if(std::as_const(graph()->comparator_it_vertex_address)(it_vertex2, it_vertex_external_rake_target))
                                        {
                                            std::tie(cluster_tree_node_mixed_rake_target.arc_backward.head, cluster_tree_node_mixed_rake_target.arc_forward.head) = std::make_tuple(it_vertex2, it_vertex_external_rake_target);
                                            cluster_tree_node_mixed_rake_target.contraction_type() = true;
                                            cluster_tree_node_mixed_rake_target.child_left() = cluster_tree_node->child_right();
                                            cluster_tree_node_mixed_rake_target.child_right() = cluster_tree_node_external_rake_target;
                                        }
                                        else if(std::as_const(graph()->comparator_it_vertex_address)(it_vertex_external_rake_target, it_vertex2))
                                        {
                                            std::tie(cluster_tree_node_mixed_rake_target.arc_backward.head, cluster_tree_node_mixed_rake_target.arc_forward.head) = std::make_tuple(it_vertex_external_rake_target, it_vertex2);
                                            cluster_tree_node_mixed_rake_target.contraction_type() = true;
                                            cluster_tree_node_mixed_rake_target.child_left() = cluster_tree_node_external_rake_target;
                                            cluster_tree_node_mixed_rake_target.child_right() = cluster_tree_node->child_right();
                                        }
                                        else std::unreachable();
                                        std::tie(it_vertex1_mixed_rake_target, it_vertex2_mixed_rake_target, it_vertex3_mixed_rake_target) = cluster_tree_node_mixed_rake_target.compress_it_vertexes();
                                        std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).join(graph(), cluster_tree_node_mixed_rake_target.p_cluster(), it_vertex1_mixed_rake_target, cluster_tree_node_mixed_rake_target.child_left()->p_cluster(), it_vertex2_mixed_rake_target, cluster_tree_node_mixed_rake_target.child_right()->p_cluster(), it_vertex3_mixed_rake_target, 2);
                                    }
                                    p_cluster_tree_node_fake_root = &cluster_tree_node_fake_root;
                                    std::tie(cluster_tree_node_fake_root.arc_backward.head, cluster_tree_node_fake_root.arc_forward.head) = std::make_tuple(p_cluster_tree_node_mixed_rake_target->arc_backward.head, p_cluster_tree_node_mixed_rake_target->arc_forward.head);
                                    cluster_tree_node_fake_root.contraction_type() = false;
                                    cluster_tree_node_fake_root.child_left() = p_cluster_tree_node_mixed_rake_center;
                                    cluster_tree_node_fake_root.child_right() = p_cluster_tree_node_mixed_rake_target;
                                    std::tie(it_vertex1_fake_root, it_vertex2_fake_root, it_vertex3_fake_root) = cluster_tree_node_fake_root.rake_it_vertexes();
                                    std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).join(graph(), cluster_tree_node_fake_root.p_cluster(), it_vertex1_fake_root, cluster_tree_node_fake_root.child_left()->p_cluster(), it_vertex2_fake_root, cluster_tree_node_fake_root.child_right()->p_cluster(), it_vertex3_fake_root, 1);
                                }

                                std::optional<bool> selector_result(selector(graph(), p_cluster_tree_node_fake_root->p_cluster(), it_vertex1_fake_root, p_cluster_tree_node_fake_root->child_left()->p_cluster(), it_vertex2_fake_root, p_cluster_tree_node_fake_root->child_right()->p_cluster(), it_vertex3_fake_root, 1));
                                if(!selector_result.has_value())
                                {
                                    if(cluster_tree_node_external1 == nullptr && cluster_tree_node_external2 == nullptr)
                                        assert(p_cluster_tree_node_fake_root == cluster_tree_node);
                                    else
                                    {
                                        assert(p_cluster_tree_node_fake_root == &cluster_tree_node_fake_root);
                                        std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).split(graph(), cluster_tree_node_fake_root.p_cluster(), it_vertex1_fake_root, cluster_tree_node_fake_root.child_left()->p_cluster(), it_vertex2_fake_root, cluster_tree_node_fake_root.child_right()->p_cluster(), it_vertex3_fake_root, 1);
                                        if(cluster_tree_node_external_rake_center == nullptr)
                                            assert(p_cluster_tree_node_mixed_rake_center == cluster_tree_node->child_left());
                                        else
                                        {
                                            assert(p_cluster_tree_node_mixed_rake_center == &cluster_tree_node_mixed_rake_center);
                                            std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).split(graph(), cluster_tree_node_mixed_rake_center.p_cluster(), it_vertex1_mixed_rake_center, cluster_tree_node_mixed_rake_center.child_left()->p_cluster(), it_vertex2_mixed_rake_center, cluster_tree_node_mixed_rake_center.child_right()->p_cluster(), it_vertex3_mixed_rake_center, 1);
                                        }
                                        if(cluster_tree_node_external_rake_target == nullptr)
                                            assert(p_cluster_tree_node_mixed_rake_target == cluster_tree_node->child_right());
                                        else
                                        {
                                            assert(p_cluster_tree_node_mixed_rake_target == &cluster_tree_node_mixed_rake_target);
                                            std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).split(graph(), cluster_tree_node_mixed_rake_target.p_cluster(), it_vertex1_mixed_rake_target, cluster_tree_node_mixed_rake_target.child_left()->p_cluster(), it_vertex2_mixed_rake_target, cluster_tree_node_mixed_rake_target.child_right()->p_cluster(), it_vertex3_mixed_rake_target, 2);
                                        }
                                        std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).join(graph(), cluster_tree_node->p_cluster(), it_vertex1, cluster_tree_node->child_left()->p_cluster(), it_vertex2, cluster_tree_node->child_right()->p_cluster(), it_vertex3, 1);
                                    }
                                    return std::nullopt;
                                }
                                else
                                {
                                    if(!selector_result.value())
                                    {
                                        if(cluster_tree_node_external1 == nullptr && cluster_tree_node_external2 == nullptr)
                                        {
                                            assert(p_cluster_tree_node_fake_root == cluster_tree_node);
                                            std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).split(graph(), cluster_tree_node->p_cluster(), it_vertex1, cluster_tree_node->child_left()->p_cluster(), it_vertex2, cluster_tree_node->child_right()->p_cluster(), it_vertex3, 1);
                                            cluster_tree_node_t *cluster_tree_node_external1_next, *cluster_tree_node_external2_next;
                                            if(cluster_tree_node->child_left()->arc_backward.head == cluster_tree_node->arc_backward.head || cluster_tree_node->child_left()->arc_backward.head == cluster_tree_node->arc_forward.head)
                                                std::tie(cluster_tree_node_external1_next, cluster_tree_node_external2_next) = std::make_tuple(cluster_tree_node->child_right(), nullptr);
                                            else if(cluster_tree_node->child_left()->arc_forward.head == cluster_tree_node->arc_backward.head || cluster_tree_node->child_left()->arc_forward.head == cluster_tree_node->arc_forward.head)
                                                std::tie(cluster_tree_node_external1_next, cluster_tree_node_external2_next) = std::make_tuple(nullptr, cluster_tree_node->child_right());
                                            else std::unreachable();
                                            std::optional<it_edge_it_vertexes_t> result(this_(this_, cluster_tree_node_external1_next, cluster_tree_node->child_left(), cluster_tree_node_external2_next));
                                            std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).join(graph(), cluster_tree_node->p_cluster(), it_vertex1, cluster_tree_node->child_left()->p_cluster(), it_vertex2, cluster_tree_node->child_right()->p_cluster(), it_vertex3, 1);
                                            return result;
                                        }
                                        else
                                        {
                                            assert(p_cluster_tree_node_fake_root == &cluster_tree_node_fake_root);
                                            std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).split(graph(), cluster_tree_node_fake_root.p_cluster(), it_vertex1_fake_root, cluster_tree_node_fake_root.child_left()->p_cluster(), it_vertex2_fake_root, cluster_tree_node_fake_root.child_right()->p_cluster(), it_vertex3_fake_root, 1);
                                            cluster_tree_node_t *p_cluster_tree_node_rake_center_next, cluster_tree_node_rake_center_next(nullptr);
                                            it_vertex_t it_vertex1_rake_center_next, it_vertex2_rake_center_next, it_vertex3_rake_center_next;
                                            if(cluster_tree_node_external_rake_center == nullptr)
                                            {
                                                assert(p_cluster_tree_node_mixed_rake_center == cluster_tree_node->child_left());

                                                p_cluster_tree_node_rake_center_next = p_cluster_tree_node_mixed_rake_target;
                                            }
                                            else
                                            {
                                                assert(p_cluster_tree_node_mixed_rake_center == &cluster_tree_node_mixed_rake_center);
                                                std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).split(graph(), cluster_tree_node_mixed_rake_center.p_cluster(), it_vertex1_mixed_rake_center, cluster_tree_node_mixed_rake_center.child_left()->p_cluster(), it_vertex2_mixed_rake_center, cluster_tree_node_mixed_rake_center.child_right()->p_cluster(), it_vertex3_mixed_rake_center, 1);

                                                p_cluster_tree_node_rake_center_next = &cluster_tree_node_rake_center_next;
                                                std::tie(cluster_tree_node_rake_center_next.arc_backward.head, cluster_tree_node_rake_center_next.arc_forward.head) = std::make_tuple(cluster_tree_node_external_rake_center->arc_backward.head, cluster_tree_node_external_rake_center->arc_forward.head);
                                                cluster_tree_node_rake_center_next.contraction_type() = false;
                                                cluster_tree_node_rake_center_next.child_left() = p_cluster_tree_node_mixed_rake_target;
                                                cluster_tree_node_rake_center_next.child_right() = cluster_tree_node_external_rake_center;
                                                std::tie(it_vertex1_rake_center_next, it_vertex2_rake_center_next, it_vertex3_rake_center_next) = cluster_tree_node_rake_center_next.rake_it_vertexes();
                                                std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).join(graph(), cluster_tree_node_rake_center_next.p_cluster(), it_vertex1_rake_center_next, cluster_tree_node_rake_center_next.child_left()->p_cluster(), it_vertex2_rake_center_next, cluster_tree_node_rake_center_next.child_right()->p_cluster(), it_vertex3_rake_center_next, 1);
                                            }
                                            cluster_tree_node_t *cluster_tree_node_external1_next, *cluster_tree_node_external2_next;
                                            if(cluster_tree_node->child_left()->arc_backward.head == cluster_tree_node->arc_backward.head || cluster_tree_node->child_left()->arc_backward.head == cluster_tree_node->arc_forward.head)
                                                std::tie(cluster_tree_node_external1_next, cluster_tree_node_external2_next) = std::make_tuple(p_cluster_tree_node_rake_center_next, nullptr);
                                            else if(cluster_tree_node->child_left()->arc_forward.head == cluster_tree_node->arc_backward.head || cluster_tree_node->child_left()->arc_forward.head == cluster_tree_node->arc_forward.head)
                                                std::tie(cluster_tree_node_external1_next, cluster_tree_node_external2_next) = std::make_tuple(nullptr, p_cluster_tree_node_rake_center_next);
                                            else std::unreachable();
                                            std::optional<it_edge_it_vertexes_t> result(this_(this_, cluster_tree_node_external1_next, cluster_tree_node->child_left(), cluster_tree_node_external2_next));
                                            if(cluster_tree_node_external_rake_center == nullptr)
                                                assert(p_cluster_tree_node_rake_center_next == p_cluster_tree_node_mixed_rake_target);
                                            else
                                            {
                                                assert(p_cluster_tree_node_rake_center_next == &cluster_tree_node_rake_center_next);
                                                std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).split(graph(), cluster_tree_node_rake_center_next.p_cluster(), it_vertex1_rake_center_next, cluster_tree_node_rake_center_next.child_left()->p_cluster(), it_vertex2_rake_center_next, cluster_tree_node_rake_center_next.child_right()->p_cluster(), it_vertex3_rake_center_next, 1);
                                            }
                                            if(cluster_tree_node_external_rake_target == nullptr)
                                                assert(p_cluster_tree_node_mixed_rake_target == cluster_tree_node->child_right());
                                            else
                                            {
                                                assert(p_cluster_tree_node_mixed_rake_target == &cluster_tree_node_mixed_rake_target);
                                                std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).split(graph(), cluster_tree_node_mixed_rake_target.p_cluster(), it_vertex1_mixed_rake_target, cluster_tree_node_mixed_rake_target.child_left()->p_cluster(), it_vertex2_mixed_rake_target, cluster_tree_node_mixed_rake_target.child_right()->p_cluster(), it_vertex3_mixed_rake_target, 2);
                                            }
                                            std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).join(graph(), cluster_tree_node->p_cluster(), it_vertex1, cluster_tree_node->child_left()->p_cluster(), it_vertex2, cluster_tree_node->child_right()->p_cluster(), it_vertex3, 1);
                                            return result;
                                        }
                                    }
                                    else
                                    {
                                        if(cluster_tree_node_external1 == nullptr && cluster_tree_node_external2 == nullptr)
                                        {
                                            assert(p_cluster_tree_node_fake_root == cluster_tree_node);
                                            std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).split(graph(), cluster_tree_node->p_cluster(), it_vertex1, cluster_tree_node->child_left()->p_cluster(), it_vertex2, cluster_tree_node->child_right()->p_cluster(), it_vertex3, 1);
                                            cluster_tree_node_t *cluster_tree_node_external1_next, *cluster_tree_node_external2_next;
                                            if(cluster_tree_node->child_left()->arc_backward.head == cluster_tree_node->arc_backward.head || cluster_tree_node->child_left()->arc_forward.head == cluster_tree_node->arc_backward.head)
                                                std::tie(cluster_tree_node_external1_next, cluster_tree_node_external2_next) = std::make_tuple(cluster_tree_node->child_left(), nullptr);
                                            else if(cluster_tree_node->child_left()->arc_backward.head == cluster_tree_node->arc_forward.head || cluster_tree_node->child_left()->arc_forward.head == cluster_tree_node->arc_forward.head)
                                                std::tie(cluster_tree_node_external1_next, cluster_tree_node_external2_next) = std::make_tuple(nullptr, cluster_tree_node->child_left());
                                            else std::unreachable();
                                            std::optional<it_edge_it_vertexes_t> result(this_(this_, cluster_tree_node_external1_next, cluster_tree_node->child_right(), cluster_tree_node_external2_next));
                                            std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).join(graph(), cluster_tree_node->p_cluster(), it_vertex1, cluster_tree_node->child_left()->p_cluster(), it_vertex2, cluster_tree_node->child_right()->p_cluster(), it_vertex3, 1);
                                            return result;
                                        }
                                        else
                                        {
                                            assert(p_cluster_tree_node_fake_root == &cluster_tree_node_fake_root);
                                            std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).split(graph(), cluster_tree_node_fake_root.p_cluster(), it_vertex1_fake_root, cluster_tree_node_fake_root.child_left()->p_cluster(), it_vertex2_fake_root, cluster_tree_node_fake_root.child_right()->p_cluster(), it_vertex3_fake_root, 1);
                                            if(cluster_tree_node_external_rake_target == nullptr)
                                                assert(p_cluster_tree_node_mixed_rake_target == cluster_tree_node->child_right());
                                            else
                                            {
                                                assert(p_cluster_tree_node_mixed_rake_target == &cluster_tree_node_mixed_rake_target);
                                                std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).split(graph(), cluster_tree_node_mixed_rake_target.p_cluster(), it_vertex1_mixed_rake_target, cluster_tree_node_mixed_rake_target.child_left()->p_cluster(), it_vertex2_mixed_rake_target, cluster_tree_node_mixed_rake_target.child_right()->p_cluster(), it_vertex3_mixed_rake_target, 2);
                                            }
                                            cluster_tree_node_t *cluster_tree_node_external1_next, *cluster_tree_node_external2_next;
                                            if(cluster_tree_node->child_left()->arc_backward.head == cluster_tree_node->arc_backward.head || cluster_tree_node->child_left()->arc_forward.head == cluster_tree_node->arc_backward.head)
                                                std::tie(cluster_tree_node_external1_next, cluster_tree_node_external2_next) = std::make_tuple(p_cluster_tree_node_mixed_rake_center, cluster_tree_node_external_rake_target);
                                            else if(cluster_tree_node->child_left()->arc_backward.head == cluster_tree_node->arc_forward.head || cluster_tree_node->child_left()->arc_forward.head == cluster_tree_node->arc_forward.head)
                                                std::tie(cluster_tree_node_external1_next, cluster_tree_node_external2_next) = std::make_tuple(cluster_tree_node_external_rake_target, p_cluster_tree_node_mixed_rake_center);
                                            else std::unreachable();
                                            std::optional<it_edge_it_vertexes_t> result(this_(this_, cluster_tree_node_external1_next, cluster_tree_node->child_right(), cluster_tree_node_external2_next));
                                            if(cluster_tree_node_external_rake_center == nullptr)
                                                assert(p_cluster_tree_node_mixed_rake_center == cluster_tree_node->child_left());
                                            else
                                            {
                                                assert(p_cluster_tree_node_mixed_rake_center == &cluster_tree_node_mixed_rake_center);
                                                std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).split(graph(), cluster_tree_node_mixed_rake_center.p_cluster(), it_vertex1_mixed_rake_center, cluster_tree_node_mixed_rake_center.child_left()->p_cluster(), it_vertex2_mixed_rake_center, cluster_tree_node_mixed_rake_center.child_right()->p_cluster(), it_vertex3_mixed_rake_center, 1);
                                            }
                                            std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).join(graph(), cluster_tree_node->p_cluster(), it_vertex1, cluster_tree_node->child_left()->p_cluster(), it_vertex2, cluster_tree_node->child_right()->p_cluster(), it_vertex3, 1);
                                            return result;
                                        }
                                    }
                                }
                            }
                            else // compress
                            {
                                it_vertex_t it_vertex1, it_vertex2, it_vertex3;
                                std::tie(it_vertex1, it_vertex2, it_vertex3) = cluster_tree_node->compress_it_vertexes();
                                cluster_tree_node_t *p_cluster_tree_node_mixed1, cluster_tree_node_mixed1(nullptr), *p_cluster_tree_node_mixed2, cluster_tree_node_mixed2(nullptr), *p_cluster_tree_node_fake_root, cluster_tree_node_fake_root(nullptr);
                                it_vertex_t it_vertex1_mixed1, it_vertex2_mixed1, it_vertex3_mixed1;
                                it_vertex_t it_vertex1_mixed2, it_vertex2_mixed2, it_vertex3_mixed2;
                                it_vertex_t it_vertex1_fake_root, it_vertex2_fake_root, it_vertex3_fake_root;
                                it_vertex_t it_vertex_external1, it_vertex_external2;
                                if(cluster_tree_node_external1 == nullptr && cluster_tree_node_external2 == nullptr)
                                {
                                    p_cluster_tree_node_fake_root = cluster_tree_node;
                                    std::tie(it_vertex1_fake_root, it_vertex2_fake_root, it_vertex3_fake_root) = std::make_tuple(it_vertex1, it_vertex2, it_vertex3);
                                }
                                else
                                {
                                    std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).split(graph(), cluster_tree_node->p_cluster(), it_vertex1, cluster_tree_node->child_left()->p_cluster(), it_vertex2, cluster_tree_node->child_right()->p_cluster(), it_vertex3, 2);
                                    if(cluster_tree_node_external1 == nullptr)
                                    {
                                        p_cluster_tree_node_mixed1 = cluster_tree_node->child_left();
                                        it_vertex_external1 = it_vertex1;
                                    }
                                    else
                                    {
                                        p_cluster_tree_node_mixed1 = &cluster_tree_node_mixed1;
                                        it_vertex_external1 = [&]()
                                        {
                                            if(it_vertex1 == cluster_tree_node_external1->arc_backward.head)
                                                return cluster_tree_node_external1->arc_forward.head;
                                            else if(it_vertex1 == cluster_tree_node_external1->arc_forward.head)
                                                return cluster_tree_node_external1->arc_backward.head;
                                            else std::unreachable();
                                        }();
                                        if(std::as_const(graph()->comparator_it_vertex_address)(it_vertex2, it_vertex_external1))
                                        {
                                            std::tie(cluster_tree_node_mixed1.arc_backward.head, cluster_tree_node_mixed1.arc_forward.head) = std::make_tuple(it_vertex2, it_vertex_external1);
                                            cluster_tree_node_mixed1.contraction_type() = true;
                                            cluster_tree_node_mixed1.child_left() = cluster_tree_node->child_left();
                                            cluster_tree_node_mixed1.child_right() = cluster_tree_node_external1;
                                        }
                                        else if(std::as_const(graph()->comparator_it_vertex_address)(it_vertex_external1, it_vertex2))
                                        {
                                            std::tie(cluster_tree_node_mixed1.arc_backward.head, cluster_tree_node_mixed1.arc_forward.head) = std::make_tuple(it_vertex_external1, it_vertex2);
                                            cluster_tree_node_mixed1.contraction_type() = true;
                                            cluster_tree_node_mixed1.child_left() = cluster_tree_node_external1;
                                            cluster_tree_node_mixed1.child_right() = cluster_tree_node->child_left();
                                        }
                                        else std::unreachable();
                                        std::tie(it_vertex1_mixed1, it_vertex2_mixed1, it_vertex3_mixed1) = cluster_tree_node_mixed1.compress_it_vertexes();
                                        std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).join(graph(), cluster_tree_node_mixed1.p_cluster(), it_vertex1_mixed1, cluster_tree_node_mixed1.child_left()->p_cluster(), it_vertex2_mixed1, cluster_tree_node_mixed1.child_right()->p_cluster(), it_vertex3_mixed1, 2);
                                    }
                                    if(cluster_tree_node_external2 == nullptr)
                                    {
                                        p_cluster_tree_node_mixed2 = cluster_tree_node->child_right();
                                        it_vertex_external2 = it_vertex3;
                                    }
                                    else
                                    {
                                        p_cluster_tree_node_mixed2 = &cluster_tree_node_mixed2;
                                        it_vertex_external2 = [&]()
                                        {
                                            if(it_vertex3 == cluster_tree_node_external2->arc_backward.head)
                                                return cluster_tree_node_external2->arc_forward.head;
                                            else if(it_vertex3 == cluster_tree_node_external2->arc_forward.head)
                                                return cluster_tree_node_external2->arc_backward.head;
                                            else std::unreachable();
                                        }();
                                        if(std::as_const(graph()->comparator_it_vertex_address)(it_vertex2, it_vertex_external2))
                                        {
                                            std::tie(cluster_tree_node_mixed2.arc_backward.head, cluster_tree_node_mixed2.arc_forward.head) = std::make_tuple(it_vertex2, it_vertex_external2);
                                            cluster_tree_node_mixed2.contraction_type() = true;
                                            cluster_tree_node_mixed2.child_left() = cluster_tree_node->child_right();
                                            cluster_tree_node_mixed2.child_right() = cluster_tree_node_external2;
                                        }
                                        else if(std::as_const(graph()->comparator_it_vertex_address)(it_vertex_external2, it_vertex2))
                                        {
                                            std::tie(cluster_tree_node_mixed2.arc_backward.head, cluster_tree_node_mixed2.arc_forward.head) = std::make_tuple(it_vertex_external2, it_vertex2);
                                            cluster_tree_node_mixed2.contraction_type() = true;
                                            cluster_tree_node_mixed2.child_left() = cluster_tree_node_external2;
                                            cluster_tree_node_mixed2.child_right() = cluster_tree_node->child_right();
                                        }
                                        else std::unreachable();
                                        std::tie(it_vertex1_mixed2, it_vertex2_mixed2, it_vertex3_mixed2) = cluster_tree_node_mixed2.compress_it_vertexes();
                                        std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).join(graph(), cluster_tree_node_mixed2.p_cluster(), it_vertex1_mixed2, cluster_tree_node_mixed2.child_left()->p_cluster(), it_vertex2_mixed2, cluster_tree_node_mixed2.child_right()->p_cluster(), it_vertex3_mixed2, 2);
                                    }
                                    p_cluster_tree_node_fake_root = &cluster_tree_node_fake_root;
                                    if(std::as_const(graph()->comparator_it_vertex_address)(it_vertex_external1, it_vertex_external2))
                                    {
                                        std::tie(cluster_tree_node_fake_root.arc_backward.head, cluster_tree_node_fake_root.arc_forward.head) = std::make_tuple(it_vertex_external1, it_vertex_external2);
                                        cluster_tree_node_fake_root.contraction_type() = true;
                                        cluster_tree_node_fake_root.child_left() = p_cluster_tree_node_mixed1;
                                        cluster_tree_node_fake_root.child_right() = p_cluster_tree_node_mixed2;
                                    }
                                    else if(std::as_const(graph()->comparator_it_vertex_address)(it_vertex_external2, it_vertex_external1))
                                    {
                                        std::tie(cluster_tree_node_fake_root.arc_backward.head, cluster_tree_node_fake_root.arc_forward.head) = std::make_tuple(it_vertex_external2, it_vertex_external1);
                                        cluster_tree_node_fake_root.contraction_type() = true;
                                        cluster_tree_node_fake_root.child_left() = p_cluster_tree_node_mixed2;
                                        cluster_tree_node_fake_root.child_right() = p_cluster_tree_node_mixed1;
                                    }
                                    else std::unreachable();
                                    std::tie(it_vertex1_fake_root, it_vertex2_fake_root, it_vertex3_fake_root) = cluster_tree_node_fake_root.compress_it_vertexes();
                                    std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).join(graph(), cluster_tree_node_fake_root.p_cluster(), it_vertex1_fake_root, cluster_tree_node_fake_root.child_left()->p_cluster(), it_vertex2_fake_root, cluster_tree_node_fake_root.child_right()->p_cluster(), it_vertex3_fake_root, 2);
                                }

                                std::optional<bool> selector_result(selector(graph(), p_cluster_tree_node_fake_root->p_cluster(), it_vertex1_fake_root, p_cluster_tree_node_fake_root->child_left()->p_cluster(), it_vertex2_fake_root, p_cluster_tree_node_fake_root->child_right()->p_cluster(), it_vertex3_fake_root, 2));
                                if(!selector_result.has_value())
                                {
                                    if(cluster_tree_node_external1 == nullptr && cluster_tree_node_external2 == nullptr)
                                        assert(p_cluster_tree_node_fake_root == cluster_tree_node);
                                    else
                                    {
                                        assert(p_cluster_tree_node_fake_root == &cluster_tree_node_fake_root);
                                        std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).split(graph(), cluster_tree_node_fake_root.p_cluster(), it_vertex1_fake_root, cluster_tree_node_fake_root.child_left()->p_cluster(), it_vertex2_fake_root, cluster_tree_node_fake_root.child_right()->p_cluster(), it_vertex3_fake_root, 2);
                                        if(cluster_tree_node_external1 == nullptr)
                                            assert(p_cluster_tree_node_mixed1 == cluster_tree_node->child_left());
                                        else
                                        {
                                            assert(p_cluster_tree_node_mixed1 == &cluster_tree_node_mixed1);
                                            std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).split(graph(), cluster_tree_node_mixed1.p_cluster(), it_vertex1_mixed1, cluster_tree_node_mixed1.child_left()->p_cluster(), it_vertex2_mixed1, cluster_tree_node_mixed1.child_right()->p_cluster(), it_vertex3_mixed1, 2);
                                        }
                                        if(cluster_tree_node_external2 == nullptr)
                                            assert(p_cluster_tree_node_mixed2 == cluster_tree_node->child_right());
                                        else
                                        {
                                            assert(p_cluster_tree_node_mixed2 == &cluster_tree_node_mixed2);
                                            std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).split(graph(), cluster_tree_node_mixed2.p_cluster(), it_vertex1_mixed2, cluster_tree_node_mixed2.child_left()->p_cluster(), it_vertex2_mixed2, cluster_tree_node_mixed2.child_right()->p_cluster(), it_vertex3_mixed2, 2);
                                        }
                                        std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).join(graph(), cluster_tree_node->p_cluster(), it_vertex1, cluster_tree_node->child_left()->p_cluster(), it_vertex2, cluster_tree_node->child_right()->p_cluster(), it_vertex3, 2);
                                    }
                                    return std::nullopt;
                                }
                                else
                                {
                                    if(!selector_result.value())
                                    {
                                        if(cluster_tree_node_external1 == nullptr && cluster_tree_node_external2 == nullptr)
                                        {
                                            assert(p_cluster_tree_node_fake_root == cluster_tree_node);
                                            std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).split(graph(), cluster_tree_node->p_cluster(), it_vertex1, cluster_tree_node->child_left()->p_cluster(), it_vertex2, cluster_tree_node->child_right()->p_cluster(), it_vertex3, 2);
                                            cluster_tree_node_t *cluster_tree_node_external1_next, *cluster_tree_node_external2_next;
                                            if(cluster_tree_node->child_left()->arc_backward.head == cluster_tree_node->arc_backward.head)
                                                std::tie(cluster_tree_node_external1_next, cluster_tree_node_external2_next) = std::make_tuple(nullptr, cluster_tree_node->child_right());
                                            else if(cluster_tree_node->child_left()->arc_forward.head == cluster_tree_node->arc_backward.head)
                                                std::tie(cluster_tree_node_external1_next, cluster_tree_node_external2_next) = std::make_tuple(cluster_tree_node->child_right(), nullptr);
                                            else std::unreachable();
                                            std::optional<it_edge_it_vertexes_t> result(this_(this_, cluster_tree_node_external1_next, cluster_tree_node->child_left(), cluster_tree_node_external2_next));
                                            std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).join(graph(), cluster_tree_node->p_cluster(), it_vertex1, cluster_tree_node->child_left()->p_cluster(), it_vertex2, cluster_tree_node->child_right()->p_cluster(), it_vertex3, 2);
                                            return result;
                                        }
                                        else
                                        {
                                            assert(p_cluster_tree_node_fake_root == &cluster_tree_node_fake_root);
                                            std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).split(graph(), cluster_tree_node_fake_root.p_cluster(), it_vertex1_fake_root, cluster_tree_node_fake_root.child_left()->p_cluster(), it_vertex2_fake_root, cluster_tree_node_fake_root.child_right()->p_cluster(), it_vertex3_fake_root, 2);
                                            if(cluster_tree_node_external1 == nullptr)
                                                assert(p_cluster_tree_node_mixed1 == cluster_tree_node->child_left());
                                            else
                                            {
                                                assert(p_cluster_tree_node_mixed1 == &cluster_tree_node_mixed1);
                                                std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).split(graph(), cluster_tree_node_mixed1.p_cluster(), it_vertex1_mixed1, cluster_tree_node_mixed1.child_left()->p_cluster(), it_vertex2_mixed1, cluster_tree_node_mixed1.child_right()->p_cluster(), it_vertex3_mixed1, 2);
                                            }
                                            cluster_tree_node_t *cluster_tree_node_external1_next, *cluster_tree_node_external2_next;
                                            if(cluster_tree_node->child_left()->arc_backward.head == cluster_tree_node->arc_backward.head)
                                                std::tie(cluster_tree_node_external1_next, cluster_tree_node_external2_next) = std::make_tuple(cluster_tree_node_external1, p_cluster_tree_node_mixed2);
                                            else if(cluster_tree_node->child_left()->arc_forward.head == cluster_tree_node->arc_backward.head)
                                                std::tie(cluster_tree_node_external1_next, cluster_tree_node_external2_next) = std::make_tuple(p_cluster_tree_node_mixed2, cluster_tree_node_external1);
                                            else std::unreachable();
                                            std::optional<it_edge_it_vertexes_t> result(this_(this_, cluster_tree_node_external1_next, cluster_tree_node->child_left(), cluster_tree_node_external2_next));
                                            if(cluster_tree_node_external2 == nullptr)
                                                assert(p_cluster_tree_node_mixed2 == cluster_tree_node->child_right());
                                            else
                                            {
                                                assert(p_cluster_tree_node_mixed2 == &cluster_tree_node_mixed2);
                                                std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).split(graph(), cluster_tree_node_mixed2.p_cluster(), it_vertex1_mixed2, cluster_tree_node_mixed2.child_left()->p_cluster(), it_vertex2_mixed2, cluster_tree_node_mixed2.child_right()->p_cluster(), it_vertex3_mixed2, 2);
                                            }
                                            std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).join(graph(), cluster_tree_node->p_cluster(), it_vertex1, cluster_tree_node->child_left()->p_cluster(), it_vertex2, cluster_tree_node->child_right()->p_cluster(), it_vertex3, 2);
                                            return result;
                                        }
                                    }
                                    else
                                    {
                                        if(cluster_tree_node_external1 == nullptr && cluster_tree_node_external2 == nullptr)
                                        {
                                            assert(p_cluster_tree_node_fake_root == cluster_tree_node);
                                            std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).split(graph(), cluster_tree_node->p_cluster(), it_vertex1, cluster_tree_node->child_left()->p_cluster(), it_vertex2, cluster_tree_node->child_right()->p_cluster(), it_vertex3, 2);
                                            cluster_tree_node_t *cluster_tree_node_external1_next, *cluster_tree_node_external2_next;
                                            if(cluster_tree_node->child_right()->arc_forward.head == cluster_tree_node->arc_forward.head)
                                                std::tie(cluster_tree_node_external1_next, cluster_tree_node_external2_next) = std::make_tuple(cluster_tree_node->child_left(), nullptr);
                                            else if(cluster_tree_node->child_left()->arc_backward.head == cluster_tree_node->arc_forward.head)
                                                std::tie(cluster_tree_node_external1_next, cluster_tree_node_external2_next) = std::make_tuple(nullptr, cluster_tree_node->child_left());
                                            else std::unreachable();
                                            std::optional<it_edge_it_vertexes_t> result(this_(this_, cluster_tree_node_external1_next, cluster_tree_node->child_right(), cluster_tree_node_external2_next));
                                            std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).join(graph(), cluster_tree_node->p_cluster(), it_vertex1, cluster_tree_node->child_left()->p_cluster(), it_vertex2, cluster_tree_node->child_right()->p_cluster(), it_vertex3, 2);
                                            return result;
                                        }
                                        else
                                        {
                                            assert(p_cluster_tree_node_fake_root == &cluster_tree_node_fake_root);
                                            std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).split(graph(), cluster_tree_node_fake_root.p_cluster(), it_vertex1_fake_root, cluster_tree_node_fake_root.child_left()->p_cluster(), it_vertex2_fake_root, cluster_tree_node_fake_root.child_right()->p_cluster(), it_vertex3_fake_root, 2);
                                            if(cluster_tree_node_external2 == nullptr)
                                                assert(p_cluster_tree_node_mixed2 == cluster_tree_node->child_right());
                                            else
                                            {
                                                assert(p_cluster_tree_node_mixed2 == &cluster_tree_node_mixed2);
                                                std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).split(graph(), cluster_tree_node_mixed2.p_cluster(), it_vertex1_mixed2, cluster_tree_node_mixed2.child_left()->p_cluster(), it_vertex2_mixed2, cluster_tree_node_mixed2.child_right()->p_cluster(), it_vertex3_mixed2, 2);
                                            }
                                            cluster_tree_node_t *cluster_tree_node_external1_next, *cluster_tree_node_external2_next;
                                            if(cluster_tree_node->child_right()->arc_forward.head == cluster_tree_node->arc_forward.head)
                                                std::tie(cluster_tree_node_external1_next, cluster_tree_node_external2_next) = std::make_tuple(p_cluster_tree_node_mixed1, cluster_tree_node_external2);
                                            else if(cluster_tree_node->child_right()->arc_backward.head == cluster_tree_node->arc_forward.head)
                                                std::tie(cluster_tree_node_external1_next, cluster_tree_node_external2_next) = std::make_tuple(cluster_tree_node_external2, p_cluster_tree_node_mixed1);
                                            else std::unreachable();
                                            std::optional<it_edge_it_vertexes_t> result(this_(this_, cluster_tree_node_external1_next, cluster_tree_node->child_right(), cluster_tree_node_external2_next));
                                            if(cluster_tree_node_external1 == nullptr)
                                                assert(p_cluster_tree_node_mixed1 == cluster_tree_node->child_left());
                                            else
                                            {
                                                assert(p_cluster_tree_node_mixed1 == &cluster_tree_node_mixed1);
                                                std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).split(graph(), cluster_tree_node_mixed1.p_cluster(), it_vertex1_mixed1, cluster_tree_node_mixed1.child_left()->p_cluster(), it_vertex2_mixed1, cluster_tree_node_mixed1.child_right()->p_cluster(), it_vertex3_mixed1, 2);
                                            }
                                            std::as_const(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations).join(graph(), cluster_tree_node->p_cluster(), it_vertex1, cluster_tree_node->child_left()->p_cluster(), it_vertex2, cluster_tree_node->child_right()->p_cluster(), it_vertex3, 2);
                                            return result;
                                        }
                                    }
                                }
                            }
                        }
                        else std::unreachable();
                    };
                    return non_local_search_impl(non_local_search_impl, nullptr, root_cluster_tree_node, nullptr);
                }

                cluster_list_node_end_t *create_node_end() { return tagged_ptr_bit0_setted(new cluster_list_node_end_t()); }
                void destroy_node_end() { delete tagged_ptr_bit0_unsetted(cluster_list_node_end); }
                augmented_graph_part_t()
                    requires(std::is_same_v<comparator_it_edge_it_vertexes_internal_operations_t, void>)
                    : cluster_list_node_end(create_node_end()),
                      it_vertexes_it_vertexes_arc_and_it_edge_it_vertexes_base_level(std::cref(graph()->comparator_it_vertex_address), allocator_it_vertex_it_vertexes_arc_and_it_edge_it_vertexes_base_level_t(graph()->vertexes.get_allocator())),
                      it_vertexes_trivial_component(std::cref(graph()->comparator_it_vertex_address), allocator_it_vertex_t(graph()->vertexes.get_allocator())),
                      it_edges_fundamental(std::cref(graph()->comparator_it_edge), allocator_it_edge_it_vertexes_t(graph()->vertexes.get_allocator()))
                {
                }
                augmented_graph_part_t()
                    requires(!std::is_same_v<comparator_it_edge_it_vertexes_internal_operations_t, void>)
                    : cluster_list_node_end(create_node_end()),
                      it_vertexes_it_vertexes_arc_and_it_edge_it_vertexes_base_level(std::cref(graph()->comparator_it_vertex_address), allocator_it_vertex_it_vertexes_arc_and_it_edge_it_vertexes_base_level_t(graph()->vertexes.get_allocator())),
                      it_vertexes_trivial_component(std::cref(graph()->comparator_it_vertex_address), allocator_it_vertex_t(graph()->vertexes.get_allocator())),
                      it_edges_fundamental(std::cref(tagged_ptr_bit0_unsetted(cluster_list_node_end)->internal_operations.comparator_it_edge_it_vertexes), allocator_it_edge_it_vertexes_t(graph()->vertexes.get_allocator()))
                {
                }
                ~augmented_graph_part_t() { destroy_node_end(); }

                void vertex_inserted(it_vertex_t it_vertex)
                {
                    it_vertexes_trivial_component.insert(it_vertex);
                }
                void vertex_erasing(it_vertex_t it_vertex)
                {
                    auto it_single_vertex = it_vertexes_trivial_component.find(it_vertex);
                    assert(it_single_vertex != it_vertexes_trivial_component.end());
                    unexpose_relaxed(it_vertex);
                    it_vertexes_trivial_component.erase(it_single_vertex);
                }
                void edge_inserted(it_edge_it_vertexes_t it_edge)
                {
                    link(it_edge);
                }
                void edge_erasing(it_edge_it_vertexes_t it_edge)
                {
                    cut(it_edge);
                }
                void edge_updating(it_edge_it_vertexes_t it_edge)
                {
                    cut(it_edge);
                }
                void edge_updated(it_edge_it_vertexes_t it_edge)
                {
                    link(it_edge);
                }
            };
        } // namespace augmented_graph
    } // namespace detail

    template<typename vertex_t_, typename edge_t_, std::strict_weak_order<vertex_t_, vertex_t_> comparator_vertex_t_ = std::less<vertex_t_>, std::strict_weak_order<edge_t_, edge_t_> comparator_edge_t_ = std::less<edge_t_>, typename allocator_vertex_t_ = std::allocator<vertex_t_>>
    struct augmented_graph_type_aliases_t
    {
        using vertex_t = vertex_t_;
        using comparator_vertex_t = comparator_vertex_t_;
        using edge_t = edge_t_;
        using comparator_edge_t = comparator_edge_t_;

        using allocator_vertex_t = allocator_vertex_t_;
        using vertexes_t = std::multiset<vertex_t, std::reference_wrapper<comparator_vertex_t const>, allocator_vertex_t>;
        using it_vertex_t = vertexes_t::iterator;
        struct comparator_it_vertex_t
        {
            comparator_vertex_t comparator_vertex;
            bool operator()(it_vertex_t const &lhs, it_vertex_t const &rhs) const
            {
                if(comparator_vertex(*lhs, *rhs))
                    return true;
                else if(comparator_vertex(*rhs, *lhs))
                    return false;
                else
                    return std::as_const(detail::utility::unmove(std::less<typename vertexes_t::value_type const *>{}))(&*lhs, &*rhs);
            }
        };
        struct comparator_it_vertex_address_t
        {
            bool operator()(it_vertex_t const &lhs, it_vertex_t const &rhs) const
            {
                return std::as_const(detail::utility::unmove(std::less<typename vertexes_t::value_type const *>{}))(&*lhs, &*rhs);
            }
        };

        using allocator_edges_it_vertexes_t = std::allocator_traits<allocator_vertex_t>::template rebind_alloc<std::pair<edge_t const, std::pair<it_vertex_t, it_vertex_t>>>;
        using edges_it_vertexes_t = std::multimap<edge_t, std::pair<it_vertex_t, it_vertex_t>, std::reference_wrapper<comparator_edge_t const>, allocator_edges_it_vertexes_t>;
        using it_edge_it_vertexes_t = edges_it_vertexes_t ::iterator;
        struct comparator_it_edge_it_vertexes_t
        {
            comparator_edge_t comparator_edge;
            bool operator()(it_edge_it_vertexes_t const &lhs, it_edge_it_vertexes_t const &rhs) const
            {
                if(comparator_edge(lhs->first, rhs->first))
                    return true;
                else if(comparator_edge(rhs->first, lhs->first))
                    return false;
                else
                    return std::as_const(detail::utility::unmove(std::less<typename edges_it_vertexes_t::key_type const *>{}))(&lhs->first, &rhs->first);
            }
        };
        struct comparator_it_edge_it_vertexes_address_t
        {
            bool operator()(it_edge_it_vertexes_t const &lhs, it_edge_it_vertexes_t const &rhs) const
            {
                return std::as_const(detail::utility::unmove(std::less<typename edges_it_vertexes_t::key_type const *>{}))(&lhs->first, &rhs->first);
            }
        };

        using allocator_it_edge_it_vertexes_t = std::allocator_traits<allocator_vertex_t>::template rebind_alloc<it_edge_it_vertexes_t>;
        using allocator_it_vertex_it_edges_it_vertexes_t = std::allocator_traits<allocator_vertex_t>::template rebind_alloc<std::pair< //
            it_vertex_t const, //
            std::set< //
                it_edge_it_vertexes_t, //
                std::reference_wrapper<comparator_it_edge_it_vertexes_address_t const>, //
                allocator_it_edge_it_vertexes_t //
                > //
            >>;
        using allocator_it_vertex_it_vertexes_it_edges_it_vertexes_t = std::allocator_traits<allocator_vertex_t>::template rebind_alloc<std::pair< //
            it_vertex_t const, //
            std::map< //
                it_vertex_t, //
                std::set< //
                    it_edge_it_vertexes_t, //
                    std::reference_wrapper<comparator_it_edge_it_vertexes_address_t const>, //
                    allocator_it_edge_it_vertexes_t //
                    >, //
                std::reference_wrapper<comparator_it_vertex_address_t const>, //
                allocator_it_vertex_it_edges_it_vertexes_t //
                > //
            >>;
        using it_vertexes_it_vertexes_it_edges_t = std::map< //
            it_vertex_t, //
            std::map< //
                it_vertex_t, //
                std::set< //
                    it_edge_it_vertexes_t, //
                    std::reference_wrapper<comparator_it_edge_it_vertexes_address_t const>, //
                    allocator_it_edge_it_vertexes_t //
                    >, //
                std::reference_wrapper<comparator_it_vertex_address_t const>, //
                allocator_it_vertex_it_edges_it_vertexes_t //
                >, //
            std::reference_wrapper<comparator_it_vertex_address_t const>, //
            allocator_it_vertex_it_vertexes_it_edges_it_vertexes_t //
            >;

        using it_vertexes_it_edges_t = std::map< //
            it_vertex_t, //
            std::set< //
                it_edge_it_vertexes_t, //
                std::reference_wrapper<comparator_it_edge_it_vertexes_address_t const>, //
                allocator_it_edge_it_vertexes_t //
                >, //
            std::reference_wrapper<comparator_it_vertex_address_t const>, //
            allocator_it_vertex_it_edges_it_vertexes_t //
            >;
    };

    template<typename vertex_t_, typename edge_t_, typename parts_data_structure_and_parameters_t_, std::strict_weak_order<vertex_t_, vertex_t_> comparator_vertex_t_ = std::less<vertex_t_>, std::strict_weak_order<edge_t_, edge_t_> comparator_edge_t_ = std::less<edge_t_>, typename allocator_vertex_t_ = std::allocator<vertex_t_>>
    struct augmented_graph_t
    {
        using augmented_graph_type_aliases_t = augmented_graph_type_aliases_t<vertex_t_, edge_t_, comparator_vertex_t_, comparator_edge_t_, allocator_vertex_t_>;

        using vertex_t = augmented_graph_type_aliases_t::vertex_t;
        using comparator_vertex_t = augmented_graph_type_aliases_t::comparator_vertex_t;
        using edge_t = augmented_graph_type_aliases_t::edge_t;
        using comparator_edge_t = augmented_graph_type_aliases_t::comparator_edge_t;
        using parts_data_structure_and_parameters_t = parts_data_structure_and_parameters_t_;

        using allocator_vertex_t = augmented_graph_type_aliases_t::allocator_vertex_t;
        using vertexes_t = augmented_graph_type_aliases_t::vertexes_t;
        using it_vertex_t = augmented_graph_type_aliases_t::it_vertex_t;
        using comparator_it_vertex_t = augmented_graph_type_aliases_t::comparator_it_vertex_t;
        comparator_it_vertex_t comparator_it_vertex;
        using comparator_it_vertex_address_t = augmented_graph_type_aliases_t::comparator_it_vertex_address_t;
        comparator_it_vertex_address_t comparator_it_vertex_address;
        vertexes_t vertexes;
        std::size_t size_vertex() const { return vertexes.size(); }
        it_vertex_t begin_vertex() { return vertexes.begin(); }
        it_vertex_t end_vertex() { return vertexes.end(); }

        using allocator_edges_it_vertexes_t = augmented_graph_type_aliases_t::allocator_edges_it_vertexes_t;
        using edges_it_vertexes_t = augmented_graph_type_aliases_t::edges_it_vertexes_t;
        using it_edge_it_vertexes_t = augmented_graph_type_aliases_t::it_edge_it_vertexes_t;
        using comparator_it_edge_it_vertexes_t = augmented_graph_type_aliases_t::comparator_it_edge_it_vertexes_t;
        comparator_it_edge_it_vertexes_t comparator_it_edge;
        using comparator_it_edge_it_vertexes_address_t = augmented_graph_type_aliases_t::comparator_it_edge_it_vertexes_address_t;
        comparator_it_edge_it_vertexes_address_t comparator_it_edge_address;
        edges_it_vertexes_t edges_it_vertexes;
        std::size_t size_edge() const { return edges_it_vertexes.size(); }
        it_edge_it_vertexes_t begin_edge() { return edges_it_vertexes.begin(); }
        it_edge_it_vertexes_t end_edge() { return edges_it_vertexes.end(); }

        using allocator_it_edge_it_vertexes_t = augmented_graph_type_aliases_t::allocator_it_edge_it_vertexes_t;
        using allocator_it_vertex_it_edges_it_vertexes_t = augmented_graph_type_aliases_t::allocator_it_vertex_it_edges_it_vertexes_t;
        using allocator_it_vertex_it_vertexes_it_edges_it_vertexes_t = augmented_graph_type_aliases_t::allocator_it_vertex_it_vertexes_it_edges_it_vertexes_t;
        using it_vertexes_it_vertexes_it_edges_t = augmented_graph_type_aliases_t::it_vertexes_it_vertexes_it_edges_t;
        it_vertexes_it_vertexes_it_edges_t it_vertexes_it_vertexes_it_edges;

        using it_vertexes_it_edges_t = augmented_graph_type_aliases_t::it_vertexes_it_edges_t;
        it_vertexes_it_edges_t it_vertexes_it_edges;

        augmented_graph_t(comparator_vertex_t const &comparator_vertex, comparator_edge_t const &comparator_edge, allocator_vertex_t const &allocator_vertexes = allocator_vertex_t())
            : comparator_it_vertex{comparator_vertex},
              comparator_it_vertex_address{},
              vertexes(std::cref(comparator_it_vertex.comparator_vertex), allocator_vertex_t(allocator_vertexes)),
              comparator_it_edge{comparator_edge},
              comparator_it_edge_address{},
              edges_it_vertexes(std::cref(comparator_it_edge.comparator_edge), allocator_edges_it_vertexes_t(allocator_vertexes)),
              it_vertexes_it_vertexes_it_edges(std::cref(comparator_it_vertex_address), allocator_it_vertex_it_vertexes_it_edges_it_vertexes_t(allocator_vertexes)),
              it_vertexes_it_edges(std::cref(comparator_it_vertex_address), allocator_it_vertex_it_edges_it_vertexes_t(allocator_vertexes))
        {}
        augmented_graph_t(allocator_vertex_t const &allocator_vertexes)
            : augmented_graph_t(comparator_vertex_t(), comparator_edge_t(), allocator_vertexes)
        {}
        augmented_graph_t()
            : augmented_graph_t(allocator_vertex_t())
        {}
        ~augmented_graph_t()
        {
            while(!vertexes.empty())
                erase_vertex(vertexes.begin());
        }

        template<std::size_t index, typename part_data_structure_and_parameters_t>
        struct part_data_structure_and_parameters_t_to_part_t: std::type_identity<detail::augmented_graph::augmented_graph_part_t<augmented_graph_t, index, typename part_data_structure_and_parameters_t::first_type, typename part_data_structure_and_parameters_t::second_type::template type<augmented_graph_t>>>
        {};
        using parts_t = detail::utility::map_transform_t<parts_data_structure_and_parameters_t, part_data_structure_and_parameters_t_to_part_t>;
        static constexpr std::size_t parts_count = std::tuple_size_v<parts_t>;
        template<std::size_t I>
            requires(I >= 0 && I < parts_count)
        using part_t = std::tuple_element_t<I, parts_t>;
        template<std::size_t I>
            requires(I >= 0 && I < parts_count)
        static constexpr augmented_graph_part_data_structure_e part_data_structure = std::tuple_element_t<I, parts_data_structure_and_parameters_t>::first_type::value;
        template<std::size_t I>
            requires(I >= 0 && I < parts_count)
        using part_parameters = std::tuple_element_t<I, parts_data_structure_and_parameters_t>::second_type;

        parts_t parts;
        template<std::size_t I>
            requires(I >= 0 && I < parts_count)
        part_t<I> const &part() const &
        {
            return std::get<I>(parts);
        }
        template<std::size_t I>
            requires(I >= 0 && I < parts_count)
        part_t<I> &part() &
        {
            return std::get<I>(parts);
        }
        template<std::size_t I>
            requires(I >= 0 && I < parts_count)
        part_t<I> const &&part() const &&
        {
            return std::get<I>(parts);
        }
        template<std::size_t I>
            requires(I >= 0 && I < parts_count)
        part_t<I> &&part() &&
        {
            return std::get<I>(parts);
        }
        template<std::size_t I>
            requires(I >= 0 && I < parts_count)
        static constexpr std::size_t part_offset()
        {
#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Winvalid-offsetof"
#endif
            return offsetof(augmented_graph_t, parts) + detail::tuple::tuple_element_offset<I, parts_t>();
#ifdef __clang__
    #pragma clang diagnostic pop
#endif
        }

        it_vertex_t insert_vertex(vertex_t vertex)
        {
            [ this, &vertex ]<std::size_t... I>(std::index_sequence<I...>)
            {
                (..., [this, &vertex]
                    { this->part<I>().vertex_inserting(vertex); }());
            }
            (std::make_index_sequence<parts_count>());

            it_vertex_t it_vertex = vertexes.insert(vertex);
            it_vertexes_it_edges.emplace(std::piecewise_construct, std::forward_as_tuple(it_vertex), std::forward_as_tuple(std::cref(comparator_it_edge_address)));

            [ this, &it_vertex ]<std::size_t... I>(std::index_sequence<I...>)
            {
                (..., [this, &it_vertex]
                    { this->part<I>().vertex_inserted(it_vertex); }());
            }
            (std::make_index_sequence<parts_count>());

            return it_vertex;
        }
        it_vertex_t erase_vertex(it_vertex_t it_vertex)
        {
            assert(it_vertex != vertexes.end());

            auto it_vertex_it_edges = it_vertexes_it_edges.find(it_vertex);
            assert(it_vertex_it_edges != it_vertexes_it_edges.end());
            while(!it_vertex_it_edges->second.empty())
                this->erase_edge(*it_vertex_it_edges->second.begin());

            [ this, &it_vertex ]<std::size_t... I>(std::index_sequence<I...>)
            {
                (..., [this, &it_vertex]
                    { this->part<I>().vertex_erasing(it_vertex); }());
            }
            (std::make_index_sequence<parts_count>());

            assert(it_vertex_it_edges->second.empty());
            it_vertexes_it_edges.erase(it_vertex_it_edges);

            it_vertex_t it_vertex_next = std::ranges::next(it_vertex);
            auto node_handle_vertex = vertexes.extract(it_vertex);

            [ this, &node_handle_vertex ]<std::size_t... I>(std::index_sequence<I...>)
            {
                (..., [this, &node_handle_vertex]
                    { this->part<I>().vertex_erased(node_handle_vertex); }());
            }
            (std::make_index_sequence<parts_count>());

            return it_vertex_next;
        }
        template<std::invocable<vertex_t &> updater_t>
        void update_vertex(it_vertex_t it_vertex, updater_t updater)
        {
            assert(it_vertex != vertexes.end());

            [ this, &it_vertex ]<std::size_t... I>(std::index_sequence<I...>)
            {
                (..., [this, &it_vertex]
                    { this->part<I>().vertex_updating(it_vertex); }());
            }
            (std::make_index_sequence<parts_count>());

            auto node_handle_vertex = vertexes.extract(it_vertex);
            updater(node_handle_vertex.value());
            vertexes.insert(std::move(node_handle_vertex));

            [ this, &it_vertex ]<std::size_t... I>(std::index_sequence<I...>)
            {
                (..., [this, &it_vertex]
                    { this->part<I>().vertex_updated(it_vertex); }());
            }
            (std::make_index_sequence<parts_count>());
        }
        it_edge_it_vertexes_t insert_edge(it_vertex_t it_vertex1, it_vertex_t it_vertex2, edge_t edge)
        {
            assert(it_vertex1 != vertexes.end() && it_vertex2 != vertexes.end());

            if(std::as_const(comparator_it_vertex_address)(it_vertex2, it_vertex1))
                std::ranges::swap(it_vertex1, it_vertex2);

            [ this, &it_vertex1, &it_vertex2, &edge ]<std::size_t... I>(std::index_sequence<I...>)
            {
                (..., [this, &it_vertex1, &it_vertex2, &edge]
                    { this->part<I>().edge_inserting(it_vertex1, it_vertex2, edge); }());
            }
            (std::make_index_sequence<parts_count>());

            it_edge_it_vertexes_t it_edge = edges_it_vertexes.emplace(std::piecewise_construct, std::forward_as_tuple(edge), std::forward_as_tuple(std::piecewise_construct, std::forward_as_tuple(it_vertex1), std::forward_as_tuple(it_vertex2)));
            if(it_vertex1 == it_vertex2)
            {
                it_vertexes_it_edges.at(it_vertex1).insert(it_edge);

                auto it_vertex_vertexes_edges = it_vertexes_it_vertexes_it_edges.emplace(std::piecewise_construct, std::forward_as_tuple(it_vertex1), std::forward_as_tuple(std::cref(comparator_it_vertex_address))).first;
                auto it_vertex_vertex_edges = it_vertex_vertexes_edges->second.emplace(std::piecewise_construct, std::forward_as_tuple(it_vertex2), std::forward_as_tuple(std::cref(comparator_it_edge_address))).first;
                it_vertex_vertex_edges->second.insert(it_edge);
            }
            else
            {
                it_vertexes_it_edges.at(it_vertex1).insert(it_edge);
                it_vertexes_it_edges.at(it_vertex2).insert(it_edge);
                {
                    auto it_vertex_vertexes_edges = it_vertexes_it_vertexes_it_edges.emplace(std::piecewise_construct, std::forward_as_tuple(it_vertex1), std::forward_as_tuple(std::cref(comparator_it_vertex_address))).first;
                    auto it_vertex_vertex_edges = it_vertex_vertexes_edges->second.emplace(std::piecewise_construct, std::forward_as_tuple(it_vertex2), std::forward_as_tuple(std::cref(comparator_it_edge_address))).first;
                    it_vertex_vertex_edges->second.insert(it_edge);
                }
                {
                    auto it_vertex_vertexes_edges = it_vertexes_it_vertexes_it_edges.emplace(std::piecewise_construct, std::forward_as_tuple(it_vertex2), std::forward_as_tuple(std::cref(comparator_it_vertex_address))).first;
                    auto it_vertex_vertex_edges = it_vertex_vertexes_edges->second.emplace(std::piecewise_construct, std::forward_as_tuple(it_vertex1), std::forward_as_tuple(std::cref(comparator_it_edge_address))).first;
                    it_vertex_vertex_edges->second.insert(it_edge);
                }
            }

            [ this, &it_edge ]<std::size_t... I>(std::index_sequence<I...>)
            {
                (..., [this, &it_edge]
                    { this->part<I>().edge_inserted(it_edge); }());
            }
            (std::make_index_sequence<parts_count>());

            return it_edge;
        }
        it_edge_it_vertexes_t erase_edge(it_edge_it_vertexes_t it_edge)
        {
            assert(it_edge != edges_it_vertexes.end());

            [ this, &it_edge ]<std::size_t... I>(std::index_sequence<I...>)
            {
                (..., [this, &it_edge]
                    { this->part<I>().edge_erasing(it_edge); }());
            }
            (std::make_index_sequence<parts_count>());

            if(it_edge->second.first == it_edge->second.second)
            {
                it_vertexes_it_edges.at(it_edge->second.first).erase(it_edge);

                it_vertexes_it_vertexes_it_edges.at(it_edge->second.first).at(it_edge->second.second).erase(it_edge);
                if(it_vertexes_it_vertexes_it_edges.at(it_edge->second.first).at(it_edge->second.second).empty())
                {
                    it_vertexes_it_vertexes_it_edges.at(it_edge->second.first).erase(it_edge->second.second);
                    if(it_vertexes_it_vertexes_it_edges.at(it_edge->second.first).empty())
                        it_vertexes_it_vertexes_it_edges.erase(it_edge->second.first);
                }
            }
            else
            {
                it_vertexes_it_edges.at(it_edge->second.first).erase(it_edge);
                it_vertexes_it_edges.at(it_edge->second.second).erase(it_edge);

                it_vertexes_it_vertexes_it_edges.at(it_edge->second.first).at(it_edge->second.second).erase(it_edge);
                if(it_vertexes_it_vertexes_it_edges.at(it_edge->second.first).at(it_edge->second.second).empty())
                {
                    it_vertexes_it_vertexes_it_edges.at(it_edge->second.first).erase(it_edge->second.second);
                    if(it_vertexes_it_vertexes_it_edges.at(it_edge->second.first).empty())
                        it_vertexes_it_vertexes_it_edges.erase(it_edge->second.first);
                }
                it_vertexes_it_vertexes_it_edges.at(it_edge->second.second).at(it_edge->second.first).erase(it_edge);
                if(it_vertexes_it_vertexes_it_edges.at(it_edge->second.second).at(it_edge->second.first).empty())
                {
                    it_vertexes_it_vertexes_it_edges.at(it_edge->second.second).erase(it_edge->second.first);
                    if(it_vertexes_it_vertexes_it_edges.at(it_edge->second.second).empty())
                        it_vertexes_it_vertexes_it_edges.erase(it_edge->second.second);
                }
            }

            it_edge_it_vertexes_t it_edge_next = std::ranges::next(it_edge);
            auto node_handle_edge = edges_it_vertexes.extract(it_edge);

            [ this, &node_handle_edge ]<std::size_t... I>(std::index_sequence<I...>)
            {
                (..., [this, &node_handle_edge]
                    { this->part<I>().edge_erased(node_handle_edge); }());
            }
            (std::make_index_sequence<parts_count>());

            return it_edge_next;
        }
        template<std::invocable<edge_t &> updater_t>
        void update_edge(it_edge_it_vertexes_t it_edge_it_vertexes, updater_t updater)
        {
            assert(it_edge_it_vertexes != edges_it_vertexes.end());

            [ this, &it_edge_it_vertexes ]<std::size_t... I>(std::index_sequence<I...>)
            {
                (..., [this, &it_edge_it_vertexes]
                    { this->part<I>().edge_updating(it_edge_it_vertexes); }());
            }
            (std::make_index_sequence<parts_count>());

            auto node_handle_edge = edges_it_vertexes.extract(it_edge_it_vertexes);
            updater(node_handle_edge.key());
            edges_it_vertexes.insert(std::move(node_handle_edge));

            [ this, &it_edge_it_vertexes ]<std::size_t... I>(std::index_sequence<I...>)
            {
                (..., [this, &it_edge_it_vertexes]
                    { this->part<I>().edge_updated(it_edge_it_vertexes); }());
            }
            (std::make_index_sequence<parts_count>());
        }
    };
} // namespace augmented_containers

#endif // AUGMENTED_GRAPH_HPP
