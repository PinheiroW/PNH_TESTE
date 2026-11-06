













modded class MissionGameplay
{
	protected float m_AZWhisperIn = 60;
	protected float m_AZWhisperFollow;
	protected ref EffectSound m_AZWhisper;
	protected vector m_AZWhisperAt;
	protected ref array<vector> m_AZWhisperSpots;
	protected ref array<vector> m_AZWhisperBeds;
	protected ref array<bool> m_AZWhisperBedShhh;
	protected float m_AZCircleCooldown;
	protected ref array<float> m_AZWhisperBedAt;
	protected float m_AZWhisperClock;
	protected float m_AZBedCheck;

	protected int m_AZCircleLeft;
	protected float m_AZCircleNext;
	protected vector m_AZCircleCenter;
	protected vector m_AZCircleToward;
	protected float m_AZCircleAng;
	protected bool m_AZCircleShhh;
	protected ref EffectSound m_AZLine;
	protected float m_AZFarIn = 8;

	protected float m_AZSayAt;
	protected int m_AZSaid;
	protected ref array<string> m_AZSaidNames;

	protected int m_AZRitStage;
	protected float m_AZRitPeakAt;
	protected ref EffectSound m_AZMurmur;
	protected float m_AZRingIn;
	protected float m_AZHeartIn;
	protected bool m_AZWasOnAltar;
	protected bool m_AZWasVampire;
	protected float m_AZRiverAt;
	protected float m_AZEarIn;






	protected ref EffectSound m_AZPortalTone;
	protected bool m_AZPortalCalled;
	protected ref EffectSound m_AZPortalRim;
	protected float m_AZPortalWhisperIn;
	protected bool m_AZPortalComeDown;
	protected float m_AZPortalCheck;
	protected float m_AZTightBeatIn;
	protected float m_AZTightPitchIn;
	protected bool m_AZMoonSeen;
	protected float m_AZMoonCueIn = -1;
	protected bool m_AZMusicMuted;
	protected float m_AZMusicSaved = -1;
	protected float m_AZMusicCheck;



	protected float m_AZNsFade = 1;
	protected bool m_AZNsLogged;
	protected bool m_AZNsNone;
	protected float m_AZGuideCheck;
	protected float m_AZGuideAlone;
	protected string m_AZGuideNight;
	protected ref array<string> m_AZGuideSaid;

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		AZWhisperTick(timeslice);
	}




	protected void AZPortalTick(PlayerBase p, float dt)
	{
		m_AZPortalCheck -= dt;
		if (m_AZPortalCheck > 0)
			return;
		m_AZPortalCheck = 0.5;
		bool open = AZ_VampCryptA1.Ready() && p.IsAlive();
		float d = vector.Distance(p.GetPosition(), AZ_VampCryptA1.Crack());
		if (open && d < 25)
		{
			if (d < 10 && !m_AZPortalRim)
				m_AZPortalRim = SEffectManager.PlaySound("AZ_VampPortal_Rim_SoundSet", AZ_VampCryptA1.Crack(), 2.0, 0.3, true);
			else if (d >= 12 && m_AZPortalRim)
			{
				m_AZPortalRim.SetSoundFadeOut(2.0);
				m_AZPortalRim.SoundStop();
				m_AZPortalRim = null;
			}
			if (d < 15)
				AZPortalWhispers(p, d, dt);
			else
				m_AZPortalWhisperIn = 1.0;
			if (!m_AZPortalTone)
				m_AZPortalTone = SEffectManager.PlaySound("AZ_VampPortal_Tone_SoundSet", AZ_VampCryptA1.Crack(), 2.0, 0.5, true);
			if (d < 15 && !m_AZPortalCalled)
			{
				m_AZPortalCalled = true;
				SEffectManager.PlaySound("AZ_CryptCall_SoundSet", AZ_VampCryptA1.Crack());
			}
			return;
		}
		if (d > 40 || !open)
			m_AZPortalCalled = false;
		if (m_AZPortalTone)
		{
			m_AZPortalTone.SoundStop();
			m_AZPortalTone = null;
		}
		if (m_AZPortalRim)
		{
			m_AZPortalRim.SoundStop();
			m_AZPortalRim = null;
		}
		m_AZPortalComeDown = false;
	}



	protected void AZPortalWhispers(PlayerBase p, float d, float dt)
	{
		if (d < 2.0 && !m_AZPortalComeDown)
		{
			m_AZPortalComeDown = true;
			AZPortalVoice(p, "AZ_VampPortal_ComeDown_SoundSet", 0.8);
			m_AZPortalWhisperIn = 4.0;
			return;
		}
		m_AZPortalWhisperIn -= dt;
		if (m_AZPortalWhisperIn > 0)
			return;
		if (d >= 10)
		{
			AZPortalVoice(p, "AZ_VampPortal_Far_SoundSet", Math.RandomFloat(6, 9));
			m_AZPortalWhisperIn = Math.RandomFloat(4, 6);
		}
		else if (d >= 5)
		{
			AZPortalVoice(p, "AZ_VampPortal_Mid_SoundSet", Math.RandomFloat(3, 5));
			AZPortalVoice(p, "AZ_VampPortal_Mid_SoundSet", Math.RandomFloat(3, 5));
			m_AZPortalWhisperIn = Math.RandomFloat(3, 4);
		}
		else
		{
			for (int i = 0; i < 3; i++)
				AZPortalVoice(p, "AZ_VampPortal_Near_SoundSet", Math.RandomFloat(1.0, 2.0));
			m_AZPortalWhisperIn = Math.RandomFloat(2.2, 3.0);
		}
	}

	protected void AZPortalVoice(PlayerBase p, string soundSet, float r)
	{
		float a = Math.RandomFloat(0, Math.PI2);
		vector off = Vector(Math.Sin(a), 0, Math.Cos(a));
		vector me = p.GetPosition();
		vector at = me + off * r;
		at[1] = me[1] + 1.6;
		SEffectManager.PlaySound(soundSet, at);
	}




	protected void AZMoonCue(PlayerBase p, float dt)
	{
		if (!p.m_AZBloodMoon)
		{
			m_AZMoonSeen = false;
			m_AZMoonCueIn = -1;
			return;
		}
		if (!m_AZMoonSeen)
		{
			m_AZMoonSeen = true;
			if (AZMoonNight() != AZMoonCueRead())
				m_AZMoonCueIn = 2.5;
		}
		if (m_AZMoonCueIn < 0)
			return;
		m_AZMoonCueIn -= dt;
		if (m_AZMoonCueIn > 0)
			return;
		m_AZMoonCueIn = -1;
		AZMoonCueWrite(AZMoonNight());
		if (!p.IsAlive() || AZ_VampCrypt.Inside(p.GetPosition()))
			return;
		SEffectManager.PlaySound("AZ_VampBloodMoonRise_SoundSet", p.GetPosition());
	}


	protected string AZMoonNight()
	{
		int y, mo, d, h, mi;
		GetGame().GetWorld().GetDate(y, mo, d, h, mi);
		if (h < 12)
			d = d - 1;
		return y.ToString() + "-" + mo.ToString() + "-" + d.ToString();
	}

	protected string AZMoonCueRead()
	{
		string line = "";
		FileHandle f = OpenFile("$profile:AZ_VampMoonCue.txt", FileMode.READ);
		if (f)
		{
			FGets(f, line);
			CloseFile(f);
		}
		line = line.Trim();
		return line;
	}

	protected void AZMoonCueWrite(string night)
	{
		FileHandle f = OpenFile("$profile:AZ_VampMoonCue.txt", FileMode.WRITE);
		if (!f)
			return;
		FPrint(f, night);
		CloseFile(f);
	}

	protected void AZNamalskHold(bool below, float dt)
	{
		if (!below)
		{
			m_AZNsFade = 1;
			m_AZNsLogged = false;
			return;
		}
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p)
			return;
		Class dm;
		if (m_AZNsNone)
			return;
		if (!GetGame().GameScript.CallFunction(p, "GetDynamicMusic", dm, null))
		{
			Print("[AZ_Vamp] crypt music: PlayerBase has no GetDynamicMusic (no Namalsk Survival), nothing to hold");
			m_AZNsNone = true;
			return;
		}
		if (!dm)
			return;
		EnScript.SetClassVar(dm, "m_LastEventPlayed", 0, 30.0);
		EnScript.SetClassVar(dm, "m_LastLocationPlayed", 0, 30.0);
		EnScript.SetClassVar(dm, "m_LastTimePlayed", 0, 30.0);
		AbstractWave w;
		EnScript.GetClassVar(dm, "m_ActiveMusic", 0, w);
		if (!m_AZNsLogged)
		{
			m_AZNsLogged = true;
			string playing = "no";
			if (w)
				playing = "yes";
			Print("[AZ_Vamp] crypt music: Namalsk DynamicMusic found, a track playing: " + playing + "; holding it while inside");
		}
		if (!w || m_AZNsFade <= 0)
			return;
		m_AZNsFade = Math.Max(0, m_AZNsFade - dt / 1.5);
		w.SetVolumeRelative(m_AZNsFade);
		if (m_AZNsFade <= 0)
			GetGame().GameScript.CallFunction(dm, "StopMusic", null, null);
	}

	protected void AZMusicMute(bool below, float dt)
	{
		AZNamalskHold(below, dt);
		AbstractSoundScene scene = GetGame().GetSoundScene();
		if (!scene)
			return;
		if (below)
		{
			m_AZMusicCheck -= dt;
			if (m_AZMusicMuted && m_AZMusicCheck > 0)
				return;


			float now = scene.GetMusicVolume();
			if (!m_AZMusicMuted || now > 0.01)
			{
				if (now > 0.01)
					m_AZMusicSaved = now;
				else if (m_AZMusicSaved < 0)
					m_AZMusicSaved = g_Game.m_volume_music;
				scene.SetMusicVolume(0, 1.5);
			}
			m_AZMusicMuted = true;
			m_AZMusicCheck = 3.0;
			return;
		}
		if (!m_AZMusicMuted)
			return;
		m_AZMusicMuted = false;
		if (m_AZMusicSaved >= 0)
			scene.SetMusicVolume(m_AZMusicSaved, 2.0);
		else
			scene.SetMusicVolume(g_Game.m_volume_music, 2.0);
	}

	protected void AZWhisperTick(float dt)
	{
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		bool below = false;
		if (p)
			below = AZ_VampCrypt.Inside(p.GetPosition());
		DynamicMusicPlayer.s_AZQuiet = below;
		AZMusicMute(below, dt);
		AZ_VampRitualCandle.AZ_SetHear(below);
		if (AZ_VampWhisperLines.s_QuietDucked && GetGame().GetTime() >= AZ_VampWhisperLines.s_QuietUntil)
		{
			AZ_VampWhisperLines.s_QuietDucked = false;
			AZ_VampWhisperLines.Duck(1.0, 3.0);
		}
		if (AZ_VampWhisperLines.DuckTick(dt))
			AZ_VampRitualCandle.AZ_ApplyDuck(AZ_VampWhisperLines.s_DuckLevel);
		if (p)
			AZPortalTick(p, dt);
		if (p)
			AZMoonCue(p, dt);
		if (p)
			AZRitualTick(p, dt);
		if (p && p.IsAlive() && m_AZRitStage < 3)
			AZFarTick(p, dt);
		if (!p || !p.IsAlive() || !p.m_AZEventOn || p.m_AZVampire || p.IsUnconscious())
			return;
		if (AZ_VampWhisperLines.Quiet())
			return;
		m_AZWhisperClock += dt;
		if (m_AZWhisper && m_AZWhisper.IsSoundPlaying() && AZWhisperLit(p, m_AZWhisperAt))
		{
			AZWhisperFade();
			m_AZWhisperFollow = 0;
			m_AZCircleLeft = 0;
			Print("[AZ_Vamp] whisper stopped by a light");
		}
		AZWhisperQueues(p);
		AZCircleTick(p, dt);
		AZBedTick(p, dt);
		AZGuideTick(p, dt);
		if (m_AZWhisperFollow > 0)
		{
			m_AZWhisperFollow -= dt;
			if (m_AZWhisperFollow <= 0)
				AZWhisperMoved(p);
		}
		m_AZWhisperIn -= dt;
		if (m_AZWhisperIn > 0)
			return;
		if (AZ_VampCrypt.Inside(p.GetPosition()))
		{
			m_AZWhisperIn = Math.RandomFloat(40, 110);
			if (m_AZRitStage < 2)
				AZWhisperCrypt(p);
			return;
		}
		m_AZWhisperIn = 20;
		if (GetGame().GetWorld().IsNight() && AZWhisperNearVampire(p))
			m_AZWhisperIn = Math.RandomFloat(150, 300);
	}



	protected void AZRitualTick(PlayerBase p, float dt)
	{
		int stage = AZ_VampWhisperLines.s_ClientRitStage;
		if (GetGame().GetTime() - AZ_VampWhisperLines.s_ClientRitAt > 8000)
			stage = 0;
		vector c = AZ_VampWhisperLines.s_ClientRitCenter;
		if (stage == 3 && m_AZRitStage != 3)
		{
			m_AZRitPeakAt = m_AZWhisperClock;
			if (p.m_AZOnAltar)
				m_AZRiverAt = m_AZWhisperClock;
		}
		m_AZRitStage = stage;
		float intoPeak = m_AZWhisperClock - m_AZRitPeakAt;
		bool calm = stage == 3 && intoPeak >= 16;


		int k = AZ_VampWhisperLines.s_ClientRitKneel;
		bool murmur = (stage == 1 || stage == 2) && k > 0;
		if (murmur && !m_AZMurmur)
			m_AZMurmur = SEffectManager.PlaySound("AZ_RitualMurmur_SoundSet", c, 3.0, 2.0, true);
		else if (!murmur && m_AZMurmur)
		{
			m_AZMurmur.SetSoundAutodestroy(true);
			m_AZMurmur.SoundStop();
			m_AZMurmur = null;
		}
		int qtr = AZ_VampWhisperLines.s_ClientRitQuarter;
		if (stage != 1 && stage != 2)
			qtr = 0;
		if (m_AZMurmur)
		{
			float mv = 0.35 + 0.2 * Math.Min(k - 1, 3);
			if (stage == 2)
				mv = mv + 0.15;
			mv = mv + 0.12 * qtr;
			m_AZMurmur.SetSoundVolume(Math.Min(mv, 1.0));
		}



		bool human = p.IsAlive() && !p.m_AZVampire;
		if (human && (stage == 2 || qtr >= 1 || (stage == 3 && !calm)) && vector.Distance(p.GetPosition(), c) < 25)
		{
			m_AZRingIn -= dt;
			if (m_AZRingIn <= 0)
			{
				float r1 = 7;
				float r2 = 10;
				m_AZRingIn = Math.RandomFloat(6, 10);
				if (stage == 1 && qtr == 1)
				{
					r1 = 9;
					r2 = 12;
					m_AZRingIn = Math.RandomFloat(10, 16);
				}
				else if (qtr >= 3)
				{
					r1 = 5;
					r2 = 8;
					m_AZRingIn = Math.RandomFloat(3.5, 5.5);
				}
				if (stage == 3)
				{
					r1 = 4;
					r2 = 7;
					m_AZRingIn = Math.RandomFloat(2.5, 4);
				}
				float a = Math.RandomFloat(0, Math.PI2);
				vector off = Vector(Math.Sin(a), 0, Math.Cos(a));
				vector at = c + off * Math.RandomFloat(r1, r2);
				at[1] = c[1] + Math.RandomFloat(0.8, 2.2);
				if (!AZWhisperLit(p, at))
				{
					string ws = "AZ_VampWhisperVoice_SoundSet";
					float q = Math.RandomFloat01();
					if (q < 0.45)
						ws = "AZ_VampWhisperBreath_SoundSet";
					else if (q < 0.6)
						ws = "AZ_VampWhisperShhh_SoundSet";
					AZWhisperPlay(p, at, ws, "ritual ring");
				}
			}
		}
		else
			m_AZRingIn = Math.RandomFloat(1, 3);
		if (calm && m_AZWhisper && m_AZWhisper.IsSoundPlaying())
			AZWhisperFade();



		if (qtr >= 2 && !p.m_AZOnAltar && p.IsAlive() && vector.Distance(p.GetPosition(), c) < 25)
		{
			m_AZTightBeatIn -= dt;
			if (m_AZTightBeatIn <= 0)
			{
				float tg = 1.05;
				float tv = 0.22;
				if (qtr >= 3)
				{
					tg = 0.78;
					tv = 0.4;
				}
				m_AZTightBeatIn = tg;
				EffectSound tb = SEffectManager.PlaySound("AZ_VampStareBeat_SoundSet", p.GetPosition());
				if (tb)
					tb.SetSoundVolume(tv * AZ_VampWhisperLines.s_DuckLevel);
			}
		}
		else
			m_AZTightBeatIn = 0.5;
		m_AZTightPitchIn -= dt;
		if (m_AZTightPitchIn <= 0)
		{
			m_AZTightPitchIn = 1.0;
			float fr = 1.0;
			if (qtr >= 3)
				fr = 1.04;
			else if (qtr == 2)
				fr = 1.02;
			AZ_VampRitualCandle.AZ_ApplyPitch(fr);
		}

		if (p.m_AZOnAltar && p.IsAlive())
		{
			m_AZHeartIn -= dt;
			if (m_AZHeartIn <= 0)
			{
				float gap = 1.15;
				if (stage == 2)
					gap = 0.85;
				else if (stage == 3)
					gap = Math.Max(0.42, 0.78 - intoPeak * 0.022);
				m_AZHeartIn = gap;
				SEffectManager.PlaySound("AZ_RitualHeart_SoundSet", p.GetPosition());
			}
		}


		if (m_AZWasOnAltar && !p.m_AZOnAltar && stage == 3)
		{
			m_AZEarIn = 1.0;
			m_AZRiverAt = m_AZWhisperClock;
		}
		m_AZWasOnAltar = p.m_AZOnAltar;
		if (m_AZEarIn > 0)
		{
			m_AZEarIn -= dt;
			if (m_AZEarIn <= 0)
				SEffectManager.PlaySound("AZ_RitualEar_SoundSet", p.GetPosition());
		}


		if (p.m_AZVampire && !m_AZWasVampire && m_AZRiverAt > 0 && m_AZWhisperClock - m_AZRiverAt < 40)
		{
			SEffectManager.PlaySound("AZ_RitualWake_SoundSet", p.GetPosition());
			m_AZRiverAt = 0;
			Print("[AZ_Vamp] ritual wake sting");
		}
		m_AZWasVampire = p.m_AZVampire;
	}




	protected void AZGuideTick(PlayerBase p, float dt)
	{
		if (AZ_VampWhisperLines.Quiet())
			return;
		m_AZGuideCheck -= dt;
		if (m_AZGuideCheck > 0)
			return;
		m_AZGuideCheck = 1.0;
		if (!GetGame().GetWorld().IsNight())
			return;
		string nk = AZ_VampWhisperLines.NightKey();
		if (nk != m_AZGuideNight || !m_AZGuideSaid)
		{
			m_AZGuideNight = nk;
			m_AZGuideSaid = new array<string>;
		}
		vector me = p.GetPosition();
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition(me, AZ_VampRitual.AREA_M, objs, null);
		bool vamp = false;
		bool atAltar = false;
		bool lit = false;
		foreach (Object o : objs)
		{
			PlayerBase v = PlayerBase.Cast(o);
			if (v && v != p && v.m_AZVampire && v.IsAlive())
				vamp = true;
			AZ_VampRitualCandle c = AZ_VampRitualCandle.Cast(o);
			if (c)
			{
				if (c.AZ_IsLit())
					lit = true;
				if (vector.Distance(c.GetPosition(), me) < AZ_VampRitual.NEAR_M)
					atAltar = true;
			}
		}
		if (vamp)
		{
			m_AZGuideAlone = 0;
			return;
		}
		if (atAltar && !lit)
			AZGuideSay(p, "guide_light");
		if (p.m_AZOnAltar && AZ_VampWhisperLines.s_ClientRitKneel == 0)
			m_AZGuideAlone += 1.0;
		else
			m_AZGuideAlone = 0;
		if (m_AZGuideAlone >= 20)
			AZGuideSay(p, "guide_kneel");
	}

	protected void AZGuideSay(PlayerBase p, string name)
	{
		if (m_AZGuideSaid.Find(name) >= 0)
			return;
		m_AZGuideSaid.Insert(name);
		m_AZLine = SEffectManager.PlaySound("AZ_VampLine_" + name + "_SoundSet", p.GetPosition());
		if (m_AZLine)
			m_AZLine.SetSoundAutodestroy(true);
		Print("[AZ_Vamp] countess guide " + name);
	}




	protected void AZFarTick(PlayerBase p, float dt)
	{
		if (AZ_VampWhisperLines.Quiet())
			return;
		m_AZFarIn -= dt;
		if (m_AZFarIn > 0)
			return;
		m_AZFarIn = Math.RandomFloat(14, 35);
		vector me = p.GetPosition();
		if (!AZ_VampCrypt.Inside(me))
			return;
		float a = Math.RandomFloat(0, Math.PI2);
		vector off = Vector(Math.Sin(a), 0, Math.Cos(a));
		vector at = me + off * Math.RandomFloat(10, 25);
		at[1] = me[1] + Math.RandomFloat(-1, 4);
		SEffectManager.PlaySound("AZ_CryptFar_SoundSet", at);
	}

	protected void AZWhisperFade()
	{
		if (!m_AZWhisper)
			return;
		m_AZWhisper.SetSoundFadeOut(0.8);
		m_AZWhisper.SoundStop();
	}


	protected void AZWhisperQueues(PlayerBase p)
	{
		array<string> lines = AZ_VampWhisperLines.s_ClientLines;
		if (lines && lines.Count() > 0)
		{
			string name = lines[0];
			lines.Remove(0);

			m_AZLine = SEffectManager.PlaySound("AZ_VampLine_" + name + "_SoundSet", p.GetPosition());
			if (m_AZLine)
				m_AZLine.SetSoundAutodestroy(true);
			Print("[AZ_Vamp] whisper line heard " + name);
		}
		array<ref Param3<vector, vector, int>> circles = AZ_VampWhisperLines.s_ClientCircles;
		if (circles && circles.Count() > 0)
		{
			Param3<vector, vector, int> c = circles[0];
			if (m_AZCircleLeft <= 0)
				AZCircleStart(c.param1, c.param2, c.param3, false);
			circles.Remove(0);
		}
	}

	protected void AZCircleStart(vector center, vector toward, int count, bool shhh)
	{
		m_AZCircleCenter = center;
		m_AZCircleToward = toward;
		m_AZCircleLeft = count;
		m_AZCircleShhh = shhh;
		m_AZCircleNext = Math.RandomFloat(0.3, 1.0);
		m_AZCircleAng = Math.RandomFloat(0, Math.PI2);
		string tag = "";
		if (shhh)
			tag = " (lurker)";
		Print("[AZ_Vamp] whisper circle " + count + tag);
	}

	protected void AZCircleTick(PlayerBase p, float dt)
	{
		if (m_AZCircleLeft <= 0)
			return;
		if (vector.Distance(p.GetPosition(), m_AZCircleCenter) > 7)
		{
			AZWhisperFade();
			m_AZCircleLeft = 0;
			Print("[AZ_Vamp] whisper circle faded (backed off)");
			return;
		}
		m_AZCircleNext -= dt;
		if (m_AZCircleNext > 0)
			return;
		m_AZCircleNext = Math.RandomFloat(2.0, 3.5);
		m_AZCircleLeft--;

		m_AZCircleAng = m_AZCircleAng + Math.RandomFloat(1.2, 2.3);
		float a = m_AZCircleAng;
		if (m_AZCircleToward.Length() > 0.1 && Math.RandomFloat01() < 0.5)
			a = Math.Atan2(m_AZCircleToward[0], m_AZCircleToward[2]) + Math.RandomFloat(-0.9, 0.9);
		vector me = p.GetPosition();
		vector off = Vector(Math.Sin(a), 0, Math.Cos(a));
		vector at = me + off * Math.RandomFloat(3, 6);
		at[1] = me[1] + Math.RandomFloat(0.8, 1.8);
		if (AZWhisperLit(p, at))
		{
			m_AZCircleLeft = 0;
			return;
		}
		string soundSet = "AZ_VampWhisperBreath_SoundSet";
		if (m_AZCircleLeft == 0 && m_AZCircleShhh)
			soundSet = "AZ_VampWhisperShhh_SoundSet";
		else if (Math.RandomFloat01() < 0.4)
			soundSet = "AZ_VampWhisperVoice_SoundSet";
		AZWhisperPlay(p, at, soundSet, "circle");
	}





	protected void AZBedTick(PlayerBase p, float dt)
	{
		m_AZBedCheck -= dt;
		if (m_AZBedCheck > 0 || m_AZCircleLeft > 0 || m_AZWhisperClock < m_AZCircleCooldown || m_AZRitStage >= 2)
			return;
		m_AZBedCheck = 2;
		vector me = p.GetPosition();
		if (!AZ_VampCrypt.Inside(me))
			return;
		AZWhisperBuildSpots();
		for (int i = 0; i < m_AZWhisperBeds.Count(); i++)
		{
			vector b = m_AZWhisperBeds[i];
			float d = vector.Distance(b, me);
			if (d < 2.5 || d > 8 || m_AZWhisperClock < m_AZWhisperBedAt[i] || AZWhisperLit(p, b))
				continue;
			m_AZWhisperBedAt[i] = m_AZWhisperClock + 240;
			if (Math.RandomFloat01() > 0.6)
				continue;
			vector toward = b - me;
			toward[1] = 0;
			toward.Normalize();
			m_AZCircleCooldown = m_AZWhisperClock + 150;
			AZCircleStart(me, toward, Math.RandomIntInclusive(2, 3), m_AZWhisperBedShhh[i]);
			return;
		}
	}

	protected void AZWhisperCrypt(PlayerBase p)
	{
		AZWhisperBuildSpots();
		vector me = p.GetPosition();
		vector look = GetGame().GetCurrentCameraDirection();
		array<vector> pick = new array<vector>;
		foreach (vector s : m_AZWhisperSpots)
		{
			float d = vector.Distance(s, me);
			if (d < 4 || d > 16)
				continue;
			vector to = s - me;
			to.Normalize();
			if (vector.Dot(look, to) > 0.5)
				continue;
			if (AZWhisperLit(p, s))
				continue;
			pick.Insert(s);
		}
		vector at;
		string where = "crypt";
		if (pick.Count() == 0 || Math.RandomFloat01() < 0.25)
		{

			float a = Math.Atan2(look[0], look[2]) + Math.PI + Math.RandomFloat(-1.0, 1.0);
			vector off = Vector(Math.Sin(a), 0, Math.Cos(a));
			at = me + off * Math.RandomFloat(3.5, 7);
			at[1] = me[1] + 1.6;
			where = "crypt nowhere";
			if (AZWhisperLit(p, at))
				return;
		}
		else
		{
			at = pick.GetRandomElement();
		}
		string soundSet = AZWhisperSay(p, true);
		if (soundSet == "")
			soundSet = AZWhisperSet(false);
		AZWhisperPlay(p, at, soundSet, where);
		if (Math.RandomFloat01() < 0.3)
			m_AZWhisperFollow = Math.RandomFloat(2.5, 4.5);
	}


	protected void AZWhisperMoved(PlayerBase p)
	{
		vector me = p.GetPosition();
		vector to = m_AZWhisperAt - me;
		vector side = Vector(to[2], 0, -to[0]);
		side.Normalize();
		float s = Math.RandomFloat(2, 4);
		if (Math.RandomFloat01() < 0.5)
			s = -s;
		vector at = m_AZWhisperAt + side * s;
		if (AZWhisperLit(p, at))
			return;
		AZWhisperPlay(p, at, AZWhisperSet(true), "moved");
	}

	protected bool AZWhisperNearVampire(PlayerBase p)
	{
		vector me = p.GetPosition();
		vector eye = me + "0 1.6 0";
		vector look = GetGame().GetCurrentCameraDirection();
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition(me, 25, objs, null);
		foreach (Object o : objs)
		{
			PlayerBase v = PlayerBase.Cast(o);
			if (!v || v == p || !v.m_AZVampire || !v.IsAlive())
				continue;
			vector vp = v.GetPosition();
			float d = vector.Distance(vp, me);
			if (d < 6)
				continue;
			vector head = vp + "0 1.4 0";
			vector to = head - eye;
			to.Normalize();
			if (vector.Dot(look, to) > 0.6 && AZ_VampRepel.ClearLine(eye, head, p, v))
				continue;
			if (Math.RandomFloat01() > 0.35)
				return false;
			vector jitter = Vector(Math.RandomFloat(-1.5, 1.5), 1.5, Math.RandomFloat(-1.5, 1.5));
			vector at = vp + jitter;
			if (AZWhisperLit(p, at))
				return false;
			string soundSet = AZWhisperSay(p, false);
			if (soundSet == "")
				soundSet = AZWhisperSet(true);
			AZWhisperPlay(p, at, soundSet, "near a vampire");
			return true;
		}
		return false;
	}


	protected bool AZWhisperLit(PlayerBase p, vector at)
	{
		float reach = AZ_VampRepel.LightReach(p, 14);
		if (reach <= 0)
			return false;
		vector eye = GetGame().GetCurrentCameraPosition();
		vector to = at - eye;
		if (to.Length() > reach)
			return false;
		to.Normalize();
		vector look = GetGame().GetCurrentCameraDirection();
		return vector.Dot(look, to) > 0.88;
	}




	protected string AZWhisperSay(PlayerBase p, bool crypt)
	{
		bool moon = p.m_AZBloodMoon && GetGame().GetWorld().IsNight();
		float chance = 0;
		if (crypt && moon)
			chance = 0.15;
		else if (crypt)
			chance = 0.04;
		else if (moon)
			chance = 0.2;
		if (m_AZSaid >= 2 || m_AZWhisperClock < m_AZSayAt || Math.RandomFloat01() >= chance)
			return "";
		if (!m_AZSaidNames)
			m_AZSaidNames = new array<string>;
		array<string> pool = {"play_1", "play_2", "play_3", "tease_1", "seduce_1", "seduce_2", "seduce_3", "seduce_4"};
		if (moon)
		{
			pool.Insert("moon_1");
			pool.Insert("moon_2");
			pool.Insert("moon_3");
		}
		array<string> left = new array<string>;
		foreach (string n : pool)
		{
			if (m_AZSaidNames.Find(n) < 0)
				left.Insert(n);
		}
		if (left.Count() == 0)
			return "";
		string pick = left.GetRandomElement();
		m_AZSaidNames.Insert(pick);
		m_AZSaid++;
		m_AZSayAt = m_AZWhisperClock + 480;
		return "AZ_VampLine4_" + pick + "_SoundSet";
	}


	protected string AZWhisperSet(bool noWords)
	{
		float r = Math.RandomFloat01();
		if (r < 0.4)
			return "AZ_VampWhisperBreath_SoundSet";
		if (r < 0.65)
			return "AZ_VampWhisperVoice_SoundSet";
		if (r < 0.85 || noWords)
			return "AZ_VampWhisperShhh_SoundSet";
		return "AZ_VampWhisperWord_SoundSet";
	}

	protected void AZWhisperPlay(PlayerBase p, vector at, string soundSet, string where)
	{
		m_AZWhisperAt = at;
		m_AZWhisper = SEffectManager.PlaySound(soundSet, at);
		if (m_AZWhisper)
			m_AZWhisper.SetSoundAutodestroy(true);
		Print("[AZ_Vamp] whisper " + soundSet + " " + where + " " + Math.Round(vector.Distance(at, p.GetPosition())) + " m");
	}



	protected void AZWhisperBuildSpots()
	{
		if (m_AZWhisperSpots)
			return;
		m_AZWhisperSpots = new array<vector>;
		m_AZWhisperBeds = new array<vector>;
		m_AZWhisperBedAt = new array<float>;
		m_AZWhisperBedShhh = new array<bool>;
		if (!AZ_VampCrypt.Site())
			return;


		if (AZ_VampCrypt.EntryOn())
		{

			AZWhisperSpot(-2.5, -0.3, 0.6, false);

			AZWhisperSpot(-19.4, -5.6, 1.6, false);
			AZWhisperSpot(-19.4, 5.6, 1.6, false);
			AZWhisperSpot(-17.6, -5.6, 1.6, false);
			AZWhisperSpot(-17.6, 5.6, 1.6, false);

			AZWhisperSpot(-7.0, -12.65, 0.3, false);
			AZWhisperSpot(-5.5, -12.65, 1.25, false);
			AZWhisperSpot(5.0, -12.65, 1.2, false);
			for (int i = 0; i < 3; i++)
			{
				m_AZWhisperBeds.Insert(m_AZWhisperSpots[m_AZWhisperSpots.Count() - 3 + i]);
				m_AZWhisperBedAt.Insert(0);
				m_AZWhisperBedShhh.Insert(true);
			}
		}

		AZWhisperSpot(AZ_VampCrypt.HALL_F0 + AZ_VampCrypt.BAY * 1.5, -7.3, 1.4, true);
		AZWhisperSpot(AZ_VampCrypt.HALL_F0 + AZ_VampCrypt.BAY * 4.5, -7.3, 1.4, true);
		AZWhisperSpot(AZ_VampCrypt.HALL_F0 + AZ_VampCrypt.BAY * 3.5, 7.3, 1.4, true);
		AZWhisperSpot(AZ_VampCrypt.HALL_F0 + AZ_VampCrypt.BAY * 6.5, 7.3, 1.4, true);
		for (int k = 0; k < 4; k++)
		{
			m_AZWhisperBeds.Insert(m_AZWhisperSpots[m_AZWhisperSpots.Count() - 4 + k]);
			m_AZWhisperBedAt.Insert(0);
			m_AZWhisperBedShhh.Insert(false);
		}

		AZWhisperSpot(AZ_VampCrypt.APSE_F + 8, -4, 2.0, true);
		AZWhisperSpot(AZ_VampCrypt.APSE_F + 8, 4, 2.0, true);
	}

	protected void AZWhisperSpot(float f, float r, float y, bool nave)
	{
		vector v;
		if (nave)
			v = AZ_VampCrypt.Nave(f, r, y);
		else
			v = AZ_VampCrypt.Hall(f, r, y);
		m_AZWhisperSpots.Insert(v);
	}
}


modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();
		AZ_VampWhisperLines.StartStalk();
	}
}
