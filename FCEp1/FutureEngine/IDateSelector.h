#pragma once
#include "IControl.h"
#include "IComboBox.h"
#include <functional>
#include <string>
#include <vector>
#include <ctime>

// Forward declarations
class Texture2D;

// Structure to represent a date
struct Date {
    int day;
    int month;
    int year;

    // Constructor for current date
    Date();

    // Constructor with specific date
    Date(int d, int m, int y) : day(d), month(m), year(y) {}

    // Get the date as a string (format: DD/MM/YYYY)
    std::string ToString() const;

    // Check if the date is valid
    bool IsValid() const;

    // Get the number of days in the current month/year
    int GetDaysInMonth() const;

    // Check if the year is a leap year
    bool IsLeapYear() const;
};

class IDateSelector : public IControl
{
public:
    IDateSelector();
    IDateSelector(glm::vec2 position, glm::vec2 size);
    virtual ~IDateSelector();

    // Date management
    void SetDate(const Date& date);
    void SetDate(int day, int month, int year);
    Date GetDate() const;
    void ResetToCurrentDate();

    // Event callbacks
    void SetOnDateChanged(std::function<void(const Date&)> callback) { m_OnDateChanged = callback; }

    // Appearance settings
    void SetLabelColor(const glm::vec4& color) { m_LabelColor = color; }
    void SetBackgroundColor(const glm::vec4& color) { m_BackgroundColor = color; }
    void SetCalendarIcon(Texture2D* icon) { m_CalendarIcon = icon; }

    // Overridden methods from IControl
    void Update(float delta) override;
    void Render() override;
    void PreRender() override;
    void OnMouseDown(int button) override;
    void OnMouseUp(int button) override;
    void OnMouseMove(glm::vec2 position, glm::vec2 delta) override;

private:
    Date m_CurrentDate;

    // UI components
    IComboBox* m_DayComboBox;
    IComboBox* m_MonthComboBox;
    IComboBox* m_YearComboBox;

    // Appearance settings
    glm::vec4 m_LabelColor;
    glm::vec4 m_BackgroundColor;
    Texture2D* m_CalendarIcon;

    // Event callbacks
    std::function<void(const Date&)> m_OnDateChanged;

    // Internal helper methods
    void InitializeComponents();
    void UpdateDayComboBox();
    void NotifyDateChanged();
    void LayoutComponents();
    void AddScrollersToComboBoxes();
    void AfterSet() override;
    // Maximum height for dropdown lists
    static const int MAX_DROPDOWN_HEIGHT = 200;
};