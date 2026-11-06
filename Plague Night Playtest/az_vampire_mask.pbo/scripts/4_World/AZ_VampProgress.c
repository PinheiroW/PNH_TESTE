







class AZ_VampProgress
{
	static const int FEEDS_STEP1 = 3;
	static const int FEEDS_STEP2 = 8;
	static const float FEED_MIN_ML = 300;
	static const float CALM_STEP1 = 3600;
	static const float CALM_STEP2 = 9000;
	static const int TICK_EVERY = 15;

	static int s_Ticks;
	static bool s_NightKnown;
	static bool s_WasNight;
	static int s_DawnMin = 360;
	static int s_DawnStage;
	static int s_NightNo;
	static int s_DuskEpoch;
	static int s_DuskGoalSentEpoch = -1;
	static int s_DuskAt;
	static int s_NightFeeds;
	static int s_NightTurned;
	static int s_NightCured;
	static int s_NightAsh;
	static bool s_BloodMoon;
	static bool s_ReplayBloodMoon;
	static bool s_CalendarNightPending;
	static bool s_CalendarDawnLogged;

	static bool s_CalendarPrepared;
	static bool s_CalendarPreparedBlood;
	static bool s_CalendarPreparedSafe;
	static int s_CalendarY;
	static int s_CalendarMo;
	static int s_CalendarD;
	// Day/night edge guard (31 Oct dawn-spam fix). A real dusk and dawn are
	// many real minutes apart; two edges within EDGE_DEBOUNCE_MS are a flap
	// (calendar date move, seconds reset by SetDate) and are ignored.
	static const int EDGE_DEBOUNCE_MS = 180000;
	static int s_EdgeAtMs = -1;
	static bool s_EdgeFlapLogged;
	// Dawn() runs at most once per Dusk().
	static bool s_DawnDone;


	static void OnFeed(PlayerBase vamp, float ml)
	{
		if (!vamp || ml < FEED_MIN_ML)
			return;
		SyncGoalBoundary();
		s_NightFeeds++;
		AZ_VampState.s_Data.nightFeeds = s_NightFeeds;
		AZ_VampRecord r = Rec(vamp);
		if (r)
		{
			r.feeds++;
			r.feedsTotal++;
		}
		RefreshGoal(true);
		Update(vamp);
	}




	static void OnVampKill(PlayerBase slayer)
	{
		AZ_VampRecord r = Rec(slayer);
		if (!r || !slayer.m_AZVampire)
			return;
		int to = FEEDS_STEP1;
		if (r.feeds >= FEEDS_STEP1)
			to = FEEDS_STEP2;
		if (r.feeds < to - 1)
			r.feeds = to - 1;
		Print("[AZ_Vamp] vampire kill " + AZ_VampState.Uid(slayer) + " feeds now " + r.feeds + " (+1 follows)");
		slayer.Message("One of your own, ended by your hand. Its hunger is yours now.", "colorImportant");
		OnFeed(slayer, FEED_MIN_ML);
	}

	static void OnTurned()
	{
		s_NightTurned++;
	}

	static void OnCured(PlayerBase giver)
	{
		SyncGoalBoundary();
		s_NightCured++;
		AZ_VampState.s_Data.nightCured = s_NightCured;
		AZ_VampRecord r = Rec(giver);
		if (r)
			r.cures++;
		RefreshGoal(false);
	}

	static void OnAsh()
	{
		s_NightAsh++;
	}


	static void OnSlain(PlayerBase hunter)
	{
		AZ_VampRecord r = Rec(hunter);
		if (!r)
			return;
		r.slain++;
		AZ_VampState.Save();
	}


	static AZ_VampRecord Rec(PlayerBase p)
	{
		string uid = AZ_VampState.Uid(p);
		if (uid == "" || AZ_VampState.IsDummy(uid))
			return null;
		AZ_VampRecord r = AZ_VampState.Find(uid);
		if (!r)
		{
			r = new AZ_VampRecord;
			r.uid = uid;
			if (p.GetIdentity())
				r.name = p.GetIdentity().GetName();
			AZ_VampState.s_Data.players.Insert(r);
		}
		return r;
	}

	static float HumanSeconds(PlayerBase p, AZ_VampRecord r)
	{
		float t = p.StatGet(AnalyticsManagerServer.STAT_PLAYTIME);
		if (t < r.humanBase)
		{
			r.humanBase = 0;
			AZ_VampState.Save();
		}
		return t - r.humanBase;
	}

	static void Update(PlayerBase p)
	{
		if (!p || !p.IsAlive())
			return;
		AZ_VampRecord r = Rec(p);
		if (!r)
			return;
		int hear = 0;
		int calm = 0;
		if (p.m_AZVampire)
		{
			if (r.feeds >= FEEDS_STEP2)
				hear = 2;
			else if (r.feeds >= FEEDS_STEP1)
				hear = 1;
		}
		else
		{
			float t = HumanSeconds(p, r);
			if (t >= CALM_STEP2)
				calm = 2;
			else if (t >= CALM_STEP1)
				calm = 1;
		}
		if (hear > p.m_AZHearStep)
			p.Message("Your hunger sharpens. You hear them from further away.", "colorImportant");
		if (calm > p.m_AZCalmStep)
			p.Message("Your heart is steadier. They will have to come closer to hear you.", "colorFriendly");
		if (hear != p.m_AZHearStep || calm != p.m_AZCalmStep)
		{
			p.m_AZHearStep = hear;
			p.m_AZCalmStep = calm;
			p.SetSynchDirty();
		}
	}


