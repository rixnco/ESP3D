/*
  m73_history.cpp - M73 Progress tracking and history management

  Copyright (c) 2014 Luc Lebosse. All rights reserved.

  This code is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This code is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with This code; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "m73_history.h"
#include "../../include/esp3d_config.h"


M73History m73_history;

M73History::M73History() {
  _history.reserve(MAX_HISTORY_SIZE);
}

M73History::~M73History() {
  clear();
}

void M73History::addEntry(uint8_t progress, uint32_t elapsed, uint32_t remaining) {
  // Prevent duplicate entries too close together (< 1 second)
  uint32_t now = millis();
  if (_last_entry_time > 0 && (now - _last_entry_time) < 1000) {
    return;
  }
  
  _last_entry_time = now;
  
  // Calculate accuracy based on history
  uint8_t accuracy = getPredictionAccuracy();
  
  M73HistoryEntry entry;
  entry.timestamp = now;
  entry.progress = progress;
  entry.elapsed = elapsed;
  entry.remaining = remaining;
  entry.accuracy = accuracy;
  
  _history.push_back(entry);
  
  // Keep history size under control
  if (_history.size() > MAX_HISTORY_SIZE) {
    // Remove oldest entry
    _history.erase(_history.begin());
  }
  
  esp3d_log("M73 History entry added: P=%d%%, E=%d, R=%d (accuracy=%d%%)",
    progress, elapsed, remaining, accuracy);
}

float M73History::getAverageSpeed() const {
  if (_history.size() < 2) {
    return 0.0f;
  }
  
  // Calculate average progress per second
  const M73HistoryEntry& first = _history.front();
  const M73HistoryEntry& last = _history.back();
  
  uint32_t time_diff = (last.timestamp - first.timestamp) / 1000;  // seconds
  if (time_diff == 0) {
    return 0.0f;
  }
  
  int progress_diff = (int)last.progress - (int)first.progress;
  return (float)progress_diff / (float)time_diff;
}

uint32_t M73History::getEstimatedTotalTime() const {
  if (_history.empty()) {
    return 0;
  }
  
  const M73HistoryEntry& last = _history.back();
  
  // Use M73 remaining time if available
  if (last.remaining > 0) {
    return last.elapsed + last.remaining;
  }
  
  // Otherwise calculate based on speed
  float speed = getAverageSpeed();
  if (speed > 0.0f && last.progress > 0) {
    return (uint32_t)((100.0f * last.elapsed) / last.progress);
  }
  
  return 0;
}

uint8_t M73History::getPredictionAccuracy() const {
  if (_history.size() < 3) {
    return 0;  // Not enough data
  }
  
  // Simple accuracy metric: how well previous predictions matched actual times
  // For now, return a basic score based on number of observations
  uint8_t accuracy = 20 * (uint8_t)std::min(5UL, _history.size());  // 20-100%
  return accuracy;
}

const M73HistoryEntry* M73History::getLastEntry() const {
  if (_history.empty()) {
    return nullptr;
  }
  return &_history.back();
}

const M73HistoryEntry* M73History::getEntryAt(size_t index) const {
  if (index >= _history.size()) {
    return nullptr;
  }
  return &_history[index];
}

void M73History::clear() {
  _history.clear();
  _last_entry_time = 0;
  _cached_avg_speed = 0.0f;
}

M73History::Stats M73History::getStatistics() const {
  Stats stats;
  
  if (_history.empty()) {
    return stats;
  }
  
  uint32_t sum_progress = 0;
  stats.min_progress = 100;
  stats.max_progress = 0;
  
  for (const auto& entry : _history) {
    sum_progress += entry.progress;
    if (entry.progress < stats.min_progress) {
      stats.min_progress = entry.progress;
    }
    if (entry.progress > stats.max_progress) {
      stats.max_progress = entry.progress;
    }
  }
  
  stats.avg_progress = sum_progress / _history.size();
  stats.avg_speed = getAverageSpeed();
  
  const M73HistoryEntry& last = _history.back();
  stats.total_elapsed = last.elapsed;
  stats.total_remaining = last.remaining;
  
  return stats;
}

String M73History::toJSON() const {
  String json = "[";
  
  for (size_t i = 0; i < _history.size() && i < 100; i++) {  // Limit to 100 entries in JSON
    if (i > 0) json += ",";
    
    const M73HistoryEntry& entry = _history[i];
    json += "{\"t\":" + String(entry.timestamp);
    json += ",\"p\":" + String(entry.progress);
    json += ",\"e\":" + String(entry.elapsed);
    json += ",\"r\":" + String(entry.remaining);
    json += ",\"a\":" + String(entry.accuracy);
    json += "}";
  }
  
  json += "]";
  return json;
}
