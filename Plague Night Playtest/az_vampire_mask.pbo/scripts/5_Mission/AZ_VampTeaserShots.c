










modded class MissionServer
{
	static ref array<Object> s_TzActors;
	static vector s_TzChainAt;
	static vector s_TzChainDir;

	string TzLabel(int step)
	{
		if (step == 100 || step == 130)
			return "tz_call|tz_descent|tz_candles|tz_sleepers|tz_cage|tz_boulder|tz_story_fire|tz_story_torch|tz_vpov|tz_hpov|tz_hpov_snap|tz_hpov_elder|tz_hpov_turn|tz_feed2|tz_thumb|tz_ledge|tz_statue|tz_ritual|tz_eyes|tz_eyes_ecu|tz_elder|tz_fangs|tz_bloodmoon_rise|tz_chain_a|tz_chain_b|tz_chain_c|tz_kneel_ghoul|tz_poster_eva|tz_poster_moon|tz_hook_eva|tz_hook_pov|tz_gown_check|tz_snarl_a|tz_snarl_b|tz_snarl_c|tz_bite_tight|tz_bite_med|tz_bite_over|tz_hunt_ots|tz_hunt_tele|tz_countess_whole|tz_countess_close|tz_snarl_moon|tz_feast_a|tz_feast_b|tz_offer_onlooker|tz_river_stare|tz_river_embers";
		if (step == 101 || step == 102)
			return "tz_call";
		if (step == 103 || step == 104)
			return "tz_descent";
		if (step == 180 || step == 181)
			return "tz_story_fire";
		if (step == 182 || step == 183)
			return "tz_story_torch";
		if (step == 184)
			return "tz_vpov";
		if (step == 185 || step == 186)
			return "tz_hpov";
		if (step == 187)
			return "tz_hpov_snap";
		if (step == 188)
			return "tz_hpov_elder";
		if (step == 189)
			return "tz_hpov_turn";
		if (step >= 131 && step <= 133)
			return "tz_feed2";
		if (step == 134 || step == 135)
			return "tz_thumb";
		if (step == 136 || step == 137)
			return "tz_poster_eva";
		if (step == 138 || step == 139)
			return "tz_poster_moon";
		if (step >= 190 && step <= 192)
			return "tz_hook_eva";
		if (step == 193 || step == 194)
			return "tz_hook_pov";
		if (step == 195)
			return "tz_gown_check";
		if (step == 196 || step == 197)
			return "tz_snarl_a";
		if (step == 198)
			return "tz_snarl_b";
		if (step == 199)
			return "tz_snarl_c";
		if (step == 118)
			return "tz_bite_tight";
		if (step == 119)
			return "tz_bite_med";
		if (step == 148)
			return "tz_bite_over";
		if (step == 165 || step == 166)
			return "tz_hunt_ots";
		if (step == 167)
			return "tz_hunt_tele";
		if (step == 168)
			return "tz_countess_whole";
		if (step == 178)
			return "tz_countess_close";
		if (step == 179)
			return "tz_snarl_moon";
		if (step == 172)
			return "tz_feast_a|tz_feast_b";
		if (step == 173)
			return "tz_feast_a";
		if (step == 174)
			return "tz_feast_b";
		if (step == 169)
			return "tz_offer_onlooker|tz_river_stare|tz_river_embers";
		if (step == 140 || step == 141)
			return "tz_candles";
		if (step == 142 || step == 143)
			return "tz_sleepers";
		if (step == 144 || step == 145)
			return "tz_cage";
		if (step == 146 || step == 147)
			return "tz_boulder";
		if (step == 105 || step == 106)
			return "tz_ledge";
		if (step == 175 || step == 176)
			return "tz_reveal_push";
		if (step == 107 || step == 108)
			return "tz_statue";
		if (step == 109 || step == 110)
			return "tz_ritual";
		if (step == 111 || step == 112)
			return "tz_eyes";
		if (step == 113)
			return "tz_eyes_ecu";
		if (step == 114 || step == 115)
			return "tz_elder";
		if (step == 116 || step == 117)
			return "tz_fangs";
		if (step == 120 || step == 121)
			return "tz_bloodmoon_rise";
		if (step == 122 || step == 123)
			return "tz_chain_a";
		if (step == 124 || step == 125)
			return "tz_chain_b";
		if (step == 126 || step == 127)
			return "tz_chain_c";
		if (step == 128 || step == 129)
			return "tz_kneel_ghoul";
		return "";
	}

	override string HwStepLabel(int step)
	{
		if (step >= 100 && step < 200)
			return TzLabel(step);
		return super.HwStepLabel(step);
	}

	override int HwNextStep(int step)
	{
		if (step == 88 && s_HwOnly)
			return 100;
		if (step == 104)
			return 140;
		if (step == 147)
			return 105;
		if (step == 106)
			return 175;
		if (step == 176)
			return 107;
		if (step == 117)
			return 120;
		if (step == 130)
			return 180;
		if (step == 189)
			return 131;
		if (step == 133)
			return 134;
		if (step == 139)
			return 190;
		if (step == 192)
			return 195;
		if (step == 195)
			return 193;
		if (step == 194)
			return 196;
		if (step == 199)
			return 118;
		if (step == 118)
			return 119;
		if (step == 119)
			return 148;
		if (step == 148)
			return 165;
		if (step == 167)
			return 168;
		if (step == 168)
			return 178;
		if (step == 178)
			return 179;
		if (step == 179)
			return 172;
		if (step == 174)
			return 169;
		if (step == 169)
			return 15;
		if (step >= 100 && step < 200)
			return step + 1;
		return super.HwNextStep(step);
	}

	override void HwStep(PlayerBase p, int step)
	{
		if (step < 100 || step >= 200)
		{
			super.HwStep(p, step);
			return;
		}
		if (!p)
			return;
		if (!HwWanted(step))
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 100, false, p, HwNextStep(step));
			return;
		}
		int next = TzStep(p, step);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, next, false, p, HwNextStep(step));
	}



	bool TzClear(vector look, vector cam)
	{
		return AZ_TzView.Clear(look, cam);
	}



	vector TzKeepOff(PlayerBase p, vector cam)
	{
		array<Object> bodies = new array<Object>;
		if (s_TzActors)
			bodies.Copy(s_TzActors);
		bodies.Insert(p);
		foreach (Object b : bodies)
		{
			if (!b)
				continue;
			vector bp = b.GetPosition();
			if (cam[1] < bp[1] - 0.3 || cam[1] > bp[1] + 2.3)
				continue;
			vector off = cam - bp;
			off[1] = 0;
			float d = off.Length();
			if (d >= 1.8)
				continue;
			if (d < 0.01)
				off = "1 0 0";
			off.Normalize();
			vector pushed = bp + off * 1.8;
			pushed[1] = cam[1];
			Print("[AZ_Vamp] TEST tz camera kept off " + b.GetType() + " (was " + d + " m)");
			cam = pushed;
		}
		return cam;
	}


	void TzLogDate(string why)
	{
		int y, mo, d, h, mi;
		GetGame().GetWorld().GetDate(y, mo, d, h, mi);
		Print("[AZ_Vamp] TEST tz clock " + why + ": " + y + "/" + mo + "/" + d + " " + h + ":" + mi + " night " + GetGame().GetWorld().IsNight() + " fog " + GetGame().GetWeather().GetFog().GetActual() + " moon " + GetGame().GetWorld().GetMoonIntensity());
	}



	vector TzUnblock(vector cam, vector look)
	{
		if (TzClear(look, cam))
			return cam;
		for (int up = 1; up <= 2; up++)
		{
			vector c = cam + Vector(0, 1.5 * up, 0);
			if (TzClear(look, c))
			{
				Print("[AZ_Vamp] TEST tz camera raised " + (1.5 * up) + " m to clear the view");
				return c;
			}
		}
		Print("[AZ_Vamp] TEST tz camera still blocked at " + cam + ", kept");
		return cam;
	}


	void TzShot(PlayerBase p, string label, vector cam, vector look, vector endCam, vector endLook, float dur, float fov = 0.9)
	{
		string payload = label + "#" + dur.ToString() + "#" + TzVec(endCam) + "#" + TzVec(endLook) + "#" + fov.ToString();
		HwShot(p, payload, cam, look);
	}

	string TzVec(vector v)
	{
		return string.Format("%1 %2 %3", v[0], v[1], v[2]);
	}

	static PlayerBase s_TzBiteEva;
	static int s_TzRitualStart;
	static int s_TzRitualShots;

	void TzRitualLie(PlayerBase p)
	{
		Print("[AZ_Vamp] TEST tz ritual: the offering, lie " + AZ_VampRitual.Lie(p));
	}


	void TzRitualWatch(PlayerBase p)
	{
		int now = GetGame().GetTime();
		float alt = AZ_VampCrypt.ALTAR_F;
		if (s_TzRitualShots == 0 && AZ_VampRitual.s_OfferEndAt > s_TzRitualStart)
		{
			s_TzRitualShots = 1;
			vector oc = AZ_VampCrypt.Nave(52.0, 2.2, 2.0);
			vector oa = AZ_VampCrypt.Nave(alt, 0, 0.9);
			TzShot(p, "tz_offer_onlooker", oc, oa, oc + (oa - oc).Normalized() * 0.3, oa, 4, 0.75);
			Print("[AZ_Vamp] TEST tz ritual: the offering's collapse at +" + (AZ_VampRitual.s_OfferEndAt - s_TzRitualStart) + " ms");
		}
		if (s_TzRitualShots < 2 && AZ_VampCryptLurkers.s_RiverVictim == p && AZ_VampCryptLurkers.s_RiverStartAt > s_TzRitualStart)
		{
			s_TzRitualShots = 2;
			int late = now - AZ_VampCryptLurkers.s_RiverStartAt;
			int stare = AZ_VampCryptLurkers.RIVER_ARRIVE_MS + AZ_VampCryptLurkers.STARE_AFTER_ARRIVE_MS - late;
			Print("[AZ_Vamp] TEST tz ritual: the river feed began " + (AZ_VampCryptLurkers.s_RiverStartAt - s_TzRitualStart) + " ms after the start");
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TzRitualStare, Math.Max(0, stare + 600), false, p);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TzRitualEmbers, Math.Max(0, stare + 5200), false, p);
			return;
		}
		if (now - s_TzRitualStart < 118000)
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TzRitualWatch, 200, false, p);
		else
			Print("[AZ_Vamp] TEST tz ritual FAIL: no river feed in 118 s");
	}


	void TzRitualStare(PlayerBase p)
	{
		vector eye = AZ_VampCryptLurkers.RiverSpot() + Vector(0, 1.6, 0);
		vector gaze = AZ_VampCryptLurkers.RiverFace();
		vector c = eye - gaze * 0.3;
		vector at = AZ_VampCryptLurkers.StatueFace();
		TzShot(p, "tz_river_stare", c, at, c + gaze * 0.25, at, 4.5, 0.8);
	}


	void TzRitualEmbers(PlayerBase p)
	{
		float f = AZ_VampCryptLurkers.RIVER_SPOT_F;
		vector c = AZ_VampCrypt.Nave(f - 3.0, 2.0, 1.6);
		vector at = AZ_VampCrypt.Nave(f + 8.0, 0, 1.5);
		TzShot(p, "tz_river_embers", c, at, c + (at - c).Normalized() * 0.2, at, 4, 0.85);
	}

	void TzSnarlTorchShot(PlayerBase p)
	{
		vector f = p.GetDirection();
		f[1] = 0;
		f.Normalize();
		vector head = p.GetBonePositionWS(p.GetBoneIndexByName("Head"));


		vector cam = head + f * 1.4 - "0 0.1 0";
		vector aim = head - "0 0.18 0";
		TzShot(p, "tz_snarl_moon", cam, aim, head + f * 1.3 - "0 0.1 0", aim, 4, 0.55);
	}

	int TzBiteSetup(PlayerBase p, int step)
	{
		TzClearActors();
		TzBloodMoon(p, false);
		if (step == 148)
		{
			GetGame().GetWorld().SetDate(2011, 11, 18, 22, 0);
			TzMoonSky(false);
		}
		else
		{
			GetGame().GetWorld().SetDate(2011, 11, 10, 22, 30);
			TzMoonSky(true);
		}
		if (s_TzFireAt == vector.Zero)
		{
			Fireplace bfp = TzFire();
			TzLightFire(bfp);
			if (bfp)
				s_TzFireAt = bfp.GetPosition();
		}
		vector hm = TzMoonFlat();
		vector side = Vector(hm[2], 0, -hm[0]);
		vector v = s_TzFireAt + hm * 3.0;
		TzHero(v, side, false);
		if (!s_TzHero)
			return 1000;
		if (!s_TzActors)
			s_TzActors = new array<Object>;
		if (s_TzActors.Find(s_TzHero) < 0)
			s_TzActors.Insert(s_TzHero);
		PlayerBase e = TzEva(s_TzHero.GetPosition() + side * AZ_VampFeed.ROOT_GAP, s_TzHero.GetPosition());
		if (!e)
			return 1000;
		e.SetHealth("", "Blood", AZ_VampState.Cfg().vampDrainFloor);
		e.m_AZVampFedUntil = GetGame().GetTime() + 60000;
		AZ_VampReveal.Update(e);
		AZ_VampFeed.Hold(e, s_TzHero);
		AZ_VampFeedAnim.DummyFeedStart(e);
		s_TzBiteEva = e;
		p.SetPosition(s_TzFireAt - hm * 30.0);


		s_TzHero.AZ_VampMarkBitten();
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TzBiteBleed, 1600, false, s_TzHero);
		Print("[AZ_Vamp] TEST tz bite setup " + step + " moon elev " + TzMoonElev());
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TzBiteShot, 2600, false, p, step);
		return 9500;
	}

	void TzBiteBleed(PlayerBase v)
	{
		if (v && v.GetBleedingManagerServer())
			Print("[AZ_Vamp] TEST tz bite neck bleed " + v.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("Neck"));
	}

	void TzBiteShot(PlayerBase p, int step)
	{
		if (!s_TzBiteEva || !s_TzHero)
			return;
		vector hm = TzMoonFlat();
		vector side = Vector(hm[2], 0, -hm[0]);
		vector eh = s_TzBiteEva.GetBonePositionWS(s_TzBiteEva.GetBoneIndexByName("Head"));
		vector vh = s_TzHero.GetBonePositionWS(s_TzHero.GetBoneIndexByName("Head"));
		vector bite = (eh + vh) * 0.5;
		float up = Math.Min(TzMoonElev() * 0.5, 14.0) * Math.DEG2RAD;
		vector aim = hm * Math.Cos(up) + Vector(0, Math.Sin(up), 0);
		vector cam, look;
		string lab = "tz_bite_tight";
		float fov = 0.6;
		if (step == 118)
		{
			cam = bite - hm * 1.5 + side * 0.55 - "0 0.1 0";
			look = cam + aim * 1.5;
		}
		if (step == 119)
		{
			lab = "tz_bite_med";
			fov = 0.85;
			cam = bite - hm * 2.8 + side * 0.9 - "0 0.35 0";
			look = cam + aim * 2.8;
		}
		if (step == 148)
		{
			lab = "tz_bite_over";
			fov = 0.55;
			cam = vh - side * 0.42 - hm * 0.22 + "0 0.06 0";
			look = eh;
			fov = 0.4;
		}

		TzShot(p, lab, cam, bite, cam + (look - cam).Normalized() * 0.12, look, 4.5, fov);
	}


	void TzMoonSky(bool on)
	{
		Weather w = GetGame().GetWeather();
		w.GetOvercast().SetLimits(0, 1);
		if (on)
		{
			w.GetOvercast().Set(0.0, 0, 900);
			w.GetFog().Set(0.03, 0, 900);
		}
		else
		{
			w.GetOvercast().Set(TZ_OVERCAST, 0, 900);
			w.GetFog().Set(TZ_FOG, 0, 900);
		}
		Print("[AZ_Vamp] TEST tz moon sky " + on);
	}


	vector TzMoonUp()
	{
		vector md = GetGame().GetWorld().GetSunOrMoonDirection();
		vector up = md * -1;
		if (up[1] < 0)
			up = md;
		return up;
	}

	vector TzMoonFlat()
	{
		vector h = TzMoonUp();
		h[1] = 0;
		if (h.Length() < 0.1)
			return s_TzVDir;
		return h.Normalized();
	}

	float TzMoonElev()
	{
		vector up = TzMoonUp();
		return Math.Asin(Math.Clamp(up[1], -1, 1)) * Math.RAD2DEG;
	}


	void TzPut(PlayerBase p, vector at, vector look)
	{
		p.SetPosition(at);
		vector d = look - at;
		d[1] = 0;
		p.SetDirection(d.Normalized());
	}



	static int s_TzCast;
	static const int TZ_FACES = 12;

	string TzFace(int i)
	{
		string faces[12] = {"SurvivorM_Boris", "SurvivorF_Linda", "SurvivorM_Denis", "SurvivorF_Irena", "SurvivorM_Elias", "SurvivorF_Frida",
			"SurvivorM_Kaito", "SurvivorF_Gabi", "SurvivorM_Rolf", "SurvivorF_Naomi", "SurvivorM_Peter", "SurvivorF_Helga"};
		return faces[i % TZ_FACES];
	}


	static const int TZ_OUTFITS = 8;
	void TzOutfit(int i, out string jacket, out string pants, out string boots, out string head, out string back, out string extra)
	{
		switch (i % TZ_OUTFITS)
		{
			case 0: jacket = "QuiltedJacket_Grey"; pants = "CargoPants_Grey"; boots = "WorkingBoots_Grey"; head = "BeanieHat_Grey"; back = "TaloonBag_Green"; extra = "light"; break;
			case 1: jacket = "HuntingJacket_Brown"; pants = "HunterPants_Brown"; boots = "HikingBoots_Brown"; head = "Ushanka_Green"; back = "HuntingBag"; extra = "rifle"; break;
			case 2: jacket = "WoolCoat_BrownCheck"; pants = "Jeans_Grey"; boots = "Wellies_Black"; head = "Ushanka_Black"; back = "ChildBag_Red"; extra = ""; break;
			case 3: jacket = "M65Jacket_Olive"; pants = "CargoPants_Green"; boots = "MilitaryBoots_Brown"; head = "BeanieHat_Green"; back = "AssaultBag_Ttsko"; extra = "rifle"; break;
			case 4: jacket = "DownJacket_Blue"; pants = "CanvasPants_Grey"; boots = "HikingBootsLow_Black"; head = "BeanieHat_Black"; back = "TaloonBag_Green"; extra = "light"; break;
			case 5: jacket = "DownJacket_Green"; pants = "Breeches_Browncheck"; boots = "CombatBoots_Brown"; head = "BeanieHat_Grey"; back = "HuntingBag"; extra = ""; break;
			case 6: jacket = "GorkaEJacket_Autumn"; pants = "GorkaPants_Autumn"; boots = "MilitaryBoots_Brown"; head = "Ushanka_Blue"; back = "AssaultBag_Ttsko"; extra = "rifle"; break;
			default: jacket = "DownJacket_Red"; pants = "Jeans_Black"; boots = "HikingBootsLow_Black"; head = "Ushanka_Green"; back = "ChildBag_Red"; extra = "light"; break;
		}
	}


	void TzWear(EntityAI owner, string cls)
	{
		if (!owner || cls == "")
			return;
		EntityAI it = owner.GetInventory().CreateAttachment(cls);
		if (it)
			it.SetHealth01("", "", 0.6);
		else
			Print("[AZ_Vamp] TEST tz outfit: no slot for " + cls + " on " + owner.GetType());
	}

	void TzDress(PlayerBase h, int i)
	{
		string jacket, pants, boots, head, back, extra;
		TzOutfit(i, jacket, pants, boots, head, back, extra);
		TzWear(h, jacket);
		TzWear(h, pants);
		TzWear(h, boots);
		TzWear(h, head);
		TzWear(h, back);
		if (extra == "rifle")
			TzWear(h, "Mosin9130");
		if (extra == "light" && false)
		{
			EntityAI fl = h.GetHumanInventory().CreateInHands("Flashlight");
			if (fl)
			{
				fl.GetInventory().CreateAttachment("Battery9V");
				if (fl.GetCompEM())
					fl.GetCompEM().SwitchOn();
			}
		}
	}



	void TzStrip(PlayerBase v)
	{
		array<string> slots = {"Body", "Legs", "Feet", "Headgear"};
		foreach (string sl : slots)
		{
			EntityAI it = v.FindAttachmentBySlotName(sl);
			if (it)
				GetGame().ObjectDelete(it);
		}
	}




	void TzDressVampire(PlayerBase v, bool female)
	{
		if (!v)
			return;

		string want = "AZ_VampElder_GraveGown";
		if (female && v.GetType() == "SurvivorF_Eva")
			want = "AZ_VampElder_VelvetGown";
		else if (!female)
			want = "AZ_VampElder_GreatCoat";
		EntityAI worn = v.FindAttachmentBySlotName("Body");
		if (worn && worn.GetType() == want)
			return;
		TzStrip(v);
		if (female && v.GetType() == "SurvivorF_Eva")
		{
			TzWear(v, "AZ_VampElder_VelvetGown");
		}
		else if (female)
		{
			TzWear(v, "AZ_VampElder_GraveGown");
		}
		else
		{
			TzWear(v, "AZ_VampElder_GreatCoat");
			TzWear(v, "SlacksPants_Black");
		}
		TzWear(v, "MilitaryBoots_Black");
		Print("[AZ_Vamp] TEST tz dressed elder " + v.GetType() + " " + female);
	}



	void TzDressCommonVamp(PlayerBase v)
	{
		if (!v)
			return;
		TzStrip(v);
		TzWear(v, "QuiltedJacket_Black");
		TzWear(v, "CargoPants_Black");
		TzWear(v, "WorkingBoots_Brown");
		TzWear(v, "BeanieHat_Black");
		Print("[AZ_Vamp] TEST tz dressed common vampire " + v.GetType());
	}

	PlayerBase TzHuman(vector at, vector faceDir)
	{
		int k = s_TzCast;
		s_TzCast++;
		PlayerBase h = PlayerBase.Cast(GetGame().CreateObject(TzFace(k), at, false, true));
		if (!h)
			return null;
		h.SetPosition(at);
		h.SetDirection(faceDir);
		TzDress(h, k * 3 + 1);
		if (!s_TzActors)
			s_TzActors = new array<Object>;
		s_TzActors.Insert(h);
		return h;
	}


	AZ_VampRitualSite TzSite()
	{
		foreach (AZ_VampRitualSite s : AZ_VampRitual.s_Sites)
		{
			if (s && s.m_InCrypt)
				return s;
		}
		return null;
	}


	void TzGesture(PlayerBase d, int cmd)
	{
		if (!d)
			return;
		int tzStances = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		if (cmd == AZ_VampFeedAnim.CMD_VAMP || cmd == AZ_VampRear.CMD_VAMP)
			tzStances = DayZPlayerConstants.STANCEMASK_ERECT;
		HumanCommandActionCallback cb = d.StartCommand_Action(cmd, AZ_VampVictimCB, tzStances);
		AZ_VampVictimCB vcb = AZ_VampVictimCB.Cast(cb);
		if (vcb)
			vcb.Init(60000);
		Print("[AZ_Vamp] TEST tz gesture " + cmd + " on " + d + " cb " + cb);
	}

	void TzClearActors()
	{
		if (!s_TzActors)
			return;
		foreach (Object o : s_TzActors)
		{
			if (o)
				GetGame().ObjectDelete(o);
		}
		s_TzActors.Clear();
	}

	void TzBloodMoon(PlayerBase p, bool on)
	{
		AZ_VampProgress.s_BloodMoon = on;
		p.m_AZBloodMoon = on;
		p.SetSynchDirty();
	}




	static const float TZ_STREET_X = 8424.6;
	static const float TZ_STREET_Z = 10734.8;



	void TzPickStreet()
	{
		vector c = Vector(TZ_STREET_X, 0, TZ_STREET_Z);
		c[1] = GetGame().SurfaceY(c[0], c[2]);
		array<vector> lps = new array<vector>;
		if (AZ_VampDecor.s_Lanterns)
		{
			foreach (Object ln : AZ_VampDecor.s_Lanterns)
			{
				if (ln && TzDistXZ(ln.GetPosition(), c) < 200)
					lps.Insert(ln.GetPosition());
			}
		}
		float best = -1;
		vector bestAt = c;
		vector bestDir = "1 0 0";
		float bestRoad = -1;
		vector roadAt = c;
		vector roadDir = "1 0 0";
		TzSurfaces(c);
		for (int i = 0; i < lps.Count(); i++)
		{
			for (int j = 0; j < lps.Count(); j++)
			{
				float len = TzDistXZ(lps[i], lps[j]);
				if (i == j || len < 15 || len > 35)
					continue;
				vector d = lps[j] - lps[i];
				d[1] = 0;
				d.Normalize();
				vector side = Vector(d[2], 0, -d[0]);
				for (int s = -1; s <= 1; s += 2)
				{
					vector from = lps[i] + side * (3.0 * s) - d * 3.0;
					from[1] = GetGame().SurfaceY(from[0], from[2]) + 1.2;
					vector to = from + d * 30.0;
					to[1] = GetGame().SurfaceY(to[0], to[2]) + 1.2;
					vector hit, nrm;
					int comp;
					set<Object> res = new set<Object>;
					if (DayZPhysics.RaycastRV(from, to, hit, nrm, comp, res, null, null, false, false, ObjIntersectView, 0.4))
					{
						if (vector.Distance(from, hit) < 28)
							continue;
					}
					float score = 0;
					foreach (vector lp : lps)
					{
						vector rel = lp - from;
						rel[1] = 0;
						float along = vector.Dot(rel, d);
						vector perp = rel - d * along;
						if (along > -3 && along < 33 && perp.Length() < 7)
							score += 1;
					}
					int road = TzRoad(from, d);
					if (road >= 3 && score > bestRoad)
					{
						bestRoad = score;
						roadAt = from;
						roadDir = d;
					}
					if (score > best)
					{
						best = score;
						bestAt = from;
						bestDir = d;
					}
				}
			}
		}
		if (bestRoad >= 0)
		{
			best = bestRoad;
			bestAt = roadAt;
			bestDir = roadDir;
		}
		else
			Print("[AZ_Vamp] TEST tz street: no line on a road, the best pumpkin line is used");
		bestAt[1] = GetGame().SurfaceY(bestAt[0], bestAt[2]);
		s_TzChainDir = bestDir;
		s_TzChainAt = bestAt;
		string surfs = "";
		for (int k = 0; k < 5; k++)
		{
			vector sp = bestAt + bestDir * (k * 6.0);
			string sf;
			GetGame().SurfaceGetType(sp[0], sp[2], sf);
			surfs += " " + sf;
		}
		Print("[AZ_Vamp] TEST tz street: score " + best + " (pumpkins + road) of " + lps.Count() + " Jalovisko pumpkins, from " + s_TzChainAt + " dir " + s_TzChainDir + " surfaces" + surfs);
	}

	float TzDistXZ(vector a, vector b)
	{
		vector d = a - b;
		d[1] = 0;
		return d.Length();
	}


	void TzSurfaces(vector c)
	{
		map<string, int> n = new map<string, int>;
		for (float x = -60; x <= 60; x += 6)
		{
			for (float z = -60; z <= 60; z += 6)
			{
				string sf;
				GetGame().SurfaceGetType(c[0] + x, c[2] + z, sf);
				n.Set(sf, n.Get(sf) + 1);
			}
		}
		string txt = "";
		foreach (string k, int v : n)
			txt += " " + k + "=" + v;
		Print("[AZ_Vamp] TEST tz surfaces round Jalovisko:" + txt);
	}


	int TzRoad(vector from, vector d)
	{
		int n = 0;
		for (int k = 0; k < 5; k++)
		{
			vector sp = from + d * (k * 6.0);
			string sf;
			float gy = GetGame().SurfaceY(sp[0], sp[2]);
			GetGame().SurfaceGetType3D(sp[0], gy + 0.5, sp[2], sf);
			sf.ToLower();
			if (sf.Contains("asphalt") || sf.Contains("road") || sf.Contains("concrete") || sf.Contains("gravel") || sf.Contains("cobble") || sf.Contains("dirt") || sf.Contains("path") || sf.Contains("pavement") || sf.Contains("stone") || sf.Contains("tarmac"))
				n++;
		}
		return n;
	}



	static const float TZ_FOG = 0.12;

	static const float TZ_OVERCAST = 0.6;




	static int s_TzDay;
	void TzNight()
	{
		World w = GetGame().GetWorld();
		if (s_TzDay == 0)
		{
			float lo = 2.0;
			float hi = -1.0;
			int loDay = 25;
			string txt = "";
			for (int day = 18; day <= 30; day++)
			{
				w.SetDate(2011, 11, day, 22, 0);
				float mi = w.GetMoonIntensity();
				txt += " " + day + "=" + mi;
				if (mi < lo)
				{
					lo = mi;
					loDay = day;
				}
				if (mi > hi)
					hi = mi;
			}
			s_TzDay = loDay;
			if (hi - lo < 0.01)
				s_TzDay = 25;
			Print("[AZ_Vamp] TEST tz dark moon: Nov " + s_TzDay + " of" + txt);
		}
		w.SetDate(2011, 11, s_TzDay, 22, 0);
	}


	void TzLightFire(Fireplace fire)
	{
		if (!fire)
			return;
		AZ_VampLandmarks.Fuel(fire);
		if (!fire.IsBurning())
		{
			if (!fire.GetInventory().FindAttachment(InventorySlots.GetSlotIdFromString("Paper")))
				fire.GetInventory().CreateAttachment("Paper");
			fire.StartFire(true);
		}
		Print("[AZ_Vamp] TEST tz fire at " + fire.GetPosition() + " burning " + fire.IsBurning());
	}

	void TzLogWeather()
	{
		Weather w = GetGame().GetWeather();
		Print("[AZ_Vamp] TEST tz weather overcast " + w.GetOvercast().GetActual() + " fog " + w.GetFog().GetActual() + " rain " + w.GetRain().GetActual());
	}

	void TzClearMist()
	{
		Fog fog = GetGame().GetWeather().GetFog();
		fog.SetLimits(0, 1);
		fog.Set(TZ_FOG, 0, 900);
		GetGame().GetWeather().GetOvercast().SetLimits(0, 1);
		GetGame().GetWeather().GetOvercast().Set(TZ_OVERCAST, 0, 900);
		GetGame().GetWeather().GetRain().Set(0, 0, 900);

		AZ_VampMist.s_On = true;
		AZ_VampMist.s_NextAt = GetGame().GetTime() + 600000;
		Print("[AZ_Vamp] TEST tz mist held off, fog " + fog.GetActual());
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TzLogWeather, 2000, false);
	}


	void TzChainSetup(PlayerBase p)
	{
		TzClearActors();
		TzClearMist();
		TzNight();
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TzLogDate, 1500, false, "chain");
		TzBloodMoon(p, true);
		p.SetHealth("", "Blood", AZ_VampState.Cfg().fullBlood);
		TzDressVampire(p, true);

		AZ_VampShadowState st = AZ_VampShadow.State(p);
		st.chain = 0;
		st.chainUntil = 0;
		st.coolUntil = 0;
		st.busyUntil = 0;
		st.compelUntil = 0;
		st.compelling = false;
		st.bitten.Clear();
		vector at = s_TzChainAt;
		at[1] = GetGame().SurfaceY(at[0], at[2]);
		HwGo(p, at, at + s_TzChainDir);
		for (int i = 0; i < 5; i++)
		{
			vector h = at + s_TzChainDir * (7.0 + i * 4.0);
			h[1] = GetGame().SurfaceY(h[0], h[2]);
			TzHuman(h, s_TzChainDir);
		}
	}


	void TzChainFire(PlayerBase p, int left)
	{
		if (!p || left <= 0)
			return;
		bool stepped = AZ_VampShadow.TryStep(p);
		Print("[AZ_Vamp] TEST tz chain step " + stepped + " chain " + AZ_VampShadow.State(p).chain);
		int gap = Math.Round(AZ_VampState.Cfg().chainBiteSec * 1000 + 300);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TzChainFire, gap, false, p, left - 1);
	}

	void TzKnockOut(PlayerBase v)
	{
		if (v)
			v.SetHealth("", "Shock", 0);
	}

	void TzBloodMoonOn(PlayerBase p)
	{
		TzBloodMoon(p, true);
		Print("[AZ_Vamp] TEST tz blood moon on");
	}

	void TzFangsKey(PlayerBase p)
	{
		GetGame().RPCSingleParam(p, AZ_VampToggle.RPC_FANGS_TEST, new Param1<bool>(true), true, p.GetIdentity());
	}

	void TzLogFace(PlayerBase p, string why)
	{
		if (!p)
			return;
		EntityAI mask = p.FindAttachmentBySlotName("Mask");
		string m = "none";
		if (mask)
			m = mask.GetType();
		Print("[AZ_Vamp] TEST tz thumb face " + why + ": mask " + m + " vampire " + p.m_AZVampire + " frenzy left " + (p.m_AZVampFrenzyUntil - GetGame().GetTime()) + " ms, blood moon " + p.m_AZBloodMoon);
	}

	void TzElderGone(PlayerBase elder)
	{
		if (!elder)
			return;
		vector at = elder.GetPosition();
		GetGame().RPCSingleParam(elder, AZ_VampShadow.RPC_SHADOW, new Param1<vector>(at), true);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(GetGame().ObjectDelete, 400, false, elder);
	}

	void TzDrainDry(PlayerBase p, PlayerBase victim)
	{
		AZ_VampFeed.DrainedToDeath(p, victim);
		Print("[AZ_Vamp] TEST tz drained dry, ghoul in " + AZ_VampState.Cfg().ghoulRiseSec + " s");
	}

	static vector s_TzVDir = "1 0 0";
	static vector s_TzFireAt;
	static vector s_TzE1, s_TzE2;
	static PlayerBase s_TzHero;



	void TzHero(vector at, vector faceDir, bool torch)
	{
		if (s_TzHero)
			GetGame().ObjectDelete(s_TzHero);
		at[1] = GetGame().SurfaceY(at[0], at[2]);
		faceDir[1] = 0;
		s_TzHero = PlayerBase.Cast(GetGame().CreateObject("SurvivorM_Peter", at, false, true));
		if (!s_TzHero)
			return;
		s_TzHero.SetPosition(at);
		s_TzHero.SetDirection(faceDir.Normalized());
		string jacket, pants, boots, head, back, extra;
		TzOutfit(0, jacket, pants, boots, head, back, extra);
		TzWear(s_TzHero, jacket);
		TzWear(s_TzHero, pants);
		TzWear(s_TzHero, boots);
		TzWear(s_TzHero, head);
		TzWear(s_TzHero, back);
		if (torch)
		{
			ItemBase tr = ItemBase.Cast(s_TzHero.GetHumanInventory().CreateInHands("Torch"));
			if (tr)
			{
				tr.GetInventory().CreateAttachment("Rag");
				if (tr.GetCompEM())
				{
					tr.GetCompEM().SetEnergy(tr.GetCompEM().GetEnergyMax());
					tr.GetCompEM().SwitchOn();
				}
				if (!tr.IsIgnited())
					tr.OnIgnitedThis(null);
				Print("[AZ_Vamp] TEST tz hero torch lit " + tr.IsIgnited());
			}
		}
		if (!s_TzActors)
			s_TzActors = new array<Object>;
		s_TzActors.Insert(s_TzHero);
		Print("[AZ_Vamp] TEST tz hero SurvivorM_Peter outfit 0 " + jacket + " " + pants + " " + head + " torch " + torch);
	}
	static vector s_TzSleeperAt;


	Fireplace TzFire()
	{
		if (!AZ_VampLandmarks.s_Fires)
			return null;
		vector c = Vector(TZ_STREET_X, 0, TZ_STREET_Z);
		Fireplace best;
		float bd = 1000000000.0;
		foreach (Fireplace f : AZ_VampLandmarks.s_Fires)
		{
			if (!f)
				continue;
			vector fpos = f.GetPosition();
			vector dd = fpos - c;
			dd[1] = 0;
			if (dd.Length() < bd)
			{
				bd = dd.Length();
				best = f;
			}
		}
		if (!best)
			return null;

		vector bp = best.GetPosition();
		float most = -1;
		for (int i = 0; i < 12; i++)
		{
			float a = i * Math.PI2 / 12;
			vector d = Vector(Math.Sin(a), 0, Math.Cos(a));
			vector from = bp + d * 2.0 + "0 1.5 0";
			vector to = bp + d * 16.0;
			to[1] = GetGame().SurfaceY(to[0], to[2]) + 1.5;
			float free = 14.0;
			if (!AZ_TzView.Clear(from, to))
				free = 0;
			if (free > most)
			{
				most = free;
				s_TzVDir = d;
			}
		}
		return best;
	}



	PlayerBase TzEva(vector at, vector faceTo)
	{
		vector d = faceTo - at;
		d[1] = 0;
		PlayerBase e = AZ_VampEvent.AIVampire("SurvivorF_Eva", at, d.Normalized());
		if (!e)
		{
			Print("[AZ_Vamp] TEST tz eva spawn failed at " + at);
			return null;
		}
		TzDressVampire(e, true);
		if (!s_TzActors)
			s_TzActors = new array<Object>;
		s_TzActors.Insert(e);
		Print("[AZ_Vamp] TEST tz eva at " + at);
		return e;
	}


	string TzFeedEva(PlayerBase victim)
	{
		vector fwd = victim.GetDirection();
		fwd[1] = 0;
		fwd.Normalize();
		PlayerBase front = TzEva(victim.GetPosition() + fwd * AZ_VampFeed.ROOT_GAP, victim.GetPosition());
		PlayerBase back = AZ_VampEvent.AIVampire(AZ_VampDress.Male(Math.RandomInt(0, 1000)), victim.GetPosition() - fwd * AZ_VampFeed.ROOT_GAP, fwd);
		if (!front || !back)
			return "could not spawn the vampires";
		TzDressCommonVamp(back);
		s_TzActors.Insert(back);
		victim.m_AZVampRear = false;
		AZ_VampFeed.Hold(front, victim);
		AZ_VampFeed.Join(back, victim);
		AZ_VampFeedAnim.DummyFeedStart(front);
		AZ_VampFeedAnim.DummyFeedStart(back);
		int ms = victim.m_AZVampFloorAt - GetGame().GetTime() + 500;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(TzFeedEnd, ms, false, front, back, victim);
		return "Eva at his neck, a second one behind, " + (ms / 1000) + " s";
	}

	void TzFeedEnd(PlayerBase front, PlayerBase back, PlayerBase victim)
	{
		AZ_VampEvent.FeedOnEnd2(front, back, victim);
	}

	void TzElder(vector at, vector faceTo, bool female)
	{
		vector d = faceTo - at;
		d[1] = 0;
		int seed = Math.RandomInt(0, 1000);
		string cls = AZ_VampDress.Male(seed);
		if (female)
		{
			string blondes[2] = {"SurvivorF_Frida", "SurvivorF_Helga"};
			cls = blondes[seed % 2];
		}
		PlayerBase e = AZ_VampEvent.AIVampire(cls, at, d.Normalized());
		if (!e)
		{
			Print("[AZ_Vamp] TEST tz elder spawn failed at " + at);
			return;
		}
		TzDressVampire(e, female);
		if (!s_TzActors)
			s_TzActors = new array<Object>;
		s_TzActors.Insert(e);
	}


	static const vector TZ_CHURCH = "6885.9 0 10976.3";
	static vector s_TzDoorAt;
	static vector s_TzDoorOut;


	bool TzChurchDoor()
	{
		vector c = TZ_CHURCH;
		c[1] = GetGame().SurfaceY(c[0], c[2]);
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition3D(c, 45, objs, null);
		Building ch;
		foreach (Object o : objs)
		{
			Building b = Building.Cast(o);
			if (!b)
				continue;
			string t = b.GetType();
			t.ToLower();
			if (t.Contains("church") && b.GetDoorCount() > 0)
			{
				ch = b;
				break;
			}
		}


		if (!ch)
		{
			Object chs;
			foreach (Object o4 : objs)
			{
				if (!o4)
					continue;
				string t4 = o4.GetType() + " " + o4.GetShapeName();
				t4.ToLower();
				if (t4.Contains("church"))
				{
					chs = o4;
					break;
				}
			}
			if (chs)
			{
				vector cc = chs.GetPosition();
				vector gr = cc + "-60 0 0";
				float gd2 = 1000000.0;
				if (AZ_VampLandmarks.s_Graveyards)
				{
					foreach (vector g2 : AZ_VampLandmarks.s_Graveyards)
					{
						if (TzDistXZ(g2, cc) < gd2)
						{
							gd2 = TzDistXZ(g2, cc);
							gr = g2;
						}
					}
				}
				vector fo = gr - cc;
				fo[1] = 0;
				fo.Normalize();
				float gy = GetGame().SurfaceY(cc[0], cc[2]);
				vector rf = cc + fo * 25.0;
				rf[1] = gy + 1.2;
				vector rt = cc;
				rt[1] = gy + 1.2;
				vector rh, rn;
				int rc;
				set<Object> rres = new set<Object>;
				vector front = cc + fo * 6.0;
				if (DayZPhysics.RaycastRV(rf, rt, rh, rn, rc, rres, null, null, false, false, ObjIntersectGeom, 0.1))
					front = rh;
				front = front + fo * 0.4;
				front[1] = GetGame().SurfaceY(front[0], front[2]);
				s_TzDoorAt = front;
				s_TzDoorOut = fo;
				Print("[AZ_Vamp] TEST tz ld church front: " + chs.GetType() + " " + chs.GetShapeName() + " at " + cc + ", front " + front + " facing " + fo + " (graveyard " + gd2 + " m)");
				return true;
			}
		}


		if (!ch && AZ_VampLandmarks.s_Churches)
		{
			float cd2 = 1000000.0;
			foreach (Object lc : AZ_VampLandmarks.s_Churches)
			{
				Building lb = Building.Cast(lc);
				if (lb && lb.GetDoorCount() > 0 && TzDistXZ(lb.GetPosition(), c) < cd2)
				{
					cd2 = TzDistXZ(lb.GetPosition(), c);
					ch = lb;
				}
			}
			if (ch)
			{
				c = ch.GetPosition();
				Print("[AZ_Vamp] TEST tz ld church from the landmarks: " + ch.GetType() + " at " + c + ", " + cd2 + " m from the RECREATE point");
			}
		}
		if (!ch)
		{
			float hd = 1000000.0;
			foreach (Object o3 : objs)
			{
				Building b3 = Building.Cast(o3);
				if (b3 && b3.GetDoorCount() > 0 && TzDistXZ(b3.GetPosition(), c) < hd)
				{
					hd = TzDistXZ(b3.GetPosition(), c);
					ch = b3;
				}
			}
			if (ch)
				Print("[AZ_Vamp] TEST tz ld church fallback: the house " + ch.GetType() + " " + hd + " m from the point");
		}
		if (!ch)
		{
			string near = "";
			foreach (Object o2 : objs)
			{
				Building b2 = Building.Cast(o2);
				if (b2 && b2.GetDoorCount() > 0)
					near += " " + b2.GetType();
			}
			int nl = 0;
			if (AZ_VampLandmarks.s_Churches)
				nl = AZ_VampLandmarks.s_Churches.Count();
			Print("[AZ_Vamp] TEST tz ld church: none; landmark churches " + nl + "; buildings with doors near:" + near);
			return false;
		}
		vector grave = c + "0 0 -60";
		float gd = 1000000.0;
		if (AZ_VampLandmarks.s_Graveyards)
		{
			foreach (vector g : AZ_VampLandmarks.s_Graveyards)
			{
				if (TzDistXZ(g, c) < gd)
				{
					gd = TzDistXZ(g, c);
					grave = g;
				}
			}
		}
		int best = -1;
		float bd = 1000000.0;
		for (int i = 0; i < ch.GetDoorCount(); i++)
		{
			ch.UnlockDoor(i, false);
			ch.OpenDoor(i);
			float d = TzDistXZ(ch.GetDoorSoundPos(i), grave);
			if (d < bd)
			{
				bd = d;
				best = i;
			}
		}
		vector dp = ch.GetDoorSoundPos(best);
		vector dout = grave - dp;
		dout[1] = 0;
		dout.Normalize();
		s_TzDoorOut = dout;

		vector from = dp + "0 1.5 0";
		vector to = dp - "0 3 0";
		vector hit, nrm;
		int comp;
		set<Object> res = new set<Object>;
		if (DayZPhysics.RaycastRV(from, to, hit, nrm, comp, res, null, null, false, false, ObjIntersectGeom, 0.1))
			dp[1] = hit[1];
		else
			dp[1] = GetGame().SurfaceY(dp[0], dp[2]);
		s_TzDoorAt = dp;
		Print("[AZ_Vamp] TEST tz ld church " + ch.GetType() + " doors " + ch.GetDoorCount() + " threshold door " + best + " at " + dp + " out " + dout + " graveyard " + gd + " m");
		return true;
	}


	void TzFlareMan(vector at, vector faceDir)
	{
		TzHero(at, faceDir, false);
		if (!s_TzHero)
			return;
		EntityAI fl = s_TzHero.GetHumanInventory().CreateInHands("Roadflare");
		if (fl && fl.GetCompEM())
			fl.GetCompEM().SwitchOn();
		Print("[AZ_Vamp] TEST tz ld flare " + fl);
	}


	vector TzOutOfTrees(vector c, vector dir)
	{
		dir[1] = 0;
		dir.Normalize();
		for (int k = 0; k <= 10; k++)
		{
			vector q = c + dir * (k * 0.4);
			array<Object> near = new array<Object>;
			GetGame().GetObjectsAtPosition(q, 1.8, near, null);
			bool tree = false;
			foreach (Object o : near)
			{
				if (o && (o.IsTree() || o.IsBush()))
					tree = true;
			}
			if (!tree)
			{
				if (k > 0)
					Print("[AZ_Vamp] TEST tz camera moved " + (k * 0.4) + " m out of the trees");
				return q;
			}
		}
		Print("[AZ_Vamp] TEST tz camera: trees all along, kept");
		return c;
	}


	int TzStep(PlayerBase p, int step)
	{
		vector stand, cam, look, cam2, look2, at;
		switch (step)
		{
			case 100:
			{
				bool site = AZ_VampCrypt.Site();
				TzBloodMoon(p, false);
				TzNight();
				TzClearMist();
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TzLogDate, 1500, false, "start");
				TzPickStreet();
				TzLightFire(TzFire());



				vector prepAt = p.GetPosition();
				GetGame().RPCSingleParam(p, AZ_VampShotRpc.RPC, new Param3<string, vector, vector>("tz_prep#0#0 0 0#0 0 0#1", prepAt, prepAt), true, p.GetIdentity());
				Print("[AZ_Vamp] TEST tz start, crypt site " + site + " chain at " + s_TzChainAt + " dir " + s_TzChainDir);
				return 3000;
			}
			case 101:
				GetGame().GetWorld().SetDate(2011, 11, 5, 18, 30);
				AZ_VampCrypt.ShotSpot(0, stand, cam, look);
				HwGo(p, stand, look);
				return 9000;
			case 102:
				AZ_VampCrypt.ShotSpot(0, stand, cam, look);
				cam2 = cam + "0 0.6 0";
				TzShot(p, "tz_call", cam, look, cam2, look, 6);
				return 7000;
			case 103:
				TzNight();
				stand = AZ_VampCrypt.Hall(-3.0, 0.6, 0.1);
				look = AZ_VampCrypt.Hall(3.0, 0, 0.5);
				TzPut(p, stand, look);
				return 9000;
			case 104:
				cam = AZ_VampCrypt.Hall(-7.0, -0.5, 1.4);
				look = AZ_VampCrypt.Hall(2.0, 0, 0.8);
				cam2 = AZ_VampCrypt.Hall(-2.0, 0, 1.1);
				look2 = AZ_VampCrypt.Hall(6.0, 0, -1.0);
				TzShot(p, "tz_descent", cam, look, cam2, look2, 6);
				return 7000;


			case 140:
				TzClearActors();
				TzNight();
				AZ_VampCandleHall.s_ReadyAt = 0;
				if (AZ_VampCandleHall.s_Next)
					AZ_VampCandleHall.s_Next.Clear();
				stand = AZ_VampCrypt.Hall(-16.2, 4.8, 0.05);
				look = AZ_VampCrypt.Hall(-19.0, -2.0, 0.8);
				TzPut(p, stand, look);
				return 6000;
			case 141:
				cam = AZ_VampCrypt.Hall(-16.3, 4.6, 1.8);
				cam2 = AZ_VampCrypt.Hall(-17.1, 4.0, 1.6);
				look = AZ_VampCrypt.Hall(-19.0, -2.0, 0.8);
				look2 = AZ_VampCrypt.Hall(-19.6, -3.0, 0.9);
				TzShot(p, "tz_candles", cam, look, cam2, look2, 7);
				vector inStretch = AZ_VampCrypt.Hall(-18.0, 3.0, 0.05);
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(p.SetPosition, 3500, false, inStretch);
				return 8000;
			case 142:
				TzClearActors();
				AZ_VampCryptTraps.s_AlarmReadyAt = GetGame().GetTime() + 60000;

				stand = AZ_VampCrypt.Hall(-1.2, -11.6, 1.0);
				look = AZ_VampCrypt.Hall(-6.3, -12.65, 0.8);
				TzPut(p, stand, look);
				return 6000;
			case 143:
				cam = AZ_VampCrypt.Hall(-3.4, -11.4, 1.6);
				cam2 = AZ_VampCrypt.Hall(-4.3, -11.8, 1.3);
				look = AZ_VampCrypt.Hall(-6.3, -12.65, 0.8);
				look2 = AZ_VampCrypt.Hall(-6.6, -12.8, 0.6);
				TzShot(p, "tz_sleepers", cam, look, cam2, look2, 7);
				return 8000;
			case 144:
			{
				TzClearActors();
				AZ_VampCryptTraps.s_AlarmReadyAt = GetGame().GetTime() + 60000;
				AZ_VampCryptCage.ForceArm();
				stand = AZ_VampCrypt.Hall(-11.4, -1.6, 0.05);
				look = AZ_VampCrypt.Hall(AZ_VampCryptCage.CAGE_F, AZ_VampCryptCage.CAGE_R, 0.8);
				TzPut(p, stand, look);
				vector victim = AZ_VampCrypt.Hall(AZ_VampCryptCage.CAGE_F, AZ_VampCryptCage.CAGE_R, 0.05);
				vector vdir = stand - victim;
				vdir[1] = 0;
				TzHuman(victim, vdir.Normalized());
				return 6000;
			}
			case 145:
				cam = AZ_VampCrypt.Hall(-12.0, -0.4, 2.0);
				cam2 = AZ_VampCrypt.Hall(-12.2, -0.3, 1.8);
				look = AZ_VampCrypt.Hall(AZ_VampCryptCage.CAGE_F, AZ_VampCryptCage.CAGE_R + 0.6, 1.0);
				look2 = AZ_VampCrypt.Hall(AZ_VampCryptCage.CAGE_F, AZ_VampCryptCage.CAGE_R + 0.6, 0.8);
				TzShot(p, "tz_cage", cam, look, cam2, look2, 6);
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(AZ_VampCryptCage.ForceDown, 1800, false);
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(AZ_VampCryptCage.ForceArm, 9000, false);
				return 9500;
			case 146:
				TzClearActors();
				AZ_VampCryptTraps.ForceArm();
				AZ_VampCryptTraps.s_AlarmReadyAt = GetGame().GetTime() + 60000;
				AZ_VampCryptTraps.s_WedgedUntil = GetGame().GetTime() + 60000;
				stand = AZ_VampCrypt.Hall(-13.4, -0.5, 0.05);
				look = AZ_VampCrypt.Hall(-7.5, AZ_VampCryptTraps.CW_R, 1.9);
				TzPut(p, stand, look);
				return 6000;
			case 147:
			{
				cam = AZ_VampCrypt.Hall(-12.1, -0.6, 1.7);
				cam2 = AZ_VampCrypt.Hall(-12.4, -0.3, 1.5);
				look = AZ_VampCrypt.Hall(-7.5, AZ_VampCryptTraps.CW_R, 1.9);
				float endF = AZ_VampCryptTraps.BallF(2500);
				float endY = AZ_VampCryptTraps.BallY(endF);
				look2 = AZ_VampCrypt.Hall(endF, AZ_VampCryptTraps.CW_R, endY + 0.5);

				float rollDur = AZ_VampCryptTraps.RUMBLE_MS / 1000.0 + 3.0;
				AZ_VampCryptTraps.Release("tour");
				TzShot(p, "tz_boulder", cam, look, cam2, look2, rollDur);
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(AZ_VampCryptTraps.ForceArm, 9500, false);
				return 10000;
			}
			case 105:


				TzClearActors();
				TzBloodMoon(p, false);
				TzPut(p, AZ_VampCrypt.Nave(0.4, 0, 0.05), AZ_VampCrypt.Nave(AZ_VampCrypt.CountessF(), 0, 0.05));
				return 8000;
			case 106:


				cam = AZ_VampCrypt.Nave(2.0, 0, 1.65);
				cam2 = AZ_VampCrypt.Nave(2.4, 0, 1.65);
				look = AZ_VampCrypt.Nave(AZ_VampCrypt.CountessF(), 0, 3.4);
				TzShot(p, "tz_ledge", cam, look, cam2, look, 7, 0.46);
				return 8000;


			case 175:
				TzClearActors();
				TzBloodMoon(p, false);
				TzPut(p, AZ_VampCrypt.Nave(0.4, 0, 0.05), AZ_VampCrypt.Nave(AZ_VampCrypt.CountessF(), 0, 0.05));
				return 6000;
			case 176:
			{
				float rpF = AZ_VampCrypt.CountessF();
				float rpFace = AZ_VampCrypt.CountessFeet() + 1.93 * AZ_VampCrypt.CountessScale();
				cam = AZ_VampCrypt.Nave(2.2, 0, 1.65);
				look = AZ_VampCrypt.Nave(rpF, 0, 3.4);


				cam2 = AZ_VampCrypt.Nave(rpF - 8.0, 0, rpFace - 0.6);
				look2 = AZ_VampCrypt.Nave(rpF, 0, rpFace - 0.2);
				TzShot(p, "tz_reveal_push", cam, look, cam2, look2, 12, 0.45);
				return 13000;
			}
			case 107:
				AZ_VampCrypt.ShotSpot(3, stand, cam, look);
				TzPut(p, stand, look);
				return 8000;
			case 108:




				{
					float sf = AZ_VampCrypt.CountessF();
					float sft = AZ_VampCrypt.CountessFeet();
					float ssc = AZ_VampCrypt.CountessScale();
					cam = AZ_VampCrypt.Nave(AZ_VampCrypt.APSE_F + 0.6, 0.6, 2.2);
					cam2 = AZ_VampCrypt.Nave(AZ_VampCrypt.APSE_F + 3.4, 0.3, 2.7);
					look = AZ_VampCrypt.Nave(sf, 0, sft + 1.27 * ssc);
					look2 = AZ_VampCrypt.Nave(sf, 0, sft + 1.93 * ssc);
				}
				TzShot(p, "tz_statue", cam, look, cam2, look2, 7, 0.65);
				return 8000;
			case 109:
			{
				TzClearActors();
				AZ_VampCrypt.ShotSpot(2, stand, cam, look);
				TzPut(p, stand, look);
				vector toNave = AZ_VampCrypt.Nave(0, 0, 0);
				vector naveFar = AZ_VampCrypt.Nave(10, 0, 0);
				toNave = toNave - naveFar;
				toNave[1] = 0;
				toNave.Normalize();
				vector toAltar = toNave * -1;
				AZ_VampRitualSite rs = TzSite();
				if (rs)
				{

					PlayerBase lier = TzHuman(rs.m_Lie, rs.m_LieDir);
					TzGesture(lier, DayZPlayerConstants.CMD_GESTUREFB_LYINGDOWN);
					for (int kn = 0; kn < rs.m_Kneel.Count() && kn < 3; kn++)
					{
						vector kd = rs.m_Center - rs.m_Kneel[kn];
						kd[1] = 0;
						PlayerBase kneeler = TzHuman(rs.m_Kneel[kn], kd.Normalized());
						TzGesture(kneeler, DayZPlayerConstants.CMD_GESTUREFB_KNEEL);
					}
					foreach (AZ_VampRitualCandle cdl : rs.m_Candles)
					{
						if (cdl)
							cdl.AZ_SetLit(true);
					}
					if (rs.m_Candles.Count() > 0 && rs.m_Candles[0])
						rs.m_Candles[0].AZ_SetMusic(1 | 2 | 4);
					Print("[AZ_Vamp] TEST tz ritual on the site: " + rs.m_Kneel.Count() + " kneel spots, " + rs.m_Candles.Count() + " candles");
					return 8000;
				}
				at = AZ_VampCrypt.Nave(AZ_VampCrypt.ALTAR_F, 0, AZ_VampCrypt.DAIS_Y);
				TzHuman(at, toNave);
				at = AZ_VampCrypt.Nave(AZ_VampCrypt.ALTAR_F - 1.8, -1.6, AZ_VampCrypt.DAIS_Y);
				TzHuman(at, toAltar);
				at = AZ_VampCrypt.Nave(AZ_VampCrypt.ALTAR_F - 1.8, 1.6, AZ_VampCrypt.DAIS_Y);
				TzHuman(at, toAltar);
				at = AZ_VampCrypt.Nave(AZ_VampCrypt.ALTAR_F - 2.4, 0, AZ_VampCrypt.DAIS_Y);
				TzHuman(at, toAltar);
				return 8000;
			}
			case 110:
				cam = AZ_VampCrypt.Nave(AZ_VampCrypt.ALTAR_F - 13.0, 0, 1.8);
				cam2 = AZ_VampCrypt.Nave(AZ_VampCrypt.ALTAR_F - 10.5, 0, 1.6);
				look = AZ_VampCrypt.Nave(AZ_VampCrypt.ALTAR_F, 0, 1.5);
				TzShot(p, "tz_ritual", cam, look, cam2, look, 9);
				return 10000;
			case 111:
				look = AZ_VampCrypt.Nave(AZ_VampCrypt.ALTAR_F, 0, AZ_VampCrypt.DAIS_Y);
				AZ_VampCrypt.StatueEyes(look, true);
				AZ_VampRitualSite rs2 = TzSite();
				if (rs2 && rs2.m_Candles.Count() > 0 && rs2.m_Candles[0])
					rs2.m_Candles[0].AZ_SetMusic(1 | 2 | 4 | 8);
				return 1000;
			case 112:
				cam = AZ_VampCrypt.Nave(AZ_VampCrypt.ALTAR_F - 9.0, 0, 1.6);
				cam2 = AZ_VampCrypt.Nave(AZ_VampCrypt.ALTAR_F - 6.0, 0, 1.8);
				look = AZ_VampCrypt.Nave(AZ_VampCrypt.APSE_F + 8.6, 0, AZ_VampCrypt.DAIS_Y + 3.9);
				TzShot(p, "tz_eyes", cam, look, cam2, look, 3);
				return 4000;
			case 113:
				cam = AZ_VampCrypt.Nave(AZ_VampCrypt.APSE_F + 6.8, 0, AZ_VampCrypt.DAIS_Y + 4.1);
				cam2 = AZ_VampCrypt.Nave(AZ_VampCrypt.APSE_F + 7.2, 0, AZ_VampCrypt.DAIS_Y + 4.1);
				look = AZ_VampCrypt.Nave(AZ_VampCrypt.APSE_F + 8.6, 0, AZ_VampCrypt.DAIS_Y + 4.1);
				TzShot(p, "tz_eyes_ecu", cam, look, cam2, look, 4, 0.5);
				return 5000;
			case 114:
			{
				TzClearActors();
				GetGame().RPCSingleParam(p, AZ_VampCrypt.RPC_FADE, new Param1<int>(1000), true, p.GetIdentity());
				AZ_VampRitualSite rs3 = TzSite();
				if (rs3)
				{
					foreach (AZ_VampRitualCandle c3 : rs3.m_Candles)
					{
						if (c3)
							c3.AZ_SetLit(false);
					}
					if (rs3.m_Candles.Count() > 0 && rs3.m_Candles[0])
						rs3.m_Candles[0].AZ_SetMusic(0);
				}
				look = AZ_VampCrypt.Nave(AZ_VampCrypt.ALTAR_F, 0, AZ_VampCrypt.DAIS_Y);
				AZ_VampCrypt.StatueEyes(look, false);
				at = AZ_VampCrypt.Nave(AZ_VampCrypt.ALTAR_F + 1.3, 0, AZ_VampCrypt.DAIS_Y);
				if (rs3)
					at = rs3.m_Head;
				vector toCam = AZ_VampCrypt.Nave(0, 0, 0);
				vector camFar = AZ_VampCrypt.Nave(10, 0, 0);
				toCam = toCam - camFar;
				toCam[1] = 0;
				toCam.Normalize();
				PlayerBase elder = AZ_VampEvent.AIVampire(AZ_VampEvent.VampType(""), at, toCam);
				TzDressVampire(elder, false);
				if (elder)
				{
					if (!s_TzActors)
						s_TzActors = new array<Object>;
					s_TzActors.Insert(elder);
				}
				Print("[AZ_Vamp] TEST tz elder " + elder);
				return 3000;
			}
			case 115:
				cam = AZ_VampCrypt.Nave(AZ_VampCrypt.ALTAR_F - 4.6, 0.4, 1.7);
				cam2 = AZ_VampCrypt.Nave(AZ_VampCrypt.ALTAR_F - 2.6, 0.25, 1.75);
				look = AZ_VampCrypt.Nave(AZ_VampCrypt.ALTAR_F + 1.3, 0, AZ_VampCrypt.DAIS_Y + 1.55);
				TzShot(p, "tz_elder", cam, look, cam2, look, 5, 0.7);
				if (s_TzActors && s_TzActors.Count() > 0)
					GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TzElderGone, 3000, false, PlayerBase.Cast(s_TzActors[s_TzActors.Count() - 1]));
				return 6000;
			case 116:
				TzClearActors();
				TzBloodMoon(p, false);
				AZ_VampCrypt.ShotSpot(0, stand, cam, look);
				HwGo(p, stand, look);
				p.SetHealth("", "Blood", AZ_VampState.Cfg().fullBlood);
				p.m_AZVampBareUntil = 0;
				p.m_AZVampFedUntil = 0;

				p.m_AZVampFrenzyUntil = GetGame().GetTime() + 30000;
				AZ_VampReveal.Update(p);
				return 9000;
			case 117:
			{
				vector head = p.GetBonePositionWS(p.GetBoneIndexByName("Head"));
				vector fwd = p.GetDirection();
				fwd[1] = 0;
				fwd.Normalize();

				cam = head + fwd * 0.75 + "0 -0.03 0";
				cam2 = head + fwd * 0.5 + "0 -0.02 0";
				TzShot(p, "tz_fangs", cam, head, cam2, head, 5, 0.5);
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TzFangsKey, 1500, false, p);
				return 6000;
			}
			case 120:
				TzBloodMoon(p, false);
				TzNight();
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TzLogDate, 1500, false, "bloodmoon");
				if (AZ_VampDecor.s_Lanterns && AZ_VampDecor.s_Lanterns.Count() > 0 && AZ_VampDecor.s_Lanterns[0])
				{
					at = AZ_VampDecor.s_Lanterns[0].GetPosition();
					HwGo(p, at + "8 0 8", at);
				}
				return 10000;
			case 121:
				if (AZ_VampDecor.s_Lanterns && AZ_VampDecor.s_Lanterns.Count() > 0 && AZ_VampDecor.s_Lanterns[0])
				{
					at = AZ_VampDecor.s_Lanterns[0].GetPosition();
					vector pp = p.GetPosition();
					cam = HwCam(at, pp, 10, 2.5);
					look = at + "0 3 0";
					cam2 = cam + "0 0.8 0";
					look2 = at + "0 5 0";
					TzShot(p, "tz_bloodmoon_rise", cam, look, cam2, look2, 12);
					GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TzBloodMoonOn, 2000, false, p);
				}
				return 13000;
			case 122:
			case 124:
			case 126:
				TzChainSetup(p);
				return 9000;
			case 123:
			case 125:
			case 127:
			{
				vector side = Vector(s_TzChainDir[2], 0, -s_TzChainDir[0]);
				at = s_TzChainAt + s_TzChainDir * 15.0;
				at[1] = GetGame().SurfaceY(at[0], at[2]);
				string take = "tz_chain_a";
				if (step == 123)
				{

					cam = at + s_TzChainDir * 12.0 + side * 1.6 + "0 1.7 0";
					cam2 = at + s_TzChainDir * 10.5 + side * 1.4 + "0 1.6 0";
					look = at - s_TzChainDir * 6.0 + "0 1.1 0";
					look2 = at - s_TzChainDir * 4.0 + "0 1.1 0";
				}
				if (step == 125)
				{
					take = "tz_chain_b";
					vector v2 = s_TzChainAt + s_TzChainDir * 11.0;
					v2[1] = GetGame().SurfaceY(v2[0], v2[2]);
					cam = v2 + side * 2.6 + "0 0.6 0";
					cam2 = v2 + side * 2.2 + "0 0.7 0";
					look = v2 + "0 1.3 0";
					look2 = look;
				}
				if (step == 127)
				{
					take = "tz_chain_c";
					cam = s_TzChainAt - s_TzChainDir * 3.0 + side * 2.0 + "0 6.0 0";
					cam2 = s_TzChainAt + s_TzChainDir * 3.0 + side * 2.0 + "0 5.0 0";
					look = at;
					look2 = at;
				}
				cam = TzKeepOff(p, TzUnblock(cam, look));
				cam2 = TzKeepOff(p, TzUnblock(cam2, look2));
				TzShot(p, take, cam, look, cam2, look2, 13);
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TzChainFire, 1200, false, p, AZ_VampState.Cfg().chainMax);
				return 14000;
			}
			case 128:
				TzChainSetup(p);
				for (int k = s_TzActors.Count() - 1; k >= 1; k--)
				{
					GetGame().ObjectDelete(s_TzActors[k]);
					s_TzActors.Remove(k);
				}
				return 8000;
			case 129:
			{
				vector sd = Vector(s_TzChainDir[2], 0, -s_TzChainDir[0]);
				at = s_TzChainAt + s_TzChainDir * 7.0;
				at[1] = GetGame().SurfaceY(at[0], at[2]);
				cam = at + sd * 4.0 + "0 0.5 0";
				cam2 = at + sd * 3.4 + "0 0.6 0";
				look = at + "0 0.6 0";
				cam = TzKeepOff(p, TzUnblock(cam, look));
				cam2 = TzKeepOff(p, TzUnblock(cam2, look));
				float hold = AZ_VampState.Cfg().ghoulRiseSec + 12;
				TzShot(p, "tz_kneel_ghoul", cam, look, cam2, look, hold);
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TzChainFire, 1000, false, p, 1);
				if (s_TzActors && s_TzActors.Count() > 0)
					GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TzDrainDry, 5000, false, p, PlayerBase.Cast(s_TzActors[0]));
				int holdMs = Math.Round(hold * 1000 + 1000);
				return holdMs;
			}


			case 180:
			{
				TzClearActors();
				TzBloodMoon(p, false);
				TzClearMist();
				TzNight();
				Fireplace fire = TzFire();
				if (!fire)
				{
					Print("[AZ_Vamp] TEST tz story: no village fire");
					return 1000;
				}
				s_TzFireAt = fire.GetPosition();
				TzLightFire(fire);
				vector seat = s_TzFireAt - s_TzVDir * 1.3;
				TzHero(seat, s_TzFireAt - seat, false);
				TzGesture(s_TzHero, DayZPlayerConstants.CMD_GESTUREFB_SITA);
				stand = s_TzFireAt + s_TzVDir * 25.0;
				TzPut(p, stand, s_TzFireAt);
				return 7000;
			}
			case 181:
			{
				vector side1 = Vector(s_TzVDir[2], 0, -s_TzVDir[0]);
				vector hero1 = s_TzFireAt - s_TzVDir * 1.3;
				cam = hero1 + s_TzVDir * 4.6 + side1 * 1.8;
				cam[1] = GetGame().SurfaceY(cam[0], cam[2]) + 1.0;
				cam2 = hero1 + s_TzVDir * 3.4 + side1 * 1.2;
				cam2[1] = GetGame().SurfaceY(cam2[0], cam2[2]) + 1.05;
				look = hero1 + "0 0.95 0";
				look2 = hero1 + "0 1.0 0";
				TzShot(p, "tz_story_fire", cam, look, cam2, look2, 5.5, 0.6);
				return 6500;
			}
			case 182:
			{
				vector stood = s_TzFireAt + s_TzVDir * 2.4;
				TzHero(stood, s_TzVDir, true);
				return 2000;
			}
			case 183:
			{
				vector hero2 = s_TzFireAt + s_TzVDir * 2.4;
				vector side2 = Vector(s_TzVDir[2], 0, -s_TzVDir[0]);
				cam = hero2 - s_TzVDir * 1.6 + side2 * 0.7;
				cam[1] = hero2[1] + 1.75;
				cam2 = hero2 - s_TzVDir * 0.9 + side2 * 0.6;
				cam2[1] = hero2[1] + 1.7;
				look = hero2 + s_TzVDir * 8.0 + "0 1.2 0";
				look2 = hero2 + s_TzVDir * 10.0 + "0 1.0 0";
				TzShot(p, "tz_story_torch", cam, look, cam2, look2, 5, 0.85);
				return 6000;
			}
			case 184:
			{
				p.SetHealth("", "Blood", p.GetMaxHealth("", "Blood") * 0.6);
				vector hero3 = s_TzFireAt + s_TzVDir * 2.4;
				vector side3 = Vector(s_TzVDir[2], 0, -s_TzVDir[0]);
				cam = hero3 + side3 * 12.0 + s_TzVDir * 3.0;
				cam[1] = GetGame().SurfaceY(cam[0], cam[2]) + 1.55;
				cam2 = hero3 + side3 * 1.4 - s_TzVDir * 0.7;
				cam2[1] = hero3[1] + 1.6;
				look = hero3 + "0 1.3 0";
				look2 = hero3 + "0 1.5 0";
				TzShot(p, "tz_vpov", cam, look, cam2, look2, 6, 0.75);
				return 7000;
			}
			case 185:
			{
				p.SetHealth("", "Blood", AZ_VampState.Cfg().fullBlood);
				vector eye = s_TzFireAt + s_TzVDir * 2.4;
				TzClearActors();
				vector sd5 = Vector(s_TzVDir[2], 0, -s_TzVDir[0]);
				s_TzE1 = eye + s_TzVDir * 5.0 - sd5 * 2.2;
				s_TzE1[1] = GetGame().SurfaceY(s_TzE1[0], s_TzE1[2]);
				s_TzE2 = eye + sd5 * 2.0 - s_TzVDir * 0.4;
				s_TzE2[1] = GetGame().SurfaceY(s_TzE2[0], s_TzE2[2]);
				TzEva(s_TzE1, eye);
				PlayerBase e2 = AZ_VampEvent.AIVampire(AZ_VampDress.Male(Math.RandomInt(0, 1000)), s_TzE2, (eye - s_TzE2).Normalized());
				TzDressCommonVamp(e2);
				if (e2)
					s_TzActors.Insert(e2);
				return 1500;
			}
			case 186:
			{
				vector eye2 = s_TzFireAt + s_TzVDir * 2.4 + "0 1.62 0";
				vector sd2 = Vector(s_TzVDir[2], 0, -s_TzVDir[0]);
				cam = eye2;
				cam2 = eye2 + s_TzVDir * 0.4;
				look = eye2 + s_TzVDir * 8.0 + sd2 * 3.0 - "0 0.6 0";
				look2 = eye2 + s_TzVDir * 8.0 - sd2 * 0.5 - "0 0.6 0";
				TzShot(p, "tz_hpov", cam, look, cam2, look2, 3.0, 0.95);
				return 3300;
			}
			case 187:
			{
				vector eye3 = s_TzFireAt + s_TzVDir * 2.8 + "0 1.6 0";
				vector sd3 = Vector(s_TzVDir[2], 0, -s_TzVDir[0]);
				cam = eye3;
				cam2 = eye3;
				look = eye3 + s_TzVDir * 8.0 - sd3 * 0.5 - "0 0.6 0";
				look2 = s_TzE1 + "0 1.5 0";
				TzShot(p, "tz_hpov_snap", cam, look, cam2, look2, 0.45, 0.95);
				return 1600;
			}
			case 188:
			{
				vector eye4 = s_TzFireAt + s_TzVDir * 2.8 + "0 1.6 0";
				eye4 = TzOutOfTrees(eye4, s_TzE1 - eye4);
				cam = eye4;
				vector toE1 = s_TzE1 + "0 1.6 0";
				toE1 = toE1 - eye4;
				float dE1 = toE1.Length();
				toE1.Normalize();
				cam2 = eye4 + toE1 * Math.Max(0, dE1 - 0.95);
				look = s_TzE1 + "0 1.5 0";
				look2 = s_TzE1 + "0 1.62 0";
				TzShot(p, "tz_hpov_elder", cam, look, cam2, look2, 3.2, 0.8);
				return 3500;
			}
			case 189:
			{
				vector eye5 = s_TzFireAt + s_TzVDir * 2.8 + "0 1.6 0";
				vector toE1b = s_TzE1 + "0 1.6 0";
				toE1b = toE1b - eye5;
				float dE1b = toE1b.Length();
				toE1b.Normalize();
				cam = eye5 + toE1b * Math.Max(0, dE1b - 0.95);
				cam2 = cam;
				look = s_TzE1 + "0 1.62 0";
				look2 = s_TzE2 + "0 1.55 0";
				TzShot(p, "tz_hpov_turn", cam, look, cam2, look2, 0.5, 0.9);
				return 2500;
			}
			case 131:
			{
				TzClearActors();
				TzBloodMoon(p, true);
				vector at6 = s_TzFireAt + s_TzVDir * 2.4;
				TzHero(at6, s_TzVDir, false);
				string fed = TzFeedEva(s_TzHero);
				Print("[AZ_Vamp] TEST tz feed2: " + fed);

				array<Object> near = new array<Object>;
				GetGame().GetObjectsAtPosition(at6, 2.5, near, null);
				foreach (Object o : near)
				{
					PlayerBase pv = PlayerBase.Cast(o);
					if (pv && pv != s_TzHero && pv != p && pv.m_AZVampire)
						s_TzActors.Insert(pv);
				}
				return 1800;
			}
			case 132:
			{

				vector at7 = s_TzFireAt + s_TzVDir * 2.4;
				vector sd7 = Vector(s_TzVDir[2], 0, -s_TzVDir[0]);
				vector off = s_TzVDir * 0.75 + sd7 * 0.65;
				off.Normalize();
				cam = at7 + off * 4.4;
				cam[1] = at7[1] + 1.75;
				cam2 = at7 + off * 3.0;
				cam2[1] = at7[1] + 1.6;
				look = at7 + "0 1.35 0";
				look2 = at7 + "0 1.45 0";
				TzShot(p, "tz_feed2", cam, look, cam2, look2, 5, 0.7);
				return 6000;
			}
			case 133:
				TzClearActors();
				s_TzHero = null;
				return 1000;
			case 134:
			{
				if (s_TzFireAt == vector.Zero)
				{
					Fireplace tfp = TzFire();
					TzLightFire(tfp);
					if (tfp)
						s_TzFireAt = tfp.GetPosition();
				}
				vector tat = s_TzFireAt + s_TzVDir * 6.0;
				tat[1] = GetGame().SurfaceY(tat[0], tat[2]);
				vector tface = s_TzVDir * -1;
				TzPut(p, tat, tat + tface);
				TzBloodMoon(p, false);
				TzDressVampire(p, true);
				p.SetHealth("", "Blood", AZ_VampState.Cfg().fullBlood);
				p.m_AZVampFrenzyUntil = GetGame().GetTime() + 40000;
				p.m_AZVampFedUntil = 0;
				AZ_VampReveal.Update(p);
				return 6000;
			}
			case 135:
			{
				vector th = p.GetBonePositionWS(p.GetBoneIndexByName("Head"));
				vector tf = p.GetDirection();
				tf[1] = 0;
				tf.Normalize();
				cam = th + tf * 0.62 + "0 0.05 0";
				cam2 = th + tf * 0.55 + "0 0.05 0";
				TzShot(p, "tz_thumb", cam, th, cam2, th, 6, 0.42);




				TzLogFace(p, "before key");
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TzLogFace, 2500, false, p, "after the hiss");
				return 7000;
			}


			case 136:
			{
				TzClearActors();
				TzBloodMoon(p, false);
				Fireplace pf = TzFire();
				TzLightFire(pf);
				if (pf)
					s_TzFireAt = pf.GetPosition();
				vector pat = s_TzFireAt + s_TzVDir * 2.0;
				pat[1] = GetGame().SurfaceY(pat[0], pat[2]);
				TzPut(p, pat, s_TzFireAt);
				TzDressVampire(p, true);
				p.SetHealth("", "Blood", AZ_VampState.Cfg().fullBlood);
				p.m_AZVampFrenzyUntil = 0;
				p.m_AZVampFedUntil = GetGame().GetTime() + 40000;
				AZ_VampReveal.Update(p);
				return 5000;
			}
			case 137:
			{
				vector peh = p.GetBonePositionWS(p.GetBoneIndexByName("Head"));
				vector pef = p.GetDirection();
				pef[1] = 0;
				pef.Normalize();
				vector pes = Vector(pef[2], 0, -pef[0]);
				vector pe3 = pef * 0.72 + pes * 0.55;
				pe3.Normalize();
				cam = peh + pe3 * 0.8 + "0 0.03 0";
				cam2 = peh + pe3 * 0.72 + "0 0.03 0";
				TzShot(p, "tz_poster_eva", cam, peh, cam2, peh, 5, 0.5);
				return 6000;
			}


			case 138:
			{
				TzClearActors();
				TzBloodMoon(p, true);
				GetGame().GetWorld().SetDate(2011, 11, 10, 22, 30);
				AZ_VampProgress.FullMoonDate();
				TzMoonSky(true);
				GetGame().GetWeather().GetOvercast().Set(0, 0, 3600);
				GetGame().GetWeather().GetFog().Set(0, 0, 3600);
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TzLogDate, 1500, false, "poster moon");
				Fireplace pmf = TzFire();
				TzLightFire(pmf);
				if (pmf)
					s_TzFireAt = pmf.GetPosition();
				vector pma = s_TzFireAt + s_TzVDir * 6.0;
				pma[1] = GetGame().SurfaceY(pma[0], pma[2]);
				TzEva(pma, pma + s_TzVDir * 5.0);
				TzPut(p, s_TzFireAt - s_TzVDir * 30.0, s_TzFireAt);
				return 12000;
			}
			case 139:
			{
				vector pmb = s_TzFireAt + s_TzVDir * 6.0;
				pmb[1] = GetGame().SurfaceY(pmb[0], pmb[2]);
				cam = pmb - s_TzVDir * 11.0 + "0 1.1 0";
				look = pmb + "0 3.0 0";
				cam2 = cam + "0 0.3 0";
				look2 = pmb + "0 4.0 0";
				TzShot(p, "tz_poster_moon", cam, look, cam2, look2, 5, 0.7);
				return 6000;
			}



			case 190:
			{
				TzClearActors();
				TzBloodMoon(p, false);
				GetGame().GetWorld().SetDate(2011, 11, 10, 22, 30);
				TzMoonSky(true);
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TzLogDate, 1500, false, "hook");
				Fireplace hf = TzFire();
				TzLightFire(hf);
				if (hf)
					s_TzFireAt = hf.GetPosition();
				return 3000;
			}
			case 191:
			{
				vector hm = TzMoonFlat();
				vector hat = s_TzFireAt + hm * 2.5;
				hat[1] = GetGame().SurfaceY(hat[0], hat[2]);
				TzPut(p, hat, hat - hm);
				TzDressVampire(p, true);
				p.SetHealth("", "Blood", AZ_VampState.Cfg().fullBlood);
				p.m_AZVampFrenzyUntil = 0;
				p.m_AZVampFedUntil = GetGame().GetTime() + 40000;
				AZ_VampReveal.Update(p);
				return 4000;
			}
			case 192:
			{
				vector hm2 = TzMoonFlat();
				vector hkHead = p.GetBonePositionWS(p.GetBoneIndexByName("Head"));
				float he = TzMoonElev();

				float ha = Math.Min(he * 0.55, 20.0) * Math.DEG2RAD;
				float hfov = 0.9;
				if (he > 40)
					hfov = 1.15;
				cam = hkHead - hm2 * 0.6 - "0 0.08 0";
				vector hkDir = hm2 * Math.Cos(ha) + Vector(0, Math.Sin(ha), 0);
				look = cam + hkDir * 0.6;
				cam2 = cam + hm2 * 0.1;
				look2 = look + hm2 * 0.1;
				Print("[AZ_Vamp] TEST tz hook eva moon elev " + he + " tilt " + (ha * Math.RAD2DEG) + " fov " + hfov);
				TzShot(p, "tz_hook_eva", cam, look, cam2, look2, 5, hfov);
				return 6000;
			}


			case 195:
			{
				vector gh = p.GetBonePositionWS(p.GetBoneIndexByName("Head"));
				vector gf = p.GetDirection();
				gf[1] = 0;
				gf.Normalize();
				cam = gh + gf * 1.8 - "0 0.35 0";
				cam2 = gh + gf * 1.65 - "0 0.35 0";
				look = gh - "0 0.45 0";
				look2 = look;
				TzShot(p, "tz_gown_check", cam, look, cam2, look2, 3, 0.75);
				return 4000;
			}

			case 193:
			{
				vector bm = TzMoonFlat();
				s_TzE1 = s_TzFireAt + bm * 10.0;
				s_TzE1[1] = GetGame().SurfaceY(s_TzE1[0], s_TzE1[2]);
				TzEva(s_TzE1, s_TzFireAt);
				return 2500;
			}
			case 194:
			{
				vector bm2 = TzMoonFlat();
				vector beye = s_TzFireAt + bm2 * 3.0;
				beye[1] = GetGame().SurfaceY(beye[0], beye[2]) + 1.62;
				beye = TzOutOfTrees(beye, s_TzE1 - beye);
				float be = TzMoonElev();
				float ba = Math.Min(be * 0.5, 18.0) * Math.DEG2RAD;
				cam = beye;
				look = s_TzE1 + "0 1.5 0" + Vector(0, Math.Tan(ba) * 7.0, 0);
				cam2 = beye + bm2 * 0.4;
				look2 = s_TzE1 + "0 1.6 0";
				TzShot(p, "tz_hook_pov", cam, look, cam2, look2, 4, 0.85);
				return 5000;
			}





			case 196:
			{
				TzClearActors();
				TzBloodMoon(p, false);
				TzMoonSky(false);
				if (s_TzFireAt == vector.Zero)
				{
					Fireplace sfp = TzFire();
					TzLightFire(sfp);
					if (sfp)
						s_TzFireAt = sfp.GetPosition();
				}
				vector sat = s_TzFireAt + s_TzVDir * 7.0;
				sat[1] = GetGame().SurfaceY(sat[0], sat[2]);
				TzPut(p, sat, sat - s_TzVDir);
				TzDressVampire(p, true);
				p.SetHealth("", "Blood", AZ_VampState.Cfg().fullBlood);
				p.m_AZVampFrenzyUntil = 0;
				p.m_AZVampFedUntil = GetGame().GetTime() + 60000;
				AZ_VampReveal.Update(p);
				return 5000;
			}
			case 197:
			case 198:
			case 199:
			{
				vector sh = p.GetBonePositionWS(p.GetBoneIndexByName("Head"));
				vector snF = p.GetDirection();
				snF[1] = 0;
				snF.Normalize();
				vector ss = Vector(snF[2], 0, -snF[0]);
				vector sdir = snF;
				float sdrop = 0.02;
				string slab = "tz_snarl_a";
				if (step == 198)
				{
					sdir = snF * 0.9 + ss * 0.42;
					sdir.Normalize();
					slab = "tz_snarl_b";
				}
				if (step == 199)
				{
					sdrop = 0.16;
					slab = "tz_snarl_c";
				}
				cam = sh + sdir * 0.95 - Vector(0, sdrop, 0);
				cam2 = sh + sdir * 0.88 - Vector(0, sdrop, 0);
				TzShot(p, slab, cam, sh, cam2, sh, 4, 0.36);
				return 5500;
			}




			case 118:
			case 119:
			case 148:
				return TzBiteSetup(p, step);




			case 165:
			{
				TzClearActors();
				TzBloodMoon(p, false);
				GetGame().GetWorld().SetDate(2011, 11, 10, 22, 30);
				TzMoonSky(true);
				if (s_TzFireAt == vector.Zero)
				{
					Fireplace hfp2 = TzFire();
					TzLightFire(hfp2);
					if (hfp2)
						s_TzFireAt = hfp2.GetPosition();
				}
				vector hdir = TzMoonFlat();
				vector hat2 = s_TzFireAt - hdir * 10.0;
				hat2[1] = GetGame().SurfaceY(hat2[0], hat2[2]);
				TzHero(hat2, hdir, true);
				vector eat = hat2 + hdir * 5.0;
				eat[1] = GetGame().SurfaceY(eat[0], eat[2]);
				TzPut(p, eat, hat2);
				TzDressVampire(p, true);
				p.SetHealth("", "Blood", AZ_VampState.Cfg().fullBlood);
				p.m_AZVampFrenzyUntil = 0;
				p.m_AZVampFedUntil = GetGame().GetTime() + 60000;
				AZ_VampReveal.Update(p);
				return 5000;
			}
			case 166:
			case 167:
			{
				if (!s_TzHero)
					return 1000;
				vector hh2 = s_TzHero.GetBonePositionWS(s_TzHero.GetBoneIndexByName("Head"));
				vector hd2 = p.GetPosition() - s_TzHero.GetPosition();
				hd2[1] = 0;
				hd2.Normalize();
				vector hs2 = Vector(hd2[2], 0, -hd2[0]);
				vector eh2 = p.GetBonePositionWS(p.GetBoneIndexByName("Head"));
				if (step == 166)
				{
					cam = hh2 - hd2 * 0.75 + hs2 * 0.42 + "0 0.08 0";
					TzShot(p, "tz_hunt_ots", cam, eh2 - "0 0.25 0", cam + hd2 * 0.15, eh2 - "0 0.2 0", 4, 0.62);
				}
				else
				{
					cam = hh2 - hd2 * 0.5 + hs2 * 0.3 - "0 0.04 0";
					TzShot(p, "tz_hunt_tele", cam, eh2, cam + hd2 * 0.1, eh2, 4, 0.26);
				}
				return 5000;
			}



			case 168:
			case 178:
			{
				TzClearActors();
				TzBloodMoon(p, false);
				float cap = AZ_VampCrypt.APSE_F;


				p.SetPosition(AZ_VampCrypt.Nave(cap + 0.4, -3.0, AZ_VampCrypt.DAIS_Y + 0.1));
				if (step == 168)
				{




					cam = AZ_VampCrypt.Nave(cap + 0.2, -2.4, 3.2);
					look = AZ_VampCrypt.Nave(AZ_VampCrypt.CountessF(), 0, AZ_VampCrypt.CountessFeet() + 1.5 * AZ_VampCrypt.CountessScale());
					TzShot(p, "tz_countess_whole", cam, look, cam + (look - cam).Normalized() * 0.15, look, 4, 0.8);
				}
				else
				{
					cam = AZ_VampCrypt.Nave(cap + 3.6, -1.4, AZ_VampCrypt.DAIS_Y + 1.3);
					look = AZ_VampCrypt.Nave(cap + 8.6, 0, 2.9);
					TzShot(p, "tz_countess_close", cam, look, cam + (look - cam).Normalized() * 0.2, look, 4, 0.75);
				}
				return 5500;
			}


			case 179:
			{
				TzClearActors();
				TzBloodMoon(p, false);


				TzMoonSky(true);
				GetGame().GetWorld().SetDate(2011, 11, 10, 22, 30);
				if (s_TzFireAt == vector.Zero)
				{
					Fireplace nfp = TzFire();
					TzLightFire(nfp);
					if (nfp)
						s_TzFireAt = nfp.GetPosition();
				}


				TzDressVampire(p, true);
				vector nat = s_TzFireAt + s_TzVDir * 9.0;
				nat[1] = GetGame().SurfaceY(nat[0], nat[2]);
				TzPut(p, nat, nat - s_TzVDir);
				p.SetHealth("", "Blood", AZ_VampState.Cfg().fullBlood);
				p.m_AZVampFrenzyUntil = 0;
				p.m_AZVampFedUntil = 0;
				AZ_VampReveal.Update(p);
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TzSnarlTorchShot, 5000, false, p);
				return 11000;
			}




			case 172:
			{
				TzClearActors();
				TzBloodMoon(p, true);
				GetGame().GetWorld().SetDate(2011, 11, 10, 22, 30);
				TzMoonSky(true);
				if (s_TzFireAt == vector.Zero)
				{
					Fireplace ffp = TzFire();
					TzLightFire(ffp);
					if (ffp)
						s_TzFireAt = ffp.GetPosition();
				}
				vector fmd = TzMoonFlat();
				vector fside = Vector(fmd[2], 0, -fmd[0]);
				vector fat = s_TzFireAt + fside * 30.0;
				fat[1] = GetGame().SurfaceY(fat[0], fat[2]);
				vector fcam = fmd * 0.5 + fside * 0.87;
				TzDressVampire(p, true);
				TzPut(p, fat, fat + fcam);
				p.SetHealth("", "Blood", AZ_VampState.Cfg().fullBlood);
				p.m_AZVampFrenzyUntil = GetGame().GetTime() + 60000;
				p.m_AZVampFedUntil = 0;
				AZ_VampReveal.Update(p);

				float fang[4] = {0, 110, 175, 250};
				float frad[4] = {1.0, 1.9, 2.3, 1.7};
				for (int fi = 0; fi < 4; fi++)
				{
					float fa = fang[fi] * Math.DEG2RAD;
					vector fdir = fcam * Math.Cos(fa) + Vector(fcam[2], 0, -fcam[0]) * Math.Sin(fa);
					vector fp = fat + fdir * frad[fi];
					fp[1] = GetGame().SurfaceY(fp[0], fp[2]);
					string fty = AZ_VampDress.Male(fi + 3);
					if (fi % 2 == 1)
					{
						fty = AZ_VampDress.Female(fi + 3);
						if (fty == "SurvivorF_Eva")
							fty = AZ_VampDress.Female(fi + 4);
					}
					PlayerBase fv = PlayerBase.Cast(GetGame().CreateObject(fty, fp, false, true));
					if (!fv)
						continue;
					fv.SetPosition(fp);
					fv.SetDirection(Vector(Math.Sin(fi * 1.7), 0, Math.Cos(fi * 1.7)));
					AZ_VampDress.Survivor(fv, fi + 11);
					if (!s_TzActors)
						s_TzActors = new array<Object>;
					s_TzActors.Insert(fv);
					GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TzKnockOut, 900 + fi * 150, false, fv);
				}
				Print("[AZ_Vamp] TEST tz feast at " + fat + " moon dir " + fmd + " elev " + TzMoonElev());
				return 8000;
			}
			case 173:
			case 174:
			{
				vector ff = p.GetDirection();
				ff[1] = 0;
				ff.Normalize();
				vector fstand = p.GetPosition();
				cam = fstand + ff * 4.4 + Vector(0, 1.1, 0);
				look = fstand + Vector(0, 1.0, 0);
				cam2 = fstand + ff * 4.1 + Vector(0, 1.1, 0);
				string flab = "tz_feast_a";
				if (step == 174)
					flab = "tz_feast_b";
				TzShot(p, flab, cam, look, cam2, look, 4, 0.7);
				return 5500;
			}




			case 169:
			{
				TzClearActors();
				TzBloodMoon(p, false);
				AZ_VampState.s_Data.eventActive = true;
				GetGame().GetWorld().SetDate(2011, 11, 5, 22, 0);
				if (p.m_AZVampire)
					AZ_VampEvent.Cure(p);
				s_HwGuard = false;
				AZ_VampRitual.Friends(null, "clear");
				Print("[AZ_Vamp] TEST tz ritual: " + AZ_VampRitual.Debug(p, "slab") + " | " + AZ_VampRitual.Friends(p, "3"));
				s_TzRitualStart = GetGame().GetTime();
				s_TzRitualShots = 0;
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(TzRitualLie, 4000, false, p);
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TzRitualWatch, 4500, false, p);
				return 125000;
			}
			case 130:
				TzMoonSky(false);
				TzClearActors();
				TzBloodMoon(p, false);
				Print("[AZ_Vamp] TEST tz done");
				return 1000;
		}
		return 1000;
	}
}

