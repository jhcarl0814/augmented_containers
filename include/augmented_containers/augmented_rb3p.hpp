#ifndef AUGMENTED_RB3P_HPP
#define AUGMENTED_RB3P_HPP

#include <algorithm>
#include <cassert>
#include <coroutine>
#include <exception>
#include <functional>
#include <memory>
#include <optional>
#include <ranges>
#include <utility>
#include <variant>
#include <vector>

namespace augmented_containers
{
#ifndef AUGMENTED_CONTAINERS_AUGMENTED_SEQUENCE_ENUM
    #define AUGMENTED_CONTAINERS_AUGMENTED_SEQUENCE_ENUM
    enum class augmented_sequence_physical_representation_e {
        rb3p,
        rb2p,
        finger_tree,
    };

    enum class augmented_sequence_size_management_e {
        no_size,
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
            target_pointer_t pointer_traits_static_cast(source_pointer_t source_pointer) { return std::pointer_traits<target_pointer_t>::pointer_to(*static_cast<typename std::pointer_traits<target_pointer_t>::element_type *>(std::to_address(source_pointer))); }

            template<typename target_pointer_t, typename source_pointer_t>
            target_pointer_t pointer_traits_reinterpret_cast(source_pointer_t source_pointer) { return std::pointer_traits<target_pointer_t>::pointer_to(*reinterpret_cast<typename std::pointer_traits<target_pointer_t>::element_type *>(std::to_address(source_pointer))); }
#endif // AUGMENTED_CONTAINERS_LANGUAGE_POINTER_TRAITS_CAST

#ifndef AUGMENTED_CONTAINERS_LANGUAGE_TAGGED_PTR_BIT0
    #define AUGMENTED_CONTAINERS_LANGUAGE_TAGGED_PTR_BIT0
            template<typename pointer_t>
            bool tagged_ptr_bit0_is_setted(pointer_t p) { return (reinterpret_cast<uintptr_t>(std::to_address(p)) & 0b1) != 0; }
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
            bool tagged_ptr_bit1_is_setted(pointer_t p) { return (reinterpret_cast<uintptr_t>(std::to_address(p)) & 0b10) != 0; }
            template<typename pointer_t>
            auto tagged_ptr_bit1_unsetted_relaxed(pointer_t p) { return std::pointer_traits<pointer_t>::pointer_to(*reinterpret_cast<typename std::pointer_traits<pointer_t>::element_type *>(reinterpret_cast<uintptr_t>(std::to_address(p)) & ~0b10)); }
            template<typename pointer_t>
            auto tagged_ptr_bit1_unsetted(pointer_t p) { return assert(tagged_ptr_bit1_is_setted(p)), tagged_ptr_bit1_unsetted_relaxed(p); }
            template<typename pointer_t>
            auto tagged_ptr_bit1_setted_relaxed(pointer_t p) { return std::pointer_traits<pointer_t>::pointer_to(*reinterpret_cast<typename std::pointer_traits<pointer_t>::element_type *>(reinterpret_cast<uintptr_t>(std::to_address(p)) | 0b10)); }
            template<typename pointer_t>
            auto tagged_ptr_bit1_setted(pointer_t p) { return assert(!tagged_ptr_bit1_is_setted(p)), tagged_ptr_bit1_setted_relaxed(p); }
#endif // AUGMENTED_CONTAINERS_LANGUAGE_TAGGED_PTR_BIT1

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

        namespace augmented_sequence_rb3p
        {
            using namespace language;
            using namespace concepts;
            using namespace utility;
            using namespace coroutine;

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
            {};
            template<bool add>
            struct add_node_count_member_t
            {};
            template<>
            struct add_node_count_member_t<true>
            {
                std::size_t node_count = 0;
            };

            template<typename allocator_element_t>
            struct alignas(std::max({alignof(void *), static_cast<std::size_t>(0b100)})) rb3p_node_navigator_t;

            template<typename allocator_element_t>
            struct proxy_color_t
            {
                rb3p_node_navigator_t<allocator_element_t> *this_;

                operator bool() const { return tagged_ptr_bit1_is_setted(this_->parent_); }
                proxy_color_t &operator=(bool other)
                {
                    this_->parent_ = !other ? tagged_ptr_bit1_unsetted_relaxed(this_->parent_) : tagged_ptr_bit1_setted_relaxed(this_->parent_);
                    return *this;
                }
                proxy_color_t &operator=(proxy_color_t const &other) { return this->operator=(other.operator bool()); }
            };
            template<typename allocator_element_t>
            struct proxy_parent_t
            {
                rb3p_node_navigator_t<allocator_element_t> *this_;

                operator rb3p_node_navigator_t<allocator_element_t> *() const { return tagged_ptr_bit1_unsetted_relaxed(this_->parent_); }
                rb3p_node_navigator_t<allocator_element_t> *operator->() const { return this->operator rb3p_node_navigator_t<allocator_element_t> *(); }
                proxy_parent_t &operator=(rb3p_node_navigator_t<allocator_element_t> *other)
                {
                    this_->parent_ = !tagged_ptr_bit1_is_setted(this_->parent_) ? other : tagged_ptr_bit1_setted(other);
                    return *this;
                }
                proxy_parent_t(rb3p_node_navigator_t<allocator_element_t> *this_) : this_(this_) {}
                proxy_parent_t(proxy_parent_t const &other) = default;
                proxy_parent_t &operator=(proxy_parent_t const &other) { return this->operator=(other.operator rb3p_node_navigator_t<allocator_element_t> *()); }
                friend void swap(proxy_parent_t &lhs, proxy_parent_t &rhs)
                {
                    rb3p_node_navigator_t<allocator_element_t> *temp = lhs;
                    lhs = rhs;
                    rhs = temp;
                }
                friend void swap(proxy_parent_t &lhs, proxy_parent_t &&rhs) { std::ranges::swap(lhs, rhs); }
                friend void swap(proxy_parent_t &&lhs, proxy_parent_t &rhs) { std::ranges::swap(lhs, rhs); }
                friend void swap(proxy_parent_t &&lhs, proxy_parent_t &&rhs) { std::ranges::swap(lhs, rhs); }
            };
            template<typename allocator_element_t>
            struct proxy_child_t
            {
                rb3p_node_navigator_t<allocator_element_t> *this_;
                rb3p_node_navigator_t<allocator_element_t> *(rb3p_node_navigator_t<allocator_element_t>::*p_child);
                operator rb3p_node_navigator_t<allocator_element_t> *() const { return this_->*p_child; }
                rb3p_node_navigator_t<allocator_element_t> *operator->() const { return this->operator rb3p_node_navigator_t<allocator_element_t> *(); }
                proxy_child_t &operator=(rb3p_node_navigator_t<allocator_element_t> *other)
                {
                    this_->*p_child = other;
                    return *this;
                }
                proxy_child_t(rb3p_node_navigator_t<allocator_element_t> *this_, rb3p_node_navigator_t<allocator_element_t> *(rb3p_node_navigator_t<allocator_element_t>::*p_child)) : this_(this_), p_child(p_child) {}
                proxy_child_t(proxy_child_t const &other) = default;
                proxy_child_t &operator=(proxy_child_t const &other) { return this->operator=(other.operator rb3p_node_navigator_t<allocator_element_t> *()); }
                friend void swap(proxy_child_t &lhs, proxy_child_t &rhs)
                {
                    rb3p_node_navigator_t<allocator_element_t> *temp = lhs;
                    lhs = rhs;
                    rhs = temp;
                }
                friend void swap(proxy_child_t &lhs, proxy_child_t &&rhs) { std::ranges::swap(lhs, rhs); }
                friend void swap(proxy_child_t &&lhs, proxy_child_t &rhs) { std::ranges::swap(lhs, rhs); }
                friend void swap(proxy_child_t &&lhs, proxy_child_t &&rhs) { std::ranges::swap(lhs, rhs); }
            };
        } // namespace augmented_sequence_rb3p
    } // namespace detail
} // namespace augmented_containers
namespace std
{
    template<typename allocator_element_t>
    struct std::pointer_traits<augmented_containers::detail::augmented_sequence_rb3p::proxy_parent_t<allocator_element_t>>
    {
        using pointer = augmented_containers::detail::augmented_sequence_rb3p::proxy_parent_t<allocator_element_t>;
        using element_type = augmented_containers::detail::augmented_sequence_rb3p::rb3p_node_navigator_t<allocator_element_t>;
        using difference_type = std::ptrdiff_t;
        template<class U>
        using rebind = U *;
        static element_type *pointer_to(element_type &r) { return &r; }
        static element_type *to_address(pointer p) noexcept { return p.operator element_type *(); }
    };
    template<typename allocator_element_t>
    struct std::pointer_traits<augmented_containers::detail::augmented_sequence_rb3p::proxy_child_t<allocator_element_t>>
    {
        using pointer = augmented_containers::detail::augmented_sequence_rb3p::proxy_child_t<allocator_element_t>;
        using element_type = augmented_containers::detail::augmented_sequence_rb3p::rb3p_node_navigator_t<allocator_element_t>;
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
        namespace augmented_sequence_rb3p
        {
            template<typename allocator_element_t>
            struct alignas(std::max({alignof(void *), static_cast<std::size_t>(0b100)})) rb3p_node_navigator_t
            {
                rb3p_node_navigator_t *parent_ = nullptr, *child_left_ = nullptr, *child_right_ = nullptr;

