#pragma once
#include "IControl.h"
#include "IComboBox.h"
#include "IButton.h"
#include <functional>
#include <string>
#include <ctime>

// Forward declarations
class Texture2D;

// Structure to represent time (HH:MM:SS)
struct Time {
    int hours;     // 0-23
    int minutes;   // 0-59
    int seconds;   // 0-59

    // Constructor for current time
    Time();

    // Constructor with specific time
    Time(int h, int m, int s = 0) : hours(h), minutes(m), seconds(s) {
        Validate();
    }

    // Get formatted time string (e.g., "15:35:22")
    std::string ToString() const;

    // Get formatted time string without seconds (e.g., "15:35")
    std::string ToStringNoSeconds() const;

    // Validate and correct time values
    void Validate();

    // Increment seconds by 1, handling rollover
    void IncrementSecond();

    // Compare two times
    bool operator==(const Time& other) const {
        return hours == other.hours && minutes == other.minutes && seconds == other.seconds;
    }

    bool operator!=(const Time& other) const {
        return !(*this == other);
    }
};

class ITimeSelector : public IControl
{
public:
    ITimeSelector();
    ITimeSelector(glm::vec2 position, glm::vec2 size);
    virtual ~ITimeSelector();

    // Time management
    void SetTime(const Time& time);
    void SetTime(int hours, int minutes, int seconds = 0);
    Time GetTime() const;
    void ResetToCurrentTime();

    // Display options
    void SetShowSeconds(bool show) { m_ShowSeconds = show; }
    bool GetShowSeconds() const { return m_ShowSeconds; }

    // Auto-update options
    void SetAutoUpdateSeconds(bool autoUpdate) { m_AutoUpdateSeconds = autoUpdate; }
    bool GetAutoUpdateSeconds() const { return m_AutoUpdateSeconds; }

    // Event callbacks
    void SetOnTimeChanged(std::function<void(const Time&)> callback) { m_OnTimeChanged = callback; }

    // Appearance settings
    void SetLabelColor(const glm::vec4& color) { m_LabelColor = color; }
    void SetTimeColor(const glm::vec4& color) { m_TimeColor = color; }
    void SetBackgroundColor(const glm::vec4& color) { m_BackgroundColor = color; }
    void SetClockIcon(Texture2D* icon) { m_ClockIcon = icon; }

    // Overridden methods from IControl
    void Update(float delta) override;
    void Render() override;
    void PreRender() override;
    void OnMouseDown(int button) override;
    void OnMouseUp(int button) override;
    void OnMouseMove(glm::vec2 position, glm::vec2 delta) override;
    void AfterSet() override;

private:
    Time m_CurrentTime;
    Time m_EditTime;  // Temporary time while editing

    // State
    bool m_InEditMode;
    bool m_ShowSeconds;
    bool m_AutoUpdateSeconds;
    float m_SecondAccumulator;

    // UI elements
    IButton* m_EditButton;
    IButton* m_ApplyButton;
    IButton* m_CancelButton;
    IComboBox* m_HoursCombo;
    IComboBox* m_MinutesCombo;
    IComboBox* m_SecondsCombo;

    // Appearance settings
    glm::vec4 m_LabelColor;
    glm::vec4 m_TimeColor;
    glm::vec4 m_BackgroundColor;
    Texture2D* m_ClockIcon;

    // Event callbacks
    std::function<void(const Time&)> m_OnTimeChanged;

    // Internal helper methods
    void InitializeComponents();
    void UpdateTimeDisplay();
    void NotifyTimeChanged();
    void LayoutComponents();

    // Mode switching
    void EnterEditMode();
    void ExitEditMode(bool applyChanges);

    // Dropdown population
    void PopulateHourDropdown();
    void PopulateMinuteDropdown();
    void PopulateSecondDropdown();
};