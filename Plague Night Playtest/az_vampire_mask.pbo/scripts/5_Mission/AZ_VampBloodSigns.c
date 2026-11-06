











class AZ_VampBloodSigns
{
	static const bool AMBIENT_WOLF_HOWLS = false;
	static const float LANTERN_NEAR = 60;
	static const float FRENZY_NEAR = 25;
	protected bool m_On;
	protected float m_HowlIn;
	protected float m_ScanIn;
	protected ref map<Object, AZ_VampRedLanternLight> m_Red = new map<Object, AZ_VampRedLanternLight>;
	protected ref array<ref EffectSound> m_Howls = new array<ref EffectSound>;
	protected ref map<PlayerBase, ref EffectSound> m_Frenzy = new map<PlayerBase, ref EffectSound>;
	protected ref EffectSound m_Theme;
	protected ref EffectSound m_Drone;
	protected float m_DroneIn;
	protected int m_Hour = -1;
	static int s_MoonsHeard;
	static const float THEME_SEC = 242;

	void Update(float dt, bool on)
	{
		if (on != m_On)
		{
			m_On = on;
			if (on)
			{
				for (int i = 0; i < 3; i++)
					Howl();


				s_MoonsHeard++;
				if (s_MoonsHeard == 1)
					Theme();
				else
					m_DroneIn = 0;
				m_HowlIn = Math.RandomFloat(40, 90);
			}
			else
			{
				ClearRed();
				ClearFrenzy();
				StopMusic();
			}
		}
		if (!m_On)
			return;
		m_DroneIn -= dt;
		if (m_DroneIn <= 0 && !m_Drone)
		{
			Man dm = GetGame().GetPlayer();
			if (dm)
				m_Drone = AZ_PlayMusic("AZ_PlagueNight_BloodMoonDrone_SoundSet", dm.GetPosition(), 4.0, 4.0, true);
		}
		int y, mo, d, h, mi;
		GetGame().GetWorld().GetDate(y, mo, d, h, mi);
		if (m_Hour == 23 && h == 0)
			Theme();
		m_Hour = h;
		m_HowlIn -= dt;
		if (m_HowlIn <= 0)
		{
			Howl();
			m_HowlIn = Math.RandomFloat(40, 90);
		}
		m_ScanIn -= dt;
		if (m_ScanIn <= 0)
		{
			m_ScanIn = 2;


			ScanFrenzy();
		}
	}

	protected void Theme()
	{
		Man me = GetGame().GetPlayer();
		if (!me)
			return;
		if (m_Drone)
			m_Drone.SoundStop();
		m_Drone = null;
		if (m_Theme)
			m_Theme.SoundStop();
		m_Theme = AZ_PlayMusic("AZ_PlagueNight_BloodMoon_SoundSet", me.GetPosition(), 0, 0, false);
		m_DroneIn = THEME_SEC;
	}

	protected void StopMusic()
	{
		if (m_Theme)
			m_Theme.SoundStop();
		if (m_Drone)
			m_Drone.SoundStop();
		m_Theme = null;
		m_Drone = null;
	}

	protected void Howl()
	{
		if (!AMBIENT_WOLF_HOWLS)
			return;
		Man p = GetGame().GetPlayer();
		if (!p)
			return;
		float a = Math.RandomFloat(0, Math.PI2);
		float r = Math.RandomFloat(180, 320);
		vector pos = p.GetPosition() + Vector(Math.Cos(a) * r, 0, Math.Sin(a) * r);
		pos[1] = GetGame().SurfaceY(pos[0], pos[2]) + 1;
		EffectSound s = SEffectManager.PlaySound("WolfHowl_SoundSet", pos);
		if (s)
		{
			s.SetAutodestroy(true);
			m_Howls.Insert(s);
			if (m_Howls.Count() > 8)
				m_Howls.Remove(0);
		}
	}

