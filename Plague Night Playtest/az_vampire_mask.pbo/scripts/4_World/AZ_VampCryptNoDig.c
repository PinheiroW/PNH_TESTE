

modded class ActionDigWorms
{
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (AZ_VampCrypt.Inside(player.GetPosition()))
			return false;
		return super.ActionCondition(player, target, item);
	}
}

modded class ActionDigGardenPlot
{
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (AZ_VampCrypt.Inside(player.GetPosition()))
			return false;
		return super.ActionCondition(player, target, item);
	}
}

modded class ActionDigInStash
{
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (AZ_VampCrypt.Inside(player.GetPosition()))
			return false;
		return super.ActionCondition(player, target, item);
	}
}

modded class ActionDigOutStash
{
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (AZ_VampCrypt.Inside(player.GetPosition()))
			return false;
		return super.ActionCondition(player, target, item);
	}
}
