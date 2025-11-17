#include "psugui.hpp"

#include "imgui_stdlib.h"

PsuGui::PsuGui() : Gui("PSU controller")
{
    m_psu.openDevice(m_device);

    m_psu.setMaxVoltage(0);
    m_psu.setMaxCurrent(0);
    m_psu.setActiveChannel("P6V");
    m_psu.setAdapterDelay(1000);
}

void PsuGui::renderUserSettings()
{
    ImGui::BeginTable("ControlsTable", 2, ImGuiTableFlags_BordersInnerV);
    ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, m_mainScale * 280);
    ImGui::TableSetupColumn("Control", ImGuiTableColumnFlags_WidthStretch);

    tableHelper("Delay between measurements(s): ");
    if (ImGui::InputInt("##delay", &m_delaySeconds))
    {
        if (m_delaySeconds < 0)
        {
            m_delaySeconds = 0;
        }
    }

    tableHelper("Adapter read buffer delay(ms): ");
    if (ImGui::InputInt("##adapterDelay", &m_adapterReadDelayMs))
    {
        if (m_adapterReadDelayMs < 1)
        {
            m_adapterReadDelayMs = 1;
        }
        m_psu.setAdapterDelay(m_adapterReadDelayMs);
    }

    tableHelper("Device: ");
    if (ImGui::InputText("##device", &m_device))
    {
        m_psu.changeDevice(m_device);
    }

    tableHelper("Set output to 0V between measurements: ");
    if (ImGui::Button(m_isCoolingBetweenMeasurements ? "YES" : "NO"))
    {
        m_isCoolingBetweenMeasurements = !m_isCoolingBetweenMeasurements;
    }

    tableHelper("Log file location: ");
    ImGui::InputText("##logPath", &m_logPath);

    tableHelper("Max voltage(V): ");
    if (ImGui::InputFloat("##maxVoltage", &m_maxVoltage, 0.001f, 0.01f, "%.3f"))
    {
        if (m_maxVoltage < 0.0f)
        {
            m_maxVoltage = 0.0f;
        }
        m_psu.setMaxVoltage(m_maxVoltage);
    }

    tableHelper("Max current(A): ");
    if (ImGui::InputFloat("##maxCurrent", &m_maxCurrent, 0.001f, 0.01f, "%.3f"))
    {
        if (m_maxCurrent < 0.0f)
        {
            m_maxCurrent = 0.0f;
        }
        m_psu.setMaxCurrent(m_maxCurrent);
    }

    tableHelper("Number of steps: ");
    if (ImGui::InputInt("##steps", &m_steps))
    {
        if (m_steps < 0)
        {
            m_steps = 0;
        }
    }

    tableHelper("Channel: ");
    if (ImGui::Combo("##channel", &m_channelIndex, m_channels, IM_ARRAYSIZE(m_channels)))
    {
        m_psu.setActiveChannel(m_channels[m_channelIndex]);
    }

    ImGui::EndTable();
}

void PsuGui::render()
{
    auto viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::Begin("PSU Controller", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
    renderUserSettings();
    launchAsyncMeasThread();
    ImGui::End();
}

void PsuGui::launchAsyncMeasThread()
{
    if (!m_isMeasuring)
    {
        if (ImGui::Button("Start measurement"))
        {
            m_isMeasuring       = true;
            m_measurementResult = std::async(std::launch::async,
                                             &PowerSupply::psuWriteAndLog,
                                             &m_psu,
                                             m_logPath,
                                             m_delaySeconds,
                                             m_isCoolingBetweenMeasurements,
                                             m_steps);
        }
    }
    else
    {
        ImGui::BeginDisabled();
        ImGui::Button("Start measurement");
        ImGui::EndDisabled();
        ImGui::Text("Measuring...");

        if (m_measurementResult.valid())
        {
            if (m_measurementResult.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
            {
                m_isMeasuring = false;
            }
        }
    }
}
