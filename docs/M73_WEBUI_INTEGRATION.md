# M73 Web UI Integration Guide

## Overview

This guide explains how to consume M73 data in the ESP3D Web UI and render a progress bar with dynamic ETA.

## Data Sources

### 1. WebSocket Real-Time Notifications

Event payload example:

```javascript
{
  type: "M73",
  progress: 45,
  max: 100,
  elapsed: 3600,
  remaining: 2400,
  percent: 45,
  eta_total: 6000,
  accuracy: 85,
  avg_speed: 0.0125,
  stream_elapsed: 120,
  stream_bytes: 2500000,
  stream_total: 5000000,
  timestamp: 1234567890
}
```

### 2. ESP701 Polling Fallback

Request:

```text
[ESP701]?json
```

Response (when M73 is available):

```javascript
{
  status: "processing",
  total: 5000000,
  processed: 2500000,
  type: 2,
  name: "model.gco",
  m73_progress: 45,
  m73_max: 100,
  m73_elapsed: 3600,
  m73_remaining: 2400
}
```

## Minimal Client Example

```javascript
const ws = new WebSocket("ws://esp3d.local/webui-v3")

ws.addEventListener("message", (event) => {
  const data = JSON.parse(event.data)
  if (data.type === "M73") {
    updateProgressUI(data)
  }
})

function updateProgressUI(m73) {
  const bar = document.getElementById("print-progress")
  bar.style.width = `${m73.percent}%`

  document.getElementById("progress-text").innerText =
    `${m73.percent}% (${formatTime(m73.elapsed)} / ${formatTime(m73.eta_total)})`

  const left = Math.max(0, m73.eta_total - m73.elapsed)
  document.getElementById("eta-remaining").innerText =
    `ETA: ${formatTime(left)} (${m73.accuracy}% confidence)`
}

function formatTime(seconds) {
  const h = Math.floor(seconds / 3600)
  const m = Math.floor((seconds % 3600) / 60)
  const s = seconds % 60
  if (h > 0) return `${h}h ${m}m`
  return `${m}m ${s}s`
}
```

## Polling Fallback Example

```javascript
setInterval(async () => {
  const res = await fetch("[ESP701]?json")
  const data = await res.json()

  if (data.status !== "processing") return

  if (data.m73_progress !== undefined) {
    const percent = Math.round((data.m73_progress / data.m73_max) * 100)
    updateProgressUI({
      percent,
      elapsed: data.m73_elapsed,
      eta_total: data.m73_elapsed + data.m73_remaining,
      accuracy: 60,
    })
    return
  }

  if (data.total > 0) {
    const percent = Math.round((data.processed / data.total) * 100)
    updateProgressUI({
      percent,
      elapsed: 0,
      eta_total: 0,
      accuracy: 30,
    })
  }
}, 5000)
```

## Recommended UI Behavior

- Prefer M73 values over byte-based estimation
- Show confidence when available
- Keep byte-based fallback for non-M73 printers
- Handle disconnect/reconnect gracefully
- Avoid over-animating frequent updates

## Troubleshooting

- No M73 updates:
  - verify firmware emits M73 lines
  - verify ESP701 returns m73 fields
  - verify WebSocket connection is active

- Incorrect ETA:
  - check whether `m73_remaining` is present
  - fall back to byte ratio only when needed

- Missing status in UI:
  - ensure parser handles both raw stream and JSON response paths

## Integration Points

- WebSocket message handling in UI transport layer
- Target status parser for `cmd=701`
- Status panel display logic (progress, elapsed, remaining, ETA)

This keeps UI behavior consistent across firmware variants.