	static void Tick(array<PlayerBase> online)
	{
		if (!AZ_VampEvent.Active())
		{
			CancelDuskMessages();
			bool inactiveNight = GetGame().GetWorld().IsNight();
			bool staleGoal = AZ_VampState.s_Data.bloodMoonGoalEarnedTonight || AZ_VampState.s_Data.nightFeeds != 0 || AZ_VampState.s_Data.nightCured != 0;
			if (GoalResetInactiveDay(inactiveNight, staleGoal, s_NightFeeds, s_NightCured))
			{
				if (AZ_VampState.s_Data.bloodMoonGoalEarnedTonight)
					AZ_VampState.s_Data.nightsSinceBloodMoon = 0;
				AZ_VampState.s_Data.bloodMoonGoalEarnedTonight = false;
				AZ_VampState.s_Data.nightFeeds = 0;
				AZ_VampState.s_Data.nightCured = 0;
				s_NightFeeds = 0;
				s_NightCured = 0;
				AZ_VampState.Save();
				AZ_VampAudit.Record("moon", "INACTIVE_DAY_GOAL_RESET");
			}
			if (inactiveNight && DayMin() < 720)
				s_CalendarPrepared = false;
			PrepareCalendar(false);
			s_NightKnown = false;
			s_BloodMoon = false;
			s_CalendarNightPending = false;
			s_CalendarDawnLogged = false;
			SyncMoon(online);
			SyncEventOn(online, false);
			return;
		}
		SyncEventOn(online, true);
		bool night = GetGame().GetWorld().IsNight();
		if (!s_NightKnown)
		{
			s_CalendarNightPending = false;
			s_CalendarDawnLogged = false;
		}


		bool calendarTwilight = s_CalendarNightPending && s_NightKnown && s_WasNight && !night && DayMin() >= 720;
		if (s_CalendarNightPending && (night || DayMin() < 720))
		{
			if (s_CalendarDawnLogged)
				AZ_VampAudit.Record("moon", "CALENDAR_NIGHT_RESUMED");
			s_CalendarNightPending = false;
			s_CalendarDawnLogged = false;
		}
		if (calendarTwilight && !s_CalendarDawnLogged)
		{
			s_CalendarDawnLogged = true;
			AZ_VampAudit.Record("moon", "CALENDAR_TWILIGHT_NOT_DAWN", "clockMin=" + DayMin());
		}
		if (!s_NightKnown)
		{
			s_NightKnown = true;
			s_WasNight = night;
			s_DawnStage = 0;
			s_NightNo = AZ_VampState.s_Data.nightIndex;
			TrackNightVampires();
			string restored = "night=" + night + " moonTonight=" + AZ_VampState.s_Data.bloodMoonTonight + " moonNext=" + AZ_VampState.s_Data.bloodMoonNext;
			restored += " feeds=" + AZ_VampState.s_Data.nightFeeds + " cures=" + AZ_VampState.s_Data.nightCured;
			restored += " moonNeed=" + GoalNeed() + " moonGap=" + AZ_VampState.Cfg().bloodMoonGapNights + " rest=" + AZ_VampState.s_Data.nightsSinceBloodMoon;
			restored += " seedTarget=" + AZ_VampState.Cfg().elderSeedTarget + " elderEnabled=" + AZ_VampState.Cfg().elderFallback;
			AZ_VampAudit.Record("bootstrap", "RESTORE", restored);

			if (night)
			{
				s_NightFeeds = AZ_VampState.s_Data.nightFeeds;
				s_NightCured = AZ_VampState.s_Data.nightCured;
			}
			if (night && (AZ_VampState.s_Data.bloodMoonTonight || AZ_VampState.Cfg().bloodMoonAlways) && !s_BloodMoon)
			{
				s_BloodMoon = true;
				AZ_VampState.s_Data.bloodMoonTonight = true;
				AZ_VampState.Save();
				CalendarNightFallback(true, "RESTORE");
				SyncMoon(online);
				Print("[AZ_Vamp] blood moon restored after a restart");
				AZ_VampAudit.Record("moon", "RESTORED_ACTIVE");
			}


			if (!night && AZ_VampState.s_Data.bloodMoonTonight)
			{
				s_ReplayBloodMoon = true;
				Print("[AZ_Vamp] blood moon: last night's was cut short by a restart, replayed at the next dusk");
				AZ_VampAudit.Record("moon", "REPLAY_NEXT_DUSK");
			}
			if (night && !s_BloodMoon)
				CalendarNightFallback(false, "BOOTSTRAP_ORDINARY");
			if (night)
				AZ_VampAltar.BeginNight("bootstrap");
			AZ_VampCryptExit.Restore();
		}
		else if (night != s_WasNight && !calendarTwilight)
		{
			int edgeNow = GetGame().GetTime();
			if (s_EdgeAtMs >= 0 && edgeNow - s_EdgeAtMs < EDGE_DEBOUNCE_MS)
			{
				// Flap: keep s_WasNight so the way back makes no edge either.
				if (!s_EdgeFlapLogged)
				{
					s_EdgeFlapLogged = true;
					AZ_VampAudit.Record("moon", "DAY_NIGHT_FLAP_IGNORED", "night=" + night + " sinceEdgeMs=" + (edgeNow - s_EdgeAtMs) + " clock=" + AZ_VampAudit.MoonClock());
				}
			}
			else
			{
				s_EdgeAtMs = edgeNow;
				s_EdgeFlapLogged = false;
				s_WasNight = night;
				if (night)
				{
					s_DawnStage = 0;
					s_DawnDone = false;
					Dusk(online);
				}
				else if (s_DawnDone)
					AZ_VampAudit.Record("moon", "DAWN_ALREADY_DONE", "clock=" + AZ_VampAudit.MoonClock());
				else
				{
					s_DawnMin = DayMin();
					s_DawnDone = true;
					Dawn(online);
				}
			}
		}

		if (!night && !calendarTwilight)
			PrepareCalendar(WantsFullMoonCalendar());
		if (night)
			DawnDread(online);
		SyncMoon(online);
		s_Ticks++;
		int cfgBits = AZ_VampCrypt.CfgBitsNow();
		foreach (PlayerBase pc : online)
		{
			if (pc.m_AZCfgBits != cfgBits)
			{
				pc.m_AZCfgBits = cfgBits;
				pc.SetSynchDirty();
			}
		}
		if (s_Ticks % TICK_EVERY != 0)
			return;
		bool hw = AZ_VampState.Cfg().halloween;
		foreach (PlayerBase p : online)
		{
			Update(p);
			if (p.m_AZHalloween != hw)
			{
				p.m_AZHalloween = hw;
				p.SetSynchDirty();
			}
		}
	}

	static int DayMin()
	{
		int y, mo, d, h, mi;
		GetGame().GetWorld().GetDate(y, mo, d, h, mi);
		return h * 60 + mi;
	}



