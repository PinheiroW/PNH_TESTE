modded class PlayerBase
{
	override bool CanSpawnBreathVaporEffect()
	{
		if (m_AZVampire)
			return false;
		return super.CanSpawnBreathVaporEffect();
	}
}
