class CfgPatches
{
	class JoinThem
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Scripts",
			"DZ_Characters",
			"DZ_Anims_Anm_Player",
			"DZ_Characters_Zombies",
			"DZ_Weapons_Melee"
		};
		ammo[]=
		{
			"JoinThemClaw",
			"JoinThemClawPlate"
		};
	};
};
class CfgMods
{
	class JoinThem
	{
		dir="JoinThem";
		name="JoinThem";
		author="Horizon Ascend";
		type="mod";
		inputs="JoinThem/inputs.xml";
		dependencies[]=
		{
			"World",
			"Mission"
		};
		class defs
		{
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"JoinThem/scripts/4_World"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"JoinThem/scripts/5_Mission"
				};
			};
		};
	};
};
class CfgAmmo
{
	class MeleeFist;
	class JoinThemClaw: MeleeFist
	{
		class DamageApplied
		{
			type="Melee";
			bleedThreshold=0.1;
			class Health
			{
				damage=40;
				armorDamage=0.1;
			};
			class Blood
			{
				damage=200;
			};
			class Shock
			{
				damage=24;
			};
		};
	};
	class JoinThemClawPlate: JoinThemClaw
	{
		class DamageApplied
		{
			type="Melee";
			bleedThreshold=0.1;
			class Health
			{
				damage=66.666664;
				armorDamage=0.1;
			};
			class Blood
			{
				damage=200;
			};
			class Shock
			{
				damage=24;
			};
		};
	};
};
