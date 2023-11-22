#pragma once

#include <cstdio> // for sprintf
#include <ctime>
#include <vector>

#include <iostream>


struct TimeSlot
{
  std::vector<int> days;
  std::string start, stop;
};

bool isDayInRange(int day, const std::vector<int> &days)
{
  for (int d : days)
  {
    ESP_LOGD("TextualTimeSlot.h", "    day %d", d);
    if (day == d)
    {
      return true;
    }
  }
  return false;
}

bool isTimeInRange(const std::string &now, const TimeSlot &timeSlot)
{
  int hourNow, minuteNow, hourStart, minuteStart, hourStop, minuteStop;
  sscanf(now.c_str(), "%d:%d", &hourNow, &minuteNow);
  sscanf(timeSlot.start.c_str(), "%d:%d", &hourStart, &minuteStart);
  sscanf(timeSlot.stop.c_str(), "%d:%d", &hourStop, &minuteStop);

  // Check if the current time is within the time slot
  if (hourNow > hourStart || (hourNow == hourStart && minuteNow >= minuteStart))
  {
    if (hourNow < hourStop || (hourNow == hourStop && minuteNow <= minuteStop))
    {
      return true;
    }
  }

  return false;
}

// Function to split a string based on a separator
std::vector<std::string> strSplit(const std::string &input, const std::string &separator)
{
  std::vector<std::string> result;
  size_t start = 0, end = 0;
  while ((end = input.find(separator, start)) != std::string::npos)
  {
    result.push_back(input.substr(start, end - start));
    start = end + separator.length();
  }
  result.push_back(input.substr(start));
  return result;
}

std::vector<TimeSlot> parseTimeSlots(const std::string &timeSlotString)
{
  std::vector<TimeSlot> result;

  std::string separator = "; ";
  std::vector<std::string> strSlots = strSplit(timeSlotString, separator);
  for (const auto &slot : strSlots)
  {
    ESP_LOGVV("TextualTimeSlot.h", "parseTimeSlots:");
    ESP_LOGVV("TextualTimeSlot.h", "- slot %s", slot.c_str());

    // Split the time slot into days, start, and stop
    std::vector<std::string> slotParts = strSplit(slot, " ");

    // Left Trim white space
    for (auto &part : slotParts)
    {
      part.erase(0, part.find_first_not_of(" "));
    }

    TimeSlot timeSlot;

    // Tokenize day
    for (char day : slotParts[0])
    {
      if (isdigit(day))
      {
        timeSlot.days.push_back(day - '0'); // [1-7] 1: Sunday ... 7: Saturday
      }
    }

    // Copy the start and stop times
    timeSlot.start = slotParts[1];
    timeSlot.stop = slotParts[2];

    // Aggiungi la fascia oraria al vettore risultante
    result.push_back(timeSlot);
  }
  return result;
}

std::string formatTime(const ESPTime &time)
{
  char buffer[6]; // Buffer to store the formatted string, e.g., "hh:mm"
  sprintf(buffer, "%02d:%02d", time.hour, time.minute);
  return std::string(buffer);
}


bool isInTimeSlots(const std::string &x)
{
  auto currentTime = id(sntp_time).now();
  int currentDay = currentTime.day_of_week; // [1-7] 1 sunday ... 7 saturday
  
  ESP_LOGD("TextualTimeSlot.h", "Check isInTimeSlots:");
  ESP_LOGD("TextualTimeSlot.h", "currentDay %d", currentDay);
  ESP_LOGD("TextualTimeSlot.h", "current formatted time %s", formatTime(currentTime).c_str());
  
  std::vector<TimeSlot> timeSlots = parseTimeSlots(x);

  for (const auto &slot : timeSlots)
  {
    ESP_LOGD("TextualTimeSlot.h", "- iterating slot:");
    ESP_LOGD("TextualTimeSlot.h", "  - isDayInRange");
    if (isDayInRange(currentDay, slot.days))
    {
      ESP_LOGD("TextualTimeSlot.h", "  - isTimeInRange");
      if (isTimeInRange(formatTime(currentTime), slot))
      {
        return true;
      }
    }
  }

  return false;
}

void onTest3(const std::string &x)
{
  std::string result = isInTimeSlots(x) ? "true" : "false";
  ESP_LOGD("TextualTimeSlot.h", "result: %s", result.c_str());
}

void onTest1()
{
  // ESP_LOGI("TextualTimeSlot.h", "Value of my text %s", id(my_text).state.c_str());

  // Check if the current day is in the allowed days
  // Assuming now is in the format "hh:mm"
  auto time = id(sntp_time).now();
  int currentDay = time.day_of_week; // [1-7] 1 sunday ... 7 saturday

  ESP_LOGI("TextualTimeSlot.h", "time %d", time);
  ESP_LOGI("TextualTimeSlot.h", "currentDay %d", currentDay);
  // isDayInRange(currentDay, timeSlot.days
}

void onTest2(std::string x)
{
  // Test the strSplit function
  std::string inputString = x;
  std::string separator = "; ";
  std::vector<std::string> strSlots = strSplit(inputString, separator);

  // Log the result
  ESP_LOGD("TextualTimeSlot.h", "Results:");
  for (const auto &word : strSlots)
  {
    ESP_LOGD("TextualTimeSlot.h", "- __%s__", word.c_str());
  }
}
