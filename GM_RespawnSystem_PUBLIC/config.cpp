class CfgPatches
{
	class GM_RespawnSystem_PUBLIC
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Scripts",
			"DZ_Characters",
			"DZ_Characters_Tops",
			"DZ_Characters_Pants"
		};
	};
};
class CfgMods
{
	class GM_RespawnSystem_PUBLIC
	{
		dir="GM_RespawnSystem_PUBLIC";
		picture="";
		action="";
		hideName=1;
		hidePicture=1;
		name="GM_RespawnSystem_PUBLIC";
		credits="GHOST";
		author="GHOST";
		authorID="";
		version=1;
		extra=0;
		type="mod";
		inputs="GM_RespawnSystem_PUBLIC/Inputs.xml";
		dependencies[]=
		{
			"Game",
			"World",
			"Mission"
		};
		class defs
		{
			class gameScriptModule
			{
				value="";
				files[]=
				{
					"GM_RespawnSystem_PUBLIC/scripts/3_Game"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"GM_RespawnSystem_PUBLIC/scripts/4_World"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"GM_RespawnSystem_PUBLIC/scripts/5_Mission"
				};
			};
		};
	};
};
class CfgVehicles
{
	class TShirt_ColorBase;
	class GM_TShirt_ghost: TShirt_ColorBase
	{
		scope=2;
		visibilityModifier=0.85000002;
		hiddenSelectionsTextures[]=
		{
			"GM_RespawnSystem_PUBLIC\data\g_d\gd_tshirt.paa",
			"GM_RespawnSystem_PUBLIC\data\g_d\gd_tshirt.paa",
			"GM_RespawnSystem_PUBLIC\data\g_d\gd_tshirt.paa"
		};
	};
	class TrackSuitPants_ColorBase;
	class GM_TrackSuitPants_ghost: TrackSuitPants_ColorBase
	{
		scope=2;
		visibilityModifier=0.85000002;
		hiddenSelectionsTextures[]=
		{
			"GM_RespawnSystem_PUBLIC\data\g_d\gd_pants.paa",
			"GM_RespawnSystem_PUBLIC\data\g_d\gd_pants.paa",
			"GM_RespawnSystem_PUBLIC\data\g_d\gd_pants.paa"
		};
	};
};
