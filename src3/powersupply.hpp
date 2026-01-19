#pragma once

#include "gpibdevice.hpp"

#include <cinttypes>
#include <tuple>

class PowerSupply : public GPIBDevice
{
  public:
    void setMaxVoltage(const double maxVoltage);
    void setMaxCurrent(const double maxCurrent);
    void setActiveChannel(const std::string& channel);

    void psuWriteAndLog(const std::string& logCsvPath,
                        const uint32_t     delayBetweenMeasurementsSeconds,
                        const bool         isCoolingBetweenMeasurements,
                        const uint32_t     steps);

  private:
    double      m_maxVoltage{};
    double      m_maxCurrent{};
    std::string m_channel{};

    std::tuple<std::string, std::string> getCoolMeasurements(const double voltageStep, const uint32_t sleepSeconds);
    std::tuple<std::string, std::string> getHotMeasurements(const double voltageStep, const uint32_t sleepSeconds);
};