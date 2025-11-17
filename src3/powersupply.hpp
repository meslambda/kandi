#pragma once

#include "gpibdevice.hpp"

#include <cinttypes>
#include <tuple>

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

  private:
    float       m_maxVoltage{};
    float       m_maxCurrent{};
    std::string m_channel{};

    float                    stringToFloat(std::string_view s);
    std::tuple<float, float> getMeasurements(const float    voltageStep,
                                             const bool     isCoolingBetweenMeasurements,
                                             const uint32_t delayBetweenMeasurementsSeconds);
};