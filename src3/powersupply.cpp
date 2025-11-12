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
    constexpr auto maxCurrentError = 0.1f;
    const float    dV              = m_maxVoltage / steps;

    const auto gpibCmd = std::format("APPL {},0,{:.1f};OUTP ON", m_channel, m_maxCurrent);
    writeCmd(gpibCmd);

    std::ofstream csv(logCsvPath);
    csv << "Time(s),Voltage(V),Current(A)\n";

    size_t i               = 1;
    auto   measuredCurrent = 0.0f;
    while (i <= steps && measuredCurrent < m_maxCurrent - maxCurrentError)
    {
        const float voltage = dV * i;

        const auto gpibCmd = std::format("VOLT {:.1f};*WAI;MEAS:CURR?", voltage);
        measuredCurrent    = stof(queryCmd(gpibCmd));
        handleCoolingAndDelay(isCoolingBetweenMeasurements, delayBetweenMeasurementsSeconds);

        csv << (i - 1) * delayBetweenMeasurementsSeconds << "," << voltage << "," << measuredCurrent << "\n";
        i++;
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
