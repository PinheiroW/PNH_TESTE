PNH_Core v0.10.9 - Life 3 Journal Preparation (Read-Only Lab)

Based on v0.10.8.1 (the Formula too complex compilation fix is retained).
Adds PNH_Life3JournalPreparation, config at $profile:PNH/Core/Life3JournalPreparation.json.
First boot creates config OFF. Restart after switching Enabled to true for read-only checks.
Logs: [PNH_CORE][LJP] JOURNAL_CANDIDATE_OK / JOURNAL_PREP_READY.
No journal or player file is ever written by this module. It only writes a default disabled config on first start.
The simulated DEATH_RECORDED marker is an in-memory candidate, NOT an actual death event.

The original SecondLifeTransitionGuard still enforces LifeId 2 and is NOT suitable for authorizing life 3 journal writes. Do not turn on legacy write gates for life 3.
Known temp markers checked: .tmp/.pending/.staging. Not an exhaustive filesystem/atomicity guarantee.
This version has not been compiled by DayZ Tools; test only in isolated laboratory.
