# Augmented Containers

This library provides a stronger version of `sequence containers` (let containers (and its subranges) always have several accompanying results of algorithms/views), `<algorithm>` and `<ranges>` (when the input sequence changes, refresh output values/ranges in logarithmic time complexity). To help understand what kind of problems it solves: [Dynamic problem (algorithms) - Wikipedia](https://en.wikipedia.org/wiki/Dynamic_problem_(algorithms)), [Augmented map - Wikipedia](https://en.wikipedia.org/wiki/Augmented_map).

## Doc

- augmented deque (under development) [https://jhcarl0814.github.io/augmented_containers_doc/augmented_deque.html](https://jhcarl0814.github.io/augmented_containers_doc/augmented_deque.html)
- augmented sequence (under development) [https://jhcarl0814.github.io/augmented_containers_doc/augmented_sequence.html](https://jhcarl0814.github.io/augmented_containers_doc/augmented_sequence.html)

## Requirements

- This library depends on C++23. Support of legacy versions (C++11, 14, 17, 20) is possible but takes extra efforts.
- (Each component in this library is a single header file.)

## Status

- Under Development
  - augmented deque
  - augmented sequence
- Design Completed, Scheduled for Development
  - augmented \*\*\*\*\*
  - augmented \*\*\*/\*\*\*/\*\*\*\*\*\*\*\*/\*\*\*\*\*\*\*\*
- Under Conception
  - augmented \*\*\*\*\*

## Todo

- augmented deque and augmented sequence
  - [ ] `ctor(range)`, `insert(range)`, `insert_range(range)`, `append_range(range)`, `prepend_range(range)` change from inserting one by one to bulk loading
  - [ ] `assign` and `operator=` reuse existing nodes if `move/copy assignable`
  - [ ] formalize `projector` and `accumulator` concepts
  - [ ] add the api section to doc
- augmented sequence
  - [ ] `erase(range)` change from erasing one by one to spliting and concatenating

## Waiting On

  - `explicit object parameter (deducing this)` compilers' support
  - `homogeneous variadic function parameters` (P1219)
