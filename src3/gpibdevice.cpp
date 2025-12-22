#include "gpibdevice.hpp"

#include <format>
#include <thread>

GPIBDevice::~GPIBDevice()
{
    closeDevice();
}

void GPIBDevice::setAdapterDelay(const uint32_t delayMs)
{
    const auto gpibCmd = std::format("++read_tmo_ms {}", delayMs);
    writeCmd(gpibCmd);
    m_adapterReadDelay = delayMs;
}

void GPIBDevice::openDevice(const std::string& device)
{
    constexpr auto baudRate = 115200u;
    m_serial.openDevice(device.c_str(), baudRate);

    writeCmd("++mode 1");    // Controller mode
    writeCmd("++addr 5");    // Used GPIB address
    writeCmd("++auto 0");    // Autoread OFF
    writeCmd("++eoi 1");     // Enable EOI insertion
    writeCmd("++eos 3");     // Don't append instrument commands
    writeCmd("++savecfg 0"); // Autosave to EEPROM OFF

    // Read delay
    writeCmd(std::format("++read_tmo_ms {}", m_adapterReadDelay));
}

void GPIBDevice::changeDevice(const std::string& device)
{
    closeDevice();
    openDevice(device);
}

std::string GPIBDevice::readString()
{
    constexpr auto bufSize = 4096u; // Hardware buffer max size
    std::string    buf(bufSize, '\0');

    writeCmd("++read eoi");
    const auto readLen = m_serial.readString(buf.data(), '\n', bufSize - 1, m_adapterReadDelay);

    if (readLen <= 0)
    {
        return std::string();
    }

    buf.resize(readLen);
    if (buf.back() == '\n')
    {
        buf.pop_back();
    }
    return buf;
}

void GPIBDevice::writeCmd(const std::string& s)
{
    auto out = s;
    if (!out.empty() && out.back() != '\n')
    {
        out.push_back('\n');
    }
    m_serial.writeString(out.c_str());
}

std::string GPIBDevice::queryCmd(const std::string& cmd)
{
    writeCmd(cmd);
    return readString();
}

void GPIBDevice::closeDevice()
{
    m_serial.closeDevice();
}
