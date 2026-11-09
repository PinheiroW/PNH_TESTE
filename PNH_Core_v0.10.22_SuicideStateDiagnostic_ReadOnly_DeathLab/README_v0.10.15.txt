PNH_Core v0.10.15 - Passive Death Event Observer

NEW: PNH_DeathEventObserverConfig.c, PNH_DeathEventObserver.c, existing PNH_LifeCyclePlayerHook.c (reused, no second PlayerBase override)
Changed: PNH_CoreModule.c initialization and bootstrap version.

DeathEventObserver.json created OFF by default; observation is in-memory logging only.
The PlayerBase.EEKilled(Object killer) hook delegates to super and does NOT change health, damage, inventory, respawn, any official life records, or journals.

DO NOT kill the existing LifeId=3 lab character to test. The initial test is bootstrap only, no new death.
The hook observes final vanilla deaths only; it does NOT detect intercepted pre-death JoinThem/PNH infected transformations, which require a separate verified transition source.
Actual publish, commit, recovery, and deduplication remain NOT IMPLEMENTED in this version.

Installation: back up old source and profile, replace source, compile with DayZ Tools, replace PBO. Verify logs show Bootstrap v0.10.15 and [DEO] DeathEventObserver.json created (OFF). No enablement needed for first test.