	static void DawnDread(array<PlayerBase> online)
	{
		int warn = AZ_VampState.Cfg().dawnWarnMin;
		if (warn <= 0 || s_DawnStage >= 2)
			return;
		int left = (s_DawnMin - DayMin() + 1440) % 1440;
		if (left > warn)
			return;
		string line;
		if (s_DawnStage == 0 && left > warn / 4)
		{
			s_DawnStage = 1;
			line = "The sky to the east is turning grey. Find a roof before the sun finds you.";
		}
		else if (left <= warn / 4)
		{
			s_DawnStage = 2;
			line = "Your skin prickles. Dawn is minutes away. Find a roof.";
		}
		else
			return;
		foreach (PlayerBase p : online)
		{
			if (p && p.IsAlive() && p.m_AZVampire)
				p.Message(line, "colorImportant");
		}
		Print("[AZ_Vamp] dawn warning " + s_DawnStage + " (" + left + " game min left)");
	}

	static void SyncEventOn(array<PlayerBase> online, bool on)
	{
		foreach (PlayerBase p : online)
		{
			if (p.m_AZEventOn != on)
			{
				p.m_AZEventOn = on;
				p.SetSynchDirty();
			}
		}
	}

	static bool GoalBoundaryDue(bool known, bool previousNight, bool physicalNight, bool pending, int minute)
	{
		if (known && previousNight && !physicalNight && pending && minute >= 720)
		{
			return false;
		}
		return !known || previousNight != physicalNight;
	}

	static void SyncGoalBoundary()
	{
		if (!AZ_VampEvent.Active())
			return;
		if (!GoalBoundaryDue(s_NightKnown, s_WasNight, GetGame().GetWorld().IsNight(), s_CalendarNightPending, DayMin()))
			return;


		array<PlayerBase> online;
		AZ_VampState.OnlinePlayers(online);
		Tick(online);
	}

	static bool GoalResetInactiveDay(bool night, bool stale, int feeds, int cures)
	{
		return !night && (stale || feeds != 0 || cures != 0);
	}

	static bool GoalMayEarn(bool immediate, bool eventOn, bool night, bool active, int need, int rest, int gap, bool alreadyEarned)
	{
		return immediate && eventOn && night && !active && !alreadyEarned && need > 0 && rest >= gap;
	}

	static int GoalNetScore(int feeds, int cures)
	{
		int score = feeds - 2 * cures;
		if (score < 0)
		{
			return 0;
		}
		return score;
	}

	static float GoalRatio(bool eligible, bool active, int need, int score)
	{
		if (active)
		{
			return 1.0;
		}
		if (!eligible || need <= 0 || score <= 0)
		{
			return 0.0;
		}
		if (score >= need)
		{
			return 1.0;
		}
		return score * 1.0 / need;
	}

	static bool GoalReached(bool eligible, bool active, int need, int score)
	{
		return eligible && !active && need > 0 && score >= need;
	}

	static bool GoalQueueEarned(bool immediate, string decision)
	{
		return !immediate && decision == "EARNED";
	}

	static int GoalRevision(int revision, bool active, bool alreadyActive)
	{
		if (!active || (alreadyActive && revision > 0))
		{
			return revision;
		}
		if (revision >= 2147483646 || revision < 0)
		{
			return 1;
		}
		return revision + 1;
	}

	static void TrackNightVampires()
	{
		if (!AZ_VampState.s_Data || !AZ_VampEvent.Active() || !GetGame().GetWorld().IsNight())
			return;
		int vampires = AZ_VampState.CountOnlineVampires();
		if (vampires <= AZ_VampState.s_Data.nightVampiresPeak)
			return;
		AZ_VampState.s_Data.nightVampiresPeak = vampires;
		AZ_VampState.Save();
	}

	static int GoalNeed()
	{
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		if (cfg.bloodMoonScaleOn)
		{
			int peak = 0;
			if (AZ_VampState.s_Data)
				peak = AZ_VampState.s_Data.nightVampiresPeak;
			return Math.Clamp(Math.Round(cfg.bloodMoonFeedsPerVampire * peak), cfg.bloodMoonFeedsMin, cfg.bloodMoonFeedsMax);
		}
		int need = cfg.bloodMoonFeeds;
		if (need < 0)
			return 0;
		return need;
	}

	static int GoalScore()
	{
		return GoalNetScore(s_NightFeeds, s_NightCured);
	}

	static bool GoalEligible()
	{
		if (!AZ_VampState.s_Data || AZ_VampState.s_Data.bloodMoonNext)
			return false;
		return GoalMayEarn(AZ_VampState.Cfg().bloodMoonImmediate, AZ_VampEvent.Active(), GetGame().GetWorld().IsNight(), s_BloodMoon, GoalNeed(), AZ_VampState.s_Data.nightsSinceBloodMoon, AZ_VampState.Cfg().bloodMoonGapNights, AZ_VampState.s_Data.bloodMoonGoalEarnedTonight);
	}

	static float GoalProgress()
	{
		return GoalRatio(GoalEligible(), s_BloodMoon, GoalNeed(), GoalScore());
	}

	static bool DelayEarnedBloodMoon()
	{
		int now = DayMin();
		int left = (s_DawnMin - now + 1440) % 1440;
		return !GetGame().GetWorld().IsNight() || (now < 720 && now >= s_DawnMin) || left <= 90;
	}

	static void RefreshGoal(bool fromFeed)
	{
		// A feed may arrive between the regular population samples.
		TrackNightVampires();
		bool activated = false;
		bool skySnapped = false;
		if (fromFeed && GoalReached(GoalEligible(), s_BloodMoon, GoalNeed(), GoalScore()))
		{
			AZ_VampState.s_Data.bloodMoonGoalEarnedTonight = true;
			if (AZ_VampState.Cfg().bloodMoonDawnDelay && DelayEarnedBloodMoon())
			{
				// Persist the same next-dusk queue used by non-immediate earning at dawn.
				AZ_VampState.s_Data.bloodMoonNext = true;
				AZ_VampState.Save();
				AZ_VampAudit.Record("moon", "GOAL_DELAYED_NEXT_DUSK");
				if (AZ_VampState.Cfg().broadcasts)
					AZ_VampTurn.Announce("The blood moon will rise at next dusk.");
			}
			else
			{
				s_BloodMoon = true;
				AZ_VampState.s_Data.bloodMoonTonight = true;
				activated = true;
				if (!AZ_VampState.Cfg().bloodMoonDawnDelay && DelayEarnedBloodMoon())
					skySnapped = FullMoonAtClock("EARNED_DELAY_OFF");
				else
					skySnapped = SnapBloodMoonSky();
			}
		}
		array<PlayerBase> online;
		AZ_VampState.OnlinePlayers(online);
		SyncMoon(online);
		AZ_VampState.Save();
		if (activated)
		{
			AZ_VampAudit.Record("moon", "GOAL_REACHED", "score=" + GoalScore() + " need=" + GoalNeed() + " revision=" + AZ_VampState.s_Data.bloodMoonRevision + " skySnapped=" + skySnapped);
			if (AZ_VampState.Cfg().broadcasts)
				AZ_VampTurn.Announce("The blood moon rises.");
		}
		if (AZ_VampCryptA1.Ready())
			AZ_VampCryptA1.Refresh();
	}

