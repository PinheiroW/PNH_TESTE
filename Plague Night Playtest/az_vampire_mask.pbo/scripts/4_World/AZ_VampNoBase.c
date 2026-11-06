


modded class CraftFenceKit
{
	override bool CanDo(ItemBase ingredients[], PlayerBase player)
	{
		return false;
	}
}

modded class CraftWatchtowerKit
{
	override bool CanDo(ItemBase ingredients[], PlayerBase player)
	{
		return false;
	}
}

modded class CraftTerritoryFlagKit
{
	override bool CanDo(ItemBase ingredients[], PlayerBase player)
	{
		return false;
	}
}

modded class CraftShelterKit
{
	override bool CanDo(ItemBase ingredients[], PlayerBase player)
	{
		return false;
	}
}

modded class KitBase
{
	override bool IsDeployable()
	{
		return false;
	}
}
