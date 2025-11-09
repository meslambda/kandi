#pragma once

#include <cinttypes>

#include "serialib.h"

class GPIBDevice
{
  public:
    ~GPIBDevice();

    void        openDevice(const std::string& device, const uint32_t baudRate);
    void        changeDevice(const std::string& device, const uint32_t baudRate);
    std::string readString(const uint32_t timeoutSeconds);
    void        writeCmd(const std::string& s);
    std::string queryCmd(const std::string& cmd, const uint32_t timeoutSeconds);

    // std::string readStringFast(const uint32_t timeoutSeconds);

  private:
    serialib m_serial{};
    // uint32_t m_adapterReadDelay = 500;

    void closeDevice();
};