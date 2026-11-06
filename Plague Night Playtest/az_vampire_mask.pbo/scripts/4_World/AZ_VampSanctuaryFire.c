// AZ-2604: keep vanilla heating; recover a failed client loop without restarting particles.
modded class Fireplace
{
	protected bool m_AZSanctuaryFire;
	protected ref Timer m_AZSanctuarySoundTimer;
	protected int m_AZSanctuarySoundRetries;

	void Fireplace()
	{
		RegisterNetSyncVariableBool("m_AZSanctuaryFire");
	}

	void AZ_StartSanctuaryFire()
	{
		if (!GetGame().IsServer())
			return;
		m_AZSanctuaryFire = true;
		// StartFire already sets m_IsBurning and starts vanilla heating/UTS.
		// Publish the intensity immediately instead of waiting for the first heating tick.
		StartFire();
		RefreshFireState();
		Synchronize();
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(AZ_LogSanctuaryHeat);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_LogSanctuaryHeat, 30000, false);
	}

	protected void AZ_LogSanctuaryHeat()
	{
		bool sourceActive = false;
		if (m_UTSource)
			sourceActive = m_UTSource.IsActive();
		Print("[AZ-2604] sanctuary fire burning=" + IsBurning() + " temperature=" + GetTemperature() + " sourceActive=" + sourceActive);
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		if (GetGame().IsDedicatedServer() || !m_AZSanctuaryFire || m_AZSanctuarySoundTimer)
			return;
		// Allow sound headers/fades to finish before considering a retry.
		m_AZSanctuarySoundTimer = new Timer(CALL_CATEGORY_GAMEPLAY);
		m_AZSanctuarySoundTimer.Run(5, this, "AZ_CheckSanctuarySound", null, true);
	}

	void AZ_CheckSanctuarySound()
	{
		if (!IsBurning())
		{
			m_AZSanctuarySoundRetries = 0;
			return;
		}
		if (m_SoundFireLoop && m_SoundFireLoop.IsSoundPlaying())
		{
			m_AZSanctuarySoundRetries = 0;
			return;
		}
		if (vector.DistanceSq(GetGame().GetCurrentCameraPosition(), GetPosition()) > 900 || m_AZSanctuarySoundRetries >= 3)
			return;
		int state = GetFireState();
		if (state != FireplaceFireState.START_FIRE && state != FireplaceFireState.SMALL_FIRE && state != FireplaceFireState.NORMAL_FIRE)
			return;
		// PlaySoundSetLoop treats any existing handle as success, even a stopped one.
		SoundFireStop();
		if (state == FireplaceFireState.NORMAL_FIRE)
			SoundFireHeavyStart();
		else
			SoundFireLightStart();
		m_AZSanctuarySoundRetries++;
		Print("[AZ-2604] sanctuary fire sound retry=" + m_AZSanctuarySoundRetries + " state=" + state);
	}

	override void EEDelete(EntityAI parent)
	{
		if (m_AZSanctuarySoundTimer)
			m_AZSanctuarySoundTimer.Stop();
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(AZ_LogSanctuaryHeat);
		super.EEDelete(parent);
	}
}
