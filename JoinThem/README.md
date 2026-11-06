# JoinThem 0.3.0

Install @JoinThem on the server and every client. Add it to -mod, copy Keys/JoinThem.bikey into the server keys directory, then restart both. No Humans or Expansion AI dependency is required.

Lethal normal damage opens a centered Join Them? / Respawn choice while unconscious. Join Them revives the same survivor with full health, blood, shock, food and water, removes bleeding and broken legs, keeps worn gear and applies the survivor's native decay texture. Respawn performs normal player death, leaving the original corpse and gear to the native CleanupLifetimeDeadPlayer setting in globals.xml.

Controls: WASD movement, C crouch/stand, Mouse 1 attack. With empty hands, look at a dead survivor within two meters and hold Mouse 2 at the Skin survivor ( Hold Mouse 2 ) prompt. Releasing cancels the action. Skinning uses the native animation and a synchronized skinning sound, produces raw human meat once per corpse, and retains the corpse and equipment.

Joined characters can eat raw meat and cooked human meat, and can still drink. A full-size raw human steak restores 30% of maximum food (1500 energy) and 25 HP. A baked, boiled or dried human steak gives half: 15% food (750 energy) and 12.5 HP. Partial steaks and bites give proportional rewards. Frozen, rotten and burnt meat cannot be consumed. Health and food are capped at their normal maximum. Human meat uses these rewards instead of also applying vanilla human-meat nutrition and cannibal agents. Other raw animal meat uses native consumption behavior. Normal survivors retain their normal diet.

Weapons are blocked from joined characters' hands; worn and carried gear remains available. Native consumption animations work while holding food/drink. The empty-handed profile uses infected attack clips and a native injured movement overlay. Movement controller changes that caused jumping were removed. Hunger, thirst, temperature and warmth from fires remain native.

Male/female zombie idle vocals play randomly when stationary with a 12-28 second cooldown, plus attack vocals and a terminal infected grunt on death. Attacks use server-authoritative range, line of sight and blocking checks.

Known limits: joined status is not saved across server restarts. Scripted SetHealth(0), administrative deletion and kill paths bypassing EEOnDamageCalculated cannot open the choice. Validation coverage and remaining visual/audio checks are in VALIDATION.md. Test missions, logs and private signing keys are excluded from the package.


Death as a joined player uses the native death/Respawn screen; Join Them is offered only to a living survivor entering the initial pending state. Zombie-player torso attacks aim for low health after two unarmored hits, three light-armor hits or four plate-carrier hits. Blocking still prevents valid frontal hits. This uses dedicated ammo profiles and leaves ordinary survivor punches unchanged.


