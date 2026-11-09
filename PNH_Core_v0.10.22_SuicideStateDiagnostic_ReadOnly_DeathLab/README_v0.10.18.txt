PNH_Core v0.10.18 - FDO low-damage diagnostic (READ-ONLY DEATHLAB ONLY)

Based on full v0.10.17 source. Only changes:
- PNH_FatalDamageObserverConfig.c: adds bool DiagnosticMode=false (legacy JSON may omit field; defaults false).
- PNH_FatalDamageObserver.c: optional diagnostic logging for low-damage events; includes heuristicLethal=calculated zone Health >= current global Health, NOT fatal proof.
- PNH_CoreModule.c: bootstrap version string 0.10.18.
- PlayerBase hook stays identical: super exactly once, its return passed unchanged, no health modifications.

Safeguards: server-only, player identity required, maximum 40 total event lines/process, at most 20 below-threshold diagnostic lines/process. No persist/commit/transform; configuration creation only when missing. Existing DEO/DTC remain OFF.

TEST: compile PBO in DayZ Tools; start only DeathLab port 2502 and inspect bootstrap. Stop server, edit ONLY DeathLab/profiles/PNH/Core/FatalDamageObserver.json to Enabled=true and DiagnosticMode=true; restart and verify the log. Receive ONE light infected hit on a real logged-in player, inspect [FDO] DAMAGE_DIAGNOSTIC or DAMAGE_CALCULATED. Do not induce fatal damage. Stop and set Enabled=false and DiagnosticMode=false after test.

NO live DayZ/Enforce compilation or runtime test performed here. Investigate compile errors and rollback to backup immediately. Keep PNH_Lab/profiles and storage_2 untouched.

Limitations: some damage paths bypass this callback, absence of log does not prove no callback; GetDamage(dmgZone,Health) is zone damage, not global applied health; heuristicLethal is a comparison only; no future life transition or infected implementation.
