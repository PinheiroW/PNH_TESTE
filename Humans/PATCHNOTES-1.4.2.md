# Humans 1.4.2

- Civilian Humans now have a 75% default chance of spawning with a random weapon from the existing twelve-weapon pool. Crowbars remain excluded from new rolls. Soldiers still always spawn with baseball bats.
- The server creates `Humans/Settings.json` inside its configured profiles directory. Set `MeleeSpawnChancePercent` from 0 to 100 and restart the server. The change applies to newly rolled loadouts, not existing saved Humans. Invalid JSON falls back to 75% and logs an error without overwriting the file.
- Removed the fixed 25 HP/25 shock player-hit override. Weapons now use their vanilla default melee damage, shock, armor and body-part processing and impact effects. Fists also return to native punch damage. Player blocking remains effective.

Example Settings.json:
```json
{
  "MeleeSpawnChancePercent": 75
}
```

Install on server and clients and restart. Source changes are mirrored to P:\Humans.

Validation: PBO built and signed; animation/resource validation passed. The previous 327 runtime checks describe 1.4.1, not this release; the full runtime suite was not rerun for 1.4.2.
