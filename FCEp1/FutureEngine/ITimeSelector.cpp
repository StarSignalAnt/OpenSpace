#include "ITimeSelector.h"
#include "UIHelp.h"
#include "GameInput.h"
#include "Texture2D.h"
#include <ctime>
#include <iomanip>
#include <sstream>

// Time struct implementation
Time::Time() {
    // Get current time
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);

    hours = now->tm_hour;    // 0-23
    minutes = now->tm_min;   // 0-59
    seconds = now->tm_sec;   // 0-59
}

std::string Time::ToString() const {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hours << ":"
        << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setfill('0') << std::setw(2) << seconds;
    return oss.str();
}

std::string Time::ToStringNoSeconds() const {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hours << ":"
        << std::setfill('0') << std::setw(2) << minutes;
    return oss.str();
}

void Time::Validate() {
    // Constrain hours to 0-23
    hours = (hours < 0) ? 0 : (hours > 23) ? 23 : hours;

    // Constrain minutes to 0-59
    minutes = (minutes < 0) ? 0 : (minutes > 59) ? 59 : minutes;

    // Constrain seconds to 0-59
    seconds = (seconds < 0) ? 0 : (seconds > 59) ? 59 : seconds;
}

void Time::IncrementSecond() {
    seconds++;
    if (seconds >= 60) {
        seconds = 0;
        minutes++;
        if (minutes >= 60) {
            minutes = 0;
            hours = (hours + 1) % 24;
        }
    }
}

// ITimeSelector implementation
ITimeSelector::ITimeSelector()
    : IControl(),
    m_CurrentTime(),
    m_EditTime(),
    m_InEditMode(false),
    m_ShowSeconds(true),
    m_AutoUpdateSeconds(true),
    m_SecondAccumulator(0.0f),
    m_EditButton(nullptr),
    m_ApplyButton(nullptr),
    m_CancelButton(nullptr),
    m_HoursCombo(nullptr),
    m_MinutesCombo(nullptr),
    m_SecondsCombo(nullptr),
    m_LabelColor(glm::vec4(0.9f, 0.9f, 0.9f, 1.0f)),
    m_TimeColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)),
    m_BackgroundColor(glm::vec4(0.2f, 0.2f, 0.2f, 0.8f)),
    m_ClockIcon(nullptr)
{
    InitializeComponents();
}

ITimeSelector::ITimeSelector(glm::vec2 position, glm::vec2 size)
    : IControl(position, size),
    m_CurrentTime(),
    m_EditTime(),
    m_InEditMode(false),
    m_ShowSeconds(true),
    m_AutoUpdateSeconds(true),
    m_SecondAccumulator(0.0f),
    m_EditButton(nullptr),
    m_ApplyButton(nullptr),
    m_CancelButton(nullptr),
    m_HoursCombo(nullptr),
    m_MinutesCombo(nullptr),
    m_SecondsCombo(nullptr),
    m_LabelColor(glm::vec4(0.9f, 0.9f, 0.9f, 1.0f)),
    m_TimeColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)),
    m_BackgroundColor(glm::vec4(0.2f, 0.2f, 0.2f, 0.8f)),
    m_ClockIcon(nullptr)
{
    InitializeComponents();
}

ITimeSelector::~ITimeSelector()
{
    // Child controls will be deleted by IControl destructor
}

void ITimeSelector::SetTime(const Time& time)
{
    if (m_CurrentTime != time) {
        m_CurrentTime = time;
        m_CurrentTime.Validate();

        if (m_InEditMode) {
            // Update dropdowns to reflect the new time
            if (m_HoursCombo) m_HoursCombo->SetSelectedIndex(m_CurrentTime.hours);
            if (m_MinutesCombo) m_MinutesCombo->SetSelectedIndex(m_CurrentTime.minutes);
            if (m_SecondsCombo) m_SecondsCombo->SetSelectedIndex(m_CurrentTime.seconds);
        }

        NotifyTimeChanged();
    }
}

void ITimeSelector::SetTime(int hours, int minutes, int seconds)
{
    SetTime(Time(hours, minutes, seconds));
}

Time ITimeSelector::GetTime() const
{
    return m_CurrentTime;
}

void ITimeSelector::ResetToCurrentTime()
{
    SetTime(Time());
}

void ITimeSelector::Update(float delta)
{
    // Auto-update seconds if enabled and not in edit mode
    if (m_AutoUpdateSeconds && !m_InEditMode) {
        m_SecondAccumulator += delta;

        // Update once per second
        if (m_SecondAccumulator >= 1.0f) {
            m_SecondAccumulator -= 1.0f;

            // Only update if seconds display is enabled
            if (m_ShowSeconds) {
                m_CurrentTime.IncrementSecond();
                NotifyTimeChanged();
            }
        }
    }

    // Update child controls
    UpdateChildren(delta);
}

