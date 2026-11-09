PNH_Core v0.10.20 - DeathClassification (DCL) read-only laboratory

Changes from v0.10.19:
- New PNH_DeathClassification.c / Config.c
- Bootstrap updated and DCL.Initialize called after FDC.Initialize
- FDC.OnKilled sends same PlayerBase entity state to DCL.OnDeath
- Four evidence labels: POTENTIALLY_LETHAL_DAMAGE, DEATH_WITHOUT_OBSERVED_DAMAGE, STALE_DAMAGE, INSUFFICIENT_EVIDENCE
- Classifier is disabled by default, writes only initial config if missing.
- To test classification, enable FDC.Enabled=true AND DeathClassification.Enabled=true in DeathLab with server stopped.
- This is diagnostic only, does not assert cause, change damage, transform, or commit.
- Not engine/compiler tested. Snapshot PNH profile before performing death tests.
- If FDC is OFF, DCL cannot receive events.