modded class MissionGameplay
{
	protected bool m_TzOn;
	protected bool m_TzUsed;
	protected float m_TzT;
	protected float m_TzDur;
	protected float m_TzFov;
	protected vector m_TzCam0, m_TzCam1, m_TzLook0, m_TzLook1;
	protected bool m_TzHead;
	protected ScriptedLightBase m_TzFillLight;
	protected ScriptedLightBase m_TzKeyLight;

	protected ScriptedLightBase m_TzCoolRim;



	void TzSnarlLights(vector face)
	{
		TzKey(false, vector.Zero);
		if (m_TzCoolRim)
		{
			m_TzCoolRim.Destroy();
			m_TzCoolRim = null;
		}
		vector toCam = m_TzCam0 - face;
		toCam[1] = 0;
		toCam.Normalize();
		vector side = Vector(toCam[2], 0, -toCam[0]);
		m_TzKeyLight = ScriptedLightBase.CreateLight(AZ_TzKeyLight, face + toCam * 0.35 + side * 0.75 - "0 0.22 0");
		m_TzCoolRim = ScriptedLightBase.CreateLight(AZ_TzCoolRimLight, face - toCam * 0.55 - side * 0.45 + "0 0.3 0");
		Print("[AZ_Vamp] TEST tz snarl lights key " + m_TzKeyLight + " rim " + m_TzCoolRim);
	}



	void TzMoonLights(vector face)
	{
		TzKey(false, vector.Zero);
		if (m_TzCoolRim)
		{
			m_TzCoolRim.Destroy();
			m_TzCoolRim = null;
		}
		vector toCam = m_TzCam0 - face;
		toCam[1] = 0;
		toCam.Normalize();
		vector side = Vector(toCam[2], 0, -toCam[0]);
		m_TzKeyLight = ScriptedLightBase.CreateLight(AZ_TzMoonKeyLight, face + toCam * 0.6 + side * 0.6 + "0 0.4 0");
		m_TzCoolRim = ScriptedLightBase.CreateLight(AZ_TzCoolRimLight, face - toCam * 0.55 - side * 0.45 + "0 0.3 0");
	}


	void TzExposure(float mul)
	{
		World w = GetGame().GetWorld();
		if (mul == 1.0)
		{
			w.SetEyeAccom(-1);
			return;
		}
		float cur = w.GetEyeAccom();
		Print("[AZ_Vamp] TEST tz exposure: adapted " + cur + " x " + mul);
		if (cur > 0)
			w.SetEyeAccom(cur * mul);
	}


	void TzHuntRim(vector face)
	{
		TzKey(false, vector.Zero);
		if (m_TzCoolRim)
		{
			m_TzCoolRim.Destroy();
			m_TzCoolRim = null;
		}
		vector toCam = m_TzCam0 - face;
		toCam[1] = 0;
		toCam.Normalize();
		m_TzCoolRim = ScriptedLightBase.CreateLight(AZ_TzCoolRimLight, face - toCam * 0.6 + "0 0.35 0");
	}


	void TzKey(bool on, vector face)
	{
		if (m_TzKeyLight)
		{
			m_TzKeyLight.Destroy();
			m_TzKeyLight = null;
		}
		if (!on)
			return;
		vector toCam = m_TzCam0 - face;
		toCam[1] = 0;
		toCam.Normalize();
		vector side = Vector(toCam[2], 0, -toCam[0]);
		vector at = face + toCam * 0.9 + side * 0.5 + "0 0.25 0";
		m_TzKeyLight = ScriptedLightBase.CreateLight(AZ_TzKeyLight, at);
		Print("[AZ_Vamp] TEST tz key light at " + at);
	}
	protected ScriptedLightBase m_TzTorch;
	protected ScriptedLightBase m_TzRim;


	void TzRim(bool on)
	{
		if (m_TzRim)
		{
			m_TzRim.Destroy();
			m_TzRim = null;
		}
		if (!on)
			return;
		vector away = m_TzLook0 - m_TzCam0;
		away[1] = 0;
		away.Normalize();
		vector at = m_TzLook0 + away * 0.7 + "0 0.25 0";
		m_TzRim = ScriptedLightBase.CreateLight(AZ_TzRimLight, at);
	}
	protected bool m_TzHuman;
	protected bool m_TzNoSight;
	protected float m_TzShake;


	void TzTorch(bool on)
	{
		if (m_TzTorch)
		{
			m_TzTorch.Destroy();
			m_TzTorch = null;
		}
		if (on)
			m_TzTorch = ScriptedLightBase.CreateLight(AZ_TzTorchLight, m_TzCam0);
	}
	protected bool m_TzTintOn;
	protected float m_TzTintK;


	void TzFill(bool on)
	{
		if (m_TzFillLight)
		{
			m_TzFillLight.Destroy();
			m_TzFillLight = null;
		}
		if (!on)
			return;
		vector at = m_TzLook0 + "0 1.6 0";
		vector toCam = m_TzCam0 - m_TzLook0;
		at = at + toCam * 0.35;
		m_TzFillLight = ScriptedLightBase.CreateLight(AZ_TzFillLight, at);
		Print("[AZ_Vamp] TEST tz fill light at " + at);
	}
	protected ref array<ScriptedLightBase> m_TzEyes;
	protected Object m_TzCountess;
	protected vector m_TzEyeL, m_TzEyeR;


	bool TzFindEyes(vector near)
	{
		m_TzCountess = null;
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition(near, 25, objs, null);
		foreach (Object o : objs)
		{
			if (o && o.GetType() == "AZ_Crypt_Countess")
				m_TzCountess = o;
		}
		if (!m_TzCountess || !m_TzCountess.MemoryPointExists("eye_l") || !m_TzCountess.MemoryPointExists("eye_r"))
		{
			Print("[AZ_Vamp] TEST tz countess eyes: statue " + m_TzCountess + ", no eye points");
			return false;
		}
		vector ml = m_TzCountess.GetMemoryPointPos("eye_l");
		m_TzEyeL = m_TzCountess.ModelToWorld(ml);
		vector mr = m_TzCountess.GetMemoryPointPos("eye_r");
		m_TzEyeR = m_TzCountess.ModelToWorld(mr);
		return true;
	}

	void TzStatueEyes(bool on, vector near)
	{
		if (m_TzEyes)
		{
			foreach (ScriptedLightBase l : m_TzEyes)
			{
				if (l)
					l.Destroy();
			}
			m_TzEyes.Clear();
		}
		if (!on || !TzFindEyes(near))
			return;
		if (!m_TzEyes)
			m_TzEyes = new array<ScriptedLightBase>;
		array<vector> at = new array<vector>;
		at.Insert(m_TzEyeL);
		at.Insert(m_TzEyeR);
		foreach (vector e : at)
		{
			ScriptedLightBase l2 = ScriptedLightBase.CreateLight(AZ_VampEyeLight, e);
			if (!l2)
				continue;
			l2.SetRadiusTo(0.35);
			l2.SetBrightnessTo(18.0);
			l2.SetFlareVisible(true);
			m_TzEyes.Insert(l2);
		}
		Print("[AZ_Vamp] TEST tz countess eyes lit at " + m_TzEyeL + " / " + m_TzEyeR);
	}


	void TzEyesFrame()
	{
		if (!m_TzCountess)
			return;
		vector mid = (m_TzEyeL + m_TzEyeR) * 0.5;
		vector toCam = m_TzCam0 - mid;
		toCam[1] = 0;
		if (toCam.Length() < 0.1)
			return;
		toCam.Normalize();
		m_TzCam0 = mid + toCam * 1.6;
		m_TzCam1 = mid + toCam * 1.15;
		m_TzLook0 = mid;
		m_TzLook1 = mid;
		m_TzFov = 0.55;
		Print("[AZ_Vamp] TEST tz eyes framed on the eye points, mid " + mid);
	}




	void TzMoonFrame()
	{
		World w = GetGame().GetWorld();
		vector md = w.GetSunOrMoonDirection();
		vector up = md * -1;
		if (up[1] < 0)
			up = md;
		Print("[AZ_Vamp] TEST tz moon dir " + up + " sunOrMoon " + w.GetSunOrMoon() + " intensity " + w.GetMoonIntensity());
		if (w.GetSunOrMoon() >= 0.5)
			return;
		vector h = up;
		h[1] = 0;
		if (h.Length() < 0.1)
			return;
		h.Normalize();
		vector base = m_TzLook0 - "0 3 0";
		float yaws[5] = {0, 30, -30, 60, -60};
		for (int i = 0; i < 5; i++)
		{
			float a = yaws[i] * Math.DEG2RAD;
			vector hh = Vector(h[0] * Math.Cos(a) - h[2] * Math.Sin(a), 0, h[0] * Math.Sin(a) + h[2] * Math.Cos(a));
			vector c0 = base - hh * 10.0;
			c0[1] = GetGame().SurfaceY(c0[0], c0[2]) + 1.6;
			if (!AZ_TzView.Clear(base + "0 1 0", c0))
				continue;
			m_TzCam0 = c0;
			m_TzCam1 = c0 + "0 0.8 0";
			m_TzLook0 = base + "0 1.2 0";
			m_TzLook1 = m_TzCam1 + up * 100.0;
			m_TzFov = 1.0;
			Print("[AZ_Vamp] TEST tz moon framing yaw " + yaws[i] + " cam " + c0);
			return;
		}
		Print("[AZ_Vamp] TEST tz moon framing: no clear side, server framing kept");
	}



	void TzHiss()
	{
		PlayerBase me = PlayerBase.Cast(GetGame().GetPlayer());
		if (!me || !me.GetEmoteManager())
			return;
		bool can = me.GetEmoteManager().CanPlayEmote(EmoteConstants.ID_EMOTE_TAUNTKISS);
		if (can)
			me.GetEmoteManager().CreateEmoteCBFromMenu(EmoteConstants.ID_EMOTE_TAUNTKISS);
		Print("[AZ_Vamp] TEST tz thumb hiss emote " + can);
	}

	override void VampHwShot(string label, vector cam, vector target)
	{
		if (label == "done")
		{
			m_TzOn = false;
			m_TzHead = false;
			TzStatueEyes(false, vector.Zero);
			TzFill(false);
			TzKey(false, vector.Zero);
			if (m_TzCoolRim)
			{
				m_TzCoolRim.Destroy();
				m_TzCoolRim = null;
			}
			TzRim(false);
			TzExposure(1.0);
			TzTorch(false);
			m_TzHuman = false;
			m_TzNoSight = false;
			m_TzTintOn = false;
			AZ_PPETzTint.Get().SetTint(0);
			PlayerBase pd = PlayerBase.Cast(GetGame().GetPlayer());
			if (pd)
			{
				pd.m_AZVampTalk = false;
				pd.m_AZVampSightSet = -1;
			}
			if (!m_TzUsed)
				TzHud(true);
			else
				Print("[AZ_Vamp] TEST tz hud stays hidden after the teaser");
			super.VampHwShot(label, cam, target);
			return;
		}
		if (label.IndexOf("#") < 0)
		{
			super.VampHwShot(label, cam, target);
			return;
		}
		array<string> parts = new array<string>;
		label.Split("#", parts);
		if (parts[0] == "tz_prep")
		{
			TzQuality();
			return;
		}
		if (parts.Count() < 5)
			return;
		m_TzDur = Math.Max(0.1, parts[1].ToFloat());
		m_TzCam0 = cam;
		m_TzLook0 = target;
		m_TzCam1 = parts[2].ToVector();
		m_TzLook1 = parts[3].ToVector();
		m_TzFov = parts[4].ToFloat();
		m_TzT = 0;
		m_TzOn = true;
		m_TzUsed = true;


		int cy, cmo, cd, chh, cmi;
		GetGame().GetWorld().GetDate(cy, cmo, cd, chh, cmi);
		Print("[AZ_Vamp] TEST tz client clock " + parts[0] + ": " + cy + "/" + cmo + "/" + cd + " " + chh + ":" + cmi + " night " + GetGame().GetWorld().IsNight() + " moon " + GetGame().GetWorld().GetMoonIntensity() + " sunOrMoon " + GetGame().GetWorld().GetSunOrMoon());


		bool snarl = parts[0].IndexOf("tz_snarl") == 0;
		bool snarlMoon = parts[0] == "tz_snarl_moon";
		m_TzHead = parts[0] == "tz_fangs" || parts[0] == "tz_thumb" || parts[0] == "tz_poster_eva" || snarl;
		if (snarl)
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(TzHiss, 600, false);
		if (parts[0] == "tz_poster_eva" || parts[0] == "tz_hook_eva")
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(TzHiss, 900, false);
		if (parts[0] == "tz_poster_moon")
			m_TzTintOn = true;
		if (parts[0] == "tz_thumb")
		{
			m_TzTintOn = true;
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(TzHiss, 900, false);
		}
		PlayerBase me = PlayerBase.Cast(GetGame().GetPlayer());
		if (me)
			me.m_AZVampTalk = m_TzHead;
		if (parts[0] == "tz_poster_moon")
		{

			GetGame().GetWorld().SetDate(2011, 11, 10, 22, 30);
			AZ_VampProgress.FullMoonDate();
			GetGame().GetWeather().GetFog().Set(0, 0, 3600);
			GetGame().GetWeather().GetOvercast().Set(0, 0, 3600);
			Print("[AZ_Vamp] TEST tz poster moon client: moon " + GetGame().GetWorld().GetMoonIntensity() + " fog " + GetGame().GetWeather().GetFog().GetActual());
		}
		if (parts[0] == "tz_bloodmoon_rise" || parts[0] == "tz_poster_moon")
			TzMoonFrame();

		TzStatueEyes(parts[0] == "tz_eyes_ecu", m_TzLook0);
		TzFill(parts[0] == "tz_sleepers");

		PlayerBase kme = PlayerBase.Cast(GetGame().GetPlayer());
		bool biteShot = parts[0].IndexOf("tz_bite") == 0;
		if (snarlMoon)
			TzKey(false, vector.Zero);
		else if (snarl && kme)
			TzSnarlLights(kme.GetBonePositionWS(kme.GetBoneIndexByName("Head")));
		else if (biteShot && parts[0] == "tz_bite_over")
			TzMoonLights(m_TzLook0);
		else if (biteShot)
			TzSnarlLights(m_TzLook0);
		else if (parts[0].IndexOf("tz_hunt") == 0 && kme)
			TzHuntRim(kme.GetBonePositionWS(kme.GetBoneIndexByName("Head")));
		else if ((parts[0] == "tz_thumb" || parts[0] == "tz_poster_eva" || parts[0] == "tz_hook_eva" || parts[0] == "tz_gown_check") && kme)
			TzKey(true, kme.GetBonePositionWS(kme.GetBoneIndexByName("Head")));
		else if (parts[0] == "tz_hpov_elder" || parts[0] == "tz_hook_pov")
			TzKey(true, m_TzLook1);
		else
			TzKey(false, vector.Zero);
		TzRim(parts[0] == "tz_thumb" || parts[0] == "tz_poster_eva" || parts[0] == "tz_hook_eva");


		float accom = 1.0;
		if (snarlMoon)
			accom = 0.45;
		else if (parts[0] == "tz_river_stare" || parts[0] == "tz_river_embers" || parts[0] == "tz_offer_onlooker")
			accom = 1.6;
		TzExposure(accom);

		m_TzTintOn = parts[0] == "tz_bloodmoon_rise" || parts[0].IndexOf("tz_chain") == 0 || parts[0] == "tz_kneel_ghoul" || parts[0] == "tz_feed2" || parts[0] == "tz_thumb" || parts[0] == "tz_poster_moon" || parts[0].IndexOf("tz_hook") == 0 || parts[0].IndexOf("tz_bite") == 0 || parts[0].IndexOf("tz_feast") == 0;
		if (parts[0].IndexOf("tz_feast") == 0)
		{


			GetGame().GetWorld().SetDate(2011, 11, 10, 22, 30);
			PlayerBase fme = PlayerBase.Cast(GetGame().GetPlayer());
			bool fbm = false;
			if (fme)
				fbm = fme.m_AZBloodMoon;
			Print("[AZ_Vamp] TEST tz feast client: blood moon " + fbm + " tint " + m_TzTintOn + " moon " + GetGame().GetWorld().GetMoonIntensity() + " overcast " + GetGame().GetWeather().GetOvercast().GetActual() + " sight off " + m_TzNoSight);
		}
		if (m_TzTintOn)
			m_AZMoonOn = true;
		m_TzHuman = parts[0].IndexOf("tz_hpov") == 0 || parts[0].IndexOf("tz_hook") == 0 || parts[0].IndexOf("tz_hunt") == 0;

		m_TzNoSight = m_TzHuman || snarl || parts[0] == "tz_feast_a" || parts[0].IndexOf("tz_bite") == 0 || parts[0].IndexOf("tz_story") == 0 || parts[0] == "tz_feed2" || parts[0].IndexOf("tz_ld_") == 0 || parts[0].IndexOf("tz_poster") == 0;
		m_TzShake = 0;
		if (parts[0] == "tz_hpov")
			m_TzShake = 0.012;
		if (parts[0] == "tz_hpov_snap")
			m_TzShake = 0.02;
		if (parts[0] == "tz_hpov_elder" || parts[0] == "tz_hook_pov")
			m_TzShake = 0.008;
		if (parts[0] == "tz_hpov_turn")
			m_TzShake = 0.045;
		TzTorch(m_TzHuman);
		if (!m_TzNoSight && me)
			me.m_AZVampSightSet = -1;
		m_TzTintK = 0;
		if (parts[0] == "tz_bloodmoon_rise")
			m_TzTintK = -2.0;
		if (parts[0] == "tz_eyes_ecu")
			TzEyesFrame();
		TzHud(false);
		super.VampHwShot(parts[0], m_TzCam0, m_TzLook0);
		if (m_VampCam)
			m_VampCam.SetFOV(m_TzFov);
		Print("[AZ_Vamp] TEST tz move " + parts[0] + " " + m_TzDur + " s");
	}



	void TzHud(bool show)
	{
		Hud hud = GetHud();
		if (hud)
		{
			hud.ShowHud(show);
			hud.ShowQuickBar(show);
		}
		if (m_HudRootWidget)
			m_HudRootWidget.Show(show);


		TzNotifications(show);
		if (!show)
		{
			HideDebugMonitor();
			TzQuality();
		}
	}


	void TzNotifications(bool show)
	{
		WorkspaceWidget ws = GetGame().GetWorkspace();
		if (!ws)
			return;
		Widget n = ws.FindAnyWidget("NotificationRoot");
		if (n && n.IsVisible() != show)
			n.Show(show);
	}



	protected bool m_TzQualitySet;
	void TzQuality()
	{
		if (m_TzQualitySet)
			return;
		m_TzQualitySet = true;
		GameOptions o = new GameOptions();
		int maxed[] = {OptionAccessType.AT_OBJECTS_DETAIL, OptionAccessType.AT_TEXTURE_DETAIL, OptionAccessType.AT_SHADOW_DETAIL,
			OptionAccessType.AT_WATER_DETAIL, OptionAccessType.AT_HDR_DETAIL, OptionAccessType.AT_FSAA_DETAIL, OptionAccessType.AT_ANISO_DETAIL,
			OptionAccessType.AT_ATOC_DETAIL, OptionAccessType.AT_AMBIENT_OCCLUSION, OptionAccessType.AT_POSTPROCESS_EFFECTS,
			OptionAccessType.AT_QUALITY_PREFERENCE, OptionAccessType.AT_OPTIONS_TERRAIN, OptionAccessType.AT_OPTIONS_FXAA_VALUE,
			OptionAccessType.AT_OPTIONS_VISIBILITY_COMBO, OptionAccessType.AT_OPTIONS_OBJECT_VISIBILITY_COMBO,
			OptionAccessType.AT_OPTIONS_TERRAIN_SHADER};
		foreach (int t : maxed)
			TzOption(o, t, true);
		TzOption(o, OptionAccessType.AT_ROTATION_BLUR, false);
		TzOption(o, OptionAccessType.AT_VSYNC_VALUE, false);
		o.Apply();
		Print("[AZ_Vamp] TEST tz capture quality applied");
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(TzProfileLine, 1500, false);
	}


	void TzProfileLine()
	{
		GameOptions o = new GameOptions();
		int types[] = {OptionAccessType.AT_OBJECTS_DETAIL, OptionAccessType.AT_TEXTURE_DETAIL, OptionAccessType.AT_SHADOW_DETAIL,
			OptionAccessType.AT_WATER_DETAIL, OptionAccessType.AT_HDR_DETAIL, OptionAccessType.AT_FSAA_DETAIL, OptionAccessType.AT_ANISO_DETAIL,
			OptionAccessType.AT_ATOC_DETAIL, OptionAccessType.AT_AMBIENT_OCCLUSION, OptionAccessType.AT_POSTPROCESS_EFFECTS,
			OptionAccessType.AT_QUALITY_PREFERENCE, OptionAccessType.AT_OPTIONS_TERRAIN, OptionAccessType.AT_OPTIONS_FXAA_VALUE,
			OptionAccessType.AT_OPTIONS_VISIBILITY_COMBO, OptionAccessType.AT_OPTIONS_OBJECT_VISIBILITY_COMBO,
			OptionAccessType.AT_OPTIONS_TERRAIN_SHADER};
		string line = "";
		foreach (int t : types)
		{
			ListOptionsAccess la = ListOptionsAccess.Cast(o.GetOptionByType(t));
			SwitchOptionsAccess sa = SwitchOptionsAccess.Cast(o.GetOptionByType(t));
			if (la)
				line += " " + t + "=" + la.GetIndex() + "/" + la.GetItemsCount();
			else if (sa)
				line += " " + t + "=" + sa.GetIndex() + "/2";
			else
				line += " " + t + "=na";
		}
		SwitchOptionsAccess rb = SwitchOptionsAccess.Cast(o.GetOptionByType(OptionAccessType.AT_ROTATION_BLUR));
		ListOptionsAccess rbl = ListOptionsAccess.Cast(o.GetOptionByType(OptionAccessType.AT_ROTATION_BLUR));
		if (rb)
			line += " rotblur=" + rb.GetIndex();
		else if (rbl)
			line += " rotblur=" + rbl.GetIndex();
		NumericOptionsAccess rbn = NumericOptionsAccess.Cast(o.GetOptionByType(OptionAccessType.AT_ROTATION_BLUR));
		if (rbn)
			line += " rotblur=" + rbn.ReadValue();
		float fov = -1;
		NumericOptionsAccess fa = NumericOptionsAccess.Cast(o.GetOptionByType(OptionAccessType.AT_OPTIONS_FIELD_OF_VIEW));
		if (fa)
			fov = fa.ReadValue();
		int sw, sh;
		GetScreenSize(sw, sh);
		Print("[AZ_Vamp] TEST tz profile options" + line + " fov " + fov + " screen " + sw + "x" + sh);
	}

	void TzOption(GameOptions o, int type, bool top)
	{
		OptionsAccess a = o.GetOptionByType(type);
		ListOptionsAccess la = ListOptionsAccess.Cast(a);
		NumericOptionsAccess na = NumericOptionsAccess.Cast(a);
		SwitchOptionsAccess sa = SwitchOptionsAccess.Cast(a);
		if (la)
		{
			int i = 0;
			if (top)
				i = la.GetItemsCount() - 1;
			la.SetIndex(i);
			Print("[AZ_Vamp] TEST tz option " + type + " list -> " + i);
		}
		else if (na)
		{
			float v = na.GetMin();
			if (top)
				v = na.GetMax();
			na.WriteValue(v);
			Print("[AZ_Vamp] TEST tz option " + type + " value -> " + v);
		}
		else if (sa)
		{
			bool on = sa.GetIndex() != 0;
			if (on != top)
				sa.Switch();
			Print("[AZ_Vamp] TEST tz option " + type + " switch -> " + top);
		}
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		if (m_TzUsed && !m_TzOn)
			HideDebugMonitor();
		if (!m_TzOn || !m_VampCam)
			return;
		HideDebugMonitor();
		if (m_HudRootWidget && m_HudRootWidget.IsVisible())
			m_HudRootWidget.Show(false);
		TzNotifications(false);
		m_TzT += timeslice;
		float s = Math.Clamp(m_TzT / m_TzDur, 0, 1);
		s = s * s * (3 - 2 * s);
		vector c = m_TzCam0 + (m_TzCam1 - m_TzCam0) * s;
		vector l = m_TzLook0 + (m_TzLook1 - m_TzLook0) * s;
		if (m_TzTintOn)
		{


			m_TzTintK = Math.Min(1.0, m_TzTintK + timeslice / 1.5);
			AZ_PPETzTint.Get().SetTint(Math.Max(0, m_TzTintK));
		}
		if (m_TzNoSight)
			AZ_PPEVampSight.Get().SetSight(0);
		if (m_TzHuman)
		{

			float tt = m_TzT;
			vector shake = Vector(Math.Sin(tt * 7.3) + 0.5 * Math.Sin(tt * 19.0), Math.Sin(tt * 9.1 + 1.0) + 0.4 * Math.Sin(tt * 23.0), Math.Sin(tt * 6.1 + 2.0));
			c = c + shake * m_TzShake;
			vector lshake = Vector(Math.Sin(tt * 5.3 + 0.5), Math.Sin(tt * 4.7 + 1.5), 0);
			l = l + lshake * (m_TzShake * 3.0);
		}
		if (m_TzHead)
		{
			PlayerBase me = PlayerBase.Cast(GetGame().GetPlayer());
			if (me)
				l = me.GetBonePositionWS(me.GetBoneIndexByName("Head")) + "0 0.06 0";
		}
		m_VampCam.SetPosition(c);
		m_VampCam.LookAt(l);
		m_VampCam.SetFOV(m_TzFov);
		if (m_TzTorch)
		{
			vector beam = l - c;
			beam.Normalize();
			vector right = Vector(beam[2], 0, -beam[0]);
			vector tp = c + right * 0.18 - "0 0.12 0";
			m_TzTorch.SetPosition(tp);
			m_TzTorch.SetDirection(beam);
		}
	}
}