                static rb3p_node_navigator_t *untagged_root_or_tagged_end(rb3p_node_navigator_t *node_end) { return tagged_ptr_bit0_unsetted(node_end)->parent_ == node_end ? node_end : tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end)->parent_); }
                static rb3p_node_navigator_t *untagged_front_or_tagged_end(rb3p_node_navigator_t *node_end) { return tagged_ptr_bit0_unsetted(node_end)->child_right_ == node_end ? node_end : tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end)->child_right_); }
                static rb3p_node_navigator_t *untagged_back_or_tagged_end(rb3p_node_navigator_t *node_end) { return tagged_ptr_bit0_unsetted(node_end)->child_left_ == node_end ? node_end : tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end)->child_left_); }

                proxy_color_t<allocator_element_t> color() { return {this}; }

                proxy_parent_t<allocator_element_t> parent() { return {this}; }

                proxy_child_t<allocator_element_t> child_left() { return {this, &rb3p_node_navigator_t::child_left_}; }
                proxy_child_t<allocator_element_t> child_right() { return {this, &rb3p_node_navigator_t::child_right_}; }

                struct p_child_left_or_right_t
                {
                    proxy_child_t<allocator_element_t> (rb3p_node_navigator_t::*mp_child_left_or_right)();
                    friend decltype(auto) operator->*(rb3p_node_navigator_t * navigator, p_child_left_or_right_t const &p_child_left_or_right) { return (navigator->*p_child_left_or_right.mp_child_left_or_right)(); }

                    template<bool is_reversed>
                    static constexpr p_child_left_or_right_t make_p_child_left_or_right(bool child_left_or_right) { return {.mp_child_left_or_right = !child_left_or_right ? (!is_reversed ? &rb3p_node_navigator_t::child_left : &rb3p_node_navigator_t::child_right) : (!is_reversed ? &rb3p_node_navigator_t::child_right : &rb3p_node_navigator_t::child_left)}; }
                };

                template<bool is_reversed_>
                struct parent_info_t
                {
                    static constexpr bool is_reversed = is_reversed_;
                    static constexpr p_child_left_or_right_t p_child_left = p_child_left_or_right_t::template make_p_child_left_or_right<is_reversed>(false), p_child_right = p_child_left_or_right_t::template make_p_child_left_or_right<is_reversed>(true);

                    rb3p_node_navigator_t *parent;
                    bool is_left_or_right_child_of_parent;
                    bool is_end() const { return tagged_ptr_bit0_is_setted(parent); }

                    parent_info_t() : parent(nullptr) {}
                    parent_info_t(rb3p_node_navigator_t *this_) : parent(this_->parent())
                    {
                        if (!is_end())
                        {
                            if (parent->*p_child_left == this_) is_left_or_right_child_of_parent = false;
                            else if (parent->*p_child_right == this_) is_left_or_right_child_of_parent = true;
                            else std::unreachable();
                        }
                    }
                };

                template<typename node_end_t>
                struct node_end_functions_t
                {
                    static node_end_t *create_node_end()
                    {
                        node_end_t *result = new node_end_t();
                        result->parent() = result->child_left() = result->child_right() = tagged_ptr_bit0_setted(result);
                        return tagged_ptr_bit0_setted(result);
                    }
                };
            };

            template<typename config_t>
            struct rb3p_node_end_t : rb3p_node_navigator_t<typename config_t::allocator_element_t>, rb3p_node_navigator_t<typename config_t::allocator_element_t>::template node_end_functions_t<rb3p_node_end_t<config_t>>, add_node_count_member_t<static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_node_end>
            {
                typename config_t::accumulator_t accumulator;
                rb3p_node_end_t()
                {
                    this->parent() = tagged_ptr_bit0_setted(this);
                }
            };
            template<typename config_t>
            struct rb3p_node_t : rb3p_node_navigator_t<typename config_t::allocator_element_t>, add_accumulated_storage_member_t<typename config_t::pointer_element_t, typename config_t::accumulated_storage_t, rb3p_node_t<config_t>>, add_node_count_member_t<static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_each_node_except_node_end>
            {
                alignas(typename config_t::element_t) std::byte element_buffer[sizeof(typename config_t::element_t)]; // element_t element;
                typename config_t::const_pointer_element_t p_element() const { return std::pointer_traits<typename config_t::const_pointer_element_t>::pointer_to(*reinterpret_cast<typename config_t::element_t const *>(&element_buffer)); }
                typename config_t::pointer_element_t p_element() { return std::pointer_traits<typename config_t::pointer_element_t>::pointer_to(*reinterpret_cast<typename config_t::element_t *>(&element_buffer)); }
            };
            template<typename ostream_t, typename allocator_element_t_>
            ostream_t &&operator<<(ostream_t &&ostream, rb3p_node_navigator_t<allocator_element_t_> &rb3p_node_navigator_all)
            {
                struct config_t
                {
                    using element_t = char;
                    using allocator_element_t = std::allocator<char>;
                    using accumulator_t = void;
                    using augmented_sequence_physical_representation_t = std::integral_constant<augmented_sequence_physical_representation_e, augmented_sequence_physical_representation_e::rb3p>;
                    using augmented_sequence_size_management_t = std::integral_constant<augmented_sequence_size_management_e, augmented_sequence_size_management_e::no_size>;

                    using pointer_element_t = char *;
                    using const_pointer_element_t = char const *;
                    using accumulated_storage_t = void;
                };
                rb3p_node_t<config_t> &rb3p_node = static_cast<rb3p_node_t<config_t> &>(rb3p_node_navigator_all);
                if (std::addressof(rb3p_node) == nullptr)
                    return std::forward<ostream_t>(ostream << std::addressof(rb3p_node));
                else
                    return std::forward<ostream_t>(ostream << std::addressof(rb3p_node) << rb3p_node.color() //
                                                           << rb3p_node.parent() << rb3p_node.child_left() << rb3p_node.child_right() << *rb3p_node.p_element());
            }


            template<bool is_const_, bool is_reversed_, typename config_t>
            struct rb3p_iterator_t
            {
                static constexpr bool is_const = is_const_;
                static constexpr bool is_reversed = is_reversed_;

                using element_t = typename config_t::element_t;
                using pointer_element_t = typename config_t::pointer_element_t;

                using accumulated_storage_t = typename config_t::accumulated_storage_t;
                using pointer_accumulated_storage_t = typename std::pointer_traits<pointer_element_t>::template rebind<conditional_const_t<is_const, accumulated_storage_t>>;

                using navigator_t = rb3p_node_navigator_t<typename config_t::allocator_element_t>;
                using pointer_navigator_t = typename std::pointer_traits<pointer_element_t>::template rebind<navigator_t>;

                using node_end_t = rb3p_node_end_t<config_t>;
                using pointer_node_end_t = typename std::pointer_traits<pointer_element_t>::template rebind<node_end_t>;

                using node_t = rb3p_node_t<config_t>;

                static constexpr typename navigator_t::p_child_left_or_right_t p_child_left = navigator_t::p_child_left_or_right_t::template make_p_child_left_or_right<is_reversed>(false), p_child_right = navigator_t::p_child_left_or_right_t::template make_p_child_left_or_right<is_reversed>(true);

                template<bool is_reversed_predecessor>
                static navigator_t *predecessor(navigator_t *node)
                {
                    constexpr bool is_reversed = rb3p_iterator_t::is_reversed ^ is_reversed_predecessor;

                    constexpr typename navigator_t::p_child_left_or_right_t p_child_left = navigator_t::p_child_left_or_right_t::template make_p_child_left_or_right<is_reversed>(false), p_child_right = navigator_t::p_child_left_or_right_t::template make_p_child_left_or_right<is_reversed>(true);

                    if (tagged_ptr_bit0_is_setted(node)) // node_end
                    {
                        if (tagged_ptr_bit0_unsetted(node)->*p_child_left == node) // count==0
                            return node;
                        else // count!=0
                            return tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node)->*p_child_left);
                    }
                    else // node (not end)
                    {
                        if (tagged_ptr_bit0_is_setted(node->*p_child_left)) // (when root has left tree) root's leftmost descendent / (when root doesn't have left tree) root itself
                            return node->*p_child_left;
                        else
                        {
                            if (node->*p_child_left == nullptr) // (when (some not root) has left tree) (some not root)'s leftmost descendent / (when (not root) doesn't have left tree) (not root) itself
                            {
                                assert(!tagged_ptr_bit0_is_setted(node->parent())); // can not be root
                                navigator_t *node_previous = node;
                                typename navigator_t::template parent_info_t<is_reversed> node_current(node);
                                while (!node_current.is_left_or_right_child_of_parent) // find first left parent <-> right child
                                    node_previous = std::exchange(node_current, typename navigator_t::template parent_info_t<is_reversed>(node_current.parent)).parent;
                                return node_current.parent;
                            }
                            else // itself has left tree
                            {
                                navigator_t *node_current = node->*p_child_left;
                                while (node_current->*p_child_right != nullptr) // find rightmost descendent of left tree
                                    node_current = node_current->*p_child_right;
                                return node_current;
                            }
                        }
                    }
                }

                navigator_t *current_node = nullptr;
                rb3p_iterator_t(navigator_t *current_node) : current_node(current_node) {}
                bool is_end() const
                {
                    assert(current_node != nullptr);
                    return tagged_ptr_bit0_is_setted(current_node);
                }

                using non_const_iterator_t = rb3p_iterator_t<false, is_reversed, config_t>;
                using const_iterator_t = rb3p_iterator_t<true, is_reversed, config_t>;
                non_const_iterator_t to_non_const() const { return {current_node}; }
                const_iterator_t to_const() const { return {current_node}; }
                rb3p_iterator_t(non_const_iterator_t const &rhs) requires (is_const) : current_node(rhs.current_node) {} // https://quuxplusone.github.io/blog/2018/12/01/const-iterator-antipatterns/
                const_iterator_t &operator=(non_const_iterator_t const &rhs) & requires (is_const)
                {
                    current_node = rhs.current_node;
                    return *this;
                }

                // std::input_or_output_iterator / std::weakly_incrementable
                rb3p_iterator_t(rb3p_iterator_t const &) = default;
                rb3p_iterator_t &operator=(rb3p_iterator_t const &) & = default;
                using difference_type = std::ptrdiff_t;
                rb3p_iterator_t &operator++() &
                {
                    assert(current_node != nullptr);
                    current_node = predecessor<true>(current_node);
                    return *this;
                }
                rb3p_iterator_t operator++(int) &
                {
                    rb3p_iterator_t temp = *this;
                    operator++();
                    return temp;
                }

                // std::forward_iterator / std::sentinel_for / std::semiregular, std::forward_iterator / std::incrementable / std::regular
                rb3p_iterator_t() = default;

                // std::input_iterator / std::indirectly_readable
                using value_type = element_t;
                using pointer = typename std::pointer_traits<pointer_element_t>::template rebind<conditional_const_t<is_const, value_type>>;
                using reference = conditional_const_t<is_const, value_type> &;
                reference operator*() const &
                {
                    assert(current_node != nullptr);
                    assert(!tagged_ptr_bit0_is_setted(current_node));
                    return conditional_as_const<is_const>(*static_cast<node_t *>(std::to_address(current_node))->p_element());
                }
                pointer to_pointer_element() const & { return std::pointer_traits<pointer>::pointer_to(operator*()); }
                pointer operator->() const & { return to_pointer_element(); }
                static rb3p_iterator_t from_element_pointer(pointer ptr)
                {
#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Winvalid-offsetof"
#endif
                    return {std::pointer_traits<pointer_navigator_t>::pointer_to(*reinterpret_cast<node_t *>(const_cast<std::byte *>(reinterpret_cast<conditional_const_t<is_const, std::byte> *>(std::to_address(ptr))) - offsetof(node_t, element_buffer)))};
#ifdef __clang__
    #pragma clang diagnostic pop
#endif
                }

                pointer_accumulated_storage_t to_pointer_accumulated_storage() const & requires (!std::is_same_v<accumulated_storage_t, void>)
                {
                    assert(current_node != nullptr);
                    assert(!tagged_ptr_bit0_is_setted(current_node));
                    return std::pointer_traits<pointer_accumulated_storage_t>::pointer_to(conditional_as_const<is_const>(*static_cast<node_t *>(std::to_address(current_node))->p_accumulated_storage()));
                }
                static rb3p_iterator_t from_accumulated_storage_pointer(pointer_accumulated_storage_t ptr) requires (!std::is_same_v<accumulated_storage_t, void>)
                {
#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Winvalid-offsetof"
#endif
                    return {std::pointer_traits<pointer_navigator_t>::pointer_to(*reinterpret_cast<node_t *>(const_cast<std::byte *>(reinterpret_cast<conditional_const_t<is_const, std::byte> *>(std::to_address(ptr))) - offsetof(node_t, accumulated_storage_buffer)))};
#ifdef __clang__
    #pragma clang diagnostic pop
#endif
                }

                // std::forward_iterator / std::sentinel_for / __WeaklyEqualityComparableWith, std::forward_iterator / std::incrementable / std::regular
                template<std::bool_constant<is_const> * = nullptr> requires (is_const)
                friend bool operator==(const_iterator_t const &lhs, const_iterator_t const &rhs)
                {
                    assert((lhs.current_node != nullptr) == (rhs.current_node != nullptr));
                    return lhs.current_node == rhs.current_node;
                }
                template<std::bool_constant<is_const> * = nullptr> requires (!is_const)
                friend bool operator==(const_iterator_t const &lhs, non_const_iterator_t const &rhs) { return lhs == rhs.to_const(); }
                template<std::bool_constant<is_const> * = nullptr> requires (!is_const)
                friend bool operator==(non_const_iterator_t const &lhs, const_iterator_t const &rhs) { return lhs.to_const() == rhs; }
                template<std::bool_constant<is_const> * = nullptr> requires (!is_const)
                friend bool operator==(non_const_iterator_t const &lhs, non_const_iterator_t const &rhs) { return lhs.to_const() == rhs.to_const(); }
                friend bool operator==(rb3p_iterator_t const &lhs, [[maybe_unused]] std::default_sentinel_t const &rhs)
                {
                    assert(lhs.current_node != nullptr);
                    return tagged_ptr_bit0_is_setted(lhs.current_node);
                }

                // std::bidirectional_iterator
                rb3p_iterator_t &operator--() &
                {
                    assert(current_node != nullptr);
                    current_node = predecessor<false>(current_node);
                    return *this;
                }
                rb3p_iterator_t operator--(int) &
                {
                    rb3p_iterator_t temp = *this;
                    operator--();
                    return temp;
                }

                static constexpr bool support_random_access = static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_each_node_except_node_end;
                static std::size_t size_from_node_end(node_end_t *node_end) requires (support_random_access)
                {
                    if (tagged_ptr_bit0_unsetted(node_end)->parent() == node_end)
                        return 0;
                    else
                        return static_cast<node_t *>(tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end)->parent()))->node_count;
                }

                // std::random_access_iterator / std::totally_ordered / __PartiallyOrderedWith
                std::tuple<std::size_t, std::size_t> index_impl() const & requires (support_random_access)
                {
                    assert(current_node != nullptr);
                    if (tagged_ptr_bit0_is_setted(current_node))
                        return std::make_tuple(size_from_node_end(static_cast<node_end_t *>(current_node)), size_from_node_end(static_cast<node_end_t *>(current_node)));
                    else
                    {
                        navigator_t *current_node = this->current_node;
                        std::size_t current_index = 0;
                        while (true)
                        {
                            if (current_node->*p_child_left != nullptr && !tagged_ptr_bit0_is_setted(current_node->*p_child_left))
                                current_index += static_cast<node_t *>(static_cast<navigator_t *>(current_node->*p_child_left))->node_count;
                            while (true)
                            {
                                typename navigator_t::template parent_info_t<is_reversed> parent_info(current_node);
                                if (parent_info.is_end())
                                    goto reached_root;
                                else
                                {
                                    current_node = parent_info.parent;
                                    if (parent_info.is_left_or_right_child_of_parent)
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
                template<std::bool_constant<is_const> * = nullptr> requires (support_random_access && is_const)
                friend std::strong_ordering operator<=>(const_iterator_t const &lhs, const_iterator_t const &rhs)
                {
                    return std::get<0>(lhs.index_impl()) <=> std::get<0>(rhs.index_impl());
                }
                template<std::bool_constant<is_const> * = nullptr> requires (support_random_access && !is_const)
                friend std::strong_ordering operator<=>(const_iterator_t const &lhs, non_const_iterator_t const &rhs) { return lhs <=> rhs.to_const(); }
                template<std::bool_constant<is_const> * = nullptr> requires (support_random_access && !is_const)
                friend std::strong_ordering operator<=>(non_const_iterator_t const &lhs, const_iterator_t const &rhs) { return lhs.to_const() <=> rhs; }
                template<std::bool_constant<is_const> * = nullptr> requires (support_random_access && !is_const)
                friend std::strong_ordering operator<=>(non_const_iterator_t const &lhs, non_const_iterator_t const &rhs) { return lhs.to_const() <=> rhs.to_const(); }
                friend std::strong_ordering operator<=>(rb3p_iterator_t const &lhs, [[maybe_unused]] std::default_sentinel_t const &rhs) requires (support_random_access)
                {
                    assert(lhs.current_node != nullptr);
                    if (tagged_ptr_bit0_is_setted(lhs.current_node))
                        return std::strong_ordering::equal;
                    else
                        return std::strong_ordering::less;
                }
                std::size_t index() const & requires (support_random_access)
                {
                    assert(current_node != nullptr);
                    return std::get<0>(index_impl());
                }
                template<std::bool_constant<is_const> * = nullptr> requires (support_random_access && is_const)
                friend std::ptrdiff_t operator-(const_iterator_t const &lhs, const_iterator_t const &rhs)
                {
                    assert(lhs.current_node != nullptr);
                    assert(rhs.current_node != nullptr);
                    return static_cast<std::ptrdiff_t>(std::get<0>(lhs.index_impl())) - static_cast<std::ptrdiff_t>(std::get<0>(rhs.index_impl()));
                }
                template<std::bool_constant<is_const> * = nullptr> requires (support_random_access && !is_const)
                friend std::ptrdiff_t operator-(const_iterator_t const &lhs, non_const_iterator_t const &rhs) { return lhs - rhs.to_const(); }
                template<std::bool_constant<is_const> * = nullptr> requires (support_random_access && !is_const)
                friend std::ptrdiff_t operator-(non_const_iterator_t const &lhs, const_iterator_t const &rhs) { return lhs.to_const() - rhs; }
                template<std::bool_constant<is_const> * = nullptr> requires (support_random_access && !is_const)
                friend std::ptrdiff_t operator-(non_const_iterator_t const &lhs, non_const_iterator_t const &rhs) { return lhs.to_const() - rhs.to_const(); }
                friend std::ptrdiff_t operator-([[maybe_unused]] std::default_sentinel_t const &lhs, rb3p_iterator_t const &rhs) requires (support_random_access)
                {
                    assert(rhs.current_list_node != nullptr);
                    auto [index, size] = rhs.index_impl();
                    return static_cast<std::ptrdiff_t>(size) - static_cast<std::ptrdiff_t>(index);
                }
                friend std::ptrdiff_t operator-(rb3p_iterator_t const &lhs, [[maybe_unused]] std::default_sentinel_t const &rhs) requires (support_random_access)
                {
                    return -(rhs - lhs);
                }

                // std::random_access_iterator
                template<bool is_reversed_move_impl>
                void move_impl(std::size_t distance) & requires (support_random_access)
                {
                    constexpr bool is_reversed = rb3p_iterator_t::is_reversed ^ is_reversed_move_impl;

                    static constexpr typename navigator_t::p_child_left_or_right_t p_child_left = navigator_t::p_child_left_or_right_t::template make_p_child_left_or_right<is_reversed>(false), p_child_right = navigator_t::p_child_left_or_right_t::template make_p_child_left_or_right<is_reversed>(true);

                    assert(distance != 0);
                    auto find_in_tree = [](auto &this_, navigator_t *root, std::size_t index) -> navigator_t * //
                    {
                        if (root->*p_child_left != nullptr && !tagged_ptr_bit0_is_setted(root->*p_child_left))
                        {
                            if (index < static_cast<node_t *>(static_cast<navigator_t *>(root->*p_child_left))->node_count)
                                return this_(this_, root->*p_child_left, index);
                            else
                                index -= static_cast<node_t *>(static_cast<navigator_t *>(root->*p_child_left))->node_count;

                            if (index == 0)
                                return root;
                            else
                                --index;

                            assert(root->*p_child_right != nullptr && !tagged_ptr_bit0_is_setted(root->*p_child_right));
                            return this_(this_, root->*p_child_right, index);
                        }
                        else
                        {
                            if (index == 0)
                                return root;
                            else
                                --index;

                            assert(root->*p_child_right != nullptr && !tagged_ptr_bit0_is_setted(root->*p_child_right));
                            return this_(this_, root->*p_child_right, index);
                        }
                    };
                    while (true)
                    {
                        --distance;
                        if (current_node->*p_child_right != nullptr && !tagged_ptr_bit0_is_setted(current_node->*p_child_right))
                        {
                            if (distance < static_cast<node_t *>(static_cast<navigator_t *>(current_node->*p_child_right))->node_count)
                            {
                                current_node = find_in_tree(find_in_tree, current_node->*p_child_right, distance);
                                return;
                            }
                            else
                                distance -= static_cast<node_t *>(static_cast<navigator_t *>(current_node->*p_child_right))->node_count;
                        }
                        while (true)
                        {
                            typename navigator_t::template parent_info_t<is_reversed> parent_info(current_node);
                            if (parent_info.is_end())
                            {
                                distance %= size_from_node_end(static_cast<node_end_t *>(parent_info.parent)) + 1;
                                if (distance == 0)
                                    current_node = parent_info.parent;
                                else
                                    current_node = find_in_tree(find_in_tree, current_node, distance - 1);
                                return;
                            }
                            else
                            {
                                current_node = parent_info.parent;
                                if (!parent_info.is_left_or_right_child_of_parent)
                                    break;
                            }
                        }
                        if (distance == 0)
                            return;
                    }
                }
                rb3p_iterator_t &operator+=(std::ptrdiff_t offset) & requires (support_random_access)
                {
                    assert(current_node != nullptr);
                    if (offset == 0)
                        ;
                    else
                    {
                        if (tagged_ptr_bit0_is_setted(current_node))
                        {
                            offset %= static_cast<std::ptrdiff_t>(size_from_node_end(static_cast<node_end_t *>(current_node))) + 1;
                            auto find_in_tree = [](auto &this_, navigator_t *root, std::size_t index) -> navigator_t * //
                            {
                                if (root->*p_child_left != nullptr && !tagged_ptr_bit0_is_setted(root->*p_child_left))
                                {
                                    if (index < static_cast<node_t *>(static_cast<navigator_t *>(root->*p_child_left))->node_count)
                                        return this_(this_, root->*p_child_left, index);
                                    else
                                        index -= static_cast<node_t *>(static_cast<navigator_t *>(root->*p_child_left))->node_count;

                                    if (index == 0)
                                        return root;
                                    else
                                        --index;

                                    assert(root->*p_child_right != nullptr && !tagged_ptr_bit0_is_setted(root->*p_child_right));
                                    return this_(this_, root->*p_child_right, index);
                                }
                                else
                                {
                                    if (index == 0)
                                        return root;
                                    else
                                        --index;

                                    assert(root->*p_child_right != nullptr && !tagged_ptr_bit0_is_setted(root->*p_child_right));
                                    return this_(this_, root->*p_child_right, index);
                                }
                            };
                            if (offset == 0)
                                ;
                            else if (offset > 0)
                            {
                                current_node = find_in_tree(find_in_tree, tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(current_node)->parent()), offset - 1);
                            }
                            else if (offset < 0)
                            {
                                current_node = find_in_tree(find_in_tree, tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(current_node)->parent()), offset + (static_cast<std::ptrdiff_t>(size_from_node_end(static_cast<node_end_t *>(current_node))) + 1) - 1);
                            }
                            else std::unreachable();
                        }
                        else
                        {
                            if (offset == 0)
                                ;
                            else if (offset > 0)
                                move_impl<false>(offset);
                            else if (offset < 0)
                                move_impl<true>(-offset);
                            else std::unreachable();
                        }
                    }
                    return *this;
                }
                rb3p_iterator_t &operator-=(std::ptrdiff_t offset) & requires (support_random_access)
                {
                    return operator+=(-offset);
                }
                rb3p_iterator_t operator+(std::ptrdiff_t offset) const & requires (support_random_access)
                {
                    rb3p_iterator_t temp = *this;
                    temp += offset;
                    return temp;
                }
                rb3p_iterator_t operator-(std::ptrdiff_t offset) const & requires (support_random_access)
                {
                    return operator+(-offset);
                }
                friend rb3p_iterator_t operator+(std::ptrdiff_t offset, rb3p_iterator_t const &this_) requires (support_random_access)
                {
                    return *this_ + offset;
                }
                reference operator[](std::ptrdiff_t offset) const & requires (support_random_access)
                {
                    return *(*this + offset);
                }

                using iterator_concept = std::bidirectional_iterator_tag;
            };

            template<bool is_reversed_, typename config_t>
            struct rb3p_functor_t
            {
                static constexpr bool is_reversed = is_reversed_;

                using element_t = typename config_t::element_t;
                using allocator_element_t = typename config_t::allocator_element_t;
                using accumulated_storage_t = typename config_t::accumulated_storage_t;
                using accumulator_t = typename config_t::accumulator_t;

                using navigator_t = rb3p_node_navigator_t<typename config_t::allocator_element_t>;
                using node_end_t = rb3p_node_end_t<config_t>;
                using node_t = rb3p_node_t<config_t>;

                static constexpr bool uses_siblings = []() consteval -> bool //
                {
                    if constexpr (requires { std::convertible_to<decltype(accumulator_t::uses_siblings), bool>; })
                        return accumulator_t::uses_siblings;
                    return false;
                }();

                static constexpr typename navigator_t::p_child_left_or_right_t p_child_left = navigator_t::p_child_left_or_right_t::template make_p_child_left_or_right<is_reversed>(false), p_child_right = navigator_t::p_child_left_or_right_t::template make_p_child_left_or_right<is_reversed>(true);

                static bool empty(node_end_t *node_end)
                {
                    assert(tagged_ptr_bit0_is_setted(node_end));
                    if constexpr (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::no_size)
                        return tagged_ptr_bit0_unsetted(node_end)->parent() == node_end;
                    else if constexpr (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_node_end)
                        return tagged_ptr_bit0_unsetted(node_end)->parent() == node_end;
                    //                        return node_end->node_count == 0;
                    else if constexpr (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_each_node_except_node_end)
                        return tagged_ptr_bit0_unsetted(node_end)->parent() == node_end;
                    else
                        std::unreachable();
                }
                static std::size_t size(node_end_t *node_end)
                {
                    assert(tagged_ptr_bit0_is_setted(node_end));
                    if constexpr (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::no_size)
                        return std::ranges::distance(std::ranges::next(rb2p_iterator_t<false, false, config_t>(node_end)), rb2p_iterator_t<false, false, config_t>(node_end));
                    else if constexpr (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_node_end)
                        return tagged_ptr_bit0_unsetted(node_end)->node_count;
                    else if constexpr (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_each_node_except_node_end)
                    {
                        if (tagged_ptr_bit0_unsetted(node_end)->parent() == node_end)
                            return 0;
                        else
                        {
                            node_t *root = static_cast<node_t *>(tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end)->parent()));
                            return root->node_count;
                        }
                    }
                    else
                        std::unreachable();
                }
                static bool one_provided_not_empty(node_end_t *node_end)
                {
                    assert(tagged_ptr_bit0_is_setted(node_end));
                    if constexpr (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::no_size)
                    {
                        assert(tagged_ptr_bit0_unsetted(node_end)->parent() != node_end);
                        navigator_t *root = tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end)->parent());
                        return tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end)->child_left()) == root && tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end)->child_right()) == root;
                    }
                    else if constexpr (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_node_end)
                    {
                        assert(tagged_ptr_bit0_unsetted(node_end)->node_count != 0);
                        return tagged_ptr_bit0_unsetted(node_end)->node_count == 1;
                    }
                    else if constexpr (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_each_node_except_node_end)
                    {
                        assert(tagged_ptr_bit0_unsetted(node_end)->parent() != node_end);
                        node_t *root = static_cast<node_t *>(tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end)->parent()));
                        return root->node_count == 1;
                    }
                    else
                        std::unreachable();
                }

                struct refresh_node_count_and_accumulated_storage_schedule_t
                {
                    navigator_t *node;
                };
                struct refresh_node_count_and_accumulated_storage_and_above_schedule_t
                {
                    navigator_t *node;
                };
                struct refresh_node_count_and_accumulated_storage_and_above_until_schedule_t
                {
                    navigator_t *node;
                    navigator_t *node_end;
                };
                using schedules_t = std::conditional_t<uses_siblings, std::vector<std::variant<refresh_node_count_and_accumulated_storage_schedule_t, refresh_node_count_and_accumulated_storage_and_above_schedule_t, refresh_node_count_and_accumulated_storage_and_above_until_schedule_t>>, std::nullptr_t>;

                static void refresh_node_count(navigator_t *node)
                {
                    if constexpr (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_each_node_except_node_end)
                        static_cast<node_t *>(node)->node_count = (node->child_left() != nullptr && !tagged_ptr_bit0_is_setted(node->child_left()) ? static_cast<node_t *>(static_cast<navigator_t *>(node->child_left()))->node_count : 0) + 1 + (node->child_right() != nullptr && !tagged_ptr_bit0_is_setted(node->child_right()) ? static_cast<node_t *>(static_cast<navigator_t *>(node->child_right()))->node_count : 0);
                }
                static void refresh_accumulated_storage(accumulator_t const &accumulator, navigator_t *node)
                {
                    if constexpr (!std::is_same_v<accumulated_storage_t, void>)
                    {
                        auto get_left_operand = [&](auto return_accumulated_tuple) //
                        { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> void //
                          {
                              if (node->child_left() == nullptr || tagged_ptr_bit0_is_setted(node->child_left()))
                                  return_accumulated_tuple(accumulated_tuple_so_far);
                              else
                                  return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*static_cast<node_t *>(static_cast<navigator_t *>(node->child_left()))->p_accumulated_storage()))));
                          }; };
                        auto get_middle_operand = [&](auto return_accumulated_tuple) { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> void { return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::cref(*static_cast<node_t *>(node)->p_element())))); }; };
                        auto get_right_operand = [&](auto return_accumulated_tuple) //
                        { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> void //
                          {
                              if (node->child_right() == nullptr || tagged_ptr_bit0_is_setted(node->child_right()))
                                  return_accumulated_tuple(accumulated_tuple_so_far);
                              else
                                  return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*static_cast<node_t *>(static_cast<navigator_t *>(node->child_right()))->p_accumulated_storage()))));
                          }; };
                        auto return_accumulated_tuple = [&](auto accumulated_tuple_so_far) { accumulator.update_accumulated_storage(*static_cast<node_t *>(node)->p_accumulated_storage(), accumulated_tuple_so_far); };
                        get_left_operand(get_middle_operand(get_right_operand(return_accumulated_tuple)))(std::make_tuple());
                    }
                }
                static void refresh_node_count_and_accumulated_storage_impl(accumulator_t const &accumulator, navigator_t *node)
                {
                    refresh_node_count(node);
                    refresh_accumulated_storage(accumulator, node);
                }
                static void refresh_node_count_and_accumulated_storage(schedules_t &schedules, accumulator_t const &accumulator, navigator_t *node)
                {
                    if constexpr (uses_siblings)
                        schedules.push_back(refresh_node_count_and_accumulated_storage_schedule_t{.node = node});
                    else
                        refresh_node_count_and_accumulated_storage_impl(accumulator, node);
                }
                static void refresh_accumulated_storage_and_above(accumulator_t const &accumulator, navigator_t *node)
                {
                    if constexpr (!std::is_same_v<accumulated_storage_t, void>)
                    {
                        refresh_accumulated_storage(accumulator, node);
                        while (!tagged_ptr_bit0_is_setted(node->parent()))
                        {
                            node = node->parent();
                            refresh_accumulated_storage(accumulator, node);
                        }
                    }
                }
                static void refresh_node_count_and_accumulated_storage_and_above_impl(accumulator_t const &accumulator, navigator_t *node)
                {
                    if constexpr (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_each_node_except_node_end || !std::is_same_v<accumulated_storage_t, void>)
                    {
                        refresh_node_count_and_accumulated_storage_impl(accumulator, node);
                        while (!tagged_ptr_bit0_is_setted(node->parent()))
                        {
                            node = node->parent();
                            refresh_node_count_and_accumulated_storage_impl(accumulator, node);
                        }
                    }
                }
                static void refresh_node_count_and_accumulated_storage_and_above(schedules_t &schedules, accumulator_t const &accumulator, navigator_t *node)
                {
                    if constexpr (uses_siblings)
                        schedules.push_back(refresh_node_count_and_accumulated_storage_and_above_schedule_t{.node = node});
                    else
                        refresh_node_count_and_accumulated_storage_and_above_impl(accumulator, node);
                }
                static void refresh_node_count_and_accumulated_storage_and_above_until_impl(accumulator_t const &accumulator, navigator_t *node, navigator_t *node_end)
                {
                    if constexpr (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_each_node_except_node_end || !std::is_same_v<accumulated_storage_t, void>)
                    {
                        refresh_node_count_and_accumulated_storage_impl(accumulator, node);
                        while (!tagged_ptr_bit0_is_setted(node->parent()))
                        {
                            node = node->parent();
                            if (node == node_end)
                                break;
                            refresh_node_count_and_accumulated_storage_impl(accumulator, node);
                        }
                    }
                }
                static void refresh_node_count_and_accumulated_storage_and_above_until(schedules_t &schedules, accumulator_t const &accumulator, navigator_t *node, navigator_t *node_end)
                {
                    if constexpr (uses_siblings)
                        schedules.push_back(refresh_node_count_and_accumulated_storage_and_above_until_schedule_t{.node = node, .node_end = node_end});
                    else
                        refresh_node_count_and_accumulated_storage_and_above_until_impl(accumulator, node, node_end);
                }

                static void run_schedules(schedules_t &schedules, accumulator_t const &accumulator)
                {
                    if constexpr (uses_siblings)
                    {
                        for (auto &schedule : schedules)
                        {
                            std::visit([&]<typename schedule_t>(schedule_t &schedule) -> void //
                                       {
                                           if constexpr(std::is_same_v<schedule_t,refresh_node_count_and_accumulated_storage_schedule_t>)
                                               refresh_node_count_and_accumulated_storage_impl(accumulator,schedule.node);
                                           else if constexpr(std::is_same_v<schedule_t,refresh_node_count_and_accumulated_storage_and_above_schedule_t>)
                                               refresh_node_count_and_accumulated_storage_and_above_impl(accumulator,schedule.node);
                                           else if constexpr(std::is_same_v<schedule_t,refresh_node_count_and_accumulated_storage_and_above_until_schedule_t>)
                                           refresh_node_count_and_accumulated_storage_and_above_until_impl(accumulator,schedule.node,schedule.node_end); },
                                       schedule);
                        }
                    }
                }

                static void parent_inter_set(navigator_t *this_, typename navigator_t::template parent_info_t<is_reversed> const &parent_info)
                {
                    if (this_ != nullptr)
                        this_->parent() = parent_info.parent;
                    if (parent_info.is_end())
                        tagged_ptr_bit0_unsetted(parent_info.parent)->parent() = tagged_ptr_bit0_setted(this_);
                    else
                        (!parent_info.is_left_or_right_child_of_parent ? parent_info.parent->*p_child_left : parent_info.parent->*p_child_right) = this_;
                }
                static void child_left_inter_set(navigator_t *this_, navigator_t *child_left_new)
                {
                    assert(child_left_new == nullptr || !tagged_ptr_bit0_is_setted(child_left_new));
                    this_->*p_child_left = child_left_new;
                    if (child_left_new != nullptr)
                        child_left_new->parent() = this_;
                }
                static void child_right_inter_set(navigator_t *this_, navigator_t *child_right_new)
                {
                    assert(child_right_new == nullptr || !tagged_ptr_bit0_is_setted(child_right_new));
                    this_->*p_child_right = child_right_new;
                    if (child_right_new != nullptr)
                        child_right_new->parent() = this_;
                }

                struct bnode_t
                {
                    std::tuple<std::optional<navigator_t *>, navigator_t *, std::optional<navigator_t *>> keys;
                    int key_count;
                    typename navigator_t::template parent_info_t<is_reversed> center_key_parent_info;
                    std::optional<navigator_t *> opt_child_at(int index /* -3 / -1 / 1 / 3 */)
                    {
                        if (index == -3)
                        {
                            if (!std::get<0>(keys).has_value()) return std::nullopt;
                            else
                            {
                                if (std::get<0>(keys).value()->*p_child_left == nullptr /*|| tagged_ptr_bit0_is_setted(std::get<0>(keys).value()->*p_child_left)*/)
                                {
                                    assert(!tagged_ptr_bit0_is_setted(std::get<0>(keys).value()->*p_child_left));
                                    return std::nullopt;
                                }
                                else return std::get<0>(keys).value()->*p_child_left;
                            }
                        }
                        else if (index == -1)
                        {
                            if (!std::get<0>(keys).has_value())
                                return std::get<1>(keys)->*p_child_left;
                            else
                                return std::get<0>(keys).value()->*p_child_right;
                        }
                        else if (index == 1)
                        {
                            if (!std::get<2>(keys).has_value())
                                return std::get<1>(keys)->*p_child_right;
                            else
                                return std::get<2>(keys).value()->*p_child_left;
                        }
                        else if (index == 3)
                        {
                            if (!std::get<2>(keys).has_value()) return std::nullopt;
                            else
                            {
                                if (std::get<2>(keys).value()->*p_child_right == nullptr /*|| tagged_ptr_bit0_is_setted(std::get<2>(keys).value()->*p_child_right)*/)
                                {
                                    assert(!tagged_ptr_bit0_is_setted(std::get<2>(keys).value()->*p_child_right));
                                    return std::nullopt;
                                }
                                else return std::get<2>(keys).value()->*p_child_right;
                            }
                        }
                        else return std::nullopt;
                    }
                    navigator_t *key_at(int index /* -2 / 0 / 2 */)
                    {
                        if (index == -2)
                            return std::get<0>(keys).value();
                        else if (index == 0)
                            return std::get<1>(keys);
                        else if (index == 2)
                            return std::get<2>(keys).value();
                        else std::unreachable();
                    }
                    template<typename... other_affected_pointers_t>
                    navigator_t *exchange_key_at(int index /* -2 / 0 / 2 */, navigator_t *const key_new, other_affected_pointers_t &...other_affected_pointers)
                    {
                        assert(key_new != nullptr);
                        if (index == -2)
                        {
                            key_new->color() = std::get<0>(keys).value()->color();
                            child_left_inter_set(key_new, std::get<0>(keys).value()->*p_child_left);
                            child_right_inter_set(key_new, std::get<0>(keys).value()->*p_child_right);

                            child_left_inter_set(std::get<1>(keys), key_new);

                            (..., ([&]() {if(other_affected_pointers == std::get<0>(keys).value())other_affected_pointers = key_new; }()));
                            return std::exchange(std::get<0>(keys).value(), key_new);
                        }
                        else if (index == 0)
                        {
                            key_new->color() = std::get<1>(keys)->color();
                            parent_inter_set(key_new, center_key_parent_info);
                            child_left_inter_set(key_new, std::get<1>(keys)->*p_child_left);
                            child_right_inter_set(key_new, std::get<1>(keys)->*p_child_right);

                            (..., ([&]() {if(other_affected_pointers == std::get<1>(keys))other_affected_pointers = key_new; }()));
                            return std::exchange(std::get<1>(keys), key_new);
                        }
                        else if (index == 2)
                        {
                            key_new->color() = std::get<2>(keys).value()->color();
                            child_left_inter_set(key_new, std::get<2>(keys).value()->*p_child_left);
                            child_right_inter_set(key_new, std::get<2>(keys).value()->*p_child_right);

                            child_right_inter_set(std::get<1>(keys), key_new);

                            (..., ([&]() {if(other_affected_pointers == std::get<2>(keys).value())other_affected_pointers = key_new; }()));
                            return std::exchange(std::get<2>(keys).value(), key_new);
                        }
                        else std::unreachable();
                    }
                };
                struct bnode_up_t : bnode_t
                {
                    int child_index; // inserting, -3 / -1 / 1 / 3
                    static bnode_up_t get_bnode_from_key(navigator_t *key, bool child_is_left_or_right_child_of_key)
                    {
                        bnode_up_t bnode;
                        if (!key->color()) // black
                        {
                            if (key->*p_child_left == nullptr || tagged_ptr_bit0_is_setted(key->*p_child_left) || !(key->*p_child_left)->color()) std::get<0>(bnode.keys).reset();
                            else std::get<0>(bnode.keys) = key->*p_child_left;
                            std::get<1>(bnode.keys) = key;
                            if (key->*p_child_right == nullptr || tagged_ptr_bit0_is_setted(key->*p_child_right) || !(key->*p_child_right)->color()) std::get<2>(bnode.keys).reset();
                            else std::get<2>(bnode.keys) = key->*p_child_right;

                            bnode.child_index = !child_is_left_or_right_child_of_key ? -1 : 1;
                        }
                        else // red
                        {
                            typename navigator_t::template parent_info_t<is_reversed> key_red_parent_info(key);
                            navigator_t *key_black = std::get<1>(bnode.keys) = key_red_parent_info.parent;
                            if (!key_red_parent_info.is_left_or_right_child_of_parent)
                            {
                                std::get<0>(bnode.keys).emplace(key);
                                if (key_black->*p_child_right == nullptr || tagged_ptr_bit0_is_setted(key_black->*p_child_right) || !(key_black->*p_child_right)->color()) std::get<2>(bnode.keys).reset();
                                else std::get<2>(bnode.keys) = key_black->*p_child_right;

                                bnode.child_index = !child_is_left_or_right_child_of_key ? -3 : -1;
                            }
                            else
                            {
                                if (key_black->*p_child_left == nullptr || tagged_ptr_bit0_is_setted(key_black->*p_child_left) || !(key_black->*p_child_left)->color()) std::get<0>(bnode.keys).reset();
                                else std::get<0>(bnode.keys) = key_black->*p_child_left;
                                std::get<2>(bnode.keys).emplace(key);

                                bnode.child_index = !child_is_left_or_right_child_of_key ? 1 : 3;
                            }
                        }
                        bnode.center_key_parent_info = typename navigator_t::template parent_info_t<is_reversed>(std::get<1>(bnode.keys));
                        bnode.key_count = std::get<0>(bnode.keys).has_value() + 1 + std::get<2>(bnode.keys).has_value();
                        return bnode;
                    };
                    static std::tuple<bnode_up_t, navigator_t *> inserting_get_bnode_from_key_black_sibling(navigator_t *key_black_sibling, bool key_black_sibling_is_left_or_right_sibling)
                    {
                        assert(!key_black_sibling->color());

                        bnode_up_t bnode;

                        if (key_black_sibling->*p_child_left == nullptr || tagged_ptr_bit0_is_setted(key_black_sibling->*p_child_left) || !(key_black_sibling->*p_child_left)->color()) std::get<0>(bnode.keys).reset();
                        else std::get<0>(bnode.keys) = key_black_sibling->*p_child_left;
                        std::get<1>(bnode.keys) = key_black_sibling;
                        if (key_black_sibling->*p_child_right == nullptr || tagged_ptr_bit0_is_setted(key_black_sibling->*p_child_right) || !(key_black_sibling->*p_child_right)->color()) std::get<2>(bnode.keys).reset();
                        else std::get<2>(bnode.keys) = key_black_sibling->*p_child_right;

                        bnode.child_index = !key_black_sibling_is_left_or_right_sibling //
                            ? (std::get<2>(bnode.keys).has_value() ? 3 : 1) //
                            : (std::get<0>(bnode.keys).has_value() ? -3 : -1);
                        bnode.center_key_parent_info = typename navigator_t::template parent_info_t<is_reversed>(std::get<1>(bnode.keys));
                        bnode.key_count = std::get<0>(bnode.keys).has_value() + 1 + std::get<2>(bnode.keys).has_value();

                        navigator_t *bnode_rightmost_or_leftmost_child = !key_black_sibling_is_left_or_right_sibling //
                            ? (std::get<2>(bnode.keys).has_value() ? std::get<2>(bnode.keys).value()->*p_child_right : std::get<1>(bnode.keys)->*p_child_right) //
                            : (std::get<0>(bnode.keys).has_value() ? std::get<0>(bnode.keys).value()->*p_child_left : std::get<1>(bnode.keys)->*p_child_left);
                        return std::make_tuple(bnode, bnode_rightmost_or_leftmost_child);
                    };
                };
                struct bnode_erase_t : bnode_t
                {
                    int key_to_be_erased_index; // erasing, -2 / 0 / 2
                    static bnode_erase_t erasing_get_bnode(navigator_t *node_key_to_be_erased)
                    {
                        bnode_erase_t bnode;
                        if (!node_key_to_be_erased->color()) // black
                        {
                            if (node_key_to_be_erased->*p_child_left == nullptr || tagged_ptr_bit0_is_setted(node_key_to_be_erased->*p_child_left) || !(node_key_to_be_erased->*p_child_left)->color()) std::get<0>(bnode.keys).reset();
                            else std::get<0>(bnode.keys) = node_key_to_be_erased->*p_child_left;
                            std::get<1>(bnode.keys) = node_key_to_be_erased;
                            if (node_key_to_be_erased->*p_child_right == nullptr || tagged_ptr_bit0_is_setted(node_key_to_be_erased->*p_child_right) || !(node_key_to_be_erased->*p_child_right)->color()) std::get<2>(bnode.keys).reset();
                            else std::get<2>(bnode.keys) = node_key_to_be_erased->*p_child_right;

                            bnode.key_to_be_erased_index = 0;
                        }
                        else // red
                        {
                            typename navigator_t::template parent_info_t<is_reversed> key_red_parent_info(node_key_to_be_erased);
                            navigator_t *key_black = std::get<1>(bnode.keys) = key_red_parent_info.parent;
                            if (!key_red_parent_info.is_left_or_right_child_of_parent)
                            {
                                std::get<0>(bnode.keys).emplace(node_key_to_be_erased);
                                if (key_black->*p_child_right == nullptr || tagged_ptr_bit0_is_setted(key_black->*p_child_right) || !(key_black->*p_child_right)->color()) std::get<2>(bnode.keys).reset();
                                else std::get<2>(bnode.keys) = key_black->*p_child_right;

                                bnode.key_to_be_erased_index = -2;
                            }
                            else
                            {
                                if (key_black->*p_child_left == nullptr || tagged_ptr_bit0_is_setted(key_black->*p_child_left) || !(key_black->*p_child_left)->color()) std::get<0>(bnode.keys).reset();
                                else std::get<0>(bnode.keys) = key_black->*p_child_left;
                                std::get<2>(bnode.keys).emplace(node_key_to_be_erased);

                                bnode.key_to_be_erased_index = 2;
                            }
                        }
                        bnode.center_key_parent_info = typename navigator_t::template parent_info_t<is_reversed>(std::get<1>(bnode.keys));
                        bnode.key_count = std::get<0>(bnode.keys).has_value() + 1 + std::get<2>(bnode.keys).has_value();
                        return bnode;
                    }
                    static std::tuple<bnode_erase_t, navigator_t *, navigator_t *> erasing_get_bnode_from_key_black_sibling(navigator_t *key_black_sibling, bool key_black_sibling_is_left_or_right_sibling)
                    {
                        assert(!key_black_sibling->color());

                        bnode_erase_t bnode;

                        if (key_black_sibling->*p_child_left == nullptr || tagged_ptr_bit0_is_setted(key_black_sibling->*p_child_left) || !(key_black_sibling->*p_child_left)->color()) std::get<0>(bnode.keys).reset();
                        else std::get<0>(bnode.keys) = key_black_sibling->*p_child_left;
                        std::get<1>(bnode.keys) = key_black_sibling;
                        if (key_black_sibling->*p_child_right == nullptr || tagged_ptr_bit0_is_setted(key_black_sibling->*p_child_right) || !(key_black_sibling->*p_child_right)->color()) std::get<2>(bnode.keys).reset();
                        else std::get<2>(bnode.keys) = key_black_sibling->*p_child_right;

                        bnode.key_to_be_erased_index = !key_black_sibling_is_left_or_right_sibling //
                            ? (std::get<2>(bnode.keys).has_value() ? 2 : 0) //
                            : (std::get<0>(bnode.keys).has_value() ? -2 : 0);
                        bnode.center_key_parent_info = typename navigator_t::template parent_info_t<is_reversed>(std::get<1>(bnode.keys));
                        bnode.key_count = std::get<0>(bnode.keys).has_value() + 1 + std::get<2>(bnode.keys).has_value();

                        navigator_t *bnode_rightmost_or_leftmost_child = !key_black_sibling_is_left_or_right_sibling //
                            ? (std::get<2>(bnode.keys).has_value() ? std::get<2>(bnode.keys).value()->*p_child_right : std::get<1>(bnode.keys)->*p_child_right) //
                            : (std::get<0>(bnode.keys).has_value() ? std::get<0>(bnode.keys).value()->*p_child_left : std::get<1>(bnode.keys)->*p_child_left);
                        navigator_t *bnode_second_rightmost_or_second_leftmost_child = !key_black_sibling_is_left_or_right_sibling //
                            ? (std::get<2>(bnode.keys).has_value() ? std::get<2>(bnode.keys).value()->*p_child_left : (!std::get<0>(bnode.keys).has_value() ? std::get<1>(bnode.keys)->*p_child_left : std::get<0>(bnode.keys).value()->*p_child_right)) //
                            : (std::get<0>(bnode.keys).has_value() ? std::get<0>(bnode.keys).value()->*p_child_right : (!std::get<2>(bnode.keys).has_value() ? std::get<1>(bnode.keys)->*p_child_right : std::get<2>(bnode.keys).value()->*p_child_left));
                        return std::make_tuple(bnode, bnode_rightmost_or_leftmost_child, bnode_second_rightmost_or_second_leftmost_child);
                    }
                };

                static bool erase(schedules_t &schedules, node_end_t *node_end, navigator_t *const node)
                {
                    accumulator_t const &accumulator = tagged_ptr_bit0_unsetted(node_end)->accumulator;
                    assert(node != nullptr);
                    assert(node != node_end); // node_end
                    [[maybe_unused]] bool is_empty = empty(node_end);
                    assert(!is_empty);
                    bool will_be_empty = one_provided_not_empty(node_end);
                    if constexpr (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::no_size)
                        ;
                    else if constexpr (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_node_end)
                        --tagged_ptr_bit0_unsetted(node_end)->node_count;
                    else if constexpr (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_each_node_except_node_end)
                        ;
                    else
                        std::unreachable();
                    bool height_changed;
                    if (will_be_empty) // --count==0
                    {
                        tagged_ptr_bit0_unsetted(node_end)->parent() = tagged_ptr_bit0_unsetted(node_end)->*p_child_left = tagged_ptr_bit0_unsetted(node_end)->*p_child_right = node_end;
                        height_changed = true;
                    }
                    else // --count!=0
                    {
                        bnode_erase_t bnode = bnode_erase_t::erasing_get_bnode(node);
                        bnode_erase_t bnode_to_have_key_erased;
                        navigator_t *child_after_merge;

                        bool front_will_be_erased = false, back_will_be_erased = false;
                        navigator_t *front_new, *back_new;
                        if (tagged_ptr_bit0_is_setted(node->*p_child_left))
                        {
                            front_will_be_erased = true;
                            front_new = rb3p_iterator_t<false, is_reversed, config_t>::template predecessor<true>(node);
                        }
                        if (tagged_ptr_bit0_is_setted(node->*p_child_right))
                        {
                            back_will_be_erased = true;
                            back_new = rb3p_iterator_t<false, is_reversed, config_t>::template predecessor<false>(node);
                        }

                        if ((!node->color() && ((node->*p_child_left == nullptr || tagged_ptr_bit0_is_setted(node->*p_child_left)) || ((node->*p_child_left)->color() && (node->*p_child_left->*p_child_left == nullptr || tagged_ptr_bit0_is_setted(node->*p_child_left->*p_child_left))))) ||
                            (node->color() && (node->*p_child_left == nullptr || tagged_ptr_bit0_is_setted(node->*p_child_left)))) // leaf bnode
                        {
                            bnode_to_have_key_erased = bnode;
                            child_after_merge = nullptr;
                        }
                        else // not leaf bnode
                        {
                            navigator_t *node_current = node->*p_child_left;
                            while (node_current->*p_child_right != nullptr) // find rightmost descendent of left tree
                                node_current = node_current->*p_child_right;
                            if (tagged_ptr_bit0_is_setted(node_current->*p_child_left))
                            {
                                front_will_be_erased = true;
                                front_new = node_current;
                            }
                            bnode_to_have_key_erased = bnode_erase_t::erasing_get_bnode(node_current);
                            child_after_merge = nullptr;
                            parent_inter_set(nullptr, typename navigator_t::template parent_info_t<is_reversed>(node_current)), void(),
                                bnode.exchange_key_at(bnode.key_to_be_erased_index, node_current, bnode_to_have_key_erased.center_key_parent_info.parent);
                        }
                        while (true)
                        {
                            if (bnode_to_have_key_erased.key_count != 1)
                            {
                                if (bnode_to_have_key_erased.key_to_be_erased_index == -2)
                                {
                                    child_left_inter_set(std::get<1>(bnode_to_have_key_erased.keys), child_after_merge);

                                    refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<1>(bnode_to_have_key_erased.keys));
                                }
                                else if (bnode_to_have_key_erased.key_to_be_erased_index == 0)
                                {
                                    if (std::get<0>(bnode_to_have_key_erased.keys).has_value() && std::get<2>(bnode_to_have_key_erased.keys).has_value())
                                    {
                                        child_left_inter_set(std::get<2>(bnode_to_have_key_erased.keys).value(), child_after_merge);

                                        std::get<0>(bnode_to_have_key_erased.keys).value()->color() = false;
                                        parent_inter_set(std::get<0>(bnode_to_have_key_erased.keys).value(), bnode_to_have_key_erased.center_key_parent_info);
                                        child_right_inter_set(std::get<0>(bnode_to_have_key_erased.keys).value(), std::get<2>(bnode_to_have_key_erased.keys).value());

                                        refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<2>(bnode_to_have_key_erased.keys).value());
                                    }
                                    else if (std::get<0>(bnode_to_have_key_erased.keys).has_value())
                                    {
                                        std::get<0>(bnode_to_have_key_erased.keys).value()->color() = false;
                                        parent_inter_set(std::get<0>(bnode_to_have_key_erased.keys).value(), bnode_to_have_key_erased.center_key_parent_info);
                                        child_right_inter_set(std::get<0>(bnode_to_have_key_erased.keys).value(), child_after_merge);

                                        refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<0>(bnode_to_have_key_erased.keys).value());
                                    }
                                    else if (std::get<2>(bnode_to_have_key_erased.keys).has_value())
                                    {
                                        std::get<2>(bnode_to_have_key_erased.keys).value()->color() = false;
                                        parent_inter_set(std::get<2>(bnode_to_have_key_erased.keys).value(), bnode_to_have_key_erased.center_key_parent_info);
                                        child_left_inter_set(std::get<2>(bnode_to_have_key_erased.keys).value(), child_after_merge);

                                        refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<2>(bnode_to_have_key_erased.keys).value());
                                    }
                                    else std::unreachable();
                                }
                                else if (bnode_to_have_key_erased.key_to_be_erased_index == 2)
                                {
                                    child_right_inter_set(std::get<1>(bnode_to_have_key_erased.keys), child_after_merge);

                                    refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<1>(bnode_to_have_key_erased.keys));
                                }
                                else std::unreachable();
                                height_changed = false;
                                break;
                            }
                            else
                            {
                                if (bnode_to_have_key_erased.center_key_parent_info.is_end())
                                {
                                    parent_inter_set(child_after_merge, bnode_to_have_key_erased.center_key_parent_info);
                                    height_changed = true;
                                    break;
                                }
                                else
                                {
                                    bnode_up_t bnode_parent = bnode_up_t::get_bnode_from_key(bnode_to_have_key_erased.center_key_parent_info.parent, bnode_to_have_key_erased.center_key_parent_info.is_left_or_right_child_of_parent);

                                    std::optional<navigator_t *> opt_node_left_sibling_black;
                                    bnode_erase_t bnode_left_sibling;
                                    navigator_t *bnode_left_sibling_rightmost_child, *bnode_left_sibling_second_rightmost_child;

                                    std::optional<navigator_t *> opt_node_right_sibling_black;
                                    bnode_erase_t bnode_right_sibling;
                                    navigator_t *bnode_right_sibling_leftmost_child, *bnode_right_sibling_second_leftmost_child;

                                    auto try_grab_from_left_sibling = [&schedules, &bnode_to_have_key_erased, &child_after_merge, &bnode_parent, &opt_node_left_sibling_black, &bnode_left_sibling, &bnode_left_sibling_rightmost_child, &bnode_left_sibling_second_rightmost_child, &accumulator](auto fallback) //
                                    {
                                        return [fallback, &schedules, &bnode_to_have_key_erased, &child_after_merge, &bnode_parent, &opt_node_left_sibling_black, &bnode_left_sibling, &bnode_left_sibling_rightmost_child, &bnode_left_sibling_second_rightmost_child, &accumulator]() //
                                        {
                                            if (opt_node_left_sibling_black = bnode_parent.opt_child_at(bnode_parent.child_index - 2); opt_node_left_sibling_black.has_value())
                                            {
                                                std::tie(bnode_left_sibling, bnode_left_sibling_rightmost_child, bnode_left_sibling_second_rightmost_child) = bnode_erase_t::erasing_get_bnode_from_key_black_sibling(opt_node_left_sibling_black.value(), false);
                                                if (bnode_left_sibling.key_count != 1)
                                                {
                                                    navigator_t *key_from_parent = (parent_inter_set(nullptr, typename navigator_t::template parent_info_t<is_reversed>(bnode_left_sibling.key_at(bnode_left_sibling.key_to_be_erased_index))), void(), bnode_parent.exchange_key_at(bnode_parent.child_index - 1, bnode_left_sibling.key_at(bnode_left_sibling.key_to_be_erased_index), bnode_to_have_key_erased.center_key_parent_info.parent, bnode_left_sibling.center_key_parent_info.parent));

                                                    key_from_parent->color() = false;
                                                    parent_inter_set(key_from_parent, bnode_to_have_key_erased.center_key_parent_info);
                                                    child_left_inter_set(key_from_parent, bnode_left_sibling_rightmost_child);
                                                    child_right_inter_set(key_from_parent, child_after_merge);

                                                    bnode_to_have_key_erased = bnode_left_sibling;
                                                    child_after_merge = bnode_left_sibling_second_rightmost_child;

                                                    refresh_node_count_and_accumulated_storage_and_above_until(schedules, accumulator, key_from_parent, bnode_left_sibling.key_at(bnode_left_sibling.key_to_be_erased_index));
                                                }
                                                else
                                                    fallback();
                                            }
                                            else
                                                fallback();
                                        };
                                    };
                                    auto try_grab_from_right_sibling = [&schedules, &bnode_to_have_key_erased, &child_after_merge, &bnode_parent, &opt_node_right_sibling_black, &bnode_right_sibling, &bnode_right_sibling_leftmost_child, &bnode_right_sibling_second_leftmost_child, &accumulator](auto fallback) //
                                    {
                                        return [fallback, &schedules, &bnode_to_have_key_erased, &child_after_merge, &bnode_parent, &opt_node_right_sibling_black, &bnode_right_sibling, &bnode_right_sibling_leftmost_child, &bnode_right_sibling_second_leftmost_child, &accumulator]() //
                                        {
                                            if (opt_node_right_sibling_black = bnode_parent.opt_child_at(bnode_parent.child_index + 2); opt_node_right_sibling_black.has_value())
                                            {
                                                std::tie(bnode_right_sibling, bnode_right_sibling_leftmost_child, bnode_right_sibling_second_leftmost_child) = bnode_erase_t::erasing_get_bnode_from_key_black_sibling(opt_node_right_sibling_black.value(), true);
                                                if (bnode_right_sibling.key_count != 1)
                                                {
                                                    navigator_t *key_from_parent = (parent_inter_set(nullptr, typename navigator_t::template parent_info_t<is_reversed>(bnode_right_sibling.key_at(bnode_right_sibling.key_to_be_erased_index))), void(), bnode_parent.exchange_key_at(bnode_parent.child_index + 1, bnode_right_sibling.key_at(bnode_right_sibling.key_to_be_erased_index), bnode_to_have_key_erased.center_key_parent_info.parent, bnode_right_sibling.center_key_parent_info.parent));

                                                    key_from_parent->color() = false;
                                                    parent_inter_set(key_from_parent, bnode_to_have_key_erased.center_key_parent_info);
                                                    child_right_inter_set(key_from_parent, bnode_right_sibling_leftmost_child);
                                                    child_left_inter_set(key_from_parent, child_after_merge);

                                                    bnode_to_have_key_erased = bnode_right_sibling;
                                                    child_after_merge = bnode_right_sibling_second_leftmost_child;

                                                    refresh_node_count_and_accumulated_storage_and_above_until(schedules, accumulator, key_from_parent, bnode_right_sibling.key_at(bnode_right_sibling.key_to_be_erased_index));
                                                }
                                                else
                                                    fallback();
                                            }
                                            else
                                                fallback();
                                        };
                                    };
                                    auto try_merge_with_left_sibling = [&schedules, &bnode_to_have_key_erased, &child_after_merge, &bnode_parent, &opt_node_left_sibling_black, &accumulator](auto fallback) //
                                    {
                                        return [fallback, &schedules, &bnode_to_have_key_erased, &child_after_merge, &bnode_parent, &opt_node_left_sibling_black, &accumulator]() //
                                        {
                                            if (opt_node_left_sibling_black.has_value())
                                            {
                                                opt_node_left_sibling_black.value()->color() = true;

                                                navigator_t *key_from_parent = bnode_parent.key_at(bnode_parent.child_index - 1);
                                                key_from_parent->color() = false;
                                                child_left_inter_set(key_from_parent, opt_node_left_sibling_black.value());
                                                child_right_inter_set(key_from_parent, child_after_merge);

                                                static_cast<bnode_t &>(bnode_to_have_key_erased) = static_cast<bnode_t &>(bnode_parent);
                                                bnode_to_have_key_erased.key_to_be_erased_index = bnode_parent.child_index - 1;
                                                child_after_merge = key_from_parent;

                                                refresh_node_count_and_accumulated_storage(schedules, accumulator, key_from_parent);
                                            }
                                            else
                                                fallback();
                                        };
                                    };
                                    auto try_merge_with_right_sibling = [&schedules, &bnode_to_have_key_erased, &child_after_merge, &bnode_parent, &opt_node_right_sibling_black, &accumulator](auto fallback) //
                                    {
                                        return [fallback, &schedules, &bnode_to_have_key_erased, &child_after_merge, &bnode_parent, &opt_node_right_sibling_black, &accumulator]() //
                                        {
                                            if (opt_node_right_sibling_black.has_value())
                                            {
                                                opt_node_right_sibling_black.value()->color() = true;

                                                navigator_t *key_from_parent = bnode_parent.key_at(bnode_parent.child_index + 1);
                                                key_from_parent->color() = false;
                                                child_right_inter_set(key_from_parent, opt_node_right_sibling_black.value());
                                                child_left_inter_set(key_from_parent, child_after_merge);

                                                static_cast<bnode_t &>(bnode_to_have_key_erased) = static_cast<bnode_t &>(bnode_parent);
                                                bnode_to_have_key_erased.key_to_be_erased_index = bnode_parent.child_index + 1;
                                                child_after_merge = key_from_parent;

                                                refresh_node_count_and_accumulated_storage(schedules, accumulator, key_from_parent);
                                            }
                                            else
                                                fallback();
                                        };
                                    };
                                    try_grab_from_left_sibling(try_grab_from_right_sibling(try_merge_with_left_sibling(try_merge_with_right_sibling([]() { std::unreachable(); }))))();
                                }
                            }
                        }
                        if (front_will_be_erased)
                            tagged_ptr_bit0_unsetted(node_end)->*p_child_right = tagged_ptr_bit0_setted(front_new), front_new->*p_child_left = node_end;
                        if (back_will_be_erased)
                            tagged_ptr_bit0_unsetted(node_end)->*p_child_left = tagged_ptr_bit0_setted(back_new), back_new->*p_child_right = node_end;
                    }
                    return height_changed;
                }

                static bool insert_impl(schedules_t &schedules, node_end_t *node_end, bnode_up_t bnode_before_split, navigator_t *child_left_after_split, navigator_t *child_after_split, navigator_t *child_right_after_split)
                {
                    accumulator_t const &accumulator = tagged_ptr_bit0_unsetted(node_end)->accumulator;
                    bool height_changed;
                    while (true)
                    {
                        if (bnode_before_split.key_count != 3)
                        {
                            if (bnode_before_split.child_index == -3)
                            {
                                child_after_split->color() = true;
                                child_left_inter_set(child_after_split, child_left_after_split);
                                child_right_inter_set(child_after_split, child_right_after_split);

                                std::get<1>(bnode_before_split.keys)->color() = true;
                                child_left_inter_set(std::get<1>(bnode_before_split.keys), std::get<0>(bnode_before_split.keys).value()->*p_child_right);

                                std::get<0>(bnode_before_split.keys).value()->color() = false;
                                parent_inter_set(std::get<0>(bnode_before_split.keys).value(), bnode_before_split.center_key_parent_info);
                                child_left_inter_set(std::get<0>(bnode_before_split.keys).value(), child_after_split);
                                child_right_inter_set(std::get<0>(bnode_before_split.keys).value(), std::get<1>(bnode_before_split.keys));

                                refresh_node_count_and_accumulated_storage(schedules, accumulator, child_after_split);
                                refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, std::get<0>(bnode_before_split.keys).value());
                            }
                            else if (bnode_before_split.child_index == -1)
                            {
                                if (!std::get<0>(bnode_before_split.keys).has_value())
                                {
                                    child_after_split->color() = true;
                                    child_left_inter_set(child_after_split, child_left_after_split);
                                    child_right_inter_set(child_after_split, child_right_after_split);

                                    child_left_inter_set(std::get<1>(bnode_before_split.keys), child_after_split);

                                    refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, child_after_split);
                                }
                                else
                                {
                                    child_right_inter_set(std::get<0>(bnode_before_split.keys).value(), child_left_after_split);

                                    std::get<1>(bnode_before_split.keys)->color() = true;
                                    child_left_inter_set(std::get<1>(bnode_before_split.keys), child_right_after_split);

                                    child_after_split->color() = false;
                                    parent_inter_set(child_after_split, bnode_before_split.center_key_parent_info);
                                    child_left_inter_set(child_after_split, std::get<0>(bnode_before_split.keys).value());
                                    child_right_inter_set(child_after_split, std::get<1>(bnode_before_split.keys));

                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<0>(bnode_before_split.keys).value());
                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                    refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, child_after_split);
                                }
                            }
                            else if (bnode_before_split.child_index == 1)
                            {
                                if (!std::get<2>(bnode_before_split.keys).has_value())
                                {
                                    child_after_split->color() = true;
                                    child_right_inter_set(child_after_split, child_right_after_split);
                                    child_left_inter_set(child_after_split, child_left_after_split);

                                    child_right_inter_set(std::get<1>(bnode_before_split.keys), child_after_split);

                                    refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, child_after_split);
                                }
                                else
                                {
                                    child_left_inter_set(std::get<2>(bnode_before_split.keys).value(), child_right_after_split);

                                    std::get<1>(bnode_before_split.keys)->color() = true;
                                    child_right_inter_set(std::get<1>(bnode_before_split.keys), child_left_after_split);

                                    child_after_split->color() = false;
                                    parent_inter_set(child_after_split, bnode_before_split.center_key_parent_info);
                                    child_right_inter_set(child_after_split, std::get<2>(bnode_before_split.keys).value());
                                    child_left_inter_set(child_after_split, std::get<1>(bnode_before_split.keys));

                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<2>(bnode_before_split.keys).value());
                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                    refresh_node_count_and_accumulated_storage_and_above(schedules, accumulator, child_after_split);
                                }
                            }
                            else if (bnode_before_split.child_index == 3)
                            {
                                child_after_split->color() = true;
                                child_right_inter_set(child_after_split, child_right_after_split);
                                child_left_inter_set(child_after_split, child_left_after_split);

                                std::get<1>(bnode_before_split.keys)->color() = true;
                                child_right_inter_set(std::get<1>(bnode_before_split.keys), std::get<2>(bnode_before_split.keys).value()->*p_child_left);

                                std::get<2>(bnode_before_split.keys).value()->color() = false;
                                parent_inter_set(std::get<2>(bnode_before_split.keys).value(), bnode_before_split.center_key_parent_info);
                                child_right_inter_set(std::get<2>(bnode_before_split.keys).value(), child_after_split);
                                child_left_inter_set(std::get<2>(bnode_before_split.keys).value(), std::get<1>(bnode_before_split.keys));

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
                            if (bnode_before_split.center_key_parent_info.is_end())
                            {
                                [[maybe_unused]] auto parent_inter_set_faster = [&](navigator_t *node_root_new) //
                                {
                                    node_root_new->parent() = node_end;
                                    tagged_ptr_bit0_unsetted(node_end)->parent() = tagged_ptr_bit0_setted(node_root_new);
                                };
                                if (bnode_before_split.child_index == -3)
                                {
                                    child_after_split->color() = false;
                                    child_left_inter_set(child_after_split, child_left_after_split);
                                    child_right_inter_set(child_after_split, child_right_after_split);

                                    child_left_inter_set(std::get<1>(bnode_before_split.keys), std::get<0>(bnode_before_split.keys).value()->*p_child_right);

                                    std::get<0>(bnode_before_split.keys).value()->color() = false;
                                    parent_inter_set(std::get<0>(bnode_before_split.keys).value(), bnode_before_split.center_key_parent_info);
                                    child_left_inter_set(std::get<0>(bnode_before_split.keys).value(), child_after_split);
                                    child_right_inter_set(std::get<0>(bnode_before_split.keys).value(), std::get<1>(bnode_before_split.keys));

                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, child_after_split);
                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<0>(bnode_before_split.keys).value());
                                }
                                else if (bnode_before_split.child_index == -1)
                                {
                                    std::get<0>(bnode_before_split.keys).value()->color() = false;
                                    child_right_inter_set(std::get<0>(bnode_before_split.keys).value(), child_left_after_split);

                                    child_left_inter_set(std::get<1>(bnode_before_split.keys), child_right_after_split);

                                    child_after_split->color() = false;
                                    parent_inter_set(child_after_split, bnode_before_split.center_key_parent_info);
                                    child_left_inter_set(child_after_split, std::get<0>(bnode_before_split.keys).value());
                                    child_right_inter_set(child_after_split, std::get<1>(bnode_before_split.keys));

                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<0>(bnode_before_split.keys).value());
                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, child_after_split);
                                }
                                else if (bnode_before_split.child_index == 1)
                                {
                                    std::get<2>(bnode_before_split.keys).value()->color() = false;
                                    child_left_inter_set(std::get<2>(bnode_before_split.keys).value(), child_right_after_split);

                                    child_right_inter_set(std::get<1>(bnode_before_split.keys), child_left_after_split);

                                    child_after_split->color() = false;
                                    parent_inter_set(child_after_split, bnode_before_split.center_key_parent_info);
                                    child_right_inter_set(child_after_split, std::get<2>(bnode_before_split.keys).value());
                                    child_left_inter_set(child_after_split, std::get<1>(bnode_before_split.keys));

                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<2>(bnode_before_split.keys).value());
                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, child_after_split);
                                }
                                else if (bnode_before_split.child_index == 3)
                                {
                                    child_after_split->color() = false;
                                    child_right_inter_set(child_after_split, child_right_after_split);
                                    child_left_inter_set(child_after_split, child_left_after_split);

                                    child_right_inter_set(std::get<1>(bnode_before_split.keys), std::get<2>(bnode_before_split.keys).value()->*p_child_left);

                                    std::get<2>(bnode_before_split.keys).value()->color() = false;
                                    parent_inter_set(std::get<2>(bnode_before_split.keys).value(), bnode_before_split.center_key_parent_info);
                                    child_right_inter_set(std::get<2>(bnode_before_split.keys).value(), child_after_split);
                                    child_left_inter_set(std::get<2>(bnode_before_split.keys).value(), std::get<1>(bnode_before_split.keys));

                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, child_after_split);
                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<2>(bnode_before_split.keys).value());
                                }
                                else std::unreachable();
                                height_changed = true;
                                break;
                            }
                            else
                            {
                                navigator_t *child_after_split_old = child_after_split;

                                bnode_up_t bnode_parent = bnode_up_t::get_bnode_from_key(bnode_before_split.center_key_parent_info.parent, bnode_before_split.center_key_parent_info.is_left_or_right_child_of_parent);
                                auto try_spill_to_left_sibling = [&](auto fallback) //
                                {
                                    return [&, fallback]() //
                                    {
                                        if (std::optional<navigator_t *> opt_key_black_left_sibling = bnode_parent.opt_child_at(bnode_parent.child_index - 2); opt_key_black_left_sibling.has_value())
                                        {
                                            auto [bnode_left_sibling, bnode_left_sibling_rightmost_child] = bnode_up_t::inserting_get_bnode_from_key_black_sibling(opt_key_black_left_sibling.value(), false);
                                            if (bnode_left_sibling.key_count != 3)
                                            {
                                                if (bnode_before_split.child_index == -3)
                                                {
                                                    child_left_inter_set(std::get<0>(bnode_before_split.keys).value(), child_right_after_split);

                                                    child_right_after_split = child_left_after_split;
                                                    child_after_split = bnode_parent.exchange_key_at(bnode_parent.child_index - 1, child_after_split, bnode_left_sibling.center_key_parent_info.parent);

                                                    refresh_node_count_and_accumulated_storage_and_above_until(schedules, accumulator, std::get<0>(bnode_before_split.keys).value(), child_after_split_old);
                                                }
                                                else if (bnode_before_split.child_index == -1)
                                                {
                                                    child_after_split->color() = true;
                                                    child_left_inter_set(child_after_split, child_left_after_split);
                                                    child_right_inter_set(child_after_split, child_right_after_split);

                                                    child_left_inter_set(std::get<1>(bnode_before_split.keys), child_after_split);

                                                    child_right_after_split = std::get<0>(bnode_before_split.keys).value()->*p_child_left;
                                                    child_after_split = bnode_parent.exchange_key_at(bnode_parent.child_index - 1, std::get<0>(bnode_before_split.keys).value(), bnode_left_sibling.center_key_parent_info.parent);

                                                    refresh_node_count_and_accumulated_storage_and_above_until(schedules, accumulator, child_after_split_old, std::get<0>(bnode_before_split.keys).value());
                                                }
                                                else if (bnode_before_split.child_index == 1)
                                                {
                                                    child_left_inter_set(std::get<2>(bnode_before_split.keys).value(), child_right_after_split);

                                                    std::get<1>(bnode_before_split.keys)->color() = true;
                                                    child_left_inter_set(std::get<1>(bnode_before_split.keys), std::get<0>(bnode_before_split.keys).value()->*p_child_right);
                                                    child_right_inter_set(std::get<1>(bnode_before_split.keys), child_left_after_split);

                                                    child_after_split->color() = false;
                                                    parent_inter_set(child_after_split, bnode_before_split.center_key_parent_info);
                                                    child_left_inter_set(child_after_split, std::get<1>(bnode_before_split.keys));
                                                    child_right_inter_set(child_after_split, std::get<2>(bnode_before_split.keys).value());

                                                    child_right_after_split = std::get<0>(bnode_before_split.keys).value()->*p_child_left;
                                                    child_after_split = bnode_parent.exchange_key_at(bnode_parent.child_index - 1, std::get<0>(bnode_before_split.keys).value(), bnode_left_sibling.center_key_parent_info.parent);

                                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<2>(bnode_before_split.keys).value());
                                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                                    refresh_node_count_and_accumulated_storage_and_above_until(schedules, accumulator, child_after_split_old, std::get<0>(bnode_before_split.keys).value());
                                                }
                                                else if (bnode_before_split.child_index == 3)
                                                {
                                                    child_after_split->color() = true;
                                                    child_left_inter_set(child_after_split, child_left_after_split);
                                                    child_right_inter_set(child_after_split, child_right_after_split);

                                                    std::get<1>(bnode_before_split.keys)->color() = true;
                                                    child_left_inter_set(std::get<1>(bnode_before_split.keys), std::get<0>(bnode_before_split.keys).value()->*p_child_right);
                                                    child_right_inter_set(std::get<1>(bnode_before_split.keys), std::get<2>(bnode_before_split.keys).value()->*p_child_left);

                                                    std::get<2>(bnode_before_split.keys).value()->color() = false;
                                                    parent_inter_set(std::get<2>(bnode_before_split.keys).value(), bnode_before_split.center_key_parent_info);
                                                    child_left_inter_set(std::get<2>(bnode_before_split.keys).value(), std::get<1>(bnode_before_split.keys));
                                                    child_right_inter_set(std::get<2>(bnode_before_split.keys).value(), child_after_split);

                                                    child_right_after_split = std::get<0>(bnode_before_split.keys).value()->*p_child_left;
                                                    child_after_split = bnode_parent.exchange_key_at(bnode_parent.child_index - 1, std::get<0>(bnode_before_split.keys).value(), bnode_left_sibling.center_key_parent_info.parent);

                                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, child_after_split_old);
                                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                                    refresh_node_count_and_accumulated_storage_and_above_until(schedules, accumulator, std::get<2>(bnode_before_split.keys).value(), std::get<0>(bnode_before_split.keys).value());
                                                }
                                                else std::unreachable();
                                                bnode_before_split = bnode_left_sibling;
                                                child_left_after_split = bnode_left_sibling_rightmost_child;
                                            }
                                            else
                                                fallback();
                                        }
                                        else
                                            fallback();
                                    };
                                };
                                auto try_spill_to_right_sibling = [&](auto fallback) //
                                {
                                    return [&, fallback]() //
                                    {
                                        if (std::optional<navigator_t *> opt_key_black_right_sibling = bnode_parent.opt_child_at(bnode_parent.child_index + 2); opt_key_black_right_sibling.has_value())
                                        {
                                            auto [bnode_right_sibling, bnode_right_sibling_leftmost_child] = bnode_up_t::inserting_get_bnode_from_key_black_sibling(opt_key_black_right_sibling.value(), true);
                                            if (bnode_right_sibling.key_count != 3)
                                            {
                                                if (bnode_before_split.child_index == 3)
                                                {
                                                    child_right_inter_set(std::get<2>(bnode_before_split.keys).value(), child_left_after_split);

                                                    child_left_after_split = child_right_after_split;
                                                    child_after_split = bnode_parent.exchange_key_at(bnode_parent.child_index + 1, child_after_split, bnode_right_sibling.center_key_parent_info.parent);

                                                    refresh_node_count_and_accumulated_storage_and_above_until(schedules, accumulator, std::get<2>(bnode_before_split.keys).value(), child_after_split_old);
                                                }
                                                else if (bnode_before_split.child_index == 1)
                                                {
                                                    child_after_split->color() = true;
                                                    child_left_inter_set(child_after_split, child_left_after_split);
                                                    child_right_inter_set(child_after_split, child_right_after_split);

                                                    child_right_inter_set(std::get<1>(bnode_before_split.keys), child_after_split);

                                                    child_left_after_split = std::get<2>(bnode_before_split.keys).value()->*p_child_right;
                                                    child_after_split = bnode_parent.exchange_key_at(bnode_parent.child_index + 1, std::get<2>(bnode_before_split.keys).value(), bnode_right_sibling.center_key_parent_info.parent);

                                                    refresh_node_count_and_accumulated_storage_and_above_until(schedules, accumulator, child_after_split_old, std::get<2>(bnode_before_split.keys).value());
                                                }
                                                else if (bnode_before_split.child_index == -1)
                                                {
                                                    child_right_inter_set(std::get<0>(bnode_before_split.keys).value(), child_left_after_split);

                                                    std::get<1>(bnode_before_split.keys)->color() = true;
                                                    child_right_inter_set(std::get<1>(bnode_before_split.keys), std::get<2>(bnode_before_split.keys).value()->*p_child_left);
                                                    child_left_inter_set(std::get<1>(bnode_before_split.keys), child_right_after_split);

                                                    child_after_split->color() = false;
                                                    parent_inter_set(child_after_split, bnode_before_split.center_key_parent_info);
                                                    child_right_inter_set(child_after_split, std::get<1>(bnode_before_split.keys));
                                                    child_left_inter_set(child_after_split, std::get<0>(bnode_before_split.keys).value());

                                                    child_left_after_split = std::get<2>(bnode_before_split.keys).value()->*p_child_right;
                                                    child_after_split = bnode_parent.exchange_key_at(bnode_parent.child_index + 1, std::get<2>(bnode_before_split.keys).value(), bnode_right_sibling.center_key_parent_info.parent);

                                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<0>(bnode_before_split.keys).value());
                                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                                    refresh_node_count_and_accumulated_storage_and_above_until(schedules, accumulator, child_after_split_old, std::get<2>(bnode_before_split.keys).value());
                                                }
                                                else if (bnode_before_split.child_index == -3)
                                                {
                                                    child_after_split->color() = true;
                                                    child_left_inter_set(child_after_split, child_left_after_split);
                                                    child_right_inter_set(child_after_split, child_right_after_split);

                                                    std::get<1>(bnode_before_split.keys)->color() = true;
                                                    child_right_inter_set(std::get<1>(bnode_before_split.keys), std::get<2>(bnode_before_split.keys).value()->*p_child_left);
                                                    child_left_inter_set(std::get<1>(bnode_before_split.keys), std::get<0>(bnode_before_split.keys).value()->*p_child_right);

                                                    std::get<0>(bnode_before_split.keys).value()->color() = false;
                                                    parent_inter_set(std::get<0>(bnode_before_split.keys).value(), bnode_before_split.center_key_parent_info);
                                                    child_right_inter_set(std::get<0>(bnode_before_split.keys).value(), std::get<1>(bnode_before_split.keys));
                                                    child_left_inter_set(std::get<0>(bnode_before_split.keys).value(), child_after_split);

                                                    child_left_after_split = std::get<2>(bnode_before_split.keys).value()->*p_child_right;
                                                    child_after_split = bnode_parent.exchange_key_at(bnode_parent.child_index + 1, std::get<2>(bnode_before_split.keys).value(), bnode_right_sibling.center_key_parent_info.parent);

                                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, child_after_split_old);
                                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                                    refresh_node_count_and_accumulated_storage_and_above_until(schedules, accumulator, std::get<0>(bnode_before_split.keys).value(), std::get<2>(bnode_before_split.keys).value());
                                                }
                                                else std::unreachable();
                                                bnode_before_split = bnode_right_sibling;
                                                child_right_after_split = bnode_right_sibling_leftmost_child;
                                            }
                                            else
                                                fallback();
                                        }
                                        else
                                            fallback();
                                    };
                                };
                                auto spill_to_parent = [&]() //
                                {
                                    if (bnode_before_split.child_index == -3)
                                    {
                                        child_after_split->color() = false;
                                        child_left_inter_set(child_after_split, child_left_after_split);
                                        child_right_inter_set(child_after_split, child_right_after_split);

                                        child_left_inter_set(std::get<1>(bnode_before_split.keys), std::get<0>(bnode_before_split.keys).value()->*p_child_right);

                                        std::tie(child_left_after_split, child_after_split, child_right_after_split) = std::make_tuple(child_after_split, std::get<0>(bnode_before_split.keys).value(), std::get<1>(bnode_before_split.keys));

                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, child_after_split_old);
                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                    }
                                    else if (bnode_before_split.child_index == -1)
                                    {
                                        std::get<0>(bnode_before_split.keys).value()->color() = false;
                                        child_right_inter_set(std::get<0>(bnode_before_split.keys).value(), child_left_after_split);

                                        child_left_inter_set(std::get<1>(bnode_before_split.keys), child_right_after_split);

                                        std::tie(child_left_after_split, child_after_split, child_right_after_split) = std::make_tuple(std::get<0>(bnode_before_split.keys).value(), child_after_split, std::get<1>(bnode_before_split.keys));

                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<0>(bnode_before_split.keys).value());
                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                    }
                                    else if (bnode_before_split.child_index == 1)
                                    {
                                        std::get<2>(bnode_before_split.keys).value()->color() = false;
                                        child_left_inter_set(std::get<2>(bnode_before_split.keys).value(), child_right_after_split);

                                        child_right_inter_set(std::get<1>(bnode_before_split.keys), child_left_after_split);

                                        std::tie(child_right_after_split, child_after_split, child_left_after_split) = std::make_tuple(std::get<2>(bnode_before_split.keys).value(), child_after_split, std::get<1>(bnode_before_split.keys));

                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<2>(bnode_before_split.keys).value());
                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                    }
                                    else if (bnode_before_split.child_index == 3)
                                    {
                                        child_after_split->color() = false;
                                        child_right_inter_set(child_after_split, child_right_after_split);
                                        child_left_inter_set(child_after_split, child_left_after_split);

                                        child_right_inter_set(std::get<1>(bnode_before_split.keys), std::get<2>(bnode_before_split.keys).value()->*p_child_left);

                                        std::tie(child_right_after_split, child_after_split, child_left_after_split) = std::make_tuple(child_after_split, std::get<2>(bnode_before_split.keys).value(), std::get<1>(bnode_before_split.keys));

                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, child_after_split_old);
                                        refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_before_split.keys));
                                    }
                                    else std::unreachable();
                                    bnode_before_split = bnode_parent;
                                };
                                if (bnode_parent.child_index == -3 || bnode_parent.child_index == -1)
                                    try_spill_to_left_sibling(try_spill_to_right_sibling(spill_to_parent))();
                                else if (bnode_parent.child_index == 1 || bnode_parent.child_index == 3)
                                    try_spill_to_right_sibling(try_spill_to_left_sibling(spill_to_parent))();
                                else std::unreachable();
                            }
                        }
                    }
                    return height_changed;
                }

                static bool insert(schedules_t &schedules, node_end_t *node_end, navigator_t *const node, navigator_t *const node_new)
                {
                    accumulator_t const &accumulator = tagged_ptr_bit0_unsetted(node_end)->accumulator;
                    assert(node != nullptr);
                    assert(node_new != nullptr);
                    bool is_empty = empty(node_end);
                    if constexpr (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::no_size)
                        ;
                    else if constexpr (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_node_end)
                        ++tagged_ptr_bit0_unsetted(node_end)->node_count;
                    else if constexpr (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_each_node_except_node_end)
                        ;
                    else
                        std::unreachable();

                    bnode_up_t bnode_before_split;

                    navigator_t *child_after_split;
                    navigator_t *child_left_after_split, *child_right_after_split;

                    bool node_new_will_be_front = false, node_new_will_be_back = false;
                    auto change_front_to_node_new = [&]() { tagged_ptr_bit0_unsetted(node_end)->*p_child_right = tagged_ptr_bit0_setted(node_new), node_new->*p_child_left = node_end; };
                    auto change_back_to_node_new = [&]() { tagged_ptr_bit0_unsetted(node_end)->*p_child_left = tagged_ptr_bit0_setted(node_new), node_new->*p_child_right = node_end; };

                    bool height_changed;
                    if (node == node_end) // node_end
                    {
                        if (is_empty) // ++count==1
                        {
                            node_new->color() = false;
                            tagged_ptr_bit0_unsetted(node)->parent() = tagged_ptr_bit0_setted(node_new), node_new->parent() = node; // take care of node_end->parent

                            refresh_node_count_and_accumulated_storage(schedules, accumulator, node_new);

                            node_new_will_be_front = true;
                            node_new_will_be_back = true;
                            height_changed = true;
                            goto skip_insert_impl;
                        }
                        else // ++count!=1
                        {
                            bnode_before_split = bnode_up_t::get_bnode_from_key(tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node)->*p_child_left), true);
                            std::tie(child_left_after_split, child_after_split, child_right_after_split) = std::make_tuple(nullptr, node_new, nullptr);

                            node_new_will_be_back = true;
                        }
                    }
                    else if (node->*p_child_left == nullptr || tagged_ptr_bit0_is_setted(node->*p_child_left)) // doesn't have left tree (not root's leftmost descendent) / doesn't have left tree (root's leftmost descendent)
                    {
                        bnode_before_split = bnode_up_t::get_bnode_from_key(node, false);
                        std::tie(child_left_after_split, child_after_split, child_right_after_split) = std::make_tuple(nullptr, node_new, nullptr);
                        if (node->*p_child_left == nullptr) // doesn't have left tree (not root's leftmost descendent)
                            ;
                        else // doesn't have left tree (root's leftmost descendent)
                            node_new_will_be_front = true;
                    }
                    else // has left tree
                    {
                        navigator_t *node_current = node->*p_child_left;
                        while (node_current->*p_child_right != nullptr) // find rightmost descendent of left tree
                            node_current = node_current->*p_child_right;
                        bnode_before_split = bnode_up_t::get_bnode_from_key(node_current, true);
                        std::tie(child_left_after_split, child_after_split, child_right_after_split) = std::make_tuple(nullptr, node_new, nullptr);
                    }
                    height_changed = insert_impl(schedules, node_end, bnode_before_split, child_left_after_split, child_after_split, child_right_after_split);
