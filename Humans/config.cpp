class CfgPatches
{
	class Humans
	{
		units[]=
		{
			"HumanTraderF_Journalist",
			"HumanArmedM_HunterOld_Summer",
			"HumanArmedM_Jacket_grey",
			"HumanArmedM_JournalistSkinny",
			"HumanArmedF_SurvivorNormal_Blue",
			"HumanArmedM_SoldierNormal",
			"HumanArmedM_SoldierNormal_Female",
			"HumanM_HunterOld_Summer",
			"HumanM_Jacket_grey",
			"HumanM_JournalistSkinny",
			"HumanF_SurvivorNormal_Blue",
			"HumanM_SoldierNormal",
			"HumanM_SoldierNormal_Female"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_AI",
			"DZ_data_aiconfigs",
			"DZ_Characters_Zombies",
			"DZ_Characters_Headgear",
			"DZ_Anims_Anm_Player",
			"DZ_Scripts",
			"DZ_Sounds_Effects",
			"DZ_Weapons_Melee",
			"DZ_Weapons_Melee_Blunt",
			"DZ_Gear_Tools",
			"DZ_Sounds_Characters",
			"DZ_Pistols_Glock",
			"DZ_Weapons_Magazines",
			"DZ_Weapons_Ammunition"
		};
		ammo[]=
		{
			"HumansPlayer_MeleeFist",
			"HumansPlayer_MeleeBluntLight_1",
			"HumansPlayer_MeleeBluntLight_2",
			"HumansPlayer_MeleeBluntLight_3",
			"HumansPlayer_MeleeCraftedLight_3",
			"HumansPlayer_MeleeCraftedLight_4",
			"HumansPlayer_MeleeSharpLight_3",
			"HumansGlockHit"
		};
	};
};
class CfgMods
{
	class Humans
	{
		dir="Humans";
		inputs="Humans/inputs.xml";
		name="Humans";
		author="Horizon Ascend";
		type="mod";
		dependencies[]=
		{
			"Game",
			"World",
			"Mission"
		};
		skeletonDefinitions="Humans/anims/cfg/skeletons.anim.xml";
		class defs
		{
			class gameScriptModule
			{
				value="";
				files[]=
				{
					"Humans/scripts/3_Game"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"Humans/scripts/5_Mission"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"Humans/scripts/4_World"
				};
			};
		};
	};
};
class AIParams
{
	class AgentTeams
	{
		TeamList[]+=
		{
			"Humans"
		};
		class Humans
		{
			friends[]=
			{
				"Humans"
			};
		};
	};
};
class CfgAIBehaviours
{
	class InfectedMale;
	class HumanCivilian: InfectedMale
	{
		teamName="Humans";
	};
};
class CfgVehicles
{
	class ZombieMaleBase;
	class Human_Base: ZombieMaleBase
	{
		scope=0;
		displayName="Human";
		woman=0;
		simulation="dayzinfected";
		aiAgentTemplate="HumanCivilian";
		attachments[]={};
		hiddenSelections[]=
		{
			"camo"
		};
		class Cargo
		{
			itemsCargoSize[]={6,5};
			allowOwnedCargoManipulation=1;
			openable=0;
		};
		meleeAmmo="MeleeFist";
		class enfanimsys
		{
			meshObject="dz\characters\zombies\z_hermit_m.xob";
			graphname="Humans\anims\Human.agr";
			defaultinstance="Humans\anims\Human.asi";
			skeletonName="hermit_newbindpose.xob";
			startnode="MasterControl";
		};
		class InputController
		{
			movementSpeedMapping[]={0,1.5,3,5.5};
			lookAtFilterTimeout=0.1;
			lookAtFilterSpeed=6.283185;
		};
	};
	class HumanTraderF_Journalist: Human_Base
	{
		scope=2;
		displayName="Strange Trader";
		woman=1;
		attachments[]=
		{
			"Mask"
		};
		class enfanimsys: enfanimsys
		{
			graphname="Humans/anims/Human.agr";
			defaultinstance="Humans/anims/Human.asi";
		};
		model="\DZ\characters\zombies\journalist_normal_f.p3d";
		hiddenSelectionsTextures[]=
		{
			"Humans\data\HumanTraderF.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"dz\characters\zombies\data\journalist_normal_f.rvmat"
		};
	};
	class ZmbM_HunterOld_Summer;
	class HumanReanimatedM_HunterOld_Summer: ZmbM_HunterOld_Summer
	{
		scope=1;
		displayName="Reanimated Human";
		class Cargo
		{
			itemsCargoSize[]={6,5};
			allowOwnedCargoManipulation=1;
			openable=0;
		};
		class enfanimsys
		{
			meshObject="dz\characters\zombies\z_hermit_m.xob";
			graphname="Humans/anims/reanimated/ZmbHuman.agr";
			defaultinstance="Humans/anims/reanimated/ZmbHuman.asi";
			skeletonName="hermit_newbindpose.xob";
			startnode="MasterControl";
		};
	};
	class ZmbM_Jacket_grey;
	class HumanReanimatedM_Jacket_grey: ZmbM_Jacket_grey
	{
		scope=1;
		displayName="Reanimated Human";
		class Cargo
		{
			itemsCargoSize[]={6,5};
			allowOwnedCargoManipulation=1;
			openable=0;
		};
		class enfanimsys
		{
			meshObject="dz\characters\zombies\z_hermit_m.xob";
			graphname="Humans/anims/reanimated/ZmbHuman.agr";
			defaultinstance="Humans/anims/reanimated/ZmbHuman.asi";
			skeletonName="hermit_newbindpose.xob";
			startnode="MasterControl";
		};
	};
	class ZmbM_JournalistSkinny;
	class HumanReanimatedM_JournalistSkinny: ZmbM_JournalistSkinny
	{
		scope=1;
		displayName="Reanimated Human";
		class Cargo
		{
			itemsCargoSize[]={6,5};
			allowOwnedCargoManipulation=1;
			openable=0;
		};
		class enfanimsys
		{
			meshObject="dz\characters\zombies\z_hermit_m.xob";
			graphname="Humans/anims/reanimated/ZmbHuman.agr";
			defaultinstance="Humans/anims/reanimated/ZmbHuman.asi";
			skeletonName="hermit_newbindpose.xob";
			startnode="MasterControl";
		};
	};
	class ZmbF_SurvivorNormal_Blue;
	class HumanReanimatedF_SurvivorNormal_Blue: ZmbF_SurvivorNormal_Blue
	{
		scope=1;
		displayName="Reanimated Human";
		class Cargo
		{
			itemsCargoSize[]={6,5};
			allowOwnedCargoManipulation=1;
			openable=0;
		};
		class enfanimsys
		{
			meshObject="dz\characters\zombies\z_hermit_m.xob";
			graphname="Humans/anims/reanimated/ZmbHuman.agr";
			defaultinstance="Humans/anims/reanimated/ZmbHuman.asi";
			skeletonName="hermit_newbindpose.xob";
			startnode="MasterControl";
		};
	};
	class ZmbM_SoldierNormal;
	class HumanReanimatedM_SoldierNormal: ZmbM_SoldierNormal
	{
		scope=1;
		displayName="Reanimated Human";
		class Cargo
		{
			itemsCargoSize[]={6,5};
			allowOwnedCargoManipulation=1;
			openable=0;
		};
		class enfanimsys
		{
			meshObject="dz\characters\zombies\z_hermit_m.xob";
			graphname="Humans/anims/reanimated/ZmbHuman.agr";
			defaultinstance="Humans/anims/reanimated/ZmbHuman.asi";
			skeletonName="hermit_newbindpose.xob";
			startnode="MasterControl";
		};
	};
	class ZmbF_SoldierNormal;
	class HumanReanimatedM_SoldierNormal_Female: ZmbF_SoldierNormal
	{
		scope=1;
		displayName="Reanimated Human";
		class Cargo
		{
			itemsCargoSize[]={6,5};
			allowOwnedCargoManipulation=1;
			openable=0;
		};
		class enfanimsys
		{
			meshObject="dz\characters\zombies\z_hermit_m.xob";
			graphname="Humans/anims/reanimated/ZmbHuman.agr";
			defaultinstance="Humans/anims/reanimated/ZmbHuman.asi";
			skeletonName="hermit_newbindpose.xob";
			startnode="MasterControl";
		};
	};
	class HumanM_HunterOld_Summer: Human_Base
	{
		scope=2;
		displayName="Stranger";
		woman=0;
		model="\DZ\characters\zombies\Hunter_old_M.p3d";
		hiddenSelectionsTextures[]=
		{
			"Humans\data\human01.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"dz\characters\zombies\data\Hunter_old_M.rvmat"
		};
	};
	class HumanM_Jacket_grey: Human_Base
	{
		scope=2;
		displayName="Stranger";
		woman=0;
		model="\DZ\characters\zombies\jacket_above0.p3d";
		hiddenSelectionsTextures[]=
		{
			"Humans\data\human04.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"dz\characters\zombies\data\jacket.rvmat"
		};
	};
	class HumanM_JournalistSkinny: Human_Base
	{
		scope=2;
		displayName="Stranger";
		woman=0;
		model="\DZ\characters\zombies\journalist_skinny_m.p3d";
		hiddenSelectionsTextures[]=
		{
			"Humans\data\human03.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"dz\characters\zombies\data\journalist_skinny_m.rvmat"
		};
	};
	class HumanF_SurvivorNormal_Blue: Human_Base
	{
		scope=2;
		displayName="Stranger";
		woman=1;
		model="\DZ\characters\zombies\Survivor_normal_f.p3d";
		hiddenSelectionsTextures[]=
		{
			"Humans\data\human02.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"dz\characters\zombies\data\survivor_normal_f.rvmat"
		};
	};
	class HumanM_SoldierNormal: Human_Base
	{
		scope=2;
		displayName="Stranger";
		woman=0;
		model="\DZ\characters\zombies\soldier_normal_m.p3d";
		hiddenSelectionsTextures[]=
		{
			"Humans\data\human05.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"dz\characters\zombies\data\soldier_normal_m.rvmat"
		};
	};
	class HumanM_SoldierNormal_Female: Human_Base
	{
		scope=2;
		displayName="Stranger";
		woman=1;
		model="\DZ\characters\zombies\ZmbF_SoldierNormal.p3d";
		hiddenSelectionsTextures[]=
		{
			"Humans\data\human06.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"dz\characters\zombies\data\ZmbF_SoldierNormal.rvmat"
		};
	};
	class HumanArmedM_HunterOld_Summer: Human_Base
	{
		scope=2;
		displayName="Stranger";
		woman=0;
		model="\DZ\characters\zombies\Hunter_old_M.p3d";
		hiddenSelectionsTextures[]=
		{
			"Humans\data\human01.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"dz\characters\zombies\data\Hunter_old_M.rvmat"
		};
		class enfanimsys: enfanimsys
		{
			defaultinstance="Humans/anims/HumanArmed.asi";
		};
	};
	class HumanArmedM_Jacket_grey: Human_Base
	{
		scope=2;
		displayName="Stranger";
		woman=0;
		model="\DZ\characters\zombies\jacket_above0.p3d";
		hiddenSelectionsTextures[]=
		{
			"Humans\data\human04.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"dz\characters\zombies\data\jacket.rvmat"
		};
		class enfanimsys: enfanimsys
		{
			defaultinstance="Humans/anims/HumanArmed.asi";
		};
	};
	class HumanArmedM_JournalistSkinny: Human_Base
	{
		scope=2;
		displayName="Stranger";
		woman=0;
		model="\DZ\characters\zombies\journalist_skinny_m.p3d";
		hiddenSelectionsTextures[]=
		{
			"Humans\data\human03.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"dz\characters\zombies\data\journalist_skinny_m.rvmat"
		};
		class enfanimsys: enfanimsys
		{
			defaultinstance="Humans/anims/HumanArmed.asi";
		};
	};
	class HumanArmedF_SurvivorNormal_Blue: Human_Base
	{
		scope=2;
		displayName="Stranger";
		woman=1;
		model="\DZ\characters\zombies\Survivor_normal_f.p3d";
		hiddenSelectionsTextures[]=
		{
			"Humans\data\human02.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"dz\characters\zombies\data\survivor_normal_f.rvmat"
		};
		class enfanimsys: enfanimsys
		{
			defaultinstance="Humans/anims/HumanArmed.asi";
		};
	};
	class HumanArmedM_SoldierNormal: Human_Base
	{
		scope=2;
		displayName="Stranger";
		woman=0;
		model="\DZ\characters\zombies\soldier_normal_m.p3d";
		hiddenSelectionsTextures[]=
		{
			"Humans\data\human05.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"dz\characters\zombies\data\soldier_normal_m.rvmat"
		};
		class enfanimsys: enfanimsys
		{
			defaultinstance="Humans/anims/HumanArmed.asi";
		};
	};
	class HumanArmedM_SoldierNormal_Female: Human_Base
	{
		scope=2;
		displayName="Stranger";
		woman=1;
		model="\DZ\characters\zombies\ZmbF_SoldierNormal.p3d";
		hiddenSelectionsTextures[]=
		{
			"Humans\data\human06.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"dz\characters\zombies\data\ZmbF_SoldierNormal.rvmat"
		};
		class enfanimsys: enfanimsys
		{
			defaultinstance="Humans/anims/HumanArmed.asi";
		};
	};
};
class CfgSoundShaders
{
	class Humans_TraderFGreet1_Shader
	{
		samples[]=
		{
			
			{
				"Humans\tradersound\TraderFGreet1",
				1
			}
		};
		volume=1;
		range=20;
	};
	class Humans_TraderFGreet2_Shader
	{
		samples[]=
		{
			
			{
				"Humans\tradersound\TraderFGreet2",
				1
			}
		};
		volume=1;
		range=20;
	};
	class Humans_TraderFGreet3_Shader
	{
		samples[]=
		{
			
			{
				"Humans\tradersound\TraderFGreet3",
				1
			}
		};
		volume=1;
		range=20;
	};
	class Humans_TraderFGreet4_Shader
	{
		samples[]=
		{
			
			{
				"Humans\tradersound\TraderFGreet4",
				1
			}
		};
		volume=1;
		range=20;
	};
	class Humans_TraderFGoodbye1_Shader
	{
		samples[]=
		{
			
			{
				"Humans\tradersound\TraderFGoodbye1",
				1
			}
		};
		volume=1;
		range=20;
	};
	class Humans_MaleAttack_Shader
	{
		samples[]=
		{
			
			{
				"DZ\sounds\Characters\voice\male_1\meleeattack_1",
				1
			}
		};
		volume=1;
		range=40;
	};
	class Humans_MalePain_Shader
	{
		samples[]=
		{
			
			{
				"DZ\sounds\Characters\voice\male_1\pain_1",
				1
			}
		};
		volume=1;
		range=40;
	};
	class Humans_MaleDeath_Shader
	{
		samples[]=
		{
			
			{
				"DZ\sounds\Characters\voice\male_1\pain_3",
				1
			}
		};
		volume=1;
		range=40;
	};
	class Humans_FemaleAttack_Shader
	{
		samples[]=
		{
			
			{
				"DZ\sounds\Characters\voice\female_1\meleeattack_1",
				1
			}
		};
		volume=1;
		range=40;
	};
	class Humans_FemalePain_Shader
	{
		samples[]=
		{
			
			{
				"DZ\sounds\Characters\voice\female_1\pain_1",
				1
			}
		};
		volume=1;
		range=40;
	};
	class Humans_FemaleDeath_Shader
	{
		samples[]=
		{
			
			{
				"DZ\sounds\Characters\voice\female_1\pain_3",
				1
			}
		};
		volume=1;
		range=40;
	};
	class Humans_GlockReload_SoundShader
	{
		samples[]=
		{
			
			{
				"dz\sounds\weapons\firearms\glock19\handling\mag_in",
				1
			}
		};
		volume=0.60000002;
		range=20;
	};
	class Humans_GlockShot_SoundShader
	{
		samples[]=
		{
			
			{
				"dz\sounds\weapons\firearms\glock19\wpn_glock19_shot_close_01",
				1
			},
			
			{
				"dz\sounds\weapons\firearms\glock19\wpn_glock19_shot_close_02",
				1
			},
			
			{
				"dz\sounds\weapons\firearms\glock19\wpn_glock19_shot_close_03",
				1
			}
		};
		volume=1;
		range=200;
		rangeCurve[]=
		{
			{0,1},
			{25,0.80000001},
			{100,0.30000001},
			{200,0}
		};
	};
	class Humans_GlockTail_SoundShader
	{
		samples[]=
		{
			
			{
				"dz\sounds\weapons\firearms\glock19\glock19_tail_forest",
				1
			}
		};
		volume=0.60000002;
		range=200;
		rangeCurve[]=
		{
			{0,1},
			{50,0.60000002},
			{200,0}
		};
	};
};
class CfgSoundSets
{
	class Humans_TraderFGreet1_SoundSet
	{
		soundShaders[]=
		{
			"Humans_TraderFGreet1_Shader"
		};
		volumeFactor=1;
		spatial=1;
		doppler=0;
		loop=0;
	};
	class Humans_TraderFGreet2_SoundSet
	{
		soundShaders[]=
		{
			"Humans_TraderFGreet2_Shader"
		};
		volumeFactor=1;
		spatial=1;
		doppler=0;
		loop=0;
	};
	class Humans_TraderFGreet3_SoundSet
	{
		soundShaders[]=
		{
			"Humans_TraderFGreet3_Shader"
		};
		volumeFactor=1;
		spatial=1;
		doppler=0;
		loop=0;
	};
	class Humans_TraderFGreet4_SoundSet
	{
		soundShaders[]=
		{
			"Humans_TraderFGreet4_Shader"
		};
		volumeFactor=1;
		spatial=1;
		doppler=0;
		loop=0;
	};
	class Humans_TraderFGoodbye1_SoundSet
	{
		soundShaders[]=
		{
			"Humans_TraderFGoodbye1_Shader"
		};
		volumeFactor=1;
		spatial=1;
		doppler=0;
		loop=0;
	};
	class Humans_MaleAttack_SoundSet
	{
		soundShaders[]=
		{
			"Humans_MaleAttack_Shader"
		};
		volumeFactor=1;
		frequencyRandomizer=0.1;
		spatial=1;
		doppler=0;
		loop=0;
	};
	class Humans_MalePain_SoundSet
	{
		soundShaders[]=
		{
			"Humans_MalePain_Shader"
		};
		volumeFactor=1;
		frequencyRandomizer=0.1;
		spatial=1;
		doppler=0;
		loop=0;
	};
	class Humans_MaleDeath_SoundSet
	{
		soundShaders[]=
		{
			"Humans_MaleDeath_Shader"
		};
		volumeFactor=1;
		frequencyRandomizer=0.1;
		spatial=1;
		doppler=0;
		loop=0;
	};
	class Humans_FemaleAttack_SoundSet
	{
		soundShaders[]=
		{
			"Humans_FemaleAttack_Shader"
		};
		volumeFactor=1;
		frequencyRandomizer=0.1;
		spatial=1;
		doppler=0;
		loop=0;
	};
	class Humans_FemalePain_SoundSet
	{
		soundShaders[]=
		{
			"Humans_FemalePain_Shader"
		};
		volumeFactor=1;
		frequencyRandomizer=0.1;
		spatial=1;
		doppler=0;
		loop=0;
	};
	class Humans_FemaleDeath_SoundSet
	{
		soundShaders[]=
		{
			"Humans_FemaleDeath_Shader"
		};
		volumeFactor=1;
		frequencyRandomizer=0.1;
		spatial=1;
		doppler=0;
		loop=0;
	};
	class Humans_GlockReload_SoundSet
	{
		soundShaders[]=
		{
			"Humans_GlockReload_SoundShader"
		};
		volumeFactor=1;
		frequencyRandomizer=0;
		spatial=1;
		loop=0;
	};
	class Humans_GlockShot_SoundSet
	{
		soundShaders[]=
		{
			"Humans_GlockShot_SoundShader"
		};
		volumeFactor=1;
		frequencyRandomizer=0.02;
		spatial=1;
		loop=0;
	};
	class Humans_GlockTail_SoundSet
	{
		soundShaders[]=
		{
			"Humans_GlockTail_SoundShader"
		};
		volumeFactor=1;
		frequencyRandomizer=0;
		spatial=1;
		loop=0;
	};
};
class CfgAmmo
{
	class MeleeFist;
	class HumansPlayer_MeleeFist: MeleeFist
	{
		class DamageApplied
		{
			type="Melee";
			bleedThreshold=1;
			transferShockToDamage=0;
			class Health
			{
				damage=20;
				armorDamage=0.1;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=20;
			};
		};
	};
	class MeleeBluntLight_1;
	class HumansPlayer_MeleeBluntLight_1: MeleeBluntLight_1
	{
		class DamageApplied
		{
			type="Melee";
			bleedThreshold=1;
			transferShockToDamage=0;
			class Health
			{
				damage=25;
				armorDamage=0.1;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=25;
			};
		};
	};
	class MeleeBluntLight_2;
	class HumansPlayer_MeleeBluntLight_2: MeleeBluntLight_2
	{
		class DamageApplied
		{
			type="Melee";
			bleedThreshold=1;
			transferShockToDamage=0;
			class Health
			{
				damage=25;
				armorDamage=0.1;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=25;
			};
		};
	};
	class MeleeBluntLight_3;
	class HumansPlayer_MeleeBluntLight_3: MeleeBluntLight_3
	{
		class DamageApplied
		{
			type="Melee";
			bleedThreshold=1;
			transferShockToDamage=0;
			class Health
			{
				damage=25;
				armorDamage=0.1;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=25;
			};
		};
	};
	class MeleeCraftedLight_3;
	class HumansPlayer_MeleeCraftedLight_3: MeleeCraftedLight_3
	{
		class DamageApplied
		{
			type="Melee";
			bleedThreshold=1;
			transferShockToDamage=0;
			class Health
			{
				damage=25;
				armorDamage=0.1;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=25;
			};
		};
	};
	class MeleeCraftedLight_4;
	class HumansPlayer_MeleeCraftedLight_4: MeleeCraftedLight_4
	{
		class DamageApplied
		{
			type="Melee";
			bleedThreshold=1;
			transferShockToDamage=0;
			class Health
			{
				damage=25;
				armorDamage=0.1;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=25;
			};
		};
	};
	class MeleeSharpLight_3;
	class HumansPlayer_MeleeSharpLight_3: MeleeSharpLight_3
	{
		class DamageApplied
		{
			type="Melee";
			bleedThreshold=1;
			transferShockToDamage=0;
			class Health
			{
				damage=25;
				armorDamage=0.1;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=25;
			};
		};
	};
	class Bullet_9x19;
	class HumansGlockHit: Bullet_9x19
	{
		class DamageApplied
		{
			type="Projectile";
			bleedThreshold=1;
			class Health
			{
				damage=15;
				armorDamage=0;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=5;
			};
		};
	};
};
