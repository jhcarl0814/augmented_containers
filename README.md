# Augmented Containers

This library provides a stronger version of `sequence containers` (let containers (and its subranges) always have several accompanying results of algorithms/views), `<algorithm>` and `<ranges>` (when the input sequence changes, refresh output values/ranges in logarithmic time complexity).

## Doc

- augmented deque (under development) [https://jhcarl0814.github.io/augmented_containers_doc/augmented_deque.html](https://jhcarl0814.github.io/augmented_containers_doc/augmented_deque.html)

## Requirements

- This library depends on C++23. Support of legacy versions (C++11, 14, 17, 20) is possible but takes extra efforts.
- (Each component in this library is a single header file.)

## Status

- Under Development
  - augmented deque
- Design Completed, Scheduled for Development
  - augmented \*\*\*\*\*
  - augmented \*\*\*\*\*\*\*\*
  - augmented \*\*\*/\*\*\*/\*\*\*\*\*\*\*\*/\*\*\*\*\*\*\*\*
- Under Conception
  - augmented \*\*\*\*\*

## Todo

- augmented deque
  - [ ] the 5 special member functions
  - [ ] conventional constructors and `assign`s
  - [ ] use allocator
  - [ ] non-essential member functions
  - [ ] formalize `projector` and `accumulator` concepts
  - [ ] add the api section to doc
