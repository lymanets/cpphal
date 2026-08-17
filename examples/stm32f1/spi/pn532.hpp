#pragma once


template <class INTERFACE, class CS, class SysTimer, class Logger>
struct PN532 {
private:
  enum Command : uint8_t {
    None,
    GetFirmwareVersion = 0x02
  };

  static inline Command    command_    = None;
  static constexpr uint8_t STATUS_READ = 2;
  static constexpr uint8_t DATA_WRITE  = 1;
  static constexpr uint8_t DATA_READ   = 3;

  static constexpr uint8_t Preamble   = 0x00;
  static constexpr uint8_t StartCode1 = 0x00;
  static constexpr uint8_t StartCode2 = 0xFF;
  static constexpr uint8_t Postamble  = 0x00;

  static constexpr int16_t FromHost = 0xD4;
  static constexpr int16_t ToHost   = 0xD5;

  static constexpr uint8_t AckWaitTime = 10; // ms, timeout of waiting for ACK

  static constexpr int16_t InvalidAck   = -1;
  static constexpr int16_t Timeout      = -2;
  static constexpr int16_t InvalidFrame = -3;
  static constexpr int16_t NoSpace      = -4;

  static inline uint8_t packetBuffer_[64];

public:
  static void init() {
  }

  static uint8_t write(uint8_t data) {
    return INTERFACE::transfer(data);
  }

  static uint8_t read() {
    return INTERFACE::transfer(0);
  }

  static void wakeup() {
    CS::reset();
    SysTimer::delay_ms(2);
    CS::set();
  }

  static bool isReady() {
    CS::reset();
    SysTimer::delay_ms(1);
    uint8_t status = write(STATUS_READ) & 0x1;
    CS::set();
    return status;
  }

  static int8_t writeCommand(const uint8_t* header, uint8_t hlen, const uint8_t* body = nullptr, uint8_t blen = 0) {
    command_ = static_cast<Command>(header[0]);
    writeFrame(header, hlen, body, blen);

    uint8_t timeout = AckWaitTime;
    while (!isReady()) {
      SysTimer::delay_ms(1);
      timeout--;
      if (0 == timeout) {
        Logger::write("Time out when waiting for ACK\n");
        return -2;
      }
    }
    if (readAckFrame()) {
      Logger::write("Invalid ACK\n");
      return InvalidAck;
    }
    return 0;
  }

  static void writeFrame(const uint8_t* header, uint8_t hlen, const uint8_t* body, uint8_t blen) {
    CS::reset();
    SysTimer::delay_ms(2); // wake up PN532

    write(DATA_WRITE);
    write(Preamble);
    write(StartCode1);
    write(StartCode2);

    uint8_t length = hlen + blen + 1; // length of data field: TFI + DATA
    write(length);
    write(~length + 1); // checksum of length

    write(FromHost);
    uint8_t sum = FromHost; // sum of TFI + DATA

    Logger::write("write: ");

    for (uint8_t i = 0; i < hlen; i++) {
      write(header[i]);
      sum += header[i];

      // DMSG_HEX(header[i]);
    }
    for (uint8_t i = 0; i < blen; i++) {
      write(body[i]);
      sum += body[i];

      // DMSG_HEX(body[i]);
    }

    uint8_t checksum = ~sum + 1; // checksum of TFI + DATA
    write(checksum);
    write(Postamble);

    CS::set();

    Logger::write("\n");
  }

  static int8_t readAckFrame() {
    constexpr uint8_t ack[] = {0, 0, 0xFF, 0, 0xFF, 0};

    uint8_t ackBuf[sizeof(ack)];

    CS::reset();
    SysTimer::delay_ms(1);
    write(DATA_READ);

    for (size_t i = 0; i < sizeof(ack); i++) {
      ackBuf[i] = read();
    }

    CS::set();

    return static_cast<int8_t>(memcmp(ackBuf, ack, sizeof(ack)));
  }

  static int16_t readResponse(uint8_t buf[], uint8_t len, uint16_t timeout = 1000) {
    uint16_t time = 0;
    while (!isReady()) {
      SysTimer::delay_ms(1);
      time++;
      if (timeout > 0 && time > timeout) {
        return Timeout;
      }
    }

    CS::reset();
    SysTimer::delay_ms(1);

    int16_t result;
    do {
      write(DATA_READ);

      if (0x00 != read() || // PREAMBLE
          0x00 != read() || // STARTCODE1
          0xFF != read()    // STARTCODE2
      ) {
        result = InvalidFrame;
        break;
      }

      uint8_t length = read();
      if (0 != (uint8_t)(length + read())) {
        // checksum of length
        result = InvalidFrame;
        break;
      }

      uint8_t cmd = command_ + 1; // response command
      if (ToHost != read() || (cmd) != read()) {
        result = InvalidFrame;
        break;
      }

      Logger::write("read:  ");
      // DMSG_HEX(cmd);

      length -= 2;
      if (length > len) {
        // for (uint8_t i = 0; i < length; i++) {
        // DMSG_HEX(read()); // dump message
        // }
        Logger::write("\nNot enough space\n");
        read();
        read();
        result = NoSpace; // not enough space
        break;
      }

      uint8_t sum = ToHost + cmd;
      for (uint8_t i = 0; i < length; i++) {
        buf[i] = read();
        sum    += buf[i];

        // DMSG_HEX(buf[i]);
      }
      Logger::write("\n");

      uint8_t checksum = read();
      if (0 != (uint8_t)(sum + checksum)) {
        Logger::write("checksum is not ok\n");
        result = InvalidFrame;
        break;
      }
      read(); // POSTAMBLE

      result = length;
    } while (0);

    CS::set();

    return result;
  }

  static uint32_t getFirmwareVersion() {
    uint32_t response;

    packetBuffer_[0] = Command::GetFirmwareVersion;

    if (writeCommand(packetBuffer_, 1) == 0) {
      return 0;
    }

    // read data packet
    int16_t status = readResponse(packetBuffer_, sizeof(packetBuffer_));
    if (0 > status) {
      return 0;
    }

    response = packetBuffer_[0];
    response <<= 8;
    response |= packetBuffer_[1];
    response <<= 8;
    response |= packetBuffer_[2];
    response <<= 8;
    response |= packetBuffer_[3];

    return response;
  }
};