class AZ_TzView
{
	static bool Clear(vector look, vector cam)
	{
		vector from = look;
		vector dir = cam - look;
		dir.Normalize();
		float full = vector.Distance(cam, look);
		for (int n = 0; n < 10; n++)
		{
			vector hit, nrm;
			int comp;
			set<Object> res = new set<Object>;
			if (!DayZPhysics.RaycastRV(from, cam, hit, nrm, comp, res, null, null, false, false, ObjIntersectView, 0.2))
				return true;
			bool body = false;
			foreach (Object o : res)
			{
				if (o && (o.IsMan() || o.IsInherited(DayZCreature)))
					body = true;
			}
			if (!body)
				return false;
			from = hit + dir * 0.4;
			if (vector.Distance(from, look) >= full)
				return true;
		}
		return true;
	}
}



class AZ_PPETzTint extends PPERequester_GameplayBase
{
	void SetTint(float k)
	{
		if (k <= 0)
		{
			if (IsRequesterRunning())
				Stop();
			return;
		}
		if (!IsRequesterRunning())
			Start();
		SetTargetValueColor(PostProcessEffectType.Glow, PPEGlow.PARAM_COLORIZATIONCOLOR, {1.0, 1.0 - 0.40 * k, 1.0 - 0.45 * k, 0.0}, PPEGlow.L_23_TOXIC_TINT, PPOperators.MULTIPLICATIVE);
	}

	static AZ_PPETzTint Get()
	{
		AZ_PPETzTint r = AZ_PPETzTint.Cast(PPERequesterBank.GetRequester(AZ_PPETzTint));
		if (!r)
		{
			PPERequesterBank.RegisterRequester(AZ_PPETzTint);
			r = AZ_PPETzTint.Cast(PPERequesterBank.GetRequester(AZ_PPETzTint));
		}
		return r;
	}
}
