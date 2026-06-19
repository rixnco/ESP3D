/*
  m73_history.h - M73 Progress tracking and history management

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

#ifndef _M73_HISTORY_H
#define _M73_HISTORY_H

#include <Arduino.h>
#include <vector>

// M73 Progress entry for history
typedef struct {
  uint32_t timestamp;      // Timestamp in milliseconds
  uint8_t progress;        // Progress percentage (0-100)
  uint32_t elapsed;        // Elapsed time in seconds
  uint32_t remaining;      // Remaining time in seconds
  uint8_t accuracy;        // Prediction accuracy (0-100)
} M73HistoryEntry;

// M73 History manager for tracking printing progress
class M73History {
 public:
  M73History();
  ~M73History();
  
  // Add a progress entry to history
  void addEntry(uint8_t progress, uint32_t elapsed, uint32_t remaining);
  
  // Get average speed (% per second)
  float getAverageSpeed() const;
  
  // Get estimated total time based on history
  uint32_t getEstimatedTotalTime() const;
  
  // Get prediction accuracy (0-100)
  uint8_t getPredictionAccuracy() const;
  
  // Get last entry
  const M73HistoryEntry* getLastEntry() const;
  
  // Get entry at index
  const M73HistoryEntry* getEntryAt(size_t index) const;
  
  // Get number of entries
  size_t getEntryCount() const { return _history.size(); }
  
  // Clear history
  void clear();
  
  // Get history as JSON array
  String toJSON() const;
  
  // Get statistics
  struct Stats {
    uint8_t min_progress = 0;
    uint8_t max_progress = 0;
    uint8_t avg_progress = 0;
    uint32_t total_elapsed = 0;
    uint32_t total_remaining = 0;
    float avg_speed = 0.0f;
  };
  
  Stats getStatistics() const;

 private:
  std::vector<M73HistoryEntry> _history;
  static const size_t MAX_HISTORY_SIZE = 1000;  // Limit memory usage
  
  uint32_t _last_entry_time = 0;
  float _cached_avg_speed = 0.0f;
};

// Global M73 history instance
extern M73History m73_history;

#endif  // _M73_HISTORY_H
