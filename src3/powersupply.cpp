#include "powersupply.hpp"

#include <charconv>
#include <chrono>
#include <format>
#include <fstream>
#include <thread>

void PowerSupply::setMaxVoltage(const float maxVoltage)
{
    m_maxVoltage = maxVoltage;
}

void PowerSupply::setMaxCurrent(const float maxCurrent)
{
    m_maxCurrent = maxCurrent;
}

void PowerSupply::setActiveChannel(const std::string& channel)
{
    m_channel = channel;
}

void PowerSupply::psuWriteAndLog(const std::string& logCsvPath,
                                 const uint32_t     delayBetweenMeasurementsSeconds,
                                 const bool         isCoolingBetweenMeasurements,
                                 const uint32_t     steps)
{
    const auto initPsuCmd = std::format("APPL {},0,{:.3f}", m_channel, m_maxCurrent);
    writeCmd(initPsuCmd); // Set initial channel and max current

    std::ofstream csv(logCsvPath);
    csv << "Time(s),Voltage(V),Current(A)\n";

    const auto dV = m_maxVoltage / steps;
    for (auto i = 1u; i <= steps; ++i)
    {
        const auto voltage         = dV * i;
        auto       measuredCurrent = 0.0f;

        if (isCoolingBetweenMeasurements)
        {
            const auto gpibCmd = std::format("VOLT {:.3f};OUTP ON;*WAI;MEAS:CURR?", voltage);
            measuredCurrent    = stringToFloat(queryCmd(gpibCmd));
            writeCmd("OUTP OFF");
            std::this_thread::sleep_for(std::chrono::seconds(delayBetweenMeasurementsSeconds));
        }
        else
        {
            const auto gpibCmd = std::format("VOLT {:.3f};*WAI;OUTP ON", voltage);
            writeCmd(gpibCmd);
            std::this_thread::sleep_for(std::chrono::seconds(delayBetweenMeasurementsSeconds));
            measuredCurrent = stringToFloat(queryCmd("MEAS:CURR?"));
        }

        csv << (i - 1) * delayBetweenMeasurementsSeconds << "," << voltage << "," << measuredCurrent << "\n";
    }

    writeCmd("OUTP OFF;VOLT 0;CURR 0;*WAI");
}

float PowerSupply::stringToFloat(std::string_view s)
{
    if (!s.empty() && s.front() == '+')
    {
        s.remove_prefix(1);
    }

    auto val       = 0.0f;
    auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), val);
    /*if (ec != std::errc()) {
        // clear data buffers // might not work with incoming queries
    }*/
    return ec == std::errc() ? val : 0.0f;
}
