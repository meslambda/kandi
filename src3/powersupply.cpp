#include "powersupply.hpp"

#include <charconv>
#include <chrono>
#include <format>
#include <fstream>
#include <thread>
#include <tuple>

void PowerSupply::setMaxVoltage(const double maxVoltage)
{
    m_maxVoltage = maxVoltage;
}

void PowerSupply::setMaxCurrent(const double maxCurrent)
{
    m_maxCurrent = maxCurrent;
}

void PowerSupply::setActiveChannel(const std::string& channel)
{
    m_channel = channel;
}

/*double PowerSupply::stringToFloat(std::string_view s) const // Tarviiko tätä?
{
    auto val = 0.0;
    // std::cout << s << std::endl;

    if (!s.empty() && s.front() == '+')
    {
        s.remove_prefix(1);
    }

    const auto [ptr, err] = std::from_chars(s.data(), s.data() + s.size(), val);
    return err == std::errc() ? val : 0.0;
}*/

void PowerSupply::psuWriteAndLog(const std::string& logCsvPath,
                                 const uint32_t     delaySeconds,
                                 const bool         isCooling,
                                 const uint32_t     steps)
{
    std::ofstream csv(logCsvPath);
    csv << "Time(s),Voltage(V),Current(A)\n";

    writeCmd(std::format("APPL {},0,{:.3f}", m_channel, m_maxCurrent));

    const auto start = std::chrono::steady_clock::now();
    const auto dV    = m_maxVoltage / steps;
    for (auto i = 1u; i <= steps; ++i)
    {
        const auto voltage = dV * i;

        const auto [measVoltage, measCurrent] =
            isCooling ? getCoolMeasurements(voltage, delaySeconds)
                      : getHotMeasurements(voltage, delaySeconds);

        const auto now = std::chrono::steady_clock::now();
        const auto elapsed =
            std::chrono::duration_cast<std::chrono::seconds>(now - start).count();

        csv << elapsed << ',' << measVoltage << ',' << measCurrent << '\n';
    }
    writeCmd("OUTP OFF;VOLT 0");
}

std::tuple<std::string, std::string> PowerSupply::getCoolMeasurements(
    const double   voltageStep,
    const uint32_t sleepSeconds)
{
    const auto current = queryCmd(
        std::format("VOLT {:.6f};OUTP ON;*WAI;MEAS:CURR?", voltageStep)); // desimaalit
    const auto voltage = queryCmd("MEAS:VOLT?");

    writeCmd("OUTP OFF");
    std::this_thread::sleep_for(std::chrono::seconds(sleepSeconds));

    return std::make_tuple(voltage, current);
}

std::tuple<std::string, std::string> PowerSupply::getHotMeasurements(
    const double   voltageStep,
    const uint32_t sleepSeconds)
{
    const auto gpibCmd = std::format("VOLT {:.6f};OUTP ON", voltageStep);
    writeCmd(gpibCmd);
    std::this_thread::sleep_for(std::chrono::seconds(sleepSeconds));

    const auto current = queryCmd("MEAS:CURR?");
    const auto voltage = queryCmd("MEAS:VOLT?");
    return std::make_tuple(voltage, current);
}
