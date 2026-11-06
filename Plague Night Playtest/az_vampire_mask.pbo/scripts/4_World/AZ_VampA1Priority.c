#ifdef NAMALSK_SURVIVAL


class AZ_VampA1Priority
{
	static bool AtA1(vector position)
	{
		string world;
		GetGame().GetWorldName(world);
		world.ToLower();
		return world == "namalsk" && vector.Distance(position, AZ_VampCryptA1.ORB) <= 8.0;
	}

	static bool Active()
	{
		if (!GetGame().GetWorld().IsNight())
			return false;
		if (GetGame().IsServer())
			return AZ_VampProgress.s_BloodMoon;
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		return player && player.m_AZBloodMoon;
	}
}

modded class EVRStorm
{
	protected int m_AZA1PendingLogAt;
	void EVRStorm()
	{

		vfx_the_thing orb = GetOrb();
		if (orb)
			orb.AZA1SetEVROwned(m_BlowoutStaticLight);
	}

	override void UpdateServer()
	{
		if (AZ_VampA1Priority.AtA1(GetAnomalyPosition()))
		{
			if (AZ_VampA1Priority.Active())
				return;



			array<PlayerBase> players;
			AZ_VampState.OnlinePlayers(players);
			foreach (PlayerBase pending : players)
			{
				if (pending && GetGame().GetTime() < pending.m_AZCryptAt && vector.Distance(pending.GetPosition(), GetAnomalyPosition()) <= m_AnomalyScale * 6.75)
					return;
			}
		}
		super.UpdateServer();
	}

	override void TeleportObject(notnull Object object, vector position, bool stability)
	{
		PlayerBase player = PlayerBase.Cast(object);
		if (AZ_VampA1Priority.AtA1(GetAnomalyPosition()) && player && GetGame().GetTime() < player.m_AZCryptAt)
		{

			if (GetGame().GetTime() >= m_AZA1PendingLogAt)
			{
				m_AZA1PendingLogAt = GetGame().GetTime() + 1000;
				Print("[AZ_Vamp] A1 route=crypt-pending; native transfer skipped");
			}
			return;
		}
		if (player)
			Print("[AZ_Vamp] EVR route=native atA1=" + AZ_VampA1Priority.AtA1(GetAnomalyPosition()));
		// Announce the native juncture move after the crypt-pending veto.
		AZ_VampSnap.AllowMove(player);
		super.TeleportObject(object, position, stability);
	}
}

modded class vfx_the_thing
{
	protected bool m_AZA1EVROwned;
	protected bool m_AZA1Masked;
	protected bool m_AZA1LightEnabled;
	protected bool m_AZA1SiteEnabled;
	protected bool m_AZA1SiteMasked;
	protected BlowoutStaticLight m_AZA1SiteLight;
	protected ref array<EffectSound> m_AZA1MutedSounds = {};

	void AZA1SetEVROwned(BlowoutStaticLight siteLight)
	{
		m_AZA1EVROwned = true;
		m_AZA1SiteLight = siteLight;
		AZA1RefreshMask();
	}

	protected void AZA1MuteLoop(EffectSound sound)
	{
		if (!sound)
			return;
		if (m_AZA1MutedSounds.Find(sound) == -1)
			m_AZA1MutedSounds.Insert(sound);
		sound.AZA1PriorityMute(true);
	}

	protected void AZA1RefreshMask()
	{
		bool mask = AZA1ShouldMask();
		if (mask && !m_AZA1Masked)
		{
			SetInvisible(true);
			if (m_BlowoutLight)
				m_AZA1LightEnabled = m_BlowoutLight.IsEnabled();
			if (m_AnomalyAura)
				m_AnomalyAura.Stop();
			m_AnomalyAura = null;
			m_ParticleSwitched = false;
		}
		if (mask)
		{
			if (m_BlowoutLight)
				m_BlowoutLight.SetEnabled(false);
			foreach (BlowoutSound name, EffectSound wave : m_AnomalySounds)
				AZA1MuteLoop(wave);
			AZA1MuteLoop(m_WHSound);
		}
		else if (m_AZA1Masked)
		{
			SetInvisible(false);
			if (m_BlowoutLight)
				m_BlowoutLight.SetEnabled(m_AZA1LightEnabled);
			foreach (EffectSound muted : m_AZA1MutedSounds)
			{
				if (muted)
					muted.AZA1PriorityMute(false);
			}
			m_AZA1MutedSounds.Clear();
		}
		m_AZA1Masked = mask;


		bool siteMask = m_AZA1EVROwned && m_AZA1SiteLight && AZ_VampA1Priority.AtA1(m_AZA1SiteLight.GetPosition()) && AZ_VampA1Priority.Active();
		if (m_AZA1SiteLight)
		{
			if (siteMask && !m_AZA1SiteMasked)
				m_AZA1SiteEnabled = m_AZA1SiteLight.IsEnabled();
			if (siteMask)
				m_AZA1SiteLight.SetEnabled(false);
			else if (m_AZA1SiteMasked)
				m_AZA1SiteLight.SetEnabled(m_AZA1SiteEnabled);
		}
		m_AZA1SiteMasked = siteMask;
	}

	bool AZA1ShouldMask()
	{
		return m_AZA1EVROwned && !IsLantiaLink() && AZ_VampA1Priority.AtA1(GetPosition()) && AZ_VampA1Priority.Active();
	}

	override void UpdateFrame()
	{

		AZA1RefreshMask();
		if (!m_AZA1Masked)
			super.UpdateFrame();
	}

	override EffectSound RegisterSound(BlowoutSound sound)
	{
		EffectSound wave = super.RegisterSound(sound);
		if (m_AZA1Masked)
			AZA1MuteLoop(wave);
		return wave;
	}

	override void SetUnderAttack(bool state)
	{
		super.SetUnderAttack(state);
		if (m_AZA1Masked)
			AZA1MuteLoop(m_WHSound);
	}
}

modded class BlowoutLight
{
	override void OnFrameLightSource(IEntity other, float timeSlice)
	{
		super.OnFrameLightSource(other, timeSlice);


		vfx_the_thing orb = vfx_the_thing.Cast(m_Parent);
		if (orb && orb.AZA1ShouldMask())
			SetEnabled(false);
	}
}
#endif
