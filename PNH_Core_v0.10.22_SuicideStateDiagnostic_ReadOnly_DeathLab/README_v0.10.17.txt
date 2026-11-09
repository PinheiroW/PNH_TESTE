PNH_Core v0.10.17 - Fatal Damage Observer (READ ONLY)

Based on v0.10.16. Changes:
- New PNH_FatalDamageObserver.c and Config.c; configuration FatalDamageObserver.json created OFF.
- Existing PNH_LifeCyclePlayerHook.c gains EEOnDamageCalculated override, calls super exactly once, and returns the exact super result.
- If enabled, observer logs head/brain hits or damage-calculation events with Health damage >=30, capped at 40 event lines per process, with no SteamID and no player mutations.
- On startup, module bootstraps OFF. Other DEO/DTC configs are not modified.
- Damage amounts and global health are NOT definitive proof of fatality (damage zones, armor and additional engine logic matter).
- NO conversion to infected, NO journal publication, NO commit, NO cancellation of hits. It does not cover all mechanisms of death (such as bleed ticking).

TESTING: First compile and start DeathLab without connecting or killing; verify Bootstrap v0.10.17, FDO config created (OFF), and no script errors. Do NOT enable FDO until bootstrap is verified.

LIMITATIONS: Source ZIP built from verified v0.10.16 archive. No local DayZ Enforce compile/runtime testing has been performed. Verify performance and behavior in isolated lab only. Inspect interaction with CF/VPP or other mods that override EEOnDamageCalculated before production.