	protected void ScanLanterns()
	{
		Man p = GetGame().GetPlayer();
		if (!p)
			return;
		vector me = p.GetPosition();

		array<Object> gone = new array<Object>;
		foreach (Object k, AZ_VampRedLanternLight l : m_Red)
		{
			if (!k || vector.Distance(k.GetPosition(), me) > LANTERN_NEAR + 10)
				gone.Insert(k);
		}
		foreach (Object g : gone)
			Restore(g);
		array<Object> objs = new array<Object>;
		array<CargoBase> proxies = new array<CargoBase>;
		GetGame().GetObjectsAtPosition(me, LANTERN_NEAR, objs, proxies);
		foreach (Object o : objs)
		{
			AZ_VampPumpkinLantern pl = AZ_VampPumpkinLantern.Cast(o);
			if (!pl || m_Red.Contains(o))
				continue;
			pl.SetPilotLight(false);
			AZ_VampRedLanternLight light = AZ_VampRedLanternLight.Cast(AZ_VampLightGate.Light(AZ_VampRedLanternLight, pl.GetPosition()));
			if (light)
				light.AttachOnObject(pl, "0 0.25 0");
			m_Red.Insert(o, light);
		}
	}

	protected void ScanFrenzy()
	{
		Man me = GetGame().GetPlayer();
		if (!me)
			return;
		array<PlayerBase> gone = new array<PlayerBase>;
		foreach (PlayerBase k, EffectSound s : m_Frenzy)
		{
			if (!k || !k.IsAlive() || !k.m_AZVampire || vector.Distance(k.GetPosition(), me.GetPosition()) > FRENZY_NEAR + 5)
				gone.Insert(k);
		}
		foreach (PlayerBase g : gone)
		{
			EffectSound gs = m_Frenzy.Get(g);
			if (gs)
				gs.SoundStop();
			m_Frenzy.Remove(g);
		}
		array<Object> objs = new array<Object>;
		array<CargoBase> proxies = new array<CargoBase>;
		GetGame().GetObjectsAtPosition3D(me.GetPosition(), FRENZY_NEAR, objs, proxies);
		foreach (Object o : objs)
		{
			PlayerBase v = PlayerBase.Cast(o);
			if (!v || !v.IsAlive() || !v.m_AZVampire || m_Frenzy.Contains(v))
				continue;
			string snd = "AZ_VampFrenzyF_SoundSet";
			if (v.IsMale())
				snd = "AZ_VampFrenzyM_SoundSet";
			EffectSound loop = SEffectManager.PlaySoundOnObject(snd, v, 1.0, 1.0, true);
			m_Frenzy.Insert(v, loop);
		}
	}

	protected void ClearFrenzy()
	{
		foreach (PlayerBase k, EffectSound s : m_Frenzy)
		{
			if (s)
				s.SoundStop();
		}
		m_Frenzy.Clear();
	}

	protected void Restore(Object o)
	{
		AZ_VampRedLanternLight l = m_Red.Get(o);
		if (l)
			l.Destroy();
		AZ_VampPumpkinLantern pl = AZ_VampPumpkinLantern.Cast(o);
		if (pl)
			pl.UpdateGlowState();
		m_Red.Remove(o);
	}

	protected void ClearRed()
	{
		array<Object> all = new array<Object>;
		foreach (Object k, AZ_VampRedLanternLight l : m_Red)
			all.Insert(k);
		foreach (Object o : all)
			Restore(o);
	}
}

modded class MissionGameplay
{
	protected ref AZ_VampBloodSigns m_AZBloodSigns;

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p)
			return;
		bool on = p.m_AZBloodMoon;
		if (!m_AZBloodSigns)
		{
			if (!on)
				return;
			m_AZBloodSigns = new AZ_VampBloodSigns();
		}
		m_AZBloodSigns.Update(timeslice, on);
	}
}


const float AZ_MUSIC_VOLUME = 0.6;

EffectSound AZ_PlayMusic(string soundSet, vector pos, float fadeIn, float fadeOut, bool loop)
{
	EffectSound s = SEffectManager.CreateSound(soundSet, pos, fadeIn, fadeOut, loop);
	if (!s)
		return null;
	s.SetSoundWaveKind(WaveKind.WAVEMUSIC);
	// Blood moon theme/drone at 60% (Oliver 31 Oct: "a bit too intense or loud").
	s.SetSoundMaxVolume(AZ_MUSIC_VOLUME);
	s.SoundPlay();
	if (fadeIn <= 0)
		s.SetSoundVolume(AZ_MUSIC_VOLUME);
	return s;
}
