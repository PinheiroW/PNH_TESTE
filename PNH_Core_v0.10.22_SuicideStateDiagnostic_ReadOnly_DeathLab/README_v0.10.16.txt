PNH_Core v0.10.16 - Final Death Transition Candidate, READ ONLY

Based on verified v0.10.15 source. Changes:
- New PNH_DeathTransitionCandidateConfig.c and PNH_DeathTransitionCandidate.c.
- Bootstrap initializes DTC config OFF by default; existing DEO remains OFF unless lab config enabled.
- Existing DEO passes a validated player record to DTC after observing actual EEKilled.
- DTC checks current record, refuses existing final journal, and deduplicates by SteamID64+LifeId+Revision IN MEMORY for the current process.
- DTC only logs FINAL_DEATH_CANDIDATE_IN_MEMORY; it does not transform any player, intercept damage, authorize any action, create official files, or commit.
- DTC only executes when BOTH DeathEventObserver and DeathTransitionCandidate are enabled in the isolated DeathLab, and a genuine EEKilled occurs.

IMPORTANT: FINAL EEKilled is NOT the pre-fatal hook used for human-to-infected transformation; implementing that requires a separate guarded experiment. This candidate is ONLY for finalized vanilla death bookkeeping.

Test sequence: (1) back up source and compile (2) bootstrap on DeathLab WITHOUT connecting or killing anyone; expect new DeathTransitionCandidate.json created (OFF) (3) stop; validate config and logs. Do NOT restore snapshot or kill a player at this stage. No compilation test is possible in this packaging environment.
