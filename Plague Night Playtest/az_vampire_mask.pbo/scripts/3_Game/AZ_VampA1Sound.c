

modded class EffectSound
{
	protected bool m_AZA1PriorityMuted;

	void AZA1PriorityMute(bool muted)
	{
		m_AZA1PriorityMuted = muted;
		AZA1ApplyOutput();
	}

	protected void AZA1ApplyOutput()
	{
		if (!m_SoundWaveObject)
			return;
		if (m_AZA1PriorityMuted)
			m_SoundWaveObject.SetVolumeRelative(0);
		else
			m_SoundWaveObject.SetVolumeRelative(GetSoundVolume());
	}

	override void SetSoundVolume(float volume)
	{
		super.SetSoundVolume(volume);
		if (m_AZA1PriorityMuted)
			AZA1ApplyOutput();
	}

	override void Event_OnFrameUpdate(float time_delta)
	{
		super.Event_OnFrameUpdate(time_delta);
		if (m_AZA1PriorityMuted)
			AZA1ApplyOutput();
	}
}
