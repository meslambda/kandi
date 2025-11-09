#include "powersupply.hpp"

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
    const float dV        = m_maxVoltage / steps;
    auto        gpibCmd   = std::format("APPL {},0,{:.1f};OUTP ON", m_channel, m_maxCurrent);
    const auto  startTime = std::chrono::steady_clock::now();
    writeCmd(gpibCmd);

    std::ofstream csv(logCsvPath);
    csv << "Time(s),Voltage(V),Current(A)\n";

    for (auto i = 1u; i <= steps; ++i)
    {
        const float voltage = dV * i;
        const auto  gpibCmd = std::format("VOLT {:.1f};*WAI;MEAS:CURR?", voltage);

        const auto measuredCurrent = stof(queryCmd(gpibCmd, 1));

        const auto readTime = std::chrono::steady_clock::now();
        const auto dt       = std::chrono::duration<double>(readTime - startTime).count();
        csv << dt << "," << voltage << "," << measuredCurrent << "\n"
            << std::flush;

        if (measuredCurrent >= m_maxCurrent - 0.1) // Depending on psu behaviour, this could have smaller limit than what user has set.
        {
            break;
        }

        handleCoolingAndDelay(isCoolingBetweenMeasurements, delayBetweenMeasurementsSeconds);
    }
    writeCmd("VOLT 0.0;OUTP OFF");
}

void PowerSupply::handleCoolingAndDelay(const bool isCooling, const uint32_t delaySeconds)
{
    if (isCooling)
    {
        writeCmd("VOLT 0.0;OUTP OFF");
        std::this_thread::sleep_for(std::chrono::seconds(delaySeconds));
        writeCmd("OUTP ON");
        return;
    }
    std::this_thread::sleep_for(std::chrono::seconds(delaySeconds));
}