	static bool s_MoonSyncKnown;
	static bool s_MoonSyncState;
	static int s_MoonSyncOnline;
	static void SyncMoon(array<PlayerBase> online)
	{
		TrackNightVampires();
		int revision = GoalRevision(AZ_VampState.s_Data.bloodMoonRevision, s_BloodMoon, AZ_VampState.s_Data.bloodMoonRevisionActive);
		if (revision != AZ_VampState.s_Data.bloodMoonRevision || AZ_VampState.s_Data.bloodMoonRevisionActive != s_BloodMoon)
		{
			AZ_VampState.s_Data.bloodMoonRevision = revision;
			AZ_VampState.s_Data.bloodMoonRevisionActive = s_BloodMoon;
			AZ_VampState.Save();
		}
		bool eligible = GoalEligible();
		int progress = Math.Round(GoalProgress() * 1000);
		AZ_VampAudit.WatchMoonDay(s_BloodMoon);
		int queued = 0;
		foreach (PlayerBase p : online)
		{
			if (p.m_AZBloodMoon != s_BloodMoon || p.m_AZMoonProgress != progress || p.m_AZMoonEligible != eligible || p.m_AZMoonRevision != revision)
			{
				p.m_AZBloodMoon = s_BloodMoon;
				p.m_AZMoonProgress = progress;
				p.m_AZMoonEligible = eligible;
				p.m_AZMoonRevision = revision;
				p.SetSynchDirty();
				queued++;
			}
		}

		if (!s_MoonSyncKnown || s_MoonSyncState != s_BloodMoon || s_MoonSyncOnline != online.Count() || queued > 0)
			AZ_VampAudit.Record("moon", "SYNC_QUEUED", "active=" + s_BloodMoon + " progress=" + progress + " eligible=" + eligible + " revision=" + revision + " online=" + online.Count() + " changed=" + queued);
		s_MoonSyncKnown = true;
		s_MoonSyncState = s_BloodMoon;
		s_MoonSyncOnline = online.Count();
	}



	static int CalendarMonthDays(int y, int mo)
	{
		if (mo == 2)
		{
			if (y % 400 == 0 || (y % 4 == 0 && y % 100 != 0))
				return 29;
			return 28;
		}
		if (mo == 4 || mo == 6 || mo == 9 || mo == 11)
			return 30;
		return 31;
	}

	static bool BloodMoonSkyUp(World w)
	{
		// Vanilla Environment casts towards the light using the NEGATIVE direction.
		vector direction = w.GetSunOrMoonDirection();
		return w.IsNight() && w.GetSunOrMoon() < 0.5 && direction[1] < -0.1;
	}

	static bool SnapBloodMoonSky()
	{
		World w = GetGame().GetWorld();
		int y, mo, d, h, mi;
		w.GetDate(y, mo, d, h, mi);
		int now = h * 60 + mi;
		bool nightBefore = w.IsNight();
		string beforeClock = AZ_VampAudit.MoonClock();
		int left = (s_DawnMin - now + 1440) % 1440;
		// Near dawn, move the clock once at activation instead of leaving an ordinary moon.
		bool moveClock = !w.IsNight() || (now < 720 && now >= s_DawnMin) || left <= 90;
		// Try the nearest full date at the existing h:mm before moving the clock.
		int targetY = y;
		int targetMo = mo;
		int targetD = EngineFullMoonDay(w, y, mo, h, mi);
		int distance = Math.AbsInt(targetD - d);
		int prevY = y;
		int prevMo = mo - 1;
		if (prevMo < 1)
		{
			prevMo = 12;
			prevY = y - 1;
		}
		int prevD = EngineFullMoonDay(w, prevY, prevMo, h, mi);
		int prevDistance = d + CalendarMonthDays(prevY, prevMo) - prevD;
		if (prevDistance < distance)
		{
			targetY = prevY;
			targetMo = prevMo;
			targetD = prevD;
			distance = prevDistance;
		}
		int nextY = y;
		int nextMo = mo + 1;
		if (nextMo > 12)
		{
			nextMo = 1;
			nextY = y + 1;
		}
		int nextD = EngineFullMoonDay(w, nextY, nextMo, h, mi);
		int nextDistance = CalendarMonthDays(y, mo) - d + nextD;
		if (nextDistance < distance)
		{
			targetY = nextY;
			targetMo = nextMo;
			targetD = nextD;
		}
		w.SetDate(targetY, targetMo, targetD, h, mi);
		// A full phase alone does not guarantee the moon has risen.
		if (moveClock || !BloodMoonSkyUp(w))
		{
			bool found = false;
			for (int probe = 0; probe <= 23; probe++)
			{
				int probeHour = (23 + probe) % 24;
				targetD = EngineFullMoonDay(w, targetY, targetMo, probeHour, 30);
				w.SetDate(targetY, targetMo, targetD, probeHour, 30);
				if (BloodMoonSkyUp(w))
				{
					found = true;
					break;
				}
			}
			if (!found)
			{
				RestoreDateKeepNight(w, y, mo, d, h, mi, nightBefore, "SNAP");
				AZ_VampAudit.Record("moon", "CALENDAR_NO_MOON_UP");
				return false;
			}
		}
		// Do not turn a manual daytime activation into another Dusk on the next tick.
		if (s_NightKnown)
			s_WasNight = true;
		s_CalendarPrepared = false;
		Print("[AZ_Vamp] bloodmoon sky snap from=" + beforeClock + " to=" + AZ_VampAudit.MoonClock());
		return true;
	}