void ITimeSelector::Render()
{
    glm::vec2 renderPos = GetRenderPosition();

    // Draw background
    UIHelp::DrawRect(renderPos, m_Size, m_BackgroundColor);
    UIHelp::DrawOutlineRect(renderPos, m_Size, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

    // Determine if we need to render in normal or edit mode
    if (!m_InEditMode) {
        // Normal mode - display the current time with edit button

        // Draw clock icon if available
        float iconOffset = 0.0f;
        if (m_ClockIcon) {
            float iconSize = m_Size.y * 0.7f;
            glm::vec2 iconPos = renderPos + glm::vec2(5.0f, (m_Size.y - iconSize) / 2.0f);
            UIHelp::DrawImage(iconPos, glm::vec2(iconSize, iconSize), m_ClockIcon, glm::vec4(1.0f));
            iconOffset = iconSize + 10.0f;
        }

        // Draw time string
        std::string timeStr = m_ShowSeconds ? m_CurrentTime.ToString() : m_CurrentTime.ToStringNoSeconds();
        float timeWidth = UIHelp::StrWidth(timeStr);
        float textY = (m_Size.y - UIHelp::StrHeight(timeStr)) / 2.0f;

        UIHelp::DrawText(
            renderPos + glm::vec2(iconOffset+8, textY),
            timeStr,
            m_TimeColor
        );

        // The edit button is handled as a child control and renders itself
    }
    else {
        // Edit mode - combo boxes and apply/cancel buttons are handled as child controls
        // No need to render anything special here
    }

    // Render child controls (combo boxes, buttons)
    RenderChildren();
}

void ITimeSelector::PreRender()
{
    IControl::PreRender();
}

void ITimeSelector::OnMouseDown(int button)
{
    // Mouse down is handled by child controls
}

void ITimeSelector::OnMouseUp(int button)
{
    // Mouse up is handled by child controls
}

void ITimeSelector::OnMouseMove(glm::vec2 position, glm::vec2 delta)
{
    // Mouse move is handled by child controls
}

void ITimeSelector::AfterSet()
{
    // Update layout when control size changes
    LayoutComponents();
}

void ITimeSelector::InitializeComponents()
{
    // Create edit button
    m_EditButton = new IButton("Edit",glm::vec2(0, 0), glm::vec2(60, 20));
    m_EditButton->SetOnClick([this](void*data) {
        EnterEditMode();
        });

    // Create apply button (initially hidden)
    m_ApplyButton = new IButton("Apply",glm::vec2(0, 0), glm::vec2(60, 20));
    m_ApplyButton->SetOnClick([this](void *data) {
        ExitEditMode(true); // Apply changes
        });

    // Create cancel button (initially hidden)
    m_CancelButton = new IButton("Cancel",glm::vec2(0, 0), glm::vec2(60, 20));
    m_CancelButton->SetOnClick([this](void* data) {
        ExitEditMode(false); // Discard changes
        });

    // Create combo boxes for hour, minute, and second
    m_HoursCombo = new IComboBox(glm::vec2(0, 0), glm::vec2(60, 20));
    m_MinutesCombo = new IComboBox(glm::vec2(0, 0), glm::vec2(60, 20));
    m_SecondsCombo = new IComboBox(glm::vec2(0, 0), glm::vec2(60, 20));

    // Populate combo boxes
    PopulateHourDropdown();
    PopulateMinuteDropdown();
    PopulateSecondDropdown();

    // Set combo box callbacks
    m_HoursCombo->SetOnSelectionChanged([this](int index, std::string value) {
        m_EditTime.hours = index;
        });

    m_MinutesCombo->SetOnSelectionChanged([this](int index, std::string value) {
        m_EditTime.minutes = index;
        });

    m_SecondsCombo->SetOnSelectionChanged([this](int index, std::string value) {
        m_EditTime.seconds = index;
        });

    // Configure dropdown height with scrollers
    m_HoursCombo->SetMaxDropdownHeight(150);
    m_MinutesCombo->SetMaxDropdownHeight(150);
    m_SecondsCombo->SetMaxDropdownHeight(150);
    m_HoursCombo->SetUseScroller(true);
    m_MinutesCombo->SetUseScroller(true);
    m_SecondsCombo->SetUseScroller(true);

    // Add only the edit button initially
    AddChild(m_EditButton);

    // Start in display mode
    m_InEditMode = false;

    // Initial layout
    LayoutComponents();
}

void ITimeSelector::NotifyTimeChanged()
{
    if (m_OnTimeChanged) {
        m_OnTimeChanged(m_CurrentTime);
    }
}

void ITimeSelector::LayoutComponents()
{
    // Position components based on control size and current mode
    if (!m_InEditMode) {
        // Display mode - just position the edit button on the right side
        float editButtonWidth = 60.0f;
        float padding = 5.0f;
        m_EditButton->Set(glm::vec2(m_Size.x - editButtonWidth - padding, (m_Size.y - 20.0f) / 2.0f),
            glm::vec2(editButtonWidth, 20.0f));
    }
    else {
        // Edit mode - position combo boxes and buttons
        float comboWidth = 50.0f;
        float comboHeight = 25.0f;
        float buttonWidth = 60.0f;
        float buttonHeight = 25.0f;
        float padding = 5.0f;
        float totalWidth = m_Size.x - 2 * padding;

        // Allocate space for components
        float availableWidth;
        if (m_ShowSeconds) {
            // Three combo boxes plus two buttons
            availableWidth = totalWidth - 4 * padding; // 4 spaces between 5 components
            float componentWidth = availableWidth / 5.0f;

            comboWidth = componentWidth;
            buttonWidth = componentWidth;
        }
        else {
            // Two combo boxes plus two buttons
            availableWidth = totalWidth - 3 * padding; // 3 spaces between 4 components
            float componentWidth = availableWidth / 4.0f;

            comboWidth = componentWidth;
            buttonWidth = componentWidth;
        }

        // Postion elements from left to right
        float xPos = padding;
        float yPos = (m_Size.y - comboHeight) / 2.0f;

        // Hours combo
        m_HoursCombo->Set(glm::vec2(xPos, yPos), glm::vec2(comboWidth, comboHeight));
        xPos += comboWidth + padding;

        // Minutes combo
        m_MinutesCombo->Set(glm::vec2(xPos, yPos), glm::vec2(comboWidth, comboHeight));
        xPos += comboWidth + padding;

        // Seconds combo (if shown)
        if (m_ShowSeconds) {
            m_SecondsCombo->Set(glm::vec2(xPos, yPos), glm::vec2(comboWidth, comboHeight));
            xPos += comboWidth + padding;
        }

        // Apply button
        m_ApplyButton->Set(glm::vec2(xPos, yPos), glm::vec2(buttonWidth, buttonHeight));
        xPos += buttonWidth + padding;

        // Cancel button
        m_CancelButton->Set(glm::vec2(xPos, yPos), glm::vec2(buttonWidth, buttonHeight));
    }
}

void ITimeSelector::EnterEditMode()
{
    if (m_InEditMode) return; // Already in edit mode

    // Save current time for editing
    m_EditTime = m_CurrentTime;

    // Update combo box selections
    m_HoursCombo->SetSelectedIndex(m_EditTime.hours);
    m_MinutesCombo->SetSelectedIndex(m_EditTime.minutes);
    m_SecondsCombo->SetSelectedIndex(m_EditTime.seconds);

    // Switch UI components
    RemoveChild(m_EditButton);

    // Add edit mode components
    AddChild(m_HoursCombo);
    AddChild(m_MinutesCombo);
    if (m_ShowSeconds) {
        AddChild(m_SecondsCombo);
    }
    AddChild(m_ApplyButton);
    AddChild(m_CancelButton);

    // Update state
    m_InEditMode = true;

    // Update layout
    LayoutComponents();
}

void ITimeSelector::ExitEditMode(bool applyChanges)
{
    if (!m_InEditMode) return; // Not in edit mode

    if (applyChanges) {
        // Apply edited time
        m_CurrentTime = m_EditTime;
        m_CurrentTime.Validate();
        NotifyTimeChanged();
    }

    // Switch UI components
    RemoveChild(m_HoursCombo);
    RemoveChild(m_MinutesCombo);
    if (m_ShowSeconds) {
        RemoveChild(m_SecondsCombo);
    }
    RemoveChild(m_ApplyButton);
    RemoveChild(m_CancelButton);

    // Add display mode components
    AddChild(m_EditButton);

    // Update state
    m_InEditMode = false;

    // Update layout
    LayoutComponents();
}

void ITimeSelector::PopulateHourDropdown()
{
    // Add hour options (0-23)
    for (int i = 0; i < 24; i++) {
        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0') << i;
        m_HoursCombo->AddItem(oss.str());
    }
}

void ITimeSelector::PopulateMinuteDropdown()
{
    // Add minute options (0-59)
    for (int i = 0; i < 60; i++) {
        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0') << i;
        m_MinutesCombo->AddItem(oss.str());
    }
}

void ITimeSelector::PopulateSecondDropdown()
{
    // Add second options (0-59)
    for (int i = 0; i < 60; i++) {
        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0') << i;
        m_SecondsCombo->AddItem(oss.str());
    }
}