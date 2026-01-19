#include "gpibdevice.hpp"

#include <format>
#include <thread>

GPIBDevice::~GPIBDevice()
{
    m_serial.closeDevice();
}

void GPIBDevice::setGPIBAddr(const uint8_t addr)
{
    m_addr = addr;
    m_serial.writeString(std::format("++addr {}\n", m_addr).c_str());
}

void GPIBDevice::openDevice(const std::string& device)
{
    m_serial.openDevice(device.c_str(), 115200u); // Dummy baudrate
    m_serial.writeString("++savecfg 0\n");
    m_serial.writeString("++mode 1\n");
    m_serial.writeString("++auto 0\n");
    m_serial.writeString("++eoi 1\n");
    m_serial.writeString("++eos 3\n");
    m_serial.writeString("++read_tmo_ms 3000\n"); // Max delay
    m_serial.writeString(std::format("++addr {}\n", m_addr).c_str());

    writeCmd("*ESE 1;*SRE 48"); // OPC=2^0=1 and (MAV=2^4=16 + ESB=2^5=32) = 48
}

void GPIBDevice::changeDevice(const std::string& device)
{
    m_serial.closeDevice();
    openDevice(device);
}

std::string GPIBDevice::readString()
{
    m_serial.writeString("++read eoi\n");

    const auto  bufSize = 4096u;
    std::string buf(bufSize, '\0');

    const auto readLen = m_serial.readString(buf.data(), '\n', bufSize - 1u);
    if (readLen <= 0)
    {
        return std::string();
    }

    buf.resize(readLen - 1u);
    return buf;
}

void GPIBDevice::writeCmd(const std::string& cmd)
{
    constexpr auto srqPollDelayMs = 10u;

    m_serial.writeString((cmd + ";*WAI;*OPC" + '\n').c_str());
    while (!isSrqBitSet())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(srqPollDelayMs));
    }

    m_serial.writeString("*CLS\n");
    while (isSrqBitSet())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(srqPollDelayMs));
    }
}

std::string GPIBDevice::queryCmd(const std::string& cmd)
{
    constexpr auto srqPollDelayMs = 10u;
    m_serial.writeString((cmd + '\n').c_str());
    while (!isSrqBitSet())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(srqPollDelayMs));
    }
    return readString();
}

bool GPIBDevice::isSrqBitSet()
{
    constexpr auto srqReadSize = 4u; // bit + \n\r\0
    std::string    buf(srqReadSize, '0');
    m_serial.writeString("++srq\n");
    m_serial.readString(buf.data(), '\n', srqReadSize - 1u);
    return buf[0] != '0';
}
