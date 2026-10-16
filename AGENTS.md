# Agent instructions

This repository holds SushiLog, the one logger of the Sushi libraries. It follows the Sushi
Systems skills: load `sushiskills` before working here; it names the skill each task needs. The
contract of the library is `skills/logging/INTERFACE.md` and `skills/logging/SKILL.md` in
SushiSkills.

Build and test through `sl` only (`sl build`, `sl test`, `sl check`); never call CMake, Ninja or
CTest directly.

Check before reporting work as finished:

```
python tools/check_all.py .
```