	// Put the calendar back without flipping IsNight. SetDate resets the
	// seconds to :00 and sunset differs per date, so a plain restore at the
	// dusk minute can land in daylight (EU 31 Oct: 11/30 16:10 night ->
	// 11/5 16:10 day -> Dawn -> Dusk, 47 times). Order: original date, the
	// original date one minute later, else keep the date we are on.
	static bool RestoreDateKeepNight(World w, int y, int mo, int d, int h, int mi, bool wantNight, string context)
	{
		int cy, cm, cd, ch, cmi;
		w.GetDate(cy, cm, cd, ch, cmi);
		bool curNight = w.IsNight();
		w.SetDate(y, mo, d, h, mi);
		if (w.IsNight() == wantNight)
			return true;
		int h2 = h;
		int mi2 = mi + 1;
		if (mi2 >= 60)
		{
			mi2 = 0;
			h2 = (h + 1) % 24;
		}
		w.SetDate(y, mo, d, h2, mi2);
		if (w.IsNight() == wantNight)
		{
			AZ_VampAudit.Record("moon", "CALENDAR_RESTORE_NUDGED", "context=" + context + " wantNight=" + wantNight + " clock=" + AZ_VampAudit.MoonClock());
			return true;
		}
		if (curNight == wantNight)
		{
			w.SetDate(cy, cm, cd, ch, cmi);
			AZ_VampAudit.Record("moon", "CALENDAR_RESTORE_KEPT_PROBED", "context=" + context + " wantNight=" + wantNight + " clock=" + AZ_VampAudit.MoonClock());
			return true;
		}
		AZ_VampAudit.Record("moon", "CALENDAR_RESTORE_NIGHT_MISMATCH", "context=" + context + " wantNight=" + wantNight + " clock=" + AZ_VampAudit.MoonClock());
		return false;
	}

	// Dusk must keep its h:mm: test the full date and its adjacent days.
	static bool FullMoonAtClock(string context)
	{
		World w = GetGame().GetWorld();
		int y, mo, d, h, mi;
		w.GetDate(y, mo, d, h, mi);
		bool nightBefore = w.IsNight();
		int full = EngineFullMoonDay(w, y, mo, h, mi);
		array<int> offsets = {0, -1, 1};
		foreach (int offset : offsets)
		{
			int cy = y;
			int cm = mo;
			int cd = full + offset;
			if (cd < 1)
			{
				cm = cm - 1;
				if (cm < 1)
				{
					cm = 12;
					cy = cy - 1;
				}
				cd = CalendarMonthDays(cy, cm);
			}
			else if (cd > CalendarMonthDays(cy, cm))
			{
				cd = 1;
				cm = cm + 1;
				if (cm > 12)
				{
					cm = 1;
					cy = cy + 1;
				}
			}
			w.SetDate(cy, cm, cd, h, mi);
			if (BloodMoonSkyUp(w))
			{
				s_CalendarPrepared = false;
				AZ_VampAudit.Record("moon", "FULL_DATE_FIXED_CLOCK", "context=" + context + " adjacent=" + offset + " clock=" + h + ":" + mi);
				return true;
			}
		}
		// Some world light values settle on the following frame after SetDate.
		w.SetDate(y, mo, full, h, mi);
		if (w.IsNight() != nightBefore)
		{
			// The full date is on the other side of dusk at this h:mm: no deferred probing.
			RestoreDateKeepNight(w, y, mo, d, h, mi, nightBefore, context + "_FULL_DAYSIDE");
			AZ_VampAudit.Record("moon", "CALENDAR_NO_MOON_UP", "context=" + context + " eventKept=1 clockKept=1 fullDaySide=1");
			return false;
		}
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(CheckFullMoonAtClock, 250, false, y, mo, d, h, mi, full, 0, context);
		return false;
	}

	static void CheckFullMoonAtClock(int y, int mo, int d, int h, int mi, int full, int choice, string context)
	{
		World w = GetGame().GetWorld();
		int cy, cm, cd, ch, cmi;
		w.GetDate(cy, cm, cd, ch, cmi);
		// An admin/date change supersedes this bounded verification.
		if (!s_BloodMoon || ch != h || cmi != mi)
			return;
		if (BloodMoonSkyUp(w))
		{
			s_CalendarPrepared = false;
			AZ_VampAudit.Record("moon", "FULL_DATE_FIXED_CLOCK", "context=" + context + " settledChoice=" + choice + " clockKept=1");
			return;
		}
		bool nightNow = w.IsNight();
		if (choice >= 2)
		{
			RestoreDateKeepNight(w, y, mo, d, h, mi, nightNow, context);
			AZ_VampAudit.Record("moon", "CALENDAR_NO_MOON_UP", "context=" + context + " eventKept=1 clockKept=1");
			return;
		}
		int next = full - 1;
		if (choice == 1)
			next = full + 1;
		int ny = y;
		int nm = mo;
		if (next < 1)
		{
			nm = nm - 1;
			if (nm < 1)
			{
				nm = 12;
				ny = ny - 1;
			}
			next = CalendarMonthDays(ny, nm);
		}
		else if (next > CalendarMonthDays(ny, nm))
		{
			next = 1;
			nm = nm + 1;
			if (nm > 12)
			{
				nm = 1;
				ny = ny + 1;
			}
		}
		w.SetDate(ny, nm, next, h, mi);
		if (w.IsNight() != nightNow)
		{
			// Adjacent day is on the day side at this h:mm: stop here, never leave a day date behind.
			RestoreDateKeepNight(w, y, mo, d, h, mi, nightNow, context + "_ADJ_DAYSIDE");
			AZ_VampAudit.Record("moon", "CALENDAR_NO_MOON_UP", "context=" + context + " eventKept=1 clockKept=1 adjDaySide=" + choice);
			return;
		}
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(CheckFullMoonAtClock, 250, false, y, mo, d, h, mi, full, choice + 1, context);
	}

	static int EngineFullMoonDay(World w, int y, int mo, int hour = 0, int minute = 0)
	{
		int best = 1;
		float bestI = -1;
		for (int day = 1; day <= CalendarMonthDays(y, mo); day++)
		{
			w.SetDate(y, mo, day, hour, minute);
			float i = w.GetMoonIntensity();
			if (i > bestI)
			{
				bestI = i;
				best = day;
			}
		}
		return best;
	}



	static void FullMoonDate()
	{
		SnapBloodMoonSky();
	}