skip_insert_impl:;
                    if (node_new_will_be_front)
                        change_front_to_node_new();
                    if (node_new_will_be_back)
                        change_back_to_node_new();
                    return height_changed;
                }

                static void swap_root(node_end_t *node_end_lhs, node_end_t *node_end_rhs)
                {
                    if constexpr (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::no_size)
                        ;
                    else if constexpr (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_node_end)
                        std::ranges::swap(tagged_ptr_bit0_unsetted(node_end_lhs)->node_count, tagged_ptr_bit0_unsetted(node_end_rhs)->node_count);
                    else if constexpr (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_each_node_except_node_end)
                        ;
                    else
                        std::unreachable();
                    assert((tagged_ptr_bit0_unsetted(node_end_lhs)->parent() == node_end_lhs) == (tagged_ptr_bit0_unsetted(node_end_lhs)->child_right() == node_end_lhs));
                    assert((tagged_ptr_bit0_unsetted(node_end_lhs)->parent() == node_end_lhs) == (tagged_ptr_bit0_unsetted(node_end_lhs)->child_left() == node_end_lhs));
                    assert((tagged_ptr_bit0_unsetted(node_end_rhs)->parent() == node_end_rhs) == (tagged_ptr_bit0_unsetted(node_end_rhs)->child_right() == node_end_rhs));
                    assert((tagged_ptr_bit0_unsetted(node_end_rhs)->parent() == node_end_rhs) == (tagged_ptr_bit0_unsetted(node_end_rhs)->child_left() == node_end_rhs));

                    if (tagged_ptr_bit0_unsetted(node_end_lhs)->parent() != node_end_lhs)
                    {
                        tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_lhs)->child_right())->child_left() = node_end_rhs;
                        tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_lhs)->parent())->parent() = node_end_rhs;
                        tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_lhs)->child_left())->child_right() = node_end_rhs;
                    }
                    if (tagged_ptr_bit0_unsetted(node_end_rhs)->parent() != node_end_rhs)
                    {
                        tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_rhs)->child_right())->child_left() = node_end_lhs;
                        tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_rhs)->parent())->parent() = node_end_lhs;
                        tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_rhs)->child_left())->child_right() = node_end_lhs;
                    }

                    if (tagged_ptr_bit0_unsetted(node_end_lhs)->parent() != node_end_lhs && tagged_ptr_bit0_unsetted(node_end_rhs)->parent() != node_end_rhs)
                    {
                        std::ranges::swap(tagged_ptr_bit0_unsetted(node_end_lhs)->child_right(), tagged_ptr_bit0_unsetted(node_end_rhs)->child_right());
                        std::ranges::swap(tagged_ptr_bit0_unsetted(node_end_lhs)->parent(), tagged_ptr_bit0_unsetted(node_end_rhs)->parent());
                        std::ranges::swap(tagged_ptr_bit0_unsetted(node_end_lhs)->child_left(), tagged_ptr_bit0_unsetted(node_end_rhs)->child_left());
                    }
                    else if (tagged_ptr_bit0_unsetted(node_end_lhs)->parent() != node_end_lhs)
                    {
                        tagged_ptr_bit0_unsetted(node_end_rhs)->child_right() = tagged_ptr_bit0_unsetted(node_end_lhs)->child_right();
                        tagged_ptr_bit0_unsetted(node_end_lhs)->child_right() = node_end_lhs;
                        tagged_ptr_bit0_unsetted(node_end_rhs)->parent() = tagged_ptr_bit0_unsetted(node_end_lhs)->parent();
                        tagged_ptr_bit0_unsetted(node_end_lhs)->parent() = node_end_lhs;
                        tagged_ptr_bit0_unsetted(node_end_rhs)->child_left() = tagged_ptr_bit0_unsetted(node_end_lhs)->child_left();
                        tagged_ptr_bit0_unsetted(node_end_lhs)->child_left() = node_end_lhs;
                    }
                    else if (tagged_ptr_bit0_unsetted(node_end_rhs)->parent() != node_end_rhs)
                    {
                        tagged_ptr_bit0_unsetted(node_end_lhs)->child_right() = tagged_ptr_bit0_unsetted(node_end_rhs)->child_right();
                        tagged_ptr_bit0_unsetted(node_end_rhs)->child_right() = node_end_rhs;
                        tagged_ptr_bit0_unsetted(node_end_lhs)->parent() = tagged_ptr_bit0_unsetted(node_end_rhs)->parent();
                        tagged_ptr_bit0_unsetted(node_end_rhs)->parent() = node_end_rhs;
                        tagged_ptr_bit0_unsetted(node_end_lhs)->child_left() = tagged_ptr_bit0_unsetted(node_end_rhs)->child_left();
                        tagged_ptr_bit0_unsetted(node_end_rhs)->child_left() = node_end_rhs;
                    }
                };

                template<bool goto_left_or_right, bool invoked_by_concat_or_split>
                static bool concat_with_middle_key(schedules_t &schedules, node_end_t *node_end_lhs, navigator_t *const node_middle_key, node_end_t *node_end_rhs, std::conditional_t<!invoked_by_concat_or_split, std::nullptr_t, int> height_difference)
                {
                    accumulator_t const &accumulator = !goto_left_or_right ? tagged_ptr_bit0_unsetted(node_end_lhs)->accumulator : tagged_ptr_bit0_unsetted(node_end_rhs)->accumulator;
                    bool height_changed;
                    bool is_empty_lhs = empty(node_end_lhs), is_empty_rhs = empty(node_end_rhs);
                    if (is_empty_lhs || is_empty_rhs)
                    {
                        if constexpr (!goto_left_or_right)
                        {
                            if (is_empty_rhs)
                            {
                                if constexpr (!invoked_by_concat_or_split)
                                    height_changed = insert(schedules, node_end_lhs, node_end_lhs, node_middle_key);
                                else
                                {
                                    if (is_empty_lhs)
                                        height_changed = insert(schedules, node_end_lhs, node_end_lhs, node_middle_key);
                                    else
                                    {
                                        navigator_t *rightmost_descendent_of_lhs = tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_lhs)->parent());
                                        while (rightmost_descendent_of_lhs->child_right() != nullptr)
                                            rightmost_descendent_of_lhs = rightmost_descendent_of_lhs->child_right();
                                        bnode_up_t bnode_before_split = bnode_up_t::get_bnode_from_key(rightmost_descendent_of_lhs, true);
                                        height_changed = insert_impl(schedules, node_end_lhs, bnode_before_split, nullptr, node_middle_key, nullptr);
                                    }
                                }
                            }
                            else if (is_empty_lhs)
                            {
                                if constexpr (!invoked_by_concat_or_split)
                                    height_changed = insert(schedules, node_end_rhs, tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_rhs)->child_right()), node_middle_key);
                                else
                                {
                                    navigator_t *leftmost_descendent_of_rhs = tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_rhs)->parent());
                                    while (leftmost_descendent_of_rhs->child_left() != nullptr)
                                        leftmost_descendent_of_rhs = leftmost_descendent_of_rhs->child_left();
                                    bnode_up_t bnode_before_split = bnode_up_t::get_bnode_from_key(leftmost_descendent_of_rhs, false);
                                    height_changed = insert_impl(schedules, node_end_rhs, bnode_before_split, nullptr, node_middle_key, nullptr);
                                }
                                swap_root(node_end_lhs, node_end_rhs);
                            }
                            else std::unreachable();
                        }
                        else
                        {
                            if (is_empty_lhs)
                            {
                                if constexpr (!invoked_by_concat_or_split)
                                    height_changed = insert(schedules, node_end_rhs, tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_rhs)->child_right()), node_middle_key);
                                else
                                {
                                    if (is_empty_rhs)
                                        height_changed = insert(schedules, node_end_rhs, node_end_rhs, node_middle_key);
                                    else
                                    {
                                        navigator_t *leftmost_descendent_of_rhs = tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_rhs)->parent());
                                        while (leftmost_descendent_of_rhs->child_left() != nullptr)
                                            leftmost_descendent_of_rhs = leftmost_descendent_of_rhs->child_left();
                                        bnode_up_t bnode_before_split = bnode_up_t::get_bnode_from_key(leftmost_descendent_of_rhs, false);
                                        height_changed = insert_impl(schedules, node_end_rhs, bnode_before_split, nullptr, node_middle_key, nullptr);
                                    }
                                }
                            }
                            else if (is_empty_rhs)
                            {
                                if constexpr (!invoked_by_concat_or_split)
                                    height_changed = insert(schedules, node_end_lhs, node_end_lhs, node_middle_key);
                                else
                                {
                                    navigator_t *rightmost_descendent_of_lhs = tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_lhs)->parent());
                                    while (rightmost_descendent_of_lhs->child_right() != nullptr)
                                        rightmost_descendent_of_lhs = rightmost_descendent_of_lhs->child_right();
                                    bnode_up_t bnode_before_split = bnode_up_t::get_bnode_from_key(rightmost_descendent_of_lhs, true);
                                    height_changed = insert_impl(schedules, node_end_lhs, bnode_before_split, nullptr, node_middle_key, nullptr);
                                }
                                swap_root(node_end_lhs, node_end_rhs);
                            }
                            else std::unreachable();
                        }
                    }
                    else
                    {
                        navigator_t *current_rightmost_black_descendent_of_lhs,
                            *current_leftmost_black_descendent_of_rhs;
                        if constexpr (!invoked_by_concat_or_split)
                        {
                            current_rightmost_black_descendent_of_lhs = tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_lhs)->child_left());
                            if (current_rightmost_black_descendent_of_lhs->color() == true)
                            {
                                current_rightmost_black_descendent_of_lhs = current_rightmost_black_descendent_of_lhs->parent();
                                assert(current_rightmost_black_descendent_of_lhs->color() == false);
                            }
                            current_leftmost_black_descendent_of_rhs = tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_rhs)->child_right());
                            if (current_leftmost_black_descendent_of_rhs->color() == true)
                            {
                                current_leftmost_black_descendent_of_rhs = current_leftmost_black_descendent_of_rhs->parent();
                                assert(current_leftmost_black_descendent_of_rhs->color() == false);
                            }
                            while (!tagged_ptr_bit0_is_setted(current_rightmost_black_descendent_of_lhs->parent()) && !tagged_ptr_bit0_is_setted(current_leftmost_black_descendent_of_rhs->parent()))
                            {
                                current_rightmost_black_descendent_of_lhs = current_rightmost_black_descendent_of_lhs->parent();
                                if (current_rightmost_black_descendent_of_lhs->color() == true)
                                {
                                    current_rightmost_black_descendent_of_lhs = current_rightmost_black_descendent_of_lhs->parent();
                                    assert(current_rightmost_black_descendent_of_lhs->color() == false);
                                }
                                current_leftmost_black_descendent_of_rhs = current_leftmost_black_descendent_of_rhs->parent();
                                if (current_leftmost_black_descendent_of_rhs->color() == true)
                                {
                                    current_leftmost_black_descendent_of_rhs = current_leftmost_black_descendent_of_rhs->parent();
                                    assert(current_leftmost_black_descendent_of_rhs->color() == false);
                                }
                            }
                        }
                        else
                        {
                            current_rightmost_black_descendent_of_lhs = tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_lhs)->parent());
                            current_leftmost_black_descendent_of_rhs = tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_rhs)->parent());
                            if (height_difference < 0)
                            {
                                for (int current_height_difference = 0; current_height_difference != -height_difference; ++current_height_difference)
                                {
                                    current_leftmost_black_descendent_of_rhs = current_leftmost_black_descendent_of_rhs->child_left();
                                    if (current_leftmost_black_descendent_of_rhs->color() == true)
                                    {
                                        current_leftmost_black_descendent_of_rhs = current_leftmost_black_descendent_of_rhs->child_left();
                                        assert(current_leftmost_black_descendent_of_rhs->color() == false);
                                    }
                                }
                            }
                            else if (height_difference > 0)
                            {
                                for (int current_height_difference = 0; current_height_difference != height_difference; ++current_height_difference)
                                {
                                    current_rightmost_black_descendent_of_lhs = current_rightmost_black_descendent_of_lhs->child_right();
                                    if (current_rightmost_black_descendent_of_lhs->color() == true)
                                    {
                                        current_rightmost_black_descendent_of_lhs = current_rightmost_black_descendent_of_lhs->child_right();
                                        assert(current_rightmost_black_descendent_of_lhs->color() == false);
                                    }
                                }
                            }
                        }
                        if constexpr (!invoked_by_concat_or_split)
                        {
                            tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_lhs)->child_left())->child_right() = nullptr;
                            tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_rhs)->child_right())->child_left() = nullptr;
                        }
                        if (tagged_ptr_bit0_is_setted(current_rightmost_black_descendent_of_lhs->parent()) && tagged_ptr_bit0_is_setted(current_leftmost_black_descendent_of_rhs->parent()))
                        {
                            node_middle_key->color() = false;
                            child_left_inter_set(node_middle_key, current_rightmost_black_descendent_of_lhs);
                            child_right_inter_set(node_middle_key, current_leftmost_black_descendent_of_rhs);
                            if constexpr (!goto_left_or_right)
                            {
                                tagged_ptr_bit0_unsetted(node_end_lhs)->parent() = tagged_ptr_bit0_setted(node_middle_key);
                                node_middle_key->parent() = node_end_lhs;

                                if constexpr (!invoked_by_concat_or_split)
                                {
                                    tagged_ptr_bit0_unsetted(node_end_lhs)->child_left() = tagged_ptr_bit0_unsetted(node_end_rhs)->child_left();
                                    tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_lhs)->child_left())->child_right() = node_end_lhs;
                                }

                                tagged_ptr_bit0_unsetted(node_end_rhs)->parent() = tagged_ptr_bit0_unsetted(node_end_rhs)->child_left() = tagged_ptr_bit0_unsetted(node_end_rhs)->child_right() = node_end_rhs;
                            }
                            else
                            {
                                tagged_ptr_bit0_unsetted(node_end_rhs)->parent() = tagged_ptr_bit0_setted(node_middle_key);
                                node_middle_key->parent() = node_end_rhs;

                                if constexpr (!invoked_by_concat_or_split)
                                {
                                    tagged_ptr_bit0_unsetted(node_end_rhs)->child_right() = tagged_ptr_bit0_unsetted(node_end_lhs)->child_right();
                                    tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_rhs)->child_right())->child_left() = node_end_rhs;
                                }

                                tagged_ptr_bit0_unsetted(node_end_lhs)->parent() = tagged_ptr_bit0_unsetted(node_end_lhs)->child_left() = tagged_ptr_bit0_unsetted(node_end_lhs)->child_right() = node_end_lhs;
                            }
                            refresh_node_count_and_accumulated_storage(schedules, accumulator, node_middle_key);
                            height_changed = true;
                        }
                        else if (tagged_ptr_bit0_is_setted(current_leftmost_black_descendent_of_rhs->parent()))
                        {
                            auto current_rightmost_black_descendent_of_lhs_center_key_parent_info = typename navigator_t::template parent_info_t<is_reversed>(current_rightmost_black_descendent_of_lhs);
                            if constexpr (!goto_left_or_right)
                            {
                                if constexpr (!invoked_by_concat_or_split)
                                {
                                    tagged_ptr_bit0_unsetted(node_end_lhs)->child_left() = tagged_ptr_bit0_unsetted(node_end_rhs)->child_left();
                                    tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_lhs)->child_left())->child_right() = node_end_lhs;
                                }

                                tagged_ptr_bit0_unsetted(node_end_rhs)->parent() = tagged_ptr_bit0_unsetted(node_end_rhs)->child_left() = tagged_ptr_bit0_unsetted(node_end_rhs)->child_right() = node_end_rhs;

                                bnode_up_t bnode_parent_of_current_rightmost_black_descendent_of_lhs = bnode_up_t::get_bnode_from_key(current_rightmost_black_descendent_of_lhs_center_key_parent_info.parent, current_rightmost_black_descendent_of_lhs_center_key_parent_info.is_left_or_right_child_of_parent);
                                height_changed = insert_impl(schedules, node_end_lhs, bnode_parent_of_current_rightmost_black_descendent_of_lhs, current_rightmost_black_descendent_of_lhs, node_middle_key, current_leftmost_black_descendent_of_rhs);
                            }
                            else
                            {
                                tagged_ptr_bit0_unsetted(node_end_rhs)->parent() = tagged_ptr_bit0_unsetted(node_end_lhs)->parent();
                                tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_rhs)->parent())->parent() = node_end_rhs;

                                if constexpr (!invoked_by_concat_or_split)
                                {
                                    tagged_ptr_bit0_unsetted(node_end_rhs)->child_right() = tagged_ptr_bit0_unsetted(node_end_lhs)->child_right();
                                    tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_rhs)->child_right())->child_left() = node_end_rhs;
                                }

                                tagged_ptr_bit0_unsetted(node_end_lhs)->parent() = tagged_ptr_bit0_unsetted(node_end_lhs)->child_left() = tagged_ptr_bit0_unsetted(node_end_lhs)->child_right() = node_end_lhs;

                                bnode_up_t bnode_parent_of_current_rightmost_black_descendent_of_lhs = bnode_up_t::get_bnode_from_key(current_rightmost_black_descendent_of_lhs_center_key_parent_info.parent, current_rightmost_black_descendent_of_lhs_center_key_parent_info.is_left_or_right_child_of_parent);
                                height_changed = insert_impl(schedules, node_end_rhs, bnode_parent_of_current_rightmost_black_descendent_of_lhs, current_rightmost_black_descendent_of_lhs, node_middle_key, current_leftmost_black_descendent_of_rhs);
                            }
                        }
                        else if (tagged_ptr_bit0_is_setted(current_rightmost_black_descendent_of_lhs->parent()))
                        {
                            auto current_leftmost_black_descendent_of_rhs_center_key_parent_info = typename navigator_t::template parent_info_t<is_reversed>(current_leftmost_black_descendent_of_rhs);
                            if constexpr (!goto_left_or_right)
                            {
                                tagged_ptr_bit0_unsetted(node_end_lhs)->parent() = tagged_ptr_bit0_unsetted(node_end_rhs)->parent();
                                tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_lhs)->parent())->parent() = node_end_lhs;

                                if constexpr (!invoked_by_concat_or_split)
                                {
                                    tagged_ptr_bit0_unsetted(node_end_lhs)->child_left() = tagged_ptr_bit0_unsetted(node_end_rhs)->child_left();
                                    tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_lhs)->child_left())->child_right() = node_end_lhs;
                                }

                                tagged_ptr_bit0_unsetted(node_end_rhs)->parent() = tagged_ptr_bit0_unsetted(node_end_rhs)->child_left() = tagged_ptr_bit0_unsetted(node_end_rhs)->child_right() = node_end_rhs;

                                bnode_up_t bnode_parent_of_current_leftmost_black_descendent_of_rhs = bnode_up_t::get_bnode_from_key(current_leftmost_black_descendent_of_rhs_center_key_parent_info.parent, current_leftmost_black_descendent_of_rhs_center_key_parent_info.is_left_or_right_child_of_parent);
                                height_changed = insert_impl(schedules, node_end_lhs, bnode_parent_of_current_leftmost_black_descendent_of_rhs, current_rightmost_black_descendent_of_lhs, node_middle_key, current_leftmost_black_descendent_of_rhs);
                            }
                            else
                            {
                                if constexpr (!invoked_by_concat_or_split)
                                {
                                    tagged_ptr_bit0_unsetted(node_end_rhs)->child_right() = tagged_ptr_bit0_unsetted(node_end_lhs)->child_right();
                                    tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_rhs)->child_right())->child_left() = node_end_rhs;
                                }

                                tagged_ptr_bit0_unsetted(node_end_lhs)->parent() = tagged_ptr_bit0_unsetted(node_end_lhs)->child_left() = tagged_ptr_bit0_unsetted(node_end_lhs)->child_right() = node_end_lhs;

                                bnode_up_t bnode_parent_of_current_leftmost_black_descendent_of_rhs = bnode_up_t::get_bnode_from_key(current_leftmost_black_descendent_of_rhs_center_key_parent_info.parent, current_leftmost_black_descendent_of_rhs_center_key_parent_info.is_left_or_right_child_of_parent);
                                height_changed = insert_impl(schedules, node_end_rhs, bnode_parent_of_current_leftmost_black_descendent_of_rhs, current_rightmost_black_descendent_of_lhs, node_middle_key, current_leftmost_black_descendent_of_rhs);
                            }
                        }
                        else std::unreachable();

                        if constexpr (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::no_size)
                            ;
                        else if constexpr (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_node_end)
                        {
                            if constexpr (!goto_left_or_right)
                                std::tie(tagged_ptr_bit0_unsetted(node_end_lhs)->node_count, tagged_ptr_bit0_unsetted(node_end_rhs)->node_count) = std::make_tuple(tagged_ptr_bit0_unsetted(node_end_lhs)->node_count + 1 + tagged_ptr_bit0_unsetted(node_end_rhs)->node_count, 0);
                            else
                                std::tie(tagged_ptr_bit0_unsetted(node_end_rhs)->node_count, tagged_ptr_bit0_unsetted(node_end_lhs)->node_count) = std::make_tuple(tagged_ptr_bit0_unsetted(node_end_rhs)->node_count + 1 + tagged_ptr_bit0_unsetted(node_end_lhs)->node_count, 0);
                        }
                        else if constexpr (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_each_node_except_node_end)
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
                    if constexpr (!goto_left_or_right)
                    {
                        if (is_empty_rhs)
                            ;
                        else if (is_empty_lhs)
                            swap_root(node_end_lhs, node_end_rhs);
                        else
                        {
                            navigator_t *node_middle_key = tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_rhs)->child_right());
                            erase(schedules, node_end_rhs, node_middle_key);
                            concat_with_middle_key<goto_left_or_right, false>(schedules, node_end_lhs, node_middle_key, node_end_rhs, nullptr);
                        }
                    }
                    else
                    {
                        if (is_empty_lhs)
                            ;
                        else if (is_empty_rhs)
                            swap_root(node_end_lhs, node_end_rhs);
                        else
                        {
                            navigator_t *node_middle_key = tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_lhs)->child_left());
                            erase(schedules, node_end_lhs, node_middle_key);
                            concat_with_middle_key<goto_left_or_right, false>(schedules, node_end_lhs, node_middle_key, node_end_rhs, nullptr);
                        }
                    }
                }

                template<bool emit_left_or_right>
                static void split(schedules_t &schedules, node_end_t *node_end_emit, node_end_t *node_end, navigator_t *const node)
                {
                    accumulator_t const &accumulator = tagged_ptr_bit0_unsetted(node_end)->accumulator;
                    assert(node != nullptr);
                    assert(empty(node_end_emit));
                    std::size_t size_sum;
                    if constexpr (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_node_end)
                        size_sum = tagged_ptr_bit0_unsetted(node_end)->node_count;
                    if (node == node_end)
                    {
                        if constexpr (!emit_left_or_right)
                            swap_root(node_end_emit, node_end);
                        else
                            ;
                    }
                    else if (node == tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end)->child_right()))
                    {
                        if constexpr (!emit_left_or_right)
                            ;
                        else
                            swap_root(node_end_emit, node_end);
                    }
                    else
                    {
                        navigator_t *node_predecessor = rb3p_iterator_t<false, is_reversed, config_t>::template predecessor<false>(node);
                        navigator_t *leftmost_descendent = tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end)->child_right());
                        leftmost_descendent->child_left() = nullptr;
                        navigator_t *rightmost_descendent = tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end)->child_left());
                        rightmost_descendent->child_right() = nullptr;

                        bnode_t bnode_to_be_splitted;
                        int split_position;
                        {
                            bnode_erase_t bnode_to_be_splitted_temp = bnode_erase_t::erasing_get_bnode(node);
                            bnode_to_be_splitted = bnode_to_be_splitted_temp;
                            split_position = bnode_to_be_splitted_temp.key_to_be_erased_index;
                        }
                        std::tuple<int, int> height_changed_s;
                        navigator_t *root_left = nullptr, *root_right = nullptr;
                        auto link_node_end_s = [&]() //
                        {
                            assert(root_left != nullptr);
                            assert(root_right != nullptr);

                            tagged_ptr_bit0_unsetted(node_end_emit)->child_right() = tagged_ptr_bit0_setted(leftmost_descendent);
                            tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_emit)->child_right())->child_left() = node_end_emit;

                            tagged_ptr_bit0_unsetted(node_end_emit)->parent() = tagged_ptr_bit0_setted(root_left);
                            tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_emit)->parent())->parent() = node_end_emit;

                            tagged_ptr_bit0_unsetted(node_end_emit)->child_left() = tagged_ptr_bit0_setted(node_predecessor);
                            tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end_emit)->child_left())->child_right() = node_end_emit;

                            tagged_ptr_bit0_unsetted(node_end)->child_right() = tagged_ptr_bit0_setted(node);
                            tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end)->child_right())->child_left() = node_end;

                            tagged_ptr_bit0_unsetted(node_end)->parent() = tagged_ptr_bit0_setted(root_right);
                            tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end)->parent())->parent() = node_end;

                            tagged_ptr_bit0_unsetted(node_end)->child_left() = tagged_ptr_bit0_setted(rightmost_descendent);
                            tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end)->child_left())->child_right() = node_end;
                        };
                        while (true)
                        {
                            bnode_up_t bnode_parent;
                            if (!bnode_to_be_splitted.center_key_parent_info.is_end())
                                bnode_parent = bnode_up_t::get_bnode_from_key(bnode_to_be_splitted.center_key_parent_info.parent, bnode_to_be_splitted.center_key_parent_info.is_left_or_right_child_of_parent);
                            if (split_position == -2)
                            {
                                child_left_inter_set(std::get<1>(bnode_to_be_splitted.keys), std::get<0>(bnode_to_be_splitted.keys).value()->child_right());
                                root_left = std::get<0>(bnode_to_be_splitted.keys).value()->child_left();
                                root_right = std::get<1>(bnode_to_be_splitted.keys);

                                node_end_t node_end_right;
                                node_end_right.parent() = tagged_ptr_bit0_setted(root_right);
                                tagged_ptr_bit0_unsetted(node_end_right.parent())->parent() = tagged_ptr_bit0_setted(&node_end_right);

                                navigator_t *leftmost_descendent_of_tree_right = root_right;
                                while (leftmost_descendent_of_tree_right->child_left() != nullptr)
                                    leftmost_descendent_of_tree_right = leftmost_descendent_of_tree_right->child_left();

                                bnode_up_t bnode_before_split = bnode_up_t::get_bnode_from_key(leftmost_descendent_of_tree_right, false);
                                std::get<0>(bnode_to_be_splitted.keys).value()->child_left() = std::get<0>(bnode_to_be_splitted.keys).value()->child_right() = nullptr;
                                bool height_changed = insert_impl(schedules, tagged_ptr_bit0_setted(&node_end_right), bnode_before_split, nullptr, std::get<0>(bnode_to_be_splitted.keys).value(), nullptr);
                                root_right = tagged_ptr_bit0_unsetted(node_end_right.parent());

                                if (bnode_to_be_splitted.center_key_parent_info.is_end())
                                {
                                    link_node_end_s();
                                    break;
                                }
                                else
                                {
                                    bnode_to_be_splitted = static_cast<bnode_t &>(bnode_parent);
                                    split_position = bnode_parent.child_index;
                                    height_changed_s = std::make_tuple(-1, (!height_changed ? 0 : 1));
                                }
                            }
                            else if (split_position == 0)
                            {
                                int height_left = !std::get<0>(bnode_to_be_splitted.keys).has_value() ? -1 : 0;
                                int height_right = !std::get<2>(bnode_to_be_splitted.keys).has_value() ? -1 : 0;

                                root_left = std::get<1>(bnode_to_be_splitted.keys)->child_left();
                                if (root_left != nullptr)
                                    root_left->color() = false;
                                root_right = std::get<1>(bnode_to_be_splitted.keys)->child_right();
                                if (root_right != nullptr)
                                    root_right->color() = false;

                                bool height_changed;
                                if (root_right == nullptr)
                                {
                                    std::get<1>(bnode_to_be_splitted.keys)->child_left() = std::get<1>(bnode_to_be_splitted.keys)->child_right() = nullptr;
                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_to_be_splitted.keys));
                                    root_right = std::get<1>(bnode_to_be_splitted.keys);
                                    height_changed = true;
                                }
                                else
                                {
                                    node_end_t node_end_right;
                                    node_end_right.parent() = tagged_ptr_bit0_setted(root_right);
                                    tagged_ptr_bit0_unsetted(node_end_right.parent())->parent() = tagged_ptr_bit0_setted(&node_end_right);

                                    navigator_t *leftmost_descendent_of_tree_right = root_right;
                                    while (leftmost_descendent_of_tree_right->child_left() != nullptr)
                                        leftmost_descendent_of_tree_right = leftmost_descendent_of_tree_right->child_left();

                                    bnode_up_t bnode_before_split = bnode_up_t::get_bnode_from_key(leftmost_descendent_of_tree_right, false);
                                    std::get<1>(bnode_to_be_splitted.keys)->child_left() = std::get<1>(bnode_to_be_splitted.keys)->child_right() = nullptr;
                                    height_changed = insert_impl(schedules, tagged_ptr_bit0_setted(&node_end_right), bnode_before_split, nullptr, std::get<1>(bnode_to_be_splitted.keys), nullptr);
                                    root_right = tagged_ptr_bit0_unsetted(node_end_right.parent());
                                }

                                if (bnode_to_be_splitted.center_key_parent_info.is_end())
                                {
                                    link_node_end_s();
                                    break;
                                }
                                else
                                {
                                    bnode_to_be_splitted = static_cast<bnode_t &>(bnode_parent);
                                    split_position = bnode_parent.child_index;
                                    height_changed_s = std::make_tuple(height_left, height_right + (!height_changed ? 0 : 1));
                                }
                            }
                            else if (split_position == 2)
                            {
                                child_right_inter_set(std::get<1>(bnode_to_be_splitted.keys), std::get<2>(bnode_to_be_splitted.keys).value()->child_left());
                                refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<1>(bnode_to_be_splitted.keys));
                                root_right = std::get<2>(bnode_to_be_splitted.keys).value()->child_right();
                                root_left = std::get<1>(bnode_to_be_splitted.keys);

                                bool height_changed;
                                if (root_right == nullptr)
                                {
                                    std::get<2>(bnode_to_be_splitted.keys).value()->color() = false;
                                    std::get<2>(bnode_to_be_splitted.keys).value()->child_left() = std::get<2>(bnode_to_be_splitted.keys).value()->child_right() = nullptr;
                                    refresh_node_count_and_accumulated_storage(schedules, accumulator, std::get<2>(bnode_to_be_splitted.keys).value());
                                    root_right = std::get<2>(bnode_to_be_splitted.keys).value();
                                    height_changed = true;
                                }
                                else
                                {
                                    node_end_t node_end_right;
                                    node_end_right.parent() = tagged_ptr_bit0_setted(root_right);
                                    tagged_ptr_bit0_unsetted(node_end_right.parent())->parent() = tagged_ptr_bit0_setted(&node_end_right);

                                    navigator_t *leftmost_descendent_of_tree_right = root_right;
                                    while (leftmost_descendent_of_tree_right->child_left() != nullptr)
                                        leftmost_descendent_of_tree_right = leftmost_descendent_of_tree_right->child_left();

                                    bnode_up_t bnode_before_split = bnode_up_t::get_bnode_from_key(leftmost_descendent_of_tree_right, false);
                                    std::get<2>(bnode_to_be_splitted.keys).value()->child_left() = std::get<2>(bnode_to_be_splitted.keys).value()->child_right() = nullptr;
                                    height_changed = insert_impl(schedules, tagged_ptr_bit0_setted(&node_end_right), bnode_before_split, nullptr, std::get<2>(bnode_to_be_splitted.keys).value(), nullptr);
                                    root_right = tagged_ptr_bit0_unsetted(node_end_right.parent());
                                }

                                if (bnode_to_be_splitted.center_key_parent_info.is_end())
                                {
                                    link_node_end_s();
                                    break;
                                }
                                else
                                {
                                    bnode_to_be_splitted = static_cast<bnode_t &>(bnode_parent);
                                    split_position = bnode_parent.child_index;
                                    height_changed_s = std::make_tuple(0, (-1) + (!height_changed ? 0 : 1));
                                }
                            }
                            else if (split_position == -3)
                            {
                                child_left_inter_set(std::get<1>(bnode_to_be_splitted.keys), std::get<0>(bnode_to_be_splitted.keys).value()->child_right());

                                node_end_t node_end_left;
                                if (root_right != nullptr)
                                {
                                    node_end_left.parent() = tagged_ptr_bit0_setted(root_right);
                                    tagged_ptr_bit0_unsetted(node_end_left.parent())->parent() = tagged_ptr_bit0_setted(&node_end_left);
                                }

                                node_end_t node_end_right;
                                node_end_right.parent() = tagged_ptr_bit0_setted(std::get<1>(bnode_to_be_splitted.keys));
                                tagged_ptr_bit0_unsetted(node_end_right.parent())->parent() = tagged_ptr_bit0_setted(&node_end_right);

                                std::get<0>(bnode_to_be_splitted.keys).value()->child_left() = std::get<0>(bnode_to_be_splitted.keys).value()->child_right() = nullptr;
                                bool height_changed = concat_with_middle_key<true, true>(schedules, tagged_ptr_bit0_setted(&node_end_left), std::get<0>(bnode_to_be_splitted.keys).value(), tagged_ptr_bit0_setted(&node_end_right), std::get<1>(height_changed_s) - 1);
                                root_right = tagged_ptr_bit0_unsetted(node_end_right.parent());

                                if (bnode_to_be_splitted.center_key_parent_info.is_end())
                                {
                                    link_node_end_s();
                                    break;
                                }
                                else
                                {
                                    bnode_to_be_splitted = static_cast<bnode_t &>(bnode_parent);
                                    split_position = bnode_parent.child_index;
                                    height_changed_s = std::make_tuple(std::get<0>(height_changed_s) - 1, (!height_changed ? 0 : 1));
                                }
                            }
                            else if (split_position == -1)
                            {
                                int height_right = !std::get<2>(bnode_to_be_splitted.keys).has_value() ? -1 : 0;
                                if (!std::get<0>(bnode_to_be_splitted.keys).has_value())
                                {
                                    node_end_t node_end_left;
                                    if (root_right != nullptr)
                                    {
                                        node_end_left.parent() = tagged_ptr_bit0_setted(root_right);
                                        tagged_ptr_bit0_unsetted(node_end_left.parent())->parent() = tagged_ptr_bit0_setted(&node_end_left);
                                    }

                                    node_end_t node_end_right;
                                    assert(std::get<1>(bnode_to_be_splitted.keys)->child_right() != nullptr);
                                    std::get<1>(bnode_to_be_splitted.keys)->child_right()->color() = false;
                                    node_end_right.parent() = tagged_ptr_bit0_setted(std::get<1>(bnode_to_be_splitted.keys)->child_right());
                                    tagged_ptr_bit0_unsetted(node_end_right.parent())->parent() = tagged_ptr_bit0_setted(&node_end_right);

                                    std::get<1>(bnode_to_be_splitted.keys)->child_left() = std::get<1>(bnode_to_be_splitted.keys)->child_right() = nullptr;
                                    bool height_changed = concat_with_middle_key<true, true>(schedules, tagged_ptr_bit0_setted(&node_end_left), std::get<1>(bnode_to_be_splitted.keys), tagged_ptr_bit0_setted(&node_end_right), (std::get<1>(height_changed_s) - 1) - height_right);
                                    root_right = tagged_ptr_bit0_unsetted(node_end_right.parent());

                                    if (bnode_to_be_splitted.center_key_parent_info.is_end())
                                    {
                                        link_node_end_s();
                                        break;
                                    }
                                    else
                                    {
                                        bnode_to_be_splitted = static_cast<bnode_t &>(bnode_parent);
                                        split_position = bnode_parent.child_index;
                                        height_changed_s = std::make_tuple(std::get<0>(height_changed_s) - 1, (!height_changed ? 0 : 1) + height_right);
                                    }
                                }
                                else
                                {
                                    node_end_t node_end_left;
                                    assert(std::get<0>(bnode_to_be_splitted.keys).value()->child_left() != nullptr);
                                    node_end_left.parent() = tagged_ptr_bit0_setted(std::get<0>(bnode_to_be_splitted.keys).value()->child_left());
                                    tagged_ptr_bit0_unsetted(node_end_left.parent())->parent() = tagged_ptr_bit0_setted(&node_end_left);

                                    node_end_t node_end_left_middle;
                                    if (root_left != nullptr)
                                    {
                                        node_end_left_middle.parent() = tagged_ptr_bit0_setted(root_left);
                                        tagged_ptr_bit0_unsetted(node_end_left_middle.parent())->parent() = tagged_ptr_bit0_setted(&node_end_left_middle);
                                    }

                                    node_end_t node_end_middle_right;
                                    if (root_right != nullptr)
                                    {
                                        node_end_middle_right.parent() = tagged_ptr_bit0_setted(root_right);
                                        tagged_ptr_bit0_unsetted(node_end_middle_right.parent())->parent() = tagged_ptr_bit0_setted(&node_end_middle_right);
                                    }

                                    node_end_t node_end_right;
                                    assert(std::get<1>(bnode_to_be_splitted.keys)->child_right() != nullptr);
                                    std::get<1>(bnode_to_be_splitted.keys)->child_right()->color() = false;
                                    node_end_right.parent() = tagged_ptr_bit0_setted(std::get<1>(bnode_to_be_splitted.keys)->child_right());
                                    tagged_ptr_bit0_unsetted(node_end_right.parent())->parent() = tagged_ptr_bit0_setted(&node_end_right);

                                    std::get<0>(bnode_to_be_splitted.keys).value()->child_left() = std::get<0>(bnode_to_be_splitted.keys).value()->child_right() = nullptr;
                                    bool height_changed_lhs = concat_with_middle_key<false, true>(schedules, tagged_ptr_bit0_setted(&node_end_left), std::get<0>(bnode_to_be_splitted.keys).value(), tagged_ptr_bit0_setted(&node_end_left_middle), -std::get<0>(height_changed_s));
                                    root_left = tagged_ptr_bit0_unsetted(node_end_left.parent());

                                    std::get<1>(bnode_to_be_splitted.keys)->child_left() = std::get<1>(bnode_to_be_splitted.keys)->child_right() = nullptr;
                                    bool height_changed_rhs = concat_with_middle_key<true, true>(schedules, tagged_ptr_bit0_setted(&node_end_middle_right), std::get<1>(bnode_to_be_splitted.keys), tagged_ptr_bit0_setted(&node_end_right), (std::get<1>(height_changed_s) - 1) - height_right);
                                    root_right = tagged_ptr_bit0_unsetted(node_end_right.parent());

                                    if (bnode_to_be_splitted.center_key_parent_info.is_end())
                                    {
                                        link_node_end_s();
                                        break;
                                    }
                                    else
                                    {
                                        bnode_to_be_splitted = static_cast<bnode_t &>(bnode_parent);
                                        split_position = bnode_parent.child_index;
                                        height_changed_s = std::make_tuple((!height_changed_lhs ? 0 : 1) - 1, (!height_changed_rhs ? 0 : 1) + height_right);
                                    }
                                }
                            }
                            else if (split_position == 1)
                            {
                                int height_left = !std::get<0>(bnode_to_be_splitted.keys).has_value() ? -1 : 0;
                                if (!std::get<2>(bnode_to_be_splitted.keys).has_value())
                                {
                                    node_end_t node_end_right;
                                    if (root_left != nullptr)
                                    {
                                        node_end_right.parent() = tagged_ptr_bit0_setted(root_left);
                                        tagged_ptr_bit0_unsetted(node_end_right.parent())->parent() = tagged_ptr_bit0_setted(&node_end_right);
                                    }

                                    node_end_t node_end_left;
                                    assert(std::get<1>(bnode_to_be_splitted.keys)->child_left() != nullptr);
                                    std::get<1>(bnode_to_be_splitted.keys)->child_left()->color() = false;
                                    node_end_left.parent() = tagged_ptr_bit0_setted(std::get<1>(bnode_to_be_splitted.keys)->child_left());
                                    tagged_ptr_bit0_unsetted(node_end_left.parent())->parent() = tagged_ptr_bit0_setted(&node_end_left);

                                    std::get<1>(bnode_to_be_splitted.keys)->child_left() = std::get<1>(bnode_to_be_splitted.keys)->child_right() = nullptr;
                                    bool height_changed = concat_with_middle_key<false, true>(schedules, tagged_ptr_bit0_setted(&node_end_left), std::get<1>(bnode_to_be_splitted.keys), tagged_ptr_bit0_setted(&node_end_right), height_left - (std::get<0>(height_changed_s) - 1));
                                    root_left = tagged_ptr_bit0_unsetted(node_end_left.parent());

                                    if (bnode_to_be_splitted.center_key_parent_info.is_end())
                                    {
                                        link_node_end_s();
                                        break;
                                    }
                                    else
                                    {
                                        bnode_to_be_splitted = static_cast<bnode_t &>(bnode_parent);
                                        split_position = bnode_parent.child_index;
                                        height_changed_s = std::make_tuple((!height_changed ? 0 : 1) + height_left, std::get<1>(height_changed_s) - 1);
                                    }
                                }
                                else
                                {
                                    node_end_t node_end_right;
                                    assert(std::get<2>(bnode_to_be_splitted.keys).value()->child_right() != nullptr);
                                    node_end_right.parent() = tagged_ptr_bit0_setted(std::get<2>(bnode_to_be_splitted.keys).value()->child_right());
                                    tagged_ptr_bit0_unsetted(node_end_right.parent())->parent() = tagged_ptr_bit0_setted(&node_end_right);

                                    node_end_t node_end_middle_right;
                                    if (root_right != nullptr)
                                    {
                                        node_end_middle_right.parent() = tagged_ptr_bit0_setted(root_right);
                                        tagged_ptr_bit0_unsetted(node_end_middle_right.parent())->parent() = tagged_ptr_bit0_setted(&node_end_middle_right);
                                    }

                                    node_end_t node_end_left_middle;
                                    if (root_left != nullptr)
                                    {
                                        node_end_left_middle.parent() = tagged_ptr_bit0_setted(root_left);
                                        tagged_ptr_bit0_unsetted(node_end_left_middle.parent())->parent() = tagged_ptr_bit0_setted(&node_end_left_middle);
                                    }

                                    node_end_t node_end_left;
                                    assert(std::get<1>(bnode_to_be_splitted.keys)->child_left() != nullptr);
                                    std::get<1>(bnode_to_be_splitted.keys)->child_left()->color() = false;
                                    node_end_left.parent() = tagged_ptr_bit0_setted(std::get<1>(bnode_to_be_splitted.keys)->child_left());
                                    tagged_ptr_bit0_unsetted(node_end_left.parent())->parent() = tagged_ptr_bit0_setted(&node_end_left);

                                    std::get<2>(bnode_to_be_splitted.keys).value()->child_left() = std::get<2>(bnode_to_be_splitted.keys).value()->child_right() = nullptr;
                                    bool height_changed_rhs = concat_with_middle_key<true, true>(schedules, tagged_ptr_bit0_setted(&node_end_middle_right), std::get<2>(bnode_to_be_splitted.keys).value(), tagged_ptr_bit0_setted(&node_end_right), std::get<1>(height_changed_s));
                                    root_right = tagged_ptr_bit0_unsetted(node_end_right.parent());

                                    std::get<1>(bnode_to_be_splitted.keys)->child_left() = std::get<1>(bnode_to_be_splitted.keys)->child_right() = nullptr;
                                    bool height_changed_lhs = concat_with_middle_key<false, true>(schedules, tagged_ptr_bit0_setted(&node_end_left), std::get<1>(bnode_to_be_splitted.keys), tagged_ptr_bit0_setted(&node_end_left_middle), height_left - (std::get<0>(height_changed_s) - 1));
                                    root_left = tagged_ptr_bit0_unsetted(node_end_left.parent());

                                    if (bnode_to_be_splitted.center_key_parent_info.is_end())
                                    {
                                        link_node_end_s();
                                        break;
                                    }
                                    else
                                    {
                                        bnode_to_be_splitted = static_cast<bnode_t &>(bnode_parent);
                                        split_position = bnode_parent.child_index;
                                        height_changed_s = std::make_tuple((!height_changed_lhs ? 0 : 1) + height_left, (!height_changed_rhs ? 0 : 1) - 1);
                                    }
                                }
                            }
                            else if (split_position == 3)
                            {
                                child_right_inter_set(std::get<1>(bnode_to_be_splitted.keys), std::get<2>(bnode_to_be_splitted.keys).value()->child_left());

                                node_end_t node_end_right;
                                if (root_left != nullptr)
                                {
                                    node_end_right.parent() = tagged_ptr_bit0_setted(root_left);
                                    tagged_ptr_bit0_unsetted(node_end_right.parent())->parent() = tagged_ptr_bit0_setted(&node_end_right);
                                }

                                node_end_t node_end_left;
                                node_end_left.parent() = tagged_ptr_bit0_setted(std::get<1>(bnode_to_be_splitted.keys));
                                tagged_ptr_bit0_unsetted(node_end_left.parent())->parent() = tagged_ptr_bit0_setted(&node_end_left);

                                std::get<2>(bnode_to_be_splitted.keys).value()->child_left() = std::get<2>(bnode_to_be_splitted.keys).value()->child_right() = nullptr;
                                bool height_changed = concat_with_middle_key<false, true>(schedules, tagged_ptr_bit0_setted(&node_end_left), std::get<2>(bnode_to_be_splitted.keys).value(), tagged_ptr_bit0_setted(&node_end_right), -(std::get<0>(height_changed_s) - 1));
                                root_left = tagged_ptr_bit0_unsetted(node_end_left.parent());

                                if (bnode_to_be_splitted.center_key_parent_info.is_end())
                                {
                                    link_node_end_s();
                                    break;
                                }
                                else
                                {
                                    bnode_to_be_splitted = static_cast<bnode_t &>(bnode_parent);
                                    split_position = bnode_parent.child_index;
                                    height_changed_s = std::make_tuple((!height_changed ? 0 : 1), std::get<1>(height_changed_s) - 1);
                                }
                            }
                            else std::unreachable();
                        }
                        if constexpr (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_node_end)
                        {
                            std::size_t size_emitted = std::ranges::distance(std::ranges::next(rb3p_iterator_t<false, false, config_t>(node_end_emit)), rb3p_iterator_t<false, false, config_t>(node_end_emit));
                            std::tie(tagged_ptr_bit0_unsetted(node_end_emit)->node_count, tagged_ptr_bit0_unsetted(node_end)->node_count) = std::make_tuple(size_emitted, size_sum - size_emitted);
                        }
                        if constexpr (!emit_left_or_right)
                            ;
                        else
                            swap_root(node_end_emit, node_end);
                    }
                }

                static accumulated_storage_t read_range_impl(allocator_element_t const &allocator_element, node_end_t *node_end, navigator_t *node_front, navigator_t *node_back)
                {
                    if (node_front == node_back)
                        return tagged_ptr_bit0_unsetted(node_end)->accumulator.construct_accumulated_storage(allocator_element, std::make_tuple(std::cref(*static_cast<node_t *>(node_front)->p_element())));

                    struct path_vertex_t
                    {
                        path_vertex_t *next;
                        bool child_left_or_child_right;
                        navigator_t *node;
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
                    auto build_path = [&accumulator = std::as_const(tagged_ptr_bit0_unsetted(node_end)->accumulator), &allocator_element](auto &this_, path_vertex_t *path_front, path_vertex_t *path_back) -> accumulated_storage_t //
                    {
                        typename navigator_t::template parent_info_t<is_reversed> parent_info_front(path_front->node), parent_info_back(path_back->node);
                        if (!parent_info_front.is_end() || !parent_info_back.is_end())
                        {
                            path_vertex_t path_vertex_front, path_vertex_back;
                            if (!parent_info_front.is_end())
                            {
                                path_vertex_front = {
                                    .next = path_front,
                                    .child_left_or_child_right = parent_info_front.is_left_or_right_child_of_parent,
                                    .node = parent_info_front.parent,
                                };
                                path_front = &path_vertex_front;
                            }
                            if (!parent_info_back.is_end())
                            {
                                path_vertex_back = {
                                    .next = path_back,
                                    .child_left_or_child_right = parent_info_back.is_left_or_right_child_of_parent,
                                    .node = parent_info_back.parent,
                                };
                                path_back = &path_vertex_back;
                            }
                            return this_(this_, path_front, path_back);
                        }
                        else
                        {
                            assert(path_front->node == path_back->node);
                            while (path_front->next != nullptr && path_back->next != nullptr && path_front->next->node == path_back->next->node)
                            {
                                path_front = path_front->next;
                                path_back = path_back->next;
                            }
                            auto accumulate_left = [&accumulator, &allocator_element](auto &this_, path_vertex_t *path_vertex_front) -> accumulated_storage_t //
                            {
                                if (path_vertex_front->next == nullptr)
                                {
                                    if (path_vertex_front->node->child_right() == nullptr)
                                        return accumulator.construct_accumulated_storage(allocator_element, std::make_tuple(std::cref(*static_cast<node_t *>(path_vertex_front->node)->p_element())));
                                    else
                                        return accumulator.construct_accumulated_storage(allocator_element, std::make_tuple(std::cref(*static_cast<node_t *>(path_vertex_front->node)->p_element()), std::ref(*static_cast<node_t *>(static_cast<navigator_t *>(path_vertex_front->node->child_right()))->p_accumulated_storage())));
                                }
                                else
                                {
                                    if (path_vertex_front->child_left_or_child_right == false)
                                    {
                                        accumulated_storage_t intermediate_accumulated_storage_left(this_(this_, path_vertex_front->next));
                                        if (path_vertex_front->node->child_right() == nullptr)
                                            return accumulator.construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(intermediate_accumulated_storage_left), std::cref(*static_cast<node_t *>(path_vertex_front->node)->p_element())));
                                        else
                                            return accumulator.construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(intermediate_accumulated_storage_left), std::cref(*static_cast<node_t *>(path_vertex_front->node)->p_element()), std::ref(*static_cast<node_t *>(static_cast<navigator_t *>(path_vertex_front->node->child_right()))->p_accumulated_storage())));
                                    }
                                    else
                                        return this_(this_, path_vertex_front->next);
                                }
                            };
                            auto accumulate_right = [&accumulator, &allocator_element](auto &this_, path_vertex_t *path_vertex_back) -> accumulated_storage_t //
                            {
                                if (path_vertex_back->next == nullptr)
                                {
                                    if (path_vertex_back->node->child_left() == nullptr)
                                        return accumulator.construct_accumulated_storage(allocator_element, std::make_tuple(std::cref(*static_cast<node_t *>(path_vertex_back->node)->p_element())));
                                    else
                                        return accumulator.construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(*static_cast<node_t *>(static_cast<navigator_t *>(path_vertex_back->node->child_left()))->p_accumulated_storage()), std::cref(*static_cast<node_t *>(path_vertex_back->node)->p_element())));
                                }
                                else
                                {
                                    if (path_vertex_back->child_left_or_child_right == true)
                                    {
                                        accumulated_storage_t intermediate_accumulated_storage_right(this_(this_, path_vertex_back->next));
                                        if (path_vertex_back->node->child_left() == nullptr)
                                            return accumulator.construct_accumulated_storage(allocator_element, std::make_tuple(std::cref(*static_cast<node_t *>(path_vertex_back->node)->p_element()), std::ref(intermediate_accumulated_storage_right)));
                                        else
                                            return accumulator.construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(*static_cast<node_t *>(static_cast<navigator_t *>(path_vertex_back->node->child_left()))->p_accumulated_storage()), std::cref(*static_cast<node_t *>(path_vertex_back->node)->p_element()), std::ref(intermediate_accumulated_storage_right)));
                                    }
                                    else
                                        return this_(this_, path_vertex_back->next);
                                }
                            };

                            auto get_left_operand = [&](auto return_accumulated_tuple) //
                            { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> accumulated_storage_t //
                              {
                                  if (path_front->next == nullptr)
                                      return return_accumulated_tuple(accumulated_tuple_so_far);
                                  else
                                  {
                                      accumulated_storage_t intermediate_accumulated_storage_left(accumulate_left(accumulate_left, path_front->next));
                                      return return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(intermediate_accumulated_storage_left))));
                                  }
                              }; };
                            auto get_middle_operand = [&](auto return_accumulated_tuple) { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> accumulated_storage_t { return return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::cref(*static_cast<node_t *>(path_front->node)->p_element())))); }; };
                            auto get_right_operand = [&](auto return_accumulated_tuple) //
                            { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> accumulated_storage_t //
                              {
                                  if (path_back->next == nullptr)
                                      return return_accumulated_tuple(accumulated_tuple_so_far);
                                  else
                                  {
                                      accumulated_storage_t intermediate_accumulated_storage_right(accumulate_right(accumulate_right, path_back->next));
                                      return return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(intermediate_accumulated_storage_right))));
                                  }
                              }; };
                            auto return_accumulated_tuple = [&](auto accumulated_tuple_so_far) -> accumulated_storage_t { return accumulator.construct_accumulated_storage(allocator_element, accumulated_tuple_so_far); };
                            return get_left_operand(get_middle_operand(get_right_operand(return_accumulated_tuple)))(std::make_tuple());
                        }
                    };
                    return build_path(build_path, path_front, path_back);
                }

                static void update_range_impl(node_end_t *node_end, navigator_t *node_front, navigator_t *node_back)
                {
                    accumulator_t const &accumulator = tagged_ptr_bit0_unsetted(node_end)->accumulator;
                    auto refresh_tree_and_find_back = [&accumulator, &node_back](auto &this_, navigator_t *root) -> bool //
                    {
                        bool found = false;
                        if (root->child_left() != nullptr)
                            found = found || this_(this_, root->child_left());
                        found = found || root == node_back;
                        if (!found && root->child_right() != nullptr)
                            found = found || this_(this_, root->child_right());
                        refresh_accumulated_storage(accumulator, root);
                        return found;
                    };
                    if (node_front == node_back)
                        refresh_accumulated_storage_and_above(accumulator, node_front);
                    else if (node_front->child_right() != nullptr && refresh_tree_and_find_back(refresh_tree_and_find_back, node_front->child_right()))
                        refresh_accumulated_storage_and_above(accumulator, node_front);
                    else
                    {
                        refresh_accumulated_storage(accumulator, node_front);
                        while (true)
                        {
                            typename navigator_t::template parent_info_t<is_reversed> parent_info(node_front);
                            assert(!parent_info.is_end());
                            node_front = parent_info.parent;
                            if (!parent_info.is_left_or_right_child_of_parent)
                            {
                                if (node_front == node_back)
                                {
                                    refresh_accumulated_storage_and_above(accumulator, node_front);
                                    break;
                                }
                                else if (node_front->child_right() != nullptr && refresh_tree_and_find_back(refresh_tree_and_find_back, node_front->child_right()))
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
                    if (empty(node_end))
                        return node_end;
                    navigator_t *root = tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end)->parent());
                    if (!monotonic_predicate(*static_cast<node_t *>(root)->p_accumulated_storage()))
                        return node_end;

                    accumulator_t const &accumulator = tagged_ptr_bit0_unsetted(node_end)->accumulator;
                    auto search_tree = [&accumulator, &allocator_element, &monotonic_predicate](auto &this_, auto accumulated_storage_tuple_so_far, navigator_t *node) -> navigator_t * //
                    {
                        if (node->child_left() == nullptr || tagged_ptr_bit0_is_setted(node->child_left()))
                        {
                            accumulated_storage_t intermediate_accumulated_storage(accumulator.construct_accumulated_storage(allocator_element, std::tuple_cat(accumulated_storage_tuple_so_far, std::make_tuple(std::cref(*static_cast<node_t *>(node)->p_element())))));
                            if (monotonic_predicate(intermediate_accumulated_storage))
                                return node;
                            else
                            {
                                assert(node->child_right() != nullptr && !tagged_ptr_bit0_is_setted(node->child_right()));
                                return this_(this_, std::make_tuple(std::ref(intermediate_accumulated_storage)), node->child_right());
                            }
                        }
                        else
                        {
                            accumulated_storage_t intermediate_accumulated_storage_left(accumulator.construct_accumulated_storage(allocator_element, std::tuple_cat(accumulated_storage_tuple_so_far, std::make_tuple(std::ref(*static_cast<node_t *>(static_cast<navigator_t *>(node->child_left()))->p_accumulated_storage())))));
                            if (monotonic_predicate(intermediate_accumulated_storage_left))
                                return this_(this_, accumulated_storage_tuple_so_far, node->child_left());
                            else
                            {
                                accumulated_storage_t intermediate_accumulated_storage_right(accumulator.construct_accumulated_storage(allocator_element, std::make_tuple(std::ref(intermediate_accumulated_storage_left), std::cref(*static_cast<node_t *>(node)->p_element()))));
                                if (monotonic_predicate(intermediate_accumulated_storage_right))
                                    return node;
                                else
                                {
                                    assert(node->child_right() != nullptr && !tagged_ptr_bit0_is_setted(node->child_right()));
                                    return this_(this_, std::make_tuple(std::ref(intermediate_accumulated_storage_right)), node->child_right());
                                }
                            }
                        }
                    };
                    return search_tree(search_tree, std::make_tuple(), root);
                }

                template</*std::output_iterator<navigator_t*>*/ typename iterator_output_pointer_node_t, typename heap_predicate_t> requires (invocable_r<heap_predicate_t, bool, accumulated_storage_t &> && invocable_r<heap_predicate_t, bool, element_t const &>)
                static void find_by_heap_predicate(node_end_t *node_end, iterator_output_pointer_node_t iterator_output_pointer_node, heap_predicate_t const &heap_predicate)
                {
                    if (empty(node_end))
                        return;
                    navigator_t *root = tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end)->parent());
                    auto search_tree = [&heap_predicate, &iterator_output_pointer_node](auto &this_, navigator_t *node) -> void //
                    {
                        if (node->child_left() != nullptr && !tagged_ptr_bit0_is_setted(node->child_left()))
                        {
                            if (heap_predicate(*static_cast<node_t *>(static_cast<navigator_t *>(node->child_left()))->p_accumulated_storage()))
                                this_(this_, node->child_left());
                        }
                        if (heap_predicate(std::as_const(*static_cast<node_t *>(node)->p_element())))
                            *iterator_output_pointer_node++ = node;
                        if (node->child_right() != nullptr && !tagged_ptr_bit0_is_setted(node->child_right()))
                        {
                            if (heap_predicate(*static_cast<node_t *>(static_cast<navigator_t *>(node->child_right()))->p_accumulated_storage()))
                                this_(this_, node->child_right());
                        }
                    };
                    if (heap_predicate(*static_cast<node_t *>(root)->p_accumulated_storage()))
                        search_tree(search_tree, root);
                }

                template<typename heap_predicate_t> requires (invocable_r<heap_predicate_t, bool, accumulated_storage_t &> && invocable_r<heap_predicate_t, bool, element_t const &>)
                static generator_t<navigator_t *> find_by_heap_predicate_generator(node_end_t *node_end, heap_predicate_t const &heap_predicate)
                {
                    if (empty(node_end))
                        co_return;
                    navigator_t *root = tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(node_end)->parent());
                    auto search_tree = [](heap_predicate_t const &heap_predicate, auto &this_, navigator_t *node) -> generator_t<navigator_t *> //
                    {
                        if (node->child_left() != nullptr && !tagged_ptr_bit0_is_setted(node->child_left()))
                        {
                            if (heap_predicate(*static_cast<node_t *>(static_cast<navigator_t *>(node->child_left()))->p_accumulated_storage()))
                                co_yield this_(heap_predicate, this_, node->child_left());
                        }
                        if (heap_predicate(std::as_const(*static_cast<node_t *>(node)->p_element())))
                            co_yield node;
                        if (node->child_right() != nullptr && !tagged_ptr_bit0_is_setted(node->child_right()))
                        {
                            if (heap_predicate(*static_cast<node_t *>(static_cast<navigator_t *>(node->child_right()))->p_accumulated_storage()))
                                co_yield this_(heap_predicate, this_, node->child_right());
                        }
                    };
                    if (heap_predicate(*static_cast<node_t *>(root)->p_accumulated_storage()))
                        co_yield search_tree(heap_predicate, search_tree, root);
                }
            };
        } // namespace augmented_sequence_rb3p
    } // namespace detail

