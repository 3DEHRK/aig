-- ~/.hammerspoon/init.lua
-- Loop: build (headless) -> wait -> click chat -> type -> wait -> click second -> wait -> repeat
-- Hotkeys:
--   ⌥⌘P  : set chat input position
--   ⌥⌘O  : set second click position
--   ⌥⌘L  : start loop
--   ⌥⌘K  : stop loop
--   ⌥⌘T  : test one iteration (no loop)

----------------------------------------------------------------------
-- USER SETTINGS
----------------------------------------------------------------------

local HOME = os.getenv("HOME")

-- <<< YOUR PROJECT PATHS >>>
local PROJECT_DIR = HOME .. "/Documents/aig/sfml-game-framework"  -- <- set yours
local BUILD_DIR   = PROJECT_DIR .. "/build"

-- timings
local COMMAND_DELAY_SEC = 90        -- wait 90s after starting build
local AFTER_MESSAGE_DELAY_SEC = 150 -- wait 2:30 after typing "keep working"
local RESTART_DELAY_SEC = 1

-- chat text
local PROMPT_TEXT = "keep working"
local ENTER_AFTER_TYPING = true

-- micro timings
local CLICK_SLEEP = 0.15
local TYPE_SLEEP = 0.05

----------------------------------------------------------------------
-- BUILD COMMAND (headless; full log to build.log)
----------------------------------------------------------------------

local function shq(s) return string.format("%q", s) end

-- We exec to redirect ALL subsequent stdout/stderr into build.log, ensuring the full log is captured.
local bash = table.concat({
  "set -o pipefail",
  "cd " .. shq(PROJECT_DIR),
  "rm -rf build",
  "mkdir -p build",
  "cd build",
  -- from here, all output of both cmake steps goes into build.log (stdout+stderr)
  "exec > build.log 2>&1",
  "cmake ..",
  "cmake --build ."
}, " && ")

----------------------------------------------------------------------
-- PERSISTED POSITIONS
----------------------------------------------------------------------

local function savePoint(key, pt) hs.settings.set(key, {x=pt.x, y=pt.y}) end
local function loadPoint(key)
  local t = hs.settings.get(key)
  if t and t.x and t.y then return hs.geometry.point(t.x, t.y) end
  return nil
end

local POS_KEYS = { input = "copilot_input_point", second = "second_click_point" }

local positions = {
  input  = loadPoint(POS_KEYS.input),
  second = loadPoint(POS_KEYS.second),
}

----------------------------------------------------------------------
-- UTILITIES
----------------------------------------------------------------------

local function notify(txt) hs.alert.show(txt, 1.2) end
local function usleepSeconds(sec) hs.timer.usleep(math.floor(sec * 1e6)) end

local function moveAndClick(pt)
  hs.mouse.setAbsolutePosition(pt)
  usleepSeconds(CLICK_SLEEP)
  hs.eventtap.leftClick(pt, 0)
  usleepSeconds(CLICK_SLEEP)
end

local function typeText(txt)
  usleepSeconds(TYPE_SLEEP)
  hs.eventtap.keyStrokes(txt)
  if ENTER_AFTER_TYPING then hs.eventtap.keyStroke({}, "return") end
end

local function ensurePositions()
  if not positions.input then notify("Set chat input: ⌥⌘P"); return false end
  if not positions.second then notify("Set second spot: ⌥⌘O"); return false end
  return true
end

----------------------------------------------------------------------
-- HEADLESS SHELL TASK (no Terminal/iTerm)
----------------------------------------------------------------------

local currentBuildTask = nil

local function runHeadlessBuild(command)
  -- Kill any previous build task
  if currentBuildTask then
    currentBuildTask:terminate()
    currentBuildTask = nil
  end

  -- Run under /bin/bash -lc "<command>"
  local onExit = function(_, _, _)
    -- nothing to do; we use a fixed wait below via timers
  end

  currentBuildTask = hs.task.new("/bin/bash", onExit, {"-lc", command})
  currentBuildTask:start()
end

----------------------------------------------------------------------
-- LOOP CONTROL
----------------------------------------------------------------------

local loopRunning, currentTimer = false, nil

local function cancelTimer()
  if currentTimer then currentTimer:stop(); currentTimer = nil end
end

local function stopLoop()
  loopRunning = false
  cancelTimer()
  if currentBuildTask then currentBuildTask:terminate(); currentBuildTask = nil end
  notify("Loop: STOPPED")
end

local function schedule(delay, fn)
  cancelTimer()
  currentTimer = hs.timer.doAfter(delay, function()
    currentTimer = nil
    fn()
  end)
end

local function doOneIteration(nextAction)
  -- 1) run headless build (full log captured in build.log)
  runHeadlessBuild(bash)

  -- 2) wait COMMAND_DELAY_SEC, then click input and type
  schedule(COMMAND_DELAY_SEC, function()
    moveAndClick(positions.input)
    typeText(PROMPT_TEXT)

    -- 3) wait AFTER_MESSAGE_DELAY_SEC, then click second point
    schedule(AFTER_MESSAGE_DELAY_SEC, function()
      moveAndClick(positions.second)

      -- 4) small delay then loop
      schedule(RESTART_DELAY_SEC, function()
        if nextAction then nextAction() end
      end)
    end)
  end)
end

local function startLoop()
  if not ensurePositions() then return end
  if loopRunning then notify("Loop already running"); return end
  loopRunning = true
  notify("Loop: STARTED")
  local function iterate()
    if not loopRunning then return end
    doOneIteration(iterate)
  end
  iterate()
end

local function testOnce()
  if not ensurePositions() then return end
  notify("Test: single iteration")
  doOneIteration(function() notify("Test: done") end)
end

----------------------------------------------------------------------
-- HOTKEYS
----------------------------------------------------------------------

-- Set chat input position
hs.hotkey.bind({"alt","cmd"}, "P", function()
  positions.input = hs.mouse.getAbsolutePosition()
  savePoint(POS_KEYS.input, positions.input)
  notify(("Set chat input: %.0f,%.0f"):format(positions.input.x, positions.input.y))
end)

-- Set second click position
hs.hotkey.bind({"alt","cmd"}, "O", function()
  positions.second = hs.mouse.getAbsolutePosition()
  savePoint(POS_KEYS.second, positions.second)
  notify(("Set second spot: %.0f,%.0f"):format(positions.second.x, positions.second.y))
end)

-- Start/stop/test
hs.hotkey.bind({"alt","cmd"}, "L", startLoop)
hs.hotkey.bind({"alt","cmd"}, "K", stopLoop)
hs.hotkey.bind({"alt","cmd"}, "T", testOnce)

----------------------------------------------------------------------
-- SAFETY ON RELOAD/EXIT
----------------------------------------------------------------------

hs.shutdownCallback = function() stopLoop() end
