#pragma once

#include <cinttypes>

#include "serialib.h"

class GPIBDevice
{
  public:
    ~GPIBDevice();

    void        setGPIBAddr(const uint8_t addr);
    void        openDevice(const std::string& device);
    void        changeDevice(const std::string& device);
    std::string queryCmd(const std::string& cmd);
    void        writeCmd(const std::string& cmd);

  private:
    serialib m_serial{};
    uint8_t  m_addr = 5u;

    bool        isSrqBitSet();
    std::string readString();
};