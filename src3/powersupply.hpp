#pragma once

#include "gpibdevice.hpp"

#include <cinttypes>

class PowerSupply : public GPIBDevice
{
  public:
    void setMaxVoltage(const float maxVoltage);
    void setMaxCurrent(const float maxCurrent);
    void setActiveChannel(const std::string& channel);

    void psuWriteAndLog(const std::string& logCsvPath,
                        const uint32_t     delayBetweenMeasurementsSeconds,
                        const bool         isCoolingBetweenMeasurements,
                        const uint32_t     steps);
    // void setOn();

  private:
    float       m_maxVoltage{};
    float       m_maxCurrent{};
    std::string m_channel{};

    void  handleCoolingAndDelay(const bool isCooling, const uint32_t delaySeconds);
    float stringToFloat(const std::string& s);
};