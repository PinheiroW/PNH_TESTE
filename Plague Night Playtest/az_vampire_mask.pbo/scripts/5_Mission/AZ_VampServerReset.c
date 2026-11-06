


modded class MissionServer
{
	override void OnInit()
	{
		AZ_VampLootPlace.ServerReset();
		AZ_VampCryptLoot.ServerReset();
		AZ_VampVillageSupplies.ServerReset();
		super.OnInit();
	}
}
