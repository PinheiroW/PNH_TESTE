class CfgPatches
{
	class AZ_VampireAnims
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters"
		};
	};
	class AZ_VampireAnims_Feed
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters"
		};
	};
	class AZ_VampireAnims_Drain
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters"
		};
	};
	class AZ_VampireAnims_Rear
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters"
		};
	};
	class AZ_VampireAnims_Bound
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters"
		};
	};
	class AZ_VampireAnims_Hiss
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters"
		};
	};
	class AZ_VampireAnims_Snarl
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters"
		};
	};
};
class CfgVehicles
{
	class Man;
	class SurvivorBase: Man
	{
		class enfAnimSys
		{
			meshObject="dz\characters\bodies\player_testing.xob";
			graphName="dz\anims\workspaces\player\player_main\player_main.agr";
			defaultInstance="azo\az_vampire_anims\player_vamp.asi";
			skeletonName="player_testing.xob";
			startNode="MasterControl";
		};
	};
};
