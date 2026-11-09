PNH_Core v0.10.10 - Protected Journal Creation preflight (LAB, READ ONLY)

This release validates the physical-creation preconditions for SourceLifeId=3, Revision=3, including the absence of active journal, known temporary files, future history, and backup collisions. Checks prior committed history, last rotation backup, and the existing writer gates.

It does NOT implement a journal writer, hook a death event, update a player record, or create LifeId=4. The config WriteEnabled=true or a non-empty ApprovedSteamID64 is rejected (fail closed).

Bootstrap expected:
[PNH_CORE] Bootstrap v0.10.10 initialized on server
[PNH_CORE][PJC] ProtectedJournalCreation.json created (OFF)

After stopping server, set ONLY Enabled=true in profiles/PNH/Core/ProtectedJournalCreation.json, UTF-8 without BOM. Reconnect lab player.
Expected:
[PJC] CREATION_CANDIDATE_OK ... sourceLife=3 sourceRevision=3 phase=DEATH_RECORDED ... IN_MEMORY_ONLY NO_WRITE
[PJC] CREATION_PREFLIGHT_READY ... candidateNextLife=4 ... creation=DISABLED NO_WRITE

Safety constraints: connection must never create DEATH_RECORDED. Actual write must later be triggered by a vetted server-side death transition, and needs an exclusive-create/atomic publish protocol and crash tests. The legacy JsonFileLoader SaveFile is not atomic. Do not enable other writer gates. Stop server and set Enabled=false after testing.
