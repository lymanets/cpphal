#pragma once

#include "hal/timer/options.hpp"

namespace hal::timer::impl::detail {
template <class M, class T>
struct InputCaptureFilterImpl {
  static constexpr bool    valid = std::is_same_v<M, options::mode::Output>;
  static constexpr uint8_t value = 0;
};

template <>
struct InputCaptureFilterImpl<options::mode::Input, void> {
  // Default: NoFilter
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b0000;
};

template <class M>
struct InputCaptureFilterImpl<M, options::input_capture_filter::NoFilter> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b0000;
};

template <class M>
struct InputCaptureFilterImpl<M, options::input_capture_filter::FCKInt2> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b0001;
};

template <class M>
struct InputCaptureFilterImpl<M, options::input_capture_filter::FCKInt4> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b0010;
};

template <class M>
struct InputCaptureFilterImpl<M, options::input_capture_filter::FCKInt8> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b0011;
};

template <class M>
struct InputCaptureFilterImpl<M, options::input_capture_filter::FDTS2_N6> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b0100;
};

template <class M>
struct InputCaptureFilterImpl<M, options::input_capture_filter::FDTS2_N8> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b0101;
};

template <class M>
struct InputCaptureFilterImpl<M, options::input_capture_filter::FDTS4_N6> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b0110;
};

template <class M>
struct InputCaptureFilterImpl<M, options::input_capture_filter::FDTS4_N8> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b0111;
};

template <class M>
struct InputCaptureFilterImpl<M, options::input_capture_filter::FDTS8_N6> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b1000;
};

template <class M>
struct InputCaptureFilterImpl<M, options::input_capture_filter::FDTS8_N8> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b1001;
};

template <class M>
struct InputCaptureFilterImpl<M, options::input_capture_filter::FDTS16_N5> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b1010;
};

template <class M>
struct InputCaptureFilterImpl<M, options::input_capture_filter::FDTS16_N6> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b1011;
};

template <class M>
struct InputCaptureFilterImpl<M, options::input_capture_filter::FDTS16_N8> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b1100;
};

template <class M>
struct InputCaptureFilterImpl<M, options::input_capture_filter::FDTS32_N5> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b1101;
};

template <class M>
struct InputCaptureFilterImpl<M, options::input_capture_filter::FDTS32_N6> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b1110;
};

template <class M>
struct InputCaptureFilterImpl<M, options::input_capture_filter::FDTS32_N8> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b1111;
};

template <class M, class T>
struct InputMappingImpl {
  static constexpr bool    valid = std::is_same_v<M, options::mode::Output>;
  static constexpr uint8_t value = 0;
};

template <>
struct InputMappingImpl<options::mode::Input, void> {
  // Default options::input_mapping::Direct
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b01;
};

template <class M>
struct InputMappingImpl<M, options::input_mapping::Direct> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b01;
};

template <class M>
struct InputMappingImpl<M, options::input_mapping::Indirect> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b10;
};

template <class M>
struct InputMappingImpl<M, options::input_mapping::TRC> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b11;
};

template <class M, class T>
struct InputCapturePrescalerImpl {
  static constexpr bool    valid = std::is_same_v<M, options::mode::Output>;
  static constexpr uint8_t value = 0;
};

template <>
struct InputCapturePrescalerImpl<options::mode::Input, void> {
  // Default options::input_capture_prescaler::EveryValidEdge
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b00;
};

template <class M>
struct InputCapturePrescalerImpl<M, options::input_capture_prescaler::EveryValidEdge> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b00;
};

template <class M>
struct InputCapturePrescalerImpl<M, options::input_capture_prescaler::Every2ndEdge> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b01;
};

template <class M>
struct InputCapturePrescalerImpl<M, options::input_capture_prescaler::Every4thEdge> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b10;
};

template <class M>
struct InputCapturePrescalerImpl<M, options::input_capture_prescaler::Every8thEdge> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b11;
};

template <class M, class T>
struct OutputCompareModeImpl {
  static constexpr bool    valid = std::is_same_v<M, options::mode::Input>;
  static constexpr uint8_t value = 0;
};

template <class M>
struct OutputCompareModeImpl<M, options::output_compare_mode::Frozen> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b000;
};

template <class M>
struct OutputCompareModeImpl<M, options::output_compare_mode::Active> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b001;
};

template <class M>
struct OutputCompareModeImpl<M, options::output_compare_mode::Inactive> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b010;
};

template <class M>
struct OutputCompareModeImpl<M, options::output_compare_mode::Toggle> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b011;
};

template <class M>
struct OutputCompareModeImpl<M, options::output_compare_mode::PwmActiveHigh> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b110;
};

template <class M>
struct OutputCompareModeImpl<M, options::output_compare_mode::PwmActiveLow> {
  static constexpr bool    valid = true;
  static constexpr uint8_t value = 0b111;
};
}