	static void AvoidFullMoon()
	{
		World w = GetGame().GetWorld();
		int y, mo, d, h, mi;
		w.GetDate(y, mo, d, h, mi);
		bool nightBefore = w.IsNight();
		string beforeClock = AZ_VampAudit.MoonClock();
		int full = EngineFullMoonDay(w, y, mo);
		int nd = d;
		if (Math.AbsInt(d - full) <= 4)
		{
			nd = full + 9;
			if (nd > 28)
				nd = full - 9;
			if (nd < 1)
				nd = 1;
		}
		PreserveDuskAcrossCalendar(nightBefore, h, d, nd);
		w.SetDate(y, mo, nd, h, mi);
		AZ_VampAudit.Record("moon", "CALENDAR_ORDINARY", "before={" + beforeClock + "} after={" + AZ_VampAudit.MoonClock() + "}");
		if (nd != d)
			Print("[AZ_Vamp] ordinary night near the full moon (day " + full + "): day " + d + " -> " + nd);
	}

	static bool WantsBloodMoon()
	{
		return AZ_VampState.s_Data.bloodMoonNext || RealHalloween() || s_ReplayBloodMoon;
	}

	static bool WantsFullMoonCalendar()
	{
		return WantsBloodMoon();
	}


	static string CalendarPreparationReason(bool night, bool prepared, bool planned, bool wanted, bool manualActive)
	{
		if (night)
		{
			return "NIGHT";
		}
		if (manualActive)
		{
			return "MANUAL_ACTIVE";
		}
		if (prepared && planned == wanted)
		{
			return "UNCHANGED";
		}
		return "PREPARE";
	}

	static void PrepareCalendar(bool blood)
	{
		World w = GetGame().GetWorld();
		int y, mo, d, h, mi;
		w.GetDate(y, mo, d, h, mi);


		if (h < 12 || h >= 14)
			return;

		if (s_CalendarPrepared && (y != s_CalendarY || mo != s_CalendarMo || d != s_CalendarD))
			s_CalendarPrepared = false;


		if (CalendarPreparationReason(w.IsNight(), s_CalendarPrepared, s_CalendarPreparedBlood, blood, s_BloodMoon) != "PREPARE")
			return;
		s_CalendarPrepared = true;
		s_CalendarPreparedBlood = blood;
		s_CalendarPreparedSafe = false;
		string beforeClock = AZ_VampAudit.MoonClock();
		int full = EngineFullMoonDay(w, y, mo);
		int target = full;
		if (!blood)
		{
			target = d;
			if (Math.AbsInt(d - full) <= 4)
			{
				target = full + 9;
				if (target > 28)
					target = full - 9;
				if (target < 1)
					target = 1;
			}
		}
		w.SetDate(y, mo, target, h, mi);
		string reason = "PREPARE_DAYLIGHT";

		if (w.IsNight())
		{
			w.SetDate(y, mo, d, h, mi);
			reason = "PREPARE_UNSAFE_SUNSET";
		}
		else
		{
			s_CalendarPreparedSafe = true;
		}
		w.GetDate(s_CalendarY, s_CalendarMo, s_CalendarD, h, mi);
		AZ_VampAudit.Record("moon", reason, "blood=" + blood + " targetDay=" + target + " before={" + beforeClock + "} after={" + AZ_VampAudit.MoonClock() + "}");
	}

	static void CalendarNightFallback(bool blood, string context)
	{
		if (blood)
		{
			if (context == "DUSK")
				FullMoonAtClock(context);
			else
				SnapBloodMoonSky();
			return;
		}
		World w = GetGame().GetWorld();
		int y, mo, d, h, mi;
		w.GetDate(y, mo, d, h, mi);
		bool full = blood || WantsFullMoonCalendar();
		bool matched = s_CalendarPrepared && s_CalendarPreparedSafe && s_CalendarPreparedBlood == full;
		matched = matched && y == s_CalendarY && mo == s_CalendarMo && d == s_CalendarD;
		if (matched)
			AZ_VampAudit.Record("moon", "PREPARED_NIGHT", "context=" + context + " blood=" + blood);
		else
			AZ_VampAudit.Record("moon", "CALENDAR_NIGHT_FALLBACK", "context=" + context + " blood=" + blood + " calendarKept=1 lunarVisualMayDiffer=1");



	}

	static void PreserveDuskAcrossCalendar(bool nightBefore, int hour, int oldDay, int newDay)
	{
		if (nightBefore && hour >= 12 && oldDay != newDay)
		{
			s_CalendarNightPending = true;
			s_CalendarDawnLogged = false;
		}
	}

	static bool RealHalloween()
	{
		// Forced moons share the seasonal calendar and dusk activation path.
		if (AZ_VampState.Cfg().bloodMoonAlways)
			return true;
		int y, mo, d;
		GetYearMonthDay(y, mo, d);
		return mo == 10 && d == 31 && AZ_VampState.Cfg().halloween;
	}


	static int MoonScore()
	{
		return s_NightFeeds - 2 * s_NightCured;
	}

	// Every night owns its callbacks. Remove pending work and invalidate any
	// already-dispatched callback when dawn, event stop, or another dusk wins.
	static void CancelDuskMessages()
	{
		s_DuskEpoch++;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(DuskNight);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(DuskGoal);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(DuskHumans);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(DuskAltar);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(DuskShadows);
	}

	static bool DuskMessageValid(int epoch)
	{
		return epoch == s_DuskEpoch && AZ_VampEvent.Active() && GetGame().GetWorld() && GetGame().GetWorld().IsNight();
	}

	static void DuskMessageLog(string stage, int epoch)
	{
		Print("[AZ_VampDuskMessage] stage=" + stage + " epoch=" + epoch + " elapsedMs=" + (GetGame().GetTime() - s_DuskAt) + " upMs=" + GetGame().GetTime());
	}

	static void DuskGoal(int epoch)
	{
		if (!DuskMessageValid(epoch) || s_DuskGoalSentEpoch == epoch || !AZ_VampState.Cfg().broadcasts || s_BloodMoon || !GoalEligible()) return;
		s_DuskGoalSentEpoch = epoch;
		DuskMessageLog("goal", epoch);
		AZ_VampTurn.Announce("Blood moon: reach " + GoalNeed() + " points together tonight. Feeds of 300 ml+ add 1, cures remove 2. Resets at dawn.");
	}

