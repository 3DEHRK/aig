# Behavior Rules for me as an AI Model regarding this prompt

- "keep working" means: read `build/build.log` if there are errors prioritize fixing them, if there is a clean build continue on your own with the implementation of the next feature according to the tasks in `docs/features/*.md` (notably `docs/features/todo.md`)

- Do NOT under any circumstances request command EXECUTION, do not try to execute something like cmake --build !!!! 
The build will be done manually by the user after every request

- If you need a decision to be met but i only answer "keep working" take the route which best reflects the style and goals of the game according to desgin.md and readme.md

- once you finished a feature from the roadmap, check it off and protocol the necessary test into the testme.md, additionally make the game code easily testable (eg new item? -> add it to inventory)