# M73 GCODE Implementation - File Summary

## Overview

This feature adds local M73 progress tracking for SD streaming in ESP3D, with optional history analytics and WebSocket notifications, and extends ESP701 status output.

## File Inventory

### Modified Files

- `esp3d/src/modules/gcode_host/gcode_host.h`
  - M73 state members
  - M73 accessors
  - parse/broadcast declarations

- `esp3d/src/modules/gcode_host/gcode_host.cpp`
  - M73 parsing in stream command flow
  - state initialization/reset
  - conditional notification broadcast
  - history integration

- `esp3d/src/core/commands/ESP701.cpp`
  - optional M73 fields in JSON stream status response

### Added Files

- `esp3d/src/modules/gcode_host/m73_history.h`
  - history data structures
  - analytics API

- `esp3d/src/modules/gcode_host/m73_history.cpp`
  - bounded history implementation
  - estimated timing helpers
  - JSON export helpers

## Behavior Summary

- Detect `M73` in streamed GCODE lines
- Keep command forwarding unchanged
- Store latest M73 values locally
- Optionally append history entries
- Broadcast structured updates with throttling
- Report M73 values through `ESP701` status endpoint

## Data Contract Highlights

ESP701 response may include:

- `m73_progress`
- `m73_max`
- `m73_elapsed`
- `m73_remaining`

WebSocket payload may include:

- current progress and max
- elapsed and remaining seconds
- derived percent, ETA, confidence, speed

## Compatibility and Risk

- Backward compatible with existing status consumers
- Existing byte-based progress remains valid
- Main risk is UI assumptions about timing units; use explicit seconds

## Verification Checklist

- Build succeeds
- M73 lines are parsed during SD stream
- ESP701 JSON includes new keys when data exists
- WebSocket messages arrive at expected rate
- No regression when M73 is absent

## Documentation Set

- `docs/M73_IMPLEMENTATION.md`
- `docs/M73_WEBUI_INTEGRATION.md`
- `docs/M73_README.md`
- `docs/M73_FILE_SUMMARY.md`
