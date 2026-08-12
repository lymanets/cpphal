#pragma once

namespace hal::uart::events {
struct RxNotEmpty {
};

struct TxEmpty {
};

struct TransmissionComplete {
};

struct Idle {
};

struct ParityError {
};

struct Error {
};
}