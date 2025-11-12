#pragma once

#include "gui.hpp"
#include "powersupply.hpp"

#include <future>
#include <string>

class PsuGui : public Gui
{
  public:
    PsuGui();

    void render();

  private:
    PowerSupply       m_psu;
    std::future<void> m_measurementResult;

    int         m_delaySeconds                 = 0;
    bool        m_isCoolingBetweenMeasurements = true;
    float       m_maxVoltage                   = 0.0f;
    float       m_maxCurrent                   = 0.0f;
    int         m_steps                        = 0;
    std::string m_logPath                      = "../out/log.csv";
    const char* m_channels[3]                  = {"P6V", "P25V", "N25V"};
    int         m_channelIndex                 = 0;
    std::string m_device                       = "/dev/ttyUSB0";
    int         m_baudRate                     = 115200;
    int         m_adapterReadDelayMs           = 1000;

    bool m_isMeasuring = false;

    void renderUserSettings();
    void launchAsyncMeasThread();
};
