





class AZ_VampEvent
{
	static const int RPC_CMD = 4702128;
	static const int TICK_MS = 2000;
	static ref map<string, int> s_BurnWarnAt;
	static bool s_MoonChecked;

	static void Start()
	{
		s_BurnWarnAt = new map<string, int>;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Tick, TICK_MS, true);
		AZ_VampReveal.Start();
	}

	static bool Active()
	{
		return AZ_VampState.s_Data && AZ_VampState.s_Data.eventActive;
	}

	static void Tick()
	{

		if (!GetGame().GetWorld())
			return;
		int nowMs = GetGame().GetTime();
		AZ_VampWolves.Watch();
		AZ_VampSchedule.Tick();
		AZ_VampMist.Tick();
		if (nowMs >= AZ_VampState.s_CfgReloadAt)
		{
			AZ_VampState.s_CfgReloadAt = nowMs + AZ_VampState.RELOAD_MS;
			AZ_VampState.ReloadCfg();
		}
		array<PlayerBase> list;
		AZ_VampState.OnlinePlayers(list);
		int now = GetGame().GetTime();
		AZ_VampProgress.Tick(list);
		bool sun = Active() && AZ_VampSun.SunUp();
		AZ_VampAltar.Watch();
		foreach (PlayerBase p : list)
		{
			AZ_VampState.SyncBiteFlags(p);

			bool exposed = sun && p.m_AZVampire && p.IsAlive() && (!p.IsUnconscious() || p.m_AZDownUntil > 0) && AZ_VampSun.Exposed(p);
			float shield = 1;
			if (exposed)
				shield = AZ_VampSun.WeatherShield();
			if (shield <= 0)
				exposed = false;

			bool holy = !exposed && Active() && p.m_AZVampire && p.IsAlive() && !p.IsUnconscious() && AZ_VampLandmarks.OnHolyGround(p);
			bool fire = !exposed && !holy && Active() && p.m_AZVampire && p.IsAlive() && !p.IsUnconscious() && AZ_VampLandmarks.NearSanctuaryFire(p);
			holy = holy || fire;
			AZ_VampAsh.SetBurn(p, exposed || holy);
			if (exposed)
				AZ_VampTrail.Scorch(p, now);
			if (!exposed && !holy)
			{

				if (Active() && p.m_AZVampire && p.IsAlive() && !AZ_VampHunger.NoHeal(p) && p.GetHealth("", "Blood") >= AZ_VampState.Cfg().fullBlood - 100)
					p.AddHealth("", "Health", AZ_VampState.Cfg().fullHealPerSec * AZ_VampRest.DayHealFactor(p) * TICK_MS / 1000.0);
				continue;
			}
			float rate = AZ_VampState.Cfg().sunBurnPerSec * shield * AZ_VampSun.SkinCover(p);
			if (holy)
				rate = AZ_VampState.Cfg().holyBurnPerSec;
			if (fire)
				rate = AZ_VampState.Cfg().fireBurnPerSec;
			AZ_VampEvidence.BeginBurn(p, exposed, holy, fire);
			p.AddHealth("", "Health", -rate * TICK_MS / 1000.0);
			AZ_VampEvidence.EndBurn(p, exposed, holy, fire);
			string uid = AZ_VampState.Uid(p);
			if (!s_BurnWarnAt.Contains(uid) || now >= s_BurnWarnAt.Get(uid))
			{
				if (fire)
					p.Message("The fire's light sears you. Back off!", "colorImportant");
				else if (holy)
					p.Message("Holy ground. It burns! Get out!", "colorImportant");
				else
					p.Message("The sun burns your skin. Get under a roof! Clouds and clothing only slow it.", "colorImportant");
				s_BurnWarnAt.Set(uid, now + 20000);
			}
		}
	}




	static string StartEvent()
	{
		AZ_VampState.s_Data.eventActive = true;
		AZ_VampState.s_Data.stoppedByAdmin = false;
		AZ_VampState.s_Data.zeroSeeded = false;
		AZ_VampState.Save();
		if (!GetGame().GetWorld().IsNight())
		{
			AZ_VampTurn.Announce("Plague Night begins. The first vampire wakes at nightfall.");
			return "event started, patient zero is picked at nightfall";
		}
		int n = SeedZero();
		if (n == 0)
		{
			array<PlayerBase> online;
			AZ_VampState.OnlinePlayers(online);
			if (AZ_VampState.Cfg().ritualFallback)
				AZ_VampAltar.EmptyDusk(online);
			return "event started, no patient zero; ritual offered";
		}
		AZ_VampTurn.Announce("The night has teeth. Vampires are among you.");
		return "event started, patient zero: " + n;
	}

	static int SeedZero()
	{
		array<PlayerBase> list;
		AZ_VampState.OnlinePlayers(list);
		array<PlayerBase> zeros = new array<PlayerBase>;
		foreach (PlayerBase p : list)
		{
			if (AZ_VampState.s_Data.patientZero.Find(AZ_VampState.Uid(p)) >= 0)
				zeros.Insert(p);
		}
		if (AZ_VampState.Cfg().randomPatientZero && zeros.Count() == 0 && list.Count() > 0)
		{
			int per = AZ_VampState.Cfg().patientZeroPer;
			if (per < 1)
				per = 1;
			int want = list.Count() / per;
			if (want < 1)
				want = 1;
			array<PlayerBase> pool = new array<PlayerBase>;
			pool.Copy(list);
			while (zeros.Count() < want && pool.Count() > 0)
			{
				int i = Math.RandomInt(0, pool.Count());
				zeros.Insert(pool[i]);
				pool.Remove(i);
			}
		}
		foreach (PlayerBase z : zeros)
		{
			MakeVampire(z, "patient_zero");
			z.Message("Something old found you in the dark and drank. You are its now. Feed after dark.", "colorImportant");
			AZ_VampTurn.Teach(z);
		}
		if (zeros.Count() > 0)
			AZ_VampState.s_Data.zeroSeeded = true;
		AZ_VampState.Save();
		Print("[AZ_Vamp] patient zero seeded: " + zeros.Count());
		return zeros.Count();
	}

	static string StopEvent()
	{
		AZ_VampProgress.CancelDuskMessages();
		AZ_VampState.s_Data.eventActive = false;
		AZ_VampState.s_Data.stoppedByAdmin = true;
		AZ_VampState.s_Data.bloodMoonTonight = false;
		AZ_VampState.s_Data.nightFeeds = 0;
		AZ_VampState.s_Data.nightCured = 0;
		AZ_VampState.Save();
		array<PlayerBase> list;
		AZ_VampState.OnlinePlayers(list);
		int v = AZ_VampState.CountOnlineVampires();
		AZ_VampTurn.Announce("Dawn. The hunt is over: " + (list.Count() - v) + " humans survived, " + v + " vampires.");
		return "event stopped";
	}

	static void MakeVampire(PlayerBase p, string source = "admin")
	{
		bool wasVampire = p.m_AZVampire;
		AZ_VampTurn.ClearSick(p);
		AZ_VampState.SetVampire(p, true);
		p.m_AZInfectionSource = source;
		if (!wasVampire)
			AZ_VampAudit.Life(p, "TURN_COMPLETE", source);
		AZ_VampState.SetImmune(p);
		AZ_VampBind.Bind(p);
		if (!wasVampire)
			AZ_VampLEHS.Strip(p);
	}

	static void Cure(PlayerBase p)
	{
		AZ_VampAudit.Life(p, "ADMIN_CURE", p.m_AZInfectionSource);
		AZ_VampState.SetVampire(p, false);
		AZ_VampBind.Unbind(p);
		p.m_AZVampBitten = false;
		AZ_VampTurn.ClearSick(p);
		EntityAI worn = p.FindAttachmentBySlotName("Mask");
		if (worn && worn.IsKindOf("AZ_VampireMask_Base"))
			GetGame().ObjectDelete(worn);
		AZ_VampState.SetImmune(p);
	}


	static string FeedOn(PlayerBase victim, string cls = "SurvivorM_Mirek")
	{
		if (victim.m_AZVampire || victim.m_AZVampTurning || victim.m_AZVampHeld)
			return NotPrey(victim);
		PlayerBase vamp = PlayerBase.Cast(GetGame().CreateObject(cls, victim.GetPosition() + victim.GetDirection() * AZ_VampFeed.ROOT_GAP, false, true));
		if (!vamp)
			return "could not spawn the dummy vampire";
		AZ_VampDress.Elder(vamp, Math.RandomInt(0, 1000));
		vamp.SetDirection(-victim.GetDirection());
		AZ_VampState.SetVampire(vamp, true);
		AZ_VampBind.Bind(vamp);
		vamp.SetHealth("", "Blood", AZ_VampState.Cfg().vampDrainFloor);
		AZ_VampFeed.Hold(vamp, victim);
		AZ_VampFeedAnim.DummyFeedStart(vamp);

		int ms = victim.m_AZVampFloorAt - GetGame().GetTime() + 500;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(FeedOnEnd, ms, false, vamp, victim);
		return "a vampire drinks from you for " + (ms / 1000) + " s";
	}



	static string FeedMe2(PlayerBase victim, string arg)
	{
		if (victim.m_AZVampire || victim.m_AZVampTurning || victim.m_AZVampHeld)
			return NotPrey(victim);
		string cls = VampType(arg);
		vector fwd = victim.GetDirection();
		fwd[1] = 0;
		fwd.Normalize();
		PlayerBase front = AIVampire(cls, victim.GetPosition() + fwd * AZ_VampFeed.ROOT_GAP, -fwd);
		PlayerBase back = AIVampire(cls, victim.GetPosition() - fwd * AZ_VampFeed.ROOT_GAP, fwd);
		if (!front || !back)
			return "could not spawn the dummy vampires";
		victim.m_AZVampRear = false;
		AZ_VampFeed.Hold(front, victim);
		AZ_VampFeed.Join(back, victim);
		AZ_VampFeedAnim.DummyFeedStart(front);
		AZ_VampFeedAnim.DummyFeedStart(back);
		int ms = victim.m_AZVampFloorAt - GetGame().GetTime() + 500;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(FeedOnEnd2, ms, false, front, back, victim);
		return "two vampires drink from you, front and back, for " + (ms / 1000) + " s";
	}

	static void FeedOnEnd2(PlayerBase front, PlayerBase back, PlayerBase victim)
	{
		AZ_VampFeedAnim.DummyFeedEnd(front);
		AZ_VampFeedAnim.DummyFeedEnd(back);
		AZ_VampFeed.Release(null, victim);
	}


	static string VampType(string arg)
	{
		if (arg.Contains("f"))
			return AZ_VampDress.Female(Math.RandomInt(0, 1000));
		if (arg.Contains("m") || Math.RandomInt(0, 2) == 0)
			return AZ_VampDress.Male(Math.RandomInt(0, 1000));
		return AZ_VampDress.Female(Math.RandomInt(0, 1000));
	}

	static PlayerBase AIVampire(string cls, vector pos, vector dir)
	{
		pos = AZ_VampSpawn.Floor(pos, pos[1]);
		PlayerBase v = PlayerBase.Cast(GetGame().CreateObject(cls, pos, false, true));
		if (!v)
			return null;
		AZ_VampDress.Elder(v, Math.RandomInt(0, 1000));
		v.SetDirection(dir);
		AZ_VampState.SetVampire(v, true);
		AZ_VampBind.Bind(v);
		v.SetHealth("", "Blood", AZ_VampState.Cfg().vampDrainFloor);
		return v;
	}






	static string MoonAB(PlayerBase admin, string arg)
	{
		arg.ToUpper();
		World w = GetGame().GetWorld();
		if (arg == "ALTAR" || arg == "SKY")
		{
			vector at = Vector(7650.7, 25.1, 8806.6);
			vector look = AZ_VampCrypt.Nave(AZ_VampCrypt.CountessF(), 0, 2.0);
			if (arg == "SKY")
			{
				at = AZ_VampCryptA1.ExitPoint();
				look = at + Vector(0, 0, -10);
			}
			admin.SetPosition(at);
			vector face = look - at;
			face[1] = 0;
			admin.SetDirection(face.Normalized());
			return "moon A/B: at the " + arg + " spot " + at;
		}
		if (arg != "A" && arg != "B" && arg != "C" && arg != "E" && arg != "F")
			return "moon A/B: /vamp moonab A|B|C|E|F|altar|sky";
		if (!Active())
			StartEvent();
		bool moon = arg == "A" || arg == "C" || arg == "F";
		bool fullDate = arg == "A" || arg == "C" || arg == "E";
		AZ_VampProgress.s_BloodMoon = moon;
		AZ_VampState.s_Data.bloodMoonTonight = moon;
		if (fullDate)
		{
			w.SetDate(2011, 11, 5, 23, 30);
			AZ_VampProgress.FullMoonDate();
		}
		else
			w.SetDate(2011, 11, 20, 23, 30);
		admin.m_AZBloodMoon = moon;
		admin.SetSynchDirty();
		string band = "A";
		if (arg == "C")
			band = "T";
		AZ_VampBand.Send(admin, band);
		int y, mo, d, h, mi;
		w.GetDate(y, mo, d, h, mi);
		string tint = "on";
		if (band == "T")
			tint = "off";
		return "moon A/B " + arg + ": blood moon " + moon.ToString() + ", date " + y.ToString() + "-" + mo.ToString() + "-" + d.ToString() + " " + h.ToString() + ":" + mi.ToString() + ", tint " + tint;
	}

	static string ShareFeed(PlayerBase admin, string arg)
	{
		if (!admin.m_AZVampire)
			return "be a vampire first (/vamp turn me)";
		AZ_VampFeed.s_SharedTest = true;
		vector fwd = admin.GetDirection();
		fwd[1] = 0;
		fwd.Normalize();
		vector vp = admin.GetPosition() + fwd * 1.6;
		vp = AZ_VampSpawn.Floor(vp, admin.GetPosition()[1]);
		PlayerBase victim = PlayerBase.Cast(GetGame().CreateObject(DummyType(arg), vp, false, true));
		if (!victim)
			return "could not spawn the survivor";
		AZ_VampDress.Survivor(victim, Math.RandomInt(0, 1000));
		victim.SetDirection(fwd);
		PlayerBase vamp = AIVampire(DummyType(arg), vp + fwd * AZ_VampFeed.ROOT_GAP, -fwd);
		if (!vamp)
			return "could not spawn the dummy vampire";
		victim.m_AZVampRear = false;
		AZ_VampFeed.Hold(vamp, victim);
		AZ_VampFeedAnim.DummyFeedStart(vamp);
		int ms = victim.m_AZVampFloorAt - GetGame().GetTime() + 500;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(FeedOnEnd, ms, false, vamp, victim);
		return "a vampire drinks from its front for " + (ms / 1000) + " s: step up and bite it from behind (shared feed on)";
	}




	static int JMeLifetimeMs()
	{
		AZ_VampConfig cfg = AZ_VampState.Cfg();



		return 2500 + Math.Max(1, cfg.chainMax) * (Math.Max(AZ_VampFeed.MIN_J_FEED_SEC, cfg.chainBiteSec) * 1000 + 6000) + 4500 + 2000;
	}

	static string JMe(PlayerBase admin, string arg)
	{
		if (!Active())
			return "start the event first (/vamp start)";
		if (admin.m_AZVampire)
			return "be human for this one (/vamp cure me)";


		AZ_VampState.ClearImmune(admin);
		admin.SetHealth("", "Blood", admin.GetMaxHealth("", "Blood"));
		string cls = VampType(arg);
		vector fwd = admin.GetDirection();
		fwd[1] = 0;
		fwd.Normalize();
		vector rt = Vector(fwd[2], 0, -fwd[0]);
		for (int i = 0; i < 3; i++)
		{
			vector dp = admin.GetPosition() + rt * (i - 1) * 2.2 + fwd * (1.5 - Math.AbsFloat(i - 1) * 1.5);
			dp = AZ_VampSpawn.Floor(dp, admin.GetPosition()[1]);
			AZ_VampDress.Survivor(PlayerBase.Cast(GetGame().CreateObject(AZ_VampDress.Any(i), dp, false, true)), i);
		}
		PlayerBase vamp = AIVampire(cls, admin.GetPosition() + fwd * 10, -fwd);
		if (!vamp)
			return "could not spawn the dummy vampire";
		vamp.m_AZBloodMoon = true;
		vamp.SetSynchDirty();
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(JMeGo, 2500, false, vamp);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(GetGame().ObjectDelete, JMeLifetimeMs(), false, vamp);
		return "a vampire 10 m ahead strikes in 2.5 s: three survivors around you, then you";
	}

	static void JMeGo(PlayerBase vamp)
	{
		if (vamp && !AZ_VampShadow.TryStep(vamp))
			Print("[AZ_Vamp] jme: the AI vampire found no one to step to");
	}


	static string NotPrey(PlayerBase p)
	{
		if (p.m_AZVampHeld)
			return "you are being fed on right now";
		if (p.m_AZVampire)
			return "you are a vampire: /vamp cure me first";
		if (p.IsUnconscious())
			return "you are out cold and sick from the bite: /vamp cure me";
		return "you are sick from a bite (turning): /vamp cure me first";
	}


	static string DummyType(string arg)
	{
		if (arg.Contains("f"))
			return AZ_VampDress.Female(Math.RandomInt(0, 1000));
		return AZ_VampDress.Male(Math.RandomInt(0, 1000));
	}

	static string Dummy(PlayerBase admin, string kind)
	{
		PlayerBase dm = PlayerBase.Cast(GetGame().CreateObject(DummyType(kind), admin.GetPosition() + admin.GetDirection() * 2, false, true));
		if (!dm)
			return "could not spawn";
		AZ_VampDress.Survivor(dm, Math.RandomInt(0, 1000));
		if (kind.Contains("light"))
		{
			dm.SetDirection(-admin.GetDirection());
			EntityAI fl = dm.GetHumanInventory().CreateInHands("Flashlight");
			if (fl)
			{
				fl.GetInventory().CreateAttachment("Battery9V");
				if (fl.GetCompEM())
					fl.GetCompEM().SwitchOn();
			}
			AZ_VampRepel.s_TestHolders.Insert(dm);
			return "survivor 2 m ahead with a lit flashlight, facing you (at night a vampire in its beam burns)";
		}
		if (kind.Contains("item"))
		{
			dm.GetHumanInventory().CreateInHands("KitchenKnife");
			return "survivor 2 m ahead holding a knife";
		}
		return DummyType(kind) + " spawned 2 m ahead (also: /vamp dummy light, /vamp dummy item; add f for a female, e.g. lightf)";
	}

	static string CompelMe(PlayerBase victim, string cls = "SurvivorM_Mirek")
	{
		if (!Active())
			return "start the event first (/vamp start)";
		if (victim.m_AZVampire || victim.m_AZVampTurning || victim.m_AZVampCompelled || victim.IsRestrained())
			return "you are a vampire, sick or already bound (/vamp cure me first)";
		vector fwd = victim.GetDirection();
		fwd[1] = 0;
		fwd.Normalize();
		PlayerBase vamp = PlayerBase.Cast(GetGame().CreateObject(cls, victim.GetPosition() + fwd * 0.9, false, true));
		if (!vamp)
			return "could not spawn the dummy vampire";
		AZ_VampDress.Elder(vamp, Math.RandomInt(0, 1000));
		vamp.SetDirection(-fwd);
		AZ_VampState.SetVampire(vamp, true);
		AZ_VampBind.Bind(vamp);
		vamp.m_AZBloodMoon = true;
		vamp.SetSynchDirty();
		AZ_VampReveal.BareAtWill(vamp, 3);
		victim.Message("Its eyes find yours.", "colorImportant");
		int sec = AZ_VampState.Cfg().compelSec;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampShadow.Compel, AZ_VampShadow.STARE_MS, false, vamp, victim);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(GetGame().ObjectDelete, (sec + 3) * 1000, false, vamp);
		return "a dummy vampire compels you (bound " + sec + " s, it leaves after)";
	}

	static string TieMe(PlayerBase victim)
	{
		if (victim.IsRestrained())
			return "you are already bound";
		string locked = GetGame().ConfigGetTextOut("CfgVehicles Rope OnRestrainChange");
		if (locked == "")
			locked = AZ_VampTie.COMPEL_BONDS;
		if (!AZ_VampTie.Bind(victim, locked))
			return "could not bind you (see the server log)";
		victim.Message("Your hands are bound.", "colorImportant");
		return "bound with " + locked + ": try struggling out";
	}

	static void FeedOnEnd(PlayerBase vamp, PlayerBase victim)
	{
		AZ_VampFeedAnim.DummyFeedEnd(vamp);
		AZ_VampFeed.Release(vamp, victim);
	}



	static int s_MoonY, s_MoonMo, s_MoonD, s_MoonH, s_MoonMi, s_MoonBestDay;
	static float s_MoonBest;
	static void MoonScan(PlayerBase admin, int day)
	{
		World w = GetGame().GetWorld();
		if (day == 1)
		{
			w.GetDate(s_MoonY, s_MoonMo, s_MoonD, s_MoonH, s_MoonMi);
			s_MoonBest = -1;
		}
		w.SetDate(s_MoonY, s_MoonMo, day, 0, 0);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(MoonRead, 500, false, admin, day);
	}

	static void MoonRead(PlayerBase admin, int day)
	{
		float m = GetGame().GetWorld().GetMoonIntensity();
		Print("[AZ_Vamp] moon " + s_MoonY + "/" + s_MoonMo + "/" + day + " 00:00 intensity " + m);
		if (m > s_MoonBest)
		{
			s_MoonBest = m;
			s_MoonBestDay = day;
		}
		if (day < 31)
		{
			MoonScan(admin, day + 1);
			return;
		}
		GetGame().GetWorld().SetDate(s_MoonY, s_MoonMo, s_MoonD, s_MoonH, s_MoonMi);
		if (admin)
			admin.Message("[vamp] brightest moon " + s_MoonY + "/" + s_MoonMo + "/" + s_MoonBestDay + " intensity " + s_MoonBest + " (date restored)", "colorAction");
	}

	static PlayerBase FindPlayer(PlayerBase admin, string who)
	{
		if (who == "" || who == "me")
			return admin;
		who.ToLower();
		array<PlayerBase> list;
		AZ_VampState.OnlinePlayers(list);
		foreach (PlayerBase p : list)
		{
			if (!p.GetIdentity())
				continue;
			string name = p.GetIdentity().GetName();
			name.ToLower();
			if (name.Contains(who))
				return p;
		}
		return null;
	}

	static void Command(PlayerBase admin, PlayerIdentity sender, string text)
	{
		if (!sender || !admin)
			return;
		if (AZ_VampState.Cfg().adminUids.Find(sender.GetId()) < 0)
		{

			int dnow = GetGame().GetTime();
			if (dnow - admin.m_AZCmdDeniedAt >= 60000)
			{
				admin.m_AZCmdDeniedAt = dnow;
				Print("[AZ_Vamp] command denied for " + sender.GetName() + " uid " + sender.GetId() + " (add to adminUids in config.json; repeats are silent for 60 s)");
				admin.Message("Not allowed.", "colorImportant");
			}
			return;
		}
		array<string> parts = new array<string>;
		string line = text.Trim();
		line.Split(" ", parts);
		string cmd = "status";
		if (parts.Count() > 1)
			cmd = parts[1];
		string arg = "";
		if (parts.Count() > 2)
			arg = parts[2];
		string reply;
		PlayerBase target;
		switch (cmd)
		{
			case "start":
				reply = StartEvent();
				break;
			case "reload":
				reply = AZ_VampState.ReloadCfg(true);
				break;
			case "torchlights":
				reply = AZ_VampState.SetTorchLights(arg);
				break;
			case "stop":
				reply = StopEvent();
				break;
			case "lights":
				reply = AZ_VampLightTune.Send(admin, arg);
				break;
			case "drinktest":
				reply = AZ_VampDrinkTest.Start(admin);
				break;
			case "dimtest":
				admin.SetHealth("", "Shock", 0);
				reply = "dim view test: knocked out, 30 s of the bitten view (AZ-2569)";
				Print("[AZ_Vamp] dimtest " + AZ_VampState.Uid(admin));
				break;
			case "night":
				int y, mo, d, h, mi;
				GetGame().GetWorld().GetDate(y, mo, d, h, mi);
				GetGame().GetWorld().SetDate(y, mo, d, 21, 0);
				reply = "time set to 21:00 on " + y + "/" + mo + "/" + d;
				break;
			case "day":
				int yd, mod, dd2, hd, mid;
				GetGame().GetWorld().GetDate(yd, mod, dd2, hd, mid);
				GetGame().GetWorld().SetDate(yd, mod, dd2, 12, 0);
				reply = "time set to 12:00 on " + yd + "/" + mod + "/" + dd2;
				break;
			case "dawn":


				int yw, mow, dw, hw, miw;
				GetGame().GetWorld().GetDate(yw, mow, dw, hw, miw);
				if (arg.Length() >= 4 && arg.IndexOf(":") > 0)
				{
					int colon = arg.IndexOf(":");
					AZ_VampProgress.s_DawnMin = arg.Substring(0, colon).ToInt() * 60 + arg.Substring(colon + 1, arg.Length() - colon - 1).ToInt();
				}
				int dawnAt = (AZ_VampProgress.s_DawnMin - 2 + 1440) % 1440;
				GetGame().GetWorld().SetDate(yw, mow, dw, dawnAt / 60, dawnAt % 60);
				reply = "time set to " + (dawnAt / 60) + ":" + (dawnAt % 60) + " on " + yw + "/" + mow + "/" + dw + " (2 min before the dawn minute " + (AZ_VampProgress.s_DawnMin / 60) + ":" + (AZ_VampProgress.s_DawnMin % 60) + ")";
				Print("[AZ_Vamp] admin dawn: " + reply);
				break;
			case "date":
				int dy, dmo, dd, dh, dmi;
				GetGame().GetWorld().GetDate(dy, dmo, dd, dh, dmi);
				TStringArray ymd = new TStringArray;
				arg.Split("/", ymd);
				if (ymd.Count() == 3)
				{
					dy = ymd[0].ToInt();
					dmo = ymd[1].ToInt();
					dd = ymd[2].ToInt();
					GetGame().GetWorld().SetDate(dy, dmo, dd, dh, dmi);
				}
				reply = "date " + dy + "/" + dmo + "/" + dd + " " + dh + ":" + dmi;
				break;
			case "pumpkins":
				reply = AZ_VampDecor.Report(admin.GetPosition());
				break;
			case "moon":
				MoonScan(admin, 1);
				reply = "moon scan: 31 nights of the current month, ~16 s, results in chat and the server log";
				break;
			case "turn":
				target = FindPlayer(admin, arg);
				if (target)
				{
					MakeVampire(target);
					reply = "turned " + arg;
				}
				else
					reply = "no player matching " + arg;
				break;

			case "dummy":
				reply = Dummy(admin, arg);
				break;
			case "stakeme":

				if (!Active())
				{
					reply = "start the event first (/vamp start)";
					break;
				}
				vector sf = admin.GetDirection();
				sf[1] = 0;
				sf.Normalize();
				vector sr2 = Vector(sf[2], 0, -sf[0]);
				vector sb = AZ_VampSpawn.Floor(admin.GetPosition() + sf * 1.5, admin.GetPosition()[1]);
				vector sfr = AZ_VampSpawn.Floor(admin.GetPosition() + sf * 3 + sr2 * 4, admin.GetPosition()[1]);
				PlayerBase vb = PlayerBase.Cast(GetGame().CreateObject("SurvivorM_Mirek", sb, false, true));
				PlayerBase vf = PlayerBase.Cast(GetGame().CreateObject("SurvivorF_Eva", sfr, false, true));
				if (vb)
				{
					vb.SetDirection(sf);
					MakeVampire(vb);
				}
				if (vf)
				{
					vf.SetDirection(admin.GetPosition() - sfr);
					MakeVampire(vf);
				}
				if (!admin.GetItemInHands())
					admin.GetHumanInventory().CreateInHands("AZ_VampStake");
				else
					admin.GetInventory().CreateInInventory("AZ_VampStake");
				reply = "stake in hand (or inventory); vampire 1.5 m ahead, back to you; another 4 m right facing you";
				break;
			case "compelme":
				reply = CompelMe(admin, DummyType(arg));
				break;
			case "tieme":
				reply = TieMe(admin);
				break;
			case "dummies":
				vector fw = admin.GetDirection();
				fw[1] = 0;
				fw.Normalize();
				vector rt = Vector(fw[2], 0, -fw[0]);
				int made = 0;
				for (int dn = 0; dn < 4; dn++)
				{
					float side = -1;
					if (dn == 1 || dn == 3)
						side = 1;
					vector dp = admin.GetPosition() + fw * (6 + dn * 2) + rt * side * (1 + dn * 0.5);
					dp = AZ_VampSpawn.Floor(dp, admin.GetPosition()[1]);
					PlayerBase sv = PlayerBase.Cast(GetGame().CreateObject(AZ_VampDress.Any(dn + 3), dp, false, true));
					AZ_VampDress.Survivor(sv, dn + 3);
					if (sv)
						made++;
				}
				reply = made.ToString() + " survivors spawned 6-12 m ahead";
				break;
			case "titles":
				array<PlayerBase> everyone;
				AZ_VampState.OnlinePlayers(everyone);
				reply = AZ_VampProgress.Titles(everyone);
				AZ_VampTurn.Announce(reply);
				break;
			case "steps":
				AZ_VampRecord sr = AZ_VampProgress.Rec(admin);
				AZ_VampProgress.Update(admin);
				reply = "hear " + admin.m_AZHearStep + " calm " + admin.m_AZCalmStep;
				if (sr)
					reply += ", feeds " + sr.feeds + " (total " + sr.feedsTotal + "), cures " + sr.cures + ", human for " + Math.Floor(AZ_VampProgress.HumanSeconds(admin, sr) / 60) + " min";
				break;
			case "fireworks":
				AZ_VampFireworks.Launch();
				reply = "fireworks: lit at the village fires (night only)";
				break;
			case "bloodmoon":

				string started = "";
				if (!Active())
				{
					started = StartEvent() + "; ";
					AZ_VampProgress.s_BloodMoon = false;
				}
				AZ_VampProgress.s_BloodMoon = !AZ_VampProgress.s_BloodMoon;
				AZ_VampState.s_Data.bloodMoonTonight = AZ_VampProgress.s_BloodMoon;
				AZ_VampAudit.Record("moon", "MANUAL_TOGGLE", "active=" + AZ_VampProgress.s_BloodMoon);
				AZ_VampState.Save();
				if (AZ_VampProgress.s_BloodMoon)
					AZ_VampProgress.FullMoonDate();
				else
					AZ_VampProgress.AvoidFullMoon();
				if (AZ_VampCryptA1.Ready())
					AZ_VampCryptA1.Refresh();
				reply = started + "blood moon " + AZ_VampProgress.s_BloodMoon;
				break;
			case "offersmoke":
				reply = AZ_VampRitual.SmokeToggle(arg);
				break;
			case "lootaudit":
				reply = AZ_VampLootPlace.Audit();
				break;
			case "eldersmoke":
				reply = AZ_VampElderWrath.Toggle(arg);
				break;
			case "finale":
				reply = AZ_VampRitual.Finale(admin);
				break;
			case "ritual":
				reply = AZ_VampRitual.Debug(admin, arg);
				break;
			case "friends":
				reply = AZ_VampRitual.Friends(admin, arg);
				break;
			case "riverflow":
				if (arg == "on")
					AZ_VampBand.Send(admin, "F");
				else
					AZ_VampBand.Send(admin, "A");
				reply = "river flow " + arg;
				break;
			case "tp":
				if (AZ_VampAdminTp.Handles(arg))
					reply = AZ_VampAdminTp.Go(admin, arg);
				else
					reply = AZ_VampCryptLedge.AdminTp(admin, arg);
				break;
			case "moonab":
				reply = MoonAB(admin, arg);
				break;
			case "hungry":
				admin.SetHealth("", "Blood", AZ_VampState.Cfg().vampDrainFloor);
				reply = "blood set to " + AZ_VampState.Cfg().vampDrainFloor;
				break;
			case "heal":
				admin.SetHealth("", "Blood", admin.GetMaxHealth("", "Blood"));
				admin.SetHealth("", "Health", admin.GetMaxHealth("", "Health"));
				admin.SetHealth("", "Shock", admin.GetMaxHealth("", "Shock"));
				reply = "healed: full blood, health and shock (test command; COT's player menu crashes the client)";
				break;
			case "feedme":
				reply = FeedOn(admin, DummyType(arg));
				break;
			case "feedme2":
				reply = FeedMe2(admin, arg);
				break;
			case "sharefeed":
				reply = ShareFeed(admin, arg);
				break;
			case "jme":
				reply = JMe(admin, arg);
				break;
			case "probe":
				AZ_VampProbe.Run(admin.GetPosition() + admin.GetDirection() * 4);
				reply = "probe: two AI feed 4 m ahead in 5 s, numbers in the server log";
				break;
			case "drop":
				reply = AZ_VampDrop.Status();
				break;
			case "cure":
				target = FindPlayer(admin, arg);
				if (target)
				{
					Cure(target);
					reply = "cured " + arg;
				}
				else
					reply = "no player matching " + arg;
				break;
			case "reset":
				array<PlayerBase> list;
				AZ_VampState.OnlinePlayers(list);
				foreach (PlayerBase p : list)
				{
					if (p.m_AZVampire)
						Cure(p);
					else if (p.m_AZVampTurning)
						AZ_VampTurn.ClearSick(p);
				}
				AZ_VampState.s_Data.players.Clear();
				AZ_VampState.s_Data.eventActive = false;
				AZ_VampState.s_Data.stoppedByAdmin = true;
				AZ_VampState.Save();
				reply = "all vampires cured, records cleared, event off";
				break;
			default:
				reply = "event " + Active() + ", vampires online " + AZ_VampState.CountOnlineVampires() + ", records " + AZ_VampState.CountVampires() + ", night " + GetGame().GetWorld().IsNight() + ". Ritual tests: /vamp ritual awake | /vamp ritual slab | /vamp finale";
		}
		Print("[AZ_Vamp] admin " + sender.GetName() + ": " + text + " -> " + reply);
		admin.Message("[vamp] " + reply, "colorAction");
	}
}

modded class PlayerBase
{

	bool m_AZStakedHeart;
	int m_AZDownUntil;
	int m_AZDownAt;
	int m_AZDownNight;
	int m_AZDownCount;
	int m_AZCmdRpcAt;
	int m_AZCmdDeniedAt;

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (rpc_type != AZ_VampEvent.RPC_CMD || !GetGame().IsServer())
			return;


		PlayerIdentity cself = GetIdentity();
		if (!sender || !cself || sender.GetId() != cself.GetId())
			return;

		int cnow = GetGame().GetTime();
		if (cnow - m_AZCmdRpcAt < 300)
			return;
		m_AZCmdRpcAt = cnow;
		Param1<string> data;
		if (!ctx.Read(data))
			return;
		AZ_VampEvent.Command(this, sender, data.param1);
	}
}
