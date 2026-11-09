PNH_Core v0.10.22 — Suicide State Diagnostic — DeathLab only
Based byte-for-byte on v0.10.21, except for two source files + this README.

MODIFIED:
  scripts/4_World/PNH_Core/PNH_CoreModule.c
    Bootstrap label v0.10.22.
  scripts/4_World/PNH_Core/PNH_DeathMechanismObserver.c
    When an authenticated player's EEKilled event is observed, read
    player.CommitedSuicide() once (before PlayerBase.super.EEKilled).
    Add suicideState=true/false to the existing death diagnostic log.

NO NEW CONFIG OR SCHEMA CHANGES. Existing DeathMechanismObserver.json remains
SchemaVersion=1. DMO is disabled by default on a fresh installation, but
if DeathLab already has Enabled=true it will remain enabled. FDC/DCL unchanged.

NO timers, writes to player state, health changes, changes to vanilla EE
callbacks or ordering, new dependencies, persistent state, or transformation.
Output is a diagnostic observation only: suicideState=false does not prove
absence of a suicide and suicideState=true does not prove Head damage.
All other existing v0.10.21 code is unchanged.

TEST PLAN:
1) Preserve original source, compiled PBO, and DeathLab PNH profiles.
2) Rebuild PBO from this package in DayZ Tools (not compiled by author).
3) Start only isolated PNH_DeathLab (port 2502) and verify bootstrap v0.10.22.
4) Confirm DMO Enabled=true, FDC and DCL disabled, zero script errors.
5) One vanilla in-game gun suicide; inspect DMO EEKILLED_PRE_SUPER suicideState.
6) Stop server, audit PNH-profile hashes and disable DMO when finished.

PACKAGE VERIFIED: ZIP CRC and exact static diff. DayZ compiler / runtime NOT
available here; no claim of successful in-game build or test.