	// Seed/turn teaching can fire at dusk itself. Give that text its own second.
	static void DuskNight(int epoch)
	{
		if (DuskMessageValid(epoch) && AZ_VampState.Cfg().broadcasts)
		{
			int v = AZ_VampState.CountOnlineVampires();
			string walk = " vampires walk tonight.";
			if (v == 1) walk = " vampire walks tonight.";
			DuskMessageLog("night", epoch);
			if (s_BloodMoon)
				AZ_VampTurn.Announce("BLOOD MOON. " + v + walk + " Tonight they hear every heartbeat. Stay by the fires.");
			else
				AZ_VampTurn.Announce("Night falls on Namalsk. " + v + walk + " Keep the village fires burning.");
			if (!s_BloodMoon && GoalEligible())
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(DuskGoal, 10000, false, epoch);
		}
	}

	static void DuskHumans(int epoch)
	{
		if (!DuskMessageValid(epoch)) return;
		array<PlayerBase> online;
		AZ_VampState.OnlinePlayers(online);
		DuskMessageLog("humans", epoch);
		WarnHumans(online);
	}

	static void DuskAltar(int epoch)
	{
		if (!DuskMessageValid(epoch) || AZ_VampState.CountOnlineVampires() > 0) return;
		array<PlayerBase> online;
		AZ_VampState.OnlinePlayers(online);
		DuskMessageLog("altar", epoch);
		AZ_VampAltar.EmptyDusk(online);
	}

	static void DuskShadows(int epoch)
	{
		if (!DuskMessageValid(epoch) || !s_BloodMoon) return;
		array<PlayerBase> online;
		AZ_VampState.OnlinePlayers(online);
		DuskMessageLog("shadows", epoch);
		foreach (PlayerBase hv : online)
		{
			if (hv && hv.IsAlive() && hv.m_AZVampire) AZ_VampAltar.ShadowHint(hv);
		}
	}

	static void Dusk(array<PlayerBase> online)
	{
		CancelDuskMessages();
		s_DuskAt = GetGame().GetTime();
		AZ_VampState.s_Data.bloodMoonGoalEarnedTonight = false;
		s_NightNo++;
		AZ_VampState.s_Data.nightIndex = s_NightNo;
		AZ_VampAltar.BeginNight("dusk");
		AZ_VampAudit.Summary("DUSK");
		string resetCounters = "preResetFeeds=" + s_NightFeeds;
		resetCounters += " preResetCures=" + s_NightCured;
		resetCounters += " queuedMoon=" + AZ_VampState.s_Data.bloodMoonNext;
		AZ_VampAudit.Record("moon", "DUSK_COUNTER_RESET", resetCounters);
		s_NightFeeds = 0;
		s_NightTurned = 0;
		s_NightCured = 0;
		s_NightAsh = 0;
		AZ_VampState.s_Data.nightVampiresPeak = 0;


		bool none = AZ_VampState.CountOnlineVampires() == 0;
		bool opening = none && !AZ_VampState.s_Data.zeroSeeded;
		if (opening)
		{
			AZ_VampEvent.SeedZero();
			if (AZ_VampState.CountOnlineVampires() == 0 && AZ_VampState.Cfg().ritualFallback)
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(DuskAltar, 4000, false, s_DuskEpoch);
		}
		else if (none && AZ_VampState.Cfg().ritualFallback)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(DuskAltar, 4000, false, s_DuskEpoch);
		AZ_VampFireworks.Dusk(opening);
		int v = AZ_VampState.CountOnlineVampires();
		TrackNightVampires();
		string walk = " vampires walk tonight.";
		if (v == 1)
			walk = " vampire walks tonight.";
		AZ_VampLandmarks.Relight();
		AZ_VampHolyWaterSpawn.Dusk();
		AZ_VampGraves.Dusk();
		string moonSource = "NOT_SCHEDULED";
		if (AZ_VampState.s_Data.bloodMoonNext)
			moonSource = "EARNED";
		if (RealHalloween())
			moonSource = "HALLOWEEN";
		if (AZ_VampState.Cfg().bloodMoonAlways)
			moonSource = "ALWAYS";
		if (s_ReplayBloodMoon)
			moonSource = "RESTART_REPLAY";
		s_BloodMoon = WantsBloodMoon();
		AZ_VampAudit.Record("moon", moonSource, "active=" + s_BloodMoon + " need=" + GoalNeed() + " peak=" + AZ_VampState.s_Data.nightVampiresPeak + " gap=" + AZ_VampState.Cfg().bloodMoonGapNights + " rest=" + AZ_VampState.s_Data.nightsSinceBloodMoon);
		s_ReplayBloodMoon = false;
		CalendarNightFallback(s_BloodMoon, "DUSK");
		AZ_VampState.s_Data.bloodMoonNext = false;
		AZ_VampState.s_Data.bloodMoonTonight = s_BloodMoon;
		AZ_VampState.s_Data.nightFeeds = 0;
		AZ_VampState.s_Data.nightCured = 0;
		AZ_VampState.Save();
		SyncMoon(online);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(DuskNight, 2000, false, s_DuskEpoch);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(DuskHumans, 20000, false, s_DuskEpoch);
		if (s_BloodMoon)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(DuskShadows, 8000, false, s_DuskEpoch);
		// Orb presentation follows the real moon/night state at once, not only on its 60 s tick.
		if (AZ_VampCryptA1.Ready())
			AZ_VampCryptA1.Refresh();
		Print("[AZ_Vamp] dusk, vampires " + v + " blood moon " + s_BloodMoon);
	}


	static void WarnHumans(array<PlayerBase> online)
	{
		if (!AZ_VampState.s_Data.warned)
			AZ_VampState.s_Data.warned = new array<string>;
		bool any = false;
		foreach (PlayerBase p : online)
		{
			string uid = AZ_VampState.Uid(p);
			if (!p || !p.IsAlive() || p.m_AZVampire || uid == "" || AZ_VampState.s_Data.warned.Find(uid) >= 0)
				continue;
			p.Message("Something hunts at night here. Village fires burn vampires; stay near them. Holy water washes out a bite.", "colorImportant");
			AZ_VampState.s_Data.warned.Insert(uid);
			any = true;
		}
		if (any)
			AZ_VampState.Save();
	}

