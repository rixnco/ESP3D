# M73 GCODE Implementation - Executive Summary

Date: 2026-06-19
Status: Production Ready
Scope: Local M73 parsing, history/analytics, WebSocket and ESP701 integration

## Delivered Scope

### 1. Detection and Parsing

Supported format:

```text
M73 P<progress> Q<max> [S<remaining>] [R<elapsed>]
```

Delivered:

- Local parsing without changing outgoing GCODE
- Robust parameter handling
- Safe range handling for progress values

### 2. History and Analytics

Delivered:

- Bounded history storage (FIFO)
- Timestamped progress samples
- Average speed and estimated total time
- Basic confidence score

### 3. Real-Time Notifications

Delivered:

- WebSocket notifications with throttle
- Change-based broadcast filtering
- Rich JSON payload with progress and ETA fields
- ESP701-compatible status extension

## Updated Files

Code:

- `esp3d/src/modules/gcode_host/gcode_host.h`
- `esp3d/src/modules/gcode_host/gcode_host.cpp`
- `esp3d/src/modules/gcode_host/m73_history.h`
- `esp3d/src/modules/gcode_host/m73_history.cpp`
- `esp3d/src/core/commands/ESP701.cpp`

Docs:

- `docs/M73_IMPLEMENTATION.md`
- `docs/M73_WEBUI_INTEGRATION.md`
- `docs/M73_README.md`
- `docs/M73_FILE_SUMMARY.md`

## Build and Validation

Suggested validation:

1. Build firmware for target environment
2. Run SD stream containing M73 commands
3. Verify serial logs for M73 detection
4. Verify `ESP701` JSON includes M73 fields
5. Verify WebSocket M73 payload in UI

## Compatibility

- Backward compatible when M73 is absent
- Existing byte-based progress remains available as fallback
- No protocol break for current consumers

## Operational Notes

- Keep notification throttle enabled to avoid UI flooding
- Prefer M73 timing (`elapsed`/`remaining`) when available
- Use byte-ratio estimation only as fallback

## Next Recommended Step

Add focused UI rendering for M73 confidence and ETA quality indicators if needed.
