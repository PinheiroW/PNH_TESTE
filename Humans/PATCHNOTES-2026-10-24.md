# Humans — October 24, 2026

## New characters

- Added male and female soldier Humans using the original DayZ soldier models and textures.
- New spawn classes: `HumanM_SoldierNormal` and `HumanM_SoldierNormal_Female`. Both use the requested `HumanM` prefix; the female soldier uses female player voice sounds.
- Both soldiers always spawn holding a baseball bat. Their bats use the existing player bat animations and can be looted after death.
- Both soldiers share the Humans' patrol, door opening, scavenging, combat, and blocking behavior.

## Combat and reactions

- Fixed infected ignoring Humans. Infected now recognize them as enemies, pursue them, and damage them. Humans fight back against infected as well as players.
- Humans turn toward threats faster and check for targets more frequently, helping them react when approached from the side or behind.
- Punches use the normal player light-punch damage profile and reach, with body hits rather than random headshot bonuses. Armor still affects damage.
- Fixed punches missing because the attack animation turned the Human away. A committed punch connects while its target remains within reach and clear line of sight, unless the target successfully blocks it.
- Each swing can damage its original target only once. Inherited infected hit handlers cannot add extra Human melee hits.

## Defensive blocking

- Humans can now briefly raise a player-style guard instead of simply taking every hit.
- They have a 35% chance to attempt a block when an eligible nearby enemy begins a melee attack.
- Guards last 0.8–1.1 seconds and have a 4–7 second cooldown afterward, preventing continuous blocking.
- Unarmed Humans use the player fist guard; bat-equipped Humans use the player baseball-bat guard.
- Guards protect against frontal melee attacks. Rear hits and projectiles remain effective, and an unguarded hit interrupts the defensive pose.
- Humans cannot attack during a guard or cancel an attack already in progress into a block.

## Movement and awareness

- Humans now back away from blocked objects before moving sideways and trying a new approach. Recovery follows several checked waypoints, alternates sides, and widens the detour after repeated obstruction.
- Humans approach and vault suitable low fences before choosing a longer route around them. Vaulting uses a player animation and checks body clearance, the crossing path, and the landing.
- Humans take turns passing through a detected doorway. Followers wait outside while the leader opens and clears the entrance, then follow the same checked passage.
- A Human blocked at an entrance backs away and waits briefly before retrying, keeping the passage reserved until its retreat finishes.
- Humans can climb onto a reachable stationary car to pursue a player standing on it, using player climbing animations. They remain on the vehicle surface, can fight there, and climb down when the player leaves.
- Car climbing requires a clear approach and landing, with a rise of 0.45�1.9 metres. Tall walls, building roofs, and locked doors remain excluded.
- Moving players can be heard within 20 metres. Crouch-walking, prone movement, and standing still remain quiet; crouch-running is audible.
- Humans investigate the last position where they heard movement. Hearing alone does not give them a visual combat target.
- Humans avoid sitting or lying down in rain, on road-like surfaces, or near threats. Nearby threats can prevent rest even when hidden from view. Hearing a player also wakes a resting Human.

## Server package

- Included separate types, events, and example Chernarus territory XML files, with registration instructions.
- Updated the types and event entries for all six Human variants, using near-equal spawn weights.
- Humans remains standalone and does not require Expansion AI.

Release: **1.3.2**. All updated mod source files are also applied to `P:\Humans`.
