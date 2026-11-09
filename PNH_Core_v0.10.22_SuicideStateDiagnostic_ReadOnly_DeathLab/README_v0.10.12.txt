PNH_Core v0.10.12 - STAGING INSPECTOR + IN-PROCESS REENTRANCY GUARD

BASE: v0.10.11 (previous 58 files). Lab only.

CHANGES:
- PNH_JournalStaging.Initialize returns bool to selectively register ONLY CF OnClientReady read-only inspection.
- JournalStaging.json: Enabled=1, AllowStaging=0, ApprovedSteamID64="" activates read-only inspection.
- OnClientReady does NOT call StageAfterTrustedDeath; it only reports artifact state.
- New log [PNH_CORE][PJS] STAGING_INSPECTION_READY ... NO_WRITE for clean life 3.
- Existing .create.tmp reports VALID_UNPUBLISHED / INVALID / CONFLICT. No auto deletion, rename, recovery or publish.
- In-process non-reentrant guard around StageAfterTrustedDeath; NOT crash-safe / cross-process lock. StageAfterTrustedDeath remains unconnected to any event, and must NOT be manually triggered without trusted death integration.

NOT IMPLEMENTED: production write test, atomic publish, crash recovery, multi-process locking, real death trigger, life4 commit.

FIRST TEST: compile, start server, observe bootstrap v0.10.12. After normal shutdown, set only Enabled=1 (keep AllowStaging=0, ApprovedSteamID64 blank); restart, connect lab character and look for [PJS] STAGING_INSPECTION_READY ... NO_WRITE.

DO NOT ENABLE STAGING WRITES OR PROVOKE DEATH/RESPAWN.
