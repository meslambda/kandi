#pragma once

#include <cinttypes>

#include "serialib.h"

class GPIBDevice
{
  public:
    ~GPIBDevice();

    void setAdapterDelay(const uint32_t delayMs);

    void        openDevice(const std::string& device);
    void        changeDevice(const std::string& device);
    std::string readString();
    void        writeCmd(const std::string& s);
    std::string queryCmd(const std::string& cmd);

    // std::string readStringFast(const uint32_t timeoutSeconds);

  private:
    serialib m_serial{};
    uint32_t m_adapterReadDelay = 1000;

    void closeDevice();
};