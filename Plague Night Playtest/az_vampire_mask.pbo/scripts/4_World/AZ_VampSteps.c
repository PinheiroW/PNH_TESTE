



modded class PlayerBase
{
	static const float AZ_STEP_VOL = 0.35;
	static const float AZ_STEP_VOL_SPRINT = 0.6;
	static const float AZ_STEP_NOISE = 0.35;
	static const float AZ_STEP_NOISE_SPRINT = 0.6;

	protected float m_AZStepScale;

	override void OnStepEvent(string pEventType, string pUserString, int pUserInt)
	{
		m_AZStepScale = 0;

		if (m_AZVampire && (!GetGame().IsServer() || AZ_VampEvent.Active()))
		{
			HumanMovementState st = new HumanMovementState();
			GetMovementState(st);
			m_AZStepScale = 1;
			if (st.m_iMovement == DayZPlayerConstants.MOVEMENTIDX_SPRINT)
				m_AZStepScale = 2;
		}
		super.OnStepEvent(pEventType, pUserString, pUserInt);
		m_AZStepScale = 0;
	}

	override AbstractWave PlaySound(SoundObject so, SoundObjectBuilder sob)
	{
		AbstractWave wave = super.PlaySound(so, sob);
		if (wave && m_AZStepScale > 0)
		{
			if (m_AZStepScale > 1)
				wave.SetVolumeRelative(AZ_STEP_VOL_SPRINT);
			else
				wave.SetVolumeRelative(AZ_STEP_VOL);
		}
		return wave;
	}

	override void AddNoise(NoiseParams noisePar, float noiseMultiplier = 1.0)
	{
		if (m_AZStepScale > 1)
			noiseMultiplier = noiseMultiplier * AZ_STEP_NOISE_SPRINT;
		else if (m_AZStepScale > 0)
			noiseMultiplier = noiseMultiplier * AZ_STEP_NOISE;
		super.AddNoise(noisePar, noiseMultiplier);
	}
}