#ifndef AUGMENTED_CONTAINERS_AUGMENTED_SEQUENCE_HELPERS
    #define AUGMENTED_CONTAINERS_AUGMENTED_SEQUENCE_HELPERS
    namespace augmented_sequence_helpers
    {
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
    requires (static_cast<augmented_sequence_physical_representation_e>(augmented_sequence_physical_representation_t_{}) == augmented_sequence_physical_representation_e::rb3p)
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

        using navigator_t = detail::augmented_sequence_rb3p::rb3p_node_navigator_t<allocator_element_t>;
        using node_t = detail::augmented_sequence_rb3p::rb3p_node_t<config_t>;
        using node_end_t = detail::augmented_sequence_rb3p::rb3p_node_end_t<config_t>;

        allocator_element_t allocator_element;
        node_end_t *node_end;

        using iterator_t = detail::augmented_sequence_rb3p::rb3p_iterator_t<false, false, config_t>;
        static_assert(std::input_or_output_iterator<iterator_t>);
        static_assert(std::input_iterator<iterator_t>);
        static_assert(std::sentinel_for<iterator_t, iterator_t>);
        static_assert(std::sentinel_for<std::default_sentinel_t, iterator_t>);
        static_assert(std::forward_iterator<iterator_t>);
        static_assert(std::bidirectional_iterator<iterator_t>);
        using const_iterator_t = detail::augmented_sequence_rb3p::rb3p_iterator_t<true, false, config_t>;
        static_assert(std::input_or_output_iterator<const_iterator_t>);
        static_assert(std::input_iterator<const_iterator_t>);
        static_assert(std::sentinel_for<const_iterator_t, const_iterator_t>);
        static_assert(std::sentinel_for<std::default_sentinel_t, const_iterator_t>);
        static_assert(std::forward_iterator<const_iterator_t>);
        static_assert(std::bidirectional_iterator<const_iterator_t>);

        iterator_t begin() { return {navigator_t::untagged_front_or_tagged_end(node_end)}; }
        iterator_t end() { return {node_end}; }
        const_iterator_t begin() const { return {navigator_t::untagged_front_or_tagged_end(node_end)}; }
        const_iterator_t end() const { return {node_end}; }
        const_iterator_t cbegin() const { return {navigator_t::untagged_front_or_tagged_end(node_end)}; }
        const_iterator_t cend() const { return {node_end}; }


        void create_end_node()
        {
            node_end = node_end_t::create_node_end();
        }
        void destroy_end_node()
        {
            delete detail::language::tagged_ptr_bit0_unsetted(node_end);
        }
        void swap_end_node(augmented_sequence_t &other)
        {
            std::ranges::swap(this->node_end, other.node_end);
        }

        augmented_sequence_t() /* allocator is default initialized */ { create_end_node(); } // default constructor
        void clear() &
        {
            auto erase_tree = [](auto &this_, navigator_t *node) -> void //
            {
                navigator_t *child_left = node->child_left();
                navigator_t *child_right = node->child_right();
                if (child_left != nullptr && !detail::language::tagged_ptr_bit0_is_setted(child_left))
                    this_(this_, child_left);
                if (child_right != nullptr && !detail::language::tagged_ptr_bit0_is_setted(child_right))
                    this_(this_, child_right);
                static_cast<node_t *>(node)->p_element()->~element_t();
                delete static_cast<node_t *>(node);
            };
            if (!detail::augmented_sequence_rb3p::rb3p_functor_t<false, config_t>::empty(node_end))
                erase_tree(erase_tree, static_cast<navigator_t *>(detail::language::tagged_ptr_bit0_unsetted(detail::language::tagged_ptr_bit0_unsetted(node_end)->parent())));

            if constexpr (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::no_size)
                ;
            else if constexpr (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_node_end)
                detail::language::tagged_ptr_bit0_unsetted(node_end)->node_count = 0;
            else if constexpr (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_each_node_except_node_end)
                ;
            else
                std::unreachable();
        }
        explicit augmented_sequence_t(allocator_element_t const &allocator_element) : allocator_element(allocator_element) // default constructor with allocator
        {
            create_end_node();
        }
        explicit augmented_sequence_t(size_type count, allocator_element_t const &allocator_element = allocator_element_t()) : augmented_sequence_t(allocator_element) // count default-inserted constructor (with allocator)?
        {
            std::ranges::for_each(std::views::iota(static_cast<size_type>(0), count), [this]([[maybe_unused]] size_type index) { this->emplace_back(); });
        }
        explicit augmented_sequence_t(size_type count, element_t const &value, allocator_element_t const &allocator_element = allocator_element_t()) : augmented_sequence_t(allocator_element) // count copy-inserted constructor (with allocator)?
        {
            std::ranges::for_each(std::views::iota(static_cast<size_type>(0), count), [this, &value]([[maybe_unused]] size_type index) { this->emplace_back(value); });
        }
        void assign(size_type count, element_t const &value) &
        {
            this->clear();
            std::ranges::for_each(std::views::iota(static_cast<size_type>(0), count), [this, &value]([[maybe_unused]] size_type index) { this->emplace_back(value); });
        }
        template<std::input_iterator iterator_t, std::sentinel_for<iterator_t> sentinel_t>
        augmented_sequence_t(iterator_t iterator, sentinel_t sentinel, allocator_element_t const &allocator_element = allocator_element_t()) : augmented_sequence_t(allocator_element) // comparable range constructor (with allocator)?
        {
            std::ranges::for_each(std::ranges::subrange(iterator, sentinel), [this]<typename other_element_t>(other_element_t &&other_element) { this->emplace_back(std::forward<other_element_t>(other_element)); });
        }
        template<std::input_iterator iterator_t, std::sentinel_for<iterator_t> sentinel_t>
        void assign(iterator_t iterator, sentinel_t sentinel) &
        {
            this->clear();
            std::ranges::for_each(std::ranges::subrange(iterator, sentinel), [this]<typename other_element_t>(other_element_t &&other_element) { this->emplace_back(std::forward<other_element_t>(other_element)); });
        }
        augmented_sequence_t(std::initializer_list<element_t> initializer_list, allocator_element_t const &allocator_element = allocator_element_t()) : augmented_sequence_t(allocator_element) // std::initializer_list constructor (with allocator)?
        {
            std::ranges::for_each(initializer_list, [this](element_t const &other_element) { this->emplace_back(other_element); });
        }
        augmented_sequence_t &operator=(std::initializer_list<element_t> initializer_list) & // std::initializer_list assignment operator
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
        augmented_sequence_t(augmented_sequence_t const &other, std::type_identity_t<allocator_element_t> const &allocator_element) : augmented_sequence_t(allocator_element) // copy constructor with allocator
        {
            std::ranges::for_each(std::ranges::subrange(other.cbegin(), other.cend()), [this](element_t const &other_element) { this->emplace_back(other_element); });
        }
        augmented_sequence_t(augmented_sequence_t const &other) : augmented_sequence_t(other, std::allocator_traits<allocator_type>::select_on_container_copy_construction(other.allocator_element)) {} // copy constructor
        augmented_sequence_t &operator=(augmented_sequence_t const &other) & // copy assignment operator
        {
            if (this == &other)
                return *this;
            this->clear();
            if (this->allocator_element != other.allocator_element)
            {
                if constexpr (std::allocator_traits<allocator_type>::propagate_on_container_copy_assignment::value)
                {
                    destroy_end_node();
                    this->allocator_element = other.allocator_element;
                    create_end_node();
                }
            }
            std::ranges::for_each(std::ranges::subrange(other.cbegin(), other.cend()), [this](element_t const &other_element) { this->emplace_back(other_element); });
            return *this;
        }

        augmented_sequence_t(augmented_sequence_t &&other) // move constructor
            : allocator_element(([&]() //
                                 {
                                     this->node_end=other.node_end;
                                     other.create_end_node(); }(),
                                 std::move(other.allocator_element)))
        {}
        augmented_sequence_t(augmented_sequence_t &&other, std::type_identity_t<allocator_element_t> const &allocator_element) : allocator_element(allocator_element) // move constructor with allocator
        {
            if (allocator_element == other.allocator_element)
            {
                this->node_end = other.node_end;
                other.create_end_node();
            }
            else
            {
                create_end_node();
                std::ranges::for_each(std::ranges::subrange(other.begin(), other.end()), [this](element_t &other_element) { this->emplace_back(std::move(other_element)); });
            }
        }
        augmented_sequence_t &operator=(augmented_sequence_t &&other) & // move assignment operator
        {
            if (this == &other)
                return *this;
            this->clear();
            if (this->allocator_element == other.allocator_element)
                swap_end_node(other);
            else
            {
                if constexpr (std::allocator_traits<allocator_type>::propagate_on_container_move_assignment::value)
                {
                    destroy_end_node();
                    this->allocator_element = std::move(other.allocator_element);
                    create_end_node();
                    swap_end_node(other);
                }
                else
                    std::ranges::for_each(std::ranges::subrange(other.begin(), other.end()), [this](element_t &other_element) { this->emplace_back(std::move(other_element)); });
            }
            return *this;
        }
        void swap(augmented_sequence_t &other)
        {
            if (this->allocator_element == other.allocator_element)
                swap_end_node(other);
            else
            {
                if constexpr (std::allocator_traits<allocator_type>::propagate_on_container_swap::value)
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
            return detail::augmented_sequence_rb3p::rb3p_functor_t<false, config_t>::empty(node_end);
        }
        size_type size() const requires (static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_node_end || static_cast<augmented_sequence_size_management_e>(typename config_t::augmented_sequence_size_management_t{}) == augmented_sequence_size_management_e::at_each_node_except_node_end)
        {
            return detail::augmented_sequence_rb3p::rb3p_functor_t<false, config_t>::size(node_end);
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
            new (node_new->p_element()) element_t(std::forward<args_t>(args)...);
            if constexpr (!std::is_same_v<accumulated_storage_t, void>)
                detail::language::tagged_ptr_bit0_unsetted(node_end)->accumulator.construct_accumulated_storage(this->allocator_element, node_new->p_accumulated_storage(), std::make_tuple());
            typename detail::augmented_sequence_rb3p::rb3p_functor_t<false, config_t>::schedules_t schedules;
            detail::augmented_sequence_rb3p::rb3p_functor_t<false, config_t>::insert(schedules, node_end, pos.current_node, node_new);
            detail::augmented_sequence_rb3p::rb3p_functor_t<false, config_t>::run_schedules(schedules, detail::language::tagged_ptr_bit0_unsetted(node_end)->accumulator);
            return std::ranges::next(result);
        }
        iterator_t insert(const_iterator_t pos, element_t const &element) { return this->emplace(pos, element); }
        iterator_t insert(const_iterator_t pos, element_t &&element) { return this->emplace(pos, std::move(element)); }
        iterator_t insert(const_iterator_t pos, size_type count, element_t const &value)
        {
            iterator_t result = std::ranges::prev(iterator_t(pos.current_node));
            std::ranges::for_each(std::views::iota(static_cast<size_type>(0), count), [this, &pos, &value]([[maybe_unused]] size_type index) { this->emplace(pos, value); });
            return std::ranges::next(result);
        }
        template<std::input_iterator iterator_t_, std::sentinel_for<iterator_t_> sentinel_t_>
        iterator_t insert(const_iterator_t pos, iterator_t_ iterator, sentinel_t_ sentinel)
        {
            iterator_t result = std::ranges::prev(iterator_t(pos.current_node));
            std::ranges::for_each(std::ranges::subrange(iterator, sentinel), [this, &pos]<typename other_element_t>(other_element_t &&other_element) { this->emplace(pos, std::forward<other_element_t>(other_element)); });
            return std::ranges::next(result);
        }
        iterator_t insert(const_iterator_t pos, std::initializer_list<element_t> initializer_list)
        {
            iterator_t result = std::ranges::prev(iterator_t(pos.current_node));
            std::ranges::for_each(initializer_list, [this, &pos](element_t const &element) { this->emplace(pos, element); });
            return std::ranges::next(result);
        }
        iterator_t erase(const_iterator_t pos)
        {
            iterator_t result = std::ranges::prev(iterator_t(pos.current_node));
            typename detail::augmented_sequence_rb3p::rb3p_functor_t<false, config_t>::schedules_t schedules;
            detail::augmented_sequence_rb3p::rb3p_functor_t<false, config_t>::erase(schedules, node_end, pos.current_node);
            detail::augmented_sequence_rb3p::rb3p_functor_t<false, config_t>::run_schedules(schedules, detail::language::tagged_ptr_bit0_unsetted(node_end)->accumulator);
            if constexpr (!std::is_same_v<accumulated_storage_t, void>)
                detail::language::tagged_ptr_bit0_unsetted(node_end)->accumulator.destroy_accumulated_storage(this->allocator_element, static_cast<node_t *>(pos.current_node)->p_accumulated_storage());
            static_cast<node_t *>(pos.current_node)->p_element()->~element_t();
            delete static_cast<node_t *>(pos.current_node);
            return std::ranges::next(result);
        }
        iterator_t erase(const_iterator_t pos_begin, const_iterator_t pos_end)
        {
            iterator_t result = std::ranges::prev(iterator_t(pos_begin.current_node));
            for (; pos_begin != pos_end;)
            {
                navigator_t *pos_begin_current_node = pos_begin.current_node;
                ++pos_begin;
                detail::augmented_sequence_rb3p::rb3p_functor_t<false, config_t>::erase(pos_begin.current_node);
                static_cast<node_t *>(pos_begin.current_node)->p_element()->~element_t();
                delete static_cast<node_t *>(pos_begin.current_node);
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
            for (; !exhaust1 && !exhaust2; exhaust1 = (++f1 == l1), exhaust2 = (++f2 == l2))
                if (auto c = comp(*f1, *f2); c != 0)
                    return c;
            return !exhaust1 ? std::strong_ordering::greater : !exhaust2 ? std::strong_ordering::less
                                                                         : std::strong_ordering::equal;
#else
            return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
#endif
        }

        friend augmented_sequence_t &operator<<(augmented_sequence_t &lhs, augmented_sequence_t &&rhs)
        {
            if (&lhs == &rhs)
                return lhs;
            typename detail::augmented_sequence_rb3p::rb3p_functor_t<false, config_t>::schedules_t schedules;
            detail::augmented_sequence_rb3p::rb3p_functor_t<false, config_t>::template concat_without_middle_key<false>(schedules, lhs.node_end, rhs.node_end);
            detail::augmented_sequence_rb3p::rb3p_functor_t<false, config_t>::run_schedules(schedules, detail::language::tagged_ptr_bit0_unsetted(lhs.node_end)->accumulator);
            return lhs;
        }
        friend augmented_sequence_t &&operator<<(augmented_sequence_t &&lhs, augmented_sequence_t &&rhs)
        {
            if (&lhs == &rhs)
                return std::move(lhs);
            typename detail::augmented_sequence_rb3p::rb3p_functor_t<false, config_t>::schedules_t schedules;
            detail::augmented_sequence_rb3p::rb3p_functor_t<false, config_t>::template concat_without_middle_key<false>(schedules, lhs.node_end, rhs.node_end);
            detail::augmented_sequence_rb3p::rb3p_functor_t<false, config_t>::run_schedules(schedules, detail::language::tagged_ptr_bit0_unsetted(lhs.node_end)->accumulator);
            return std::move(lhs);
        }
        friend augmented_sequence_t &operator>>(augmented_sequence_t &&lhs, augmented_sequence_t &rhs)
        {
            if (&lhs == &rhs)
                return rhs;
            typename detail::augmented_sequence_rb3p::rb3p_functor_t<false, config_t>::schedules_t schedules;
            detail::augmented_sequence_rb3p::rb3p_functor_t<false, config_t>::template concat_without_middle_key<true>(schedules, lhs.node_end, rhs.node_end);
            detail::augmented_sequence_rb3p::rb3p_functor_t<false, config_t>::run_schedules(schedules, detail::language::tagged_ptr_bit0_unsetted(rhs.node_end)->accumulator);
            return rhs;
        }
        friend augmented_sequence_t &&operator>>(augmented_sequence_t &&lhs, augmented_sequence_t &&rhs)
        {
            if (&lhs == &rhs)
                return std::move(rhs);
            typename detail::augmented_sequence_rb3p::rb3p_functor_t<false, config_t>::schedules_t schedules;
            detail::augmented_sequence_rb3p::rb3p_functor_t<false, config_t>::template concat_without_middle_key<true>(schedules, lhs.node_end, rhs.node_end);
            detail::augmented_sequence_rb3p::rb3p_functor_t<false, config_t>::run_schedules(schedules, detail::language::tagged_ptr_bit0_unsetted(rhs.node_end)->accumulator);
            return std::move(rhs);
        }

        augmented_sequence_t split_emit_left(const_iterator_t pos)
        {
            augmented_sequence_t result;
            typename detail::augmented_sequence_rb3p::rb3p_functor_t<false, config_t>::schedules_t schedules;
            detail::augmented_sequence_rb3p::rb3p_functor_t<false, config_t>::template split<false>(schedules, result.node_end, node_end, pos.current_node);
            detail::augmented_sequence_rb3p::rb3p_functor_t<false, config_t>::run_schedules(schedules, detail::language::tagged_ptr_bit0_unsetted(node_end)->accumulator);
            return result;
        }
        augmented_sequence_t split_emit_right(const_iterator_t pos)
        {
            augmented_sequence_t result;
            typename detail::augmented_sequence_rb3p::rb3p_functor_t<false, config_t>::schedules_t schedules;
            detail::augmented_sequence_rb3p::rb3p_functor_t<false, config_t>::template split<true>(schedules, result.node_end, node_end, pos.current_node);
            detail::augmented_sequence_rb3p::rb3p_functor_t<false, config_t>::run_schedules(schedules, detail::language::tagged_ptr_bit0_unsetted(node_end)->accumulator);
            return result;
        }

        template<bool is_reversed = false, bool is_const_iterator_begin, bool is_const_iterator_end>
        accumulated_storage_t read_range(detail::augmented_sequence_rb3p::rb3p_iterator_t<is_const_iterator_begin, is_reversed, config_t> const &iterator_begin, detail::augmented_sequence_rb3p::rb3p_iterator_t<is_const_iterator_end, is_reversed, config_t> const &iterator_end) const
        {
            if (iterator_begin == iterator_end)
                return detail::language::tagged_ptr_bit0_unsetted(node_end)->accumulator.construct_accumulated_storage(allocator_element, std::make_tuple());

            navigator_t *node_front = iterator_begin.current_node;
            navigator_t *node_back = std::ranges::prev(iterator_end).current_node;

            return detail::augmented_sequence_rb3p::rb3p_functor_t<is_reversed, config_t>::read_range_impl(allocator_element, node_end, node_front, node_back);
        }

        template<bool is_reversed = false, bool is_const_iterator_begin, bool is_const_iterator_end>
        void update_range(detail::augmented_sequence_rb3p::rb3p_iterator_t<is_const_iterator_begin, is_reversed, config_t> const &iterator_begin, detail::augmented_sequence_rb3p::rb3p_iterator_t<is_const_iterator_end, is_reversed, config_t> const &iterator_end)
        {
            if (iterator_begin == iterator_end)
                return;

            navigator_t *node_front = iterator_begin.current_node;
            navigator_t *node_back = std::ranges::prev(iterator_end).current_node;

            detail::augmented_sequence_rb3p::rb3p_functor_t<is_reversed, config_t>::update_range_impl(node_end, node_front, node_back);
        }

        template<bool is_reversed = false, detail::concepts::invocable_r<bool, accumulated_storage_t const &> monotonic_predicate_t>
        const_iterator_t find_by_monotonic_predicate(monotonic_predicate_t const &monotonic_predicate) const
        {
            return {detail::augmented_sequence_rb3p::rb3p_functor_t<is_reversed, config_t>::find_by_monotonic_predicate(allocator_element, node_end, monotonic_predicate)};
        }
        template<bool is_reversed = false, detail::concepts::invocable_r<bool, accumulated_storage_t const &> monotonic_predicate_t>
        iterator_t find_by_monotonic_predicate(monotonic_predicate_t const &monotonic_predicate)
        {
            return {detail::augmented_sequence_rb3p::rb3p_functor_t<is_reversed, config_t>::find_by_monotonic_predicate(allocator_element, node_end, monotonic_predicate)};
        }

        template<bool is_reversed = false, /*std::output_iterator<const_iterator_t>*/ typename iterator_output_const_iterator_t, typename heap_predicate_t> requires (detail::concepts::invocable_r<heap_predicate_t, bool, accumulated_storage_t &> && detail::concepts::invocable_r<heap_predicate_t, bool, element_t const &>)
        void find_by_heap_predicate(iterator_output_const_iterator_t iterator_output_const_iterator, heap_predicate_t const &heap_predicate) const
        {
            detail::augmented_sequence_rb3p::rb3p_functor_t<is_reversed, config_t>::find_by_heap_predicate(
                node_end,
                detail::iterator::transform_output_iterator_t{
                    .wrapped_iterator = iterator_output_const_iterator,
                    .transformer = [](navigator_t *value) { return const_iterator_t{value}; },
                },
                heap_predicate
            );
        }
        template<bool is_reversed = false, /*std::output_iterator<iterator_t>*/ typename iterator_output_iterator_t, typename heap_predicate_t> requires (detail::concepts::invocable_r<heap_predicate_t, bool, accumulated_storage_t &> && detail::concepts::invocable_r<heap_predicate_t, bool, element_t const &>)
        void find_by_heap_predicate(iterator_output_iterator_t iterator_output_iterator, heap_predicate_t const &heap_predicate)
        {
            detail::augmented_sequence_rb3p::rb3p_functor_t<is_reversed, config_t>::find_by_heap_predicate(
                node_end,
                detail::iterator::transform_output_iterator_t{
                    .wrapped_iterator = iterator_output_iterator,
                    .transformer = [](navigator_t *value) { return iterator_t{value}; },
                },
                heap_predicate
            );
        }

        template<bool is_reversed = false, typename heap_predicate_t> requires (detail::concepts::invocable_r<heap_predicate_t, bool, accumulated_storage_t &> && detail::concepts::invocable_r<heap_predicate_t, bool, element_t const &>)
        detail::coroutine::generator_t<const_iterator_t> find_by_heap_predicate_generator(heap_predicate_t const &heap_predicate) const
        {
            for (navigator_t *value : detail::augmented_sequence_rb3p::rb3p_functor_t<is_reversed, config_t>::find_by_heap_predicate_generator(node_end, heap_predicate))
                co_yield const_iterator_t{value};
        }
        template<bool is_reversed = false, typename heap_predicate_t> requires (detail::concepts::invocable_r<heap_predicate_t, bool, accumulated_storage_t &> && detail::concepts::invocable_r<heap_predicate_t, bool, element_t const &>)
        detail::coroutine::generator_t<iterator_t> find_by_heap_predicate_generator(heap_predicate_t const &heap_predicate)
        {
            for (navigator_t *value : detail::augmented_sequence_rb3p::rb3p_functor_t<is_reversed, config_t>::find_by_heap_predicate_generator(node_end, heap_predicate))
                co_yield iterator_t{value};
        }
    };
} // namespace augmented_containers

#endif // AUGMENTED_RB3P_HPP
