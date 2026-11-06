# JoinThem 0.3.0 - October 29, 2026

- Death after joining now explicitly enters a final joined-death state. It uses the vanilla death/Respawn screen and cannot reopen Join Them.
- Both death paths preserve native corpse processing and worn/carried gear. Choosing Respawn still triggers normal respawning.
- Increased zombie-player strikes against survivors: clean torso hits target roughly 40 HP unarmored, 28-30 HP in light armor, and 20 HP in a plate carrier. This is tuned toward low health after 2/3/4 hits respectively, rather than simply doubling weak vanilla fists. Actual results depend on armor and its condition.
- Shock and blood damage are doubled from normal fists. Front-facing blocking, reach, line of sight and attack timing checks remain active. Ordinary survivor punches and attacks against infected keep their existing damage.
- Raw human meat for joined players uses its existing disease-free reward path: no kuru, salmonella or food-poison agents from the meat or bloody-hand consumption data are ingested. Normal survivors retain vanilla disease risk.
- Cooked human meat (baked, boiled or dried) now gives half the raw reward: 15% maximum food and 12.5 HP per full steak, proportional to quantity. It uses the same disease-free consumption path for joined players. Burnt and rotten meat remain blocked.
- Frozen human meat must thaw before the reward path can consume it. A full human steak still restores 30% maximum food and 25 HP, proportional to quantity eaten.
- Private test scene now equips full clothing, helmet, plate carrier, backpack, rifle and pocket items, and allows inspection of the previous corpse after respawning. Fixed the private fire's zero maximum-wind setting that made it extinguish immediately.
# JoinThem 0.2.1

- Fixed the missing Skin survivor ( Hold Mouse 2 ) prompt: the custom input now has its own contextual display instead of relying on the vanilla action cursor's built-in input categories.
- The prompt appears only when the skinning action is available and hides during menus, inventory and other actions.
# JoinThem 0.2.0

- Added empty-handed survivor skinning: look at a dead player within two meters and hold Mouse 2. Prompt: Skin survivor ( Hold Mouse 2 ).
- Added the native skinning animation and synchronized skinning sound, stopped when the action ends or is cancelled.
- Skinning produces raw human meat once per corpse and preserves the body and its equipment.
- Joined characters can eat raw meat only. Drinking remains available; normal survivors keep their usual diet.
- A full human steak restores 30% maximum food and 25 HP, proportional to each bite and the steak's size. Rewards cannot exceed normal maximum stats.
- Removed conflicting movement resource/controller changes that caused jumping while moving. The user confirmed the movement fix works.
- Existing Join/Respawn behavior, full healing, gear retention, crouch/stand, Mouse 1 attacks and zombie vocals remain in place.



