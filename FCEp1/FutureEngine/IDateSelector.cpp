#include "IDateSelector.h"
#include "UIHelp.h"
#include "Texture2D.h"
#include <ctime>
#include <iomanip>
#include <sstream>

void IDateSelector::AddScrollersToComboBoxes()
{
    // Configure the day combo box
    m_DayComboBox->SetMaxDropdownHeight(MAX_DROPDOWN_HEIGHT);
    m_DayComboBox->SetUseScroller(true);

    // Configure the month combo box
    m_MonthComboBox->SetMaxDropdownHeight(MAX_DROPDOWN_HEIGHT);
    m_MonthComboBox->SetUseScroller(true);

    // Configure the year combo box (definitely needs scrolling)
    m_YearComboBox->SetMaxDropdownHeight(MAX_DROPDOWN_HEIGHT);
    m_YearComboBox->SetUseScroller(true);
}

// Date struct implementation
Date::Date() {
    // Get current date
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);

    day = now->tm_mday;
    month = now->tm_mon + 1;  // tm_mon is 0-based
    year = now->tm_year + 1900;  // tm_year is years since 1900
}

std::string Date::ToString() const {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << day << "/"
        << std::setfill('0') << std::setw(2) << month << "/"
        << year;
    return oss.str();
}

bool Date::IsValid() const {
    // Basic validation
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > GetDaysInMonth()) return false;
    if (year < 1900 || year > 2100) return false;  // Reasonable range

    return true;
}

int Date::GetDaysInMonth() const {
    static const int daysInMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    if (month == 2 && IsLeapYear())
        return 29;

    return daysInMonth[month];
}

