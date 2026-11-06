


modded class PlayerBase
{
	static const vector AZ_VAMP_NECK = "0 1.45 0.08";
	bool m_AZVampHeldShown;
	Particle m_AZVampDrip;

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		if (m_AZVampHeld == m_AZVampHeldShown)
			return;
		m_AZVampHeldShown = m_AZVampHeld;
		if (m_AZVampDrip)
		{
			m_AZVampDrip.Stop();
			m_AZVampDrip = null;
		}
		if (m_AZVampHeld && !IsUnconscious())
			Particle.PlayOnObject(ParticleList.AZ_VAMP_BITE_SPURT, this, AZ_VAMP_NECK);
		if (m_AZVampHeld)
			m_AZVampDrip = Particle.PlayOnObject(ParticleList.AZ_VAMP_BITE_DRIP, this, AZ_VAMP_NECK);
	}

	override void EEDelete(EntityAI parent)
	{
		if (m_AZVampDrip)
			m_AZVampDrip.Stop();
		super.EEDelete(parent);
	}
}
