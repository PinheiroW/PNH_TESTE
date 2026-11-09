PNH_Core v0.10.19 - Fatal Damage Correlation (READ-ONLY; DEATHLAB ONLY)

Based on full packaged v0.10.18 source; adds:
  PNH_FatalDamageCorrelationConfig.c
  PNH_FatalDamageCorrelation.c
  existing PNH_LifeCyclePlayerHook.c additions
  existing PNH_CoreModule.c bootstrap/init additions

FDC is OFF by default. It creates only its own default JSON configuration on first bootstrap.
When enabled, it observes each accepted callback on the SAME PlayerBase instance in RAM.
At EEKilled (before super.EEKilled), it logs either:
  NO_PRIOR_DAMAGE_OBSERVED, RECENT_DAMAGE_OBSERVED (<=15 sec), or STALE_LAST_DAMAGE.
These are time-correlations, NOT reliable death-cause determinations.
The last observed damage is NOT necessarily the cause of death. Does not cover every death path.
No player identity or Steam ID is printed. No persistent life records are read or written.
No Health/Shock/vanilla return modifications; no transformation; no commit.
The original FDO stays OFF; DEO/DTC must remain OFF for isolated tests.
Logging capped at 20 damage and 10 death lines per server process. State per entity and bounded.
IMPORTANT: Script compilation cannot be performed outside DayZ Tools. Back up original source and PBO.
Run only one server instance at once; only DeathLab profiles/storage, port 2502. Keep PNH_Lab intact.

STEP 1: unpack, compile, run DeathLab. Expect bootstrap v0.10.19 and FDC JSON created OFF.
STEP 2: stop server. Set ONLY DeathLab/profiles/PNH/Core/FatalDamageCorrelation.json Enabled=true; keep FDO=false, DEO=false, DTC=false.
STEP 3: restart DeathLab; verify [FDC] Enabled=true and zero script exceptions.
STEP 4: non-fatal single light hit then inspect DAMAGE_OBSERVED. No death test yet.
STEP 5: only after evaluating evidence, separately authorize one controlled death test.
STEP 6: stop server, return FDC Enabled=false and audit PNH files vs pretest baseline.

Limitations: no compile/runtime validation in this environment; potential version-specific Enforce API differences require local testing. Do not put on production.
