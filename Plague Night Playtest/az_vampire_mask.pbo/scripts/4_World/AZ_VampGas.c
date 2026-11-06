// AZ-2590: only the contaminated-area agent bypasses vampire disease immunity.
// Vanilla AreaExposureMdfr routes inhalation through mask protection; vampires
// deliberately inhale the same dose regardless of NBC or space-suit masks.
modded class AreaExposureMdfr
{
	override float TransmitAgents(PlayerBase player, float count)
	{
		if (player && player.m_AZVampire && AZ_VampState.Cfg().vampireGasDamage)
		{
			player.InsertAgent(eAgents.CHEMICAL_POISON, count);
			return count;
		}
		return super.TransmitAgents(player, count);
	}
}

// Chemical protection of worn NBC clothing/filters cannot protect vampires.
// Other protection categories and all human equipment retain vanilla behavior.
modded class ItemBase
{
	override float GetProtectionLevel(int type, bool consider_filter = false, int system = 0)
	{
		if (type == DEF_CHEMICAL && GetGame().IsServer())
		{
			PlayerBase wearer = PlayerBase.Cast(GetHierarchyRootPlayer());
			if (wearer && wearer.m_AZVampire && AZ_VampState.Cfg().vampireGasDamage)
				return 0;
		}
		return super.GetProtectionLevel(type, consider_filter, system);
	}
}
