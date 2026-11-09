PNH_Core v0.10.14 -- Journal Publication Boundary (READ ONLY)

New module: PNH_JournalPublicationPreflight.c
Config: $profile:PNH/Core/JournalPublicationPreflight.json (created OFF)

After a successful bootstrap, stop server, set Enabled=1 (UTF-8 no BOM), restart and connect the existing life-3 lab player. If the v0.10.13 isolated lab artifact exists with LAB_TEST_ONLY, the expected log is:
[PNH_CORE][JPP] LAB_ONLY_REJECTED ... publish=DENIED NO_WRITE
This is a POSITIVE security test: a laboratory JSON must not be published as a real death.

No implementation of journal publication, death event, recovery, or life-4 commit is provided by this release.
No file write is performed by JPP except creating its OFF config at first bootstrap.
The prior PNH_LabStagingTest stays disabled via its existing JSON.
Do not turn on any other writer flags. Do not delete the existing lab artifact.
