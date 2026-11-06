# Humans 1.4.0 - October 28, 2026

- Keeps pursuit up to 80 metres from the Human, as requested.
- Remembers observed hostile targets for 10 minutes. Memory uses last observed/heard positions; it does not continuously track players through walls. After losing sight, Humans search the last observation. They can recognize a remembered target again within 80 metres when visible.
- Hears standing footsteps and crouch-running/jogging within 20 metres. Quiet crouch-walking and prone movement remain inaudible.
- Uses quiet rear approaches against visible, unaware players, and reacts when the player turns or readies an attack.
- Attempts to move around a frontal player guard rather than wasting swings into the block.
- Chooses brief blocks or reachable dodge steps in response to nearby attacks. Both have cooldowns; dodging grants no invulnerability.
- Adds short collision-checked bypass steering and recent-obstacle memory. Keeps existing retreat, doorway queues, low-fence vaulting and reachable car climbing.
- Adds civilian weapon variety: BaseballBat, NailedBaseballBat, BarbedBaseballBat, Wrench, LugWrench, PoliceBaton, MeatTenderizer, Pipe, Machete, CrudeMachete, OrientalMachete and Hammer.
- Uses matching vanilla one-handed pipe/two-handed bat player animation profiles, measured right-hand attachment transforms, native damage/range and native swing/contact sounds. Attack timings match clip events.
- Drops each equipped weapon as the same lootable item on death. No replacement copy is spawned for loot.
- Keeps the two soldier variants on guaranteed baseball bats. Crowbars stay out of the random pool. Civilian total armed probability stays at the previous roughly 1 in 30.
- Keeps normal player-light-punch damage and directional player blocking.

Installation: replace @Humans on server and clients and restart. Existing spawn classnames and XML files remain valid. Copy the included HorizonHumans.bikey to the server keys directory if signature checking is enabled. No Expansion AI or CF dependency is added.

Memory lasts for the running NPC; deleting it or restarting the server clears its learned observations. Navigation still depends on the map's navmesh and reachable geometry. No AI can guarantee a route through broken navmesh or impossible/locked barriers.
