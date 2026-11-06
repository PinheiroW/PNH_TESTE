# Humans 1.4.1 — October 28, 2026

- Fixed player attack detection: Humans now recognize DayZ's current light/heavy melee command, including punches and weapon attacks.
- Removed blocking cooldown and random blocking chance. Visible frontal attacks within 3 metres trigger a defensive guard whenever the Human can act.
- Humans can interrupt their own melee swing to block. Cancelled swings cannot inflict a delayed hit or play a delayed swing sound.
- Blocking releases shortly after the incoming attack ends so Humans can counterattack. Rear attacks, projectiles, hit reactions and traversal still matter.
- Removed the separate dodge timer's ability to prevent a block.
- Faster close pursuit and movement around a player's guard.
- Humans try to step out from behind teammates near a player before the normal stuck timeout, while preserving doorway turn-taking.
- A newly acquired enemy clears an old obstacle-recovery route so Humans can turn toward the threat immediately.
- Reduced the extra pause between attacks while preserving native animation completion, weapon contact timing and damage.
- Humans stay alert after close combat instead of resuming a slow sneak when the player turns away.
- Successful hits against players now deal exactly 25 HP and 25 shock per hit with every loadout. Player blocking prevents both. This fixed player damage ignores body-part and worn-armor multipliers; native invulnerability is retained.

Pursuit remains 80 metres, hearing remains 20 metres, and observation memory remains 10 minutes. Weapon variety, soldier baseball bats, loot and player blocking are retained. Damage against infected remains the native weapon/fist damage.

Install the new @Humans on the server and every client, then restart. No classnames or server XML entries changed.
