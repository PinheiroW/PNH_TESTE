PNH_Core v0.10.13 — Isolated Lab Staging JSON Write Test (SAFE OFF)

- Adds PNH_LabStagingTest independent of PNH_JournalStaging.
- Bootstrap creates $profile:PNH/Core/LabStagingTest.json disabled; no character login or death event can invoke the lab writer.
- First validation: bootstrap, expect [PNH_CORE][PLS] LabStagingTest.json created (OFF).
- Test mode (later, only after inspecting code and lab backup): startup-only opt-in requires Enabled=1, LabWriteRequested=1, ApprovedSteamID64, ApprovalPhrase exact I_APPROVE_LAB_STAGING_LIFE3.
- Isolated output: $profile:PNH/LabStaging/<SteamID64>.life3.labtest.tmp; Phase=LAB_TEST_ONLY, never DEATH_RECORDED.
- Writer aborts on any occupied output path, unknown partial files, active journal, active known writers, unexpected player state, missing history.
- Readback checks schema, owner, source life/revision/state, phase; never overwrites/deletes existing output.
- Not atomic and not durable recovery; no publication, death hook or life4 commit. Existing v0.10.12 module remains unchanged.
- Only one server process may write to profile directory; never run concurrently with production.
- DO NOT enable any write flag for bootstrap test.
