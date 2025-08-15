-- === User settings ===
local SHELL_CMD = [[
cd ~/Documents/aig/sfml-game-framework &&
rm -rf build &&
mkdir -p build &&
cd build &&
{
  echo "===== $(date '+%Y-%m-%d %H:%M:%S') ====="
  echo "PWD: $(pwd)"
  cmake ..
  cmake --build .
} > build.log 2>&1
]]
local PROMPT_TEXT       = [[keep working]]
local VS_CODE_NAME      = "Visual Studio Code"  -- or "VSCodium"
local ENTER_KEY_TO_SEND = true                  -- Copilot sends on Enter

-- Timings (seconds)
local DELAY_BEFORE_CMD   = 1
local WAIT_BEFORE_PROMPT = 80
local WAIT_BEFORE_KEEP   = 130
local LOOP_BUFFER        = 5
local LOOP_SECONDS       = DELAY_BEFORE_CMD + WAIT_BEFORE_PROMPT + WAIT_BEFORE_KEEP + LOOP_BUFFER
-- ======================

local loopTimer = nil
local liveTimers = {}   -- just hold refs; no deletion on fire

-- ===== Utilities =====
local function note(title, txt)
  hs.notify.new({title=title, informativeText=txt}):send()
end

local function say(msg)
  hs.task.new("/usr/bin/say", nil, {msg}):start()
  hs.printf("[HS] %s", msg)
end

local function flashAtPoint(pt)
  local r = hs.drawing.circle(hs.geometry.rect(pt.x-10, pt.y-10, 20, 20))
  r:setStroke(true):setFill(false):setStrokeWidth(4):setStrokeColor({red=1,green=0,blue=0,alpha=0.9})
  r:show()
  hs.timer.doAfter(0.18, function() r:delete() end)
end

local function typeText(text)
  local old = hs.pasteboard.getContents()
  hs.pasteboard.setContents(text)
  hs.eventtap.keyStroke({"cmd"}, "v", 0)
  hs.timer.usleep(120000)
  hs.pasteboard.setContents(old or "")
end

local function runShellCommand()
  say("Running build command")
  hs.task.new("/bin/zsh", function() end, {"-lc", SHELL_CMD}):start()
end

local function focusVSCode()
  hs.application.launchOrFocus(VS_CODE_NAME)
  hs.timer.usleep(300000)
end

local function clickAtPoint(pt)
  local restore = hs.mouse.absolutePosition()
  hs.mouse.absolutePosition(pt)
  flashAtPoint(pt)
  hs.eventtap.leftClick(pt, 0)
  hs.timer.usleep(150000)
  hs.mouse.absolutePosition(restore)
end

local function clickCopilotInput()
  local pt = hs.settings.get("copilotInputPoint")
  if not pt then
    say("Copilot input point not set")
    note("Copilot input not set","Press ⌃⌥⌘P while cursor is on the Copilot chat field.")
    return false
  end
  say(string.format("Clicking Copilot input at x=%.0f y=%.0f", pt.x, pt.y))
  clickAtPoint(pt)
  return true
end

local function clickKeepButton()
  local pt = hs.settings.get("keepButtonPoint")
  if not pt then
    say("Keep button point not set")
    note("Keep button not set","Press ⌃⌥⌘K while cursor is on the Keep button.")
    return false
  end
  say(string.format("Clicking Keep button at x=%.0f y=%.0f", pt.x, pt.y))
  clickAtPoint(pt)
  return true
end

local function sendPrompt()
  say("Sending prompt to Copilot")
  typeText(PROMPT_TEXT)
  if ENTER_KEY_TO_SEND then
    hs.eventtap.keyStroke({}, "return", 0)
  end
end

-- Robust one-shot timer that keeps a strong ref
local function doAfterKeepRef(seconds, fn)
  local t = hs.timer.doAfter(seconds, function()
    local ok, err = pcall(fn)
    if not ok then
      say("Timer error: "..tostring(err))
      hs.printf("Timer error: %s", tostring(err))
    end
  end)
  table.insert(liveTimers, t)
  return t
end

-- ===== Orchestrator =====
-- One cycle: +1s build → +90s prompt → +220s keep  (total ~311s). Loop every 316s.
local function runCycle()
  doAfterKeepRef(DELAY_BEFORE_CMD, function()
    runShellCommand()

    doAfterKeepRef(WAIT_BEFORE_PROMPT, function()
      focusVSCode()
      if clickCopilotInput() then
        sendPrompt()
      end
    end)

    doAfterKeepRef(WAIT_BEFORE_PROMPT + WAIT_BEFORE_KEEP, function()
      focusVSCode()
      if not clickKeepButton() then
        say("Keep click failed (point missing?)")
      end
    end)
  end)
end

-- ===== Loop controller =====
local function startLoop()
  if loopTimer then
    note("Already running","Automation loop is already active.")
    say("Loop already running")
    return
  end
  say("Starting automation loop")
  runCycle()
  loopTimer = hs.timer.doEvery(LOOP_SECONDS, runCycle)
  note("Started","Repeating every "..tostring(LOOP_SECONDS).."s. Press ⌃⌥⌘E to stop.")
end

local function stopLoop()
  if loopTimer then
    loopTimer:stop()
    loopTimer = nil
    say("Stopped automation loop")
    note("Stopped","Automation loop stopped.")
  else
    note("Not running","No loop to stop.")
  end
end

-- ===== Hotkeys =====
hs.hotkey.bind({"ctrl","alt","cmd"}, "P", function()
  local pt = hs.mouse.absolutePosition()
  hs.settings.set("copilotInputPoint", pt)
  say("Saved Copilot input position")
  note("Saved Copilot input", string.format("x=%.0f, y=%.0f", pt.x, pt.y))
end)

hs.hotkey.bind({"ctrl","alt","cmd"}, "K", function()
  local pt = hs.mouse.absolutePosition()
  hs.settings.set("keepButtonPoint", pt)
  say("Saved Keep button position")
  note("Saved Keep button", string.format("x=%.0f, y=%.0f", pt.x, pt.y))
end)

hs.hotkey.bind({"ctrl","alt","cmd"}, "D", function()
  local p1 = hs.settings.get("copilotInputPoint")
  local p2 = hs.settings.get("keepButtonPoint")
  say("Dumping saved points")
  note("Saved points",
       string.format("Copilot: %s\nKeep: %s",
         p1 and string.format("x=%.0f y=%.0f", p1.x, p1.y) or "not set",
         p2 and string.format("x=%.0f y=%.0f", p2.x, p2.y) or "not set"))
end)

hs.hotkey.bind({"ctrl","alt","cmd"}, "T", function()
  say("Running test cycle")
  runCycle()
  note("Test cycle","Ran one full cycle.")
end)

hs.hotkey.bind({"ctrl","alt","cmd"}, "S", startLoop)
hs.hotkey.bind({"ctrl","alt","cmd"}, "E", stopLoop)

-- Click Keep now (isolation test)
hs.hotkey.bind({"ctrl","alt","cmd"}, "G", function()
  focusVSCode()
  if not clickKeepButton() then
    note("Keep test","Keep button not set or not visible.")
  end
end)

note("Hammerspoon ready",
     "Hotkeys: P save Copilot, K save Keep, D dump, T test cycle, S start, E stop, G Keep now.")
say("Hammerspoon ready")

