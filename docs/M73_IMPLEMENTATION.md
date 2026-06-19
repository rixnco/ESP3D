# M73 GCODE Implementation for ESP3D

## Overview

This implementation adds local and intelligent `M73` handling to ESP3D SD streaming, with real-time WebSocket notifications for the Web UI.

## Key Features

- Full `M73` parsing (`P`, `Q`, `S`, `R`)
- Local M73 detection without altering outgoing GCODE
- Real-time WebSocket notifications (500 ms throttle)
- Progress history with lightweight analytics
- ETA estimation based on reported values and observed trend
- Native integration into `ESP701` stream status responses
- Backward compatibility with byte-based progress tracking

## Architecture

### Data Flow

```text
SD file stream
  -> GcodeHost reads line
  -> processCommand()
      -> parseM73() for non-ESP commands
      -> forward command to printer unchanged
  -> handle()
      -> broadcastM73() when relevant
  -> WebSocket notification to UI
```

### Components

#### `gcode_host.h`

Adds M73 tracking state:

- `_m73_progress`
- `_m73_max`
- `_m73_elapsed_time`
- `_m73_remaining_time`
- `_m73_has_data`
- `_m73_start_time`
- `_m73_last_broadcast`
- `_m73_last_progress`

Public accessors are exposed for `ESP701`.

#### `gcode_host.cpp`

Adds:

- `parseM73(const char* command)`
- `broadcastM73()`

Behavior:

- Parse `M73 P<progress> Q<max> [S<remaining>] [R<elapsed>]`
- Store latest values
- Add entries to history
- Broadcast updates only when needed (throttle + change detection)

#### `m73_history.h/.cpp`

Provides history and analytics support:

- Time-stamped entries
- Average speed
- Estimated total time
- Basic confidence metric
- Compact JSON export for diagnostics

Memory is capped with FIFO behavior.

#### `ESP701.cpp`

Extends stream status JSON when M73 data is available:

```json
{
  "status": "processing",
  "total": 5000000,
  "processed": 2500000,
  "type": 2,
  "name": "model.gco",
  "m73_progress": 45,
  "m73_max": 100,
  "m73_elapsed": 3600,
  "m73_remaining": 2400
}
```

## WebSocket Payload

Typical real-time payload:

```json
{
  "type": "M73",
  "progress": 45,
  "max": 100,
  "elapsed": 3600,
  "remaining": 2400,
  "percent": 45,
  "eta_total": 6000,
  "accuracy": 85,
  "avg_speed": 0.0125,
  "stream_elapsed": 120,
  "stream_bytes": 2500000,
  "stream_total": 5000000,
  "timestamp": 1234567890
}
```

## Supported M73 Examples

```gcode
M73 P0 Q100
M73 P25 Q100
M73 P50 Q100 S1800
M73 P75 Q100 R1800 S600
M73 P100 Q100 R3600
```

Notes:

- Case-insensitive detection
- Optional parameters are supported
- Progress is clamped to safe ranges

## Performance Notes

- CPU overhead is low (simple parsing and conditional dispatch)
- Notification rate is limited (500 ms)
- History storage is bounded
- Existing behavior remains available as fallback

## Validation Checklist

- Build succeeds with `GCODE_HOST_FEATURE`
- M73 is detected during SD streaming
- `ESP701` status includes M73 fields when available
- WebSocket clients receive M73 updates
- UI still works when M73 is not present

## File List

Modified:

- `esp3d/src/modules/gcode_host/gcode_host.h`
- `esp3d/src/modules/gcode_host/gcode_host.cpp`
- `esp3d/src/core/commands/ESP701.cpp`

Added:

- `esp3d/src/modules/gcode_host/m73_history.h`
- `esp3d/src/modules/gcode_host/m73_history.cpp`

Related docs:

- `docs/M73_WEBUI_INTEGRATION.md`
- `docs/M73_README.md`
- `docs/M73_FILE_SUMMARY.md`
