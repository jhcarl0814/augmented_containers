#ifndef AUGMENTED_DEQUE_H
#define AUGMENTED_DEQUE_H

#include <tuple>
#include <functional>
#include <cassert>
#include <ranges>
#include <iterator>
#include <algorithm>
#include <cstddef>
#include <concepts>

namespace augmented_containers
{
    namespace detail
    {
        namespace tagged_ptr_ns
        {
            template<typename scalar_t>
            bool tagged_ptr_bit0_is_set(scalar_t p) { return (reinterpret_cast<uintptr_t>(p) & 0b1) != 0; };
            template<typename scalar_t>
            scalar_t tagged_ptr_bit0_unsetted_relaxed(scalar_t p) { return reinterpret_cast<scalar_t>(reinterpret_cast<uintptr_t>(p) & ~0b1); }
            template<typename scalar_t>
            scalar_t tagged_ptr_bit0_unsetted(scalar_t p) { return assert(tagged_ptr_bit0_is_set(p)), tagged_ptr_bit0_unsetted_relaxed(p); }
            template<typename scalar_t>
            scalar_t tagged_ptr_bit0_setted_relaxed(scalar_t p) { return reinterpret_cast<scalar_t>(reinterpret_cast<uintptr_t>(p) | 0b1); }
            template<typename scalar_t>
            scalar_t tagged_ptr_bit0_setted(scalar_t p) { return assert(!tagged_ptr_bit0_is_set(p)), tagged_ptr_bit0_setted_relaxed(p); }
        } // namespace tagged_ptr_ns

        namespace mp
        {
            template<bool is_const = true, typename T = void>
            using conditional_const_t = std::conditional_t<is_const, const T, T>;

            template<bool is_const = true, typename T = void>
            constexpr conditional_const_t<is_const, T> &conditional_as_const(T &_Val) noexcept { return _Val; }
            template<bool is_const = true, typename T = void>
            void conditional_as_const(T const &&) = delete;

            template<class F, typename Ret, class... Args>
            concept invocable_r = std::invocable<F, Args...> && std::same_as<Ret, std::invoke_result_t<F, Args...>>; // https://stackoverflow.com/questions/61932900/c-template-function-specify-argument-type-of-callback-functor-lambda-while-st#comment109544863_61933163

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


            template<typename tuple_t, typename T>
            struct type_is_in_tuple: std::bool_constant<false>
            {};
            template<typename... tuple_elements_t, typename T>
            struct type_is_in_tuple<std::tuple<tuple_elements_t...>, T>: std::bool_constant<(... || std::is_same_v<tuple_elements_t, T>)>
            {};
            template<typename tuple_t, typename T>
            constexpr bool type_is_in_tuple_v = type_is_in_tuple<tuple_t, T>::value;

            template<typename map_t>
            struct is_unique_map
            {
                template<std::size_t I, typename... accumulated_elements_t>
                struct iteration: std::bool_constant<
                                      !type_is_in_tuple_v<typename std::tuple_element_t<I, map_t>::first_type, std::tuple<accumulated_elements_t...>> &&
                                      iteration<I + 1, std::tuple<accumulated_elements_t..., typename std::tuple_element_t<I, map_t>::first_type>>::value>
                {
                };
                template<typename... accumulated_elements_t>
                struct iteration<std::tuple_size_v<map_t>, accumulated_elements_t...>: std::bool_constant<true>
                {
                };
                static constexpr bool value = iteration<0>::value;
            };
            template<typename map_t>
            constexpr bool is_unique_map_v = is_unique_map<map_t>::value;

            template<typename map_t, typename key_t, typename default_value_t>
            struct map_find_first
            {
                template<std::size_t I>
                struct iteration: std::type_identity<typename std::conditional_t<std::is_same_v<typename std::tuple_element_t<I, map_t>::first_type, key_t>, std::type_identity<typename std::tuple_element_t<I, map_t>::second_type>, iteration<I + 1>>::type>
                {
                };
                template<>
                struct iteration<std::tuple_size_v<map_t>>: std::type_identity<default_value_t>
                {
                };
                using type = typename iteration<0>::type;
            };
            template<typename map_t, typename key_t, typename default_value_t>
            using map_find_first_t = typename map_find_first<map_t, key_t, default_value_t>::type;

            template<typename map_t, typename key_t>
            struct map_find_first_index
            {
                template<std::size_t I>
                struct iteration: std::conditional_t<std::is_same_v<typename std::tuple_element_t<I, map_t>::first_type, key_t>, std::type_identity<std::integral_constant<std::size_t, I>>, iteration<I + 1>>::type
                {
                };
                template<>
                struct iteration<std::tuple_size_v<map_t>>: std::integral_constant<std::size_t, std::tuple_size_v<map_t>>
                {
                };
                using type = typename iteration<0>::type;
            };
            template<typename map_t, typename key_t>
            using map_find_first_index_t = typename map_find_first_index<map_t, key_t>::type;

            template<typename map_t, typename key_t>
            struct map_erase_first
            {
                template<std::size_t I, typename remaining_item_list_t, typename accumulated_item_list_t>
                struct iteration;
                template<std::size_t I, typename remaining_item_t, typename... remaining_items_t, typename... accumulated_items_t>
                struct iteration<I, std::tuple<remaining_item_t, remaining_items_t...>, std::tuple<accumulated_items_t...>>: std::type_identity<typename std::conditional_t<
                                                                                                                                 std::is_same_v<typename remaining_item_t::first_type, key_t>,
                                                                                                                                 std::type_identity<std::tuple<accumulated_items_t..., remaining_items_t...>>,
                                                                                                                                 iteration<I + 1, std::tuple<remaining_items_t...>, std::tuple<accumulated_items_t..., remaining_item_t>> //
                                                                                                                                 >::type>
                {
                };
                template<typename... accumulated_items_t>
                struct iteration<std::tuple_size_v<map_t>, std::tuple<>, std::tuple<accumulated_items_t...>>: std::type_identity<std::tuple<accumulated_items_t...>>
                {
                };
                using type = typename iteration<0, map_t, std::tuple<>>::type;
            };
            template<typename map_t, typename key_t>
            using map_erase_first_t = typename map_erase_first<map_t, key_t>::type;

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
        } // namespace mp

        namespace augmented_deque
        {
            using namespace tagged_ptr_ns;
            using namespace mp;

            template<typename projected_storage_t>
            struct add_projected_storage_member_t
            {
                alignas(projected_storage_t) std::byte projected_storage_buffer[sizeof(projected_storage_t)]; // projected_storage_t projected_storage;
                projected_storage_t *p_projected_storage() { return reinterpret_cast<projected_storage_t *>(&projected_storage_buffer); }
            };
            template<>
            struct add_projected_storage_member_t<void>
            {
            };

            template<typename accumulated_storage_t, typename derived_t>
            struct add_accumulated_storage_member_t
            {
                alignas(accumulated_storage_t) std::byte accumulated_storage_buffer[sizeof(accumulated_storage_t)]; // accumulated_storage_t accumulated_storage;
                accumulated_storage_t *p_accumulated_storage() { return reinterpret_cast<accumulated_storage_t *>(&accumulated_storage_buffer); }

                static derived_t const *from_accumulated_storage_address(accumulated_storage_t const *address) { return reinterpret_cast<derived_t *>(const_cast<std::byte *>(reinterpret_cast<std::byte const *>(address) - offsetof(derived_t, accumulated_storage_buffer))); }
                static derived_t *from_accumulated_storage_address(accumulated_storage_t *address) { return reinterpret_cast<derived_t *>(reinterpret_cast<std::byte *>(address) - offsetof(derived_t, accumulated_storage_buffer)); }
            };
            template<typename derived_t>
            struct add_accumulated_storage_member_t<void, derived_t>
            {
            };

            template<typename accumulated_storage_t>
            struct tree_node_t: add_accumulated_storage_member_t<accumulated_storage_t, tree_node_t<accumulated_storage_t>>
            {
                tree_node_t *parent = nullptr, *child_left = nullptr, *child_right = nullptr;
            };

            struct circular_doubly_linked_list_node_navigator_t
            {
                circular_doubly_linked_list_node_navigator_t *prev, *next;

                circular_doubly_linked_list_node_navigator_t() // initialize to node_end
                    : prev(tagged_ptr_bit0_setted(this)),
                      next(tagged_ptr_bit0_setted(this))
                {}

                circular_doubly_linked_list_node_navigator_t(std::nullptr_t) // not initialized
                    : prev(nullptr),
                      next(nullptr)
                {}

                static void push_impl(circular_doubly_linked_list_node_navigator_t *tagged_end, circular_doubly_linked_list_node_navigator_t *(circular_doubly_linked_list_node_navigator_t::*prev), circular_doubly_linked_list_node_navigator_t *(circular_doubly_linked_list_node_navigator_t::*next), circular_doubly_linked_list_node_navigator_t *node_new)
                {
                    if(tagged_ptr_bit0_unsetted(tagged_end)->*prev == tagged_end && tagged_ptr_bit0_unsetted(tagged_end)->*next == tagged_end)
                    {
                        node_new->*prev = node_new->*next = tagged_end;
                        tagged_ptr_bit0_unsetted(tagged_end)->*prev = tagged_ptr_bit0_unsetted(tagged_end)->*next = tagged_ptr_bit0_setted(node_new);
                    }
                    else if(tagged_ptr_bit0_unsetted(tagged_end)->*prev != tagged_end && tagged_ptr_bit0_unsetted(tagged_end)->*next != tagged_end)
                    {
                        node_new->*prev = tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(tagged_end)->*prev);
                        node_new->*prev->*next = node_new;
                        node_new->*next = tagged_end;
                        tagged_ptr_bit0_unsetted(tagged_end)->*prev = tagged_ptr_bit0_setted(node_new);
                    }
                    else std::unreachable();
                }
                static void extract_impl(circular_doubly_linked_list_node_navigator_t *node, circular_doubly_linked_list_node_navigator_t *(circular_doubly_linked_list_node_navigator_t::*prev), circular_doubly_linked_list_node_navigator_t *(circular_doubly_linked_list_node_navigator_t::*next))
                {
                    if(tagged_ptr_bit0_is_set(node->*prev))
                        tagged_ptr_bit0_unsetted(node->*prev)->*next = tagged_ptr_bit0_setted_relaxed(node->*next);
                    else
                        node->*prev->*next = node->*next;
                    if(tagged_ptr_bit0_is_set(node->*next))
                        tagged_ptr_bit0_unsetted(node->*next)->*prev = tagged_ptr_bit0_setted_relaxed(node->*prev);
                    else
                        node->*next->*prev = node->*prev;
                }

                static circular_doubly_linked_list_node_navigator_t *untagged_prev_or_tagged_end(circular_doubly_linked_list_node_navigator_t *tagged_end)
                {
                    if(circular_doubly_linked_list_node_navigator_t *prev = tagged_ptr_bit0_unsetted(tagged_end)->prev; prev == tagged_end)
                        return prev;
                    else
                        return tagged_ptr_bit0_unsetted(prev);
                }

                static circular_doubly_linked_list_node_navigator_t *untagged_next_or_tagged_end(circular_doubly_linked_list_node_navigator_t *tagged_end)
                {
                    if(circular_doubly_linked_list_node_navigator_t *next = tagged_ptr_bit0_unsetted(tagged_end)->next; next == tagged_end)
                        return next;
                    else
                        return tagged_ptr_bit0_unsetted(next);
                }

                template<typename node_end_t>
                struct node_end_functions_t
                {
                    static node_end_t *create_tagged_end() { return tagged_ptr_bit0_setted(new node_end_t()); }
                };

