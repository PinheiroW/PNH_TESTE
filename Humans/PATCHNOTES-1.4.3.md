# Humans 1.4.3

All twelve random weapon classnames were verified against installed DayZ configs. Armed civilian rolls choose IDs 2 through 13 uniformly; default armed chance remains 75%. Crowbars are excluded; soldiers retain guaranteed bats.

Fixed unset/missing stored loadouts being converted to fists before randomization. Explicitly saved unarmed NPCs still retain their existing loadout. Failed weapon attachments now retry up to five times without discarding the selected weapon. The server logs its effective JSON spawn chance and attachment failures for diagnosis.

Use Humans/Settings.json in the server profiles directory, with MeleeSpawnChancePercent set to 75 (or 100 to check newly spawned NPCs). Restart to reload. Existing saved unarmed Humans are not rerolled.

Source applied to P:\Humans. PBO built and signed; native weapon classnames and animation resources verified. This patch has not been runtime-tested, and the cause on the reported server is not confirmed without its spawn/attachment logs. Earlier runtime reports in the package are historical.
