PNH_Core v0.10.8 — Life 3 Transition Plan / Dry Run — READ ONLY LAB

Based on all v0.10.7 sources. New module: PNH_Life3TransitionPlan, [PNH_CORE][LTP].
Configuration: $profile:PNH/Core/Life3TransitionPlan.json (created OFF at bootstrap).
When enabled, checks the life 3/revision 3 player baseline, consecutive committed histories,
archive and rotation backup consistency, no active journal or future artifacts,
known writer flags OFF, and constructs a candidate SourceLifeId=3 / Revision=3 /
Phase=DEATH_RECORDED journal record solely in memory.
Logs: JOURNAL_PLAN_OK and PLAN_READY on CLIENT_READY, with NO_WRITE.
No journal write, life commit, death, respawn, recovery or file deletion has been added.
Only config creation at bootstrap is permitted. Do not enable any existing writer gate.
This is NOT crash-proof or concurrency-safe; never infer production readiness from logs.
First test: bootstrap only. Then enable read-only and connect; do not kill the test character.
