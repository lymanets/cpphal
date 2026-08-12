#pragma once
#include "pin.hpp"

// template <class Port, std::uint8_t Number>
// struct hal::gpio::PinImpl<hal::gpio::STM32V2Policy, Port, Number> {
//   static void set_mode(Mode m) {
//     constexpr unsigned o = Number;
//     switch (m) {
//       case Mode::Input:
//         Port::MODER::template Field<o, 2>::write(0);
//         break;
//       case Mode::Output:
//         Port::MODER::template Field<o, 2>::write(1);
//         break;
//       case Mode::Alternate:
//         Port::MODER::template Field<o, 2>::write(2);
//         break;
//       case Mode::Analog:
//         Port::MODER::template Field<o, 2>::write(3);
//         break;
//     }
//   }
//
//   static void set_pull(Pull p) {
//     constexpr unsigned o = Number;
//     switch (p) {
//       case Pull::None:
//         Port::PUPDR::template Field<o, 2>::write(0);
//         break;
//       case Pull::Up:
//         Port::PUPDR::template Field<o, 2>::write(1);
//         break;
//       case Pull::Down:
//         Port::PUPDR::template Field<o, 2>::write(2);
//         break;
//     }
//   }
//
//   static void set_speed(Speed s) {
//     constexpr unsigned o = Number;
//     switch (s) {
//       case Speed::Low:
//         Port::OSPEEDR::template Field<o, 2>::write(0);
//         break;
//       case Speed::Medium:
//         Port::OSPEEDR::template Field<o, 2>::write(1);
//         break;
//       case Speed::High:
//         Port::OSPEEDR::template Field<o, 2>::write(2);
//         break;
//       case Speed::VeryHigh:
//         Port::OSPEEDR::template Field<o, 2>::write(3);
//         break;
//     }
//   }
//
//   static void set_output_type(OutputType t) {
//     if (t == OutputType::PushPull) Port::OTYPER::template Bit<Number>::reset();
//     else Port::OTYPER::template Bit<Number>::set();
//   }
//
//   template <class AF>
//   static void set_alternate() {
//     if constexpr (!std::is_same_v<AF, NoAlternate>) {
//       if constexpr (Number < 8) Port::AFRL::template Field<Number * 4, 4>::write(AF::value);
//       else Port::AFRH::template Field<(Number - 8) * 4, 4>::write(AF::value);
//     }
//   }
//
//   template <Mode M, Pull P = Pull::None, Speed S = Speed::Low,
//             OutputType OT = OutputType::PushPull, class AF = NoAlternate>
//   static void configure() {
//     Port::enable_clock();
//     set_mode(M);
//     set_pull(P);
//     if constexpr (M != Mode::Input) set_speed(S);
//     if constexpr (M == Mode::Output || M == Mode::Alternate) set_output_type(OT);
//     if constexpr (M == Mode::Alternate) set_alternate<AF>();
//   }
// }; // namespace hal::gpio