	static void Dawn(array<PlayerBase> online)
	{
		CancelDuskMessages();
		if (DayMin() < 720)
			s_CalendarPrepared = false;
		AZ_VampFeed.DawnFall();
		AZ_VampCryptExit.Dawn();
		int humans = 0;
		foreach (PlayerBase p : online)
		{
			if (p.IsAlive() && !p.m_AZVampire)
				humans++;
		}
		string line = "Dawn. " + humans + " humans alive";
		if (s_NightCured > 0)
			line += ", " + s_NightCured + " cured";
		if (s_NightTurned > 0)
			line += ", " + s_NightTurned + " turned";
		if (s_NightAsh > 0)
			line += ", " + s_NightAsh + " burned to ash";
		if (AZ_VampLandmarks.s_Fires && AZ_VampLandmarks.s_Fires.Count() > 0)
			line += ". " + AZ_VampLandmarks.FiresBurning() + " of " + AZ_VampLandmarks.s_Fires.Count() + " fires lit";
		if (AZ_VampState.Cfg().broadcasts)
			AZ_VampTurn.Announce(line + ".");

		int need = GoalNeed();
		if (need > 0)
		{
			bool say = AZ_VampState.Cfg().broadcasts;
			int score = MoonScore();
			if (score < 0)
				score = 0;
			int gap = AZ_VampState.Cfg().bloodMoonGapNights;
			if (s_BloodMoon || AZ_VampState.s_Data.bloodMoonGoalEarnedTonight)
				AZ_VampState.s_Data.nightsSinceBloodMoon = 0;
			else
				AZ_VampState.s_Data.nightsSinceBloodMoon++;
			int rest = AZ_VampState.s_Data.nightsSinceBloodMoon;
			string decision = AZ_VampSeedPolicy.MoonReason(need, score, rest, gap);
			if (AZ_VampState.Cfg().bloodMoonImmediate && AZ_VampState.s_Data.bloodMoonGoalEarnedTonight)
				decision = "IMMEDIATE_COMPLETED";
			else if (AZ_VampState.Cfg().bloodMoonImmediate && decision == "EARNED")
				decision = "IMMEDIATE_NO_NEXT_QUEUE";
			AZ_VampAudit.Record("moon", decision, "score=" + score + " need=" + need + " rest=" + rest + " gap=" + gap);
			AZ_VampState.Save();
			if (decision == "GAP")
			{
				if (say)
					AZ_VampTurn.Announce("The vampires fed well (" + score + " of " + need + "), but the moon has not healed since the last blood moon.");
			}
			else if (GoalQueueEarned(AZ_VampState.Cfg().bloodMoonImmediate, decision))
			{
				AZ_VampState.s_Data.bloodMoonNext = true;
				AZ_VampState.Save();
				if (say)
					AZ_VampTurn.Announce("The vampires fed well (" + score + " of " + need + "). Tomorrow night the moon bleeds.");
			}
			else if (say && !AZ_VampState.Cfg().bloodMoonImmediate)
				AZ_VampTurn.Announce("The moon stays pale. The vampires fed " + score + " of " + need + " (every cure takes 2 back).");
		}
		else
			AZ_VampAudit.Record("moon", "DISABLED", "need=" + need);
		AZ_VampAudit.Record("moon", "DAWN_END", "wasActive=" + s_BloodMoon + " scheduledNext=" + AZ_VampState.s_Data.bloodMoonNext);
		AZ_VampAudit.Summary("DAWN");
		AZ_VampState.s_Data.bloodMoonGoalEarnedTonight = false;
		s_BloodMoon = false;
		AZ_VampState.s_Data.bloodMoonTonight = false;
		AZ_VampState.s_Data.nightFeeds = 0;
		AZ_VampState.s_Data.nightCured = 0;
		AZ_VampState.Save();
		SyncMoon(online);
		if (AZ_VampCryptA1.Ready())
			AZ_VampCryptA1.Refresh();
		Print("[AZ_Vamp] dawn: score " + MoonScore() + " next blood moon " + AZ_VampState.s_Data.bloodMoonNext + ", humans " + humans + " feeds " + s_NightFeeds + " cured " + s_NightCured + " turned " + s_NightTurned + " ash " + s_NightAsh);
	}


	static string Titles(array<PlayerBase> online)
	{
		string saver, elder, last, hunter;
		int bestCures, bestFeeds, bestSlain;
		float bestAlive = -1;
		foreach (AZ_VampRecord r : AZ_VampState.s_Data.players)
		{
			if (r.cures > bestCures)
			{
				bestCures = r.cures;
				saver = r.name;
			}
			if (r.slain > bestSlain)
			{
				bestSlain = r.slain;
				hunter = r.name;
			}
			if (r.feedsTotal > bestFeeds)
			{
				bestFeeds = r.feedsTotal;
				elder = r.name;
			}
		}
		foreach (PlayerBase p : online)
		{
			AZ_VampRecord pr = Rec(p);
			if (!pr || p.m_AZVampire || !p.IsAlive())
				continue;
			float t = HumanSeconds(p, pr);
			if (t > bestAlive)
			{
				bestAlive = t;
				last = pr.name;
			}
		}
		string res = "";
		if (last != "")
			res += "Last Human Standing: " + last + " (" + Math.Floor(bestAlive / 60) + " min). ";
		if (saver != "")
			res += "Most Lives Saved: " + saver + " (" + bestCures + "). ";
		if (hunter != "")
			res += "Vampire Hunter: " + hunter + " (" + bestSlain + " slain). ";
		if (elder != "")
			res += "Eldest Vampire: " + elder + " (" + bestFeeds + " feeds).";
		if (res == "")
			res = "No titles yet.";
		return res;
	}


	static bool s_ProbeWasActive;
	static void ProbeNight(int step)
	{
		World w = GetGame().GetWorld();
		switch (step)
		{
			case 0:
				s_ProbeWasActive = AZ_VampState.s_Data.eventActive;
				AZ_VampState.s_Data.eventActive = true;
				w.SetDate(2011, 11, 5, 14, 0);
				break;
			case 1:
				w.SetDate(2011, 11, 5, 22, 0);
				break;
			case 2:
				s_NightFeeds = 99;
				w.SetDate(2011, 11, 6, 10, 0);
				break;
			case 3:
				w.SetDate(2011, 11, 6, 22, 0);
				break;
			case 4:
				Print("[AZ_Vamp] probe blood moon night " + s_BloodMoon);
				w.SetDate(2011, 11, 7, 10, 0);
				break;
			default:
				AZ_VampState.s_Data.eventActive = s_ProbeWasActive;
				AZ_VampState.s_Data.bloodMoonNext = false;
				AZ_VampState.Save();
				Print("[AZ_Vamp] progress probe done");
				return;
		}
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ProbeNight, 7000, false, step + 1);
	}
}
