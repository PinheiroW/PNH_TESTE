PNH_Core v0.10.11 - isolated staging primitive (LAB)
=====================================================
Based on v0.10.10 tested source.

New: PNH_JournalStagingConfig.c and PNH_JournalStaging.c.
Bootstrap creates $profile:PNH/Core/JournalStaging.json with Enabled=false,
AllowStaging=false and ApprovedSteamID64 empty.

The server registers NO CF hooks for this new module, and never calls its
StageAfterTrustedDeath method. On a successful bootstrap, expect:
[PNH_CORE] Bootstrap v0.10.11 initialized on server
[PNH_CORE][PJS] JournalStaging.json created (OFF)
On next restart:
[PNH_CORE][PJS] stageMethod=AVAILABLE wiring=NONE publish=NOT_IMPLEMENTED commit=NOT_IMPLEMENTED NO_WRITE

Staging primitive is compile-testable but NOT runtime-tested. It accepts a
trusted server-only invocation in a FUTURE release, validates config, existing
player/current life, history, backup and conflicting files, writes only the
.create.tmp journal, and reads it back. It leaves failed/incomplete temps for
manual investigation. No final journal publish, rename/move, player update,
commit, deletion, or retry is implemented. Do NOT toggle staging flags in lab.

Known limitations: no crash-safe atomic journal publish, no concurrency lock,
no trusted death signal wired, only known partial filename patterns scanned,
and no SHA-256 from within Enforce. Do NOT use on production. Do NOT provoke
death/respawn. Existing feature configs should stay OFF.
