// AZ-2569: a human knocked out while a vampire feeds on them sees a dim, blurred, desaturated world
// instead of vanilla's full black-out. Client only. Normal unconsciousness stays vanilla.
//
// The PPE classes (cap + blur requester) are in 3_Game/AZ_VampVictimViewPPE.c.
// How: vanilla PlayerBase.OnUnconsciousUpdate re-Starts PPERequester_UnconEffects every frame with a vignette of
// ~2.0 (full black). We cap that vignette while s_Level > 0 (modded OnStart in 3_Game), and run our own requester for
// blur and desaturation. s_Level eases 0..1 and is driven only from MissionGameplay.OnUpdate, so respawn,
// death, disconnect and mission end all fall back to level 0 (= vanilla exactly) with no per-player cleanup.
class AZ_VampVictimView
{
	// ---- tunables ----
	static const float DIM_VIGNETTE = 0.95;		// vanilla is 2.0 (black). Lower = more of the centre of the screen visible. 0.6 bright, 1.2 very dark.
	static const float BLUR = 0.55;				// GaussFilter intensity 0..1. Enough to lose faces and gear text.
	static const float SATURATION = 0.25;		// 1 = normal colour, 0 = grey.
	static const float FADE_IN_SEC = 1.5;		// vanilla black -> dim view
	static const float FADE_OUT_SEC = 1.5;		// dim view -> vanilla (or fully gone on wake / death)

	static const float HEAR = 0.6;				// game sound while dimmed, x the player's volume. Vanilla uncon mutes to 0.

	static float s_Level;
	static bool s_Hearing;						// 0 = vanilla, 1 = full dim view

	// Vignette cap used by the vanilla uncon requester. 2.0 (no cap) when level is 0.
	static float VignetteCap()
	{
		return Math.Lerp(2.0, DIM_VIGNETTE, s_Level);
	}

	static int s_TestUntil;					// "/vamp dimtest": show the dim view while knocked out, with no vampire

	static bool WantDim(PlayerBase p)
	{
		if (!p || !p.IsAlive() || !p.IsUnconscious())
			return false;
		if (GetGame().GetTime() < s_TestUntil)
			return true;
		return p.m_AZVampHeld;	// net-synced bool: true on the victim's own client while a vampire holds / feeds on them
	}

	static void Reset()
	{
		s_Level = 0;
		AZ_VampUnconCap.s_Vignette = 2.0;
		s_Hearing = false;
		AZ_PPEVampDim r = AZ_PPEVampDim.Get();
		if (r && r.IsRequesterRunning())
			r.Stop();
	}

	// The drain pulse and compel trance (4_World/AZ_VampFeed.c) are only stopped by the body that started them,
	// so death or respawn mid-feed left them on screen. Stop whichever no longer matches the current player.
	static void SweepFeedEffects(PlayerBase p)
	{
		bool alive = p && p.IsAlive();
		AZ_PPEVampDrain drain = AZ_PPEVampDrain.Get();
		if (drain && drain.IsRequesterRunning() && !(alive && p.m_AZVampHeld))
			drain.Stop();
		AZ_PPEVampCompel compel = AZ_PPEVampCompel.Get();
		if (compel && compel.IsRequesterRunning() && !(alive && p.m_AZVampCompelled && !p.m_AZVampHeld))
			compel.Stop();
	}

	static void Tick(float dt)
	{
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		SweepFeedEffects(p);
		bool want = WantDim(p);
		if (!p || !p.IsAlive())
		{
			// dead, respawning or no player: drop everything at once
			if (s_Level > 0 || AZ_PPEVampDim.Get().IsRequesterRunning())
				Reset();
			return;
		}
		float before = s_Level;
		if (want)
			s_Level = Math.Clamp(s_Level + dt / Math.Max(0.05, FADE_IN_SEC), 0, 1);
		else
			s_Level = Math.Clamp(s_Level - dt / Math.Max(0.05, FADE_OUT_SEC), 0, 1);

		AZ_PPEVampDim r = AZ_PPEVampDim.Get();
		if (!r)
			return;
		AZ_VampUnconCap.s_Vignette = VignetteCap();
		// Vanilla mutes all game sound on uncon start and restores it on wake. Bring it back while dimmed,
		// and mute again only if the dim view ends while still knocked out (on wake vanilla already restored it).
		if (want && !s_Hearing)
		{
			GetGame().GetSoundScene().SetSoundVolume(g_Game.m_volume_sound * HEAR, 1.5);
			s_Hearing = true;
		}
		else if (!want && s_Hearing)
		{
			if (p.IsUnconscious())
				GetGame().GetSoundScene().SetSoundVolume(0, 1.5);
			s_Hearing = false;
		}
		if (s_Level > 0)
			r.SetLevel(BLUR * s_Level, 1 - (1 - SATURATION) * s_Level);
		else if (before > 0 || r.IsRequesterRunning())
			r.Stop();
	}
}


modded class MissionGameplay
{
	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		AZ_VampVictimView.Tick(timeslice);
	}

	override void OnMissionFinish()
	{
		AZ_VampVictimView.Reset();
		super.OnMissionFinish();
	}
}
