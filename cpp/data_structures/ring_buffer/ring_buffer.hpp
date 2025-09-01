#pragma once

#include <optional>
#include <concepts>
#include <type_traits>
#include <array>



template <typename T>
concept RingBuffer = requires(T buf, typename T::value_type in, typename T::value_type& out) {
  typename T::value_type;
  {buf.enqueue(in)} -> std::same_as<bool>;
  {buf.try_dequeue(out) } -> std::same_as<bool>;
};

template <typename T>
concept PODType = std::is_trivially_copyable_v<T> && std::is_trivially_destructible_v<T>;

template <PODType T, std::size_t Capacity>
requires (Capacity > 0)
class SPSCRingBufferSimple {
public: 
  using value_type = T;

  static constexpr std::size_t kCapacity = Capacity;

  SPSCRingBufferSimple() = default;

  ~SPSCRingBufferSimple() = default;

  SPSCRingBufferSimple(const SPSCRingBufferSimple&)            = delete;
  SPSCRingBufferSimple& operator=(const SPSCRingBufferSimple&) = delete;

  SPSCRingBufferSimple(SPSCRingBufferSimple&&)            noexcept = delete;
  SPSCRingBufferSimple& operator=(SPSCRingBufferSimple&&) noexcept = delete;

  [[nodiscard]] constexpr std::size_t capacity() const noexcept { return kCapacity; }
  [[nodiscard]] bool empty() const noexcept;
  [[nodiscard]] bool full() const noexcept;
  [[nodiscard]] std::size_t size() const noexcept;

  bool enqueue(const T& value);
  bool enqueue(T&& value);

  bool try_dequeue(T& out);

private:
  alignas(64) std::array<T, Capacity> buffer_{};
  alignas(64) std::atomic<std::size_t> head_{0};
  alignas(64) std::atomic<std::size_t> tail_{0};
};



