#ifndef AUGMENTED_DEQUE_HPP
#define AUGMENTED_DEQUE_HPP

#include <algorithm>
#include <cassert>
#include <concepts>
#include <coroutine>
#include <cstddef>
#include <functional>
#include <iterator>
#include <memory>
#include <ranges>
#include <tuple>
#include <utility>

namespace augmented_containers
{
    namespace detail
    {
        namespace language
        {
#ifndef AUGMENTED_CONTAINERS_LANGUAGE_POINTER_TRAITS_CAST
    #define AUGMENTED_CONTAINERS_LANGUAGE_POINTER_TRAITS_CAST
            template<typename target_pointer_t, typename source_pointer_t>
            target_pointer_t pointer_traits_static_cast(source_pointer_t source_pointer) { return std::pointer_traits<target_pointer_t>::pointer_to(*static_cast<typename std::pointer_traits<target_pointer_t>::element_type *>(std::to_address(source_pointer))); }

            template<typename target_pointer_t, typename source_pointer_t>
            target_pointer_t pointer_traits_reinterpret_cast(source_pointer_t source_pointer) { return std::pointer_traits<target_pointer_t>::pointer_to(*reinterpret_cast<typename std::pointer_traits<target_pointer_t>::element_type *>(std::to_address(source_pointer))); }
#endif // AUGMENTED_CONTAINERS_LANGUAGE_POINTER_TRAITS_CAST

#ifndef AUGMENTED_CONTAINERS_LANGUAGE_TAGGED_PTR_BIT0
    #define AUGMENTED_CONTAINERS_LANGUAGE_TAGGED_PTR_BIT0
            template<typename pointer_t>
            bool tagged_ptr_bit0_is_setted(pointer_t p) { return (reinterpret_cast<uintptr_t>(std::to_address(p)) & 0b1) != 0; }
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
            constexpr std::size_t zu(std::size_t v) { return v; }
            constexpr std::ptrdiff_t z(std::ptrdiff_t v) { return v; }
#endif // AUGMENTED_CONTAINERS_LANGUAGE_LITERALS
        } // namespace language

#ifndef AUGMENTED_CONTAINERS_CONCEPTS
    #define AUGMENTED_CONTAINERS_CONCEPTS
        namespace concepts
        {
            template<typename F, typename Ret, typename... Args>
            concept invocable_r = std::invocable<F, Args...> && (std::same_as<Ret, void> || std::convertible_to<std::invoke_result_t<F, Args...>, Ret>) /*&& !
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
            using conditional_const_t = std::conditional_t<is_const, T const, T>;

            template<bool is_const = true, typename T = void>
            constexpr conditional_const_t<is_const, T> &conditional_as_const(T &_Val) noexcept { return _Val; }
            template<bool is_const = true, typename T = void>
            void conditional_as_const(T const &&) = delete;

            template<typename list_t, typename element_t>
            struct list_find_first_index
            {
                template<std::size_t I>
                struct iteration : std::conditional_t<std::is_same_v<typename std::tuple_element_t<I, list_t>, element_t>, std::type_identity<std::integral_constant<std::size_t, I>>, iteration<I + 1>>::type
                {};
                template<>
                struct iteration<std::tuple_size_v<list_t>> : std::integral_constant<std::size_t, std::tuple_size_v<list_t>>
                {};
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
                struct iterations<std::tuple<elements_t...>> : std::type_identity<std::tuple<typename transformer_t<elements_t>::type...>>
                {};
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
                struct impl<std::index_sequence<I...>> : std::type_identity<std::tuple<std::tuple_element_t<1 + I, list_t>...>>
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
                struct iterations<std::tuple<elements_t...>, std::index_sequence<I...>> : std::type_identity<std::tuple<typename transformer_t<I, elements_t>::type...>>
                {};
                using type = typename iterations<map_t, std::make_index_sequence<std::tuple_size_v<map_t>>>::type;
            };
            template<typename map_t, template<std::size_t index, typename item_t> typename transformer_t>
            using map_transform_t = typename map_transform<map_t, transformer_t>::type;
        } // namespace utility
#endif // AUGMENTED_CONTAINERS_UTILITY

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

                    promise_t() : root_or_current(std::coroutine_handle<promise_t>::from_promise(*this)) {}
                    generator_t get_return_object() { return {std::coroutine_handle<promise_t>::from_promise(*this)}; }
                    std::suspend_never initial_suspend() { return {}; }
                    auto final_suspend() noexcept
                    {
                        struct final_awaitable_t
                        {
                            bool await_ready() noexcept { return false; }
                            std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_t> continuation) noexcept
                            {
                                if (continuation.promise().continuation)
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
                        if (exception == nullptr) throw;
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
                                for (std::coroutine_handle<promise_t> state = generator.handle.promise().root_or_current; state != continuation; state = state.promise().continuation)
                                    state.promise().root_or_current = root;
                                generator.handle.promise().exception = &exception;
                                return std::noop_coroutine();
                            }
                            void await_resume() noexcept
                            {
                                if (exception)
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
                generator_t() : handle(nullptr) {}
                generator_t(std::coroutine_handle<promise_t> handle) : handle(handle) {}
                generator_t(generator_t &&other) : handle(std::exchange(other.handle, nullptr)) {}
                generator_t &operator=(generator_t &&other) &
                {
                    if (this == &other)
                        return;
                    if (handle) handle.destroy();
                    handle = std::exchange(other.handle, nullptr);
                    return *this;
                }
                ~generator_t()
                {
                    if (handle)
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

        namespace augmented_deque
        {
            using namespace language;
            using namespace concepts;
            using namespace utility;
            using namespace memory;
            using namespace iterator;
            using namespace coroutine;

            template<typename pointer_element_t, typename projected_storage_t>
            struct add_projected_storage_member_t
            {
                alignas(projected_storage_t) std::byte projected_storage_buffer[sizeof(projected_storage_t)]; // projected_storage_t projected_storage;
                typename std::pointer_traits<pointer_element_t>::template rebind<projected_storage_t> p_projected_storage() { return std::pointer_traits<typename std::pointer_traits<pointer_element_t>::template rebind<projected_storage_t>>::pointer_to(*reinterpret_cast<projected_storage_t *>(&projected_storage_buffer)); }
            };
            template<typename pointer_element_t>
            struct add_projected_storage_member_t<pointer_element_t, void>
            {};

            template<typename pointer_element_t, typename accumulated_storage_t, typename derived_t>
            struct add_accumulated_storage_member_t
            {
                alignas(accumulated_storage_t) std::byte accumulated_storage_buffer[sizeof(accumulated_storage_t)]; // accumulated_storage_t accumulated_storage;
                typename std::pointer_traits<pointer_element_t>::template rebind<accumulated_storage_t> p_accumulated_storage() { return std::pointer_traits<typename std::pointer_traits<pointer_element_t>::template rebind<accumulated_storage_t>>::pointer_to(*reinterpret_cast<accumulated_storage_t *>(&accumulated_storage_buffer)); }

                static typename std::pointer_traits<pointer_element_t>::template rebind<derived_t const> from_accumulated_storage_pointer(typename std::pointer_traits<pointer_element_t>::template rebind<accumulated_storage_t const> pointer) { return std::pointer_traits<typename std::pointer_traits<pointer_element_t>::template rebind<derived_t const>>::pointer_to(*reinterpret_cast<derived_t const *>(reinterpret_cast<std::byte const *>(std::to_address(pointer)) - offsetof(derived_t, accumulated_storage_buffer))); }
                static typename std::pointer_traits<pointer_element_t>::template rebind<derived_t> from_accumulated_storage_pointer(typename std::pointer_traits<pointer_element_t>::template rebind<accumulated_storage_t> pointer)
                {
#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Winvalid-offsetof"
#endif
                    return std::pointer_traits<typename std::pointer_traits<pointer_element_t>::template rebind<derived_t>>::pointer_to(*reinterpret_cast<derived_t *>(reinterpret_cast<std::byte *>(std::to_address(pointer)) - offsetof(derived_t, accumulated_storage_buffer)));
#ifdef __clang__
    #pragma clang diagnostic pop
#endif
                }
            };
            template<typename pointer_element_t, typename derived_t>
            struct add_accumulated_storage_member_t<pointer_element_t, void, derived_t>
            {};

            template<typename sequence_config_t>
            struct alignas(std::max({alignof(std::conditional_t<std::is_same_v<typename sequence_config_t::accumulated_storage_t, void>, std::byte, typename sequence_config_t::accumulated_storage_t>), alignof(typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<void>), static_cast<std::size_t>(0b10)})) tree_node_t : add_accumulated_storage_member_t<typename sequence_config_t::pointer_element_t, typename sequence_config_t::accumulated_storage_t, tree_node_t<sequence_config_t>>
            {
                typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<tree_node_t> parent = nullptr, child_left = nullptr, child_right = nullptr;
                tree_node_t(typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<tree_node_t> parent) : parent(parent) {}
            };

            template<typename allocator_element_t>
            struct alignas(std::max({alignof(typename std::pointer_traits<typename std::allocator_traits<allocator_element_t>::pointer>::template rebind<void>), static_cast<std::size_t>(0b10)})) circular_doubly_linked_list_node_navigator_t
            {
                using pointer_navigator_t = typename std::pointer_traits<typename std::allocator_traits<allocator_element_t>::pointer>::template rebind<circular_doubly_linked_list_node_navigator_t>;
                pointer_navigator_t prev, next;

                circular_doubly_linked_list_node_navigator_t() // initialize to node_end
                    : prev(tagged_ptr_bit0_setted(std::pointer_traits<pointer_navigator_t>::pointer_to(*this))),
                      next(tagged_ptr_bit0_setted(std::pointer_traits<pointer_navigator_t>::pointer_to(*this)))
                {}

                circular_doubly_linked_list_node_navigator_t(std::nullptr_t) : prev(nullptr), next(nullptr) {} // not initialized

                static void push_impl(pointer_navigator_t tagged_end, pointer_navigator_t(circular_doubly_linked_list_node_navigator_t::*prev), pointer_navigator_t(circular_doubly_linked_list_node_navigator_t::*next), pointer_navigator_t node_new)
                {
                    if (tagged_ptr_bit0_unsetted(tagged_end)->*prev == tagged_end && tagged_ptr_bit0_unsetted(tagged_end)->*next == tagged_end)
                    {
                        node_new->*prev = node_new->*next = tagged_end;
                        tagged_ptr_bit0_unsetted(tagged_end)->*prev = tagged_ptr_bit0_unsetted(tagged_end)->*next = tagged_ptr_bit0_setted(node_new);
                    }
                    else if (tagged_ptr_bit0_unsetted(tagged_end)->*prev != tagged_end && tagged_ptr_bit0_unsetted(tagged_end)->*next != tagged_end)
                    {
                        node_new->*prev = tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(tagged_end)->*prev);
                        node_new->*prev->*next = node_new;
                        node_new->*next = tagged_end;
                        tagged_ptr_bit0_unsetted(tagged_end)->*prev = tagged_ptr_bit0_setted(node_new);
                    }
                    else std::unreachable();
                }
                static void extract_impl(pointer_navigator_t node, pointer_navigator_t(circular_doubly_linked_list_node_navigator_t::*prev), pointer_navigator_t(circular_doubly_linked_list_node_navigator_t::*next))
                {
                    if (tagged_ptr_bit0_is_setted(node->*prev))
                        tagged_ptr_bit0_unsetted(node->*prev)->*next = tagged_ptr_bit0_setted_relaxed(node->*next);
                    else
                        node->*prev->*next = node->*next;
                    if (tagged_ptr_bit0_is_setted(node->*next))
                        tagged_ptr_bit0_unsetted(node->*next)->*prev = tagged_ptr_bit0_setted_relaxed(node->*prev);
                    else
                        node->*next->*prev = node->*prev;
                }

                static pointer_navigator_t untagged_prev_or_tagged_end(pointer_navigator_t tagged_end)
                {
                    if (pointer_navigator_t prev = tagged_ptr_bit0_unsetted(tagged_end)->prev; prev == tagged_end)
                        return prev;
                    else
                        return tagged_ptr_bit0_unsetted(prev);
                }

                static pointer_navigator_t untagged_next_or_tagged_end(pointer_navigator_t tagged_end)
                {
                    if (pointer_navigator_t next = tagged_ptr_bit0_unsetted(tagged_end)->next; next == tagged_end)
                        return next;
                    else
                        return tagged_ptr_bit0_unsetted(next);
                }

                template<typename node_end_t>
                struct node_end_functions_t
                {
                    using pointer_node_end_t = typename std::pointer_traits<typename std::allocator_traits<allocator_element_t>::pointer>::template rebind<node_end_t>;
                    static pointer_node_end_t create_tagged_end(allocator_element_t const &allocator_element) { return tagged_ptr_bit0_setted(new_expression<node_end_t>(allocator_element)); }
                };

                template<typename node_t>
                struct node_functions_t
                {
                    using pointer_node_t = typename std::pointer_traits<typename std::allocator_traits<allocator_element_t>::pointer>::template rebind<node_t>;
                    static pointer_node_t untagged_prev(pointer_navigator_t tagged_end) { return pointer_traits_static_cast<pointer_node_t>(tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(tagged_end)->prev)); }
                    static pointer_node_t untagged_next(pointer_navigator_t tagged_end) { return pointer_traits_static_cast<pointer_node_t>(tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(tagged_end)->next)); }
                };
            };

            template<typename sequence_config_t>
            struct list_node_end_t;

            template<typename sequence_config_t>
            struct digit_node_end_t : circular_doubly_linked_list_node_navigator_t<typename sequence_config_t::allocator_element_t>, circular_doubly_linked_list_node_navigator_t<typename sequence_config_t::allocator_element_t>::template node_end_functions_t<digit_node_end_t<sequence_config_t>>, add_accumulated_storage_member_t<typename sequence_config_t::pointer_element_t, typename sequence_config_t::accumulated_storage_t, digit_node_end_t<sequence_config_t>>
            {
                std::size_t node_count = 0;
                typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<list_node_end_t<sequence_config_t>> list_node_end = nullptr;

                typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<circular_doubly_linked_list_node_navigator_t<typename sequence_config_t::allocator_element_t>> middle = nullptr;
                typename sequence_config_t::projector_and_accumulator_t projector_and_accumulator;
            };
            template<typename sequence_config_t>
            struct digit_node_t : circular_doubly_linked_list_node_navigator_t<typename sequence_config_t::allocator_element_t>, circular_doubly_linked_list_node_navigator_t<typename sequence_config_t::allocator_element_t>::template node_functions_t<digit_node_t<sequence_config_t>>, add_accumulated_storage_member_t<typename std::allocator_traits<typename sequence_config_t::allocator_element_t>::pointer, typename sequence_config_t::accumulated_storage_t, digit_node_t<sequence_config_t>>
            {
                typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<tree_node_t<sequence_config_t>> tree_left = nullptr, tree_right = nullptr;
                std::size_t digit_position;

                digit_node_t(std::size_t digit_position) : circular_doubly_linked_list_node_navigator_t<typename sequence_config_t::allocator_element_t>(nullptr), digit_position(digit_position) {}
            };

            template<typename sequence_config_t>
            struct list_node_end_t : circular_doubly_linked_list_node_navigator_t<typename sequence_config_t::allocator_element_t>, circular_doubly_linked_list_node_navigator_t<typename sequence_config_t::allocator_element_t>::template node_end_functions_t<list_node_end_t<sequence_config_t>>
            {
                std::size_t node_count = 0, front_element_count = 0, back_element_count = 0;
                typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<digit_node_end_t<sequence_config_t>> digit_node_end = nullptr;

                typename sequence_config_t::actual_projected_storage_end_t actual_projected_storage_end;
            };
            template<typename sequence_config_t> requires (sequence_config_t::requested_stride == 1)
            struct list_node_end_t<sequence_config_t> : circular_doubly_linked_list_node_navigator_t<typename sequence_config_t::allocator_element_t>, circular_doubly_linked_list_node_navigator_t<typename sequence_config_t::allocator_element_t>::template node_end_functions_t<list_node_end_t<sequence_config_t>>
            {
                std::size_t node_count = 0;
                typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<digit_node_end_t<sequence_config_t>> digit_node_end = nullptr;

                typename sequence_config_t::actual_projected_storage_end_t actual_projected_storage_end;
            };
            template<typename sequence_config_t>
            struct list_node_t : circular_doubly_linked_list_node_navigator_t<typename sequence_config_t::allocator_element_t>, circular_doubly_linked_list_node_navigator_t<typename sequence_config_t::allocator_element_t>::template node_functions_t<list_node_t<sequence_config_t>>
            {
                typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<tree_node_t<sequence_config_t>> parent = nullptr;
                typename sequence_config_t::actual_projected_storage_t actual_projected_storage;

                using actual_projected_storage_t = typename sequence_config_t::actual_projected_storage_t;

                list_node_t(typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<tree_node_t<sequence_config_t>> parent) : parent(parent) {}
                template<typename sequence_config_t_ = sequence_config_t> requires (sequence_config_t_::sequence_index != 0)
                list_node_t(
                    typename std::pointer_traits<typename sequence_config_t_::pointer_element_t>::template rebind<tree_node_t<sequence_config_t_>> parent,
                    typename std::pointer_traits<typename sequence_config_t_::pointer_element_t>::template rebind<typename sequence_config_t_::stride1_sequence_t::list_node_t> p_stride1_sequence_list_node
                ) : parent(parent),
                    actual_projected_storage{
                        .child = p_stride1_sequence_list_node,
                    }
                {}
            };

            template<typename iterator_t> // https://stackoverflow.com/questions/6006614/c-static-polymorphism-crtp-and-using-typedefs-from-derived-classes
            struct const_iterator
            {};
            template<typename iterator_t>
            using const_iterator_t = typename const_iterator<iterator_t>::type;

            template<typename iterator_t> // https://stackoverflow.com/questions/6006614/c-static-polymorphism-crtp-and-using-typedefs-from-derived-classes
            struct non_const_iterator
            {};
            template<typename iterator_t>
            using non_const_iterator_t = typename non_const_iterator<iterator_t>::type;

            template<bool is_const_, typename sequence_config_t, typename derived_t>
            struct iterator_list_node_t
            {
                using navigator_t = circular_doubly_linked_list_node_navigator_t<typename sequence_config_t::allocator_element_t>;
                using list_node_t = list_node_t<sequence_config_t>;
                using list_node_end_t = list_node_end_t<sequence_config_t>;
                using tree_node_t = tree_node_t<sequence_config_t>;
                using digit_node_t = digit_node_t<sequence_config_t>;
                using digit_node_end_t = digit_node_end_t<sequence_config_t>;

                using pointer_navigator_t = typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<navigator_t>;
                using pointer_list_node_t = typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<list_node_t>;
                using pointer_list_node_end_t = typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<list_node_end_t>;
                using pointer_tree_node_t = typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<tree_node_t>;
                using pointer_digit_node_t = typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<digit_node_t>;
                using pointer_digit_node_end_t = typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<digit_node_end_t>;

                static pointer_list_node_t p_tree_node_to_p_list_node(pointer_tree_node_t p) { return pointer_traits_reinterpret_cast<pointer_list_node_t>(tagged_ptr_bit0_unsetted(p)); }
                static pointer_tree_node_t p_list_node_to_p_tree_node(pointer_list_node_t p) { return pointer_traits_reinterpret_cast<pointer_tree_node_t>(tagged_ptr_bit0_setted(p)); }
                static pointer_tree_node_t p_digit_node_to_p_tree_node(pointer_digit_node_t p) { return pointer_traits_reinterpret_cast<pointer_tree_node_t>(tagged_ptr_bit0_setted(p)); }
                static pointer_digit_node_t p_tree_node_to_p_digit_node(pointer_tree_node_t p) { return pointer_traits_reinterpret_cast<pointer_digit_node_t>(tagged_ptr_bit0_unsetted(p)); }

                static constexpr bool is_reversed_ = false;
                static constexpr pointer_navigator_t(navigator_t::*p_prev) = !is_reversed_ ? &navigator_t::prev : &navigator_t::next;
                static constexpr pointer_navigator_t(navigator_t::*p_next) = !is_reversed_ ? &navigator_t::next : &navigator_t::prev;
                static constexpr pointer_tree_node_t(digit_node_t::*p_tree_left) = !is_reversed_ ? &digit_node_t::tree_left : &digit_node_t::tree_right;
                static constexpr pointer_tree_node_t(digit_node_t::*p_tree_right) = !is_reversed_ ? &digit_node_t::tree_right : &digit_node_t::tree_left;
                static constexpr pointer_tree_node_t(tree_node_t::*p_child_left) = !is_reversed_ ? &tree_node_t::child_left : &tree_node_t::child_right;
                static constexpr pointer_tree_node_t(tree_node_t::*p_child_right) = !is_reversed_ ? &tree_node_t::child_right : &tree_node_t::child_left;

                pointer_navigator_t current_list_node = nullptr;
                iterator_list_node_t(pointer_navigator_t current_list_node) : current_list_node(current_list_node) {}
                bool is_end() const
                {
                    assert(current_list_node != nullptr);
                    return tagged_ptr_bit0_is_setted(current_list_node);
                }

                static constexpr bool is_const = is_const_;
                using non_const_iterator_t = non_const_iterator_t<derived_t>;
                using const_iterator_t = const_iterator_t<derived_t>;
                non_const_iterator_t to_non_const() const { return {current_list_node}; }
                const_iterator_t to_const() const { return {current_list_node}; }
                iterator_list_node_t(non_const_iterator_t const &rhs) requires (is_const) : current_list_node(rhs.current_list_node) {} // https://quuxplusone.github.io/blog/2018/12/01/const-iterator-antipatterns/
                const_iterator_t &operator=(non_const_iterator_t const &rhs) & requires (is_const)
                {
                    current_list_node = rhs.current_list_node;
                    return static_cast<const_iterator_t &>(*this);
                }

                // std::input_or_output_iterator / std::weakly_incrementable
                iterator_list_node_t(iterator_list_node_t const &) = default;
                iterator_list_node_t &operator=(iterator_list_node_t const &) & = default;
                using difference_type = std::ptrdiff_t;
                derived_t &operator++() &
                {
                    assert(current_list_node != nullptr);
                    if (tagged_ptr_bit0_is_setted(current_list_node))
                        current_list_node = navigator_t::untagged_next_or_tagged_end(current_list_node);
                    else
                        current_list_node = current_list_node->next;
                    return static_cast<derived_t &>(*this);
                }
                derived_t operator++(int) &
                {
                    derived_t temp = static_cast<derived_t &>(*this);
                    operator++();
                    return temp;
                }
                // std::forward_iterator / std::sentinel_for / std::semiregular, std::forward_iterator / std::incrementable / std::regular
                iterator_list_node_t() = default;

                struct demonstration_only_input_iterator_t
                {
                    // std::input_iterator / std::indirectly_readable
                    using value_type = typename sequence_config_t::actual_projected_storage_t;
                    using pointer = typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<conditional_const_t<is_const, value_type>>;
                    using reference = conditional_const_t<is_const, value_type> &;
                    reference operator*() const &
                    {
                        assert(this->current_list_node != nullptr);
                        assert(!tagged_ptr_bit0_is_setted(this->current_list_node));
                        return conditional_as_const<is_const>(static_cast<list_node_t *>(std::to_address(this->current_list_node))->actual_projected_storage);
                    }
                    pointer to_pointer() const & { return std::pointer_traits<pointer>::pointer_to(operator*()); }
                    pointer operator->() const & { return to_pointer(); }
                    static iterator_list_node_t from_actual_projected_storage_pointer(pointer ptr) { return {std::pointer_traits<pointer_navigator_t>::pointer_to(*reinterpret_cast<list_node_t *>(const_cast<std::byte *>(reinterpret_cast<conditional_const_t<is_const, std::byte> *>(std::to_address(ptr))) - offsetof(list_node_t, actual_projected_storage)))}; }
                };

                // std::forward_iterator / std::sentinel_for / __WeaklyEqualityComparableWith, std::forward_iterator / std::incrementable / std::regular
                template<std::bool_constant<is_const> * = nullptr> requires (is_const)
                friend bool operator==(const_iterator_t const &lhs, const_iterator_t const &rhs)
                {
                    assert((lhs.current_list_node != nullptr) == (rhs.current_list_node != nullptr));
                    return lhs.current_list_node == rhs.current_list_node;
                }
                template<std::bool_constant<is_const> * = nullptr> requires (!is_const)
                friend bool operator==(const_iterator_t const &lhs, non_const_iterator_t const &rhs) { return lhs == rhs.to_const(); }
                template<std::bool_constant<is_const> * = nullptr> requires (!is_const)
                friend bool operator==(non_const_iterator_t const &lhs, const_iterator_t const &rhs) { return lhs.to_const() == rhs; }
                template<std::bool_constant<is_const> * = nullptr> requires (!is_const)
                friend bool operator==(non_const_iterator_t const &lhs, non_const_iterator_t const &rhs) { return lhs.to_const() == rhs.to_const(); }
                friend bool operator==(derived_t const &lhs, [[maybe_unused]] std::default_sentinel_t const &rhs)
                {
                    assert(lhs.current_list_node != nullptr);
                    return tagged_ptr_bit0_is_setted(lhs.current_list_node);
                }

                // std::bidirectional_iterator
                derived_t &operator--() &
                {
                    assert(current_list_node != nullptr);
                    if (tagged_ptr_bit0_is_setted(current_list_node))
                        current_list_node = navigator_t::untagged_prev_or_tagged_end(current_list_node);
                    else
                        current_list_node = current_list_node->prev;
                    return static_cast<derived_t &>(*this);
                }
                derived_t operator--(int) &
                {
                    derived_t temp = static_cast<derived_t &>(*this);
                    operator--();
                    return temp;
                }

                // std::random_access_iterator / std::totally_ordered / __PartiallyOrderedWith
                template<std::bool_constant<is_const> * = nullptr> requires (is_const)
                friend std::strong_ordering operator<=>(const_iterator_t const &lhs, const_iterator_t const &rhs)
                {
                    assert((lhs.current_list_node != nullptr) == (rhs.current_list_node != nullptr));
                    if (lhs.current_list_node == rhs.current_list_node)
                        return std::strong_ordering::equal;
                    if (tagged_ptr_bit0_is_setted(lhs.current_list_node))
                        return std::strong_ordering::greater;
                    if (tagged_ptr_bit0_is_setted(rhs.current_list_node))
                        return std::strong_ordering::less;

                    pointer_tree_node_t tree_node_lhs = pointer_traits_reinterpret_cast<pointer_tree_node_t>(lhs.current_list_node);
                    pointer_tree_node_t tree_node_rhs = pointer_traits_reinterpret_cast<pointer_tree_node_t>(rhs.current_list_node);
                    pointer_tree_node_t tree_node_lhs_parent = pointer_traits_static_cast<pointer_list_node_t>(lhs.current_list_node)->parent;
                    pointer_tree_node_t tree_node_rhs_parent = pointer_traits_static_cast<pointer_list_node_t>(rhs.current_list_node)->parent;
                    auto same_digit_node_reached = [&tree_node_lhs, &tree_node_rhs, &tree_node_lhs_parent]() //
                    {
                        if (pointer_digit_node_t digit_node_parent = p_tree_node_to_p_digit_node(tree_node_lhs_parent);
                            tagged_ptr_bit0_unsetted_relaxed(digit_node_parent->tree_left) == tree_node_lhs && tagged_ptr_bit0_unsetted_relaxed(digit_node_parent->tree_right) == tree_node_rhs)
                            return std::strong_ordering::less;
                        else if (tagged_ptr_bit0_unsetted_relaxed(digit_node_parent->tree_left) == tree_node_rhs && tagged_ptr_bit0_unsetted_relaxed(digit_node_parent->tree_right) == tree_node_lhs)
                            return std::strong_ordering::greater;
                        else std::unreachable();
                    };
                    while (true)
                    {
                        if (tree_node_lhs_parent == tree_node_rhs_parent)
                        {
                            if (pointer_tree_node_t tree_node_parent = tree_node_lhs_parent; tagged_ptr_bit0_is_setted(tree_node_parent))
                                return same_digit_node_reached();
                            else
                            {
                                if (tagged_ptr_bit0_unsetted_relaxed(tree_node_parent->child_left) == tree_node_lhs && tagged_ptr_bit0_unsetted_relaxed(tree_node_parent->child_right) == tree_node_rhs)
                                    return std::strong_ordering::less;
                                else if (tagged_ptr_bit0_unsetted_relaxed(tree_node_parent->child_left) == tree_node_rhs && tagged_ptr_bit0_unsetted_relaxed(tree_node_parent->child_right) == tree_node_lhs)
                                    return std::strong_ordering::greater;
                                else std::unreachable();
                            }
                        }
                        else
                        {
                            if (tagged_ptr_bit0_is_setted(tree_node_lhs_parent))
                                break;
                            if (tagged_ptr_bit0_is_setted(tree_node_rhs_parent))
                                break;
                            tree_node_lhs = std::exchange(tree_node_lhs_parent, tree_node_lhs_parent->parent);
                            tree_node_rhs = std::exchange(tree_node_rhs_parent, tree_node_rhs_parent->parent);
                        }
                    }
                    while (!tagged_ptr_bit0_is_setted(tree_node_lhs_parent))
                        tree_node_lhs = std::exchange(tree_node_lhs_parent, tree_node_lhs_parent->parent);
                    while (!tagged_ptr_bit0_is_setted(tree_node_rhs_parent))
                        tree_node_rhs = std::exchange(tree_node_rhs_parent, tree_node_rhs_parent->parent);
                    if (tree_node_lhs_parent == tree_node_rhs_parent)
                        return same_digit_node_reached();
                    else
                    {
                        if (pointer_digit_node_t digit_node_lhs = p_tree_node_to_p_digit_node(tree_node_lhs_parent),
                            digit_node_rhs = p_tree_node_to_p_digit_node(tree_node_rhs_parent);
                            tagged_ptr_bit0_is_setted(digit_node_lhs->next) || pointer_traits_static_cast<pointer_digit_node_t>(digit_node_lhs->next)->digit_position < digit_node_lhs->digit_position)
                        {
                            if (tagged_ptr_bit0_is_setted(digit_node_rhs->next) || pointer_traits_static_cast<pointer_digit_node_t>(digit_node_rhs->next)->digit_position < digit_node_rhs->digit_position)
                                return digit_node_rhs->digit_position <=> digit_node_lhs->digit_position;
                            else
                                return std::strong_ordering::greater;
                        }
                        else
                        {
                            if (tagged_ptr_bit0_is_setted(digit_node_rhs->next) || pointer_traits_static_cast<pointer_digit_node_t>(digit_node_rhs->next)->digit_position < digit_node_rhs->digit_position)
                                return std::strong_ordering::less;
                            else
                                return digit_node_lhs->digit_position <=> digit_node_rhs->digit_position;
                        }
                    }
                }
                template<std::bool_constant<is_const> * = nullptr> requires (!is_const)
                friend std::strong_ordering operator<=>(const_iterator_t const &lhs, non_const_iterator_t const &rhs) { return lhs <=> rhs.to_const(); }
                template<std::bool_constant<is_const> * = nullptr> requires (!is_const)
                friend std::strong_ordering operator<=>(non_const_iterator_t const &lhs, const_iterator_t const &rhs) { return lhs.to_const() <=> rhs; }
                template<std::bool_constant<is_const> * = nullptr> requires (!is_const)
                friend std::strong_ordering operator<=>(non_const_iterator_t const &lhs, non_const_iterator_t const &rhs) { return lhs.to_const() <=> rhs.to_const(); }
                friend std::strong_ordering operator<=>(derived_t const &lhs, [[maybe_unused]] std::default_sentinel_t const &rhs)
                {
                    assert(lhs.current_list_node != nullptr);
                    if (tagged_ptr_bit0_is_setted(lhs.current_list_node))
                        return std::strong_ordering::equal;
                    else
                        return std::strong_ordering::less;
                }

                // std::random_access_iterator
                void move_impl(
                    std::size_t distance,
                    pointer_list_node_t (*p_untagged_prev)(pointer_navigator_t tagged_end), pointer_list_node_t (*p_untagged_next)(pointer_navigator_t tagged_end),
                    pointer_navigator_t(navigator_t::*p_prev), pointer_navigator_t(navigator_t::*p_next),
                    pointer_tree_node_t(digit_node_t::*p_tree_left), pointer_tree_node_t(digit_node_t::*p_tree_right),
                    pointer_tree_node_t(tree_node_t::*p_child_left), pointer_tree_node_t(tree_node_t::*p_child_right)
                ) &
                {
                    assert(distance != 0);
                    pointer_tree_node_t tree_node = pointer_traits_reinterpret_cast<pointer_tree_node_t>(current_list_node);
                    std::size_t index_in_tree_node = 0, tree_node_range_size = 1;
                    index_in_tree_node += distance;
                    auto find_inside_tree_node = [&]() //
                    {
                        while (tree_node_range_size != 1)
                        {
                            if (index_in_tree_node < tree_node_range_size / 2)
                                std::tie(index_in_tree_node, tree_node_range_size, tree_node) = std::forward_as_tuple(index_in_tree_node, tree_node_range_size / 2, tree_node->*p_child_left);
                            else
                                std::tie(index_in_tree_node, tree_node_range_size, tree_node) = std::forward_as_tuple(index_in_tree_node - tree_node_range_size / 2, tree_node_range_size / 2, tree_node->*p_child_right);
                        }
                        return p_tree_node_to_p_list_node(tree_node);
                    };
                    pointer_tree_node_t tree_node_parent = pointer_traits_static_cast<pointer_list_node_t>(current_list_node)->parent;
                    while (!tagged_ptr_bit0_is_setted(tree_node_parent))
                    {
                        if (tree_node == tagged_ptr_bit0_unsetted_relaxed(tree_node_parent->*p_child_left))
                        {
                            std::tie(index_in_tree_node, tree_node_range_size, tree_node) = std::forward_as_tuple(index_in_tree_node, tree_node_range_size * 2, std::exchange(tree_node_parent, tree_node_parent->parent));
                            if (index_in_tree_node < tree_node_range_size)
                            {
                                current_list_node = find_inside_tree_node();
                                return;
                            }
                        }
                        else if (tree_node == tagged_ptr_bit0_unsetted_relaxed(tree_node_parent->*p_child_right))
                            std::tie(index_in_tree_node, tree_node_range_size, tree_node) = std::forward_as_tuple(index_in_tree_node + tree_node_range_size, tree_node_range_size * 2, std::exchange(tree_node_parent, tree_node_parent->parent));
                        else std::unreachable();
                    }
                    pointer_digit_node_t digit_node = p_tree_node_to_p_digit_node(tree_node_parent);
                    while (true)
                    {
                        // advance to next tree, return if found
                        if (tree_node == tagged_ptr_bit0_unsetted_relaxed(digit_node->*p_tree_left) && digit_node->*p_tree_right != nullptr)
                        {
                            std::tie(index_in_tree_node, tree_node_range_size, tree_node) = std::forward_as_tuple(index_in_tree_node - tree_node_range_size, tree_node_range_size, tagged_ptr_bit0_unsetted_relaxed(digit_node->*p_tree_right));
                            if (index_in_tree_node < tree_node_range_size)
                            {
                                current_list_node = tree_node_range_size == 1 ? reinterpret_cast<list_node_t *>(tree_node) : find_inside_tree_node();
                                return;
                            }
                        }
                        else if ((tree_node == tagged_ptr_bit0_unsetted_relaxed(digit_node->*p_tree_left) && digit_node->*p_tree_right == nullptr) || tree_node == tagged_ptr_bit0_unsetted_relaxed(digit_node->*p_tree_right))
                        {
                            if (tagged_ptr_bit0_is_setted(digit_node->*p_next))
                            {
                                pointer_list_node_end_t list_node_end = pointer_traits_static_cast<pointer_digit_node_end_t>(tagged_ptr_bit0_unsetted(digit_node->*p_next))->list_node_end;
                                std::tie(index_in_tree_node, current_list_node) = std::forward_as_tuple(index_in_tree_node - tree_node_range_size, list_node_end);
                                index_in_tree_node %= static_cast<std::ptrdiff_t>(tagged_ptr_bit0_unsetted(list_node_end)->node_count) + 1;
                                if (index_in_tree_node != 0)
                                {
                                    std::tie(index_in_tree_node, current_list_node) = std::forward_as_tuple(index_in_tree_node - 1, (*p_untagged_next)(current_list_node));
                                    if (index_in_tree_node != 0)
                                        move_impl(index_in_tree_node, p_untagged_prev, p_untagged_next, p_prev, p_next, p_tree_left, p_tree_right, p_child_left, p_child_right);
                                }
                                return;
                            }
                            else
                            {
                                digit_node = pointer_traits_static_cast<pointer_digit_node_t>(digit_node->*p_next);
                                if (digit_node->*p_tree_left != nullptr)
                                    std::tie(index_in_tree_node, tree_node_range_size, tree_node) = std::forward_as_tuple(index_in_tree_node - tree_node_range_size, 1 << digit_node->digit_position, tagged_ptr_bit0_unsetted_relaxed(digit_node->*p_tree_left));
                                else if (digit_node->*p_tree_right != nullptr)
                                    std::tie(index_in_tree_node, tree_node_range_size, tree_node) = std::forward_as_tuple(index_in_tree_node - tree_node_range_size, 1 << digit_node->digit_position, tagged_ptr_bit0_unsetted_relaxed(digit_node->*p_tree_right));
                                else std::unreachable();
                                if (index_in_tree_node < tree_node_range_size)
                                {
                                    current_list_node = tree_node_range_size == 1 ? pointer_traits_reinterpret_cast<pointer_list_node_t>(tree_node) : find_inside_tree_node();
                                    return;
                                }
                            }
                        }
                        else std::unreachable();
                    }
                }
                derived_t &operator+=(std::ptrdiff_t offset) &
                {
                    assert(current_list_node != nullptr);
                    if (offset == 0) return static_cast<derived_t &>(*this);
                    if (tagged_ptr_bit0_is_setted(current_list_node))
                    {
                        offset %= static_cast<std::ptrdiff_t>(pointer_traits_static_cast<pointer_list_node_end_t>(tagged_ptr_bit0_unsetted(current_list_node))->node_count) + 1;
                        if (offset == 0) return static_cast<derived_t &>(*this);
                        else if (offset > 0)
                        {
                            current_list_node = list_node_t::untagged_next(current_list_node);
                            --offset;
                        }
                        else if (offset < 0)
                        {
                            current_list_node = list_node_t::untagged_prev(current_list_node);
                            ++offset;
                        }
                        else std::unreachable();
                    }
                    if (offset == 0) return static_cast<derived_t &>(*this);
                    else if (offset > 0)
                        move_impl(
                            offset,
                            &list_node_t::untagged_prev, &list_node_t::untagged_next,
                            &navigator_t::prev, &navigator_t::next,
                            &digit_node_t::tree_left, &digit_node_t::tree_right,
                            &tree_node_t::child_left, &tree_node_t::child_right
                        );
                    else if (offset < 0)
                        move_impl(
                            -offset,
                            &list_node_t::untagged_next, &list_node_t::untagged_prev,
                            &navigator_t::next, &navigator_t::prev,
                            &digit_node_t::tree_right, &digit_node_t::tree_left,
                            &tree_node_t::child_right, &tree_node_t::child_left
                        );
                    else std::unreachable();
                    return static_cast<derived_t &>(*this);
                }
                derived_t &operator-=(std::ptrdiff_t offset) &
                {
                    return operator+=(-offset);
                }
                derived_t operator+(std::ptrdiff_t offset) const &
                {
                    derived_t temp = static_cast<derived_t const &>(*this);
                    temp += offset;
                    return temp;
                }
                derived_t operator-(std::ptrdiff_t offset) const &
                {
                    return operator+(-offset);
                }
                friend derived_t operator+(std::ptrdiff_t offset, derived_t const &this_)
                {
                    return *this_ + offset;
                }
                struct demonstration_only_random_access_iterator_t
                {
                    typename demonstration_only_input_iterator_t::reference operator[](std::ptrdiff_t offset) const & { return *(*this + offset); }
                };

                std::tuple<std::size_t, pointer_list_node_end_t> index_impl() const &
                {
                    if (tagged_ptr_bit0_is_setted(current_list_node))
                        return {static_cast<pointer_list_node_end_t>(tagged_ptr_bit0_unsetted(current_list_node))->node_count, static_cast<pointer_list_node_end_t>(current_list_node)};
                    else
                    {
                        std::size_t index_in_tree_node = 0, tree_node_range_size = 1;

                        pointer_tree_node_t tree_node = pointer_traits_reinterpret_cast<pointer_tree_node_t>(current_list_node);
                        pointer_tree_node_t tree_node_parent = pointer_traits_static_cast<pointer_list_node_t>(current_list_node)->parent;
                        while (!tagged_ptr_bit0_is_setted(tree_node_parent))
                        {
                            if (tree_node == tagged_ptr_bit0_unsetted_relaxed(tree_node_parent->child_left))
                                std::tie(index_in_tree_node, tree_node_range_size, tree_node) = std::forward_as_tuple(index_in_tree_node, tree_node_range_size * 2, std::exchange(tree_node_parent, tree_node_parent->parent));
                            else if (tree_node == tagged_ptr_bit0_unsetted_relaxed(tree_node_parent->child_right))
                                std::tie(index_in_tree_node, tree_node_range_size, tree_node) = std::forward_as_tuple(index_in_tree_node + tree_node_range_size, tree_node_range_size * 2, std::exchange(tree_node_parent, tree_node_parent->parent));
                            else std::unreachable();
                        }
                        if (pointer_digit_node_t digit_node = p_tree_node_to_p_digit_node(tree_node_parent);
                            tagged_ptr_bit0_is_setted(digit_node->next) || pointer_traits_static_cast<pointer_digit_node_t>(digit_node->next)->digit_position < digit_node->digit_position)
                        {
                            if (tree_node == tagged_ptr_bit0_unsetted_relaxed(digit_node->tree_left) && digit_node->tree_right != nullptr)
                                std::tie(index_in_tree_node, tree_node_range_size) = std::forward_as_tuple(index_in_tree_node, tree_node_range_size + tree_node_range_size);
                            pointer_navigator_t digit_node_navigator = digit_node->next;
                            for (; !tagged_ptr_bit0_is_setted(digit_node_navigator); digit_node_navigator = digit_node_navigator->next)
                            {
                                digit_node = pointer_traits_static_cast<pointer_digit_node_t>(digit_node_navigator);
                                if (digit_node->tree_left != nullptr)
                                    std::tie(index_in_tree_node, tree_node_range_size) = std::forward_as_tuple(index_in_tree_node, tree_node_range_size + (zu(1) << digit_node->digit_position));
                                if (digit_node->tree_right != nullptr)
                                    std::tie(index_in_tree_node, tree_node_range_size) = std::forward_as_tuple(index_in_tree_node, tree_node_range_size + (zu(1) << digit_node->digit_position));
                            }
                            return {tagged_ptr_bit0_unsetted(pointer_traits_static_cast<pointer_digit_node_end_t>(tagged_ptr_bit0_unsetted(digit_node_navigator))->list_node_end)->node_count - (tree_node_range_size - index_in_tree_node), pointer_traits_static_cast<pointer_digit_node_end_t>(tagged_ptr_bit0_unsetted(digit_node_navigator))->list_node_end};
                        }
                        else
                        {
                            if (digit_node->tree_left != nullptr && tree_node == tagged_ptr_bit0_unsetted_relaxed(digit_node->tree_right))
                                std::tie(index_in_tree_node, tree_node_range_size) = std::forward_as_tuple(tree_node_range_size + index_in_tree_node, tree_node_range_size + tree_node_range_size);
                            pointer_navigator_t digit_node_navigator = digit_node->prev;
                            for (; !tagged_ptr_bit0_is_setted(digit_node_navigator); digit_node_navigator = digit_node_navigator->prev)
                            {
                                digit_node = pointer_traits_static_cast<pointer_digit_node_t>(digit_node_navigator);
                                if (digit_node->tree_right != nullptr)
                                    std::tie(index_in_tree_node, tree_node_range_size) = std::forward_as_tuple(index_in_tree_node + (zu(1) << digit_node->digit_position), tree_node_range_size + (zu(1) << digit_node->digit_position));
                                if (digit_node->tree_left != nullptr)
                                    std::tie(index_in_tree_node, tree_node_range_size) = std::forward_as_tuple(index_in_tree_node + (zu(1) << digit_node->digit_position), tree_node_range_size + (zu(1) << digit_node->digit_position));
                            }
                            return {index_in_tree_node, pointer_traits_static_cast<pointer_digit_node_end_t>(tagged_ptr_bit0_unsetted(digit_node_navigator))->list_node_end};
                        }
                    }
                }
                std::size_t index() const &
                {
                    assert(current_list_node != nullptr);
                    return std::get<0>(index_impl());
                }
                template<std::bool_constant<is_const> * = nullptr> requires (is_const)
                friend std::ptrdiff_t operator-(const_iterator_t const &lhs, const_iterator_t const &rhs)
                {
                    assert(lhs.current_list_node != nullptr);
                    assert(rhs.current_list_node != nullptr);
                    return static_cast<std::ptrdiff_t>(std::get<0>(lhs.index_impl())) - static_cast<std::ptrdiff_t>(std::get<0>(rhs.index_impl()));
                }
                template<std::bool_constant<is_const> * = nullptr> requires (!is_const)
                friend std::ptrdiff_t operator-(const_iterator_t const &lhs, non_const_iterator_t const &rhs) { return lhs - rhs.to_const(); }
                template<std::bool_constant<is_const> * = nullptr> requires (!is_const)
                friend std::ptrdiff_t operator-(non_const_iterator_t const &lhs, const_iterator_t const &rhs) { return lhs.to_const() - rhs; }
                template<std::bool_constant<is_const> * = nullptr> requires (!is_const)
                friend std::ptrdiff_t operator-(non_const_iterator_t const &lhs, non_const_iterator_t const &rhs) { return lhs.to_const() - rhs.to_const(); }
                friend std::ptrdiff_t operator-([[maybe_unused]] std::default_sentinel_t const &lhs, derived_t const &rhs)
                {
                    assert(rhs.current_list_node != nullptr);
                    auto [index, list_node_end] = rhs.index_impl();
                    return static_cast<std::ptrdiff_t>(tagged_ptr_bit0_unsetted(list_node_end)->node_count) - static_cast<std::ptrdiff_t>(index);
                }
                friend std::ptrdiff_t operator-(derived_t const &lhs, [[maybe_unused]] std::default_sentinel_t const &rhs)
                {
                    return -(rhs - lhs);
                }

                using iterator_concept = std::bidirectional_iterator_tag;
            };

            template<bool is_const, typename sequence_config_t>
            struct iterator_element_t : iterator_list_node_t<is_const, sequence_config_t, iterator_element_t<is_const, sequence_config_t>>
            {
                using base_t = iterator_list_node_t<is_const, sequence_config_t, iterator_element_t<is_const, sequence_config_t>>;
                using list_node_t = typename base_t::list_node_t;

                using base_t::base_t;
                using base_t::operator=;

                // std::input_iterator / std::indirectly_readable
                using value_type = typename sequence_config_t::element_t;
                using pointer = typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<conditional_const_t<is_const, value_type>>;
                using reference = conditional_const_t<is_const, value_type> &;
                reference operator*() const &
                {
                    assert(this->current_list_node != nullptr);
                    assert(!tagged_ptr_bit0_is_setted(this->current_list_node));
                    return conditional_as_const<is_const>(*static_cast<list_node_t *>(std::to_address(this->current_list_node))->actual_projected_storage.p_element());
                }
                pointer to_pointer() const & { return std::pointer_traits<pointer>::pointer_to(operator*()); }
                pointer operator->() const & { return to_pointer(); }
                static iterator_element_t from_element_pointer(pointer ptr)
                {
#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Winvalid-offsetof"
#endif
                    return {std::pointer_traits<typename base_t::pointer_navigator_t>::pointer_to(*reinterpret_cast<list_node_t *>(const_cast<std::byte *>(reinterpret_cast<conditional_const_t<is_const, std::byte> *>(std::to_address(ptr))) - offsetof(list_node_t, actual_projected_storage.element_buffer)))};
#ifdef __clang__
    #pragma clang diagnostic pop
#endif
                }

                // std::random_access_iterator
                conditional_const_t<is_const, value_type> &operator[](std::ptrdiff_t offset) const & { return *(*this + offset); }
            };
            template<bool is_const, typename sequence_config_t>
            struct const_iterator<iterator_element_t<is_const, sequence_config_t>> : std::type_identity<iterator_element_t<true, sequence_config_t>>
            {
            };
            template<bool is_const, typename sequence_config_t>
            struct non_const_iterator<iterator_element_t<is_const, sequence_config_t>> : std::type_identity<iterator_element_t<false, sequence_config_t>>
            {
            };

            template<bool is_const, typename sequence_config_t>
            struct iterator_projected_storage_t : iterator_list_node_t<is_const, sequence_config_t, iterator_projected_storage_t<is_const, sequence_config_t>>
            {
                using base_t = iterator_list_node_t<is_const, sequence_config_t, iterator_projected_storage_t<is_const, sequence_config_t>>;
                using list_node_t = typename base_t::list_node_t;

                using base_t::base_t;
                using base_t::operator=;

                // std::input_iterator / std::indirectly_readable
                using value_type = std::conditional_t<std::is_same_v<typename sequence_config_t::projected_storage_t, void>, std::ranges::dangling, typename sequence_config_t::projected_storage_t>;
                using pointer = typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<conditional_const_t<is_const, value_type>>;
                using reference = conditional_const_t<is_const, value_type> &;
                reference operator*() const &
                {
                    assert(this->current_list_node != nullptr);
                    assert(!tagged_ptr_bit0_is_setted(this->current_list_node));
                    if constexpr (std::is_same_v<typename sequence_config_t::projected_storage_t, void>)
                    {
                        static std::ranges::dangling s;
                        return s;
                    }
                    else
                        return conditional_as_const<is_const>(*static_cast<list_node_t *>(std::to_address(this->current_list_node))->actual_projected_storage.p_projected_storage());
                }
                pointer to_pointer() const & { return std::pointer_traits<pointer>::pointer_to(operator*()); }
                pointer operator->() const & { return to_pointer(); }
                static iterator_projected_storage_t from_projected_storage_pointer(pointer ptr)
                {
                    if constexpr (std::is_same_v<typename sequence_config_t::projected_storage_t, void>)
                        return {};
                    else
#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Winvalid-offsetof"
#endif
                        return {std::pointer_traits<typename base_t::pointer_navigator_t>::pointer_to(*reinterpret_cast<list_node_t *>(const_cast<std::byte *>(reinterpret_cast<conditional_const_t<is_const, std::byte> *>(std::to_address(ptr))) - offsetof(list_node_t, actual_projected_storage.projected_storage_buffer)))};
#ifdef __clang__
    #pragma clang diagnostic pop
#endif
                }

                // std::random_access_iterator
                conditional_const_t<is_const, value_type> &operator[](std::ptrdiff_t offset) const & { return *(*this + offset); }
            };
            template<bool is_const, typename sequence_config_t>
            struct const_iterator<iterator_projected_storage_t<is_const, sequence_config_t>> : std::type_identity<iterator_projected_storage_t<true, sequence_config_t>>
            {
            };
            template<bool is_const, typename sequence_config_t>
            struct non_const_iterator<iterator_projected_storage_t<is_const, sequence_config_t>> : std::type_identity<iterator_projected_storage_t<false, sequence_config_t>>
            {
            };

            template<bool is_reversed>
            struct tuple_identity_or_reversed_functor_t
            {
                template<typename tuple_t> requires (!is_reversed)
                auto operator()(tuple_t tuple) const { return tuple; }
                template<typename tuple_t> requires (is_reversed)
                auto operator()(tuple_t tuple) const
                {
                    return [&]<std::size_t... I>(std::index_sequence<I...>) { return std::tuple<std::tuple_element_t<std::tuple_size_v<tuple_t> - 1 - I, tuple_t>...>(std::get<std::tuple_size_v<tuple_t> - 1 - I>(tuple)...); }(std::make_index_sequence<std::tuple_size_v<tuple_t>>());
                }
            };

            template<bool is_const_, bool is_reversed_, typename allocator_element_t, typename sequence_t>
            struct sequence_functor_t
            {
                using navigator_t = circular_doubly_linked_list_node_navigator_t<typename sequence_t::sequence_config_t::allocator_element_t>;
                using digit_node_t = typename sequence_t::digit_node_t;
                using digit_node_end_t = typename sequence_t::digit_node_end_t;
                using tree_node_t = typename sequence_t::tree_node_t;
                using list_node_t = typename sequence_t::list_node_t;
                using list_node_end_t = typename sequence_t::list_node_end_t;
                using projected_storage_t = typename sequence_t::projected_storage_t;
                using accumulated_storage_t = typename sequence_t::accumulated_storage_t;

                using pointer_navigator_t = typename std::pointer_traits<typename sequence_t::sequence_config_t::pointer_element_t>::template rebind<navigator_t>;
                using pointer_list_node_t = typename std::pointer_traits<typename sequence_t::sequence_config_t::pointer_element_t>::template rebind<list_node_t>;
                using pointer_list_node_end_t = typename std::pointer_traits<typename sequence_t::sequence_config_t::pointer_element_t>::template rebind<list_node_end_t>;
                using pointer_tree_node_t = typename std::pointer_traits<typename sequence_t::sequence_config_t::pointer_element_t>::template rebind<tree_node_t>;
                using pointer_digit_node_t = typename std::pointer_traits<typename sequence_t::sequence_config_t::pointer_element_t>::template rebind<digit_node_t>;
                using pointer_digit_node_end_t = typename std::pointer_traits<typename sequence_t::sequence_config_t::pointer_element_t>::template rebind<digit_node_end_t>;

                static pointer_list_node_t p_tree_node_to_p_list_node(pointer_tree_node_t p) { return pointer_traits_reinterpret_cast<pointer_list_node_t>(tagged_ptr_bit0_unsetted(p)); }
                static pointer_tree_node_t p_list_node_to_p_tree_node(pointer_list_node_t p) { return pointer_traits_reinterpret_cast<pointer_tree_node_t>(tagged_ptr_bit0_setted(p)); }
                static pointer_tree_node_t p_digit_node_to_p_tree_node(pointer_digit_node_t p) { return pointer_traits_reinterpret_cast<pointer_tree_node_t>(tagged_ptr_bit0_setted(p)); }
                static pointer_digit_node_t p_tree_node_to_p_digit_node(pointer_tree_node_t p) { return pointer_traits_reinterpret_cast<pointer_digit_node_t>(tagged_ptr_bit0_unsetted(p)); }

                allocator_element_t const &allocator_element;
                conditional_const_t<is_const_, sequence_t> *sequence;
                static constexpr typename sequence_t::digit_proxy_t (sequence_t::*p_digit_front)(void) = !is_reversed_ ? static_cast<typename sequence_t::digit_proxy_t (sequence_t::*)(void)>(&sequence_t::digit_front) : static_cast<typename sequence_t::digit_proxy_t (sequence_t::*)(void)>(&sequence_t::digit_back);
                static constexpr typename sequence_t::digit_proxy_t (sequence_t::*p_digit_back)(void) = !is_reversed_ ? static_cast<typename sequence_t::digit_proxy_t (sequence_t::*)(void)>(&sequence_t::digit_back) : static_cast<typename sequence_t::digit_proxy_t (sequence_t::*)(void)>(&sequence_t::digit_front);
                static constexpr pointer_navigator_t(navigator_t::*p_prev) = !is_reversed_ ? &navigator_t::prev : &navigator_t::next;
                static constexpr pointer_navigator_t(navigator_t::*p_next) = !is_reversed_ ? &navigator_t::next : &navigator_t::prev;
                static constexpr pointer_tree_node_t(sequence_t::digit_node_t::*p_tree_left) = !is_reversed_ ? &digit_node_t::tree_left : &digit_node_t::tree_right;
                static constexpr pointer_tree_node_t(sequence_t::digit_node_t::*p_tree_right) = !is_reversed_ ? &digit_node_t::tree_right : &digit_node_t::tree_left;
                static constexpr pointer_tree_node_t(sequence_t::tree_node_t::*p_child_left) = !is_reversed_ ? &tree_node_t::child_left : &tree_node_t::child_right;
                static constexpr pointer_tree_node_t(sequence_t::tree_node_t::*p_child_right) = !is_reversed_ ? &tree_node_t::child_right : &tree_node_t::child_left;
                static constexpr tuple_identity_or_reversed_functor_t<is_reversed_> tuple_identity_or_reversed;

                struct next_or_prev_impl_t
                {
                    pointer_navigator_t(navigator_t::*p_next);
                    friend pointer_digit_node_t operator->*(pointer_digit_node_t digit_node, next_or_prev_impl_t const &next) { return pointer_traits_static_cast<pointer_digit_node_t>(digit_node->*next.p_next); }
                };

                void construct_list_node_projected_storage_if_exists(pointer_list_node_t list_node) const
                {
                    if constexpr (sequence_t::requested_stride == 1)
                    {
                        if constexpr (!std::is_same_v<projected_storage_t, void>)
                            sequence->projector_and_accumulator().construct_projected_storage(allocator_element, list_node->actual_projected_storage.p_projected_storage(), typename sequence_t::iterator_projected_storage_t{list_node}, typename sequence_t::iterator_element_t{list_node});
                    }
                    else
                    {
                        static_assert(!std::is_same_v<projected_storage_t, void>);
                        typename sequence_t::iterator_projected_storage_t it_list_node{list_node}, it_list_node_next = std::ranges::next(it_list_node);
                        typename sequence_t::stride1_sequence_t::iterator_element_t it_chunk_begin{list_node->actual_projected_storage.child},
                            it_chunk_end{it_list_node_next == sequence->end_projected_storage() ? pointer_traits_static_cast<pointer_navigator_t>(pointer_traits_static_cast<pointer_list_node_end_t>(tagged_ptr_bit0_unsetted(it_list_node_next.current_list_node))->actual_projected_storage_end.child) : pointer_traits_static_cast<pointer_navigator_t>(pointer_traits_static_cast<pointer_list_node_t>(it_list_node_next.current_list_node)->actual_projected_storage.child)};
                        std::size_t chunk_size = 1;
                        assert(it_chunk_end - it_chunk_begin == static_cast<std::ptrdiff_t>(chunk_size));
                        sequence->projector_and_accumulator().construct_projected_storage(allocator_element, list_node->actual_projected_storage.p_projected_storage(), it_list_node, it_chunk_begin, it_chunk_end, chunk_size);
                    }
                }
                void update_list_node_projected_storage_if_exists(pointer_list_node_t list_node) const
                {
                    if constexpr (sequence_t::requested_stride == 1)
                    {
                        if constexpr (!std::is_same_v<projected_storage_t, void>)
                            sequence->projector_and_accumulator().update_projected_storage(*list_node->actual_projected_storage.p_projected_storage(), typename sequence_t::iterator_projected_storage_t{list_node}, typename sequence_t::iterator_element_t{list_node});
                    }
                    else
                    {
                        static_assert(!std::is_same_v<projected_storage_t, void>);
                        typename sequence_t::iterator_projected_storage_t it_list_node{list_node}, it_list_node_next = std::ranges::next(it_list_node);
                        typename sequence_t::stride1_sequence_t::iterator_element_t it_chunk_begin{list_node->actual_projected_storage.child},
                            it_chunk_end{it_list_node_next == sequence->end_projected_storage() ? pointer_traits_static_cast<pointer_navigator_t>(pointer_traits_static_cast<pointer_list_node_end_t>(tagged_ptr_bit0_unsetted(it_list_node_next.current_list_node))->actual_projected_storage_end.child) : pointer_traits_static_cast<pointer_navigator_t>(pointer_traits_static_cast<pointer_list_node_t>(it_list_node_next.current_list_node)->actual_projected_storage.child)};
                        std::size_t chunk_size;
                        if (it_list_node == sequence->begin_projected_storage())
                            chunk_size = sequence->list_front_element_count();
                        else if (it_list_node_next == sequence->end_projected_storage())
                            chunk_size = sequence->list_back_element_count();
                        else
                            chunk_size = sequence->stride;
                        assert(it_chunk_end - it_chunk_begin == static_cast<std::ptrdiff_t>(chunk_size));
                        sequence->projector_and_accumulator().update_projected_storage(*list_node->actual_projected_storage.p_projected_storage(), it_list_node, it_chunk_begin, it_chunk_end, chunk_size);
                    }
                }
                void update_or_construct_tree_node_accumulated_storage_if_exists(pointer_tree_node_t tree_node, bool construct = false) const
                {
                    if constexpr (!std::is_same_v<accumulated_storage_t, void>)
                    {
                        auto get_left_operand = [&](auto return_accumulated_tuple) //
                        { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> void //
                          {
                              if (tagged_ptr_bit0_is_setted(tree_node->*p_child_left))
                                  return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::cref(*p_tree_node_to_p_list_node(tree_node->*p_child_left)->actual_projected_storage.p_projected_storage_or_p_element()))));
                              else
                                  return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*(tree_node->*p_child_left)->p_accumulated_storage()))));
                          }; };
                        auto get_right_operand = [&](auto return_accumulated_tuple) //
                        { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> void //
                          {
                              if (tagged_ptr_bit0_is_setted(tree_node->*p_child_right))
                                  return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::cref(*p_tree_node_to_p_list_node(tree_node->*p_child_right)->actual_projected_storage.p_projected_storage_or_p_element()))));
                              else
                                  return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*(tree_node->*p_child_right)->p_accumulated_storage()))));
                          }; };
                        auto return_accumulated_tuple = [&](auto accumulated_tuple_so_far) //
                        {
                            if (construct)
                                sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, tree_node->p_accumulated_storage(), tuple_identity_or_reversed(accumulated_tuple_so_far));
                            else
                                sequence->projector_and_accumulator().update_accumulated_storage(*tree_node->p_accumulated_storage(), tuple_identity_or_reversed(accumulated_tuple_so_far));
                        };
                        get_left_operand(get_right_operand(return_accumulated_tuple))(std::make_tuple());
                    }
                }
                void update_digit_node_end_accumulated_storage_if_exists(pointer_digit_node_t digit_front, pointer_digit_node_t digit_back) const
                {
                    if constexpr (!std::is_same_v<accumulated_storage_t, void>)
                    {
                        if (sequence->digit_middle() == tagged_ptr_bit0_unsetted(sequence->digit_node_end))
                            sequence->projector_and_accumulator().update_accumulated_storage(*tagged_ptr_bit0_unsetted(sequence->digit_node_end)->p_accumulated_storage(), std::make_tuple());
                        else
                        {
                            auto get_left_operand = [&](auto return_accumulated_tuple) //
                            { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> void //
                              {
                                  if (digit_front != sequence->digit_middle())
                                      return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*digit_front->p_accumulated_storage()))));
                                  else
                                  {
                                      if (digit_front->*p_tree_left != nullptr)
                                      {
                                          if (digit_front->digit_position != 0)
                                              return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*(digit_front->*p_tree_left)->p_accumulated_storage()))));
                                          else
                                              return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::cref(*p_tree_node_to_p_list_node(digit_front->*p_tree_left)->actual_projected_storage.p_projected_storage_or_p_element()))));
                                      }
                                      else
                                          return_accumulated_tuple(accumulated_tuple_so_far);
                                  }
                              }; };
                            auto get_right_operand = [&](auto return_accumulated_tuple) //
                            { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> void //
                              {
                                  if (sequence->digit_middle() != digit_back)
                                      return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*digit_back->p_accumulated_storage()))));
                                  else
                                  {
                                      if (digit_back->*p_tree_right != nullptr)
                                      {
                                          if (digit_back->digit_position != 0)
                                              return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*(digit_back->*p_tree_right)->p_accumulated_storage()))));
                                          else
                                              return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::cref(*p_tree_node_to_p_list_node(digit_back->*p_tree_right)->actual_projected_storage.p_projected_storage_or_p_element()))));
                                      }
                                      else
                                          return_accumulated_tuple(accumulated_tuple_so_far);
                                  }
                              }; };
                            auto return_accumulated_tuple = [&](auto accumulated_tuple_so_far) { sequence->projector_and_accumulator().update_accumulated_storage(*tagged_ptr_bit0_unsetted(sequence->digit_node_end)->p_accumulated_storage(), tuple_identity_or_reversed(accumulated_tuple_so_far)); };
                            get_left_operand(get_right_operand(return_accumulated_tuple))(std::make_tuple());
                        }
                    }
                }
                void update_or_construct_digit_node_right_accumulated_storage_if_exists(pointer_digit_node_t digit_node_right, bool construct = false) const
                {
                    if constexpr (!std::is_same_v<accumulated_storage_t, void>)
                    {
                        auto get_left_operand = [&](auto return_accumulated_tuple) //
                        { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> void //
                          {
                              next_or_prev_impl_t next [[maybe_unused]]{p_next}, prev{p_prev};
                              if (sequence->digit_middle() != digit_node_right->*p_prev)
                                  return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*(digit_node_right->*prev)->p_accumulated_storage()))));
                              else
                              {
                                  if (digit_node_right->*prev->*p_tree_right != nullptr)
                                      return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*(digit_node_right->*prev->*p_tree_right)->p_accumulated_storage()))));
                                  else
                                      return_accumulated_tuple(accumulated_tuple_so_far);
                              }
                          }; };
                        auto get_middle_operand = [&](auto return_accumulated_tuple) //
                        { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> void //
                          {
                              if (digit_node_right->*p_tree_left != nullptr)
                              {
                                  if (digit_node_right->digit_position != 0)
                                      return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*(digit_node_right->*p_tree_left)->p_accumulated_storage()))));
                                  else
                                      return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::cref(*p_tree_node_to_p_list_node(digit_node_right->*p_tree_left)->actual_projected_storage.p_projected_storage_or_p_element()))));
                              }
                              else
                                  return_accumulated_tuple(accumulated_tuple_so_far);
                          }; };
                        auto get_right_operand = [&](auto return_accumulated_tuple) //
                        { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> void //
                          {
                              if (digit_node_right->*p_tree_right != nullptr)
                              {
                                  if (digit_node_right->digit_position != 0)
                                      return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*(digit_node_right->*p_tree_right)->p_accumulated_storage()))));
                                  else
                                      return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::cref(*p_tree_node_to_p_list_node(digit_node_right->*p_tree_right)->actual_projected_storage.p_projected_storage_or_p_element()))));
                              }
                              else
                                  return_accumulated_tuple(accumulated_tuple_so_far);
                          }; };
                        auto return_accumulated_tuple = [&](auto accumulated_tuple_so_far) //
                        {
                            if (construct)
                                sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, digit_node_right->p_accumulated_storage(), tuple_identity_or_reversed(accumulated_tuple_so_far));
                            else
                                sequence->projector_and_accumulator().update_accumulated_storage(*digit_node_right->p_accumulated_storage(), tuple_identity_or_reversed(accumulated_tuple_so_far));
                        };
                        get_left_operand(get_middle_operand(get_right_operand(return_accumulated_tuple)))(std::make_tuple());
                    }
                }
                void update_digit_node_left_accumulated_storage_if_exists(pointer_digit_node_t digit_node_left) const
                {
                    if constexpr (!std::is_same_v<accumulated_storage_t, void>)
                    {
                        auto get_left_operand = [&](auto return_accumulated_tuple) //
                        { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> void //
                          {
                              if (digit_node_left->*p_tree_left != nullptr)
                              {
                                  if (digit_node_left->digit_position != 0)
                                      return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*(digit_node_left->*p_tree_left)->p_accumulated_storage()))));
                                  else
                                      return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::cref(*p_tree_node_to_p_list_node(digit_node_left->*p_tree_left)->actual_projected_storage.p_projected_storage_or_p_element()))));
                              }
                              else
                                  return_accumulated_tuple(accumulated_tuple_so_far);
                          }; };
                        auto get_middle_operand = [&](auto return_accumulated_tuple) //
                        { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> void //
                          {
                              if (digit_node_left->*p_tree_right != nullptr)
                              {
                                  if (digit_node_left->digit_position != 0)
                                      return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*(digit_node_left->*p_tree_right)->p_accumulated_storage()))));
                                  else
                                      return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::cref(*p_tree_node_to_p_list_node(digit_node_left->*p_tree_right)->actual_projected_storage.p_projected_storage_or_p_element()))));
                              }
                              else
                                  return_accumulated_tuple(accumulated_tuple_so_far);
                          }; };
                        auto get_right_operand = [&](auto return_accumulated_tuple) //
                        { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> void //
                          {
                              next_or_prev_impl_t next{p_next}, prev [[maybe_unused]]{p_prev};
                              if (sequence->digit_middle() != digit_node_left->*p_next)
                                  return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*(digit_node_left->*next)->p_accumulated_storage()))));
                              else
                              {
                                  if (digit_node_left->*next->*p_tree_left != nullptr)
                                      return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*(digit_node_left->*next->*p_tree_left)->p_accumulated_storage()))));
                                  else
                                      return_accumulated_tuple(accumulated_tuple_so_far);
                              }
                          }; };
                        auto return_accumulated_tuple = [&](auto accumulated_tuple_so_far) { sequence->projector_and_accumulator().update_accumulated_storage(*digit_node_left->p_accumulated_storage(), tuple_identity_or_reversed(accumulated_tuple_so_far)); };
                        get_left_operand(get_middle_operand(get_right_operand(return_accumulated_tuple)))(std::make_tuple());
                    }
                }
                void push_impl(invocable_r<pointer_list_node_t, pointer_tree_node_t /*parent*/> auto push_back_and_get_p_list_node) const
                {
                    next_or_prev_impl_t next [[maybe_unused]]{p_next}, prev{p_prev};
                    pointer_digit_node_t digit_back = pointer_traits_static_cast<pointer_digit_node_t>(static_cast<pointer_navigator_t>((sequence->*p_digit_back)()));
                    pointer_digit_node_t digit_front = pointer_traits_static_cast<pointer_digit_node_t>(static_cast<pointer_navigator_t>((sequence->*p_digit_front)()));
                    {
                        if (sequence->list_node_count() == 0) // 0 -> 1
                        {
                            assert(digit_front == pointer_traits_static_cast<pointer_navigator_t>(tagged_ptr_bit0_unsetted(sequence->digit_node_end)));
                            assert(sequence->digit_middle() == tagged_ptr_bit0_unsetted(sequence->digit_node_end));
                            assert(digit_back == pointer_traits_static_cast<pointer_navigator_t>(tagged_ptr_bit0_unsetted(sequence->digit_node_end)));

                            sequence->digit_middle() = digit_front = digit_back = new_expression<digit_node_t>(allocator_element, 0);
                            navigator_t::push_impl(sequence->digit_node_end, p_prev, p_next, digit_back);

                            pointer_list_node_t list_node_back;
                            digit_back->*p_tree_left = p_list_node_to_p_tree_node(list_node_back = push_back_and_get_p_list_node(p_digit_node_to_p_tree_node(digit_back)));

                            construct_list_node_projected_storage_if_exists(list_node_back);
                            // digit_node_back_accumulated_storage update skipped because it's digit_middle
                            update_digit_node_end_accumulated_storage_if_exists(digit_front, digit_back);
                        }
                        else
                        {
                            if (digit_back->digit_position != 0) // .+0 -> .+1
                            {
                                digit_back = new_expression<digit_node_t>(allocator_element, 0);
                                navigator_t::push_impl(sequence->digit_node_end, p_prev, p_next, digit_back);

                                pointer_list_node_t list_node_back;
                                digit_back->*p_tree_left = p_list_node_to_p_tree_node(list_node_back = push_back_and_get_p_list_node(p_digit_node_to_p_tree_node(digit_back)));

                                construct_list_node_projected_storage_if_exists(list_node_back);
                                update_or_construct_digit_node_right_accumulated_storage_if_exists(digit_back, true);
                                update_digit_node_end_accumulated_storage_if_exists(digit_front, digit_back);
                            }
                            else
                            {
                                if (digit_back->*p_tree_right == nullptr) // .*1 -> .*2
                                {
                                    pointer_list_node_t list_node_back;
                                    digit_back->*p_tree_right = p_list_node_to_p_tree_node(list_node_back = push_back_and_get_p_list_node(p_digit_node_to_p_tree_node(digit_back)));

                                    construct_list_node_projected_storage_if_exists(list_node_back);
                                    if (sequence->digit_middle() != digit_back)
                                        update_or_construct_digit_node_right_accumulated_storage_if_exists(digit_back);
                                    update_digit_node_end_accumulated_storage_if_exists(digit_front, digit_back);
                                }
                                else if (digit_back->*p_tree_left == nullptr) // .*1 -> .*2
                                {
                                    pointer_list_node_t list_node_back;
                                    digit_back->*p_tree_left = std::exchange(digit_back->*p_tree_right, p_list_node_to_p_tree_node(list_node_back = push_back_and_get_p_list_node(p_digit_node_to_p_tree_node(digit_back))));

                                    construct_list_node_projected_storage_if_exists(list_node_back);
                                    if (sequence->digit_middle() != digit_back)
                                        update_or_construct_digit_node_right_accumulated_storage_if_exists(digit_back);
                                    update_digit_node_end_accumulated_storage_if_exists(digit_front, digit_back);
                                }
                                else // .*[01]2+ -> .*[12]1+
                                {
                                    digit_back = new_expression<digit_node_t>(allocator_element, 0);
                                    navigator_t::push_impl(sequence->digit_node_end, p_prev, p_next, digit_back);

                                    pointer_list_node_t list_node_back;
                                    digit_back->*p_tree_left = p_list_node_to_p_tree_node(list_node_back = push_back_and_get_p_list_node(p_digit_node_to_p_tree_node(digit_back)));
                                    construct_list_node_projected_storage_if_exists(list_node_back);
                                    pointer_digit_node_t p_digit_node_whose_digit_position_is_to_be_raised = digit_back->*prev;
                                    bool should_update_left_accumulated_storage, should_update_self_accumulated_storage, should_update_right_accumulated_storage;
                                    while (true)
                                    {
                                        pointer_tree_node_t tree_root = new_expression<tree_node_t>(allocator_element, p_digit_node_to_p_tree_node(p_digit_node_whose_digit_position_is_to_be_raised));
                                        tree_root->*p_child_left = p_digit_node_whose_digit_position_is_to_be_raised->*p_tree_left;
                                        tree_root->*p_child_right = p_digit_node_whose_digit_position_is_to_be_raised->*p_tree_right;
                                        if (p_digit_node_whose_digit_position_is_to_be_raised->digit_position == 0)
                                            p_tree_node_to_p_list_node(tree_root->*p_child_left)->parent = p_tree_node_to_p_list_node(tree_root->*p_child_right)->parent = tree_root;
                                        else
                                            (tree_root->*p_child_left)->parent = (tree_root->*p_child_right)->parent = tree_root;
                                        update_or_construct_tree_node_accumulated_storage_if_exists(tree_root, true);
                                        p_digit_node_whose_digit_position_is_to_be_raised->*p_tree_left = tree_root;
                                        p_digit_node_whose_digit_position_is_to_be_raised->*p_tree_right = nullptr;
                                        ++p_digit_node_whose_digit_position_is_to_be_raised->digit_position;
                                        if (p_digit_node_whose_digit_position_is_to_be_raised == sequence->digit_middle())
                                        {
                                            should_update_left_accumulated_storage = true, should_update_self_accumulated_storage = false, should_update_right_accumulated_storage = true;
                                            break;
                                        }
                                        assert(p_digit_node_whose_digit_position_is_to_be_raised->*p_prev != sequence->digit_node_end);
                                        if ((p_digit_node_whose_digit_position_is_to_be_raised->*prev)->digit_position != p_digit_node_whose_digit_position_is_to_be_raised->digit_position)
                                        {
                                            should_update_left_accumulated_storage = false, should_update_self_accumulated_storage = true, should_update_right_accumulated_storage = true;
                                            break;
                                        }
                                        if (int digit = (p_digit_node_whose_digit_position_is_to_be_raised->*prev->*p_tree_left != nullptr) + (p_digit_node_whose_digit_position_is_to_be_raised->*prev->*p_tree_right != nullptr); digit == 2)
                                            p_digit_node_whose_digit_position_is_to_be_raised = p_digit_node_whose_digit_position_is_to_be_raised->*prev;
                                        else if (digit == 1) // .*12+ -> .*21+
                                        {
                                            should_update_self_accumulated_storage = (p_digit_node_whose_digit_position_is_to_be_raised->*p_prev != sequence->digit_middle()), should_update_right_accumulated_storage = true;
                                            if (p_digit_node_whose_digit_position_is_to_be_raised->*prev->*p_tree_right == nullptr)
                                            {
                                                should_update_left_accumulated_storage = false;
                                                p_digit_node_whose_digit_position_is_to_be_raised->*prev->*p_tree_right = tree_root;
                                            }
                                            else if (p_digit_node_whose_digit_position_is_to_be_raised->*prev->*p_tree_left == nullptr)
                                            {
                                                should_update_left_accumulated_storage = sequence->digit_middle() == p_digit_node_whose_digit_position_is_to_be_raised->*prev;
                                                p_digit_node_whose_digit_position_is_to_be_raised->*prev->*p_tree_left = std::exchange(p_digit_node_whose_digit_position_is_to_be_raised->*prev->*p_tree_right, tree_root);
                                            }
                                            else std::unreachable();
                                            tree_root->parent = p_digit_node_to_p_tree_node(p_digit_node_whose_digit_position_is_to_be_raised->*prev);
                                            navigator_t::extract_impl(p_digit_node_whose_digit_position_is_to_be_raised, p_prev, p_next);
                                            delete_expression<digit_node_t>(allocator_element, std::exchange(p_digit_node_whose_digit_position_is_to_be_raised, p_digit_node_whose_digit_position_is_to_be_raised->*prev));
                                            break;
                                        }
                                        else std::unreachable();
                                    }
                                    if (should_update_self_accumulated_storage)
                                        update_or_construct_digit_node_right_accumulated_storage_if_exists(p_digit_node_whose_digit_position_is_to_be_raised);
                                    if (should_update_left_accumulated_storage)
                                    {
                                        for (pointer_navigator_t digit_node_navigator = p_digit_node_whose_digit_position_is_to_be_raised->*p_prev; digit_node_navigator != sequence->digit_node_end; digit_node_navigator = digit_node_navigator->*p_prev)
                                        {
                                            pointer_digit_node_t digit_node_left = pointer_traits_static_cast<pointer_digit_node_t>(digit_node_navigator);
                                            update_digit_node_left_accumulated_storage_if_exists(digit_node_left);
                                        }
                                    }
                                    if (should_update_right_accumulated_storage)
                                    {
                                        for (pointer_navigator_t digit_node_navigator = p_digit_node_whose_digit_position_is_to_be_raised->*p_next; digit_node_navigator != sequence->digit_node_end; digit_node_navigator = digit_node_navigator->*p_next)
                                        {
                                            pointer_digit_node_t digit_node_right = pointer_traits_static_cast<pointer_digit_node_t>(digit_node_navigator);
                                            update_or_construct_digit_node_right_accumulated_storage_if_exists(digit_node_right, digit_back == digit_node_right);
                                        }
                                    }
                                    update_digit_node_end_accumulated_storage_if_exists(digit_front, digit_back);
                                }
                            }
                        }
                    }
                    (sequence->*p_digit_back)() = digit_back;
                    (sequence->*p_digit_front)() = digit_front;
                }
                void pop_impl(std::invocable<> auto pop_list_node) const
                {
                    next_or_prev_impl_t next{p_next}, prev{p_prev};
                    assert(sequence->list_node_count() != 0); // 0
                    pop_list_node();
                    pointer_digit_node_t const digit_back_const = pointer_traits_static_cast<pointer_digit_node_t>(static_cast<pointer_navigator_t>((sequence->*p_digit_back)()));
                    pointer_digit_node_t const digit_front_const [[maybe_unused]] = pointer_traits_static_cast<pointer_digit_node_t>(static_cast<pointer_navigator_t>((sequence->*p_digit_front)()));
                    if (digit_back_const->digit_position == 0) // .*[12] -> .*[01]
                    {
                        auto delete_digit_position0 = [&]() //
                        {
                            if (digit_back_const == sequence->digit_middle())
                            {
                                assert(digit_front_const == sequence->digit_middle());
                                assert(digit_back_const->*p_prev == sequence->digit_node_end);
                                assert(digit_back_const->*p_next == sequence->digit_node_end);
                                assert(tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(sequence->digit_node_end)->*p_prev) == digit_back_const);
                                assert(tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(sequence->digit_node_end)->*p_next) == digit_back_const);
                                navigator_t::extract_impl(digit_back_const, p_prev, p_next);
                                delete_expression<digit_node_t>(allocator_element, digit_back_const);
                                sequence->digit_middle() = (sequence->*p_digit_front)() = (sequence->*p_digit_back)() = tagged_ptr_bit0_unsetted(sequence->digit_node_end);
                            }
                            else
                            {
                                (sequence->*p_digit_back)() = digit_back_const->*prev;
                                navigator_t::extract_impl(digit_back_const, p_prev, p_next);
                                delete_expression<digit_node_t>(allocator_element, digit_back_const);
                            }
                        };
                        if (digit_back_const->*p_tree_right == nullptr || digit_back_const->*p_tree_left == nullptr)
                            delete_digit_position0();
                        else
                        {
                            digit_back_const->*p_tree_right = nullptr;
                            if (sequence->digit_middle() != digit_back_const)
                                update_or_construct_digit_node_right_accumulated_storage_if_exists(digit_back_const);
                        }
                        update_digit_node_end_accumulated_storage_if_exists(pointer_traits_static_cast<pointer_digit_node_t>(static_cast<pointer_navigator_t>((sequence->*p_digit_front)())), pointer_traits_static_cast<pointer_digit_node_t>(static_cast<pointer_navigator_t>((sequence->*p_digit_back)())));
                    }
                    else // .*[12]0+ -> .*[01]1+
                    {
                        pointer_digit_node_t digit_back = pointer_traits_static_cast<pointer_digit_node_t>(static_cast<pointer_navigator_t>((sequence->*p_digit_back)()));
                        pointer_digit_node_t digit_front = pointer_traits_static_cast<pointer_digit_node_t>(static_cast<pointer_navigator_t>((sequence->*p_digit_front)()));
                        pointer_tree_node_t p_tree_to_be_broken_down;
                        bool will_delete_digit_back;
                        if (digit_back->*p_tree_right == nullptr)
                            std::tie(will_delete_digit_back, p_tree_to_be_broken_down) = std::forward_as_tuple(true, digit_back->*p_tree_left);
                        else if (digit_back->*p_tree_left == nullptr)
                            std::tie(will_delete_digit_back, p_tree_to_be_broken_down) = std::forward_as_tuple(true, digit_back->*p_tree_right);
                        else
                            std::tie(will_delete_digit_back, p_tree_to_be_broken_down) = std::forward_as_tuple(false, std::exchange(digit_back->*p_tree_right, nullptr));
                        pointer_digit_node_t current_digit_back = digit_back;
                        for (std::ptrdiff_t current_digit_position = digit_back->digit_position - 1; current_digit_position != -1; --current_digit_position)
                        {
                            current_digit_back->*p_next = new_expression<digit_node_t>(allocator_element, static_cast<std::size_t>(current_digit_position));
                            current_digit_back->*p_next->*p_prev = current_digit_back;
                            current_digit_back->*next->*p_tree_left = p_tree_to_be_broken_down->*p_child_left;
                            current_digit_back->*next->*p_tree_right = nullptr;
                            current_digit_back = current_digit_back->*next;
                            if (current_digit_back->digit_position == 0)
                                p_tree_node_to_p_list_node(current_digit_back->*p_tree_left)->parent = p_digit_node_to_p_tree_node(current_digit_back);
                            else
                                (current_digit_back->*p_tree_left)->parent = p_digit_node_to_p_tree_node(current_digit_back);
                            if constexpr (!std::is_same_v<accumulated_storage_t, void>)
                                sequence->projector_and_accumulator().destroy_accumulated_storage(allocator_element, p_tree_to_be_broken_down->p_accumulated_storage());
                            delete_expression<tree_node_t>(allocator_element, std::exchange(p_tree_to_be_broken_down, p_tree_to_be_broken_down->*p_child_right));
                        }
                        current_digit_back->*p_next = sequence->digit_node_end;
                        tagged_ptr_bit0_unsetted(sequence->digit_node_end)->*p_prev = tagged_ptr_bit0_setted(current_digit_back);
                        auto update_accumulated_storage = [this](pointer_digit_node_t digit_node, bool should_update_left_accumulated_storage, bool should_update_self_accumulated_storage, bool should_update_right_accumulated_storage) //
                        {
                            if (should_update_self_accumulated_storage)
                                update_or_construct_digit_node_right_accumulated_storage_if_exists(digit_node);
                            if (should_update_left_accumulated_storage)
                            {
                                for (pointer_navigator_t digit_node_navigator = digit_node->*p_prev; digit_node_navigator != sequence->digit_node_end; digit_node_navigator = digit_node_navigator->*p_prev)
                                {
                                    pointer_digit_node_t digit_node_left = pointer_traits_static_cast<pointer_digit_node_t>(digit_node_navigator);
                                    update_digit_node_left_accumulated_storage_if_exists(digit_node_left);
                                }
                            }
                            if (should_update_right_accumulated_storage)
                            {
                                for (pointer_navigator_t digit_node_navigator = digit_node->*p_next; digit_node_navigator != sequence->digit_node_end; digit_node_navigator = digit_node_navigator->*p_next)
                                {
                                    pointer_digit_node_t digit_node_right = pointer_traits_static_cast<pointer_digit_node_t>(digit_node_navigator);
                                    update_or_construct_digit_node_right_accumulated_storage_if_exists(digit_node_right, true);
                                }
                            }
                        };
                        if (!will_delete_digit_back)
                        {
                            update_accumulated_storage(digit_back, false, sequence->digit_middle() != digit_back, true);
                            digit_back = current_digit_back;
                        }
                        else
                        {
                            if (digit_back != sequence->digit_middle())
                            {
                                navigator_t::extract_impl(digit_back, p_prev, p_next);
                                update_accumulated_storage(digit_back, false, false, true);
                                if constexpr (!std::is_same_v<accumulated_storage_t, void>)
                                    sequence->projector_and_accumulator().destroy_accumulated_storage(allocator_element, digit_back->p_accumulated_storage());
                                delete_expression<digit_node_t>(allocator_element, std::exchange(digit_back, current_digit_back));
                            }
                            else
                            {
                                if (digit_back->*p_prev == sequence->digit_node_end)
                                {
                                    assert(digit_front == digit_back);
                                    assert(sequence->digit_middle() == digit_back);
                                    sequence->digit_middle() = digit_front = digit_back->*next;
                                    navigator_t::extract_impl(digit_back, p_prev, p_next);
                                    update_accumulated_storage(pointer_traits_static_cast<pointer_digit_node_t>(static_cast<pointer_navigator_t>(sequence->digit_middle())), false, false, true);
                                    delete_expression<digit_node_t>(allocator_element, std::exchange(digit_back, current_digit_back));
                                }
                                else if ((digit_back->*prev)->digit_position + 1 != digit_back->digit_position)
                                {
                                    sequence->digit_middle() = digit_back->*next;
                                    navigator_t::extract_impl(digit_back, p_prev, p_next);
                                    update_accumulated_storage(pointer_traits_static_cast<pointer_digit_node_t>(static_cast<pointer_navigator_t>(sequence->digit_middle())), true, false, true);
                                    delete_expression<digit_node_t>(allocator_element, std::exchange(digit_back, current_digit_back));
                                }
                                else if (int digit = (digit_back->*prev->*p_tree_left != nullptr) + (digit_back->*prev->*p_tree_right != nullptr); digit == 1)
                                {
                                    if ((digit_back->*next)->digit_position == 0)
                                        p_tree_node_to_p_list_node(digit_back->*next->*p_tree_left)->parent = p_digit_node_to_p_tree_node(digit_back->*prev);
                                    else
                                        (digit_back->*next->*p_tree_left)->parent = p_digit_node_to_p_tree_node(digit_back->*prev);
                                    if (digit_back->*prev->*p_tree_left != nullptr)
                                        digit_back->*prev->*p_tree_right = digit_back->*next->*p_tree_left;
                                    else
                                        digit_back->*prev->*p_tree_left = std::exchange(digit_back->*prev->*p_tree_right, digit_back->*next->*p_tree_left);

                                    bool current_digit_back_is_p_digit_back_p_next = digit_back->*p_next->*p_next == sequence->digit_node_end;

                                    pointer_digit_node_t p_digit_back_p_next = digit_back->*next;
                                    navigator_t::extract_impl(digit_back->*p_next, p_prev, p_next);
                                    delete_expression<digit_node_t>(allocator_element, p_digit_back_p_next);

                                    navigator_t::extract_impl(digit_back, p_prev, p_next);

                                    sequence->digit_middle() = digit_back->*prev;
                                    if constexpr (!std::is_same_v<accumulated_storage_t, void>)
                                        sequence->projector_and_accumulator().destroy_accumulated_storage(allocator_element, (digit_back->*prev)->p_accumulated_storage());
                                    update_accumulated_storage(digit_back->*prev, true, false, true);
                                    if (current_digit_back_is_p_digit_back_p_next)
                                        delete_expression<digit_node_t>(allocator_element, std::exchange(digit_back, pointer_traits_static_cast<pointer_digit_node_t>(static_cast<pointer_navigator_t>(sequence->digit_middle()))));
                                    else
                                        delete_expression<digit_node_t>(allocator_element, std::exchange(digit_back, current_digit_back));
                                }
                                else if (digit == 2)
                                {
                                    pointer_tree_node_t tree_root = std::pointer_traits<pointer_tree_node_t>::pointer_to(*new_expression<tree_node_t>(allocator_element, p_digit_node_to_p_tree_node(digit_back->*prev)));
                                    tree_root->*p_child_left = digit_back->*prev->*p_tree_left;
                                    tree_root->*p_child_right = digit_back->*prev->*p_tree_right;
                                    if ((digit_back->*prev)->digit_position == 0)
                                        p_tree_node_to_p_list_node(tree_root->*p_child_left)->parent = p_tree_node_to_p_list_node(tree_root->*p_child_right)->parent = tree_root;
                                    else
                                        (tree_root->*p_child_left)->parent = (tree_root->*p_child_right)->parent = tree_root;
                                    update_or_construct_tree_node_accumulated_storage_if_exists(tree_root, true);
                                    digit_back->*prev->*p_tree_left = tree_root;
                                    digit_back->*prev->*p_tree_right = nullptr;
                                    ++(digit_back->*prev)->digit_position;

                                    navigator_t::extract_impl(digit_back, p_prev, p_next);
                                    sequence->digit_middle() = digit_back->*prev;
                                    if constexpr (!std::is_same_v<accumulated_storage_t, void>)
                                        sequence->projector_and_accumulator().destroy_accumulated_storage(allocator_element, (digit_back->*prev)->p_accumulated_storage());
                                    update_accumulated_storage(digit_back->*prev, true, false, true);
                                    delete_expression<digit_node_t>(allocator_element, std::exchange(digit_back, current_digit_back));
                                }
                                else
                                    std::unreachable();
                            }
                        }
                        update_digit_node_end_accumulated_storage_if_exists(digit_front, digit_back);
                        (sequence->*p_digit_back)() = digit_back;
                        (sequence->*p_digit_front)() = digit_front;
                    }
                }

                void update_range_impl(pointer_list_node_t list_node_range_front, pointer_list_node_t list_node_range_back) const
                {
                    assert(!tagged_ptr_bit0_is_setted(list_node_range_front));
                    assert(pointer_traits_static_cast<pointer_navigator_t>(list_node_range_front) != pointer_traits_static_cast<pointer_navigator_t>(tagged_ptr_bit0_unsetted(sequence->list_node_end)));
                    assert(!tagged_ptr_bit0_is_setted(list_node_range_back));
                    assert(pointer_traits_static_cast<pointer_navigator_t>(list_node_range_back) != pointer_traits_static_cast<pointer_navigator_t>(tagged_ptr_bit0_unsetted(sequence->list_node_end)));
                    next_or_prev_impl_t next{&navigator_t::next}, prev{&navigator_t::prev};
                    auto descend = [&](pointer_tree_node_t tree_node) //
                    {
                        auto descend_impl = [this, &list_node_range_back](auto &this_, pointer_tree_node_t tree_node) //
                        {
                            if (tagged_ptr_bit0_is_setted(tree_node))
                            {
                                update_list_node_projected_storage_if_exists(p_tree_node_to_p_list_node(tree_node));
                                return p_tree_node_to_p_list_node(tree_node) == list_node_range_back;
                            }
                            else
                            {
                                bool found_list_node_range_back = this_(this_, tree_node->child_left) || this_(this_, tree_node->child_right);
                                update_or_construct_tree_node_accumulated_storage_if_exists(tree_node);
                                return found_list_node_range_back;
                            }
                        };
                        return descend_impl(descend_impl, tree_node);
                    };
                    pointer_tree_node_t tree_node = p_list_node_to_p_tree_node(list_node_range_front);
                    pointer_tree_node_t tree_node_parent = list_node_range_front->parent;
                    bool list_node_range_back_is_inside_tree_node = false;
                    list_node_range_back_is_inside_tree_node = list_node_range_back_is_inside_tree_node || descend(tree_node);
                    while (!tagged_ptr_bit0_is_setted(tree_node_parent))
                    {
                        if (!list_node_range_back_is_inside_tree_node && tree_node == tree_node_parent->child_left)
                            list_node_range_back_is_inside_tree_node = list_node_range_back_is_inside_tree_node || descend(tree_node_parent->child_right);
                        update_or_construct_tree_node_accumulated_storage_if_exists(tree_node_parent);
                        tree_node = std::exchange(tree_node_parent, tree_node_parent->parent);
                    }
                    pointer_digit_node_t digit_node_front = p_tree_node_to_p_digit_node(tree_node_parent), digit_node_back = digit_node_front;
                    auto is_at_middle_s_left_or_right = [&sequence = sequence, prev](pointer_digit_node_t digit_node, pointer_tree_node_t tree_node) //
                    {
                        if (digit_node == sequence->digit_middle())
                        {
                            if (tree_node == pointer_traits_static_cast<pointer_digit_node_t>(static_cast<pointer_navigator_t>(sequence->digit_middle()))->tree_left) return false;
                            else if (tree_node == pointer_traits_static_cast<pointer_digit_node_t>(static_cast<pointer_navigator_t>(sequence->digit_middle()))->tree_right) return true;
                            else std::unreachable();
                        }
                        else
                        {
                            if (tagged_ptr_bit0_is_setted(digit_node->prev) || (digit_node->*prev)->digit_position < digit_node->digit_position) return false;
                            else return true;
                        }
                    };
                    bool front_is_at_middle_s_left_or_right = is_at_middle_s_left_or_right(digit_node_front, tree_node);
                    if (!list_node_range_back_is_inside_tree_node)
                    {
                        while (true)
                        {
                            // advance to next tree, break if found
                            if (digit_node_back->tree_left == tree_node && digit_node_back->tree_right != nullptr)
                            {
                                if (descend(tree_node = digit_node_back->tree_right))
                                    break;
                            }
                            else if ((digit_node_back->tree_left == tree_node && digit_node_back->tree_right == nullptr) || digit_node_back->tree_right == tree_node)
                            {
                                assert(!tagged_ptr_bit0_is_setted(digit_node_back->next));
                                digit_node_back = digit_node_back->*next;
                                if (digit_node_back->tree_left != nullptr)
                                {
                                    if (descend(tree_node = digit_node_back->tree_left))
                                        break;
                                }
                                else if (digit_node_back->tree_right != nullptr)
                                {
                                    if (descend(tree_node = digit_node_back->tree_right))
                                        break;
                                }
                                else std::unreachable();
                            }
                            else std::unreachable();
                        }
                    }
                    bool back_is_at_middle_s_left_or_right = is_at_middle_s_left_or_right(digit_node_back, tree_node);
                    if (!front_is_at_middle_s_left_or_right && !back_is_at_middle_s_left_or_right)
                    {
                        for (pointer_navigator_t digit_node_navigator = digit_node_back == sequence->digit_middle() ? digit_node_back->prev : digit_node_back; !tagged_ptr_bit0_is_setted(digit_node_navigator); digit_node_navigator = digit_node_navigator->prev)
                            update_digit_node_left_accumulated_storage_if_exists(pointer_traits_static_cast<pointer_digit_node_t>(digit_node_navigator));
                    }
                    else if (front_is_at_middle_s_left_or_right && back_is_at_middle_s_left_or_right)
                    {
                        for (pointer_navigator_t digit_node_navigator = digit_node_front == sequence->digit_middle() ? digit_node_front->next : digit_node_front; !tagged_ptr_bit0_is_setted(digit_node_navigator); digit_node_navigator = digit_node_navigator->next)
                            update_or_construct_digit_node_right_accumulated_storage_if_exists(pointer_traits_static_cast<pointer_digit_node_t>(digit_node_navigator));
                    }
                    else if (!front_is_at_middle_s_left_or_right && back_is_at_middle_s_left_or_right)
                    {
                        for (pointer_navigator_t digit_node_navigator = static_cast<pointer_navigator_t>(sequence->digit_middle())->prev; !tagged_ptr_bit0_is_setted(digit_node_navigator); digit_node_navigator = digit_node_navigator->prev)
                            update_digit_node_left_accumulated_storage_if_exists(pointer_traits_static_cast<pointer_digit_node_t>(digit_node_navigator));
                        for (pointer_navigator_t digit_node_navigator = static_cast<pointer_navigator_t>(sequence->digit_middle())->next; !tagged_ptr_bit0_is_setted(digit_node_navigator); digit_node_navigator = digit_node_navigator->next)
                            update_or_construct_digit_node_right_accumulated_storage_if_exists(pointer_traits_static_cast<pointer_digit_node_t>(digit_node_navigator));
                    }
                    else std::unreachable();
                    update_digit_node_end_accumulated_storage_if_exists(pointer_traits_static_cast<pointer_digit_node_t>(static_cast<pointer_navigator_t>((sequence->*p_digit_front)())), pointer_traits_static_cast<pointer_digit_node_t>(static_cast<pointer_navigator_t>((sequence->*p_digit_back)())));
                }
                accumulated_storage_t read_range_impl(pointer_list_node_t list_node_range_front, pointer_list_node_t list_node_range_back) const
                {
                    assert(!tagged_ptr_bit0_is_setted(list_node_range_front));
                    assert(static_cast<pointer_navigator_t>(list_node_range_front) != static_cast<pointer_navigator_t>(tagged_ptr_bit0_unsetted(sequence->list_node_end)));
                    assert(!tagged_ptr_bit0_is_setted(list_node_range_back));
                    assert(static_cast<pointer_navigator_t>(list_node_range_back) != static_cast<pointer_navigator_t>(tagged_ptr_bit0_unsetted(sequence->list_node_end)));
                    next_or_prev_impl_t next{&navigator_t::next}, prev [[maybe_unused]]{&navigator_t::prev};
                    if (list_node_range_front == list_node_range_back)
                        return sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple(std::cref(*list_node_range_front->actual_projected_storage.p_projected_storage_or_p_element())));
                    else
                    {
                        enum {
                            status_waiting_for_collision = 0,
                            status_accumulating_piplings = 1,
                            status_waiting_at_digit_node = 2,
                        };
                        auto recursive_polymorphic_lambda = [this, next,
                                                             tree_node_front = p_list_node_to_p_tree_node(list_node_range_front), tree_node_back = p_list_node_to_p_tree_node(list_node_range_back),
                                                             status_front = status_waiting_for_collision, status_back = status_waiting_for_collision,
                                                             digit_node_front = pointer_digit_node_t(nullptr), digit_node_back = pointer_digit_node_t(nullptr)](auto &this_, auto accumulated_tuple_so_far_front, auto accumulated_tuple_so_far_back) mutable -> accumulated_storage_t //
                        {
                            pointer_tree_node_t tree_node_front_parent;
                            pointer_tree_node_t tree_node_back_parent;
                            if (status_front != status_waiting_at_digit_node)
                                tree_node_front_parent = tagged_ptr_bit0_is_setted(tree_node_front) ? p_tree_node_to_p_list_node(tree_node_front)->parent : tree_node_front->parent;
                            if (status_back != status_waiting_at_digit_node)
                                tree_node_back_parent = tagged_ptr_bit0_is_setted(tree_node_back) ? p_tree_node_to_p_list_node(tree_node_back)->parent : tree_node_back->parent;
                            if (status_front != status_waiting_at_digit_node && status_back != status_waiting_at_digit_node && tree_node_front_parent == tree_node_back_parent)
                            {
                                if (status_front == status_waiting_for_collision && status_back == status_waiting_for_collision)
                                {
                                    if (tagged_ptr_bit0_is_setted(tree_node_front_parent))
                                    {
                                        if (tagged_ptr_bit0_is_setted(tree_node_front))
                                            return sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple(std::cref(*p_tree_node_to_p_list_node(tree_node_front)->actual_projected_storage.p_projected_storage_or_p_element()), std::cref(*p_tree_node_to_p_list_node(tree_node_back)->actual_projected_storage.p_projected_storage_or_p_element())));
                                        else
                                            return sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(*tree_node_front->p_accumulated_storage()), std::ref(*tree_node_back->p_accumulated_storage())));
                                    }
                                    else
                                        return sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(*tree_node_front_parent->p_accumulated_storage())));
                                }
                                else
                                    return sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::tuple_cat(accumulated_tuple_so_far_front, accumulated_tuple_so_far_back));
                            }
                            else if (status_front == status_waiting_at_digit_node && status_back == status_waiting_at_digit_node)
                            {
                                if (digit_node_front == digit_node_back ||
                                    (digit_node_front->next == digit_node_back &&
                                     ((tree_node_front == digit_node_front->tree_left && digit_node_front->tree_right == nullptr) || tree_node_front == digit_node_front->tree_right) &&
                                     (tree_node_back == digit_node_back->tree_left || (digit_node_back->tree_left == nullptr && tree_node_back == digit_node_back->tree_right))))
                                    return sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::tuple_cat(accumulated_tuple_so_far_front, accumulated_tuple_so_far_back));
                                else
                                {
                                    pointer_tree_node_t tree_to_be_accumulated;
                                    if ((tree_node_front == digit_node_front->tree_left && digit_node_front->tree_right == nullptr) || tree_node_front == digit_node_front->tree_right)
                                    {
                                        digit_node_front = digit_node_front->*next;
                                        if (digit_node_front->tree_left != nullptr)
                                            tree_to_be_accumulated = digit_node_front->tree_left;
                                        else if (digit_node_front->tree_right != nullptr)
                                            tree_to_be_accumulated = digit_node_front->tree_right;
                                        else std::unreachable();
                                    }
                                    else if (tree_node_front == digit_node_front->tree_left && digit_node_front->tree_right != nullptr)
                                    {
                                        tree_to_be_accumulated = digit_node_front->tree_right;
                                    }
                                    else std::unreachable();
                                    if (tagged_ptr_bit0_is_setted(tree_to_be_accumulated))
                                    {
                                        accumulated_storage_t intermediate_accumulated_storage(sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::tuple_cat(accumulated_tuple_so_far_front, std::make_tuple(std::cref(*p_tree_node_to_p_list_node(tree_to_be_accumulated)->actual_projected_storage.p_projected_storage_or_p_element())))));
                                        return tree_node_front = tree_to_be_accumulated, this_(this_, std::make_tuple(std::ref(intermediate_accumulated_storage)), accumulated_tuple_so_far_back);
                                    }
                                    else
                                    {
                                        accumulated_storage_t intermediate_accumulated_storage(sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::tuple_cat(accumulated_tuple_so_far_front, std::make_tuple(std::ref(*tree_to_be_accumulated->p_accumulated_storage())))));
                                        return tree_node_front = tree_to_be_accumulated, this_(this_, std::make_tuple(std::ref(intermediate_accumulated_storage)), accumulated_tuple_so_far_back);
                                    }
                                }
                            }
                            else
                            {
                                auto get_accumulated_tuple_back = [&](auto accumulated_tuple_so_far_front) -> accumulated_storage_t //
                                {
                                    if (status_back == status_waiting_at_digit_node)
                                        return this_(this_, accumulated_tuple_so_far_front, accumulated_tuple_so_far_back);
                                    else
                                    {
                                        if (tagged_ptr_bit0_is_setted(tree_node_back_parent))
                                        {
                                            status_back = status_waiting_at_digit_node;
                                            digit_node_back = p_tree_node_to_p_digit_node(tree_node_back_parent);
                                            return this_(this_, accumulated_tuple_so_far_front, accumulated_tuple_so_far_back);
                                        }
                                        else
                                        {
                                            if (status_back == status_waiting_for_collision)
                                            {
                                                if (tree_node_back == tree_node_back_parent->child_right)
                                                    return tree_node_back = tree_node_back_parent, this_(this_, accumulated_tuple_so_far_front, std::make_tuple(std::ref(*tree_node_back->p_accumulated_storage())));
                                                else if (tree_node_back == tree_node_back_parent->child_left)
                                                {
                                                    status_back = status_accumulating_piplings;
                                                    return tree_node_back = tree_node_back_parent, this_(this_, accumulated_tuple_so_far_front, accumulated_tuple_so_far_back);
                                                }
                                                else std::unreachable();
                                            }
                                            else if (status_back == status_accumulating_piplings)
                                            {
                                                if (tree_node_back == tree_node_back_parent->child_right)
                                                {
                                                    accumulated_storage_t intermediate_accumulated_storage(sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::tuple_cat(std::make_tuple(std::ref(*tree_node_back_parent->child_left->p_accumulated_storage())), accumulated_tuple_so_far_back)));
                                                    return tree_node_back = tree_node_back_parent, this_(this_, accumulated_tuple_so_far_front, std::make_tuple(std::ref(intermediate_accumulated_storage)));
                                                }
                                                else if (tree_node_back == tree_node_back_parent->child_left)
                                                    return tree_node_back = tree_node_back_parent, this_(this_, accumulated_tuple_so_far_front, accumulated_tuple_so_far_back);
                                                else std::unreachable();
                                            }
                                            else std::unreachable();
                                        }
                                    }
                                };
                                auto get_accumulated_tuple_front = [&, get_accumulated_tuple_back]() -> accumulated_storage_t //
                                {
                                    if (status_front == status_waiting_at_digit_node)
                                        return get_accumulated_tuple_back(accumulated_tuple_so_far_front);
                                    else
                                    {
                                        if (tagged_ptr_bit0_is_setted(tree_node_front_parent))
                                        {
                                            status_front = status_waiting_at_digit_node;
                                            digit_node_front = p_tree_node_to_p_digit_node(tree_node_front_parent);
                                            return get_accumulated_tuple_back(accumulated_tuple_so_far_front);
                                        }
                                        else
                                        {
                                            if (status_front == status_waiting_for_collision)
                                            {
                                                if (tree_node_front == tree_node_front_parent->child_left)
                                                    return tree_node_front = tree_node_front_parent, get_accumulated_tuple_back(std::make_tuple(std::ref(*tree_node_front->p_accumulated_storage())));
                                                else if (tree_node_front == tree_node_front_parent->child_right)
                                                {
                                                    status_front = status_accumulating_piplings;
                                                    return tree_node_front = tree_node_front_parent, get_accumulated_tuple_back(accumulated_tuple_so_far_front);
                                                }
                                                else std::unreachable();
                                            }
                                            else if (status_front == status_accumulating_piplings)
                                            {
                                                if (tree_node_front == tree_node_front_parent->child_left)
                                                {
                                                    accumulated_storage_t intermediate_accumulated_storage(sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::tuple_cat(accumulated_tuple_so_far_front, std::make_tuple(std::ref(*tree_node_front_parent->child_right->p_accumulated_storage())))));
                                                    return tree_node_front = tree_node_front_parent, get_accumulated_tuple_back(std::make_tuple(std::ref(intermediate_accumulated_storage)));
                                                }
                                                else if (tree_node_front == tree_node_front_parent->child_right)
                                                    return tree_node_front = tree_node_front_parent, get_accumulated_tuple_back(accumulated_tuple_so_far_front);
                                                else std::unreachable();
                                            }
                                            else std::unreachable();
                                        }
                                    }
                                };
                                return get_accumulated_tuple_front();
                            }
                        };
                        return recursive_polymorphic_lambda(recursive_polymorphic_lambda, std::make_tuple(std::cref(*list_node_range_front->actual_projected_storage.p_projected_storage_or_p_element())), std::make_tuple(std::cref(*list_node_range_back->actual_projected_storage.p_projected_storage_or_p_element())));
                    }
                }

                template<invocable_r<bool, accumulated_storage_t const &> monotonic_predicate_t>
                pointer_navigator_t find_by_monotonic_predicate(monotonic_predicate_t const &monotonic_predicate) const
                {
                    if (sequence->digit_middle() == tagged_ptr_bit0_unsetted(sequence->digit_node_end)) // sequence is empty
                        return sequence->list_node_end;
                    else
                    {
                        if (!monotonic_predicate(*tagged_ptr_bit0_unsetted(sequence->digit_node_end)->p_accumulated_storage())) // turning point is not inside sequence
                            return sequence->list_node_end;
                        else
                        {
                            auto branch_based_on_predicate = [&](auto down, auto next) //
                            {
                                return [&, down, next](accumulated_storage_t &accumulated_storage_so_far, accumulated_storage_t &accumulated_storage_so_far_including_node, auto node) -> pointer_navigator_t //
                                {
                                    if(monotonic_predicate(accumulated_storage_so_far_including_node))
                                        return down(accumulated_storage_so_far, node);
                                    else
                                        return next(accumulated_storage_so_far_including_node); };
                            };
                            auto recursive_polymorphic_lambda = [this, branch_based_on_predicate](auto &this_, accumulated_storage_t &accumulated_storage_so_far, auto node) -> pointer_navigator_t //
                            {
                                if constexpr (std::is_same_v<decltype(node), pointer_digit_node_t>)
                                {
                                    pointer_digit_node_t digit_node = node;
                                    bool is_left_or_right = tagged_ptr_bit0_is_setted(digit_node->next) || pointer_traits_static_cast<pointer_digit_node_t>(digit_node->next)->digit_position < digit_node->digit_position;
                                    if (!is_left_or_right)
                                    {
                                        pointer_digit_node_t digit_node_left = digit_node;
                                        auto get_left_operand = [&](auto return_accumulated_storage) //
                                        { return [&, return_accumulated_storage](accumulated_storage_t &accumulated_storage_so_far) -> pointer_navigator_t //
                                          {
                                              if (digit_node_left->tree_left != nullptr)
                                              {
                                                  if (digit_node_left->digit_position != 0)
                                                      return return_accumulated_storage(accumulated_storage_so_far, unmove(sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(accumulated_storage_so_far), std::ref(*digit_node_left->tree_left->p_accumulated_storage())))), digit_node_left->tree_left);
                                                  else
                                                      return return_accumulated_storage(accumulated_storage_so_far, unmove(sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(accumulated_storage_so_far), std::cref(*p_tree_node_to_p_list_node(digit_node_left->tree_left)->actual_projected_storage.p_projected_storage_or_p_element())))), p_tree_node_to_p_list_node(digit_node_left->tree_left));
                                              }
                                              else
                                                  return return_accumulated_storage(accumulated_storage_so_far, accumulated_storage_so_far, nullptr);
                                          }; };
                                        auto get_middle_operand = [&](auto return_accumulated_storage) //
                                        { return [&, return_accumulated_storage](accumulated_storage_t &accumulated_storage_so_far) -> pointer_navigator_t //
                                          {
                                              if (digit_node_left->tree_right != nullptr)
                                              {
                                                  if (digit_node_left->digit_position != 0)
                                                      return return_accumulated_storage(accumulated_storage_so_far, unmove(sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(accumulated_storage_so_far), std::ref(*digit_node_left->tree_right->p_accumulated_storage())))), digit_node_left->tree_right);
                                                  else
                                                      return return_accumulated_storage(accumulated_storage_so_far, unmove(sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(accumulated_storage_so_far), std::cref(*p_tree_node_to_p_list_node(digit_node_left->tree_right)->actual_projected_storage.p_projected_storage_or_p_element())))), p_tree_node_to_p_list_node(digit_node_left->tree_right));
                                              }
                                              else
                                                  return return_accumulated_storage(accumulated_storage_so_far, accumulated_storage_so_far, nullptr);
                                          }; };
                                        auto get_right_operand = [&](auto return_accumulated_storage) //
                                        {
                                            return [&, return_accumulated_storage](accumulated_storage_t &accumulated_storage_so_far) -> pointer_navigator_t //
                                            {
                                                next_or_prev_impl_t next{p_next}, prev [[maybe_unused]]{p_prev};
                                                if (sequence->digit_middle() != digit_node_left->*next)
                                                    return return_accumulated_storage(accumulated_storage_so_far, unmove(sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(accumulated_storage_so_far), std::ref(*(digit_node_left->*next)->p_accumulated_storage())))), digit_node_left->*next);
                                                else
                                                {
                                                    if ((digit_node_left->*next)->tree_left != nullptr)
                                                        return return_accumulated_storage(accumulated_storage_so_far, unmove(sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(accumulated_storage_so_far), std::ref(*(digit_node_left->*next)->tree_left->p_accumulated_storage())))), (digit_node_left->*next)->tree_left);
                                                    else
                                                        return return_accumulated_storage(accumulated_storage_so_far, accumulated_storage_so_far, nullptr);
                                                }
                                            };
                                        };
                                        return get_left_operand(
                                            branch_based_on_predicate(
                                                std::bind(this_, this_, std::placeholders::_1, std::placeholders::_2),
                                                get_middle_operand(
                                                    branch_based_on_predicate(
                                                        std::bind(this_, this_, std::placeholders::_1, std::placeholders::_2),
                                                        get_right_operand(
                                                            branch_based_on_predicate(
                                                                std::bind(this_, this_, std::placeholders::_1, std::placeholders::_2),
                                                                []([[maybe_unused]] accumulated_storage_t &accumulated_storage_so_far) -> pointer_navigator_t { std::unreachable(); }
                                                            )
                                                        )
                                                    )
                                                )
                                            )
                                        )(accumulated_storage_so_far);
                                    }
                                    else
                                    {
                                        pointer_digit_node_t digit_node_right = digit_node;
                                        auto get_left_operand = [&](auto return_accumulated_storage) //
                                        { return [&, return_accumulated_storage](accumulated_storage_t &accumulated_storage_so_far) -> pointer_navigator_t //
                                          {
                                              next_or_prev_impl_t next [[maybe_unused]]{p_next}, prev{p_prev};
                                              if (sequence->digit_middle() != digit_node_right->*prev)
                                                  return return_accumulated_storage(accumulated_storage_so_far, unmove(sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(accumulated_storage_so_far), std::ref(*(digit_node_right->*prev)->p_accumulated_storage())))), digit_node_right->*prev);
                                              else
                                              {
                                                  if ((digit_node_right->*prev)->tree_right != nullptr)
                                                      return return_accumulated_storage(accumulated_storage_so_far, unmove(sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(accumulated_storage_so_far), std::ref(*(digit_node_right->*prev)->tree_right->p_accumulated_storage())))), (digit_node_right->*prev)->tree_right);
                                                  else
                                                      return return_accumulated_storage(accumulated_storage_so_far, accumulated_storage_so_far, nullptr);
                                              }
                                          }; };
                                        auto get_middle_operand = [&](auto return_accumulated_storage) //
                                        { return [&, return_accumulated_storage](accumulated_storage_t &accumulated_storage_so_far) -> pointer_navigator_t //
                                          {
                                              if (digit_node_right->tree_left != nullptr)
                                              {
                                                  if (digit_node_right->digit_position != 0)
                                                      return return_accumulated_storage(accumulated_storage_so_far, unmove(sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(accumulated_storage_so_far), std::ref(*digit_node_right->tree_left->p_accumulated_storage())))), digit_node_right->tree_left);
                                                  else
                                                      return return_accumulated_storage(accumulated_storage_so_far, unmove(sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(accumulated_storage_so_far), std::cref(*p_tree_node_to_p_list_node(digit_node_right->tree_left)->actual_projected_storage.p_projected_storage_or_p_element())))), p_tree_node_to_p_list_node(digit_node_right->tree_left));
                                              }
                                              else
                                                  return return_accumulated_storage(accumulated_storage_so_far, accumulated_storage_so_far, nullptr);
                                          }; };
                                        auto get_right_operand = [&](auto return_accumulated_storage) //
                                        { return [&, return_accumulated_storage](accumulated_storage_t &accumulated_storage_so_far) -> pointer_navigator_t //
                                          {
                                              if (digit_node_right->tree_right != nullptr)
                                              {
                                                  if (digit_node_right->digit_position != 0)
                                                      return return_accumulated_storage(accumulated_storage_so_far, unmove(sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(accumulated_storage_so_far), std::ref(*digit_node_right->tree_right->p_accumulated_storage())))), digit_node_right->tree_right);
                                                  else
                                                      return return_accumulated_storage(accumulated_storage_so_far, unmove(sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(accumulated_storage_so_far), std::cref(*p_tree_node_to_p_list_node(digit_node_right->tree_right)->actual_projected_storage.p_projected_storage_or_p_element())))), p_tree_node_to_p_list_node(digit_node_right->tree_right));
                                              }
                                              else
                                                  return return_accumulated_storage(accumulated_storage_so_far, accumulated_storage_so_far, nullptr);
                                          }; };
                                        return get_left_operand(
                                            branch_based_on_predicate(
                                                std::bind(this_, this_, std::placeholders::_1, std::placeholders::_2),
                                                get_middle_operand(
                                                    branch_based_on_predicate(
                                                        std::bind(this_, this_, std::placeholders::_1, std::placeholders::_2),
                                                        get_right_operand(
                                                            branch_based_on_predicate(
                                                                std::bind(this_, this_, std::placeholders::_1, std::placeholders::_2),
                                                                []([[maybe_unused]] accumulated_storage_t &accumulated_storage_so_far) -> pointer_navigator_t { std::unreachable(); }
                                                            )
                                                        )
                                                    )
                                                )
                                            )
                                        )(accumulated_storage_so_far);
                                    }
                                }
                                else if constexpr (std::is_same_v<decltype(node), pointer_tree_node_t>)
                                {
                                    pointer_tree_node_t tree_node = node;
                                    auto get_left_operand = [&](auto return_accumulated_storage) //
                                    { return [&, return_accumulated_storage](accumulated_storage_t &accumulated_storage_so_far) -> pointer_navigator_t //
                                      {
                                          if (tagged_ptr_bit0_is_setted(tree_node->child_left))
                                              return return_accumulated_storage(accumulated_storage_so_far, unmove(sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(accumulated_storage_so_far), std::cref(*p_tree_node_to_p_list_node(tree_node->child_left)->actual_projected_storage.p_projected_storage_or_p_element())))), p_tree_node_to_p_list_node(tree_node->child_left));
                                          else
                                              return return_accumulated_storage(accumulated_storage_so_far, unmove(sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(accumulated_storage_so_far), std::ref(*tree_node->child_left->p_accumulated_storage())))), tree_node->child_left);
                                      }; };
                                    auto get_right_operand = [&](auto return_accumulated_storage) //
                                    { return [&, return_accumulated_storage](accumulated_storage_t &accumulated_storage_so_far) -> pointer_navigator_t //
                                      {
                                          if (tagged_ptr_bit0_is_setted(tree_node->child_right))
                                              return return_accumulated_storage(accumulated_storage_so_far, unmove(sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(accumulated_storage_so_far), std::cref(*p_tree_node_to_p_list_node(tree_node->child_right)->actual_projected_storage.p_projected_storage_or_p_element())))), p_tree_node_to_p_list_node(tree_node->child_right));
                                          else
                                              return return_accumulated_storage(accumulated_storage_so_far, unmove(sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(accumulated_storage_so_far), std::ref(*tree_node->child_right->p_accumulated_storage())))), tree_node->child_right);
                                      }; };
                                    return get_left_operand(
                                        branch_based_on_predicate(
                                            std::bind(this_, this_, std::placeholders::_1, std::placeholders::_2),
                                            get_right_operand(
                                                branch_based_on_predicate(
                                                    std::bind(this_, this_, std::placeholders::_1, std::placeholders::_2),
                                                    []([[maybe_unused]] accumulated_storage_t &accumulated_storage_so_far) -> pointer_navigator_t { std::unreachable(); }
                                                )
                                            )
                                        )
                                    )(accumulated_storage_so_far);
                                }
                                else if constexpr (std::is_same_v<decltype(node), pointer_list_node_t>)
                                {
                                    pointer_list_node_t list_node = node;
                                    return list_node;
                                }
                                else
                                {
                                    std::unreachable();
                                }
                            };

                            auto get_left_operand = [&](auto return_accumulated_storage) //
                            { return [&, return_accumulated_storage](accumulated_storage_t &accumulated_storage_so_far) -> pointer_navigator_t //
                              {
                                  pointer_digit_node_t digit_front = pointer_traits_static_cast<pointer_digit_node_t>(static_cast<pointer_navigator_t>(sequence->digit_front()));
                                  if (digit_front != sequence->digit_middle())
                                      return return_accumulated_storage(accumulated_storage_so_far, unmove(sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(accumulated_storage_so_far), std::ref(*digit_front->p_accumulated_storage())))), static_cast<pointer_digit_node_t>(digit_front));
                                  else
                                  {
                                      if (digit_front->tree_left != nullptr)
                                      {
                                          if (digit_front->digit_position != 0)
                                              return return_accumulated_storage(accumulated_storage_so_far, unmove(sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(accumulated_storage_so_far), std::ref(*(digit_front->tree_left)->p_accumulated_storage())))), digit_front->tree_left);
                                          else
                                              return return_accumulated_storage(accumulated_storage_so_far, unmove(sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(accumulated_storage_so_far), std::cref(*p_tree_node_to_p_list_node(digit_front->tree_left)->actual_projected_storage.p_projected_storage_or_p_element())))), p_tree_node_to_p_list_node(digit_front->tree_left));
                                      }
                                      else
                                          return return_accumulated_storage(accumulated_storage_so_far, accumulated_storage_so_far, nullptr);
                                  }
                              }; };
                            auto get_right_operand = [&](auto return_accumulated_storage) //
                            { return [&, return_accumulated_storage](accumulated_storage_t &accumulated_storage_so_far) -> pointer_navigator_t //
                              {
                                  pointer_digit_node_t digit_back = pointer_traits_static_cast<pointer_digit_node_t>(static_cast<pointer_navigator_t>(sequence->digit_back()));
                                  if (sequence->digit_middle() != digit_back)
                                      return return_accumulated_storage(accumulated_storage_so_far, unmove(sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(accumulated_storage_so_far), std::ref(*digit_back->p_accumulated_storage())))), static_cast<pointer_digit_node_t>(digit_back));
                                  else
                                  {
                                      if (digit_back->tree_right != nullptr)
                                      {
                                          if (digit_back->digit_position != 0)
                                              return return_accumulated_storage(accumulated_storage_so_far, unmove(sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(accumulated_storage_so_far), std::ref(*(digit_back->tree_right)->p_accumulated_storage())))), digit_back->tree_right);
                                          else
                                              return return_accumulated_storage(accumulated_storage_so_far, unmove(sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(accumulated_storage_so_far), std::cref(*p_tree_node_to_p_list_node(digit_back->tree_right)->actual_projected_storage.p_projected_storage_or_p_element())))), p_tree_node_to_p_list_node(digit_back->tree_right));
                                      }
                                      else
                                          return return_accumulated_storage(accumulated_storage_so_far, accumulated_storage_so_far, nullptr);
                                  }
                              }; };
                            accumulated_storage_t intermediate_accumulated_storage(sequence->projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple()));
                            return get_left_operand(
                                branch_based_on_predicate(
                                    std::bind(recursive_polymorphic_lambda, recursive_polymorphic_lambda, std::placeholders::_1, std::placeholders::_2),
                                    get_right_operand(
                                        branch_based_on_predicate(
                                            std::bind(recursive_polymorphic_lambda, recursive_polymorphic_lambda, std::placeholders::_1, std::placeholders::_2),
                                            []([[maybe_unused]] accumulated_storage_t &accumulated_storage_so_far) -> pointer_navigator_t { std::unreachable(); }
                                        )
                                    )
                                )
                            )(intermediate_accumulated_storage);
                        }
                    }
                }

                template</*std::output_iterator<pointer_navigator_t>*/ typename iterator_output_pointer_list_node_t, typename heap_predicate_t> requires (invocable_r<heap_predicate_t, bool, accumulated_storage_t &> && invocable_r<heap_predicate_t, bool, typename sequence_t::projected_storage_or_element_t const &>)
                void find_by_heap_predicate(iterator_output_pointer_list_node_t iterator_output_pointer_list_node, heap_predicate_t const &heap_predicate) const
                {
                    if (sequence->digit_middle() == tagged_ptr_bit0_unsetted(sequence->digit_node_end)) // sequence is empty
                        return;
                    else
                    {
                        if (!heap_predicate(*tagged_ptr_bit0_unsetted(sequence->digit_node_end)->p_accumulated_storage())) // all elements are less than the value inside the predicate
                            return;
                        else
                        {
                            auto branch_based_on_predicate = [&](auto down) //
                            {
                                return [&, down](auto &accumulated_storage_or_projected_storage_or_element_covered_by_node, auto node) -> void //
                                {
                                    if(heap_predicate(accumulated_storage_or_projected_storage_or_element_covered_by_node))
                                        down(node); };
                            };
                            auto recursive_polymorphic_lambda = [this, branch_based_on_predicate, &iterator_output_pointer_list_node](auto &this_, auto node) -> void //
                            {
                                if constexpr (std::is_same_v<decltype(node), pointer_digit_node_t>)
                                {
                                    pointer_digit_node_t digit_node = node;
                                    bool is_left_or_right = tagged_ptr_bit0_is_setted(digit_node->next) || pointer_traits_static_cast<pointer_digit_node_t>(digit_node->next)->digit_position < digit_node->digit_position;
                                    if (!is_left_or_right)
                                    {
                                        pointer_digit_node_t digit_node_left = digit_node;
                                        auto get_left_operand = [&](auto return_accumulated_storage) //
                                        { return [&, return_accumulated_storage]() -> void //
                                          {
                                              if (digit_node_left->tree_left != nullptr)
                                              {
                                                  if (digit_node_left->digit_position != 0)
                                                      return_accumulated_storage(*digit_node_left->tree_left->p_accumulated_storage(), digit_node_left->tree_left);
                                                  else
                                                      return_accumulated_storage(std::as_const(*p_tree_node_to_p_list_node(digit_node_left->tree_left)->actual_projected_storage.p_projected_storage_or_p_element()), p_tree_node_to_p_list_node(digit_node_left->tree_left));
                                              }
                                              else
                                                  ;
                                          }; };
                                        auto get_middle_operand = [&](auto return_accumulated_storage) //
                                        { return [&, return_accumulated_storage]() -> void //
                                          {
                                              if (digit_node_left->tree_right != nullptr)
                                              {
                                                  if (digit_node_left->digit_position != 0)
                                                      return_accumulated_storage(*digit_node_left->tree_right->p_accumulated_storage(), digit_node_left->tree_right);
                                                  else
                                                      return_accumulated_storage(std::as_const(*p_tree_node_to_p_list_node(digit_node_left->tree_right)->actual_projected_storage.p_projected_storage_or_p_element()), p_tree_node_to_p_list_node(digit_node_left->tree_right));
                                              }
                                              else
                                                  ;
                                          }; };
                                        auto get_right_operand = [&](auto return_accumulated_storage) //
                                        {
                                            return [&, return_accumulated_storage]() -> void //
                                            {
                                                next_or_prev_impl_t next{p_next}, prev [[maybe_unused]]{p_prev};
                                                if (sequence->digit_middle() != digit_node_left->*next)
                                                    return_accumulated_storage(*(digit_node_left->*next)->p_accumulated_storage(), digit_node_left->*next);
                                                else
                                                {
                                                    if ((digit_node_left->*next)->tree_left != nullptr)
                                                        return_accumulated_storage(*(digit_node_left->*next)->tree_left->p_accumulated_storage(), (digit_node_left->*next)->tree_left);
                                                    else
                                                        ;
                                                }
                                            };
                                        };
                                        get_left_operand(branch_based_on_predicate(std::bind(this_, this_, std::placeholders::_1)))();
                                        get_middle_operand(branch_based_on_predicate(std::bind(this_, this_, std::placeholders::_1)))();
                                        get_right_operand(branch_based_on_predicate(std::bind(this_, this_, std::placeholders::_1)))();
                                    }
                                    else
                                    {
                                        pointer_digit_node_t digit_node_right = digit_node;
                                        auto get_left_operand = [&](auto return_accumulated_storage) //
                                        { return [&, return_accumulated_storage]() -> void //
                                          {
                                              next_or_prev_impl_t next [[maybe_unused]]{p_next}, prev{p_prev};
                                              if (sequence->digit_middle() != digit_node_right->*prev)
                                                  return_accumulated_storage(*(digit_node_right->*prev)->p_accumulated_storage(), digit_node_right->*prev);
                                              else
                                              {
                                                  if ((digit_node_right->*prev)->tree_right != nullptr)
                                                      return_accumulated_storage(*(digit_node_right->*prev)->tree_right->p_accumulated_storage(), (digit_node_right->*prev)->tree_right);
                                                  else
                                                      ;
                                              }
                                          }; };
                                        auto get_middle_operand = [&](auto return_accumulated_storage) //
                                        { return [&, return_accumulated_storage]() -> void //
                                          {
                                              if (digit_node_right->tree_left != nullptr)
                                              {
                                                  if (digit_node_right->digit_position != 0)
                                                      return_accumulated_storage(*digit_node_right->tree_left->p_accumulated_storage(), digit_node_right->tree_left);
                                                  else
                                                      return_accumulated_storage(std::as_const(*p_tree_node_to_p_list_node(digit_node_right->tree_left)->actual_projected_storage.p_projected_storage_or_p_element()), p_tree_node_to_p_list_node(digit_node_right->tree_left));
                                              }
                                              else
                                                  ;
                                          }; };
                                        auto get_right_operand = [&](auto return_accumulated_storage) //
                                        { return [&, return_accumulated_storage]() -> void //
                                          {
                                              if (digit_node_right->tree_right != nullptr)
                                              {
                                                  if (digit_node_right->digit_position != 0)
                                                      return_accumulated_storage(*digit_node_right->tree_right->p_accumulated_storage(), digit_node_right->tree_right);
                                                  else
                                                      return_accumulated_storage(std::as_const(*p_tree_node_to_p_list_node(digit_node_right->tree_right)->actual_projected_storage.p_projected_storage_or_p_element()), p_tree_node_to_p_list_node(digit_node_right->tree_right));
                                              }
                                              else
                                                  ;
                                          }; };
                                        get_left_operand(branch_based_on_predicate(std::bind(this_, this_, std::placeholders::_1)))();
                                        get_middle_operand(branch_based_on_predicate(std::bind(this_, this_, std::placeholders::_1)))();
                                        get_right_operand(branch_based_on_predicate(std::bind(this_, this_, std::placeholders::_1)))();
                                    }
                                }
                                else if constexpr (std::is_same_v<decltype(node), pointer_tree_node_t>)
                                {
                                    pointer_tree_node_t tree_node = node;
                                    auto get_left_operand = [&](auto return_accumulated_storage) //
                                    { return [&, return_accumulated_storage]() -> void //
                                      {
                                          if (tagged_ptr_bit0_is_setted(tree_node->child_left))
                                              return_accumulated_storage(std::as_const(*p_tree_node_to_p_list_node(tree_node->child_left)->actual_projected_storage.p_projected_storage_or_p_element()), p_tree_node_to_p_list_node(tree_node->child_left));
                                          else
                                              return_accumulated_storage(*tree_node->child_left->p_accumulated_storage(), tree_node->child_left);
                                      }; };
                                    auto get_right_operand = [&](auto return_accumulated_storage) //
                                    { return [&, return_accumulated_storage]() -> void //
                                      {
                                          if (tagged_ptr_bit0_is_setted(tree_node->child_right))
                                              return_accumulated_storage(std::as_const(*p_tree_node_to_p_list_node(tree_node->child_right)->actual_projected_storage.p_projected_storage_or_p_element()), p_tree_node_to_p_list_node(tree_node->child_right));
                                          else
                                              return_accumulated_storage(*tree_node->child_right->p_accumulated_storage(), tree_node->child_right);
                                      }; };
                                    get_left_operand(branch_based_on_predicate(std::bind(this_, this_, std::placeholders::_1)))();
                                    get_right_operand(branch_based_on_predicate(std::bind(this_, this_, std::placeholders::_1)))();
                                }
                                else if constexpr (std::is_same_v<decltype(node), pointer_list_node_t>)
                                {
                                    pointer_list_node_t list_node = node;
                                    *iterator_output_pointer_list_node++ = list_node;
                                }
                                else
                                {
                                    std::unreachable();
                                }
                            };
                            auto get_left_operand = [&](auto return_accumulated_storage) //
                            { return [&, return_accumulated_storage]() -> void //
                              {
                                  pointer_digit_node_t digit_front = pointer_traits_static_cast<pointer_digit_node_t>(static_cast<pointer_navigator_t>(sequence->digit_front()));
                                  if (digit_front != sequence->digit_middle())
                                      return_accumulated_storage(*digit_front->p_accumulated_storage(), static_cast<pointer_digit_node_t>(digit_front));
                                  else
                                  {
                                      if (digit_front->tree_left != nullptr)
                                      {
                                          if (digit_front->digit_position != 0)
                                              return_accumulated_storage(*(digit_front->tree_left)->p_accumulated_storage(), digit_front->tree_left);
                                          else
                                              return_accumulated_storage(std::as_const(*p_tree_node_to_p_list_node(digit_front->tree_left)->actual_projected_storage.p_projected_storage_or_p_element()), p_tree_node_to_p_list_node(digit_front->tree_left));
                                      }
                                      else
                                          ;
                                  }
                              }; };
                            auto get_right_operand = [&](auto return_accumulated_storage) //
                            { return [&, return_accumulated_storage]() -> void //
                              {
                                  pointer_digit_node_t digit_back = pointer_traits_static_cast<pointer_digit_node_t>(static_cast<pointer_navigator_t>(sequence->digit_back()));
                                  if (sequence->digit_middle() != digit_back)
                                      return_accumulated_storage(*digit_back->p_accumulated_storage(), static_cast<pointer_digit_node_t>(digit_back));
                                  else
                                  {
                                      if (digit_back->tree_right != nullptr)
                                      {
                                          if (digit_back->digit_position != 0)
                                              return_accumulated_storage(*(digit_back->tree_right)->p_accumulated_storage(), digit_back->tree_right);
                                          else
                                              return_accumulated_storage(std::as_const(*p_tree_node_to_p_list_node(digit_back->tree_right)->actual_projected_storage.p_projected_storage_or_p_element()), p_tree_node_to_p_list_node(digit_back->tree_right));
                                      }
                                      else
                                          ;
                                  }
                              }; };
                            get_left_operand(branch_based_on_predicate(std::bind(recursive_polymorphic_lambda, recursive_polymorphic_lambda, std::placeholders::_1)))();
                            get_right_operand(branch_based_on_predicate(std::bind(recursive_polymorphic_lambda, recursive_polymorphic_lambda, std::placeholders::_1)))();
                        }
                    }
                }

                template<typename heap_predicate_t> requires (invocable_r<heap_predicate_t, bool, accumulated_storage_t &> && invocable_r<heap_predicate_t, bool, typename sequence_t::projected_storage_or_element_t const &>)
                generator_t<pointer_navigator_t> find_by_heap_predicate_generator(heap_predicate_t const &heap_predicate) const
                {
                    if (sequence->digit_middle() == tagged_ptr_bit0_unsetted(sequence->digit_node_end)) // sequence is empty
                        co_return;
                    else
                    {
                        if (!heap_predicate(*tagged_ptr_bit0_unsetted(sequence->digit_node_end)->p_accumulated_storage())) // all elements are less than the value inside the predicate
                            co_return;
                        else
                        {
                            auto branch_based_on_predicate = [&](auto down) //
                            {
                                return [&heap_predicate, down](auto &accumulated_storage_or_projected_storage_or_element_covered_by_node, auto node) -> generator_t<pointer_navigator_t> //
                                {
                                    return [](auto &accumulated_storage_or_projected_storage_or_element_covered_by_node, auto node, auto &heap_predicate, auto down) -> generator_t<pointer_navigator_t> //
                                    {
                                        if (heap_predicate(accumulated_storage_or_projected_storage_or_element_covered_by_node))
                                            co_yield down(node);
                                    }(accumulated_storage_or_projected_storage_or_element_covered_by_node, node, heap_predicate, down);
                                };
                            };
                            auto recursive_polymorphic_lambda = [this, branch_based_on_predicate](auto &this_, auto node) -> generator_t<pointer_navigator_t> //
                            { return [](auto &this_, auto node, auto that, auto &branch_based_on_predicate) -> generator_t<pointer_navigator_t> //
                              {
                                  if constexpr (std::is_same_v<decltype(node), pointer_digit_node_t>)
                                  {
                                      pointer_digit_node_t digit_node = node;
                                      bool is_left_or_right = tagged_ptr_bit0_is_setted(digit_node->next) || pointer_traits_static_cast<pointer_digit_node_t>(digit_node->next)->digit_position < digit_node->digit_position;
                                      if (!is_left_or_right)
                                      {
                                          pointer_digit_node_t digit_node_left = digit_node;
                                          auto get_left_operand = [&](auto return_accumulated_storage) //
                                          { return [](pointer_digit_node_t digit_node_left, auto return_accumulated_storage) -> generator_t<pointer_navigator_t> //
                                            {
                                                if (digit_node_left->tree_left != nullptr)
                                                {
                                                    if (digit_node_left->digit_position != 0)
                                                        co_yield return_accumulated_storage(*digit_node_left->tree_left->p_accumulated_storage(), digit_node_left->tree_left);
                                                    else
                                                        co_yield return_accumulated_storage(std::as_const(*p_tree_node_to_p_list_node(digit_node_left->tree_left)->actual_projected_storage.p_projected_storage_or_p_element()), p_tree_node_to_p_list_node(digit_node_left->tree_left));
                                                }
                                                else
                                                    ;
                                            }(digit_node_left, return_accumulated_storage); };
                                          auto get_middle_operand = [&](auto return_accumulated_storage) //
                                          { return [](pointer_digit_node_t digit_node_left, auto return_accumulated_storage) -> generator_t<pointer_navigator_t> //
                                            {
                                                if (digit_node_left->tree_right != nullptr)
                                                {
                                                    if (digit_node_left->digit_position != 0)
                                                        co_yield return_accumulated_storage(*digit_node_left->tree_right->p_accumulated_storage(), digit_node_left->tree_right);
                                                    else
                                                        co_yield return_accumulated_storage(std::as_const(*p_tree_node_to_p_list_node(digit_node_left->tree_right)->actual_projected_storage.p_projected_storage_or_p_element()), p_tree_node_to_p_list_node(digit_node_left->tree_right));
                                                }
                                                else
                                                    ;
                                            }(digit_node_left, return_accumulated_storage); };
                                          auto get_right_operand = [&](auto return_accumulated_storage) //
                                          {
                                              return [](pointer_digit_node_t digit_node_left, auto return_accumulated_storage, auto that) -> generator_t<pointer_navigator_t> //
                                              {
                                                  next_or_prev_impl_t next{that->p_next}, prev [[maybe_unused]]{that->p_prev};
                                                  if (that->sequence->digit_middle() != digit_node_left->*next)
                                                      co_yield return_accumulated_storage(*(digit_node_left->*next)->p_accumulated_storage(), digit_node_left->*next);
                                                  else
                                                  {
                                                      if ((digit_node_left->*next)->tree_left != nullptr)
                                                          co_yield return_accumulated_storage(*(digit_node_left->*next)->tree_left->p_accumulated_storage(), (digit_node_left->*next)->tree_left);
                                                      else
                                                          ;
                                                  }
                                              }(digit_node_left, return_accumulated_storage, that);
                                          };
                                          co_yield get_left_operand(branch_based_on_predicate(std::bind(this_, this_, std::placeholders::_1)));
                                          co_yield get_middle_operand(branch_based_on_predicate(std::bind(this_, this_, std::placeholders::_1)));
                                          co_yield get_right_operand(branch_based_on_predicate(std::bind(this_, this_, std::placeholders::_1)));
                                      }
                                      else
                                      {
                                          pointer_digit_node_t digit_node_right = digit_node;
                                          auto get_left_operand = [&](auto return_accumulated_storage) //
                                          { return [](pointer_digit_node_t digit_node_right, auto return_accumulated_storage, auto that) -> generator_t<pointer_navigator_t> //
                                            {
                                                next_or_prev_impl_t next [[maybe_unused]]{that->p_next}, prev{that->p_prev};
                                                if (that->sequence->digit_middle() != digit_node_right->*prev)
                                                    co_yield return_accumulated_storage(*(digit_node_right->*prev)->p_accumulated_storage(), digit_node_right->*prev);
                                                else
                                                {
                                                    if ((digit_node_right->*prev)->tree_right != nullptr)
                                                        co_yield return_accumulated_storage(*(digit_node_right->*prev)->tree_right->p_accumulated_storage(), (digit_node_right->*prev)->tree_right);
                                                    else
                                                        ;
                                                }
                                            }(digit_node_right, return_accumulated_storage, that); };
                                          auto get_middle_operand = [&](auto return_accumulated_storage) //
                                          { return [](pointer_digit_node_t digit_node_right, auto return_accumulated_storage) -> generator_t<pointer_navigator_t> //
                                            {
                                                if (digit_node_right->tree_left != nullptr)
                                                {
                                                    if (digit_node_right->digit_position != 0)
                                                        co_yield return_accumulated_storage(*digit_node_right->tree_left->p_accumulated_storage(), digit_node_right->tree_left);
                                                    else
                                                        co_yield return_accumulated_storage(std::as_const(*p_tree_node_to_p_list_node(digit_node_right->tree_left)->actual_projected_storage.p_projected_storage_or_p_element()), p_tree_node_to_p_list_node(digit_node_right->tree_left));
                                                }
                                                else
                                                    ;
                                            }(digit_node_right, return_accumulated_storage); };
                                          auto get_right_operand = [&](auto return_accumulated_storage) //
                                          { return [](pointer_digit_node_t digit_node_right, auto return_accumulated_storage) -> generator_t<pointer_navigator_t> //
                                            {
                                                if (digit_node_right->tree_right != nullptr)
                                                {
                                                    if (digit_node_right->digit_position != 0)
                                                        co_yield return_accumulated_storage(*digit_node_right->tree_right->p_accumulated_storage(), digit_node_right->tree_right);
                                                    else
                                                        co_yield return_accumulated_storage(std::as_const(*p_tree_node_to_p_list_node(digit_node_right->tree_right)->actual_projected_storage.p_projected_storage_or_p_element()), p_tree_node_to_p_list_node(digit_node_right->tree_right));
                                                }
                                                else
                                                    ;
                                            }(digit_node_right, return_accumulated_storage); };
                                          co_yield get_left_operand(branch_based_on_predicate(std::bind(this_, this_, std::placeholders::_1)));
                                          co_yield get_middle_operand(branch_based_on_predicate(std::bind(this_, this_, std::placeholders::_1)));
                                          co_yield get_right_operand(branch_based_on_predicate(std::bind(this_, this_, std::placeholders::_1)));
                                      }
                                  }
                                  else if constexpr (std::is_same_v<decltype(node), pointer_tree_node_t>)
                                  {
                                      pointer_tree_node_t tree_node = node;
                                      auto get_left_operand = [&](auto return_accumulated_storage) //
                                      { return [](pointer_tree_node_t tree_node, auto return_accumulated_storage) -> generator_t<pointer_navigator_t> //
                                        {
                                            if (tagged_ptr_bit0_is_setted(tree_node->child_left))
                                                co_yield return_accumulated_storage(std::as_const(*p_tree_node_to_p_list_node(tree_node->child_left)->actual_projected_storage.p_projected_storage_or_p_element()), p_tree_node_to_p_list_node(tree_node->child_left));
                                            else
                                                co_yield return_accumulated_storage(*tree_node->child_left->p_accumulated_storage(), tree_node->child_left);
                                        }(tree_node, return_accumulated_storage); };
                                      auto get_right_operand = [&](auto return_accumulated_storage) //
                                      { return [](pointer_tree_node_t tree_node, auto return_accumulated_storage) -> generator_t<pointer_navigator_t> //
                                        {
                                            if (tagged_ptr_bit0_is_setted(tree_node->child_right))
                                                co_yield return_accumulated_storage(std::as_const(*p_tree_node_to_p_list_node(tree_node->child_right)->actual_projected_storage.p_projected_storage_or_p_element()), p_tree_node_to_p_list_node(tree_node->child_right));
                                            else
                                                co_yield return_accumulated_storage(*tree_node->child_right->p_accumulated_storage(), tree_node->child_right);
                                        }(tree_node, return_accumulated_storage); };
                                      co_yield get_left_operand(branch_based_on_predicate(std::bind(this_, this_, std::placeholders::_1)));
                                      co_yield get_right_operand(branch_based_on_predicate(std::bind(this_, this_, std::placeholders::_1)));
                                  }
                                  else if constexpr (std::is_same_v<decltype(node), pointer_list_node_t>)
                                  {
                                      pointer_list_node_t list_node = node;
                                      co_yield list_node;
                                  }
                                  else
                                  {
                                      std::unreachable();
                                  }
                              }(this_, node, this, branch_based_on_predicate); };
                            auto get_left_operand = [&](auto return_accumulated_storage) //
                            { return [](auto that, auto return_accumulated_storage) -> generator_t<pointer_navigator_t> //
                              {
                                  pointer_digit_node_t digit_front = pointer_traits_static_cast<pointer_digit_node_t>(static_cast<pointer_navigator_t>(that->sequence->digit_front()));
                                  if (digit_front != that->sequence->digit_middle())
                                      co_yield return_accumulated_storage(*digit_front->p_accumulated_storage(), static_cast<pointer_digit_node_t>(digit_front));
                                  else
                                  {
                                      if (digit_front->tree_left != nullptr)
                                      {
                                          if (digit_front->digit_position != 0)
                                              co_yield return_accumulated_storage(*(digit_front->tree_left)->p_accumulated_storage(), digit_front->tree_left);
                                          else
                                              co_yield return_accumulated_storage(std::as_const(*p_tree_node_to_p_list_node(digit_front->tree_left)->actual_projected_storage.p_projected_storage_or_p_element()), p_tree_node_to_p_list_node(digit_front->tree_left));
                                      }
                                      else
                                          ;
                                  }
                              }(this, return_accumulated_storage); };
                            auto get_right_operand = [&](auto return_accumulated_storage) //
                            { return [](auto that, auto return_accumulated_storage) -> generator_t<pointer_navigator_t> //
                              {
                                  pointer_digit_node_t digit_back = pointer_traits_static_cast<pointer_digit_node_t>(static_cast<pointer_navigator_t>(that->sequence->digit_back()));
                                  if (that->sequence->digit_middle() != digit_back)
                                      co_yield return_accumulated_storage(*digit_back->p_accumulated_storage(), static_cast<pointer_digit_node_t>(digit_back));
                                  else
                                  {
                                      if (digit_back->tree_right != nullptr)
                                      {
                                          if (digit_back->digit_position != 0)
                                              co_yield return_accumulated_storage(*(digit_back->tree_right)->p_accumulated_storage(), digit_back->tree_right);
                                          else
                                              co_yield return_accumulated_storage(std::as_const(*p_tree_node_to_p_list_node(digit_back->tree_right)->actual_projected_storage.p_projected_storage_or_p_element()), p_tree_node_to_p_list_node(digit_back->tree_right));
                                      }
                                      else
                                          ;
                                  }
                              }(this, return_accumulated_storage); };
                            co_yield get_left_operand(branch_based_on_predicate(std::bind(recursive_polymorphic_lambda, recursive_polymorphic_lambda, std::placeholders::_1)));
                            co_yield get_right_operand(branch_based_on_predicate(std::bind(recursive_polymorphic_lambda, recursive_polymorphic_lambda, std::placeholders::_1)));
                        }
                    }
                }
            };

            template<bool is_reversed_, typename allocator_element_t, typename sequence_possibly_const_t>
            static sequence_functor_t<std::is_const_v<std::remove_pointer_t<sequence_possibly_const_t>>, is_reversed_, allocator_element_t, std::remove_const_t<std::remove_pointer_t<sequence_possibly_const_t>>> make_sequence_functor(allocator_element_t const &allocator_element, sequence_possibly_const_t *sequence)
            {
                return sequence_functor_t<std::is_const_v<std::remove_pointer_t<sequence_possibly_const_t>>, is_reversed_, allocator_element_t, std::remove_const_t<std::remove_pointer_t<sequence_possibly_const_t>>>{
                    .allocator_element = allocator_element,
                    .sequence = sequence,
                };
            }

            template<std::size_t requested_stride_>
            struct add_stride_member
            {
                static constexpr std::size_t stride = requested_stride_;
                static constexpr std::size_t requested_stride = requested_stride_;
            };
            template<>
            struct add_stride_member<std::numeric_limits<std::size_t>::max()>
            {
                std::size_t stride = 1;
                static constexpr std::size_t requested_stride = std::numeric_limits<std::size_t>::max();
            };
        } // namespace augmented_deque
    } // namespace detail

    namespace augmented_deque_helpers
    {
        template<typename projecting_n_ary_functor_t>
        struct extract_builtin_projected_storage_from_projecting_n_ary_functor : std::type_identity<void>
        {};
        template<typename projecting_n_ary_functor_t> requires requires { typename projecting_n_ary_functor_t::projected_storage_t; }
        struct extract_builtin_projected_storage_from_projecting_n_ary_functor<projecting_n_ary_functor_t> : std::type_identity<typename projecting_n_ary_functor_t::projected_storage_t>
        {};
        template<typename projecting_n_ary_functor_t>
        using extract_builtin_projected_storage_from_projecting_n_ary_functor_t = typename extract_builtin_projected_storage_from_projecting_n_ary_functor<projecting_n_ary_functor_t>::type;

        template<typename projecting_n_ary_functor_t_ = void, typename projected_storage_t_ = extract_builtin_projected_storage_from_projecting_n_ary_functor_t<projecting_n_ary_functor_t_>>
        struct projector_wrapping_projecting_n_ary_functor_t
        {
            using projected_storage_t = void; // void means to skip the project operation and use the value itself to accumulate, only allowed when requested_stride==1
        };
        template<typename projecting_n_ary_functor_t_, typename projected_storage_t_> requires (!std::is_same_v<projected_storage_t_, void>)
        struct projector_wrapping_projecting_n_ary_functor_t<projecting_n_ary_functor_t_, projected_storage_t_>
        {
            using projected_storage_t = projected_storage_t_;
            projecting_n_ary_functor_t_ project_n_ary_functor;

            template<typename allocator_element_t, typename it_projected_storage_t, typename it_element_t> // when requested_stride==1
            void construct_projected_storage(allocator_element_t const &allocator_element, typename std::pointer_traits<typename std::allocator_traits<allocator_element_t>::pointer>::template rebind<projected_storage_t> pointer_projected_storage, [[maybe_unused]] it_projected_storage_t it_this_list_node, it_element_t it_also_this_list_node) const
            {
                using allocator_projected_storage_t = typename std::allocator_traits<allocator_element_t>::template rebind_alloc<projected_storage_t>;
                std::allocator_traits<allocator_projected_storage_t>::construct(detail::utility::unmove(allocator_projected_storage_t(allocator_element)), std::to_address(pointer_projected_storage), project_n_ary_functor(it_also_this_list_node));
            }

            template<typename allocator_element_t, typename it_element_t> // when requested_stride>1
            projected_storage_t construct_projected_storage([[maybe_unused]] allocator_element_t const &allocator_element, it_element_t it_chunk_begin, it_element_t it_chunk_end, std::size_t chunk_size) const
            {
                assert(chunk_size != 0);
                return projected_storage_t(project_n_ary_functor(it_chunk_begin, it_chunk_end, chunk_size));
            }
            template<typename allocator_element_t, typename it_projected_storage_t, typename it_element_t> // when requested_stride>1
            void construct_projected_storage(allocator_element_t const &allocator_element, typename std::pointer_traits<typename std::allocator_traits<allocator_element_t>::pointer>::template rebind<projected_storage_t> pointer_projected_storage, [[maybe_unused]] it_projected_storage_t it_this_list_node, it_element_t it_chunk_begin, it_element_t it_chunk_end, std::size_t chunk_size) const
            {
                assert(chunk_size != 0);
                using allocator_projected_storage_t = typename std::allocator_traits<allocator_element_t>::template rebind_alloc<projected_storage_t>;
                std::allocator_traits<allocator_projected_storage_t>::construct(detail::utility::unmove(allocator_projected_storage_t(allocator_element)), std::to_address(pointer_projected_storage), project_n_ary_functor(it_chunk_begin, it_chunk_end, chunk_size));
            }

            template<typename allocator_element_t>
            void destroy_projected_storage(allocator_element_t const &allocator_element, typename std::pointer_traits<typename std::allocator_traits<allocator_element_t>::pointer>::template rebind<projected_storage_t> pointer_projected_storage) const
            {
                using allocator_projected_storage_t = typename std::allocator_traits<allocator_element_t>::template rebind_alloc<projected_storage_t>;
                std::allocator_traits<allocator_projected_storage_t>::destroy(detail::utility::unmove(allocator_projected_storage_t(allocator_element)), std::to_address(pointer_projected_storage));
            }

            template<typename it_projected_storage_t, typename it_element_t> // when requested_stride==1
            bool update_projected_storage(projected_storage_t &value, [[maybe_unused]] it_projected_storage_t it_this_list_node, it_element_t it_also_this_list_node) const
            {
                projected_storage_t value_new(project_n_ary_functor(it_also_this_list_node));
                if (value == value_new)
                    return false;
                else
                {
                    value = value_new;
                    return true;
                }
            }

            template<typename it_projected_storage_t, typename it_element_t> // when requested_stride>1
            bool update_projected_storage(projected_storage_t &value, [[maybe_unused]] it_projected_storage_t it_this_list_node, it_element_t it_chunk_begin, it_element_t it_chunk_end, std::size_t chunk_size) const
            {
                assert(chunk_size != 0);
                projected_storage_t value_new(project_n_ary_functor(it_chunk_begin, it_chunk_end, chunk_size));
                if (value == value_new)
                    return false;
                else
                {
                    value = value_new;
                    return true;
                }
            }
        };

        template<typename accumulating_binary_functor_t>
        struct extract_builtin_accumulated_storage_from_accumulating_binary_functor : std::type_identity<void>
        {};
        template<typename accumulating_binary_functor_t> requires requires { typename accumulating_binary_functor_t::accumulated_storage_t; }
        struct extract_builtin_accumulated_storage_from_accumulating_binary_functor<accumulating_binary_functor_t> : std::type_identity<typename accumulating_binary_functor_t::accumulated_storage_t>
        {};
        template<typename accumulating_binary_functor_t>
        using extract_builtin_accumulated_storage_from_accumulating_binary_functor_t = typename extract_builtin_accumulated_storage_from_accumulating_binary_functor<accumulating_binary_functor_t>::type;

        template<std::size_t index, typename accumulating_binary_functor_t, typename Tuple>
        decltype(auto) tuple_fold(accumulating_binary_functor_t const &accumulate_binary_functor, Tuple const &t) // https://stackoverflow.com/questions/47216057/how-to-write-a-fold-sum-function-for-c-tuple
        {
            if constexpr (index == 0)
            {
                if constexpr (requires { accumulate_binary_functor.identity_element(); })
                    return accumulate_binary_functor(accumulate_binary_functor.identity_element(), std::get<index>(t));
                else
                    return std::get<index>(t);
            }
            else
                return accumulate_binary_functor(tuple_fold<index - 1>(accumulate_binary_functor, t), std::get<index>(t));
        }

        template<typename accumulating_binary_functor_t_ = void>
        struct accumulator_wrapping_accumulating_binary_functor_t
        {
            using accumulated_storage_t = void; // void means to skip the accumulate operation
        };
        template<typename accumulating_binary_functor_t_> requires (!std::is_same_v<extract_builtin_accumulated_storage_from_accumulating_binary_functor_t<accumulating_binary_functor_t_>, void>)
        struct accumulator_wrapping_accumulating_binary_functor_t<accumulating_binary_functor_t_>
        {
            using accumulated_storage_t = extract_builtin_accumulated_storage_from_accumulating_binary_functor_t<accumulating_binary_functor_t_>;
            accumulating_binary_functor_t_ accumulate_binary_functor;

            template<typename allocator_element_t, typename... Args>
            accumulated_storage_t construct_accumulated_storage([[maybe_unused]] allocator_element_t const &allocator_element, std::tuple<Args &...> const &values) const
            {
                if constexpr (sizeof...(Args) == 0)
                {
                    if constexpr (requires { accumulate_binary_functor.identity_element(); })
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
                if constexpr (sizeof...(Args) == 0)
                {
                    if constexpr (requires { accumulate_binary_functor.identity_element(); })
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
                auto compare_and_update = [&value](accumulated_storage_t const &value_new) //
                {
                    if constexpr (requires { value == value_new; })
                    {
                        if (value == value_new)
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
                if constexpr (sizeof...(Args) == 0)
                {
                    if constexpr (requires { accumulate_binary_functor.identity_element(); })
                        return compare_and_update(accumulated_storage_t(accumulate_binary_functor.identity_element()));
                    else
                        return compare_and_update(accumulated_storage_t());
                }
                else
                    return compare_and_update(accumulated_storage_t(tuple_fold<sizeof...(Args) - 1>(accumulate_binary_functor, values)));
            }
        };

        template<typename projecting_n_ary_functor_t = void, typename accumulating_binary_functor_t = void>
        struct projector_and_accumulator_wrapping_projecting_and_accumulating_functors_t :
            projector_wrapping_projecting_n_ary_functor_t<projecting_n_ary_functor_t>,
            accumulator_wrapping_accumulating_binary_functor_t<accumulating_binary_functor_t>
        {
            using projector_t = projector_wrapping_projecting_n_ary_functor_t<projecting_n_ary_functor_t>;
            using accumulator_t = accumulator_wrapping_accumulating_binary_functor_t<accumulating_binary_functor_t>;
        };

        template<typename element_t_, std::strict_weak_order<element_t_, element_t_> binary_comparator_t, bool min_or_max = false>
        struct projecting_n_ary_functor_wrapping_binary_comparator_t
        {
            using projected_storage_t = element_t_;
            binary_comparator_t binary_functor;

            template<typename It_Chunk>
            projected_storage_t operator()(It_Chunk it_chunk_begin, It_Chunk it_chunk_end, [[maybe_unused]] std::size_t chunk_size) const
            {
                assert(it_chunk_begin != it_chunk_end);
                if constexpr (!min_or_max)
                    return *std::ranges::min_element(it_chunk_begin, it_chunk_end, binary_functor);
                else
                    return *std::ranges::max_element(it_chunk_begin, it_chunk_end, binary_functor);
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
        struct accumulating_binary_functor_wrapping_homogeneous_binary_functor_t : homogeneous_binary_functor_t, add_identity_element_member<identity_element_c>
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

        template<typename element_t>
        using empty_projector_and_accumulator_t = projector_and_accumulator_wrapping_projecting_and_accumulating_functors_t<
            void,
            void //
            >;
        static_assert(std::is_same_v<empty_projector_and_accumulator_t<int>::projected_storage_t, void>);
        /**/ static_assert(std::is_same_v<empty_projector_and_accumulator_t<int>::projector_t, projector_wrapping_projecting_n_ary_functor_t<void>>);
        /**/ static_assert(std::is_same_v<empty_projector_and_accumulator_t<int>::projector_t::projected_storage_t, void>);
        static_assert(std::is_same_v<empty_projector_and_accumulator_t<int>::accumulated_storage_t, void>);
        /**/ static_assert(std::is_same_v<empty_projector_and_accumulator_t<int>::accumulator_t, accumulator_wrapping_accumulating_binary_functor_t<void>>);
        /**/ static_assert(std::is_same_v<empty_projector_and_accumulator_t<int>::accumulator_t::accumulated_storage_t, void>);

        template<typename element_t>
        using example_stride1_chunk1_projector_skipped_and_accumulator_plus_t = projector_and_accumulator_wrapping_projecting_and_accumulating_functors_t<
            void,
            accumulating_binary_functor_wrapping_homogeneous_binary_functor_t<element_t, std::plus<element_t>> //
            >;
        static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_plus_t<int>::projected_storage_t, void>);
        /**/ static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_plus_t<int>::projector_t, projector_wrapping_projecting_n_ary_functor_t<void>>);
        /**/ static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_plus_t<int>::projector_t::projected_storage_t, void>);
        static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_plus_t<int>::accumulated_storage_t, int>);
        /**/ static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_plus_t<int>::accumulator_t, accumulator_wrapping_accumulating_binary_functor_t<accumulating_binary_functor_wrapping_homogeneous_binary_functor_t<int, std::plus<int>>>>);
        /**/ static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_plus_t<int>::accumulator_t::accumulated_storage_t, int>);

        static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_plus_t<std::string>::projected_storage_t, void>);
        /**/ static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_plus_t<std::string>::projector_t, projector_wrapping_projecting_n_ary_functor_t<void>>);
        /**/ static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_plus_t<std::string>::projector_t::projected_storage_t, void>);
        static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_plus_t<std::string>::accumulated_storage_t, std::string>);
        /**/ static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_plus_t<std::string>::accumulator_t, accumulator_wrapping_accumulating_binary_functor_t<accumulating_binary_functor_wrapping_homogeneous_binary_functor_t<std::string, std::plus<std::string>>>>);
        /**/ static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_plus_t<std::string>::accumulator_t::accumulated_storage_t, std::string>);

        template<typename element_t>
        using example_stride1_chunk1_projector_skipped_and_accumulator_logical_and_t = projector_and_accumulator_wrapping_projecting_and_accumulating_functors_t<
            void,
            accumulating_binary_functor_wrapping_homogeneous_binary_functor_t<element_t, std::logical_and<element_t>, std::true_type> //
            >;
        static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_logical_and_t<bool>::projected_storage_t, void>);
        /**/ static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_logical_and_t<bool>::projector_t, projector_wrapping_projecting_n_ary_functor_t<void>>);
        /**/ static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_logical_and_t<bool>::projector_t::projected_storage_t, void>);
        static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_logical_and_t<bool>::accumulated_storage_t, bool>);
        /**/ static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_logical_and_t<bool>::accumulator_t, accumulator_wrapping_accumulating_binary_functor_t<accumulating_binary_functor_wrapping_homogeneous_binary_functor_t<bool, std::logical_and<bool>, std::true_type>>>);
        /**/ static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_logical_and_t<bool>::accumulator_t::accumulated_storage_t, bool>);
        /**/ static_assert(std::type_identity_t<accumulating_binary_functor_wrapping_homogeneous_binary_functor_t<bool, std::logical_and<bool>, std::true_type> const>().identity_element() == true);
        template<typename element_t>
        using example_stride1_chunk1_projector_skipped_and_accumulator_logical_or_t = projector_and_accumulator_wrapping_projecting_and_accumulating_functors_t<
            void,
            accumulating_binary_functor_wrapping_homogeneous_binary_functor_t<element_t, std::logical_or<element_t>, std::false_type> //
            >;
        static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_logical_or_t<bool>::projected_storage_t, void>);
        /**/ static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_logical_or_t<bool>::projector_t, projector_wrapping_projecting_n_ary_functor_t<void>>);
        /**/ static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_logical_or_t<bool>::projector_t::projected_storage_t, void>);
        static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_logical_or_t<bool>::accumulated_storage_t, bool>);
        /**/ static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_logical_or_t<bool>::accumulator_t, accumulator_wrapping_accumulating_binary_functor_t<accumulating_binary_functor_wrapping_homogeneous_binary_functor_t<bool, std::logical_or<bool>, std::false_type>>>);
        /**/ static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_logical_or_t<bool>::accumulator_t::accumulated_storage_t, bool>);
        /**/ static_assert(std::type_identity_t<accumulating_binary_functor_wrapping_homogeneous_binary_functor_t<bool, std::logical_or<bool>, std::false_type> const>().identity_element() == false);

        template<typename element_t, typename sequence_t>
        using example_stride1_chunk1_projector_skipped_and_accumulator_sequence_t = projector_and_accumulator_wrapping_projecting_and_accumulating_functors_t<
            void,
            accumulating_sequence_binary_functor_t<element_t, sequence_t> //
            >;
        static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_sequence_t<char, std::string>::projected_storage_t, void>);
        /**/ static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_sequence_t<char, std::string>::projector_t, projector_wrapping_projecting_n_ary_functor_t<void>>);
        /**/ static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_sequence_t<char, std::string>::projector_t::projected_storage_t, void>);
        static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_sequence_t<char, std::string>::accumulated_storage_t, std::string>);
        /**/ static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_sequence_t<char, std::string>::accumulator_t, accumulator_wrapping_accumulating_binary_functor_t<accumulating_sequence_binary_functor_t<char, std::string>>>);
        /**/ static_assert(std::is_same_v<example_stride1_chunk1_projector_skipped_and_accumulator_sequence_t<char, std::string>::accumulator_t::accumulated_storage_t, std::string>);

        template<typename element_t>
        using example_chunkgt1_projector_min_element_and_accumulator_max_t = projector_and_accumulator_wrapping_projecting_and_accumulating_functors_t<
            projecting_n_ary_functor_wrapping_binary_comparator_t<element_t, std::less<element_t>>,
            accumulating_binary_functor_wrapping_homogeneous_binary_functor_t<element_t, max_t<element_t>> //
            >;
        static_assert(std::is_same_v<example_chunkgt1_projector_min_element_and_accumulator_max_t<int>::projected_storage_t, int>);
        /**/ static_assert(std::is_same_v<example_chunkgt1_projector_min_element_and_accumulator_max_t<int>::projector_t, projector_wrapping_projecting_n_ary_functor_t<projecting_n_ary_functor_wrapping_binary_comparator_t<int, std::less<int>>>>);
        /**/ static_assert(std::is_same_v<example_chunkgt1_projector_min_element_and_accumulator_max_t<int>::projector_t::projected_storage_t, int>);
        static_assert(std::is_same_v<example_chunkgt1_projector_min_element_and_accumulator_max_t<int>::accumulated_storage_t, int>);
        /**/ static_assert(std::is_same_v<example_chunkgt1_projector_min_element_and_accumulator_max_t<int>::accumulator_t, accumulator_wrapping_accumulating_binary_functor_t<accumulating_binary_functor_wrapping_homogeneous_binary_functor_t<int, max_t<int>>>>);
        /**/ static_assert(std::is_same_v<example_chunkgt1_projector_min_element_and_accumulator_max_t<int>::accumulator_t::accumulated_storage_t, int>);
        template<typename element_t>
        using example_chunkgt1_projector_max_element_and_accumulator_min_t = projector_and_accumulator_wrapping_projecting_and_accumulating_functors_t<
            projecting_n_ary_functor_wrapping_binary_comparator_t<element_t, std::less<element_t>, true>,
            accumulating_binary_functor_wrapping_homogeneous_binary_functor_t<element_t, min_t<element_t>> //
            >;
        static_assert(std::is_same_v<example_chunkgt1_projector_max_element_and_accumulator_min_t<int>::projected_storage_t, int>);
        /**/ static_assert(std::is_same_v<example_chunkgt1_projector_max_element_and_accumulator_min_t<int>::projector_t, projector_wrapping_projecting_n_ary_functor_t<projecting_n_ary_functor_wrapping_binary_comparator_t<int, std::less<int>, true>>>);
        /**/ static_assert(std::is_same_v<example_chunkgt1_projector_max_element_and_accumulator_min_t<int>::projector_t::projected_storage_t, int>);
        static_assert(std::is_same_v<example_chunkgt1_projector_max_element_and_accumulator_min_t<int>::accumulated_storage_t, int>);
        /**/ static_assert(std::is_same_v<example_chunkgt1_projector_max_element_and_accumulator_min_t<int>::accumulator_t, accumulator_wrapping_accumulating_binary_functor_t<accumulating_binary_functor_wrapping_homogeneous_binary_functor_t<int, min_t<int>>>>);
        /**/ static_assert(std::is_same_v<example_chunkgt1_projector_max_element_and_accumulator_min_t<int>::accumulator_t::accumulated_storage_t, int>);
    } // namespace augmented_deque_helpers

    template<
        typename element_t_,
        typename allocator_element_t_ = std::allocator<element_t_>,
        typename requested_stride_to_projector_and_accumulator_map_t_ = std::tuple<> //
        >
    struct augmented_deque_t
    {
        using element_t = element_t_;
        using allocator_element_t = allocator_element_t_;
        using pointer_element_t = typename std::allocator_traits<allocator_element_t>::pointer;
        using requested_stride_to_projector_and_accumulator_map_t = requested_stride_to_projector_and_accumulator_map_t_;

        using value_type = element_t;
        using allocator_type = allocator_element_t;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type &;
        using const_reference = value_type const &;
        using pointer = typename std::allocator_traits<allocator_type>::pointer;
        using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;

        using not_empty_stride_to_projector_and_accumulator_map_t = std::conditional_t<
            std::tuple_size_v<requested_stride_to_projector_and_accumulator_map_t> == 0,
            std::tuple<std::pair<std::integral_constant<std::size_t, 1>, augmented_deque_helpers::empty_projector_and_accumulator_t<element_t>>>,
            requested_stride_to_projector_and_accumulator_map_t>;
        using stride_to_projector_and_accumulator_map_t = std::conditional_t<
            std::is_same_v<typename std::tuple_element_t<0, not_empty_stride_to_projector_and_accumulator_map_t>::first_type, std::integral_constant<std::size_t, 1>>,
            not_empty_stride_to_projector_and_accumulator_map_t,
            detail::utility::tuple_cat_t<std::tuple<std::pair<std::integral_constant<std::size_t, 1>, augmented_deque_helpers::empty_projector_and_accumulator_t<element_t>>>, not_empty_stride_to_projector_and_accumulator_map_t>>;

        template<std::size_t sequence_index, std::size_t requested_stride, typename projector_and_accumulator_t>
        struct augmented_deque_sequence_t;
        template<std::size_t sequence_index, std::size_t requested_stride, typename projector_and_accumulator_t_> requires (sequence_index != 0)
        struct augmented_deque_sequence_t<sequence_index, requested_stride, projector_and_accumulator_t_>;
        template<typename projector_and_accumulator_t_>
        struct augmented_deque_sequence_t<0, 1, projector_and_accumulator_t_>;

        using stride1_projector_and_accumulator_t = typename std::tuple_element_t<0, stride_to_projector_and_accumulator_map_t>::second_type;
        using stride1_sequence_t = augmented_deque_sequence_t<0, 1, stride1_projector_and_accumulator_t>;

        using other_strides_stride_to_projector_and_accumulator_map_t = detail::utility::list_without_first_t<stride_to_projector_and_accumulator_map_t>;
        template<std::size_t index, typename stride_to_projector_and_accumulator_item_t>
        struct stride_to_projector_and_accumulator_item_to_sequence : std::type_identity<augmented_deque_sequence_t<index + 1, stride_to_projector_and_accumulator_item_t::first_type::value, typename stride_to_projector_and_accumulator_item_t::second_type>>
        {};
        using other_stride_sequences_t = detail::utility::map_transform_t<other_strides_stride_to_projector_and_accumulator_map_t, stride_to_projector_and_accumulator_item_to_sequence>;

        template<std::size_t sequence_index_, std::size_t requested_stride_, typename projector_and_accumulator_t_> requires (sequence_index_ != 0)
        struct augmented_deque_sequence_t<sequence_index_, requested_stride_, projector_and_accumulator_t_> : detail::augmented_deque::add_stride_member<requested_stride_>
        {
            static constexpr std::size_t sequence_index = sequence_index_;
            static constexpr std::size_t requested_stride = requested_stride_;
            using projector_and_accumulator_t = projector_and_accumulator_t_;

            using stride1_sequence_t = augmented_deque_t::stride1_sequence_t;

            using projected_storage_t = typename projector_and_accumulator_t::projected_storage_t;
            using projected_storage_or_element_t = std::conditional_t<!std::is_same_v<projected_storage_t, void>, projected_storage_t, element_t>;
            using pointer_projected_storage_or_pointer_element_t = typename std::pointer_traits<pointer_element_t>::template rebind<projected_storage_or_element_t>;
            using accumulated_storage_t = typename projector_and_accumulator_t::accumulated_storage_t;
            static_assert((requested_stride_ == 1 && (!std::is_same_v<projected_storage_t, void> || !std::is_same_v<accumulated_storage_t, void>)) || (requested_stride_ != 1 && !std::is_same_v<projected_storage_t, void>));
            struct sequence_config_t
            {
                using element_t = augmented_deque_t::element_t;
                using allocator_element_t = augmented_deque_t::allocator_element_t;
                using pointer_element_t = augmented_deque_t::pointer_element_t;

                static constexpr std::size_t sequence_index = augmented_deque_sequence_t::sequence_index;
                static constexpr std::size_t requested_stride = augmented_deque_sequence_t::requested_stride;
                using projector_and_accumulator_t = augmented_deque_sequence_t::projector_and_accumulator_t;

                using stride1_sequence_t = augmented_deque_sequence_t::stride1_sequence_t;

                using projected_storage_t = augmented_deque_sequence_t::projected_storage_t;
                using projected_storage_or_element_t = augmented_deque_sequence_t::projected_storage_or_element_t;
                using pointer_projected_storage_or_pointer_element_t = augmented_deque_sequence_t::pointer_projected_storage_or_pointer_element_t;
                using accumulated_storage_t = augmented_deque_sequence_t::accumulated_storage_t;
                struct actual_projected_storage_t : detail::augmented_deque::add_projected_storage_member_t<pointer_element_t, projected_storage_t>
                {
                    typename std::pointer_traits<pointer_element_t>::template rebind<typename stride1_sequence_t::list_node_t> child;

                    pointer_element_t p_element() { return child->actual_projected_storage.p_element(); }
                    pointer_projected_storage_or_pointer_element_t p_projected_storage_or_p_element()
                    {
                        if constexpr (!std::is_same_v<projected_storage_t, void>) return this->p_projected_storage();
                        else return p_element();
                    }
                };
                struct actual_projected_storage_end_t
                {
                    typename std::pointer_traits<pointer_element_t>::template rebind<typename stride1_sequence_t::list_node_end_t> child;
                };
            };
            using navigator_t = detail::augmented_deque::circular_doubly_linked_list_node_navigator_t<typename sequence_config_t::allocator_element_t>;
            using list_node_end_t = detail::augmented_deque::list_node_end_t<sequence_config_t>;
            using list_node_t = detail::augmented_deque::list_node_t<sequence_config_t>;
            using tree_node_t = detail::augmented_deque::tree_node_t<sequence_config_t>;
            using digit_node_end_t = detail::augmented_deque::digit_node_end_t<sequence_config_t>;
            using digit_node_t = detail::augmented_deque::digit_node_t<sequence_config_t>;

            using pointer_navigator_t = typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<navigator_t>;
            using pointer_list_node_t = typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<list_node_t>;
            using pointer_list_node_end_t = typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<list_node_end_t>;
            using pointer_tree_node_t = typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<tree_node_t>;
            using pointer_digit_node_t = typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<digit_node_t>;
            using pointer_digit_node_end_t = typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<digit_node_end_t>;

            using iterator_projected_storage_t = detail::augmented_deque::iterator_projected_storage_t<false, sequence_config_t>;
            static_assert(std::input_or_output_iterator<iterator_projected_storage_t>);
            static_assert(std::input_iterator<iterator_projected_storage_t>);
            static_assert(std::sentinel_for<iterator_projected_storage_t, iterator_projected_storage_t>);
            static_assert(std::sentinel_for<std::default_sentinel_t, iterator_projected_storage_t>);
            static_assert(std::forward_iterator<iterator_projected_storage_t>);
            static_assert(std::bidirectional_iterator<iterator_projected_storage_t>);
            static_assert(std::sized_sentinel_for<iterator_projected_storage_t, iterator_projected_storage_t>);
            static_assert(std::sized_sentinel_for<std::default_sentinel_t, iterator_projected_storage_t>);
            //            static_assert(std::random_access_iterator<iterator_projected_storage_t>);
            using const_iterator_projected_storage_t = detail::augmented_deque::iterator_projected_storage_t<true, sequence_config_t>;
            static_assert(std::input_or_output_iterator<const_iterator_projected_storage_t>);
            static_assert(std::input_iterator<const_iterator_projected_storage_t>);
            static_assert(std::sentinel_for<const_iterator_projected_storage_t, const_iterator_projected_storage_t>);
            static_assert(std::sentinel_for<std::default_sentinel_t, const_iterator_projected_storage_t>);
            static_assert(std::forward_iterator<const_iterator_projected_storage_t>);
            static_assert(std::bidirectional_iterator<const_iterator_projected_storage_t>);
            static_assert(std::sized_sentinel_for<const_iterator_projected_storage_t, const_iterator_projected_storage_t>);
            static_assert(std::sized_sentinel_for<std::default_sentinel_t, const_iterator_projected_storage_t>);
            //            static_assert(std::random_access_iterator<const_iterator_projected_storage_t>);
            iterator_projected_storage_t begin_projected_storage() { return {navigator_t::untagged_next_or_tagged_end(list_node_end)}; }
            iterator_projected_storage_t end_projected_storage() { return {list_node_end}; }
            const_iterator_projected_storage_t begin_projected_storage() const { return {navigator_t::untagged_next_or_tagged_end(list_node_end)}; }
            const_iterator_projected_storage_t end_projected_storage() const { return {list_node_end}; }
            const_iterator_projected_storage_t cbegin_projected_storage() const { return {navigator_t::untagged_next_or_tagged_end(list_node_end)}; }
            const_iterator_projected_storage_t cend_projected_storage() const { return {list_node_end}; }

            pointer_list_node_end_t list_node_end;
            std::size_t const &list_node_count() const { return detail::language::tagged_ptr_bit0_unsetted(list_node_end)->node_count; }
            std::size_t const &list_front_element_count() const { return detail::language::tagged_ptr_bit0_unsetted(list_node_end)->front_element_count; }
            std::size_t const &list_back_element_count() const { return detail::language::tagged_ptr_bit0_unsetted(list_node_end)->back_element_count; }
            std::size_t &list_node_count() { return detail::language::tagged_ptr_bit0_unsetted(list_node_end)->node_count; }
            std::size_t &list_front_element_count() { return detail::language::tagged_ptr_bit0_unsetted(list_node_end)->front_element_count; }
            std::size_t &list_back_element_count() { return detail::language::tagged_ptr_bit0_unsetted(list_node_end)->back_element_count; }

            pointer_digit_node_end_t digit_node_end;
            projector_and_accumulator_t const &projector_and_accumulator() const { return detail::language::tagged_ptr_bit0_unsetted(digit_node_end)->projector_and_accumulator; }
            pointer_navigator_t digit_front() const { return detail::language::tagged_ptr_bit0_unsetted(detail::language::tagged_ptr_bit0_unsetted(this->digit_node_end)->next); }
            pointer_navigator_t digit_middle() const { return detail::language::tagged_ptr_bit0_unsetted(detail::language::tagged_ptr_bit0_unsetted(this->digit_node_end)->middle); }
            pointer_navigator_t digit_back() const { return detail::language::tagged_ptr_bit0_unsetted(detail::language::tagged_ptr_bit0_unsetted(this->digit_node_end)->prev); }
            struct digit_proxy_t
            {
                augmented_deque_sequence_t *this_;
                pointer_navigator_t(navigator_t::*next_or_prev);
                operator pointer_navigator_t() { return detail::language::tagged_ptr_bit0_unsetted(detail::language::tagged_ptr_bit0_unsetted(this_->digit_node_end)->*next_or_prev); }
                digit_proxy_t &operator=(pointer_navigator_t digit_front_new)
                {
                    detail::language::tagged_ptr_bit0_unsetted(this_->digit_node_end)->*next_or_prev = detail::language::tagged_ptr_bit0_setted(digit_front_new);
                    return *this;
                }
            };
            digit_proxy_t digit_front()
            {
                return {
                    .this_ = this,
                    .next_or_prev = &navigator_t::next,
                };
            }
            auto digit_middle()
            {
                struct proxy_t
                {
                    augmented_deque_sequence_t *this_;
                    operator pointer_navigator_t()
                    {
                        return detail::language::tagged_ptr_bit0_unsetted(detail::language::tagged_ptr_bit0_unsetted(this_->digit_node_end)->middle);
                    }
                    proxy_t &operator=(pointer_navigator_t digit_middle_new)
                    {
                        detail::language::tagged_ptr_bit0_unsetted(this_->digit_node_end)->middle = detail::language::tagged_ptr_bit0_setted(digit_middle_new);
                        return *this;
                    }
                } proxy{.this_ = this};
                return proxy;
            }
            digit_proxy_t digit_back()
            {
                return {
                    .this_ = this,
                    .next_or_prev = &navigator_t::prev,
                };
            }

            void push_back(allocator_element_t const &allocator_element, typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<typename stride1_sequence_t::sequence_config_t::list_node_t> p_stride1_sequence_list_node)
            {
                auto sequence_functor_forward = detail::augmented_deque::make_sequence_functor<false>(allocator_element, this);
                auto push_back_and_get_p_list_node = [&](pointer_tree_node_t parent) //
                {
                    ++list_node_count();
                    pointer_list_node_t list_back = detail::memory::new_expression<list_node_t>(allocator_element, parent, p_stride1_sequence_list_node);
                    navigator_t::push_impl(list_node_end, &navigator_t::prev, &navigator_t::next, list_back);
                    std::get<sequence_index - 1>(p_stride1_sequence_list_node->actual_projected_storage.parents) = list_back;
                    return list_back;
                };
                if (list_node_count() == 0)
                {
                    assert(list_front_element_count() == 0);
                    assert(list_back_element_count() == 0);
                    ++list_front_element_count();
                    ++list_back_element_count();
                    sequence_functor_forward.push_impl(push_back_and_get_p_list_node);
                }
                else
                {
                    if (list_back_element_count() != this->stride)
                    {
                        if (list_node_count() == 1)
                        {
                            assert(list_front_element_count() == list_back_element_count());
                            ++list_front_element_count();
                        }
                        ++list_back_element_count();
                        std::get<sequence_index - 1>(p_stride1_sequence_list_node->actual_projected_storage.parents) = list_node_t::untagged_prev(list_node_end);

                        sequence_functor_forward.update_range_impl(list_node_t::untagged_prev(list_node_end), list_node_t::untagged_prev(list_node_end));
                    }
                    else
                    {
                        list_back_element_count() = 1;
                        sequence_functor_forward.push_impl(push_back_and_get_p_list_node);
                    }
                }
            }
            void push_front(allocator_element_t const &allocator_element, typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<typename stride1_sequence_t::sequence_config_t::list_node_t> p_stride1_sequence_list_node)
            {
                auto sequence_functor_backward = detail::augmented_deque::make_sequence_functor<true>(allocator_element, this);
                auto push_back_and_get_p_list_node = [&](pointer_tree_node_t parent) //
                {
                    ++list_node_count();
                    pointer_list_node_t list_front = detail::memory::new_expression<list_node_t>(allocator_element, parent, p_stride1_sequence_list_node);
                    navigator_t::push_impl(list_node_end, &navigator_t::next, &navigator_t::prev, list_front);
                    std::get<sequence_index - 1>(p_stride1_sequence_list_node->actual_projected_storage.parents) = list_front;
                    return list_front;
                };
                if (list_node_count() == 0)
                {
                    assert(list_front_element_count() == 0);
                    assert(list_back_element_count() == 0);
                    ++list_front_element_count();
                    ++list_back_element_count();
                    sequence_functor_backward.push_impl(push_back_and_get_p_list_node);
                }
                else
                {
                    if (list_front_element_count() != this->stride)
                    {
                        if (list_node_count() == 1)
                        {
                            assert(list_front_element_count() == list_back_element_count());
                            ++list_back_element_count();
                        }
                        ++list_front_element_count();
                        list_node_t::untagged_next(list_node_end)->actual_projected_storage.child = p_stride1_sequence_list_node;
                        std::get<sequence_index - 1>(p_stride1_sequence_list_node->actual_projected_storage.parents) = list_node_t::untagged_next(list_node_end);

                        auto sequence_functor_forward = detail::augmented_deque::make_sequence_functor<false>(allocator_element, this);
                        sequence_functor_forward.update_range_impl(list_node_t::untagged_next(list_node_end), list_node_t::untagged_next(list_node_end));
                    }
                    else
                    {
                        list_front_element_count() = 1;
                        sequence_functor_backward.push_impl(push_back_and_get_p_list_node);
                    }
                }
            }
            void pop_back(allocator_element_t const &allocator_element, [[maybe_unused]] typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<typename stride1_sequence_t::sequence_config_t::list_node_end_t> p_stride1_sequence_list_node_end)
            {
                auto sequence_functor_forward = detail::augmented_deque::make_sequence_functor<false>(allocator_element, this);
                auto pop_list_node = [&]() //
                {
                    --list_node_count();
                    pointer_list_node_t list_back = list_node_t::untagged_prev(list_node_end);
                    list_node_t::extract_impl(list_back, &navigator_t::prev, &navigator_t::next);
                    detail::memory::delete_expression<list_node_t>(allocator_element, list_back);
                };
                assert(list_node_count() != 0);
                if (list_back_element_count() != 1)
                {
                    if (list_node_count() == 1)
                    {
                        assert(list_front_element_count() == list_back_element_count());
                        --list_front_element_count();
                    }
                    --list_back_element_count();

                    sequence_functor_forward.update_range_impl(list_node_t::untagged_prev(list_node_end), list_node_t::untagged_prev(list_node_end));
                }
                else
                {
                    if (list_node_count() == 1)
                    {
                        assert(list_front_element_count() == list_back_element_count());
                        --list_front_element_count();
                        --list_back_element_count();
                    }
                    else if (list_node_count() == 2)
                        list_back_element_count() = list_front_element_count();
                    else
                        list_back_element_count() = this->stride;
                    sequence_functor_forward.pop_impl(pop_list_node);
                }
            }
            void pop_front(allocator_element_t const &allocator_element, typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<typename stride1_sequence_t::sequence_config_t::list_node_end_t> p_stride1_sequence_list_node_end)
            {
                auto sequence_functor_backward = detail::augmented_deque::make_sequence_functor<true>(allocator_element, this);
                auto pop_list_node = [&]() //
                {
                    --list_node_count();
                    pointer_list_node_t list_front = list_node_t::untagged_next(list_node_end);
                    list_node_t::extract_impl(list_front, &navigator_t::next, &navigator_t::prev);
                    detail::memory::delete_expression<list_node_t>(allocator_element, list_front);
                };
                assert(list_node_count() != 0);
                if (list_front_element_count() != 1)
                {
                    if (list_node_count() == 1)
                    {
                        assert(list_front_element_count() == list_back_element_count());
                        --list_back_element_count();
                    }
                    --list_front_element_count();
                    list_node_t::untagged_next(list_node_end)->actual_projected_storage.child = stride1_sequence_t::list_node_t::untagged_next(p_stride1_sequence_list_node_end);

                    auto sequence_functor_forward = detail::augmented_deque::make_sequence_functor<false>(allocator_element, this);
                    sequence_functor_forward.update_range_impl(list_node_t::untagged_next(list_node_end), list_node_t::untagged_next(list_node_end));
                }
                else
                {
                    if (list_node_count() == 1)
                    {
                        assert(list_front_element_count() == list_back_element_count());
                        --list_front_element_count();
                        --list_back_element_count();
                    }
                    else if (list_node_count() == 2)
                        list_front_element_count() = list_back_element_count();
                    else
                        list_front_element_count() = this->stride;
                    sequence_functor_backward.pop_impl(pop_list_node);
                }
            }
        };
        template<typename projector_and_accumulator_t_>
        struct augmented_deque_sequence_t<0, 1, projector_and_accumulator_t_> : detail::augmented_deque::add_stride_member<1>
        {
            static constexpr std::size_t sequence_index = 0;
            static constexpr std::size_t requested_stride = 1;
            using projector_and_accumulator_t = projector_and_accumulator_t_;

            using projected_storage_t = typename projector_and_accumulator_t::projected_storage_t;
            using projected_storage_or_element_t = std::conditional_t<!std::is_same_v<projected_storage_t, void>, projected_storage_t, element_t>;
            using pointer_projected_storage_or_pointer_element_t = typename std::pointer_traits<pointer_element_t>::template rebind<projected_storage_or_element_t>;
            using accumulated_storage_t = typename projector_and_accumulator_t::accumulated_storage_t;
            struct sequence_config_t
            {
                using element_t = augmented_deque_t::element_t;
                using allocator_element_t = augmented_deque_t::allocator_element_t;
                using pointer_element_t = augmented_deque_t::pointer_element_t;

                static constexpr std::size_t sequence_index = augmented_deque_sequence_t::sequence_index;
                static constexpr std::size_t requested_stride = augmented_deque_sequence_t::requested_stride;
                using projector_and_accumulator_t = augmented_deque_sequence_t::projector_and_accumulator_t;

                using list_node_end_t = detail::augmented_deque::list_node_end_t<sequence_config_t>;
                using list_node_t = detail::augmented_deque::list_node_t<sequence_config_t>;

                template<std::size_t index, typename stride_to_projector_and_accumulator_item_t>
                struct stride_to_projector_and_accumulator_item_to_pointer_to_sequence_list_node_t : std::type_identity<typename std::pointer_traits<pointer_element_t>::template rebind<typename augmented_deque_sequence_t<index + 1, stride_to_projector_and_accumulator_item_t::first_type::value, typename stride_to_projector_and_accumulator_item_t::second_type>::list_node_t>>
                {};
                using pointers_to_other_strides_sequence_list_node_t = detail::utility::map_transform_t<other_strides_stride_to_projector_and_accumulator_map_t, stride_to_projector_and_accumulator_item_to_pointer_to_sequence_list_node_t>;

                template<std::size_t index, typename stride_to_projector_and_accumulator_item_t>
                struct stride_to_projector_and_accumulator_item_to_pointer_to_sequence_list_node_end_t : std::type_identity<typename std::pointer_traits<pointer_element_t>::template rebind<typename augmented_deque_sequence_t<index + 1, stride_to_projector_and_accumulator_item_t::first_type::value, typename stride_to_projector_and_accumulator_item_t::second_type>::list_node_end_t>>
                {};
                using pointers_to_other_strides_sequence_list_node_end_t = detail::utility::map_transform_t<other_strides_stride_to_projector_and_accumulator_map_t, stride_to_projector_and_accumulator_item_to_pointer_to_sequence_list_node_end_t>;

                using projected_storage_t = augmented_deque_sequence_t::projected_storage_t;
                using projected_storage_or_element_t = augmented_deque_sequence_t::projected_storage_or_element_t;
                using pointer_projected_storage_or_pointer_element_t = augmented_deque_sequence_t::pointer_projected_storage_or_pointer_element_t;
                using accumulated_storage_t = augmented_deque_sequence_t::accumulated_storage_t;
                struct actual_projected_storage_t : detail::augmented_deque::add_projected_storage_member_t<pointer_element_t, projected_storage_t>
                {
                    pointers_to_other_strides_sequence_list_node_t parents;

                    alignas(element_t) std::byte element_buffer[sizeof(element_t)]; // element_t element;
                    pointer_element_t p_element() { return std::pointer_traits<pointer_element_t>::pointer_to(*reinterpret_cast<element_t *>(&element_buffer)); }
                    pointer_projected_storage_or_pointer_element_t p_projected_storage_or_p_element()
                    {
                        if constexpr (!std::is_same_v<projected_storage_t, void>) return this->p_projected_storage();
                        else return p_element();
                    }
                };
                struct actual_projected_storage_end_t
                {
                    pointers_to_other_strides_sequence_list_node_end_t parents;
                };
            };

            using navigator_t = detail::augmented_deque::circular_doubly_linked_list_node_navigator_t<typename sequence_config_t::allocator_element_t>;
            using list_node_end_t = detail::augmented_deque::list_node_end_t<sequence_config_t>;
            using list_node_t = detail::augmented_deque::list_node_t<sequence_config_t>;
            using tree_node_t = detail::augmented_deque::tree_node_t<sequence_config_t>;
            using digit_node_end_t = detail::augmented_deque::digit_node_end_t<sequence_config_t>;
            using digit_node_t = detail::augmented_deque::digit_node_t<sequence_config_t>;

            using pointer_navigator_t = typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<navigator_t>;
            using pointer_list_node_t = typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<list_node_t>;
            using pointer_list_node_end_t = typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<list_node_end_t>;
            using pointer_tree_node_t = typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<tree_node_t>;
            using pointer_digit_node_t = typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<digit_node_t>;
            using pointer_digit_node_end_t = typename std::pointer_traits<typename sequence_config_t::pointer_element_t>::template rebind<digit_node_end_t>;

            using iterator_element_t = detail::augmented_deque::iterator_element_t<false, sequence_config_t>;
            static_assert(std::input_or_output_iterator<iterator_element_t>);
            static_assert(std::input_iterator<iterator_element_t>);
            static_assert(std::sentinel_for<iterator_element_t, iterator_element_t>);
            static_assert(std::sentinel_for<std::default_sentinel_t, iterator_element_t>);
            static_assert(std::forward_iterator<iterator_element_t>);
            static_assert(std::bidirectional_iterator<iterator_element_t>);
            static_assert(std::sized_sentinel_for<iterator_element_t, iterator_element_t>);
            static_assert(std::sized_sentinel_for<std::default_sentinel_t, iterator_element_t>);
            //            static_assert(std::random_access_iterator<iterator_element_t>);
            using const_iterator_element_t = detail::augmented_deque::iterator_element_t<true, sequence_config_t>;
            static_assert(std::input_or_output_iterator<const_iterator_element_t>);
            static_assert(std::input_iterator<const_iterator_element_t>);
            static_assert(std::sentinel_for<const_iterator_element_t, const_iterator_element_t>);
            static_assert(std::sentinel_for<std::default_sentinel_t, const_iterator_element_t>);
            static_assert(std::forward_iterator<const_iterator_element_t>);
            static_assert(std::bidirectional_iterator<const_iterator_element_t>);
            static_assert(std::sized_sentinel_for<const_iterator_element_t, const_iterator_element_t>);
            static_assert(std::sized_sentinel_for<std::default_sentinel_t, const_iterator_element_t>);
            //            static_assert(std::random_access_iterator<const_iterator_element_t>);
            iterator_element_t begin_element() { return {navigator_t::untagged_next_or_tagged_end(list_node_end)}; }
            iterator_element_t end_element() { return {list_node_end}; }
            const_iterator_element_t begin_element() const { return {navigator_t::untagged_next_or_tagged_end(list_node_end)}; }
            const_iterator_element_t end_element() const { return {list_node_end}; }
            const_iterator_element_t cbegin_element() const { return {navigator_t::untagged_next_or_tagged_end(list_node_end)}; }
            const_iterator_element_t cend_element() const { return {list_node_end}; }

            using iterator_projected_storage_t = detail::augmented_deque::iterator_projected_storage_t<false, sequence_config_t>;
            static_assert(std::is_same_v<projected_storage_t, void> || std::input_or_output_iterator<iterator_projected_storage_t>);
            static_assert(std::is_same_v<projected_storage_t, void> || std::input_iterator<iterator_projected_storage_t>);
            static_assert(std::is_same_v<projected_storage_t, void> || std::sentinel_for<iterator_projected_storage_t, iterator_projected_storage_t>);
            static_assert(std::is_same_v<projected_storage_t, void> || std::sentinel_for<std::default_sentinel_t, iterator_projected_storage_t>);
            static_assert(std::is_same_v<projected_storage_t, void> || std::forward_iterator<iterator_projected_storage_t>);
            static_assert(std::is_same_v<projected_storage_t, void> || std::bidirectional_iterator<iterator_projected_storage_t>);
            static_assert(std::is_same_v<projected_storage_t, void> || std::sized_sentinel_for<iterator_projected_storage_t, iterator_projected_storage_t>);
            static_assert(std::is_same_v<projected_storage_t, void> || std::sized_sentinel_for<std::default_sentinel_t, iterator_projected_storage_t>);
            //            static_assert(std::is_same_v<projected_storage_t, void> || std::random_access_iterator<iterator_projected_storage_t>);
            using const_iterator_projected_storage_t = detail::augmented_deque::iterator_projected_storage_t<true, sequence_config_t>;
            static_assert(std::is_same_v<projected_storage_t, void> || std::input_or_output_iterator<const_iterator_projected_storage_t>);
            static_assert(std::is_same_v<projected_storage_t, void> || std::input_iterator<const_iterator_projected_storage_t>);
            static_assert(std::is_same_v<projected_storage_t, void> || std::sentinel_for<const_iterator_projected_storage_t, const_iterator_projected_storage_t>);
            static_assert(std::is_same_v<projected_storage_t, void> || std::sentinel_for<std::default_sentinel_t, const_iterator_projected_storage_t>);
            static_assert(std::is_same_v<projected_storage_t, void> || std::forward_iterator<const_iterator_projected_storage_t>);
            static_assert(std::is_same_v<projected_storage_t, void> || std::bidirectional_iterator<const_iterator_projected_storage_t>);
            static_assert(std::is_same_v<projected_storage_t, void> || std::sized_sentinel_for<const_iterator_projected_storage_t, const_iterator_projected_storage_t>);
            static_assert(std::is_same_v<projected_storage_t, void> || std::sized_sentinel_for<std::default_sentinel_t, const_iterator_projected_storage_t>);
            //            static_assert(std::is_same_v<projected_storage_t, void> || std::random_access_iterator<const_iterator_projected_storage_t>);
            iterator_projected_storage_t begin_projected_storage() { return {navigator_t::untagged_next_or_tagged_end(list_node_end)}; }
            iterator_projected_storage_t end_projected_storage() { return {list_node_end}; }
            const_iterator_projected_storage_t begin_projected_storage() const { return {navigator_t::untagged_next_or_tagged_end(list_node_end)}; }
            const_iterator_projected_storage_t end_projected_storage() const { return {list_node_end}; }
            const_iterator_projected_storage_t cbegin_projected_storage() const { return {navigator_t::untagged_next_or_tagged_end(list_node_end)}; }
            const_iterator_projected_storage_t cend_projected_storage() const { return {list_node_end}; }


            pointer_list_node_end_t list_node_end;
            std::size_t const &list_node_count() const { return detail::language::tagged_ptr_bit0_unsetted(list_node_end)->node_count; }
            std::size_t &list_node_count() { return detail::language::tagged_ptr_bit0_unsetted(list_node_end)->node_count; }

            pointer_digit_node_end_t digit_node_end;
            projector_and_accumulator_t const &projector_and_accumulator() const { return detail::language::tagged_ptr_bit0_unsetted(digit_node_end)->projector_and_accumulator; }
            pointer_navigator_t digit_front() const { return detail::language::tagged_ptr_bit0_unsetted(detail::language::tagged_ptr_bit0_unsetted(this->digit_node_end)->next); }
            pointer_navigator_t digit_middle() const { return detail::language::tagged_ptr_bit0_unsetted(detail::language::tagged_ptr_bit0_unsetted(this->digit_node_end)->middle); }
            pointer_navigator_t digit_back() const { return detail::language::tagged_ptr_bit0_unsetted(detail::language::tagged_ptr_bit0_unsetted(this->digit_node_end)->prev); }
            struct digit_proxy_t
            {
                augmented_deque_sequence_t *this_;
                pointer_navigator_t(navigator_t::*next_or_prev);
                operator pointer_navigator_t() { return detail::language::tagged_ptr_bit0_unsetted(detail::language::tagged_ptr_bit0_unsetted(this_->digit_node_end)->*next_or_prev); }
                digit_proxy_t &operator=(pointer_navigator_t digit_front_new)
                {
                    detail::language::tagged_ptr_bit0_unsetted(this_->digit_node_end)->*next_or_prev = detail::language::tagged_ptr_bit0_setted(digit_front_new);
                    return *this;
                }
            };
            digit_proxy_t digit_front()
            {
                return {
                    .this_ = this,
                    .next_or_prev = &navigator_t::next,
                };
            }
            auto digit_middle()
            {
                struct proxy_t
                {
                    augmented_deque_sequence_t *this_;
                    operator pointer_navigator_t() { return detail::language::tagged_ptr_bit0_unsetted(detail::language::tagged_ptr_bit0_unsetted(this_->digit_node_end)->middle); }
                    proxy_t &operator=(pointer_navigator_t digit_middle_new)
                    {
                        detail::language::tagged_ptr_bit0_unsetted(this_->digit_node_end)->middle = detail::language::tagged_ptr_bit0_setted(digit_middle_new);
                        return *this;
                    }
                } proxy{.this_ = this};
                return proxy;
            }
            digit_proxy_t digit_back()
            {
                return {
                    .this_ = this,
                    .next_or_prev = &navigator_t::prev,
                };
            }

            template<typename... args_t>
            void emplace_back(allocator_element_t const &allocator_element, args_t &&...args)
            {
                auto sequence_functor_forward = detail::augmented_deque::make_sequence_functor<false>(allocator_element, this);
                sequence_functor_forward.push_impl([&](pointer_tree_node_t parent) //
                                                   {
                                                       ++list_node_count();
                                                       pointer_list_node_t list_back = detail::memory::new_expression<list_node_t>(allocator_element, parent);
                                                       navigator_t::push_impl(list_node_end, &navigator_t::prev, &navigator_t::next, list_back);
                                                       std::allocator_traits<allocator_element_t>::construct(detail::utility::unmove(allocator_element_t(allocator_element)), std::to_address(list_back->actual_projected_storage.p_element()), std::forward<args_t>(args)...);
                                                       return list_back; });
            }
            template<typename... args_t>
            void emplace_front(allocator_element_t const &allocator_element, args_t &&...args)
            {
                auto sequence_functor_backward = detail::augmented_deque::make_sequence_functor<true>(allocator_element, this);
                sequence_functor_backward.push_impl([&](pointer_tree_node_t parent) //
                                                    {
                                                        ++list_node_count();
                                                        pointer_list_node_t list_front = detail::memory::new_expression<list_node_t>(allocator_element, parent);
                                                        navigator_t::push_impl(list_node_end, &navigator_t::next, &navigator_t::prev, list_front);
                                                        std::allocator_traits<allocator_element_t>::construct(detail::utility::unmove(allocator_element_t(allocator_element)), std::to_address(list_front->actual_projected_storage.p_element()), std::forward<args_t>(args)...);
                                                        return list_front; });
            }
            void pop_back(allocator_element_t const &allocator_element)
            {
                auto sequence_functor_forward = detail::augmented_deque::make_sequence_functor<false>(allocator_element, this);
                sequence_functor_forward.pop_impl([&]() //
                                                  {
                                                      --list_node_count();
                                                      pointer_list_node_t list_back = list_node_t::untagged_prev(list_node_end);
                                                      list_node_t::extract_impl(list_back, &navigator_t::prev, &navigator_t::next);
                                                      detail::memory::delete_expression<list_node_t>(allocator_element, list_back); });
            }
            void pop_front(allocator_element_t const &allocator_element)
            {
                auto sequence_functor_backward = detail::augmented_deque::make_sequence_functor<true>(allocator_element, this);
                sequence_functor_backward.pop_impl([&]() //
                                                   {
                                                       --list_node_count();
                                                       pointer_list_node_t list_front = list_node_t::untagged_next(list_node_end);
                                                       list_node_t::extract_impl(list_front, &navigator_t::next, &navigator_t::prev);
                                                       detail::memory::delete_expression<list_node_t>(allocator_element, list_front); });
            }
        };

        allocator_element_t allocator_element;
        allocator_type get_allocator() const noexcept { return allocator_element; }
        stride1_sequence_t stride1_sequence;
        other_stride_sequences_t other_stride_sequences;

        static constexpr std::size_t sequences_count = std::tuple_size_v<stride_to_projector_and_accumulator_map_t>;
        using sequences_t = detail::utility::tuple_cat_t<std::tuple<stride1_sequence_t>, other_stride_sequences_t>;
        template<std::size_t I>
        using sequence_t = std::tuple_element_t<I, sequences_t>;
        template<std::size_t I>
        sequence_t<I> &sequence() &
        {
            if constexpr (I == 0) return stride1_sequence;
            else return std::get<I - 1>(other_stride_sequences);
        }
        template<std::size_t I>
        sequence_t<I> const &sequence() const &
        {
            if constexpr (I == 0) return stride1_sequence;
            else return std::get<I - 1>(other_stride_sequences);
        }
        template<std::size_t I>
        sequence_t<I> &&sequence() &&
        {
            if constexpr (I == 0) return std::move(stride1_sequence);
            else return std::move(std::get<I - 1>(other_stride_sequences));
        }
        template<std::size_t I>
        sequence_t<I> const &&sequence() const &&
        {
            if constexpr (I == 0) return std::move(stride1_sequence);
            else return std::move(std::get<I - 1>(other_stride_sequences));
        }

        void create_end_nodes()
        {
            [&]<std::size_t... I>(std::index_sequence<I...>) {
                (..., ([this] {
                        sequence<I>().list_node_end = sequence_t<I>::list_node_end_t::create_tagged_end(allocator_element);
                        sequence<I>().digit_node_end = sequence_t<I>::digit_node_end_t::create_tagged_end(allocator_element);
                        detail::language::tagged_ptr_bit0_unsetted(sequence<I>().digit_node_end)->middle = sequence<I>().digit_node_end;
                        detail::language::tagged_ptr_bit0_unsetted(sequence<I>().digit_node_end)->list_node_end = sequence<I>().list_node_end;
                        detail::language::tagged_ptr_bit0_unsetted(sequence<I>().list_node_end)->digit_node_end = sequence<I>().digit_node_end;
                        if constexpr(!std::is_same_v<typename sequence_t<I>::accumulated_storage_t, void>)
                            sequence<I>().projector_and_accumulator().construct_accumulated_storage(allocator_element, detail::language::tagged_ptr_bit0_unsetted(sequence<I>().digit_node_end)->p_accumulated_storage(), std::make_tuple()); }()));
            }(std::make_index_sequence<sequences_count>());

            [&]<std::size_t... I>(std::index_sequence<I...>) {
                (..., (detail::language::tagged_ptr_bit0_unsetted(std::get<I>(other_stride_sequences).list_node_end)->actual_projected_storage_end.child = stride1_sequence.list_node_end, std::get<I>(detail::language::tagged_ptr_bit0_unsetted(stride1_sequence.list_node_end)->actual_projected_storage_end.parents) = std::get<I>(other_stride_sequences).list_node_end));
            }(std::make_index_sequence<std::tuple_size_v<other_strides_stride_to_projector_and_accumulator_map_t>>());
        }
        void destroy_end_nodes()
        {
            [&]<std::size_t... I>(std::index_sequence<I...>) {
                (..., ([this] {
                              if constexpr(!std::is_same_v<typename sequence_t<I>::accumulated_storage_t, void>)
                                  sequence<I>().projector_and_accumulator().destroy_accumulated_storage(allocator_element, detail::language::tagged_ptr_bit0_unsetted(sequence<I>().digit_node_end)->p_accumulated_storage());
                              detail::memory::delete_expression<typename sequence_t<I>::digit_node_end_t>(allocator_element, detail::language::tagged_ptr_bit0_unsetted(sequence<I>().digit_node_end));
                              detail::memory::delete_expression<typename sequence_t<I>::list_node_end_t>(allocator_element, detail::language::tagged_ptr_bit0_unsetted(sequence<I>().list_node_end)); }()));
            }(std::make_index_sequence<sequences_count>());
        }
        void swap_end_nodes(augmented_deque_t &other)
        {
            [&]<std::size_t... I>(std::index_sequence<I...>) {
                (..., (std::ranges::swap(std::tie(sequence<I>().list_node_end, sequence<I>().digit_node_end), std::tie(other.sequence<I>().list_node_end, other.sequence<I>().digit_node_end))));
            }(std::make_index_sequence<sequences_count>());
        }

        augmented_deque_t() /* allocator is default initialized */ { create_end_nodes(); } // default constructor
        void clear() &
        {
            while (!empty())
                this->pop_back();
        }
        explicit augmented_deque_t(allocator_element_t const &allocator_element) : allocator_element(allocator_element) // default constructor with allocator
        {
            create_end_nodes();
        }
        explicit augmented_deque_t(size_type count, allocator_element_t const &allocator_element = allocator_element_t()) : augmented_deque_t(allocator_element) // count default-inserted constructor (with allocator)?
        {
            std::ranges::for_each(std::views::iota(static_cast<size_type>(0), count), [this]([[maybe_unused]] size_type index) { this->emplace_back(); });
        }
        explicit augmented_deque_t(size_type count, element_t const &value, allocator_element_t const &allocator_element = allocator_element_t()) : augmented_deque_t(allocator_element) // count copy-inserted constructor (with allocator)?
        {
            std::ranges::for_each(std::views::iota(static_cast<size_type>(0), count), [this, &value]([[maybe_unused]] size_type index) { this->emplace_back(value); });
        }
        void assign(size_type count, element_t const &value) &
        {
            this->clear();
            std::ranges::for_each(std::views::iota(static_cast<size_type>(0), count), [this, &value]([[maybe_unused]] size_type index) { this->emplace_back(value); });
        }
        template<std::input_iterator iterator_t, std::sentinel_for<iterator_t> sentinel_t>
        augmented_deque_t(iterator_t iterator, sentinel_t sentinel, allocator_element_t const &allocator_element = allocator_element_t()) : augmented_deque_t(allocator_element) // comparable range constructor (with allocator)?
        {
            std::ranges::for_each(std::ranges::subrange(iterator, sentinel), [this]<typename other_element_t>(other_element_t &&other_element) { this->emplace_back(std::forward<other_element_t>(other_element)); });
        }
        template<std::input_iterator iterator_t, std::sentinel_for<iterator_t> sentinel_t>
        void assign(iterator_t iterator, sentinel_t sentinel) &
        {
            this->clear();
            std::ranges::for_each(std::ranges::subrange(iterator, sentinel), [this]<typename other_element_t>(other_element_t &&other_element) { this->emplace_back(std::forward<other_element_t>(other_element)); });
        }
        augmented_deque_t(std::initializer_list<element_t> initializer_list, allocator_element_t const &allocator_element = allocator_element_t()) : augmented_deque_t(allocator_element) // std::initializer_list constructor (with allocator)?
        {
            std::ranges::for_each(initializer_list, [this](element_t const &other_element) { this->emplace_back(other_element); });
        }
        augmented_deque_t &operator=(std::initializer_list<element_t> initializer_list) & // std::initializer_list assignment operator
        {
            this->clear();
            std::ranges::for_each(initializer_list, [this](element_t const &other_element) { this->emplace_back(other_element); });
            return *this;
        }
        void assign(std::initializer_list<element_t> initializer_list) &
        {
            this->clear();
            std::ranges::for_each(initializer_list, [this](element_t const &other_element) { this->emplace_back(other_element); });
        }
        augmented_deque_t(augmented_deque_t const &other, std::type_identity_t<allocator_element_t> const &allocator_element) : augmented_deque_t(allocator_element) // copy constructor with allocator
        {
            std::ranges::for_each(std::ranges::subrange(other.sequence<0>().cbegin_element(), other.sequence<0>().cend_element()), [this](element_t const &other_element) { this->emplace_back(other_element); });
        }
        augmented_deque_t(augmented_deque_t const &other) : augmented_deque_t(other, std::allocator_traits<allocator_type>::select_on_container_copy_construction(other.allocator_element)) {} // copy constructor
        augmented_deque_t &operator=(augmented_deque_t const &other) & // copy assignment operator
        {
            if (this == &other)
                return *this;
            this->clear();
            if (this->allocator_element != other.allocator_element)
            {
                if constexpr (std::allocator_traits<allocator_type>::propagate_on_container_copy_assignment::value)
                {
                    destroy_end_nodes();
                    this->allocator_element = other.allocator_element;
                    create_end_nodes();
                }
            }
            std::ranges::for_each(std::ranges::subrange(other.sequence<0>().cbegin_element(), other.sequence<0>().cend_element()), [this](element_t const &other_element) { this->emplace_back(other_element); });
            return *this;
        }

        augmented_deque_t(augmented_deque_t &&other) // move constructor
            : allocator_element(([&]() //
                                 {
                                     stride1_sequence.list_node_end = other.list_node_end, stride1_sequence.digit_node_end = other.digit_node_end;
                                     [&]<std::size_t... I>(std::index_sequence<I...>)
                                     {
                                         (..., (std::get<I>(other_stride_sequences).list_node_end = std::get<I>(other.other_stride_sequences).list_node_end, std::get<I>(other_stride_sequences).digit_node_end = std::get<I>(other.other_stride_sequences).digit_node_end));
                                     }(std::make_index_sequence<std::tuple_size_v<other_strides_stride_to_projector_and_accumulator_map_t>>());
                                     other.create_end_nodes(); }(),
                                 std::move(other.allocator_element)))
        {}
        augmented_deque_t(augmented_deque_t &&other, std::type_identity_t<allocator_element_t> const &allocator_element) : allocator_element(allocator_element) // move constructor with allocator
        {
            if (allocator_element == other.allocator_element)
            {
                stride1_sequence.list_node_end = other.list_node_end, stride1_sequence.digit_node_end = other.digit_node_end;
                [&]<std::size_t... I>(std::index_sequence<I...>) {
                    (..., (std::get<I>(other_stride_sequences).list_node_end = std::get<I>(other.other_stride_sequences).list_node_end, std::get<I>(other_stride_sequences).digit_node_end = std::get<I>(other.other_stride_sequences).digit_node_end));
                }(std::make_index_sequence<std::tuple_size_v<other_strides_stride_to_projector_and_accumulator_map_t>>());
                other.create_end_nodes();
            }
            else
            {
                create_end_nodes();
                std::ranges::for_each(std::ranges::subrange(other.sequence<0>().begin_element(), other.sequence<0>().end_element()), [this](element_t &other_element) { this->emplace_back(std::move(other_element)); });
            }
        }
        augmented_deque_t &operator=(augmented_deque_t &&other) & // move assignment operator
        {
            if (this == &other)
                return *this;
            this->clear();
            if (this->allocator_element == other.allocator_element)
                swap_end_nodes(other);
            else
            {
                if constexpr (std::allocator_traits<allocator_type>::propagate_on_container_move_assignment::value)
                {
                    destroy_end_nodes();
                    this->allocator_element = std::move(other.allocator_element);
                    create_end_nodes();
                    swap_end_nodes(other);
                }
                else
                    std::ranges::for_each(std::ranges::subrange(other.sequence<0>().begin_element(), other.sequence<0>().end_element()), [this](element_t &other_element) { this->emplace_back(std::move(other_element)); });
            }
            return *this;
        }
        void swap(augmented_deque_t &other)
        {
            if (this->allocator_element == other.allocator_element)
                swap_end_nodes(other);
            else
            {
                if constexpr (std::allocator_traits<allocator_type>::propagate_on_container_swap::value)
                {
                    std::ranges::swap(this->allocator_element, other.allocator_element);
                    swap_end_nodes(other);
                }
                else
                {
                    augmented_deque_t temp_this(other, this->allocator_element);
                    augmented_deque_t temp_other(*this, other.allocator_element);
                    swap_end_nodes(temp_this);
                    other.swap_end_nodes(temp_other);
                }
            }
        }
        ~augmented_deque_t()
        {
            clear();
            destroy_end_nodes();
        }

        bool empty() const { return stride1_sequence.list_node_count() == 0; }
        size_type size() const { return stride1_sequence.list_node_count(); }

        reference at(size_type pos) { return stride1_sequence.begin_element()[pos]; }
        const_reference at(size_type pos) const { return stride1_sequence.begin_element()[pos]; }
        reference operator[](size_type pos) { return stride1_sequence.begin_element()[pos]; }
        const_reference operator[](size_type pos) const { return stride1_sequence.begin_element()[pos]; }
        reference front() { return stride1_sequence.begin_element()[0]; }
        const_reference front() const { return stride1_sequence.begin_element()[0]; }
        reference back() { return stride1_sequence.end_element()[-1]; }
        const_reference back() const { return stride1_sequence.end_element()[-1]; }

        template<typename... args_t>
        void emplace_back(args_t... args)
        {
            stride1_sequence.emplace_back(this->allocator_element, std::forward<args_t>(args)...);
            [&]<std::size_t... I>(std::index_sequence<I...>) {
                (..., std::get<I>(other_stride_sequences).push_back(this->allocator_element, stride1_sequence_t::list_node_t::untagged_prev(stride1_sequence.list_node_end)));
            }(std::make_index_sequence<std::tuple_size_v<other_strides_stride_to_projector_and_accumulator_map_t>>());
        }
        template<typename... args_t>
        void emplace_front(args_t... args)
        {
            stride1_sequence.emplace_front(this->allocator_element, std::forward<args_t>(args)...);
            [&]<std::size_t... I>(std::index_sequence<I...>) {
                (..., std::get<I>(other_stride_sequences).push_front(this->allocator_element, stride1_sequence_t::list_node_t::untagged_next(stride1_sequence.list_node_end)));
            }(std::make_index_sequence<std::tuple_size_v<other_strides_stride_to_projector_and_accumulator_map_t>>());
        }
        void push_back(element_t const &value) { this->emplace_back(value); }
        void push_front(element_t const &value) { this->emplace_front(value); }

        void pop_back()
        {
            stride1_sequence.pop_back(this->allocator_element);
            [&]<std::size_t... I>(std::index_sequence<I...>) {
                (..., std::get<I>(other_stride_sequences).pop_back(this->allocator_element, stride1_sequence.list_node_end));
            }(std::make_index_sequence<std::tuple_size_v<other_strides_stride_to_projector_and_accumulator_map_t>>());
        }
        void pop_front()
        {
            stride1_sequence.pop_front(this->allocator_element);
            [&]<std::size_t... I>(std::index_sequence<I...>) {
                (..., std::get<I>(other_stride_sequences).pop_front(this->allocator_element, stride1_sequence.list_node_end));
            }(std::make_index_sequence<std::tuple_size_v<other_strides_stride_to_projector_and_accumulator_map_t>>());
        }

        friend void swap(augmented_deque_t &lhs, augmented_deque_t &rhs) { lhs.swap(rhs); }
        friend bool operator==(augmented_deque_t const &lhs, augmented_deque_t const &rhs)
        {
            return std::ranges::equal(std::ranges::subrange(lhs.stride1_sequence.begin_element(), lhs.stride1_sequence.end_element()), std::ranges::subrange(rhs.stride1_sequence.begin_element(), rhs.stride1_sequence.end_element()));
        }
        friend auto operator<=>(augmented_deque_t const &lhs, augmented_deque_t const &rhs)
        {
#ifdef __EMSCRIPTEN__
            auto f1 = lhs.stride1_sequence.begin_element(), l1 = lhs.stride1_sequence.end_element(), f2 = rhs.stride1_sequence.begin_element(), l2 = rhs.stride1_sequence.end_element();
            bool exhaust1 = (f1 == l1);
            bool exhaust2 = (f2 == l2);
            for (; !exhaust1 && !exhaust2; exhaust1 = (++f1 == l1), exhaust2 = (++f2 == l2))
                if (auto c = comp(*f1, *f2); c != 0)
                    return c;
            return !exhaust1 ? std::strong_ordering::greater : !exhaust2 ? std::strong_ordering::less
                                                                         : std::strong_ordering::equal;
#else
            return std::lexicographical_compare_three_way(lhs.stride1_sequence.begin_element(), lhs.stride1_sequence.end_element(), rhs.stride1_sequence.begin_element(), rhs.stride1_sequence.end_element());
#endif
        }

        template<bool is_const>
        struct sequence_t_to_conditional_const_iterator_projected_storage_t
        {
            template<typename sequence_t>
            struct sequence_t_to_possibly_const_iterator_projected_storage_t : std::type_identity<detail::augmented_deque::iterator_projected_storage_t<is_const, typename sequence_t::sequence_config_t>>
            {};
        };
        template<bool is_const, typename sequence_config_t, std::size_t I = detail::utility::list_find_first_index_t<detail::utility::list_transform_t<sequences_t, sequence_t_to_conditional_const_iterator_projected_storage_t<is_const>::template sequence_t_to_possibly_const_iterator_projected_storage_t>, detail::augmented_deque::iterator_projected_storage_t<is_const, sequence_config_t>>::value> requires (I != sequences_count)
        static detail::augmented_deque::iterator_element_t<is_const, typename stride1_sequence_t::sequence_config_t> to_iterator_element(detail::augmented_deque::iterator_projected_storage_t<is_const, sequence_config_t> const &possibly_const_iterator_projected_storage)
        {
            if constexpr (I == 0)
                return {possibly_const_iterator_projected_storage.current_list_node};
            else
            {
                if (possibly_const_iterator_projected_storage.is_end())
                    return {detail::language::pointer_traits_static_cast<typename sequence_t<I>::pointer_list_node_end_t>(detail::language::tagged_ptr_bit0_unsetted(possibly_const_iterator_projected_storage.current_list_node))->actual_projected_storage_end.child};
                else
                    return {detail::language::pointer_traits_static_cast<typename sequence_t<I>::pointer_list_node_t>(possibly_const_iterator_projected_storage.current_list_node)->actual_projected_storage.child};
            }
        }

        template<std::size_t I, bool is_const> requires (I < sequences_count)
        static detail::augmented_deque::iterator_projected_storage_t<is_const, typename sequence_t<I>::sequence_config_t> to_iterator_projected_storage(detail::augmented_deque::iterator_element_t<is_const, typename stride1_sequence_t::sequence_config_t> const &possibly_const_iterator_element)
        {
            if constexpr (I == 0)
                return detail::augmented_deque::iterator_projected_storage_t<is_const, typename stride1_sequence_t::sequence_config_t>{possibly_const_iterator_element.current_list_node};
            else
            {
                if (possibly_const_iterator_element.is_end())
                    return detail::augmented_deque::iterator_projected_storage_t<is_const, typename std::tuple_element_t<I - 1, other_stride_sequences_t>::sequence_config_t>{std::get<I - 1>(detail::language::pointer_traits_static_cast<typename stride1_sequence_t::pointer_list_node_end_t>(detail::language::tagged_ptr_bit0_unsetted(possibly_const_iterator_element.current_list_node))->actual_projected_storage_end.parents)};
                else
                    return detail::augmented_deque::iterator_projected_storage_t<is_const, typename std::tuple_element_t<I - 1, other_stride_sequences_t>::sequence_config_t>{std::get<I - 1>(detail::language::pointer_traits_static_cast<typename stride1_sequence_t::pointer_list_node_t>(possibly_const_iterator_element.current_list_node)->actual_projected_storage.parents)};
            }
        }

        void update_range(typename stride1_sequence_t::const_iterator_element_t const &const_iterator_element_begin, typename stride1_sequence_t::const_iterator_element_t const &const_iterator_element_end)
        {
            [&]<std::size_t... I>(std::index_sequence<I...>) {
                (..., ([this, &const_iterator_element_begin, &const_iterator_element_end] {
                    typename sequence_t<I>::const_iterator_projected_storage_t const_iterator_projected_storage_begin=to_iterator_projected_storage<I>(const_iterator_element_begin),const_iterator_projected_storage_end=to_iterator_projected_storage<I>(const_iterator_element_end);
                    assert(const_iterator_projected_storage_begin <= const_iterator_projected_storage_end);
                    if(const_iterator_projected_storage_begin != const_iterator_projected_storage_end)
                    {
                        auto sequence_functor_forward = detail::augmented_deque::make_sequence_functor<false>(allocator_element, &sequence<I>());
                        sequence_functor_forward.update_range_impl(detail::language::pointer_traits_static_cast<typename sequence_t<I>::pointer_list_node_t>(const_iterator_projected_storage_begin.current_list_node),
                            detail::language::pointer_traits_static_cast<typename sequence_t<I>::pointer_list_node_t>(std::ranges::prev(const_iterator_projected_storage_end).current_list_node));
                    } }()));
            }(std::make_index_sequence<sequences_count>());
        }

        template<typename = void, bool is_const, typename sequence_config_t, std::size_t I = detail::utility::list_find_first_index_t<detail::utility::list_transform_t<sequences_t, sequence_t_to_conditional_const_iterator_projected_storage_t<is_const>::template sequence_t_to_possibly_const_iterator_projected_storage_t>, detail::augmented_deque::iterator_projected_storage_t<is_const, sequence_config_t>>::value> requires (I < sequences_count)
        typename sequence_t<I>::accumulated_storage_t
            read_range(detail::augmented_deque::iterator_projected_storage_t<is_const, sequence_config_t> const &const_iterator_projected_storage_begin, detail::augmented_deque::iterator_projected_storage_t<is_const, sequence_config_t> const &const_iterator_projected_storage_end) const
        {
            assert(const_iterator_projected_storage_begin <= const_iterator_projected_storage_end);
            if (const_iterator_projected_storage_begin != const_iterator_projected_storage_end)
            {
                auto sequence_functor_forward = detail::augmented_deque::make_sequence_functor<false>(allocator_element, &sequence<I>());
                return sequence_functor_forward.read_range_impl(detail::language::pointer_traits_static_cast<typename sequence_t<I>::pointer_list_node_t>(const_iterator_projected_storage_begin.current_list_node), detail::language::pointer_traits_static_cast<typename sequence_t<I>::pointer_list_node_t>(std::ranges::prev(const_iterator_projected_storage_end).current_list_node));
            }
            else
                return sequence<I>().projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple());
        }

        template<size_t I> requires (I < sequences_count)
        typename sequence_t<I>::accumulated_storage_t read_range(typename stride1_sequence_t::const_iterator_element_t const &const_iterator_element_begin, typename stride1_sequence_t::const_iterator_element_t const &const_iterator_element_end) const
        {
            if constexpr (I == 0)
            {
                assert(const_iterator_element_begin <= const_iterator_element_end);
                return read_range(augmented_deque_t::to_iterator_projected_storage<I>(const_iterator_element_begin), augmented_deque_t::to_iterator_projected_storage<I>(const_iterator_element_end));
            }
            else
            {
                assert(const_iterator_element_begin <= const_iterator_element_end);
                if constexpr (sequence_t<I>::requested_stride == 1)
                    return read_range(augmented_deque_t::to_iterator_projected_storage<I>(const_iterator_element_begin), augmented_deque_t::to_iterator_projected_storage<I>(const_iterator_element_end));
                else
                {
                    if (typename sequence_t<I>::const_iterator_projected_storage_t const_iterator_projected_storage_begin = augmented_deque_t::to_iterator_projected_storage<I>(const_iterator_element_begin), const_iterator_projected_storage_end = augmented_deque_t::to_iterator_projected_storage<I>(const_iterator_element_end); const_iterator_projected_storage_begin == const_iterator_projected_storage_end)
                    {
                        if (const_iterator_element_begin == const_iterator_element_end)
                            return sequence<I>().projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple());
                        else
                        {
                            typename sequence_t<I>::projected_storage_t intermediate_projected_storage(sequence<I>().projector_and_accumulator().construct_projected_storage(allocator_element, const_iterator_element_begin, const_iterator_element_end, const_iterator_element_end - const_iterator_element_begin));
                            return sequence<I>().projector_and_accumulator().construct_accumulated_storage(allocator_element, std::make_tuple(std::cref(intermediate_projected_storage)));
                        }
                    }
                    else
                    {
                        auto get_left_operand = [&](auto return_accumulated_tuple) //
                        { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) //
                          {
                              if (auto const_iterator_element_chunk_begin = augmented_deque_t::to_iterator_element(const_iterator_projected_storage_begin); const_iterator_element_chunk_begin == const_iterator_element_begin)
                                  return return_accumulated_tuple(accumulated_tuple_so_far);
                              else
                              {
                                  ++const_iterator_projected_storage_begin;
                                  const_iterator_element_chunk_begin = augmented_deque_t::to_iterator_element(const_iterator_projected_storage_begin);
                                  typename sequence_t<I>::projected_storage_t intermediate_projected_storage(sequence<I>().projector_and_accumulator().construct_projected_storage(allocator_element, const_iterator_element_begin, const_iterator_element_chunk_begin, const_iterator_element_chunk_begin - const_iterator_element_begin));
                                  return return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::cref(intermediate_projected_storage))));
                              }
                          }; };
                        auto get_middle_operand = [&](auto return_accumulated_tuple) //
                        { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) //
                          {
                              if (const_iterator_projected_storage_begin == const_iterator_projected_storage_end)
                                  return return_accumulated_tuple(accumulated_tuple_so_far);
                              else
                              {
                                  typename sequence_t<I>::accumulated_storage_t intermediate_accumulated_storage(read_range(const_iterator_projected_storage_begin, const_iterator_projected_storage_end));
                                  return return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(intermediate_accumulated_storage))));
                              }
                          }; };
                        auto get_right_operand = [&](auto return_accumulated_tuple) //
                        { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) //
                          {
                              if (auto const_iterator_element_chunk_end = augmented_deque_t::to_iterator_element(const_iterator_projected_storage_end); const_iterator_element_chunk_end == const_iterator_element_end)
                                  return return_accumulated_tuple(accumulated_tuple_so_far);
                              else
                              {
                                  typename sequence_t<I>::projected_storage_t intermediate_projected_storage(sequence<I>().projector_and_accumulator().construct_projected_storage(allocator_element, const_iterator_element_chunk_end, const_iterator_element_end, const_iterator_element_end - const_iterator_element_chunk_end));
                                  return return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::cref(intermediate_projected_storage))));
                              }
                          }; };
                        auto return_accumulated_tuple = [&](auto accumulated_tuple_so_far) { return sequence<I>().projector_and_accumulator().construct_accumulated_storage(allocator_element, accumulated_tuple_so_far); };
                        return get_left_operand(get_middle_operand(get_right_operand(return_accumulated_tuple)))(std::make_tuple());
                    }
                }
            }
        }

        template<size_t I, detail::concepts::invocable_r<bool, typename sequence_t<I>::accumulated_storage_t const &> monotonic_predicate_t> requires (I < sequences_count)
        typename sequence_t<I>::iterator_projected_storage_t find_by_monotonic_predicate(monotonic_predicate_t const &monotonic_predicate)
        {
            auto sequence_functor_forward = detail::augmented_deque::make_sequence_functor<false>(allocator_element, &sequence<I>());
            return {sequence_functor_forward.find_by_monotonic_predicate(monotonic_predicate)};
        }
        template<size_t I, detail::concepts::invocable_r<bool, typename sequence_t<I>::accumulated_storage_t const &> monotonic_predicate_t> requires (I < sequences_count)
        typename sequence_t<I>::const_iterator_projected_storage_t find_by_monotonic_predicate(monotonic_predicate_t const &monotonic_predicate) const
        {
            auto sequence_functor_forward = detail::augmented_deque::make_sequence_functor<false>(allocator_element, &sequence<I>());
            return {sequence_functor_forward.find_by_monotonic_predicate(monotonic_predicate)};
        }

        template<size_t I, /*std::output_iterator<typename sequence_t<I>::iterator_projected_storage_t>*/ typename iterator_output_iterator_projected_storage_t, typename heap_predicate_t> requires (I < sequences_count && detail::concepts::invocable_r<heap_predicate_t, bool, typename sequence_t<I>::accumulated_storage_t &> && detail::concepts::invocable_r<heap_predicate_t, bool, typename sequence_t<I>::projected_storage_or_element_t const &>)
        void find_by_heap_predicate(iterator_output_iterator_projected_storage_t iterator_output_iterator_projected_storage, heap_predicate_t const &heap_predicate)
        {
            auto sequence_functor_forward = detail::augmented_deque::make_sequence_functor<false>(allocator_element, &sequence<I>());
            sequence_functor_forward.find_by_heap_predicate(
                detail::iterator::transform_output_iterator_t{
                    .wrapped_iterator = iterator_output_iterator_projected_storage,
                    .transformer = [](typename sequence_t<I>::pointer_navigator_t value) { return typename sequence_t<I>::iterator_projected_storage_t{value}; },
                },
                heap_predicate
            );
        }
        template<size_t I, /*std::output_iterator<typename sequence_t<I>::const_iterator_projected_storage_t>*/ typename iterator_output_const_iterator_projected_storage_t, typename heap_predicate_t> requires (I < sequences_count && detail::concepts::invocable_r<heap_predicate_t, bool, typename sequence_t<I>::accumulated_storage_t &> && detail::concepts::invocable_r<heap_predicate_t, bool, typename sequence_t<I>::projected_storage_or_element_t const &>)
        void find_by_heap_predicate(iterator_output_const_iterator_projected_storage_t iterator_output_const_iterator_projected_storage, heap_predicate_t const &heap_predicate) const
        {
            auto sequence_functor_forward = detail::augmented_deque::make_sequence_functor<false>(allocator_element, &sequence<I>());
            sequence_functor_forward.find_by_heap_predicate(
                detail::iterator::transform_output_iterator_t{
                    .wrapped_iterator = iterator_output_const_iterator_projected_storage,
                    .transformer = [](typename sequence_t<I>::pointer_navigator_t value) { return typename sequence_t<I>::const_iterator_projected_storage_t{value}; },
                },
                heap_predicate
            );
        }

        template<size_t I, typename heap_predicate_t> requires (I < sequences_count && detail::concepts::invocable_r<heap_predicate_t, bool, typename sequence_t<I>::accumulated_storage_t &> && detail::concepts::invocable_r<heap_predicate_t, bool, typename sequence_t<I>::projected_storage_or_element_t const &>)
        detail::coroutine::generator_t<typename sequence_t<I>::iterator_projected_storage_t> find_by_heap_predicate_generator(heap_predicate_t const &heap_predicate)
        {
            auto sequence_functor_forward = detail::augmented_deque::make_sequence_functor<false>(allocator_element, &sequence<I>());
            for (typename sequence_t<I>::pointer_navigator_t value : sequence_functor_forward.find_by_heap_predicate_generator(heap_predicate))
                co_yield typename sequence_t<I>::iterator_projected_storage_t{value};
        }
        template<size_t I, typename heap_predicate_t> requires (I < sequences_count && detail::concepts::invocable_r<heap_predicate_t, bool, typename sequence_t<I>::accumulated_storage_t &> && detail::concepts::invocable_r<heap_predicate_t, bool, typename sequence_t<I>::projected_storage_or_element_t const &>)
        detail::coroutine::generator_t<typename sequence_t<I>::const_iterator_projected_storage_t> find_by_heap_predicate_generator(heap_predicate_t const &heap_predicate) const
        {
            auto sequence_functor_forward = detail::augmented_deque::make_sequence_functor<false>(allocator_element, &sequence<I>());
            for (typename sequence_t<I>::pointer_navigator_t value : sequence_functor_forward.find_by_heap_predicate_generator(heap_predicate))
                co_yield typename sequence_t<I>::const_iterator_projected_storage_t{value};
        }
    };

    template<std::input_iterator iterator_t, std::sentinel_for<iterator_t> sentinel_t, typename allocator_t = std::allocator<typename std::iterator_traits<iterator_t>::value_type>> requires (requires { std::declval<allocator_t &>().allocate(std::size_t{}); })
    augmented_deque_t(iterator_t, sentinel_t, allocator_t = allocator_t()) -> augmented_deque_t<typename std::iterator_traits<iterator_t>::value_type, allocator_t>;
} // namespace augmented_containers

#endif // AUGMENTED_DEQUE_HPP
