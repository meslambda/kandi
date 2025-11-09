#include "gpibdevice.hpp"

#include <chrono>
#include <thread>

GPIBDevice::~GPIBDevice()
{
    closeDevice();
}

void GPIBDevice::openDevice(const std::string& device, const uint32_t baudRate)
{
    m_serial.openDevice(device.c_str(), baudRate);

    writeCmd("++mode 1");
    writeCmd("++addr 5");
    writeCmd("++auto 0");
    writeCmd("++eoi 1");
    writeCmd("++eos 3");
    writeCmd("++read_tmo_ms 500");
    writeCmd("++savecfg 0");
}

void GPIBDevice::changeDevice(const std::string& device, const uint32_t baudRate)
{
    closeDevice();
    openDevice(device, baudRate);
}

std::string GPIBDevice::readString(const uint32_t timeoutSeconds)
{
    constexpr auto bufferSize = 2048u;
    char           buf[bufferSize]{};

    writeCmd("++read eoi");
    auto        readLen = m_serial.readString(buf, '\n', bufferSize - 1, timeoutSeconds * 1000);
    std::string s(buf, readLen);
    if (s.back() == '\n')
    {
        s.pop_back();
    }
    return s;
}

void GPIBDevice::writeCmd(const std::string& s)
{
    std::string out = s;
    if (out.back() != '\n')
    {
        out.push_back('\n');
    }
    m_serial.writeString(out.c_str());
}

std::string GPIBDevice::queryCmd(const std::string& cmd, const uint32_t timeoutSeconds)
{
    writeCmd(cmd);
    return readString(timeoutSeconds);
}

void GPIBDevice::closeDevice()
{
    m_serial.closeDevice();
}

/* If needed, this can be used to manually set read lenght timeout by setting read_tmo_ms low on adapter side and
 * controlling the timeout manually here. Lowest delays seems to be ~300 ms.
 */
/*std::string GPIBDevice::readStringFast(const uint32_t timeoutSeconds)
{
    constexpr auto bufferSize = 2048u;
    char           buf[bufferSize]{};
    auto           startTime = std::chrono::steady_clock::now();

    while (std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime).count() < timeoutSeconds)
    {
        writeCmd("++read eoi"); // poll on every loop

        auto readLen = m_serial.readString(buf, '\n', bufferSize - 1, m_adapterReadDelay); // 3rd param timeout is value here + read_tmo_ms
        if (readLen > 1)
        {
            std::string s(buf, readLen);
            if (s.back() == '\n')
            {
                s.pop_back();
            }
            return s;
        }
    }
    return "readString timeout";
}*/