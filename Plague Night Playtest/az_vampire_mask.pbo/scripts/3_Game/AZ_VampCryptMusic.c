





modded class DynamicMusicPlayer
{
	static bool s_AZQuiet;
	static const float AZ_QUIET_FADE_S = 2.0;
	protected float m_AZQuietFade = 1.0;

	override void OnUpdate(float timeslice)
	{
		if (!s_AZQuiet)
		{
			m_AZQuietFade = 1.0;
			super.OnUpdate(timeslice);
			return;
		}
		if (m_SoundPlaying)
		{
			m_AZQuietFade = Math.Max(0, m_AZQuietFade - timeslice / AZ_QUIET_FADE_S);
			m_SoundPlaying.SetFadeOutFactor(m_AZQuietFade);
			if (m_AZQuietFade <= 0)
			{
				m_SoundPlaying.Stop();
				m_SoundPlaying = null;
			}
		}
	}
}