bool Date::IsLeapYear() const {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// IDateSelector implementation
IDateSelector::IDateSelector()
    : IControl(),
    m_CurrentDate(),
    m_DayComboBox(nullptr),
    m_MonthComboBox(nullptr),
    m_YearComboBox(nullptr),
    m_LabelColor(glm::vec4(0.9f, 0.9f, 0.9f, 1.0f)),
    m_BackgroundColor(glm::vec4(0.2f, 0.2f, 0.2f, 0.8f)),
    m_CalendarIcon(nullptr)
{
    InitializeComponents();
}

IDateSelector::IDateSelector(glm::vec2 position, glm::vec2 size)
    : IControl(position, size),
    m_CurrentDate(),
    m_DayComboBox(nullptr),
    m_MonthComboBox(nullptr),
    m_YearComboBox(nullptr),
    m_LabelColor(glm::vec4(0.9f, 0.9f, 0.9f, 1.0f)),
    m_BackgroundColor(glm::vec4(0.2f, 0.2f, 0.2f, 0.8f)),
    m_CalendarIcon(nullptr)
{
    InitializeComponents();
}

IDateSelector::~IDateSelector()
{
    // Note: Child controls will be deleted by IControl's destructor
}

void IDateSelector::SetDate(const Date& date)
{
    if (date.IsValid())
    {
        m_CurrentDate = date;

        // Update combo boxes
        m_YearComboBox->SetSelectedIndex(date.year - 1900);
        m_MonthComboBox->SetSelectedIndex(date.month - 1);

        // Update days in month based on new month/year
        UpdateDayComboBox();

        // Set day after updating day combo box
        if (date.day <= m_CurrentDate.GetDaysInMonth())
        {
            m_DayComboBox->SetSelectedIndex(date.day - 1);
        }

        NotifyDateChanged();
    }
}

void IDateSelector::SetDate(int day, int month, int year)
{
    SetDate(Date(day, month, year));
}

Date IDateSelector::GetDate() const
{
    return m_CurrentDate;
}

void IDateSelector::ResetToCurrentDate()
{
    SetDate(Date());
}

void IDateSelector::Update(float delta)
{
    // Update child controls
    UpdateChildren(delta);
}

void IDateSelector::Render()
{
    glm::vec2 renderPos = GetRenderPosition();

    // Draw a subtle background
    UIHelp::DrawRect(renderPos, m_Size, m_BackgroundColor);
    UIHelp::DrawOutlineRect(renderPos, m_Size, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

    // Draw calendar icon if available
    if (m_CalendarIcon)
    {
        float iconSize = m_Size.y * 0.7f;
        glm::vec2 iconPos = renderPos + glm::vec2(5.0f, (m_Size.y - iconSize) / 2.0f);
        UIHelp::DrawImage(iconPos, glm::vec2(iconSize, iconSize), m_CalendarIcon, glm::vec4(1.0f));
    }

    // Draw date label
    float labelX = renderPos.x + (m_CalendarIcon ? m_Size.y : 10.0f);
    std::string dateLabel = "Date: ";
   // UIHelp::DrawText(glm::vec2(labelX, renderPos.y + (m_Size.y - UIHelp::StrHeight(dateLabel)) / 2.0f),
    //    dateLabel, m_LabelColor);

    // Render child controls (combo boxes)
    RenderChildren();
}

void IDateSelector::PreRender()
{
    IControl::PreRender();
}

void IDateSelector::OnMouseDown(int button)
{
    // Handle in child controls
}

void IDateSelector::OnMouseUp(int button)
{
    // Handle in child controls
}

void IDateSelector::OnMouseMove(glm::vec2 position, glm::vec2 delta)
{
    // Handle in child controls
}

void IDateSelector::InitializeComponents()
{
    // Create day combo box with fixed dropdown height
    m_DayComboBox = new IComboBox(glm::vec2(0, 0), glm::vec2(60, 30));

    // Create month combo box with fixed dropdown height
    m_MonthComboBox = new IComboBox(glm::vec2(0, 0), glm::vec2(120, 30));
    const char* monthNames[] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };
    for (int i = 0; i < 12; ++i)
    {
        m_MonthComboBox->AddItem(monthNames[i]);
    }

    // Create year combo box with fixed dropdown height
    m_YearComboBox = new IComboBox(glm::vec2(0, 0), glm::vec2(80, 30));

    // Add years (from 1900 to current year + 100)
    int currentYear = m_CurrentDate.year;
    for (int year = 1900; year <= currentYear + 100; ++year)
    {
        m_YearComboBox->AddItem(std::to_string(year));
    }

    // Set up callbacks
    m_DayComboBox->SetOnSelectionChanged([this](int index, std::string value) {
        m_CurrentDate.day = index + 1;
        NotifyDateChanged();
        });

    m_MonthComboBox->SetOnSelectionChanged([this](int index, std::string value) {
        m_CurrentDate.month = index + 1;
        UpdateDayComboBox();
        NotifyDateChanged();
        });

    m_YearComboBox->SetOnSelectionChanged([this](int index, std::string value) {
        m_CurrentDate.year = 1900 + index;
        UpdateDayComboBox();
        NotifyDateChanged();
        });

    // Initial population of days
    UpdateDayComboBox();

    // Set initial selections
    m_DayComboBox->SetSelectedIndex(m_CurrentDate.day - 1);
    m_MonthComboBox->SetSelectedIndex(m_CurrentDate.month - 1);
    m_YearComboBox->SetSelectedIndex(m_CurrentDate.year - 1900);

    // Add combo boxes as children
    AddChild(m_DayComboBox);
    AddChild(m_MonthComboBox);
    AddChild(m_YearComboBox);

    // Set initial layout
    LayoutComponents();

    // Attach IVerticalScrollers to each combo box's internal dropdown
    AddScrollersToComboBoxes();
}

void IDateSelector::UpdateDayComboBox()
{
    // Store current selection
    int currentDay = m_DayComboBox->GetSelectedIndex() + 1;

    // Clear existing items
    while (m_DayComboBox->GetItemCount() > 0)
    {
        m_DayComboBox->RemoveItem(0);
    }

    // Add days based on current month and year
    int daysInMonth = m_CurrentDate.GetDaysInMonth();
    for (int day = 1; day <= daysInMonth; ++day)
    {
        // Add leading zero for single-digit days
        std::string dayStr = (day < 10) ? "0" + std::to_string(day) : std::to_string(day);
        m_DayComboBox->AddItem(dayStr);
    }

    // Restore selection if valid, otherwise set to 1
    if (currentDay <= daysInMonth)
    {
        m_DayComboBox->SetSelectedIndex(currentDay - 1);
        m_CurrentDate.day = currentDay;
    }
    else
    {
        m_DayComboBox->SetSelectedIndex(0);
        m_CurrentDate.day = 1;
    }
}

void IDateSelector::NotifyDateChanged()
{
    if (m_OnDateChanged)
    {
        m_OnDateChanged(m_CurrentDate);
    }
}

void IDateSelector::LayoutComponents()
{
    // Calculate positions based on control size and icon
    float startX = (m_CalendarIcon ? m_Size.y + 10.0f : 10.0f) + 50.0f; // After "Date:" label

    // Position day combo box
    m_DayComboBox->Set(glm::vec2(startX, 5.0f), glm::vec2(60, m_Size.y - 10.0f));

    // Position month combo box
    m_MonthComboBox->Set(glm::vec2(startX + 70.0f, 5.0f), glm::vec2(120, m_Size.y - 10.0f));

    // Position year combo box
    m_YearComboBox->Set(glm::vec2(startX + 200.0f, 5.0f), glm::vec2(80, m_Size.y - 10.0f));
}

// Override AfterSet to update component layout when the control is resized
void IDateSelector::AfterSet()
{
    LayoutComponents();
}
