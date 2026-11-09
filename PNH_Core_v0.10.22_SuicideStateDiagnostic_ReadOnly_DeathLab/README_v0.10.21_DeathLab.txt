PNH_Core v0.10.21 — DeathMechanismObserver (DMO) — DeathLab only

Based on v0.10.20. Read-only diagnostics, disabled by default.
New files: scripts/4_World/PNH_Core/PNH_DeathMechanismObserver.c and PNH_DeathMechanismObserverConfig.c
Modified files: PNH_LifeCyclePlayerHook.c and PNH_CoreModule.c

CONFIG: $profile:PNH/Core/DeathMechanismObserver.json
Default SchemaVersion=1, Enabled=false. Only enable in PNH_DeathLab after initial bootstrap and backup.

Behavior:
- Reads Global Health, Blood, Shock at EEOnDamageCalculated (after super has calculated eligibility).
- Reads Global Health, Blood, Shock at EEKilled BEFORE super; logs only if real identity exists.
- Captures last damage time, zone, ammo, previous Blood, and whether any Head/Brain zone was observed.
- No per-frame work, no timers. Max 12 damage and 12 death diagnostic log lines per server process.
- No change to vanilla callback return or damage; no player state writes, no journal/commit, no zombie transformation.
- A death without recorded Head damage is NOT evidence that no head injury occurred.
- Comparing Blood values is NOT proof of death by bleeding. EEKilled is observed before vanilla super.
- Engine compilation/runtime NOT VERIFIED; package structure and static diff checked only.

Lab smoke test:
1) Compile PBO from this source using DayZ Tools.
2) Run PNH_DeathLab on port 2502 with existing config, profiles and storage_3.
3) Expect Bootstrap v0.10.21 and [DMO] DeathMechanismObserver.json created (OFF).
4) Stop server, enable only DMO as appropriate, restart, inspect [DMO] Enabled=true.
5) Perform one low-risk nonfatal hit; inspect [DMO] DAMAGE_SNAPSHOT; do NOT infer fatal cause.
6) Do NOT perform lethal tests until bootstrap and nonfatal observations are reviewed.
