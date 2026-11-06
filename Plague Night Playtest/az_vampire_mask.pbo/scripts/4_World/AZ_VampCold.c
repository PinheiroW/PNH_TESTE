


modded class Environment
{
	override protected void ProcessHeatComfort()
	{
		super.ProcessHeatComfort();
		if (m_Player && m_Player.m_AZVampire)
		{
			m_HeatComfort = 0;
			m_Player.GetStatHeatComfort().Set(0);
		}
	}





	override bool IsInsideBuilding()
	{
		if (!m_Player)
			return false;
		if (super.IsInsideBuilding())
			return true;
		vector azPos = m_Player.GetPosition();
		return AZ_VampCrypt.Inside(azPos) || (AZ_VampCryptLights.InCryptBox(azPos) && IsUnderRoof());
	}
}