                template<typename node_t>
                struct node_functions_t
                {
                    static node_t *untagged_prev(circular_doubly_linked_list_node_navigator_t *tagged_end) { return static_cast<node_t *>(tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(tagged_end)->prev)); }
                    static node_t *untagged_next(circular_doubly_linked_list_node_navigator_t *tagged_end) { return static_cast<node_t *>(tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(tagged_end)->next)); }
                };
            };

            template<typename sequence_config_t>
            struct list_node_end_t;

            template<typename sequence_config_t>
            struct digit_node_end_t: circular_doubly_linked_list_node_navigator_t, circular_doubly_linked_list_node_navigator_t::node_end_functions_t<digit_node_end_t<sequence_config_t>>, add_accumulated_storage_member_t<typename sequence_config_t::accumulated_storage_t, digit_node_end_t<sequence_config_t>>
            {
                std::size_t node_count = 0;
                list_node_end_t<sequence_config_t> *list_node_end = nullptr;

                circular_doubly_linked_list_node_navigator_t *middle = nullptr;
                typename sequence_config_t::projector_and_accumulator_t projector_and_accumulator;
            };
            template<typename sequence_config_t>
            struct digit_node_t: circular_doubly_linked_list_node_navigator_t, circular_doubly_linked_list_node_navigator_t::node_functions_t<digit_node_t<sequence_config_t>>, add_accumulated_storage_member_t<typename sequence_config_t::accumulated_storage_t, digit_node_t<sequence_config_t>>
            {
                tree_node_t<typename sequence_config_t::accumulated_storage_t> *tree_left = nullptr, *tree_right = nullptr;
                std::size_t digit_position;

                digit_node_t(std::size_t digit_position)
                    : circular_doubly_linked_list_node_navigator_t(nullptr),
                      digit_position(digit_position)
                {}
            };

            template<typename sequence_config_t>
            struct list_node_end_t: circular_doubly_linked_list_node_navigator_t, circular_doubly_linked_list_node_navigator_t::node_end_functions_t<list_node_end_t<sequence_config_t>>
            {
                std::size_t node_count = 0, front_element_count = 0, back_element_count = 0;
                digit_node_end_t<sequence_config_t> *digit_node_end = nullptr;

                typename sequence_config_t::actual_projected_storage_end_t actual_projected_storage_end;
            };
            template<typename sequence_config_t>
                requires(sequence_config_t::requested_stride == 1)
            struct list_node_end_t<sequence_config_t>: circular_doubly_linked_list_node_navigator_t, circular_doubly_linked_list_node_navigator_t::node_end_functions_t<list_node_end_t<sequence_config_t>>
            {
                std::size_t node_count = 0;
                digit_node_end_t<sequence_config_t> *digit_node_end = nullptr;

                typename sequence_config_t::actual_projected_storage_end_t actual_projected_storage_end;
            };
            template<typename sequence_config_t>
            struct list_node_t: circular_doubly_linked_list_node_navigator_t, circular_doubly_linked_list_node_navigator_t::node_functions_t<list_node_t<sequence_config_t>>
            {
                tree_node_t<typename sequence_config_t::accumulated_storage_t> *parent = nullptr;
                typename sequence_config_t::actual_projected_storage_t actual_projected_storage;

                using actual_projected_storage_t = typename sequence_config_t::actual_projected_storage_t;
            };

            template<typename iterator_t> // https://stackoverflow.com/questions/6006614/c-static-polymorphism-crtp-and-using-typedefs-from-derived-classes
            struct const_iterator
            {
            };
            template<typename iterator_t>
            using const_iterator_t = typename const_iterator<iterator_t>::type;

            template<typename iterator_t> // https://stackoverflow.com/questions/6006614/c-static-polymorphism-crtp-and-using-typedefs-from-derived-classes
            struct non_const_iterator
            {
            };
            template<typename iterator_t>
            using non_const_iterator_t = typename non_const_iterator<iterator_t>::type;

            template<bool is_const_template_parameter, typename sequence_config_t, typename derived_t>
            struct iterator_list_node_t
            {
                using list_node_t = list_node_t<sequence_config_t>;
                using list_node_end_t = list_node_end_t<sequence_config_t>;
                using tree_node_t = tree_node_t<typename sequence_config_t::accumulated_storage_t>;
                using digit_node_t = digit_node_t<sequence_config_t>;
                using digit_node_end_t = digit_node_end_t<sequence_config_t>;

                static list_node_t *p_tree_node_to_p_list_node(tree_node_t *p) { return reinterpret_cast<list_node_t *>(tagged_ptr_bit0_unsetted(p)); }
                static tree_node_t *p_list_node_to_p_tree_node(list_node_t *p) { return reinterpret_cast<tree_node_t *>(tagged_ptr_bit0_setted(p)); }
                static tree_node_t *p_digit_node_to_p_tree_node(digit_node_t *p) { return reinterpret_cast<tree_node_t *>(tagged_ptr_bit0_setted(p)); }
                static digit_node_t *p_tree_node_to_p_digit_node(tree_node_t *p) { return reinterpret_cast<digit_node_t *>(tagged_ptr_bit0_unsetted(p)); }

                circular_doubly_linked_list_node_navigator_t *current_list_node = nullptr;
                iterator_list_node_t(circular_doubly_linked_list_node_navigator_t *current_list_node)
                    : current_list_node(current_list_node)
                {}
                bool is_end() const
                {
                    assert(current_list_node != nullptr);
                    return tagged_ptr_bit0_is_set(current_list_node);
                }

                static constexpr bool is_const = is_const_template_parameter;
                using non_const_iterator_t = non_const_iterator_t<derived_t>;
                using const_iterator_t = const_iterator_t<derived_t>;
                non_const_iterator_t to_non_const() const { return {current_list_node}; }
                const_iterator_t to_const() const { return {current_list_node}; }
                iterator_list_node_t(non_const_iterator_t const &rhs)
                    requires(is_const) // https://quuxplusone.github.io/blog/2018/12/01/const-iterator-antipatterns/
                    : current_list_node(rhs.current_list_node)
                {}
                const_iterator_t &operator=(non_const_iterator_t const &rhs) &
                        requires(is_const)
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
                    if(tagged_ptr_bit0_is_set(current_list_node))
                        current_list_node = circular_doubly_linked_list_node_navigator_t::untagged_next_or_tagged_end(current_list_node);
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
                    conditional_const_t<is_const, value_type> &operator*() const &
                    {
                        assert(this->current_list_node != nullptr);
                        assert(!tagged_ptr_bit0_is_set(this->current_list_node));
                        return conditional_as_const<is_const>(static_cast<list_node_t *>(this->current_list_node)->actual_projected_storage);
                    }
                    conditional_const_t<is_const, value_type> *to_address() const & { return &operator*(); }
                    conditional_const_t<is_const, value_type> *operator->() const & { return &operator*(); }
                    static iterator_list_node_t from_actual_projected_storage_address(conditional_const_t<is_const, value_type> *address) { return {reinterpret_cast<list_node_t *>(const_cast<std::byte *>(reinterpret_cast<conditional_const_t<is_const, std::byte> *>(address) - offsetof(list_node_t, actual_projected_storage)))}; }
                };

                // std::forward_iterator / std::sentinel_for / __WeaklyEqualityComparableWith, std::forward_iterator / std::incrementable / std::regular
                template<std::bool_constant<is_const> * = nullptr>
                    requires(is_const)
                friend bool operator==(const_iterator_t const &lhs, const_iterator_t const &rhs)
                {
                    assert((lhs.current_list_node != nullptr) == (rhs.current_list_node != nullptr));
                    return lhs.current_list_node == rhs.current_list_node;
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
                friend bool operator==(derived_t const &lhs, [[maybe_unused]] std::default_sentinel_t const &rhs)
                {
                    assert(lhs.current_list_node != nullptr);
                    return tagged_ptr_bit0_is_set(lhs.current_list_node);
                }

                // std::bidirectional_iterator
                derived_t &operator--() &
                {
                    assert(current_list_node != nullptr);
                    if(tagged_ptr_bit0_is_set(current_list_node))
                        current_list_node = circular_doubly_linked_list_node_navigator_t::untagged_prev_or_tagged_end(current_list_node);
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
                template<std::bool_constant<is_const> * = nullptr>
                    requires(is_const)
                friend std::strong_ordering operator<=>(const_iterator_t const &lhs, const_iterator_t const &rhs)
                {
                    assert((lhs.current_list_node != nullptr) == (rhs.current_list_node != nullptr));
                    if(lhs.current_list_node == rhs.current_list_node)
                        return std::strong_ordering::equal;
                    if(tagged_ptr_bit0_is_set(lhs.current_list_node))
                        return std::strong_ordering::greater;
                    if(tagged_ptr_bit0_is_set(rhs.current_list_node))
                        return std::strong_ordering::less;

                    tree_node_t *tree_node_lhs = reinterpret_cast<tree_node_t *>(lhs.current_list_node);
                    tree_node_t *tree_node_rhs = reinterpret_cast<tree_node_t *>(rhs.current_list_node);
                    tree_node_t *tree_node_lhs_parent = static_cast<list_node_t *>(lhs.current_list_node)->parent;
                    tree_node_t *tree_node_rhs_parent = static_cast<list_node_t *>(rhs.current_list_node)->parent;
                    auto same_digit_node_reached = [&tree_node_lhs, &tree_node_rhs, &tree_node_lhs_parent]()
                    {
                        if(digit_node_t *digit_node_parent = p_tree_node_to_p_digit_node(tree_node_lhs_parent);
                            tagged_ptr_bit0_unsetted_relaxed(digit_node_parent->tree_left) == tree_node_lhs && tagged_ptr_bit0_unsetted_relaxed(digit_node_parent->tree_right) == tree_node_rhs)
                            return std::strong_ordering::less;
                        else if(tagged_ptr_bit0_unsetted_relaxed(digit_node_parent->tree_left) == tree_node_rhs && tagged_ptr_bit0_unsetted_relaxed(digit_node_parent->tree_right) == tree_node_lhs)
                            return std::strong_ordering::greater;
                        else std::unreachable();
                    };
                    while(true)
                    {
                        if(tree_node_lhs_parent == tree_node_rhs_parent)
                        {
                            if(tree_node_t *tree_node_parent = tree_node_lhs_parent; tagged_ptr_bit0_is_set(tree_node_parent))
                                return same_digit_node_reached();
                            else
                            {
                                if(tagged_ptr_bit0_unsetted_relaxed(tree_node_parent->child_left) == tree_node_lhs && tagged_ptr_bit0_unsetted_relaxed(tree_node_parent->child_right) == tree_node_rhs)
                                    return std::strong_ordering::less;
                                else if(tagged_ptr_bit0_unsetted_relaxed(tree_node_parent->child_left) == tree_node_rhs && tagged_ptr_bit0_unsetted_relaxed(tree_node_parent->child_right) == tree_node_lhs)
                                    return std::strong_ordering::greater;
                                else std::unreachable();
                            }
                        }
                        else
                        {
                            if(tagged_ptr_bit0_is_set(tree_node_lhs_parent))
                                break;
                            if(tagged_ptr_bit0_is_set(tree_node_rhs_parent))
                                break;
                            tree_node_lhs = std::exchange(tree_node_lhs_parent, tree_node_lhs_parent->parent);
                            tree_node_rhs = std::exchange(tree_node_rhs_parent, tree_node_rhs_parent->parent);
                        }
                    }
                    while(!tagged_ptr_bit0_is_set(tree_node_lhs_parent))
                        tree_node_lhs = std::exchange(tree_node_lhs_parent, tree_node_lhs_parent->parent);
                    while(!tagged_ptr_bit0_is_set(tree_node_rhs_parent))
                        tree_node_rhs = std::exchange(tree_node_rhs_parent, tree_node_rhs_parent->parent);
                    if(tree_node_lhs_parent == tree_node_rhs_parent)
                        return same_digit_node_reached();
                    else
                    {
                        if(digit_node_t *digit_node_lhs = p_tree_node_to_p_digit_node(tree_node_lhs_parent),
                            *digit_node_rhs = p_tree_node_to_p_digit_node(tree_node_rhs_parent);
                            tagged_ptr_bit0_is_set(digit_node_lhs->next) || static_cast<digit_node_t *>(digit_node_lhs->next)->digit_position < digit_node_lhs->digit_position)
                        {
                            if(tagged_ptr_bit0_is_set(digit_node_rhs->next) || static_cast<digit_node_t *>(digit_node_rhs->next)->digit_position < digit_node_rhs->digit_position)
                                return digit_node_rhs->digit_position <=> digit_node_lhs->digit_position;
                            else
                                return std::strong_ordering::greater;
                        }
                        else
                        {
                            if(tagged_ptr_bit0_is_set(digit_node_rhs->next) || static_cast<digit_node_t *>(digit_node_rhs->next)->digit_position < digit_node_rhs->digit_position)
                                return std::strong_ordering::less;
                            else
                                return digit_node_lhs->digit_position <=> digit_node_rhs->digit_position;
                        }
                    }
                }
                template<std::bool_constant<is_const> * = nullptr>
                    requires(!is_const)
                friend std::strong_ordering operator<=>(const_iterator_t const &lhs, non_const_iterator_t const &rhs)
                {
                    return lhs <=> rhs.to_const();
                }
                template<std::bool_constant<is_const> * = nullptr>
                    requires(!is_const)
                friend std::strong_ordering operator<=>(non_const_iterator_t const &lhs, const_iterator_t const &rhs)
                {
                    return lhs.to_const() <=> rhs;
                }
                template<std::bool_constant<is_const> * = nullptr>
                    requires(!is_const)
                friend std::strong_ordering operator<=>(non_const_iterator_t const &lhs, non_const_iterator_t const &rhs)
                {
                    return lhs.to_const() <=> rhs.to_const();
                }
                friend std::strong_ordering operator<=>(derived_t const &lhs, [[maybe_unused]] std::default_sentinel_t const &rhs)
                {
                    assert(lhs.current_list_node != nullptr);
                    if(tagged_ptr_bit0_is_set(lhs.current_list_node))
                        return std::strong_ordering::equal;
                    else
                        return std::strong_ordering::less;
                }

                // std::random_access_iterator
                void move_impl(std::size_t distance,
                    list_node_t *(*p_untagged_prev)(circular_doubly_linked_list_node_navigator_t *tagged_end),
                    list_node_t *(*p_untagged_next)(circular_doubly_linked_list_node_navigator_t *tagged_end),
                    circular_doubly_linked_list_node_navigator_t *(circular_doubly_linked_list_node_navigator_t::*p_prev),
                    circular_doubly_linked_list_node_navigator_t *(circular_doubly_linked_list_node_navigator_t::*p_next),
                    tree_node_t *(digit_node_t::*p_tree_left),
                    tree_node_t *(digit_node_t::*p_tree_right),
                    tree_node_t *(tree_node_t::*p_child_left),
                    tree_node_t *(tree_node_t::*p_child_right)) &
                {
                    assert(distance != 0);
                    tree_node_t *tree_node = reinterpret_cast<tree_node_t *>(current_list_node);
                    std::size_t index_in_tree_node = 0, tree_node_range_size = 1;
                    index_in_tree_node += distance;
                    auto find_inside_tree_node = [&]()
                    {
                        while(tree_node_range_size != 1)
                        {
                            if(index_in_tree_node < tree_node_range_size / 2)
                                std::tie(index_in_tree_node, tree_node_range_size, tree_node) = std::forward_as_tuple(index_in_tree_node, tree_node_range_size / 2, tree_node->*p_child_left);
                            else
                                std::tie(index_in_tree_node, tree_node_range_size, tree_node) = std::forward_as_tuple(index_in_tree_node - tree_node_range_size / 2, tree_node_range_size / 2, tree_node->*p_child_right);
                        }
                        return p_tree_node_to_p_list_node(tree_node);
                    };
                    tree_node_t *tree_node_parent = static_cast<list_node_t *>(current_list_node)->parent;
                    while(!tagged_ptr_bit0_is_set(tree_node_parent))
                    {
                        if(tree_node == tagged_ptr_bit0_unsetted_relaxed(tree_node_parent->*p_child_left))
                        {
                            std::tie(index_in_tree_node, tree_node_range_size, tree_node) = std::forward_as_tuple(index_in_tree_node, tree_node_range_size * 2, std::exchange(tree_node_parent, tree_node_parent->parent));
                            if(index_in_tree_node < tree_node_range_size)
                            {
                                current_list_node = find_inside_tree_node();
                                return;
                            }
                        }
                        else if(tree_node == tagged_ptr_bit0_unsetted_relaxed(tree_node_parent->*p_child_right))
                            std::tie(index_in_tree_node, tree_node_range_size, tree_node) = std::forward_as_tuple(index_in_tree_node + tree_node_range_size, tree_node_range_size * 2, std::exchange(tree_node_parent, tree_node_parent->parent));
                        else std::unreachable();
                    }
                    digit_node_t *digit_node = p_tree_node_to_p_digit_node(tree_node_parent);
                    while(true)
                    {
                        // advance to next tree, return if found
                        if(tree_node == tagged_ptr_bit0_unsetted_relaxed(digit_node->*p_tree_left) && digit_node->*p_tree_right != nullptr)
                        {
                            std::tie(index_in_tree_node, tree_node_range_size, tree_node) = std::forward_as_tuple(index_in_tree_node - tree_node_range_size, tree_node_range_size, tagged_ptr_bit0_unsetted_relaxed(digit_node->*p_tree_right));
                            if(index_in_tree_node < tree_node_range_size)
                            {
                                current_list_node = tree_node_range_size == 1 ? reinterpret_cast<list_node_t *>(tree_node) : find_inside_tree_node();
                                return;
                            }
                        }
                        else if((tree_node == tagged_ptr_bit0_unsetted_relaxed(digit_node->*p_tree_left) && digit_node->*p_tree_right == nullptr) || tree_node == tagged_ptr_bit0_unsetted_relaxed(digit_node->*p_tree_right))
                        {
                            if(tagged_ptr_bit0_is_set(digit_node->*p_next))
                            {
                                list_node_end_t *list_node_end = static_cast<digit_node_end_t *>(tagged_ptr_bit0_unsetted(digit_node->*p_next))->list_node_end;
                                std::tie(index_in_tree_node, current_list_node) = std::forward_as_tuple(index_in_tree_node - tree_node_range_size, list_node_end);
                                index_in_tree_node %= static_cast<std::ptrdiff_t>(tagged_ptr_bit0_unsetted(list_node_end)->node_count) + 1;
                                if(index_in_tree_node != 0)
                                {
                                    std::tie(index_in_tree_node, current_list_node) = std::forward_as_tuple(index_in_tree_node - 1, (*p_untagged_next)(current_list_node));
                                    if(index_in_tree_node != 0)
                                        move_impl(index_in_tree_node, p_untagged_prev, p_untagged_next, p_prev, p_next, p_tree_left, p_tree_right, p_child_left, p_child_right);
                                }
                                return;
                            }
                            else
                            {
                                digit_node = static_cast<digit_node_t *>(digit_node->*p_next);
                                if(digit_node->*p_tree_left != nullptr)
                                    std::tie(index_in_tree_node, tree_node_range_size, tree_node) = std::forward_as_tuple(index_in_tree_node - tree_node_range_size, 1 << digit_node->digit_position, tagged_ptr_bit0_unsetted_relaxed(digit_node->*p_tree_left));
                                else if(digit_node->*p_tree_right != nullptr)
                                    std::tie(index_in_tree_node, tree_node_range_size, tree_node) = std::forward_as_tuple(index_in_tree_node - tree_node_range_size, 1 << digit_node->digit_position, tagged_ptr_bit0_unsetted_relaxed(digit_node->*p_tree_right));
                                else std::unreachable();
                                if(index_in_tree_node < tree_node_range_size)
                                {
                                    current_list_node = tree_node_range_size == 1 ? reinterpret_cast<list_node_t *>(tree_node) : find_inside_tree_node();
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
                    if(offset == 0) return static_cast<derived_t &>(*this);
                    if(tagged_ptr_bit0_is_set(current_list_node))
                    {
                        offset %= static_cast<std::ptrdiff_t>(static_cast<list_node_end_t *>(tagged_ptr_bit0_unsetted(current_list_node))->node_count) + 1;
                        if(offset == 0) return static_cast<derived_t &>(*this);
                        else if(offset > 0)
                        {
                            current_list_node = list_node_t::untagged_next(current_list_node);
                            --offset;
                        }
                        else if(offset < 0)
                        {
                            current_list_node = list_node_t::untagged_prev(current_list_node);
                            ++offset;
                        }
                        else std::unreachable();
                    }
                    if(offset == 0) return static_cast<derived_t &>(*this);
                    else if(offset > 0)
                        move_impl(offset,
                            &list_node_t::untagged_prev, &list_node_t::untagged_next,
                            &circular_doubly_linked_list_node_navigator_t::prev, &circular_doubly_linked_list_node_navigator_t::next, &digit_node_t::tree_left, &digit_node_t::tree_right, &tree_node_t::child_left, &tree_node_t::child_right);
                    else if(offset < 0)
                        move_impl(-offset,
                            &list_node_t::untagged_next, &list_node_t::untagged_prev,
                            &circular_doubly_linked_list_node_navigator_t::next, &circular_doubly_linked_list_node_navigator_t::prev, &digit_node_t::tree_right, &digit_node_t::tree_left, &tree_node_t::child_right, &tree_node_t::child_left);
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
                    conditional_const_t<is_const, typename demonstration_only_input_iterator_t::value_type> &operator[](std::ptrdiff_t offset) const & { return *(*this + offset); }
                };

                std::tuple<std::size_t, list_node_end_t *> index_impl() const &
                {
                    if(tagged_ptr_bit0_is_set(current_list_node))
                        return {static_cast<list_node_end_t *>(tagged_ptr_bit0_unsetted(current_list_node))->node_count, static_cast<list_node_end_t *>(current_list_node)};
                    else
                    {
                        std::size_t index_in_tree_node = 0, tree_node_range_size = 1;

                        tree_node_t *tree_node = reinterpret_cast<tree_node_t *>(current_list_node);
                        tree_node_t *tree_node_parent = static_cast<list_node_t *>(current_list_node)->parent;
                        while(!tagged_ptr_bit0_is_set(tree_node_parent))
                        {
                            if(tree_node == tagged_ptr_bit0_unsetted_relaxed(tree_node_parent->child_left))
                                std::tie(index_in_tree_node, tree_node_range_size, tree_node) = std::forward_as_tuple(index_in_tree_node, tree_node_range_size * 2, std::exchange(tree_node_parent, tree_node_parent->parent));
                            else if(tree_node == tagged_ptr_bit0_unsetted_relaxed(tree_node_parent->child_right))
                                std::tie(index_in_tree_node, tree_node_range_size, tree_node) = std::forward_as_tuple(index_in_tree_node + tree_node_range_size, tree_node_range_size * 2, std::exchange(tree_node_parent, tree_node_parent->parent));
                            else std::unreachable();
                        }
                        if(digit_node_t *digit_node = p_tree_node_to_p_digit_node(tree_node_parent);
                            tagged_ptr_bit0_is_set(digit_node->next) || static_cast<digit_node_t *>(digit_node->next)->digit_position < digit_node->digit_position)
                        {
                            if(tree_node == tagged_ptr_bit0_unsetted_relaxed(digit_node->tree_left) && digit_node->tree_right != nullptr)
                                std::tie(index_in_tree_node, tree_node_range_size) = std::forward_as_tuple(index_in_tree_node, tree_node_range_size + tree_node_range_size);
                            circular_doubly_linked_list_node_navigator_t *digit_node_navigator = digit_node->next;
                            for(; !tagged_ptr_bit0_is_set(digit_node_navigator); digit_node_navigator = digit_node_navigator->next)
                            {
                                digit_node = static_cast<digit_node_t *>(digit_node_navigator);
                                if(digit_node->tree_left != nullptr)
                                    std::tie(index_in_tree_node, tree_node_range_size) = std::forward_as_tuple(index_in_tree_node, tree_node_range_size + (1 << digit_node->digit_position));
                                if(digit_node->tree_right != nullptr)
                                    std::tie(index_in_tree_node, tree_node_range_size) = std::forward_as_tuple(index_in_tree_node, tree_node_range_size + (1 << digit_node->digit_position));
                            }
                            return {tagged_ptr_bit0_unsetted(static_cast<digit_node_end_t *>(tagged_ptr_bit0_unsetted(digit_node_navigator))->list_node_end)->node_count - (tree_node_range_size - index_in_tree_node), static_cast<digit_node_end_t *>(tagged_ptr_bit0_unsetted(digit_node_navigator))->list_node_end};
                        }
                        else
                        {
                            if(digit_node->tree_left != nullptr && tree_node == tagged_ptr_bit0_unsetted_relaxed(digit_node->tree_right))
                                std::tie(index_in_tree_node, tree_node_range_size) = std::forward_as_tuple(tree_node_range_size + index_in_tree_node, tree_node_range_size + tree_node_range_size);
                            circular_doubly_linked_list_node_navigator_t *digit_node_navigator = digit_node->prev;
                            for(; !tagged_ptr_bit0_is_set(digit_node_navigator); digit_node_navigator = digit_node_navigator->prev)
                            {
                                digit_node = static_cast<digit_node_t *>(digit_node_navigator);
                                if(digit_node->tree_right != nullptr)
                                    std::tie(index_in_tree_node, tree_node_range_size) = std::forward_as_tuple(index_in_tree_node + (1 << digit_node->digit_position), tree_node_range_size + (1 << digit_node->digit_position));
                                if(digit_node->tree_left != nullptr)
                                    std::tie(index_in_tree_node, tree_node_range_size) = std::forward_as_tuple(index_in_tree_node + (1 << digit_node->digit_position), tree_node_range_size + (1 << digit_node->digit_position));
                            }
                            return {index_in_tree_node, static_cast<digit_node_end_t *>(tagged_ptr_bit0_unsetted(digit_node_navigator))->list_node_end};
                        }
                    }
                }
                std::size_t index() const &
                {
                    assert(current_list_node != nullptr);
                    return std::get<0>(index_impl());
                }
                template<std::bool_constant<is_const> * = nullptr>
                    requires(is_const)
                friend std::ptrdiff_t operator-(const_iterator_t const &lhs, const_iterator_t const &rhs)
                {
                    assert(lhs.current_list_node != nullptr);
                    assert(rhs.current_list_node != nullptr);
                    return static_cast<std::ptrdiff_t>(std::get<0>(lhs.index_impl())) - static_cast<std::ptrdiff_t>(std::get<0>(rhs.index_impl()));
                }
                template<std::bool_constant<is_const> * = nullptr>
                    requires(!is_const)
                friend std::ptrdiff_t operator-(const_iterator_t const &lhs, non_const_iterator_t const &rhs)
                {
                    return lhs - rhs.to_const();
                }
                template<std::bool_constant<is_const> * = nullptr>
                    requires(!is_const)
                friend std::ptrdiff_t operator-(non_const_iterator_t const &lhs, const_iterator_t const &rhs)
                {
                    return lhs.to_const() - rhs;
                }
                template<std::bool_constant<is_const> * = nullptr>
                    requires(!is_const)
                friend std::ptrdiff_t operator-(non_const_iterator_t const &lhs, non_const_iterator_t const &rhs)
                {
                    return lhs.to_const() - rhs.to_const();
                }
                friend std::ptrdiff_t operator-([[maybe_unused]] std::default_sentinel_t const &lhs, derived_t const &rhs)
                {
                    assert(rhs.current_list_node != nullptr);
                    auto [index, list_node_end] = rhs.index_impl();
                    return static_cast<std::ptrdiff_t>(tagged_ptr_bit0_unsetted(list_node_end)->node_count) - static_cast<std::ptrdiff_t>(index);
                }
                friend std::ptrdiff_t operator-(derived_t const &lhs, [[maybe_unused]] std::default_sentinel_t const &rhs) { return -(rhs - lhs); }

                using iterator_concept = std::random_access_iterator_tag;
            };

            template<bool is_const, typename sequence_config_t>
            struct iterator_element_t: iterator_list_node_t<is_const, sequence_config_t, iterator_element_t<is_const, sequence_config_t>>
            {
                using base_t = iterator_list_node_t<is_const, sequence_config_t, iterator_element_t<is_const, sequence_config_t>>;
                using list_node_t = typename base_t::list_node_t;

                using base_t::base_t;
                using base_t::operator=;

                // std::input_iterator / std::indirectly_readable
                using value_type = typename sequence_config_t::element_t;
                conditional_const_t<is_const, value_type> &operator*() const &
                {
                    assert(this->current_list_node != nullptr);
                    assert(!tagged_ptr_bit0_is_set(this->current_list_node));
                    return conditional_as_const<is_const>(*static_cast<list_node_t *>(this->current_list_node)->actual_projected_storage.p_element());
                }
                conditional_const_t<is_const, value_type> *to_address() const & { return &operator*(); }
                conditional_const_t<is_const, value_type> *operator->() const & { return &operator*(); }
                static iterator_element_t from_element_address(conditional_const_t<is_const, value_type> *address) { return {reinterpret_cast<list_node_t *>(const_cast<std::byte *>(reinterpret_cast<conditional_const_t<is_const, std::byte> *>(address)) - offsetof(list_node_t, actual_projected_storage.element_buffer))}; }

                // std::random_access_iterator
                conditional_const_t<is_const, value_type> &operator[](std::ptrdiff_t offset) const & { return *(*this + offset); }
            };
            template<bool is_const, typename sequence_config_t>
            struct const_iterator<iterator_element_t<is_const, sequence_config_t>>: std::type_identity<iterator_element_t<true, sequence_config_t>>
            {
            };
            template<bool is_const, typename sequence_config_t>
            struct non_const_iterator<iterator_element_t<is_const, sequence_config_t>>: std::type_identity<iterator_element_t<false, sequence_config_t>>
            {
            };

            template<bool is_const, typename sequence_config_t>
            struct iterator_projected_storage_t: iterator_list_node_t<is_const, sequence_config_t, iterator_projected_storage_t<is_const, sequence_config_t>>
            {
                using base_t = iterator_list_node_t<is_const, sequence_config_t, iterator_projected_storage_t<is_const, sequence_config_t>>;
                using list_node_t = typename base_t::list_node_t;

                using base_t::base_t;
                using base_t::operator=;

                // std::input_iterator / std::indirectly_readable
                using value_type = std::conditional_t<std::is_same_v<typename sequence_config_t::projected_storage_t, void>, std::ranges::dangling, typename sequence_config_t::projected_storage_t>;
                conditional_const_t<is_const, value_type> &operator*() const &
                {
                    assert(this->current_list_node != nullptr);
                    assert(!tagged_ptr_bit0_is_set(this->current_list_node));
                    if constexpr(std::is_same_v<typename sequence_config_t::projected_storage_t, void>)
                    {
                        static std::ranges::dangling s;
                        return s;
                    }
                    else
                        return conditional_as_const<is_const>(*static_cast<list_node_t *>(this->current_list_node)->actual_projected_storage.p_projected_storage());
                }
                conditional_const_t<is_const, value_type> *to_address() const & { return &operator*(); }
                conditional_const_t<is_const, value_type> *operator->() const & { return &operator*(); }
                static iterator_projected_storage_t from_projected_storage_address(conditional_const_t<is_const, value_type> *address)
                {
                    if constexpr(std::is_same_v<typename sequence_config_t::projected_storage_t, void>)
                        return {};
                    else
                        return {reinterpret_cast<list_node_t *>(const_cast<std::byte *>(reinterpret_cast<conditional_const_t<is_const, std::byte> *>(address)) - offsetof(list_node_t, actual_projected_storage.projected_storage_buffer))};
                }

                // std::random_access_iterator
                conditional_const_t<is_const, value_type> &operator[](std::ptrdiff_t offset) const & { return *(*this + offset); }
            };
            template<bool is_const, typename sequence_config_t>
            struct const_iterator<iterator_projected_storage_t<is_const, sequence_config_t>>: std::type_identity<iterator_projected_storage_t<true, sequence_config_t>>
            {
            };
            template<bool is_const, typename sequence_config_t>
            struct non_const_iterator<iterator_projected_storage_t<is_const, sequence_config_t>>: std::type_identity<iterator_projected_storage_t<false, sequence_config_t>>
            {
            };

            constexpr inline struct tuple_identity_functor_t
            {
                template<typename tuple_t>
                auto operator()(tuple_t tuple) const { return tuple; }
            } tuple_identity;
            constexpr inline struct tuple_reversed_functor_t
            {
                template<typename tuple_t>
                auto operator()(tuple_t tuple) const
                {
                    return [&]<std::size_t... I>(std::index_sequence<I...>) { return std::tuple<std::tuple_element_t<std::tuple_size_v<tuple_t> - 1 - I, tuple_t>...>(std::get<std::tuple_size_v<tuple_t> - 1 - I>(tuple)...); }
                    (std::make_index_sequence<std::tuple_size_v<tuple_t>>());
                }
            } tuple_reversed;

            template<typename sequence_possibly_const_t, typename sequence_t, typename tuple_identity_or_reversed_functor_t>
            struct sequence_push_or_pop_impl_t
            {
                sequence_possibly_const_t *sequence;
                typename sequence_t::digit_proxy_t (sequence_t::*p_digit_front)(void);
                typename sequence_t::digit_proxy_t (sequence_t::*p_digit_back)(void);
                circular_doubly_linked_list_node_navigator_t *(circular_doubly_linked_list_node_navigator_t::*p_prev);
                circular_doubly_linked_list_node_navigator_t *(circular_doubly_linked_list_node_navigator_t::*p_next);
                typename sequence_t::tree_node_t *(sequence_t::digit_node_t::*p_tree_left);
                typename sequence_t::tree_node_t *(sequence_t::digit_node_t::*p_tree_right);
                typename sequence_t::tree_node_t *(sequence_t::tree_node_t::*p_child_left);
                typename sequence_t::tree_node_t *(sequence_t::tree_node_t::*p_child_right);
                tuple_identity_or_reversed_functor_t tuple_identity_or_reversed;

                using digit_node_t = typename sequence_t::digit_node_t;
                using tree_node_t = typename sequence_t::tree_node_t;
                using list_node_t = typename sequence_t::list_node_t;
                using list_node_end_t = typename sequence_t::list_node_end_t;
                using projected_storage_t = typename sequence_t::projected_storage_t;
                using accumulated_storage_t = typename sequence_t::accumulated_storage_t;

                static list_node_t *p_tree_node_to_p_list_node(tree_node_t *p) { return reinterpret_cast<list_node_t *>(tagged_ptr_bit0_unsetted(p)); }
                static tree_node_t *p_list_node_to_p_tree_node(list_node_t *p) { return reinterpret_cast<tree_node_t *>(tagged_ptr_bit0_setted(p)); }
                static tree_node_t *p_digit_node_to_p_tree_node(digit_node_t *p) { return reinterpret_cast<tree_node_t *>(tagged_ptr_bit0_setted(p)); }
                static digit_node_t *p_tree_node_to_p_digit_node(tree_node_t *p) { return reinterpret_cast<digit_node_t *>(tagged_ptr_bit0_unsetted(p)); }

                struct next_or_prev_impl_t
                {
                    circular_doubly_linked_list_node_navigator_t *(circular_doubly_linked_list_node_navigator_t::*p_next);
                    friend digit_node_t *operator->*(digit_node_t *digit_node, next_or_prev_impl_t const &next)
                    {
                        return static_cast<digit_node_t *>(digit_node->*next.p_next);
                    }
                };

                void construct_list_node_projected_storage_if_exists(list_node_t *list_node) const
                {
                    if constexpr(sequence_t::requested_stride == 1)
                    {
                        if constexpr(!std::is_same_v<projected_storage_t, void>)
                            sequence->projector_and_accumulator().construct_projected_storage(list_node->actual_projected_storage.p_projected_storage(), typename sequence_t::iterator_projected_storage_t{list_node}, typename sequence_t::iterator_element_t{list_node});
                    }
                    else
                    {
                        static_assert(!std::is_same_v<projected_storage_t, void>);
                        typename sequence_t::iterator_projected_storage_t it_list_node{list_node}, it_list_node_next =
#ifdef __EMSCRIPTEN__
                                                                                                       std::next
#else
                                                                                                       std::ranges::next
#endif
                            (it_list_node);
                        typename sequence_t::stride1_sequence_t::iterator_element_t it_chunk_begin{list_node->actual_projected_storage.child},
                            it_chunk_end{it_list_node_next == sequence->end_projected_storage() ? static_cast<circular_doubly_linked_list_node_navigator_t *>(static_cast<list_node_end_t *>(tagged_ptr_bit0_unsetted(it_list_node_next.current_list_node))->actual_projected_storage_end.child) : static_cast<circular_doubly_linked_list_node_navigator_t *>(static_cast<list_node_t *>(it_list_node_next.current_list_node)->actual_projected_storage.child)};
                        std::size_t chunk_size = 1;
                        assert(it_chunk_end - it_chunk_begin == chunk_size);
                        sequence->projector_and_accumulator().construct_projected_storage(list_node->actual_projected_storage.p_projected_storage(), it_list_node, it_chunk_begin, it_chunk_end, chunk_size);
                    }
                }
                void update_list_node_projected_storage_if_exists(list_node_t *list_node) const
                {
                    if constexpr(sequence_t::requested_stride == 1)
                    {
                        if constexpr(!std::is_same_v<projected_storage_t, void>)
                            sequence->projector_and_accumulator().update_projected_storage(*list_node->actual_projected_storage.p_projected_storage(), typename sequence_t::iterator_projected_storage_t{list_node}, typename sequence_t::iterator_element_t{list_node});
                    }
                    else
                    {
                        static_assert(!std::is_same_v<projected_storage_t, void>);
                        typename sequence_t::iterator_projected_storage_t it_list_node{list_node}, it_list_node_next =
#ifdef __EMSCRIPTEN__
                                                                                                       std::next
#else
                                                                                                       std::ranges::next
#endif
                            (it_list_node);
                        typename sequence_t::stride1_sequence_t::iterator_element_t it_chunk_begin{list_node->actual_projected_storage.child},
                            it_chunk_end{it_list_node_next == sequence->end_projected_storage() ? static_cast<circular_doubly_linked_list_node_navigator_t *>(static_cast<list_node_end_t *>(tagged_ptr_bit0_unsetted(it_list_node_next.current_list_node))->actual_projected_storage_end.child) : static_cast<circular_doubly_linked_list_node_navigator_t *>(static_cast<list_node_t *>(it_list_node_next.current_list_node)->actual_projected_storage.child)};
                        std::size_t chunk_size;
                        if(it_list_node == sequence->begin_projected_storage())
                            chunk_size = sequence->list_front_element_count();
                        else if(it_list_node_next == sequence->end_projected_storage())
                            chunk_size = sequence->list_back_element_count();
                        else
                            chunk_size = sequence->stride;
                        assert(it_chunk_end - it_chunk_begin == static_cast<std::ptrdiff_t>(chunk_size));
                        sequence->projector_and_accumulator().update_projected_storage(*list_node->actual_projected_storage.p_projected_storage(), it_list_node, it_chunk_begin, it_chunk_end, chunk_size);
                    }
                }
                auto p_element_or_p_projected_storage_from_list_node(list_node_t *list_node) const
                {
                    if constexpr(sequence_t::requested_stride == 1)
                    {
                        if constexpr(!std::is_same_v<projected_storage_t, void>)
                            return list_node->actual_projected_storage.p_projected_storage();
                        else
                            return list_node->actual_projected_storage.p_element();
                    }
                    else
                    {
                        static_assert(!std::is_same_v<projected_storage_t, void>);
                        return list_node->actual_projected_storage.p_projected_storage();
                    }
                }
                void update_or_construct_tree_node_accumulated_storage_if_exists(tree_node_t *tree_node, bool construct = false) const
                {
                    if constexpr(!std::is_same_v<accumulated_storage_t, void>)
                    {
                        auto get_left_operand = [&](auto return_accumulated_tuple)
                        { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> void
                          {
                              if(tagged_ptr_bit0_is_set(tree_node->*p_child_left))
                                  return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::cref(*p_element_or_p_projected_storage_from_list_node(p_tree_node_to_p_list_node(tree_node->*p_child_left))))));
                              else
                                  return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*(tree_node->*p_child_left)->p_accumulated_storage()))));
                          }; };
                        auto get_right_operand = [&](auto return_accumulated_tuple)
                        { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> void
                          {
                              if(tagged_ptr_bit0_is_set(tree_node->*p_child_right))
                                  return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::cref(*p_element_or_p_projected_storage_from_list_node(p_tree_node_to_p_list_node(tree_node->*p_child_right))))));
                              else
                                  return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*(tree_node->*p_child_right)->p_accumulated_storage()))));
                          }; };
                        auto return_accumulated_tuple = [&](auto accumulated_tuple_so_far)
                        {
                            if(construct)
                                sequence->projector_and_accumulator().construct_accumulated_storage(tree_node->p_accumulated_storage(), tuple_identity_or_reversed(accumulated_tuple_so_far));
                            else
                                sequence->projector_and_accumulator().update_accumulated_storage(*tree_node->p_accumulated_storage(), tuple_identity_or_reversed(accumulated_tuple_so_far));
                        };
                        get_left_operand(get_right_operand(return_accumulated_tuple))(std::make_tuple());
                    }
                }
                void update_digit_node_end_accumulated_storage_if_exists(digit_node_t *digit_front, digit_node_t *digit_back) const
                {
                    if constexpr(!std::is_same_v<accumulated_storage_t, void>)
                    {
                        if(sequence->digit_middle() == tagged_ptr_bit0_unsetted(sequence->digit_node_end))
                            sequence->projector_and_accumulator().update_accumulated_storage(*tagged_ptr_bit0_unsetted(sequence->digit_node_end)->p_accumulated_storage(), std::make_tuple());
                        else
                        {
                            auto get_left_operand = [&](auto return_accumulated_tuple)
                            { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> void
                              {
                                  if(digit_front != sequence->digit_middle())
                                      return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*digit_front->p_accumulated_storage()))));
                                  else
                                  {
                                      if(digit_front->*p_tree_left != nullptr)
                                      {
                                          if(digit_front->digit_position != 0)
                                              return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*(digit_front->*p_tree_left)->p_accumulated_storage()))));
                                          else
                                              return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::cref(*p_element_or_p_projected_storage_from_list_node(p_tree_node_to_p_list_node(digit_front->*p_tree_left))))));
                                      }
                                      else
                                          return_accumulated_tuple(accumulated_tuple_so_far);
                                  }
                              }; };
                            auto get_right_operand = [&](auto return_accumulated_tuple)
                            { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> void
                              {
                                  if(sequence->digit_middle() != digit_back)
                                      return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*digit_back->p_accumulated_storage()))));
                                  else
                                  {
                                      if(digit_back->*p_tree_right != nullptr)
                                      {
                                          if(digit_back->digit_position != 0)
                                              return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*(digit_back->*p_tree_right)->p_accumulated_storage()))));
                                          else
                                              return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::cref(*p_element_or_p_projected_storage_from_list_node(p_tree_node_to_p_list_node(digit_back->*p_tree_right))))));
                                      }
                                      else
                                          return_accumulated_tuple(accumulated_tuple_so_far);
                                  }
                              }; };
                            auto return_accumulated_tuple = [&](auto accumulated_tuple_so_far)
                            { sequence->projector_and_accumulator().update_accumulated_storage(*tagged_ptr_bit0_unsetted(sequence->digit_node_end)->p_accumulated_storage(), tuple_identity_or_reversed(accumulated_tuple_so_far)); };
                            get_left_operand(get_right_operand(return_accumulated_tuple))(std::make_tuple());
                        }
                    }
                }
                void update_or_construct_digit_node_right_accumulated_storage_if_exists(digit_node_t *digit_node_right, bool construct = false) const
                {
                    if constexpr(!std::is_same_v<accumulated_storage_t, void>)
                    {
                        auto get_left_operand = [&](auto return_accumulated_tuple)
                        { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> void
                          {
                              next_or_prev_impl_t next{p_next}, prev{p_prev};
                              if(sequence->digit_middle() != digit_node_right->*p_prev)
                                  return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*(digit_node_right->*prev)->p_accumulated_storage()))));
                              else
                              {
                                  if(digit_node_right->*prev->*p_tree_right != nullptr)
                                      return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*(digit_node_right->*prev->*p_tree_right)->p_accumulated_storage()))));
                                  else
                                      return_accumulated_tuple(accumulated_tuple_so_far);
                              }
                          }; };
                        auto get_middle_operand = [&](auto return_accumulated_tuple)
                        { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> void
                          {
                              if(digit_node_right->*p_tree_left != nullptr)
                              {
                                  if(digit_node_right->digit_position != 0)
                                      return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*(digit_node_right->*p_tree_left)->p_accumulated_storage()))));
                                  else
                                      return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::cref(*p_element_or_p_projected_storage_from_list_node(p_tree_node_to_p_list_node(digit_node_right->*p_tree_left))))));
                              }
                              else
                                  return_accumulated_tuple(accumulated_tuple_so_far);
                          }; };
                        auto get_right_operand = [&](auto return_accumulated_tuple)
                        { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> void
                          {
                              if(digit_node_right->*p_tree_right != nullptr)
                              {
                                  if(digit_node_right->digit_position != 0)
                                      return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*(digit_node_right->*p_tree_right)->p_accumulated_storage()))));
                                  else
                                      return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::cref(*p_element_or_p_projected_storage_from_list_node(p_tree_node_to_p_list_node(digit_node_right->*p_tree_right))))));
                              }
                              else
                                  return_accumulated_tuple(accumulated_tuple_so_far);
                          }; };
                        auto return_accumulated_tuple = [&](auto accumulated_tuple_so_far)
                        {
                            if(construct)
                                sequence->projector_and_accumulator().construct_accumulated_storage(digit_node_right->p_accumulated_storage(), tuple_identity_or_reversed(accumulated_tuple_so_far));
                            else
                                sequence->projector_and_accumulator().update_accumulated_storage(*digit_node_right->p_accumulated_storage(), tuple_identity_or_reversed(accumulated_tuple_so_far));
                        };
                        get_left_operand(get_middle_operand(get_right_operand(return_accumulated_tuple)))(std::make_tuple());
                    }
                }
                void update_digit_node_left_accumulated_storage_if_exists(digit_node_t *digit_node_left) const
                {
                    if constexpr(!std::is_same_v<accumulated_storage_t, void>)
                    {
                        auto get_left_operand = [&](auto return_accumulated_tuple)
                        { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> void
                          {
                              if(digit_node_left->*p_tree_left != nullptr)
                              {
                                  if(digit_node_left->digit_position != 0)
                                      return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*(digit_node_left->*p_tree_left)->p_accumulated_storage()))));
                                  else
                                      return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::cref(*p_element_or_p_projected_storage_from_list_node(p_tree_node_to_p_list_node(digit_node_left->*p_tree_left))))));
                              }
                              else
                                  return_accumulated_tuple(accumulated_tuple_so_far);
                          }; };
                        auto get_middle_operand = [&](auto return_accumulated_tuple)
                        { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> void
                          {
                              if(digit_node_left->*p_tree_right != nullptr)
                              {
                                  if(digit_node_left->digit_position != 0)
                                      return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*(digit_node_left->*p_tree_right)->p_accumulated_storage()))));
                                  else
                                      return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::cref(*p_element_or_p_projected_storage_from_list_node(p_tree_node_to_p_list_node(digit_node_left->*p_tree_right))))));
                              }
                              else
                                  return_accumulated_tuple(accumulated_tuple_so_far);
                          }; };
                        auto get_right_operand = [&](auto return_accumulated_tuple)
                        { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far) -> void
                          {
                              next_or_prev_impl_t next{p_next}, prev{p_prev};
                              if(sequence->digit_middle() != digit_node_left->*p_next)
                                  return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*(digit_node_left->*next)->p_accumulated_storage()))));
                              else
                              {
                                  if(digit_node_left->*next->*p_tree_left != nullptr)
                                      return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(*(digit_node_left->*next->*p_tree_left)->p_accumulated_storage()))));
                                  else
                                      return_accumulated_tuple(accumulated_tuple_so_far);
                              }
                          }; };
                        auto return_accumulated_tuple = [&](auto accumulated_tuple_so_far)
                        { sequence->projector_and_accumulator().update_accumulated_storage(*digit_node_left->p_accumulated_storage(), tuple_identity_or_reversed(accumulated_tuple_so_far)); };
                        get_left_operand(get_middle_operand(get_right_operand(return_accumulated_tuple)))(std::make_tuple());
                    }
                }
                void push_impl(invocable_r<typename sequence_t::list_node_t *, typename sequence_t::tree_node_t * /*parent*/> auto push_back_and_get_p_list_node) const
                {
                    next_or_prev_impl_t next{p_next}, prev{p_prev};
                    digit_node_t *digit_back = static_cast<digit_node_t *>(static_cast<circular_doubly_linked_list_node_navigator_t *>((sequence->*p_digit_back)()));
                    digit_node_t *digit_front = static_cast<digit_node_t *>(static_cast<circular_doubly_linked_list_node_navigator_t *>((sequence->*p_digit_front)()));
                    {
                        if(sequence->empty()) // 0 -> 1
                        {
                            assert(digit_front == static_cast<circular_doubly_linked_list_node_navigator_t *>(tagged_ptr_bit0_unsetted(sequence->digit_node_end)));
                            assert(sequence->digit_middle() == tagged_ptr_bit0_unsetted(sequence->digit_node_end));
                            assert(digit_back == static_cast<circular_doubly_linked_list_node_navigator_t *>(tagged_ptr_bit0_unsetted(sequence->digit_node_end)));

                            sequence->digit_middle() = digit_front = digit_back = new digit_node_t(0);
                            circular_doubly_linked_list_node_navigator_t::push_impl(sequence->digit_node_end, p_prev, p_next, digit_back);

                            list_node_t *list_node_back;
                            digit_back->*p_tree_left = p_list_node_to_p_tree_node(list_node_back = push_back_and_get_p_list_node(p_digit_node_to_p_tree_node(digit_back)));

                            construct_list_node_projected_storage_if_exists(list_node_back);
                            // digit_node_back_accumulated_storage update skipped because it's digit_middle
                            update_digit_node_end_accumulated_storage_if_exists(digit_front, digit_back);
                        }
                        else
                        {
                            if(digit_back->digit_position != 0) // .+0 -> .+1
                            {
                                digit_back = new digit_node_t(0);
                                circular_doubly_linked_list_node_navigator_t::push_impl(sequence->digit_node_end, p_prev, p_next, digit_back);

                                list_node_t *list_node_back;
                                digit_back->*p_tree_left = p_list_node_to_p_tree_node(list_node_back = push_back_and_get_p_list_node(p_digit_node_to_p_tree_node(digit_back)));

                                construct_list_node_projected_storage_if_exists(list_node_back);
                                update_or_construct_digit_node_right_accumulated_storage_if_exists(digit_back, true);
                                update_digit_node_end_accumulated_storage_if_exists(digit_front, digit_back);
                            }
                            else
                            {
                                if(digit_back->*p_tree_right == nullptr) // .*1 -> .*2
                                {
                                    list_node_t *list_node_back;
                                    digit_back->*p_tree_right = p_list_node_to_p_tree_node(list_node_back = push_back_and_get_p_list_node(p_digit_node_to_p_tree_node(digit_back)));

                                    construct_list_node_projected_storage_if_exists(list_node_back);
                                    if(sequence->digit_middle() != digit_back)
                                        update_or_construct_digit_node_right_accumulated_storage_if_exists(digit_back);
                                    update_digit_node_end_accumulated_storage_if_exists(digit_front, digit_back);
                                }
                                else if(digit_back->*p_tree_left == nullptr) // .*1 -> .*2
                                {
                                    list_node_t *list_node_back;
                                    digit_back->*p_tree_left = std::exchange(digit_back->*p_tree_right, p_list_node_to_p_tree_node(list_node_back = push_back_and_get_p_list_node(p_digit_node_to_p_tree_node(digit_back))));

                                    construct_list_node_projected_storage_if_exists(list_node_back);
                                    if(sequence->digit_middle() != digit_back)
                                        update_or_construct_digit_node_right_accumulated_storage_if_exists(digit_back);
                                    update_digit_node_end_accumulated_storage_if_exists(digit_front, digit_back);
                                }
                                else // .*[01]2+ -> .*[12]1+
                                {
                                    digit_back = new digit_node_t(0);
                                    circular_doubly_linked_list_node_navigator_t::push_impl(sequence->digit_node_end, p_prev, p_next, digit_back);

                                    list_node_t *list_node_back;
                                    digit_back->*p_tree_left = p_list_node_to_p_tree_node(list_node_back = push_back_and_get_p_list_node(p_digit_node_to_p_tree_node(digit_back)));
                                    construct_list_node_projected_storage_if_exists(list_node_back);
                                    digit_node_t *p_digit_node_whose_digit_position_is_to_be_raised = digit_back->*prev;
                                    bool should_update_left_accumulated_storage, should_update_self_accumulated_storage, should_update_right_accumulated_storage;
                                    while(true)
                                    {
                                        tree_node_t *tree_root = new tree_node_t{.parent = p_digit_node_to_p_tree_node(p_digit_node_whose_digit_position_is_to_be_raised)};
                                        tree_root->*p_child_left = p_digit_node_whose_digit_position_is_to_be_raised->*p_tree_left;
                                        tree_root->*p_child_right = p_digit_node_whose_digit_position_is_to_be_raised->*p_tree_right;
                                        if(p_digit_node_whose_digit_position_is_to_be_raised->digit_position == 0)
                                            p_tree_node_to_p_list_node(tree_root->*p_child_left)->parent = p_tree_node_to_p_list_node(tree_root->*p_child_right)->parent = tree_root;
                                        else
                                            (tree_root->*p_child_left)->parent = (tree_root->*p_child_right)->parent = tree_root;
                                        update_or_construct_tree_node_accumulated_storage_if_exists(tree_root, true);
                                        p_digit_node_whose_digit_position_is_to_be_raised->*p_tree_left = tree_root;
                                        p_digit_node_whose_digit_position_is_to_be_raised->*p_tree_right = nullptr;
                                        ++p_digit_node_whose_digit_position_is_to_be_raised->digit_position;
                                        if(p_digit_node_whose_digit_position_is_to_be_raised == sequence->digit_middle())
                                        {
                                            should_update_left_accumulated_storage = true, should_update_self_accumulated_storage = false, should_update_right_accumulated_storage = true;
                                            break;
                                        }
                                        assert(p_digit_node_whose_digit_position_is_to_be_raised->*p_prev != sequence->digit_node_end);
                                        if((p_digit_node_whose_digit_position_is_to_be_raised->*prev)->digit_position != p_digit_node_whose_digit_position_is_to_be_raised->digit_position)
                                        {
                                            should_update_left_accumulated_storage = false, should_update_self_accumulated_storage = true, should_update_right_accumulated_storage = true;
                                            break;
                                        }
                                        if(int digit = (p_digit_node_whose_digit_position_is_to_be_raised->*prev->*p_tree_left != nullptr) + (p_digit_node_whose_digit_position_is_to_be_raised->*prev->*p_tree_right != nullptr); digit == 2)
                                            p_digit_node_whose_digit_position_is_to_be_raised = p_digit_node_whose_digit_position_is_to_be_raised->*prev;
                                        else if(digit == 1) // .*12+ -> .*21+
                                        {
                                            should_update_self_accumulated_storage = (p_digit_node_whose_digit_position_is_to_be_raised->*p_prev != sequence->digit_middle()), should_update_right_accumulated_storage = true;
                                            if(p_digit_node_whose_digit_position_is_to_be_raised->*prev->*p_tree_right == nullptr)
                                            {
                                                should_update_left_accumulated_storage = false;
                                                p_digit_node_whose_digit_position_is_to_be_raised->*prev->*p_tree_right = tree_root;
                                            }
                                            else if(p_digit_node_whose_digit_position_is_to_be_raised->*prev->*p_tree_left == nullptr)
                                            {
                                                should_update_left_accumulated_storage = sequence->digit_middle() == p_digit_node_whose_digit_position_is_to_be_raised->*prev;
                                                p_digit_node_whose_digit_position_is_to_be_raised->*prev->*p_tree_left = std::exchange(p_digit_node_whose_digit_position_is_to_be_raised->*prev->*p_tree_right, tree_root);
                                            }
                                            else std::unreachable();
                                            tree_root->parent = p_digit_node_to_p_tree_node(p_digit_node_whose_digit_position_is_to_be_raised->*prev);
                                            circular_doubly_linked_list_node_navigator_t::extract_impl(p_digit_node_whose_digit_position_is_to_be_raised, p_prev, p_next);
                                            delete std::exchange(p_digit_node_whose_digit_position_is_to_be_raised, p_digit_node_whose_digit_position_is_to_be_raised->*prev);
                                            break;
                                        }
                                        else std::unreachable();
                                    }
                                    if(should_update_self_accumulated_storage)
                                        update_or_construct_digit_node_right_accumulated_storage_if_exists(p_digit_node_whose_digit_position_is_to_be_raised);
                                    if(should_update_left_accumulated_storage)
                                    {
                                        for(circular_doubly_linked_list_node_navigator_t *digit_node_navigator = p_digit_node_whose_digit_position_is_to_be_raised->*p_prev; digit_node_navigator != sequence->digit_node_end; digit_node_navigator = digit_node_navigator->*p_prev)
                                        {
                                            digit_node_t *digit_node_left = static_cast<digit_node_t *>(digit_node_navigator);
                                            update_digit_node_left_accumulated_storage_if_exists(digit_node_left);
                                        }
                                    }
                                    if(should_update_right_accumulated_storage)
                                    {
                                        for(circular_doubly_linked_list_node_navigator_t *digit_node_navigator = p_digit_node_whose_digit_position_is_to_be_raised->*p_next; digit_node_navigator != sequence->digit_node_end; digit_node_navigator = digit_node_navigator->*p_next)
                                        {
                                            digit_node_t *digit_node_right = static_cast<digit_node_t *>(digit_node_navigator);
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
                    assert(!sequence->empty()); // 0
                    pop_list_node();
                    digit_node_t * const digit_back_const = static_cast<digit_node_t *>(static_cast<circular_doubly_linked_list_node_navigator_t *>((sequence->*p_digit_back)()));
                    digit_node_t * const digit_front_const = static_cast<digit_node_t *>(static_cast<circular_doubly_linked_list_node_navigator_t *>((sequence->*p_digit_front)()));
                    if(digit_back_const->digit_position == 0) // .*[12] -> .*[01]
                    {
                        auto delete_digit_position0 = [&]()
                        {
                            if(digit_back_const == sequence->digit_middle())
                            {
                                assert(digit_front_const == sequence->digit_middle());
                                assert(digit_back_const->*p_prev == sequence->digit_node_end);
                                assert(digit_back_const->*p_next == sequence->digit_node_end);
                                assert(tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(sequence->digit_node_end)->*p_prev) == digit_back_const);
                                assert(tagged_ptr_bit0_unsetted(tagged_ptr_bit0_unsetted(sequence->digit_node_end)->*p_next) == digit_back_const);
                                circular_doubly_linked_list_node_navigator_t::extract_impl(digit_back_const, p_prev, p_next);
                                delete digit_back_const;
                                sequence->digit_middle() = (sequence->*p_digit_front)() = (sequence->*p_digit_back)() = tagged_ptr_bit0_unsetted(sequence->digit_node_end);
                            }
                            else
                            {
                                (sequence->*p_digit_back)() = digit_back_const->*prev;
                                circular_doubly_linked_list_node_navigator_t::extract_impl(digit_back_const, p_prev, p_next);
                                delete digit_back_const;
                            }
                        };
                        if(digit_back_const->*p_tree_right == nullptr || digit_back_const->*p_tree_left == nullptr)
                            delete_digit_position0();
                        else
                        {
                            digit_back_const->*p_tree_right = nullptr;
                            if(sequence->digit_middle() != digit_back_const)
                                update_or_construct_digit_node_right_accumulated_storage_if_exists(digit_back_const);
                        }
                        update_digit_node_end_accumulated_storage_if_exists(static_cast<digit_node_t *>(static_cast<circular_doubly_linked_list_node_navigator_t *>((sequence->*p_digit_front)())), static_cast<digit_node_t *>(static_cast<circular_doubly_linked_list_node_navigator_t *>((sequence->*p_digit_back)())));
                    }
                    else // .*[12]0+ -> .*[01]1+
                    {
                        digit_node_t *digit_back = static_cast<digit_node_t *>(static_cast<circular_doubly_linked_list_node_navigator_t *>((sequence->*p_digit_back)()));
                        digit_node_t *digit_front = static_cast<digit_node_t *>(static_cast<circular_doubly_linked_list_node_navigator_t *>((sequence->*p_digit_front)()));
                        tree_node_t *p_tree_to_be_broken_down;
                        bool will_delete_digit_back;
                        if(digit_back->*p_tree_right == nullptr)
                            std::tie(will_delete_digit_back, p_tree_to_be_broken_down) = std::forward_as_tuple(true, digit_back->*p_tree_left);
                        else if(digit_back->*p_tree_left == nullptr)
                            std::tie(will_delete_digit_back, p_tree_to_be_broken_down) = std::forward_as_tuple(true, digit_back->*p_tree_right);
                        else
                            std::tie(will_delete_digit_back, p_tree_to_be_broken_down) = std::forward_as_tuple(false, std::exchange(digit_back->*p_tree_right, nullptr));
                        digit_node_t *current_digit_back = digit_back;
                        for(std::ptrdiff_t current_digit_position = digit_back->digit_position - 1; current_digit_position != -1; --current_digit_position)
                        {
                            current_digit_back->*p_next = new digit_node_t(static_cast<std::size_t>(current_digit_position));
                            current_digit_back->*p_next->*p_prev = current_digit_back;
                            current_digit_back->*next->*p_tree_left = p_tree_to_be_broken_down->*p_child_left;
                            current_digit_back->*next->*p_tree_right = nullptr;
                            current_digit_back = current_digit_back->*next;
                            if(current_digit_back->digit_position == 0)
                                p_tree_node_to_p_list_node(current_digit_back->*p_tree_left)->parent = p_digit_node_to_p_tree_node(current_digit_back);
                            else
                                (current_digit_back->*p_tree_left)->parent = p_digit_node_to_p_tree_node(current_digit_back);
                            if constexpr(!std::is_same_v<accumulated_storage_t, void>)
                                sequence->projector_and_accumulator().destroy_accumulated_storage(p_tree_to_be_broken_down->p_accumulated_storage());
                            delete std::exchange(p_tree_to_be_broken_down, p_tree_to_be_broken_down->*p_child_right);
                        }
                        current_digit_back->*p_next = sequence->digit_node_end;
                        tagged_ptr_bit0_unsetted(sequence->digit_node_end)->*p_prev = tagged_ptr_bit0_setted(current_digit_back);
                        auto update_accumulated_storage = [this](digit_node_t *digit_node, bool should_update_left_accumulated_storage, bool should_update_self_accumulated_storage, bool should_update_right_accumulated_storage)
                        {
                            if(should_update_self_accumulated_storage)
                                update_or_construct_digit_node_right_accumulated_storage_if_exists(digit_node);
                            if(should_update_left_accumulated_storage)
                            {
                                for(circular_doubly_linked_list_node_navigator_t *digit_node_navigator = digit_node->*p_prev; digit_node_navigator != sequence->digit_node_end; digit_node_navigator = digit_node_navigator->*p_prev)
                                {
                                    digit_node_t *digit_node_left = static_cast<digit_node_t *>(digit_node_navigator);
                                    update_digit_node_left_accumulated_storage_if_exists(digit_node_left);
                                }
                            }
                            if(should_update_right_accumulated_storage)
                            {
                                for(circular_doubly_linked_list_node_navigator_t *digit_node_navigator = digit_node->*p_next; digit_node_navigator != sequence->digit_node_end; digit_node_navigator = digit_node_navigator->*p_next)
                                {
                                    digit_node_t *digit_node_right = static_cast<digit_node_t *>(digit_node_navigator);
                                    update_or_construct_digit_node_right_accumulated_storage_if_exists(digit_node_right, true);
                                }
                            }
                        };
                        if(!will_delete_digit_back)
                        {
                            update_accumulated_storage(digit_back, false, sequence->digit_middle() != digit_back, true);
                            digit_back = current_digit_back;
                        }
                        else
                        {
                            if(digit_back != sequence->digit_middle())
                            {
                                circular_doubly_linked_list_node_navigator_t::extract_impl(digit_back, p_prev, p_next);
                                update_accumulated_storage(digit_back, false, false, true);
                                if constexpr(!std::is_same_v<accumulated_storage_t, void>)
                                    sequence->projector_and_accumulator().destroy_accumulated_storage(digit_back->p_accumulated_storage());
                                delete std::exchange(digit_back, current_digit_back);
                            }
                            else
                            {
                                if(digit_back->*p_prev == sequence->digit_node_end)
                                {
                                    assert(digit_front == digit_back);
                                    assert(sequence->digit_middle() == digit_back);
                                    sequence->digit_middle() = digit_front = digit_back->*next;
                                    circular_doubly_linked_list_node_navigator_t::extract_impl(digit_back, p_prev, p_next);
                                    update_accumulated_storage(static_cast<digit_node_t *>(static_cast<circular_doubly_linked_list_node_navigator_t *>(sequence->digit_middle())), false, false, true);
                                    delete std::exchange(digit_back, current_digit_back);
                                }
                                else if((digit_back->*prev)->digit_position + 1 != digit_back->digit_position)
                                {
                                    sequence->digit_middle() = digit_back->*next;
                                    circular_doubly_linked_list_node_navigator_t::extract_impl(digit_back, p_prev, p_next);
                                    update_accumulated_storage(static_cast<digit_node_t *>(static_cast<circular_doubly_linked_list_node_navigator_t *>(sequence->digit_middle())), true, false, true);
                                    delete std::exchange(digit_back, current_digit_back);
                                }
                                else if(int digit = (digit_back->*prev->*p_tree_left != nullptr) + (digit_back->*prev->*p_tree_right != nullptr); digit == 1)
                                {
                                    if((digit_back->*next)->digit_position == 0)
                                        p_tree_node_to_p_list_node(digit_back->*next->*p_tree_left)->parent = p_digit_node_to_p_tree_node(digit_back->*prev);
                                    else
                                        (digit_back->*next->*p_tree_left)->parent = p_digit_node_to_p_tree_node(digit_back->*prev);
                                    if(digit_back->*prev->*p_tree_left != nullptr)
                                        digit_back->*prev->*p_tree_right = digit_back->*next->*p_tree_left;
                                    else
                                        digit_back->*prev->*p_tree_left = std::exchange(digit_back->*prev->*p_tree_right, digit_back->*next->*p_tree_left);

                                    bool current_digit_back_is_p_digit_back_p_next = digit_back->*p_next->*p_next == sequence->digit_node_end;

                                    digit_node_t *p_digit_back_p_next = digit_back->*next;
                                    circular_doubly_linked_list_node_navigator_t::extract_impl(digit_back->*p_next, p_prev, p_next);
                                    delete(p_digit_back_p_next);

                                    circular_doubly_linked_list_node_navigator_t::extract_impl(digit_back, p_prev, p_next);

                                    sequence->digit_middle() = digit_back->*prev;
                                    if constexpr(!std::is_same_v<accumulated_storage_t, void>)
                                        sequence->projector_and_accumulator().destroy_accumulated_storage((digit_back->*prev)->p_accumulated_storage());
                                    update_accumulated_storage(digit_back->*prev, true, false, true);
                                    if(current_digit_back_is_p_digit_back_p_next)
                                        delete std::exchange(digit_back, static_cast<digit_node_t *>(static_cast<circular_doubly_linked_list_node_navigator_t *>(sequence->digit_middle())));
                                    else
                                        delete std::exchange(digit_back, current_digit_back);
                                }
                                else if(digit == 2)
                                {
                                    tree_node_t *tree_root = new tree_node_t{.parent = p_digit_node_to_p_tree_node(digit_back->*prev)};
                                    tree_root->*p_child_left = digit_back->*prev->*p_tree_left;
                                    tree_root->*p_child_right = digit_back->*prev->*p_tree_right;
                                    if((digit_back->*prev)->digit_position == 0)
                                        p_tree_node_to_p_list_node(tree_root->*p_child_left)->parent = p_tree_node_to_p_list_node(tree_root->*p_child_right)->parent = tree_root;
                                    else
                                        (tree_root->*p_child_left)->parent = (tree_root->*p_child_right)->parent = tree_root;
                                    update_or_construct_tree_node_accumulated_storage_if_exists(tree_root, true);
                                    digit_back->*prev->*p_tree_left = tree_root;
                                    digit_back->*prev->*p_tree_right = nullptr;
                                    ++(digit_back->*prev)->digit_position;

                                    circular_doubly_linked_list_node_navigator_t::extract_impl(digit_back, p_prev, p_next);
                                    sequence->digit_middle() = digit_back->*prev;
                                    if constexpr(!std::is_same_v<accumulated_storage_t, void>)
                                        sequence->projector_and_accumulator().destroy_accumulated_storage((digit_back->*prev)->p_accumulated_storage());
                                    update_accumulated_storage(digit_back->*prev, true, false, true);
                                    delete std::exchange(digit_back, current_digit_back);
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

                void update_range_impl(list_node_t *list_node_range_front, list_node_t *list_node_range_back) const
                {
                    assert(!tagged_ptr_bit0_is_set(list_node_range_front));
                    assert(static_cast<circular_doubly_linked_list_node_navigator_t *>(list_node_range_front) != static_cast<circular_doubly_linked_list_node_navigator_t *>(tagged_ptr_bit0_unsetted(sequence->list_node_end)));
                    assert(!tagged_ptr_bit0_is_set(list_node_range_back));
                    assert(static_cast<circular_doubly_linked_list_node_navigator_t *>(list_node_range_back) != static_cast<circular_doubly_linked_list_node_navigator_t *>(tagged_ptr_bit0_unsetted(sequence->list_node_end)));
                    next_or_prev_impl_t next{&circular_doubly_linked_list_node_navigator_t::next}, prev{&circular_doubly_linked_list_node_navigator_t::prev};
                    auto descend = [&](tree_node_t *tree_node)
                    {
                        auto descend_impl = [this, &list_node_range_back](auto &this_, tree_node_t *tree_node)
                        {
                            if(tagged_ptr_bit0_is_set(tree_node))
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
                    tree_node_t *tree_node = p_list_node_to_p_tree_node(list_node_range_front);
                    tree_node_t *tree_node_parent = list_node_range_front->parent;
                    bool list_node_range_back_is_inside_tree_node = false;
                    list_node_range_back_is_inside_tree_node = list_node_range_back_is_inside_tree_node || descend(tree_node);
                    while(!tagged_ptr_bit0_is_set(tree_node_parent))
                    {
                        if(!list_node_range_back_is_inside_tree_node && tree_node == tree_node_parent->child_left)
                            list_node_range_back_is_inside_tree_node = list_node_range_back_is_inside_tree_node || descend(tree_node_parent->child_right);
                        update_or_construct_tree_node_accumulated_storage_if_exists(tree_node_parent);
                        tree_node = std::exchange(tree_node_parent, tree_node_parent->parent);
                    }
                    digit_node_t *digit_node_front = p_tree_node_to_p_digit_node(tree_node_parent), *digit_node_back = digit_node_front;
                    auto is_at_middle_s_left_or_right = [&sequence = sequence, prev](digit_node_t *digit_node, tree_node_t *tree_node)
                    {
                        if(digit_node == sequence->digit_middle())
                        {
                            if(tree_node == static_cast<digit_node_t *>(static_cast<circular_doubly_linked_list_node_navigator_t *>(sequence->digit_middle()))->tree_left) return false;
                            else if(tree_node == static_cast<digit_node_t *>(static_cast<circular_doubly_linked_list_node_navigator_t *>(sequence->digit_middle()))->tree_right) return true;
                            else std::unreachable();
                        }
                        else
                        {
                            if(tagged_ptr_bit0_is_set(digit_node->prev) || (digit_node->*prev)->digit_position < digit_node->digit_position) return false;
                            else return true;
                        }
                    };
                    bool front_is_at_middle_s_left_or_right = is_at_middle_s_left_or_right(digit_node_front, tree_node);
                    if(!list_node_range_back_is_inside_tree_node)
                    {
                        while(true)
                        {
                            // advance to next tree, break if found
                            if(digit_node_back->tree_left == tree_node && digit_node_back->tree_right != nullptr)
                            {
                                if(descend(tree_node = digit_node_back->tree_right))
                                    break;
                            }
                            else if((digit_node_back->tree_left == tree_node && digit_node_back->tree_right == nullptr) || digit_node_back->tree_right == tree_node)
                            {
                                assert(!tagged_ptr_bit0_is_set(digit_node_back->next));
                                digit_node_back = digit_node_back->*next;
                                if(digit_node_back->tree_left != nullptr)
                                {
                                    if(descend(tree_node = digit_node_back->tree_left))
                                        break;
                                }
                                else if(digit_node_back->tree_right != nullptr)
                                {
                                    if(descend(tree_node = digit_node_back->tree_right))
                                        break;
                                }
                                else std::unreachable();
                            }
                            else std::unreachable();
                        }
                    }
                    bool back_is_at_middle_s_left_or_right = is_at_middle_s_left_or_right(digit_node_back, tree_node);
                    if(!front_is_at_middle_s_left_or_right && !back_is_at_middle_s_left_or_right)
                    {
                        for(circular_doubly_linked_list_node_navigator_t *digit_node_navigator = digit_node_back == sequence->digit_middle() ? digit_node_back->prev : digit_node_back; !tagged_ptr_bit0_is_set(digit_node_navigator); digit_node_navigator = digit_node_navigator->prev)
                            update_digit_node_left_accumulated_storage_if_exists(static_cast<digit_node_t *>(digit_node_navigator));
                    }
                    else if(front_is_at_middle_s_left_or_right && back_is_at_middle_s_left_or_right)
                    {
                        for(circular_doubly_linked_list_node_navigator_t *digit_node_navigator = digit_node_front == sequence->digit_middle() ? digit_node_front->next : digit_node_front; !tagged_ptr_bit0_is_set(digit_node_navigator); digit_node_navigator = digit_node_navigator->next)
                            update_or_construct_digit_node_right_accumulated_storage_if_exists(static_cast<digit_node_t *>(digit_node_navigator));
                    }
                    else if(!front_is_at_middle_s_left_or_right && back_is_at_middle_s_left_or_right)
                    {
                        for(circular_doubly_linked_list_node_navigator_t *digit_node_navigator = static_cast<circular_doubly_linked_list_node_navigator_t *>(sequence->digit_middle())->prev; !tagged_ptr_bit0_is_set(digit_node_navigator); digit_node_navigator = digit_node_navigator->prev)
                            update_digit_node_left_accumulated_storage_if_exists(static_cast<digit_node_t *>(digit_node_navigator));
                        for(circular_doubly_linked_list_node_navigator_t *digit_node_navigator = static_cast<circular_doubly_linked_list_node_navigator_t *>(sequence->digit_middle())->next; !tagged_ptr_bit0_is_set(digit_node_navigator); digit_node_navigator = digit_node_navigator->next)
                            update_or_construct_digit_node_right_accumulated_storage_if_exists(static_cast<digit_node_t *>(digit_node_navigator));
                    }
                    else std::unreachable();
                    update_digit_node_end_accumulated_storage_if_exists(static_cast<digit_node_t *>(static_cast<circular_doubly_linked_list_node_navigator_t *>((sequence->*p_digit_front)())), static_cast<digit_node_t *>(static_cast<circular_doubly_linked_list_node_navigator_t *>((sequence->*p_digit_back)())));
                }
                accumulated_storage_t read_range_impl(list_node_t *list_node_range_front, list_node_t *list_node_range_back) const
                {
                    assert(!tagged_ptr_bit0_is_set(list_node_range_front));
                    assert(static_cast<circular_doubly_linked_list_node_navigator_t *>(list_node_range_front) != static_cast<circular_doubly_linked_list_node_navigator_t *>(tagged_ptr_bit0_unsetted(sequence->list_node_end)));
                    assert(!tagged_ptr_bit0_is_set(list_node_range_back));
                    assert(static_cast<circular_doubly_linked_list_node_navigator_t *>(list_node_range_back) != static_cast<circular_doubly_linked_list_node_navigator_t *>(tagged_ptr_bit0_unsetted(sequence->list_node_end)));
                    next_or_prev_impl_t next{&circular_doubly_linked_list_node_navigator_t::next}, prev{&circular_doubly_linked_list_node_navigator_t::prev};
                    if(list_node_range_front == list_node_range_back)
                        return sequence->projector_and_accumulator().construct_accumulated_storage(std::make_tuple(std::cref(*p_element_or_p_projected_storage_from_list_node(list_node_range_front))));
                    else
                    {
                        enum { status_waiting_for_collision = 0,
                            status_accumulating_piplings = 1,
                            status_waiting_at_digit_node = 2,
                        };
                        auto recursive_polymorphic_lambda = [this, next,
                                                                tree_node_front = p_list_node_to_p_tree_node(list_node_range_front), tree_node_back = p_list_node_to_p_tree_node(list_node_range_back),
                                                                status_front = status_waiting_for_collision, status_back = status_waiting_for_collision,
                                                                digit_node_front = static_cast<digit_node_t *>(nullptr), digit_node_back = static_cast<digit_node_t *>(nullptr)](auto &this_, auto accumulated_tuple_so_far_front, auto accumulated_tuple_so_far_back) mutable -> accumulated_storage_t
                        {
                            tree_node_t *tree_node_front_parent;
                            tree_node_t *tree_node_back_parent;
                            if(status_front != status_waiting_at_digit_node)
                                tree_node_front_parent = tagged_ptr_bit0_is_set(tree_node_front) ? p_tree_node_to_p_list_node(tree_node_front)->parent : tree_node_front->parent;
                            if(status_back != status_waiting_at_digit_node)
                                tree_node_back_parent = tagged_ptr_bit0_is_set(tree_node_back) ? p_tree_node_to_p_list_node(tree_node_back)->parent : tree_node_back->parent;
                            if(status_front != status_waiting_at_digit_node && status_back != status_waiting_at_digit_node && tree_node_front_parent == tree_node_back_parent)
                            {
                                if(status_front == status_waiting_for_collision && status_back == status_waiting_for_collision)
                                {
                                    if(tagged_ptr_bit0_is_set(tree_node_front_parent))
                                    {
                                        if(tagged_ptr_bit0_is_set(tree_node_front))
                                            return sequence->projector_and_accumulator().construct_accumulated_storage(std::make_tuple(std::cref(*p_element_or_p_projected_storage_from_list_node(p_tree_node_to_p_list_node(tree_node_front))), std::cref(*p_element_or_p_projected_storage_from_list_node(p_tree_node_to_p_list_node(tree_node_back)))));
                                        else
                                            return sequence->projector_and_accumulator().construct_accumulated_storage(std::make_tuple(std::ref(*tree_node_front->p_accumulated_storage()), std::ref(*tree_node_back->p_accumulated_storage())));
                                    }
                                    else
                                        return sequence->projector_and_accumulator().construct_accumulated_storage(std::make_tuple(std::ref(*tree_node_front_parent->p_accumulated_storage())));
                                }
                                else
                                    return sequence->projector_and_accumulator().construct_accumulated_storage(std::tuple_cat(accumulated_tuple_so_far_front, accumulated_tuple_so_far_back));
                            }
                            else if(status_front == status_waiting_at_digit_node && status_back == status_waiting_at_digit_node)
                            {
                                if(digit_node_front == digit_node_back ||
                                    (digit_node_front->next == digit_node_back &&
                                        ((tree_node_front == digit_node_front->tree_left && digit_node_front->tree_right == nullptr) || tree_node_front == digit_node_front->tree_right) &&
                                        (tree_node_back == digit_node_back->tree_left || (digit_node_back->tree_left == nullptr && tree_node_back == digit_node_back->tree_right))))
                                    return sequence->projector_and_accumulator().construct_accumulated_storage(std::tuple_cat(accumulated_tuple_so_far_front, accumulated_tuple_so_far_back));
                                else
                                {
                                    tree_node_t *tree_to_be_accumulated;
                                    if((tree_node_front == digit_node_front->tree_left && digit_node_front->tree_right == nullptr) || tree_node_front == digit_node_front->tree_right)
                                    {
                                        digit_node_front = digit_node_front->*next;
                                        if(digit_node_front->tree_left != nullptr)
                                            tree_to_be_accumulated = digit_node_front->tree_left;
                                        else if(digit_node_front->tree_right != nullptr)
                                            tree_to_be_accumulated = digit_node_front->tree_right;
                                        else std::unreachable();
                                    }
                                    else if(tree_node_front == digit_node_front->tree_left && digit_node_front->tree_right != nullptr)
                                    {
                                        tree_to_be_accumulated = digit_node_front->tree_right;
                                    }
                                    else std::unreachable();
                                    if(tagged_ptr_bit0_is_set(tree_to_be_accumulated))
                                    {
                                        accumulated_storage_t intermediate_accumulated_storage(sequence->projector_and_accumulator().construct_accumulated_storage(std::tuple_cat(accumulated_tuple_so_far_front, std::make_tuple(std::cref(*p_element_or_p_projected_storage_from_list_node(p_tree_node_to_p_list_node(tree_to_be_accumulated)))))));
                                        return tree_node_front = tree_to_be_accumulated, this_(this_, std::make_tuple(std::ref(intermediate_accumulated_storage)), accumulated_tuple_so_far_back);
                                    }
                                    else
                                    {
                                        accumulated_storage_t intermediate_accumulated_storage(sequence->projector_and_accumulator().construct_accumulated_storage(std::tuple_cat(accumulated_tuple_so_far_front, std::make_tuple(std::ref(*tree_to_be_accumulated->p_accumulated_storage())))));
                                        return tree_node_front = tree_to_be_accumulated, this_(this_, std::make_tuple(std::ref(intermediate_accumulated_storage)), accumulated_tuple_so_far_back);
                                    }
                                }
                            }
                            else
                            {
                                auto get_accumulated_tuple_back = [&](auto accumulated_tuple_so_far_front) -> accumulated_storage_t
                                {
                                    if(status_back == status_waiting_at_digit_node)
                                        return this_(this_, accumulated_tuple_so_far_front, accumulated_tuple_so_far_back);
                                    else
                                    {
                                        if(tagged_ptr_bit0_is_set(tree_node_back_parent))
                                        {
                                            status_back = status_waiting_at_digit_node;
                                            digit_node_back = p_tree_node_to_p_digit_node(tree_node_back_parent);
                                            return this_(this_, accumulated_tuple_so_far_front, accumulated_tuple_so_far_back);
                                        }
                                        else
                                        {
                                            if(status_back == status_waiting_for_collision)
                                            {
                                                if(tree_node_back == tree_node_back_parent->child_right)
                                                    return tree_node_back = tree_node_back_parent, this_(this_, accumulated_tuple_so_far_front, std::make_tuple(std::ref(*tree_node_back->p_accumulated_storage())));
                                                else if(tree_node_back == tree_node_back_parent->child_left)
                                                {
                                                    status_back = status_accumulating_piplings;
                                                    return tree_node_back = tree_node_back_parent, this_(this_, accumulated_tuple_so_far_front, accumulated_tuple_so_far_back);
                                                }
                                                else std::unreachable();
                                            }
                                            else if(status_back == status_accumulating_piplings)
                                            {
                                                if(tree_node_back == tree_node_back_parent->child_right)
                                                {
                                                    accumulated_storage_t intermediate_accumulated_storage(sequence->projector_and_accumulator().construct_accumulated_storage(std::tuple_cat(std::make_tuple(std::ref(*tree_node_back_parent->child_left->p_accumulated_storage())), accumulated_tuple_so_far_back)));
                                                    return tree_node_back = tree_node_back_parent, this_(this_, accumulated_tuple_so_far_front, std::make_tuple(std::ref(intermediate_accumulated_storage)));
                                                }
                                                else if(tree_node_back == tree_node_back_parent->child_left)
                                                    return tree_node_back = tree_node_back_parent, this_(this_, accumulated_tuple_so_far_front, accumulated_tuple_so_far_back);
                                                else std::unreachable();
                                            }
                                            else std::unreachable();
                                        }
                                    }
                                };
                                auto get_accumulated_tuple_front = [&, get_accumulated_tuple_back]() -> accumulated_storage_t
                                {
                                    if(status_front == status_waiting_at_digit_node)
                                        return get_accumulated_tuple_back(accumulated_tuple_so_far_front);
                                    else
                                    {
                                        if(tagged_ptr_bit0_is_set(tree_node_front_parent))
                                        {
                                            status_front = status_waiting_at_digit_node;
                                            digit_node_front = p_tree_node_to_p_digit_node(tree_node_front_parent);
                                            return get_accumulated_tuple_back(accumulated_tuple_so_far_front);
                                        }
                                        else
                                        {
                                            if(status_front == status_waiting_for_collision)
                                            {
                                                if(tree_node_front == tree_node_front_parent->child_left)
                                                    return tree_node_front = tree_node_front_parent, get_accumulated_tuple_back(std::make_tuple(std::ref(*tree_node_front->p_accumulated_storage())));
                                                else if(tree_node_front == tree_node_front_parent->child_right)
                                                {
                                                    status_front = status_accumulating_piplings;
                                                    return tree_node_front = tree_node_front_parent, get_accumulated_tuple_back(accumulated_tuple_so_far_front);
                                                }
                                                else std::unreachable();
                                            }
                                            else if(status_front == status_accumulating_piplings)
                                            {
                                                if(tree_node_front == tree_node_front_parent->child_left)
                                                {
                                                    accumulated_storage_t intermediate_accumulated_storage(sequence->projector_and_accumulator().construct_accumulated_storage(std::tuple_cat(accumulated_tuple_so_far_front, std::make_tuple(std::ref(*tree_node_front_parent->child_right->p_accumulated_storage())))));
                                                    return tree_node_front = tree_node_front_parent, get_accumulated_tuple_back(std::make_tuple(std::ref(intermediate_accumulated_storage)));
                                                }
                                                else if(tree_node_front == tree_node_front_parent->child_right)
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
                        return recursive_polymorphic_lambda(recursive_polymorphic_lambda, std::make_tuple(std::cref(*p_element_or_p_projected_storage_from_list_node(list_node_range_front))), std::make_tuple(std::cref(*p_element_or_p_projected_storage_from_list_node(list_node_range_back))));
                    }
                }
            };
#ifdef __EMSCRIPTEN__
            template<typename sequence_possibly_const_t, typename sequence_t, typename tuple_identity_or_reversed_functor_t>
            sequence_push_or_pop_impl_t(sequence_possibly_const_t *sequence,
                typename sequence_t::digit_proxy_t (sequence_t::*p_digit_front)(void),
                typename sequence_t::digit_proxy_t (sequence_t::*p_digit_back)(void),
                circular_doubly_linked_list_node_navigator_t *(circular_doubly_linked_list_node_navigator_t::*p_prev),
                circular_doubly_linked_list_node_navigator_t *(circular_doubly_linked_list_node_navigator_t::*p_next),
                typename sequence_t::tree_node_t *(sequence_t::digit_node_t::*p_tree_left),
                typename sequence_t::tree_node_t *(sequence_t::digit_node_t::*p_tree_right),
                typename sequence_t::tree_node_t *(sequence_t::tree_node_t::*p_child_left),
                typename sequence_t::tree_node_t *(sequence_t::tree_node_t::*p_child_right),
                tuple_identity_or_reversed_functor_t tuple_identity_or_reversed) -> sequence_push_or_pop_impl_t<sequence_possibly_const_t, sequence_t, tuple_identity_or_reversed_functor_t>;
#endif

            template<std::size_t stride_>
            struct add_stride_member
            {
                static constexpr std::size_t stride = stride_;
                static constexpr std::size_t requested_stride = stride_;
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
        struct extract_builtin_projected_storage_from_projecting_n_ary_functor: std::type_identity<void>
        {
        };
        template<typename projecting_n_ary_functor_t>
            requires requires { typename projecting_n_ary_functor_t::projected_storage_t; }
        struct extract_builtin_projected_storage_from_projecting_n_ary_functor<projecting_n_ary_functor_t>: std::type_identity<typename projecting_n_ary_functor_t::projected_storage_t>
        {
        };
        template<typename projecting_n_ary_functor_t>
        using extract_builtin_projected_storage_from_projecting_n_ary_functor_t = typename extract_builtin_projected_storage_from_projecting_n_ary_functor<projecting_n_ary_functor_t>::type;

        template<typename projecting_n_ary_functor_t_ = void, typename projected_storage_t_ = extract_builtin_projected_storage_from_projecting_n_ary_functor_t<projecting_n_ary_functor_t_>>
        struct projector_wrapping_projecting_n_ary_functor_t
        {
            using projected_storage_t = void; // void means to skip the project operation and use the value itself to accumulate, only allowed when requested_stride==1
        };
        template<typename projecting_n_ary_functor_t_, typename projected_storage_t_>
            requires(!std::is_same_v<projected_storage_t_, void>)
        struct projector_wrapping_projecting_n_ary_functor_t<projecting_n_ary_functor_t_, projected_storage_t_>
        {
            using projected_storage_t = projected_storage_t_;
            projecting_n_ary_functor_t_ project_n_ary_functor;

            template<typename it_projected_storage_t, typename it_element_t> // when requested_stride==1
            void construct_projected_storage(projected_storage_t *storage, [[maybe_unused]] it_projected_storage_t it_this_list_node, it_element_t it_also_this_list_node) const
            {
                new(storage) projected_storage_t(project_n_ary_functor(it_also_this_list_node));
            }

            template<typename it_element_t> // when requested_stride>1
            projected_storage_t construct_projected_storage(it_element_t it_chunk_begin, it_element_t it_chunk_end, [[maybe_unused]] std::size_t chunk_size) const
            {
                assert(chunk_size != 0);
                return projected_storage_t(project_n_ary_functor(it_chunk_begin, it_chunk_end));
            }
            template<typename it_projected_storage_t, typename it_element_t> // when requested_stride>1
            void construct_projected_storage(projected_storage_t *storage, [[maybe_unused]] it_projected_storage_t it_this_list_node, it_element_t it_chunk_begin, it_element_t it_chunk_end, [[maybe_unused]] std::size_t chunk_size) const
            {
                assert(chunk_size != 0);
                new(storage) projected_storage_t(project_n_ary_functor(it_chunk_begin, it_chunk_end));
            }

            void destroy_projected_storage(projected_storage_t *storage) const
            {
                storage->~projected_storage_t();
            }

            template<typename it_projected_storage_t, typename it_element_t> // when requested_stride==1
            bool update_projected_storage(projected_storage_t &value, [[maybe_unused]] it_projected_storage_t it_this_list_node, it_element_t it_also_this_list_node) const
            {
                projected_storage_t value_new(project_n_ary_functor(it_also_this_list_node));
                if(value == value_new)
                    return false;
                else
                {
                    value = value_new;
                    return true;
                }
            }

            template<typename it_projected_storage_t, typename it_element_t> // when requested_stride>1
            bool update_projected_storage(projected_storage_t &value, [[maybe_unused]] it_projected_storage_t it_this_list_node, it_element_t it_chunk_begin, it_element_t it_chunk_end, [[maybe_unused]] std::size_t chunk_size) const
            {
                assert(chunk_size != 0);
                projected_storage_t value_new(project_n_ary_functor(it_chunk_begin, it_chunk_end));
                if(value == value_new)
                    return false;
                else
                {
                    value = value_new;
                    return true;
                }
            }
        };

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
                if constexpr(requires { accumulate_binary_functor.monoidal(); })
                    return accumulate_binary_functor(accumulate_binary_functor.monoidal(), std::get<index>(t));
                else
                    return std::get<index>(t);
            }
            else
                return accumulate_binary_functor(tuple_fold<index - 1>(accumulate_binary_functor, t), std::get<index>(t));
        }

        template<typename accumulating_binary_functor_t_ = void, typename accumulated_storage_t_ = extract_builtin_accumulated_storage_from_accumulating_binary_functor_t<accumulating_binary_functor_t_>>
        struct accumulator_wrapping_accumulating_binary_functor_t
        {
            using accumulated_storage_t = void; // void means to skip the accumulate operation
        };
        template<typename accumulating_binary_functor_t_, typename accumulated_storage_t_>
            requires(!std::is_same_v<accumulated_storage_t_, void>)
        struct accumulator_wrapping_accumulating_binary_functor_t<accumulating_binary_functor_t_, accumulated_storage_t_>
        {
            using accumulated_storage_t = accumulated_storage_t_;
            accumulating_binary_functor_t_ accumulate_binary_functor;

            template<typename... Args>
            accumulated_storage_t construct_accumulated_storage(std::tuple<Args &...> const &values) const
            {
                if constexpr(sizeof...(Args) == 0)
                {
                    if constexpr(requires { accumulate_binary_functor.monoidal(); })
                        return accumulated_storage_t(accumulate_binary_functor.monoidal());
                    else
                        return accumulated_storage_t();
                }
                else
                    return accumulated_storage_t(tuple_fold<sizeof...(Args) - 1>(accumulate_binary_functor, values));
            }
            template<typename... Args>
            void construct_accumulated_storage(accumulated_storage_t *storage, std::tuple<Args &...> const &values) const
            {
                if constexpr(sizeof...(Args) == 0)
                {
                    if constexpr(requires { accumulate_binary_functor.monoidal(); })
                        new(storage) accumulated_storage_t(accumulate_binary_functor.monoidal());
                    else
                        new(storage) accumulated_storage_t();
                }
                else
                    new(storage) accumulated_storage_t(tuple_fold<sizeof...(Args) - 1>(accumulate_binary_functor, values));
            }

            void destroy_accumulated_storage(accumulated_storage_t *storage) const
            {
                storage->~accumulated_storage_t();
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
                    if constexpr(requires { accumulate_binary_functor.monoidal(); })
                        return compare_and_update(accumulated_storage_t(accumulate_binary_functor.monoidal()));
                    else
                        return compare_and_update(accumulated_storage_t());
                }
                else
                    return compare_and_update(accumulated_storage_t(tuple_fold<sizeof...(Args) - 1>(accumulate_binary_functor, values)));
            }
        };

        template<typename projecting_n_ary_functor_t = void, typename accumulating_binary_functor_t = void>
        struct projector_and_accumulator_wrapping_projecting_and_accumulating_functors_t
            : projector_wrapping_projecting_n_ary_functor_t<projecting_n_ary_functor_t>,
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
            projected_storage_t operator()(It_Chunk it_chunk_begin, It_Chunk it_chunk_end) const
            {
                assert(it_chunk_begin != it_chunk_end);
                if constexpr(!min_or_max)
                    return *
#ifdef __EMSCRIPTEN__
                        std::min_element
#else
                        std::ranges::min_element
#endif
                        (it_chunk_begin, it_chunk_end, binary_functor);
                else
                    return *
#ifdef __EMSCRIPTEN__
                        std::max_element
#else
                        std::ranges::max_element
#endif
                        (it_chunk_begin, it_chunk_end, binary_functor);
            }
        };

        //more homogeneous binary functors
        template<typename element_t>
        struct min_t
        {
            element_t operator()(element_t const &lhs, element_t const &rhs) const
            {
                return
#ifdef __EMSCRIPTEN__
                    std::min
#else
                    std::ranges::min
#endif
                    (lhs, rhs);
            }
        };
        template<typename element_t>
        struct max_t
        {
            element_t operator()(element_t const &lhs, element_t const &rhs) const
            {
                return
#ifdef __EMSCRIPTEN__
                    std::max
#else
                    std::ranges::max
#endif
                    (lhs, rhs);
            }
        };

        template<typename monoidal_c>
        struct add_monoidal_member
        {
            static constexpr typename monoidal_c::value_type monoidal()
            {
                return monoidal_c::value;
            }
        };
        template<>
        struct add_monoidal_member<void>
        {
        };
        template<typename element_t_, typename homogeneous_binary_functor_t, typename monoidal_c = void>
        struct accumulating_binary_functor_wrapping_homogeneous_binary_functor_t: homogeneous_binary_functor_t, add_monoidal_member<monoidal_c>
        {
            using accumulated_storage_t = element_t_;
        };
        template<typename element_t_, typename sequence_t_>
        struct accumulating_sequence_binary_functor_t
        {
            using accumulated_storage_t = sequence_t_;

            sequence_t_ monoidal() const { return sequence_t_(); }
            sequence_t_ operator()(sequence_t_ lhs, element_t_ const &rhs) const
            {
                lhs.push_back(rhs);
                return lhs;
            }
            sequence_t_ operator()(sequence_t_ lhs, sequence_t_ &rhs) const
            {
#ifdef __EMSCRIPTEN__
                std::copy(rhs.begin(), rhs.end(), std::back_inserter(lhs));
#else
                std::ranges::copy(rhs, std::back_inserter(lhs));
#endif
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
        /**/ static_assert(std::type_identity_t<accumulating_binary_functor_wrapping_homogeneous_binary_functor_t<bool, std::logical_and<bool>, std::true_type> const>().monoidal() == true);
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
        /**/ static_assert(std::type_identity_t<accumulating_binary_functor_wrapping_homogeneous_binary_functor_t<bool, std::logical_or<bool>, std::false_type> const>().monoidal() == false);

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

    //    typename stride_to_projector_and_accumulator_map_t = std::tuple< //
    //                                                             std::pair<std::integral_constant<std::size_t, 1>, augmented_deque_helpers::example_stride1_chunk1_projector_skipped_and_accumulator_plus_t<element_t_>>, //
    //                                                             std::pair<std::integral_constant<std::size_t, 2>, augmented_deque_helpers::example_chunkgt1_projector_min_element_and_accumulator_max_t<element_t_>> //
    //                                                             > //
    //        >
    template<
        typename element_t_,
        typename allocator_t = std::allocator<element_t_>,
        typename requested_stride_to_projector_and_accumulator_map_t = std::tuple<> //
        >
    struct augmented_deque_t
    {
        using not_empty_stride_to_projector_and_accumulator_map_t = std::conditional_t<std::tuple_size_v<requested_stride_to_projector_and_accumulator_map_t> == 0,
            std::tuple<std::pair<std::integral_constant<std::size_t, 1>, augmented_deque_helpers::empty_projector_and_accumulator_t<element_t_>>>,
            requested_stride_to_projector_and_accumulator_map_t>;
        using stride_to_projector_and_accumulator_map_t = std::conditional_t<
            std::is_same_v<typename std::tuple_element_t<0, not_empty_stride_to_projector_and_accumulator_map_t>::first_type, std::integral_constant<std::size_t, 1>>,
            not_empty_stride_to_projector_and_accumulator_map_t,
            decltype(std::tuple_cat(std::declval<std::tuple<std::pair<std::integral_constant<std::size_t, 1>, augmented_deque_helpers::empty_projector_and_accumulator_t<element_t_>>>>(), std::declval<not_empty_stride_to_projector_and_accumulator_map_t>()))>;

        using element_t = element_t_;

        template<std::size_t sequence_index, std::size_t stride, typename projector_and_accumulator_t>
        struct augmented_deque_sequence_t;
        template<std::size_t sequence_index, std::size_t stride_, typename projector_and_accumulator_t_>
            requires(sequence_index != 0)
        struct augmented_deque_sequence_t<sequence_index, stride_, projector_and_accumulator_t_>;
        template<typename projector_and_accumulator_t_>
        struct augmented_deque_sequence_t<0, 1, projector_and_accumulator_t_>;

        using stride1_projector_and_accumulator_t = detail::mp::map_find_first_t<stride_to_projector_and_accumulator_map_t, std::integral_constant<std::size_t, 1>, augmented_deque_helpers::empty_projector_and_accumulator_t<element_t>>;
        using stride1_sequence_t = augmented_deque_sequence_t<0, 1, stride1_projector_and_accumulator_t>;

        using other_strides_stride_to_projector_and_accumulator_map_t = detail::mp::map_erase_first_t<stride_to_projector_and_accumulator_map_t, std::integral_constant<std::size_t, 1>>;
        template<std::size_t index, typename stride_to_projector_and_accumulator_item_t>
        struct stride_to_projector_and_accumulator_item_to_sequence: std::type_identity<augmented_deque_sequence_t<index + 1, stride_to_projector_and_accumulator_item_t::first_type::value, typename stride_to_projector_and_accumulator_item_t::second_type>>
        {};
        using other_stride_sequences_t = detail::mp::map_transform_t<other_strides_stride_to_projector_and_accumulator_map_t, stride_to_projector_and_accumulator_item_to_sequence>;

        template<std::size_t sequence_index, std::size_t stride_, typename projector_and_accumulator_t_>
            requires(sequence_index != 0)
        struct augmented_deque_sequence_t<sequence_index, stride_, projector_and_accumulator_t_>: detail::augmented_deque::add_stride_member<stride_>
        {
            using projector_and_accumulator_t = projector_and_accumulator_t_;

            using stride1_sequence_t = augmented_deque_t::stride1_sequence_t;

            using projected_storage_t = typename projector_and_accumulator_t::projected_storage_t;
            using accumulated_storage_t = typename projector_and_accumulator_t::accumulated_storage_t;
            static_assert((stride_ == 1 && (!std::is_same_v<projected_storage_t, void> || !std::is_same_v<accumulated_storage_t, void>)) || (stride_ != 1 && !std::is_same_v<projected_storage_t, void>));
            struct sequence_config_t
            {
                static constexpr std::size_t requested_stride = stride_;
                using projector_and_accumulator_t = augmented_deque_sequence_t::projector_and_accumulator_t;

                using projected_storage_t = augmented_deque_sequence_t::projected_storage_t;
                using accumulated_storage_t = augmented_deque_sequence_t::accumulated_storage_t;
                struct actual_projected_storage_t: detail::augmented_deque::add_projected_storage_member_t<projected_storage_t>
                {
                    typename stride1_sequence_t::list_node_t *child;
                };
                struct actual_projected_storage_end_t
                {
                    typename stride1_sequence_t::list_node_end_t *child;
                };
            };
            using list_node_end_t = detail::augmented_deque::list_node_end_t<sequence_config_t>;
            using list_node_t = detail::augmented_deque::list_node_t<sequence_config_t>;
            using actual_projected_storage_t = typename list_node_t::actual_projected_storage_t;
            using tree_node_t = detail::augmented_deque::tree_node_t<accumulated_storage_t>;
            using digit_node_end_t = detail::augmented_deque::digit_node_end_t<sequence_config_t>;
            using digit_node_t = detail::augmented_deque::digit_node_t<sequence_config_t>;

            using iterator_projected_storage_t = detail::augmented_deque::iterator_projected_storage_t<false, sequence_config_t>;
            static_assert(std::input_or_output_iterator<iterator_projected_storage_t>);
            static_assert(std::input_iterator<iterator_projected_storage_t>);
            static_assert(std::sentinel_for<iterator_projected_storage_t, iterator_projected_storage_t>);
            static_assert(std::sentinel_for<std::default_sentinel_t, iterator_projected_storage_t>);
            static_assert(std::forward_iterator<iterator_projected_storage_t>);
            static_assert(std::bidirectional_iterator<iterator_projected_storage_t>);
            static_assert(std::sized_sentinel_for<iterator_projected_storage_t, iterator_projected_storage_t>);
            static_assert(std::sized_sentinel_for<std::default_sentinel_t, iterator_projected_storage_t>);
            static_assert(std::random_access_iterator<iterator_projected_storage_t>);
            using const_iterator_projected_storage_t = detail::augmented_deque::iterator_projected_storage_t<true, sequence_config_t>;
            static_assert(std::input_or_output_iterator<const_iterator_projected_storage_t>);
            static_assert(std::input_iterator<const_iterator_projected_storage_t>);
            static_assert(std::sentinel_for<iterator_projected_storage_t, const_iterator_projected_storage_t>);
            static_assert(std::sentinel_for<std::default_sentinel_t, const_iterator_projected_storage_t>);
            static_assert(std::forward_iterator<const_iterator_projected_storage_t>);
            static_assert(std::bidirectional_iterator<const_iterator_projected_storage_t>);
            static_assert(std::sized_sentinel_for<const_iterator_projected_storage_t, const_iterator_projected_storage_t>);
            static_assert(std::sized_sentinel_for<std::default_sentinel_t, const_iterator_projected_storage_t>);
            static_assert(std::random_access_iterator<const_iterator_projected_storage_t>);
            iterator_projected_storage_t begin_projected_storage() { return {detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::untagged_next_or_tagged_end(list_node_end)}; }
            const_iterator_projected_storage_t cbegin_projected_storage() const { return {detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::untagged_next_or_tagged_end(list_node_end)}; }
            iterator_projected_storage_t end_projected_storage() { return {list_node_end}; }
            const_iterator_projected_storage_t cend_projected_storage() const { return {list_node_end}; }

            list_node_end_t *list_node_end;
            std::size_t const &list_node_count() const { return detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(list_node_end)->node_count; }
            std::size_t const &list_front_element_count() const { return detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(list_node_end)->front_element_count; }
            std::size_t const &list_back_element_count() const { return detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(list_node_end)->back_element_count; }
            std::size_t &list_node_count() { return detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(list_node_end)->node_count; }
            std::size_t &list_front_element_count() { return detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(list_node_end)->front_element_count; }
            std::size_t &list_back_element_count() { return detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(list_node_end)->back_element_count; }

            digit_node_end_t *digit_node_end;
            projector_and_accumulator_t const &projector_and_accumulator() const { return detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(digit_node_end)->projector_and_accumulator; }
            detail::augmented_deque::circular_doubly_linked_list_node_navigator_t *digit_front() const { return detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(this->digit_node_end)->next); }
            detail::augmented_deque::circular_doubly_linked_list_node_navigator_t *digit_middle() const { return detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(this->digit_node_end)->middle); }
            detail::augmented_deque::circular_doubly_linked_list_node_navigator_t *digit_back() const { return detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(this->digit_node_end)->prev); }
            struct digit_proxy_t
            {
                augmented_deque_sequence_t *this_;
                detail::augmented_deque::circular_doubly_linked_list_node_navigator_t *(detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::*next_or_prev);
                operator detail::augmented_deque::circular_doubly_linked_list_node_navigator_t *() { return detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(this_->digit_node_end)->*next_or_prev); }
                digit_proxy_t &operator=(detail::augmented_deque::circular_doubly_linked_list_node_navigator_t *digit_front_new)
                {
                    detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(this_->digit_node_end)->*next_or_prev = detail::tagged_ptr_ns::tagged_ptr_bit0_setted(digit_front_new);
                    return *this;
                }
            };
            auto digit_front()
            {
                return digit_proxy_t{
                    .this_ = this,
                    .next_or_prev = &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::next,
                };
            }
            auto digit_middle()
            {
                struct proxy_t
                {
                    augmented_deque_sequence_t *this_;
                    operator detail::augmented_deque::circular_doubly_linked_list_node_navigator_t *()
                    {
                        return detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(this_->digit_node_end)->middle);
                    }
                    proxy_t &operator=(detail::augmented_deque::circular_doubly_linked_list_node_navigator_t *digit_middle_new)
                    {
                        detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(this_->digit_node_end)->middle = detail::tagged_ptr_ns::tagged_ptr_bit0_setted(digit_middle_new);
                        return *this;
                    }
                } proxy{.this_ = this};
                return proxy;
            }
            auto digit_back()
            {
                return digit_proxy_t{
                    .this_ = this,
                    .next_or_prev = &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::prev,
                };
            }

            augmented_deque_sequence_t()
                : list_node_end(list_node_end_t::create_tagged_end()),
                  digit_node_end(digit_node_end_t::create_tagged_end())
            {
                detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(digit_node_end)->middle = digit_node_end;
                detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(digit_node_end)->list_node_end = list_node_end;
                detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(list_node_end)->digit_node_end = digit_node_end;
                projector_and_accumulator().construct_accumulated_storage(detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(digit_node_end)->p_accumulated_storage(), std::make_tuple());
            }

            bool empty() const { return list_node_count() == 0; }
            void push_back(typename stride1_sequence_t::list_node_t *p_stride1_sequence_list_node)
            {
                detail::augmented_deque::sequence_push_or_pop_impl_t push_back_impl_functor{this,
                    &augmented_deque_sequence_t::digit_front, &augmented_deque_sequence_t::digit_back,
                    &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::prev, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::next, &digit_node_t::tree_left, &digit_node_t::tree_right, &tree_node_t::child_left, &tree_node_t::child_right,
                    detail::augmented_deque::tuple_identity};
                auto push_back_and_get_p_list_node = [this, &p_stride1_sequence_list_node](tree_node_t *parent)
                {
                    ++list_node_count();
                    auto list_back = new list_node_t{
                        .parent = parent,
                        .actual_projected_storage{
                            .child = p_stride1_sequence_list_node,
                        },
                    };
                    detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::push_impl(list_node_end, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::prev, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::next, list_back);
                    std::get<sequence_index - 1>(p_stride1_sequence_list_node->actual_projected_storage.parents) = list_back;
                    return list_back;
                };
                if(list_node_count() == 0)
                {
                    assert(list_front_element_count() == 0);
                    assert(list_back_element_count() == 0);
                    ++list_front_element_count();
                    ++list_back_element_count();
                    push_back_impl_functor.push_impl(push_back_and_get_p_list_node);
                }
                else
                {
                    if(list_back_element_count() != this->stride)
                    {
                        if(list_node_count() == 1)
                        {
                            assert(list_front_element_count() == list_back_element_count());
                            ++list_front_element_count();
                        }
                        ++list_back_element_count();
                        std::get<sequence_index - 1>(p_stride1_sequence_list_node->actual_projected_storage.parents) = list_node_t::untagged_prev(list_node_end);

                        detail::augmented_deque::sequence_push_or_pop_impl_t update_range_impl_functor{this,
                            &augmented_deque_sequence_t::digit_front, &augmented_deque_sequence_t::digit_back,
                            &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::prev, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::next, &digit_node_t::tree_left, &digit_node_t::tree_right, &tree_node_t::child_left, &tree_node_t::child_right,
                            detail::augmented_deque::tuple_identity};
                        update_range_impl_functor.update_range_impl(list_node_t::untagged_prev(list_node_end), list_node_t::untagged_prev(list_node_end));
                    }
                    else
                    {
                        list_back_element_count() = 1;
                        push_back_impl_functor.push_impl(push_back_and_get_p_list_node);
                    }
                }
            }
            void push_front(typename stride1_sequence_t::list_node_t *p_stride1_sequence_list_node)
            {
                detail::augmented_deque::sequence_push_or_pop_impl_t push_front_impl_functor{this,
                    &augmented_deque_sequence_t::digit_back, &augmented_deque_sequence_t::digit_front,
                    &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::next, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::prev, &digit_node_t::tree_right, &digit_node_t::tree_left, &tree_node_t::child_right, &tree_node_t::child_left,
                    detail::augmented_deque::tuple_reversed};
                auto push_back_and_get_p_list_node = [this, &p_stride1_sequence_list_node](tree_node_t *parent)
                {
                    ++list_node_count();
                    auto list_front = new list_node_t{
                        .parent = parent,
                        .actual_projected_storage{
                            .child = p_stride1_sequence_list_node,
                        },
                    };
                    detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::push_impl(list_node_end, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::next, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::prev, list_front);
                    std::get<sequence_index - 1>(p_stride1_sequence_list_node->actual_projected_storage.parents) = list_front;
                    return list_front;
                };
                if(list_node_count() == 0)
                {
                    assert(list_front_element_count() == 0);
                    assert(list_back_element_count() == 0);
                    ++list_front_element_count();
                    ++list_back_element_count();
                    push_front_impl_functor.push_impl(push_back_and_get_p_list_node);
                }
                else
                {
                    if(list_front_element_count() != this->stride)
                    {
                        if(list_node_count() == 1)
                        {
                            assert(list_front_element_count() == list_back_element_count());
                            ++list_back_element_count();
                        }
                        ++list_front_element_count();
                        list_node_t::untagged_next(list_node_end)->actual_projected_storage.child = p_stride1_sequence_list_node;
                        std::get<sequence_index - 1>(p_stride1_sequence_list_node->actual_projected_storage.parents) = list_node_t::untagged_next(list_node_end);

                        detail::augmented_deque::sequence_push_or_pop_impl_t update_range_impl_functor{this,
                            &augmented_deque_sequence_t::digit_front, &augmented_deque_sequence_t::digit_back,
                            &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::prev, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::next, &digit_node_t::tree_left, &digit_node_t::tree_right, &tree_node_t::child_left, &tree_node_t::child_right,
                            detail::augmented_deque::tuple_identity};
                        update_range_impl_functor.update_range_impl(list_node_t::untagged_next(list_node_end), list_node_t::untagged_next(list_node_end));
                    }
                    else
                    {
                        list_front_element_count() = 1;
                        push_front_impl_functor.push_impl(push_back_and_get_p_list_node);
                    }
                }
            }
            void pop_back([[maybe_unused]] detail::augmented_deque::circular_doubly_linked_list_node_navigator_t *p_stride1_sequence_list_node_end)
            {
                detail::augmented_deque::sequence_push_or_pop_impl_t pop_back_impl_functor{this,
                    &augmented_deque_sequence_t::digit_front, &augmented_deque_sequence_t::digit_back, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::prev, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::next, &digit_node_t::tree_left, &digit_node_t::tree_right, &tree_node_t::child_left, &tree_node_t::child_right, detail::augmented_deque::tuple_identity};
                auto pop_list_node = [this]()
                {
                    --list_node_count();
                    list_node_t *list_back = list_node_t::untagged_prev(list_node_end);
                    list_node_t::extract_impl(list_back, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::prev, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::next);
                    delete list_back;
                };
                assert(list_node_count() != 0);
                if(list_back_element_count() != 1)
                {
                    if(list_node_count() == 1)
                    {
                        assert(list_front_element_count() == list_back_element_count());
                        --list_front_element_count();
                    }
                    --list_back_element_count();

                    detail::augmented_deque::sequence_push_or_pop_impl_t update_range_impl_functor{this,
                        &augmented_deque_sequence_t::digit_front, &augmented_deque_sequence_t::digit_back,
                        &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::prev, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::next, &digit_node_t::tree_left, &digit_node_t::tree_right, &tree_node_t::child_left, &tree_node_t::child_right,
                        detail::augmented_deque::tuple_identity};
                    update_range_impl_functor.update_range_impl(list_node_t::untagged_prev(list_node_end), list_node_t::untagged_prev(list_node_end));
                }
                else
                {
                    if(list_node_count() == 1)
                    {
                        assert(list_front_element_count() == list_back_element_count());
                        --list_front_element_count();
                        --list_back_element_count();
                    }
                    else if(list_node_count() == 2)
                        list_back_element_count() = list_front_element_count();
                    else
                        list_back_element_count() = this->stride;
                    pop_back_impl_functor.pop_impl(pop_list_node);
                }
            }
            void pop_front(detail::augmented_deque::circular_doubly_linked_list_node_navigator_t *p_stride1_sequence_list_node_end)
            {
                detail::augmented_deque::sequence_push_or_pop_impl_t pop_front_impl_functor{this,
                    &augmented_deque_sequence_t::digit_back, &augmented_deque_sequence_t::digit_front, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::next, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::prev, &digit_node_t::tree_right, &digit_node_t::tree_left, &tree_node_t::child_right, &tree_node_t::child_left, detail::augmented_deque::tuple_reversed};
                auto pop_list_node = [this]()
                {
                    --list_node_count();
                    list_node_t *list_front = list_node_t::untagged_next(list_node_end);
                    list_node_t::extract_impl(list_front, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::next, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::prev);
                    delete list_front;
                };
                assert(list_node_count() != 0);
                if(list_front_element_count() != 1)
                {
                    if(list_node_count() == 1)
                    {
                        assert(list_front_element_count() == list_back_element_count());
                        --list_back_element_count();
                    }
                    --list_front_element_count();
                    list_node_t::untagged_next(list_node_end)->actual_projected_storage.child = stride1_sequence_t::list_node_t::untagged_next(p_stride1_sequence_list_node_end);

                    detail::augmented_deque::sequence_push_or_pop_impl_t update_range_impl_functor{this,
                        &augmented_deque_sequence_t::digit_front, &augmented_deque_sequence_t::digit_back,
                        &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::prev, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::next, &digit_node_t::tree_left, &digit_node_t::tree_right, &tree_node_t::child_left, &tree_node_t::child_right,
                        detail::augmented_deque::tuple_identity};
                    update_range_impl_functor.update_range_impl(list_node_t::untagged_next(list_node_end), list_node_t::untagged_next(list_node_end));
                }
                else
                {
                    if(list_node_count() == 1)
                    {
                        assert(list_front_element_count() == list_back_element_count());
                        --list_front_element_count();
                        --list_back_element_count();
                    }
                    else if(list_node_count() == 2)
                        list_front_element_count() = list_back_element_count();
                    else
                        list_front_element_count() = this->stride;
                    pop_front_impl_functor.pop_impl(pop_list_node);
                }
            }

            void update_range(const_iterator_projected_storage_t const &const_iterator_projected_storage_begin, const_iterator_projected_storage_t const &const_iterator_projected_storage_end)
            {
                assert(const_iterator_projected_storage_begin <= const_iterator_projected_storage_end);
                if(const_iterator_projected_storage_begin != const_iterator_projected_storage_end)
                {
                    detail::augmented_deque::sequence_push_or_pop_impl_t push_back_impl_functor{this,
                        &augmented_deque_sequence_t::digit_front, &augmented_deque_sequence_t::digit_back,
                        &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::prev, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::next, &digit_node_t::tree_left, &digit_node_t::tree_right, &tree_node_t::child_left, &tree_node_t::child_right,
                        detail::augmented_deque::tuple_identity};
                    push_back_impl_functor.update_range_impl(static_cast<list_node_t *>(const_iterator_projected_storage_begin.current_list_node), static_cast<list_node_t *>(
#ifdef __EMSCRIPTEN__
                                                                                                                                                       std::prev
#else
                                                                                                                                                       std::ranges::prev
#endif
                                                                                                                                                       (const_iterator_projected_storage_end)
                                                                                                                                                           .current_list_node));
                }
            }
            accumulated_storage_t read_range(const_iterator_projected_storage_t const &const_iterator_projected_storage_begin, const_iterator_projected_storage_t const &const_iterator_projected_storage_end) const
            {
                assert(const_iterator_projected_storage_begin <= const_iterator_projected_storage_end);
                if(const_iterator_projected_storage_begin != const_iterator_projected_storage_end)
                {
                    detail::augmented_deque::sequence_push_or_pop_impl_t push_back_impl_functor{this,
                        &augmented_deque_sequence_t::digit_front, &augmented_deque_sequence_t::digit_back,
                        &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::prev, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::next, &digit_node_t::tree_left, &digit_node_t::tree_right, &tree_node_t::child_left, &tree_node_t::child_right,
                        detail::augmented_deque::tuple_identity};
                    return push_back_impl_functor.read_range_impl(static_cast<list_node_t *>(const_iterator_projected_storage_begin.current_list_node), static_cast<list_node_t *>(
#ifdef __EMSCRIPTEN__
                                                                                                                                                            std::prev
#else
                                                                                                                                                            std::ranges::prev
#endif
                                                                                                                                                            (const_iterator_projected_storage_end)
                                                                                                                                                                .current_list_node));
                }
                else
                    return projector_and_accumulator().construct_accumulated_storage(std::make_tuple());
            }
            template<typename T>
            struct get_const_iterator_element_t: std::type_identity<typename T::const_iterator_element_t>
            {};
            template<template<typename T> typename get_const_iterator_element_tt = get_const_iterator_element_t>
            accumulated_storage_t read_range(typename get_const_iterator_element_tt<stride1_sequence_t>::type const &const_iterator_element_begin, typename get_const_iterator_element_tt<stride1_sequence_t>::type const &const_iterator_element_end) const
            {
                assert(const_iterator_element_begin <= const_iterator_element_end);
                if constexpr(stride_ == 1)
                    return read_range(augmented_deque_t::to_iterator_projected_storage<0>(const_iterator_element_begin), augmented_deque_t::to_iterator_projected_storage<0>(const_iterator_element_end));
                else
                {
                    if(const_iterator_projected_storage_t const_iterator_projected_storage_begin = augmented_deque_t::to_iterator_projected_storage<sequence_index>(const_iterator_element_begin), const_iterator_projected_storage_end = augmented_deque_t::to_iterator_projected_storage<sequence_index>(const_iterator_element_end); const_iterator_projected_storage_begin == const_iterator_projected_storage_end)
                    {
                        if(const_iterator_element_begin == const_iterator_element_end)
                            return projector_and_accumulator().construct_accumulated_storage(std::make_tuple());
                        else
                        {
                            projected_storage_t intermediate_projected_storage(projector_and_accumulator().construct_projected_storage(const_iterator_element_begin, const_iterator_element_end, const_iterator_element_end - const_iterator_element_begin));
                            return projector_and_accumulator().construct_accumulated_storage(std::make_tuple(std::cref(intermediate_projected_storage)));
                        }
                    }
                    else
                    {
                        auto get_left_operand = [&](auto return_accumulated_tuple)
                        { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far)
                          {
                              if(auto const_iterator_element_chunk_begin = augmented_deque_t::to_iterator_element(const_iterator_projected_storage_begin); const_iterator_element_chunk_begin == const_iterator_element_begin)
                                  return return_accumulated_tuple(accumulated_tuple_so_far);
                              else
                              {
                                  ++const_iterator_projected_storage_begin;
                                  const_iterator_element_chunk_begin = augmented_deque_t::to_iterator_element(const_iterator_projected_storage_begin);
                                  projected_storage_t intermediate_projected_storage(projector_and_accumulator().construct_projected_storage(const_iterator_element_begin, const_iterator_element_chunk_begin, const_iterator_element_chunk_begin - const_iterator_element_begin));
                                  return return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::cref(intermediate_projected_storage))));
                              }
                          }; };
                        auto get_middle_operand = [&](auto return_accumulated_tuple)
                        { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far)
                          {
                              if(const_iterator_projected_storage_begin == const_iterator_projected_storage_end)
                                  return return_accumulated_tuple(accumulated_tuple_so_far);
                              else
                              {
                                  accumulated_storage_t intermediate_accumulated_storage(read_range(const_iterator_projected_storage_begin, const_iterator_projected_storage_end));
                                  return return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::ref(intermediate_accumulated_storage))));
                              }
                          }; };
                        auto get_right_operand = [&](auto return_accumulated_tuple)
                        { return [&, return_accumulated_tuple](auto accumulated_tuple_so_far)
                          {
                              if(auto const_iterator_element_chunk_end = augmented_deque_t::to_iterator_element(const_iterator_projected_storage_end); const_iterator_element_chunk_end == const_iterator_element_end)
                                  return return_accumulated_tuple(accumulated_tuple_so_far);
                              else
                              {
                                  projected_storage_t intermediate_projected_storage(projector_and_accumulator().construct_projected_storage(const_iterator_element_chunk_end, const_iterator_element_end, const_iterator_element_end - const_iterator_element_chunk_end));
                                  return return_accumulated_tuple(std::tuple_cat(accumulated_tuple_so_far, std::make_tuple(std::cref(intermediate_projected_storage))));
                              }
                          }; };
                        auto return_accumulated_tuple = [&](auto accumulated_tuple_so_far)
                        { return projector_and_accumulator().construct_accumulated_storage(accumulated_tuple_so_far); };
                        return get_left_operand(get_middle_operand(get_right_operand(return_accumulated_tuple)))(std::make_tuple());
                    }
                }
            }
        };
        template<typename projector_and_accumulator_t_>
        struct augmented_deque_sequence_t<0, 1, projector_and_accumulator_t_>: detail::augmented_deque::add_stride_member<1>
        {
            using projector_and_accumulator_t = projector_and_accumulator_t_;

            using element_t = typename augmented_deque_t::element_t;
            using projected_storage_t = typename projector_and_accumulator_t::projected_storage_t;
            using accumulated_storage_t = typename projector_and_accumulator_t::accumulated_storage_t;
            struct sequence_config_t
            {
                static constexpr std::size_t requested_stride = 1;
                using projector_and_accumulator_t = augmented_deque_sequence_t::projector_and_accumulator_t;

                template<std::size_t index, typename stride_to_projector_and_accumulator_item_t>
                struct stride_to_projector_and_accumulator_item_to_pointer_to_sequence_list_node_t: std::type_identity<typename augmented_deque_sequence_t<index + 1, stride_to_projector_and_accumulator_item_t::first_type::value, typename stride_to_projector_and_accumulator_item_t::second_type>::list_node_t *>
                {};
                using pointers_to_other_strides_sequence_list_node_t = detail::mp::map_transform_t<other_strides_stride_to_projector_and_accumulator_map_t, stride_to_projector_and_accumulator_item_to_pointer_to_sequence_list_node_t>;

                template<std::size_t index, typename stride_to_projector_and_accumulator_item_t>
                struct stride_to_projector_and_accumulator_item_to_pointer_to_sequence_list_node_end_t: std::type_identity<typename augmented_deque_sequence_t<index + 1, stride_to_projector_and_accumulator_item_t::first_type::value, typename stride_to_projector_and_accumulator_item_t::second_type>::list_node_end_t *>
                {};
                using pointers_to_other_strides_sequence_list_node_end_t = detail::mp::map_transform_t<other_strides_stride_to_projector_and_accumulator_map_t, stride_to_projector_and_accumulator_item_to_pointer_to_sequence_list_node_end_t>;

                using element_t = augmented_deque_sequence_t::element_t;
                using projected_storage_t = augmented_deque_sequence_t::projected_storage_t;
                using accumulated_storage_t = augmented_deque_sequence_t::accumulated_storage_t;
                struct actual_projected_storage_t: detail::augmented_deque::add_projected_storage_member_t<projected_storage_t>
                {
                    alignas(element_t) std::byte element_buffer[sizeof(element_t)]; // element_t element;
                    element_t *p_element() { return reinterpret_cast<element_t *>(&element_buffer); }
                    pointers_to_other_strides_sequence_list_node_t parents;
                };
                struct actual_projected_storage_end_t
                {
                    pointers_to_other_strides_sequence_list_node_end_t parents;
                };
            };
            using list_node_end_t = detail::augmented_deque::list_node_end_t<sequence_config_t>;
            using list_node_t = detail::augmented_deque::list_node_t<sequence_config_t>;
            using actual_projected_storage_t = typename list_node_t::actual_projected_storage_t;
            using tree_node_t = detail::augmented_deque::tree_node_t<accumulated_storage_t>;
            using digit_node_end_t = detail::augmented_deque::digit_node_end_t<sequence_config_t>;
            using digit_node_t = detail::augmented_deque::digit_node_t<sequence_config_t>;

            using iterator_element_t = detail::augmented_deque::iterator_element_t<false, sequence_config_t>;
            static_assert(std::input_or_output_iterator<iterator_element_t>);
            static_assert(std::input_iterator<iterator_element_t>);
            static_assert(std::sentinel_for<iterator_element_t, iterator_element_t>);
            static_assert(std::sentinel_for<std::default_sentinel_t, iterator_element_t>);
            static_assert(std::forward_iterator<iterator_element_t>);
            static_assert(std::bidirectional_iterator<iterator_element_t>);
            static_assert(std::sized_sentinel_for<iterator_element_t, iterator_element_t>);
            static_assert(std::sized_sentinel_for<std::default_sentinel_t, iterator_element_t>);
            static_assert(std::random_access_iterator<iterator_element_t>);
            using const_iterator_element_t = detail::augmented_deque::iterator_element_t<true, sequence_config_t>;
            static_assert(std::input_or_output_iterator<const_iterator_element_t>);
            static_assert(std::input_iterator<const_iterator_element_t>);
            static_assert(std::sentinel_for<const_iterator_element_t, const_iterator_element_t>);
            static_assert(std::sentinel_for<std::default_sentinel_t, const_iterator_element_t>);
            static_assert(std::forward_iterator<const_iterator_element_t>);
            static_assert(std::bidirectional_iterator<const_iterator_element_t>);
            static_assert(std::sized_sentinel_for<const_iterator_element_t, const_iterator_element_t>);
            static_assert(std::sized_sentinel_for<std::default_sentinel_t, const_iterator_element_t>);
            static_assert(std::random_access_iterator<const_iterator_element_t>);
            iterator_element_t begin_element() { return {detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::untagged_next_or_tagged_end(list_node_end)}; }
            const_iterator_element_t cbegin_element() const { return {detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::untagged_next_or_tagged_end(list_node_end)}; }
            iterator_element_t end_element() { return {list_node_end}; }
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
            static_assert(std::is_same_v<projected_storage_t, void> || std::random_access_iterator<iterator_projected_storage_t>);
            using const_iterator_projected_storage_t = detail::augmented_deque::iterator_projected_storage_t<true, sequence_config_t>;
            static_assert(std::is_same_v<projected_storage_t, void> || std::input_or_output_iterator<const_iterator_projected_storage_t>);
            static_assert(std::is_same_v<projected_storage_t, void> || std::input_iterator<const_iterator_projected_storage_t>);
            static_assert(std::is_same_v<projected_storage_t, void> || std::sentinel_for<const_iterator_projected_storage_t, const_iterator_projected_storage_t>);
            static_assert(std::is_same_v<projected_storage_t, void> || std::sentinel_for<std::default_sentinel_t, const_iterator_projected_storage_t>);
            static_assert(std::is_same_v<projected_storage_t, void> || std::forward_iterator<const_iterator_projected_storage_t>);
            static_assert(std::is_same_v<projected_storage_t, void> || std::bidirectional_iterator<const_iterator_projected_storage_t>);
            static_assert(std::is_same_v<projected_storage_t, void> || std::sized_sentinel_for<const_iterator_projected_storage_t, const_iterator_projected_storage_t>);
            static_assert(std::is_same_v<projected_storage_t, void> || std::sized_sentinel_for<std::default_sentinel_t, const_iterator_projected_storage_t>);
            static_assert(std::is_same_v<projected_storage_t, void> || std::random_access_iterator<const_iterator_projected_storage_t>);
            iterator_projected_storage_t begin_projected_storage() { return {detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::untagged_next_or_tagged_end(list_node_end)}; }
            const_iterator_projected_storage_t cbegin_projected_storage() const { return {detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::untagged_next_or_tagged_end(list_node_end)}; }
            iterator_projected_storage_t end_projected_storage() { return {list_node_end}; }
            const_iterator_projected_storage_t cend_projected_storage() const { return {list_node_end}; }


            list_node_end_t *list_node_end;
            std::size_t const &list_node_count() const { return detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(list_node_end)->node_count; }
            std::size_t &list_node_count() { return detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(list_node_end)->node_count; }

            digit_node_end_t *digit_node_end;
            projector_and_accumulator_t const &projector_and_accumulator() const { return detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(digit_node_end)->projector_and_accumulator; }
            detail::augmented_deque::circular_doubly_linked_list_node_navigator_t *digit_front() const { return detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(this->digit_node_end)->next); }
            detail::augmented_deque::circular_doubly_linked_list_node_navigator_t *digit_middle() const { return detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(this->digit_node_end)->middle); }
            detail::augmented_deque::circular_doubly_linked_list_node_navigator_t *digit_back() const { return detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(this->digit_node_end)->prev); }
            struct digit_proxy_t
            {
                augmented_deque_sequence_t *this_;
                detail::augmented_deque::circular_doubly_linked_list_node_navigator_t *(detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::*next_or_prev);
                operator detail::augmented_deque::circular_doubly_linked_list_node_navigator_t *() { return detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(this_->digit_node_end)->*next_or_prev); }
                digit_proxy_t &operator=(detail::augmented_deque::circular_doubly_linked_list_node_navigator_t *digit_front_new)
                {
                    detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(this_->digit_node_end)->*next_or_prev = detail::tagged_ptr_ns::tagged_ptr_bit0_setted(digit_front_new);
                    return *this;
                }
            };
            auto digit_front()
            {
                return digit_proxy_t{
                    .this_ = this,
                    .next_or_prev = &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::next,
                };
            }
            auto digit_middle()
            {
                struct proxy_t
                {
                    augmented_deque_sequence_t *this_;
                    operator detail::augmented_deque::circular_doubly_linked_list_node_navigator_t *() { return detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(this_->digit_node_end)->middle); }
                    proxy_t &operator=(detail::augmented_deque::circular_doubly_linked_list_node_navigator_t *digit_middle_new)
                    {
                        detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(this_->digit_node_end)->middle = detail::tagged_ptr_ns::tagged_ptr_bit0_setted(digit_middle_new);
                        return *this;
                    }
                } proxy{.this_ = this};
                return proxy;
            }
            auto digit_back()
            {
                return digit_proxy_t{
                    .this_ = this,
                    .next_or_prev = &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::prev,
                };
            }

            augmented_deque_sequence_t()
                : list_node_end(list_node_end_t::create_tagged_end()),
                  digit_node_end(digit_node_end_t::create_tagged_end())
            {
                detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(digit_node_end)->middle = digit_node_end;
                detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(digit_node_end)->list_node_end = list_node_end;
                detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(list_node_end)->digit_node_end = digit_node_end;
                if constexpr(!std::is_same_v<accumulated_storage_t, void>)
                    projector_and_accumulator().construct_accumulated_storage(detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(digit_node_end)->p_accumulated_storage(), std::make_tuple());
            }

            bool empty() const { return list_node_count() == 0; }
            void push_back(element_t const &value)
            {
                detail::augmented_deque::sequence_push_or_pop_impl_t push_back_impl_functor{this,
                    &augmented_deque_sequence_t::digit_front, &augmented_deque_sequence_t::digit_back,
                    &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::prev, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::next, &digit_node_t::tree_left, &digit_node_t::tree_right, &tree_node_t::child_left, &tree_node_t::child_right,
                    detail::augmented_deque::tuple_identity};
                push_back_impl_functor.push_impl([this, &value](tree_node_t *parent)
                    {
                            ++list_node_count();
                            list_node_t *list_back = new list_node_t{.parent = parent};
                            detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::push_impl(list_node_end, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::prev, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::next, list_back);
                            new(list_back->actual_projected_storage.element_buffer) element_t(value);
                            return list_back; });
            }
            void push_front(element_t const &value)
            {
                detail::augmented_deque::sequence_push_or_pop_impl_t push_front_impl_functor{this,
                    &augmented_deque_sequence_t::digit_back, &augmented_deque_sequence_t::digit_front,
                    &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::next, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::prev, &digit_node_t::tree_right, &digit_node_t::tree_left, &tree_node_t::child_right, &tree_node_t::child_left,
                    detail::augmented_deque::tuple_reversed};
                push_front_impl_functor.push_impl([this, &value](tree_node_t *parent)
                    {
                            ++list_node_count();
                            list_node_t *list_front = new list_node_t{.parent = parent};
                            detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::push_impl(list_node_end, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::next, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::prev, list_front);
                            new(list_front->actual_projected_storage.element_buffer) element_t(value);
                            return list_front; });
            }
            void pop_back()
            {
                detail::augmented_deque::sequence_push_or_pop_impl_t pop_back_impl_functor{this,
                    &augmented_deque_sequence_t::digit_front, &augmented_deque_sequence_t::digit_back, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::prev, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::next, &digit_node_t::tree_left, &digit_node_t::tree_right, &tree_node_t::child_left, &tree_node_t::child_right, detail::augmented_deque::tuple_identity};
                pop_back_impl_functor.pop_impl([this]()
                    {
                            --list_node_count();
                            list_node_t *list_back = list_node_t::untagged_prev(list_node_end);
                            list_node_t::extract_impl(list_back, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::prev, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::next);
                            delete list_back; });
            }
            void pop_front()
            {
                detail::augmented_deque::sequence_push_or_pop_impl_t pop_front_impl_functor{this,
                    &augmented_deque_sequence_t::digit_back, &augmented_deque_sequence_t::digit_front, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::next, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::prev, &digit_node_t::tree_right, &digit_node_t::tree_left, &tree_node_t::child_right, &tree_node_t::child_left, detail::augmented_deque::tuple_reversed};
                pop_front_impl_functor.pop_impl([this]()
                    {
                            --list_node_count();
                            list_node_t *list_front = list_node_t::untagged_next(list_node_end);
                            list_node_t::extract_impl(list_front, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::next, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::prev);
                            delete list_front; });
            }

            void update_range(const_iterator_projected_storage_t const &const_iterator_projected_storage_begin, const_iterator_projected_storage_t const &const_iterator_projected_storage_end)
            {
                assert(const_iterator_projected_storage_begin <= const_iterator_projected_storage_end);
                if(const_iterator_projected_storage_begin != const_iterator_projected_storage_end)
                {
                    detail::augmented_deque::sequence_push_or_pop_impl_t update_range_impl_functor{this,
                        &augmented_deque_sequence_t::digit_front, &augmented_deque_sequence_t::digit_back,
                        &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::prev, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::next, &digit_node_t::tree_left, &digit_node_t::tree_right, &tree_node_t::child_left, &tree_node_t::child_right,
                        detail::augmented_deque::tuple_identity};
                    update_range_impl_functor.update_range_impl(static_cast<list_node_t *>(const_iterator_projected_storage_begin.current_list_node),
                        static_cast<list_node_t *>(
#ifdef __EMSCRIPTEN__
                            std::prev
#else
                            std::ranges::prev
#endif
                            (const_iterator_projected_storage_end)
                                .current_list_node));
                }
            }
            accumulated_storage_t read_range(const_iterator_projected_storage_t const &const_iterator_projected_storage_begin, const_iterator_projected_storage_t const &const_iterator_projected_storage_end) const
            {
                assert(const_iterator_projected_storage_begin <= const_iterator_projected_storage_end);
                if(const_iterator_projected_storage_begin != const_iterator_projected_storage_end)
                {
                    detail::augmented_deque::sequence_push_or_pop_impl_t read_range_impl_functor{this,
                        &augmented_deque_sequence_t::digit_front, &augmented_deque_sequence_t::digit_back,
                        &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::prev, &detail::augmented_deque::circular_doubly_linked_list_node_navigator_t::next, &digit_node_t::tree_left, &digit_node_t::tree_right, &tree_node_t::child_left, &tree_node_t::child_right,
                        detail::augmented_deque::tuple_identity};
                    return read_range_impl_functor.read_range_impl(static_cast<list_node_t *>(const_iterator_projected_storage_begin.current_list_node),
                        static_cast<list_node_t *>(
#ifdef __EMSCRIPTEN__
                            std::prev
#else
                            std::ranges::prev
#endif
                            (const_iterator_projected_storage_end)
                                .current_list_node));
                }
                else
                    return projector_and_accumulator().construct_accumulated_storage(std::make_tuple());
            }
            accumulated_storage_t read_range(const_iterator_element_t const &const_iterator_element_begin, const_iterator_element_t const &const_iterator_element_end) const
            {
                assert(const_iterator_element_begin <= const_iterator_element_end);
                return read_range(augmented_deque_t::to_iterator_projected_storage<0>(const_iterator_element_begin), augmented_deque_t::to_iterator_projected_storage<0>(const_iterator_element_end));
            }
        };

        stride1_sequence_t stride1_sequence;
        other_stride_sequences_t other_stride_sequences;

        static constexpr std::size_t sequences_count = 1 + std::tuple_size_v<other_stride_sequences_t>;
        template<std::size_t I>
        auto &sequence()
        {
            if constexpr(I == 0)
                return stride1_sequence;
            else
                return std::get<I - 1>(other_stride_sequences);
        }
        template<std::size_t I>
        auto &sequence() const
        {
            if constexpr(I == 0)
                return stride1_sequence;
            else
                return std::get<I - 1>(other_stride_sequences);
        }
        using sequences_t = decltype(std::tuple_cat(std::declval<std::tuple<stride1_sequence_t> &>(), std::declval<other_stride_sequences_t &>()));
        template<std::size_t I>
        using sequence_t = typename std::conditional_t<I == 0, std::type_identity<stride1_sequence_t>, std::tuple_element<I - 1, other_stride_sequences_t>>::type;

        augmented_deque_t()
        {
            [&]<std::size_t... I>(std::index_sequence<I...>)
            {
                (..., (detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(std::get<I>(other_stride_sequences).list_node_end)->actual_projected_storage_end.child = stride1_sequence.list_node_end, std::get<I>(detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(stride1_sequence.list_node_end)->actual_projected_storage_end.parents) = std::get<I>(other_stride_sequences).list_node_end));
            }
            (std::make_index_sequence<std::tuple_size_v<other_strides_stride_to_projector_and_accumulator_map_t>>());
        }
        bool empty() const { return stride1_sequence.empty(); }
        void push_back(element_t const &value)
        {
            stride1_sequence.push_back(value);
            [&]<std::size_t... I>(std::index_sequence<I...>)
            {
                (..., std::get<I>(other_stride_sequences).push_back(stride1_sequence_t::list_node_t::untagged_prev(stride1_sequence.list_node_end)));
            }
            (std::make_index_sequence<std::tuple_size_v<other_strides_stride_to_projector_and_accumulator_map_t>>());
        }
        void push_front(element_t const &value)
        {
            stride1_sequence.push_front(value);
            [&]<std::size_t... I>(std::index_sequence<I...>)
            {
                (..., std::get<I>(other_stride_sequences).push_front(stride1_sequence_t::list_node_t::untagged_next(stride1_sequence.list_node_end)));
            }
            (std::make_index_sequence<std::tuple_size_v<other_strides_stride_to_projector_and_accumulator_map_t>>());
        }

        void pop_back()
        {
            stride1_sequence.pop_back();
            [&]<std::size_t... I>(std::index_sequence<I...>)
            {
                (..., std::get<I>(other_stride_sequences).pop_back(stride1_sequence.list_node_end));
            }
            (std::make_index_sequence<std::tuple_size_v<other_strides_stride_to_projector_and_accumulator_map_t>>());
        }
        void pop_front()
        {
            stride1_sequence.pop_front();
            [&]<std::size_t... I>(std::index_sequence<I...>)
            {
                (..., std::get<I>(other_stride_sequences).pop_front(stride1_sequence.list_node_end));
            }
            (std::make_index_sequence<std::tuple_size_v<other_strides_stride_to_projector_and_accumulator_map_t>>());
        }

        template<bool is_const>
        struct sequence_t_to_conditional_const_iterator_projected_storage_t
        {
            template<typename sequence_t>
            struct sequence_t_to_possibly_const_iterator_projected_storage_t: std::type_identity<detail::augmented_deque::iterator_projected_storage_t<is_const, typename sequence_t::sequence_config_t>>
            {
            };
        };
        template<bool is_const, typename sequence_config_t, std::size_t I = detail::mp::list_find_first_index_t<detail::mp::list_transform_t<sequences_t, sequence_t_to_conditional_const_iterator_projected_storage_t<is_const>::template sequence_t_to_possibly_const_iterator_projected_storage_t>, detail::augmented_deque::iterator_projected_storage_t<is_const, sequence_config_t>>::value>
        static detail::augmented_deque::iterator_element_t<is_const, typename stride1_sequence_t::sequence_config_t> to_iterator_element(detail::augmented_deque::iterator_projected_storage_t<is_const, sequence_config_t> const &possibly_const_iterator_projected_storage)
        {
            if constexpr(I == 0)
                return {possibly_const_iterator_projected_storage.current_list_node};
            else
            {
                if(possibly_const_iterator_projected_storage.is_end())
                    return {static_cast<typename sequence_t<I>::list_node_end_t *>(detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(possibly_const_iterator_projected_storage.current_list_node))->actual_projected_storage_end.child};
                else
                    return {static_cast<typename sequence_t<I>::list_node_t *>(possibly_const_iterator_projected_storage.current_list_node)->actual_projected_storage.child};
            }
        }

        template<std::size_t I, bool is_const>
        static auto to_iterator_projected_storage(detail::augmented_deque::iterator_element_t<is_const, typename stride1_sequence_t::sequence_config_t> const &possibly_const_iterator_element)
        {
            if constexpr(I == 0)
                return detail::augmented_deque::iterator_projected_storage_t<is_const, typename stride1_sequence_t::sequence_config_t>{possibly_const_iterator_element.current_list_node};
            else
            {
                if(possibly_const_iterator_element.is_end())
                    return detail::augmented_deque::iterator_projected_storage_t<is_const, typename std::tuple_element_t<I - 1, other_stride_sequences_t>::sequence_config_t>{std::get<I - 1>(static_cast<typename stride1_sequence_t::list_node_end_t *>(detail::tagged_ptr_ns::tagged_ptr_bit0_unsetted(possibly_const_iterator_element.current_list_node))->actual_projected_storage_end.parents)};
                else
                    return detail::augmented_deque::iterator_projected_storage_t<is_const, typename std::tuple_element_t<I - 1, other_stride_sequences_t>::sequence_config_t>{std::get<I - 1>(static_cast<typename stride1_sequence_t::list_node_t *>(possibly_const_iterator_element.current_list_node)->actual_projected_storage.parents)};
            }
        }

        void update_range(typename stride1_sequence_t::const_iterator_element_t const &const_iterator_element_begin, typename stride1_sequence_t::const_iterator_element_t const &const_iterator_element_end)
        {
            stride1_sequence.update_range(to_iterator_projected_storage<0>(const_iterator_element_begin), to_iterator_projected_storage<0>(const_iterator_element_end));
            [&]<std::size_t... I>(std::index_sequence<I...>)
            {
                (..., std::get<I>(other_stride_sequences).update_range(to_iterator_projected_storage<I + 1>(const_iterator_element_begin), to_iterator_projected_storage<I + 1>(const_iterator_element_end)));
            }
            (std::make_index_sequence<std::tuple_size_v<other_strides_stride_to_projector_and_accumulator_map_t>>());
        }
    };
} // namespace augmented_containers

#endif // AUGMENTED_DEQUE_H
