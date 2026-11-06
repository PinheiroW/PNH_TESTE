









class AZ_TrMoon
{

	static void Dark()
	{
		World w = GetGame().GetWorld();
		int best = 25;
		float lo = 2;
		for (int d = 1; d <= 30; d++)
		{
			w.SetDate(2011, 11, d, 22, 30);
			if (w.GetMoonIntensity() < lo)
			{
				lo = w.GetMoonIntensity();
				best = d;
			}
		}
		w.SetDate(2011, 11, best, 22, 30);
		Print("[AZ_Vamp] TEST tr moon: Nov " + best + " 22:30, intensity " + w.GetMoonIntensity());
	}



	static void Bright()
	{
		World w = GetGame().GetWorld();
		int best = 15;
		float hi = -1;
		for (int d = 1; d <= 30; d++)
		{
			w.SetDate(2011, 11, d, 23, 30);
			if (w.GetMoonIntensity() > hi)
			{
				hi = w.GetMoonIntensity();
				best = d;
			}
		}
		w.SetDate(2011, 11, best, 23, 30);
		Print("[AZ_Vamp] TEST tr moon bright: Nov " + best + " 23:30, intensity " + w.GetMoonIntensity());
	}


	static void Day()
	{
		GetGame().GetWorld().SetDate(2011, 11, 12, 11, 0);
		Print("[AZ_Vamp] TEST tr day: Nov 12 11:00");
	}
}

modded class MissionServer
{

	static const int TR_N = 15;
	static const string TR_LETTERS = "abcdefghijklmno";
	static const int TR_FRENZY_N = 22;
	static const string TR_FRENZY_LETTERS = "abcdefghijklmnopqrstuv";
	static const int TR_FRENZY_MS = 24000;
	static const int TR_FLAME_N = 5;
	static const int TR_FLAME_MS = 16000;
	static const int TR_MOON_N = 6;
	static const int TR_MOON_MS = 16000;
	static const int TR_FIN_N = 18;
	static const int TR_FIN_MS = 17000;
	static const int TR_STAKE_N = 6;
	static const int TR_STAKE_MS = 17000;
	static ref array<vector> s_TrPlaced;
	static Object s_TrB;
	static bool s_TrDone;
	static vector s_TrDoor, s_TrOut, s_TrIn, s_TrFireAt;
	static bool s_TrHouse;
	static PlayerBase s_TrCougher;

	static const vector TR_PIN = "8150.37 0 10818.93";
	static bool s_TrPinOn;

	string TrBase(int step)
	{
		if (step == 472 || step == 473)
			return "tz_tr_fin";
		if (step == 474 || step == 475)
			return "tz_tr_stake";
		if (step == 476 || step == 477)
			return "tz_tr_moon";
		if (step == 480 || step == 481)
			return "tz_tr_fire";
		if (step == 482 || step == 483)
			return "tz_tr_eyes";
		if (step == 484 || step == 485)
			return "tz_tr_win";
		if (step == 486 || step == 487)
			return "tz_tr_torch";
		if (step == 488 || step == 489)
			return "tz_tr_stalk";
		if (step == 490 || step == 491)
			return "tz_tr_bite";
		if (step == 492 || step == 493)
			return "tz_tr_repel";
		if (step == 494 || step == 495)
			return "tz_tr_orb";
		if (step == 496 || step == 497)
			return "tz_tr_household";
		if (step == 498 || step == 499)
			return "tz_tr_frenzy";
		return "";
	}

	string TrLabels(string b)
	{
		string res = "";
		int n = TR_N;
		if (b == "tz_tr_frenzy")
			n = TR_FRENZY_N;
		if (b == "tz_tr_moon")
			n = TR_MOON_N;
		if (b == "tz_tr_fin")
			n = TR_FIN_N;
		if (b == "tz_tr_stake")
			n = TR_STAKE_N;
		string lt = TR_LETTERS;
		if (b == "tz_tr_frenzy" || b == "tz_tr_fin")
			lt = TR_FRENZY_LETTERS;
		for (int i = 0; i < n; i++)
		{
			if (i > 0)
				res += "|";
			res += b + "_" + lt.Substring(i, 1);
		}
		return res;
	}

	override string HwStepLabel(int step)
	{
		if (step >= 472 && step <= 499)
			return TrLabels(TrBase(step));
		string l = super.HwStepLabel(step);
		if (step == 100 || step == 130)
		{
			for (int st = 472; st <= 498; st += 2)
				l += "|" + TrLabels(TrBase(st));
		}
		return l;
	}

	override int HwNextStep(int step)
	{
		if (step >= 472 && step < 499)
			return step + 1;
		if (step == 499)
			return 15;
		int n = super.HwNextStep(step);
		if (n == 15 && s_HwOnly && !s_TrDone && step < 472)
		{
			s_TrDone = true;
			return 472;
		}
		return n;
	}

	override void HwStep(PlayerBase p, int step)
	{
		if (step < 472 || step > 499)
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
		int next = 1000;
		if (step % 2 == 1)
		{
			if (step == 475)
			{
				for (int sk = 0; sk < TR_STAKE_N; sk++)
				{
					GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TrHoldSetup, sk * TR_STAKE_MS + 10, false, p, sk);
					GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TrHoldShot, sk * TR_STAKE_MS + 7000, false, p, sk);
				}
				next = TR_STAKE_N * TR_STAKE_MS + 500;
			}
			else if (step == 473)
			{
				for (int fn = 0; fn < TR_FIN_N; fn++)
				{
					GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TrFinSetup, fn * TR_FIN_MS + 10, false, p, fn);
					GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TrFinShot, fn * TR_FIN_MS + 7000, false, p, fn);
				}
				next = TR_FIN_N * TR_FIN_MS + 500;
			}
			else if (step == 477)
			{
				for (int lv = 0; lv < TR_MOON_N; lv++)
				{
					GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TrMoonSetup, lv * TR_MOON_MS + 10, false, p, lv);
					GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TrMoonShot, lv * TR_MOON_MS + 8000, false, p, lv);
				}
				next = TR_MOON_N * TR_MOON_MS + 500;
			}
			else if (step == 499)
			{
				for (int zv = 0; zv < TR_FRENZY_N; zv++)
				{
					GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TrFrenzySetup, zv * TR_FRENZY_MS + 10, false, p, zv);
					GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TrFrenzyShot, zv * TR_FRENZY_MS + 9000, false, p, zv);
				}
				next = TR_FRENZY_N * TR_FRENZY_MS + 500;
			}
			else if (step == 491)
			{


				for (int bv = 0; bv < TR_N; bv++)
				{
					GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TrBiteActors, bv * 9000 + 10, false, p);
					GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TrShotV, bv * 9000 + 2000, false, p, step, bv);
				}
				next = TR_N * 9000 + 500;
			}
			else
			{
				TrShotV(p, step, 0);
				for (int fv = 1; fv < TR_N; fv++)
					GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TrShotV, fv * 7500, false, p, step, fv);
				next = TR_N * 7500 + 500;
			}
		}
		else
			next = TrSetup(p, step);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, next, false, p, HwNextStep(step));
	}



	bool TrFindHouse()
	{
		if (s_TrHouse)
			return true;
		if (s_TzFireAt.Length() < 1)
			LdFire();
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition(s_TzFireAt, 400, objs, null);
		int doored = 0;
		Building best;
		float bd = 1000000;
		vector bestDoor;
		foreach (Object o : objs)
		{
			Building bb = Building.Cast(o);
			if (!bb || bb.GetDoorCount() == 0)
				continue;
			if (s_TrPinOn && TzDistXZ(bb.GetPosition(), TR_PIN) > 1.0)
				continue;
			doored++;
			if (!AZ_VampDecor.IsHome(bb.GetType()) || bb.GetType().Contains("Barn") || bb.GetType().Contains("barn"))
				continue;
			vector bc = bb.GetPosition();
			float far = 0;
			vector farDoor;
			for (int d = 0; d < bb.GetDoorCount(); d++)
			{
				vector dp = bb.GetDoorSoundPos(d);
				vector f0 = dp - bc;
				f0[1] = 0;
				if (f0.Length() > far && dp[1] - GetGame().SurfaceY(dp[0], dp[2]) < 2.5)
				{
					far = f0.Length();
					farDoor = dp;
				}
			}
			if (far < 2.0)
				continue;
			float dist = TzDistXZ(bc, s_TzFireAt);
			vector hq, hqr;
			if (!TrHearth(bb, hq, hqr))
				dist += 2000;
			if (dist < bd)
			{
				bd = dist;
				best = bb;
				bestDoor = farDoor;
			}
		}
		if (!best)
		{
			Print("[AZ_Vamp] TEST tr house: none within 400 m of " + s_TzFireAt + " (" + doored + " doored buildings, " + objs.Count() + " objects)");
			return false;
		}
		for (int k = 0; k < best.GetDoorCount(); k++)
			best.OpenDoor(k);
		s_TrB = best;
		vector c = best.GetPosition();
		s_TrOut = bestDoor - c;
		s_TrOut[1] = 0;
		s_TrOut.Normalize();
		s_TrDoor = bestDoor - "0 1.0 0";
		s_TrIn = s_TrDoor - s_TrOut * 2.2;
		s_TrFireAt = s_TrDoor - s_TrOut * 3.6;
		if (!s_TzActors)
			s_TzActors = new array<Object>;
		s_TrPlaced = new array<vector>;
		s_TrFireAt = TrSpot(s_TrFireAt);
		vector fd;
		if (TrFloor(s_TrDoor - s_TrOut * 0.3 + "0 0.5 0", fd))
			s_TrDoor = fd;
		s_TrPlaced.Clear();
		Object fp;
		vector hp, hr;
		if (TrHearth(best, hp, hr))
		{

			FireplaceIndoor fi = FireplaceIndoor.Cast(GetGame().CreateObjectEx("FireplaceIndoor", hp, ECE_KEEPHEIGHT));
			if (fi)
			{
				fi.SetPosition(hp);
				fi.SetFirePointIndex(0);
				fi.SetSmokePointPosition(best.ModelToWorld(best.GetSelectionPositionMS(FireplaceIndoor.FIREPOINT_SMOKE_POSITION + "0")));
				ItemBase wood = ItemBase.Cast(fi.GetInventory().CreateAttachment("Firewood"));
				if (wood)
					wood.SetQuantity(wood.GetQuantityMax());
				fi.GetInventory().CreateAttachment("Paper");
				fi.StartFire(true);
				s_TzActors.Insert(fi);
				fp = fi;
			}
			s_TrFireAt = hp;
			s_TrFireAt[1] = s_TrDoor[1];
		}
		else
		{
			Fireplace ff = Fireplace.Cast(GetGame().CreateObjectEx("Fireplace", s_TrFireAt, ECE_KEEPHEIGHT));
			if (ff)
			{
				ff.SetPosition(s_TrFireAt);
				TzLightFire(ff);
				s_TzActors.Insert(ff);
				fp = ff;
			}
		}
		s_TrPlaced.Insert(s_TrFireAt);

		s_TrHouse = true;
		Print("[AZ_Vamp] TEST tr house " + best.GetType() + " at " + c + " door " + s_TrDoor + " out " + s_TrOut + " fire " + fp + " " + bd + " m from the village fire");
		return true;
	}


	bool TrHearth(Object b, out vector pos, out vector rot)
	{
		if (!BuildingWithFireplace.Cast(b) || !b.MemoryPointExists(FireplaceIndoor.FIREPOINT_FIRE_POSITION + "0"))
			return false;
		return FireplaceIndoor.CanPlaceFireplaceInSelectedSpot(b, 0, pos, rot);
	}



	bool TrFloor(vector at, out vector floor, Object ignore = null)
	{
		vector from = at + "0 1.3 0";
		vector to = at - "0 2.5 0";
		vector hit, nrm;
		int comp;
		set<Object> res = new set<Object>;
		if (!DayZPhysics.RaycastRV(from, to, hit, nrm, comp, res, null, ignore, false, false, ObjIntersectView))
			return false;
		if (nrm[1] < 0.85)
			return false;
		foreach (Object o : res)
		{
			if (o && o != s_TrB && o != ignore && !o.IsTree() && !o.IsBush())
				return false;
		}
		if (s_TrPlaced)
		{
			foreach (vector q : s_TrPlaced)
			{
				if (TzDistXZ(q, hit) < 0.9)
					return false;
			}
		}
		floor = hit;
		return true;
	}


	vector TrSpot(vector want, Object ignore = null)
	{
		vector f;
		for (int ring = 0; ring <= 4; ring++)
		{
			int n = 1;
			if (ring > 0)
				n = 8;
			for (int a = 0; a < n; a++)
			{
				float ang = a * Math.PI2 / 8;
				vector c = want + Vector(Math.Sin(ang), 0, Math.Cos(ang)) * (ring * 0.35);
				if (TrFloor(c, f, ignore))
				{
					if (!s_TrPlaced)
						s_TrPlaced = new array<vector>;
					s_TrPlaced.Insert(f);
					return f;
				}
			}
		}
		vector g = want;
		g[1] = GetGame().SurfaceY(g[0], g[2]);
		if (s_TrHouse && s_TrDoor[1] > g[1])
			g[1] = s_TrDoor[1];
		Print("[AZ_Vamp] TEST tr spot: no clear floor near " + want + ", terrain " + g);
		return g;
	}


	void TrSettle(Object o, vector want)
	{
		if (!o)
			return;
		vector at = TrSpot(want, o);
		o.SetPosition(at);
	}


	void TrSettleLast(vector want)
	{
		if (s_TzActors && s_TzActors.Count() > 0)
			TrSettle(s_TzActors[s_TzActors.Count() - 1], want);
	}



	void TrHeroWinter()
	{
		if (!s_TzHero)
			return;
		EntityAI bag = s_TzHero.FindAttachmentBySlotName("Back");
		if (bag)
			GetGame().ObjectDelete(bag);
		EntityAI top = s_TzHero.FindAttachmentBySlotName("Body");
		if (top)
			GetGame().ObjectDelete(top);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TrHeroJacket, 100, false, s_TzHero);
	}

	void TrHeroJacket(PlayerBase h)
	{
		if (h && !h.FindAttachmentBySlotName("Body"))
			TzWear(h, "DownJacket_Blue");
	}


	void TrAway(PlayerBase p, vector from, vector dir)
	{
		vector at = from + dir * 40.0;
		at[1] = GetGame().SurfaceY(at[0], at[2]);
		TzPut(p, at, from);
	}

	int TrSetup(PlayerBase p, int step)
	{
		s_TrPinOn = step == 492 || step == 496;
		AZ_TrMoon.Dark();
		LdWeather(0.14, 0.6, 0.0);
		TzBloodMoon(p, false);
		s_TrPlaced = new array<vector>;
		if (step == 494)
			return TrSetOrb(p);
		if (step == 472 || step == 474)
		{
			TzClearActors();
			s_TrHouse = false;
			return 500;
		}
		if (step == 476)
		{
			TzClearActors();
			s_TrHouse = false;
			return 500;
		}
		if (step == 498)
		{
			TzClearActors();
			s_TrHouse = false;
			return 500;
		}
		if (step == 496)
		{
			if (!TrFindHouse())
				return 500;
			return TrSetHousehold(p);
		}
		if (step == 480)
			return TrSetFire(p);
		if (step == 482)
			return TrSetEyes(p);
		if (!TrFindHouse())
			return 500;
		if (step == 484)
			return TrSetWin(p);
		if (step == 486)
			return TrSetTorch(p);
		if (step == 488)
			return TrSetStalk(p);
		if (step == 490)
			return TrSetBite(p);
		if (step == 492)
			return TrSetRepel(p);
		return 1000;
	}



	int TrSetFire(PlayerBase p)
	{
		TzClearActors();
		s_TrHouse = false;
		if (!TrFindHouse())
			return 500;
		vector o = s_TrOut;
		vector r = Vector(o[2], 0, -o[0]);
		vector f = s_TrFireAt;
		TrSettle(LdSurvivor("SurvivorM_Boris", f + o * 0.8 - r * 0.9, f, 1), f + o * 0.8 - r * 0.9);
		TrSettle(LdSurvivor("SurvivorF_Irena", f - o * 0.6 + r * 1.1, f, 2), f - o * 0.6 + r * 1.1);
		TrSettle(LdSurvivor("SurvivorM_Denis", f - o * 0.6 - r * 1.1, f, 3), f - o * 0.6 - r * 1.1);


		vector tp = TrSpot(f + o * 0.8 + r * 0.9);
		s_TrCougher = TzEva(tp, f);
		if (s_TrCougher)
		{
			s_TrCougher.SetPosition(tp);
			s_TrCougher.SetHealth("", "Blood", AZ_VampState.Cfg().fullBlood);
			s_TrCougher.m_AZVampFrenzyUntil = 0;
			s_TrCougher.m_AZVampBareUntil = 0;
			AZ_VampReveal.Update(s_TrCougher);
		}
		TrAway(p, s_TrDoor, s_TrOut);
		Print("[AZ_Vamp] TEST tr fire at " + s_TrFireAt + " cougher " + s_TrCougher);
		return 6000;
	}


	int TrSetEyes(PlayerBase p)
	{
		p.m_AZVampFrenzyUntil = 0;
		p.m_AZVampBareUntil = 0;
		AZ_VampReveal.Update(p);
		return 1500;
	}


	void TrTurn(PlayerBase e, vector cam, int step)
	{
		if (!e)
			return;
		vector ff = s_TrFireAt - e.GetPosition();
		ff[1] = 0;
		ff.Normalize();
		vector tc = cam - e.GetPosition();
		tc[1] = 0;
		tc.Normalize();
		float k = step / 14.0;
		k = k * k * (3 - 2 * k);
		vector d = ff * (1 - k) + tc * k;
		d.Normalize();
		e.SetDirection(d);
	}

	void TrCough(PlayerBase p)
	{
		if (p && p.GetSymptomManager())
			p.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_COUGH);
	}

	void TrEyesOn(PlayerBase p)
	{
		if (!p)
			return;
		p.SetHealth("", "Blood", AZ_VampState.Cfg().fullBlood);
		p.m_AZVampFrenzyUntil = 0;
		AZ_VampReveal.BareAtWill(p, 3);
		Print("[AZ_Vamp] TEST tr eyes on (bare)");
	}


	int TrSetWin(PlayerBase p)
	{
		TzClearActors();
		s_TrHouse = false;
		TrFindHouse();
		TrAway(p, s_TrDoor, s_TrOut * -1);
		TzBloodMoon(p, true);
		return 14000;
	}



	int TrSetTorch(PlayerBase p)
	{
		TzHero(s_TrDoor - s_TrOut * 0.9, s_TrOut * -1, true);
		TrSettle(s_TzHero, s_TrDoor - s_TrOut * 0.9);
		TrHeroWinter();
		return 4000;
	}



	int TrSetStalk(PlayerBase p)
	{
		TzHero(s_TrIn, s_TrOut * -1, false);
		TrSettle(s_TzHero, s_TrIn);
		TrHeroWinter();
		vector r = Vector(s_TrOut[2], 0, -s_TrOut[0]);
		vector ev = s_TrDoor + s_TrOut * 0.6;
		TrSettle(TzEva(ev, s_TrIn), ev);
		vector e1 = s_TrDoor + s_TrOut * 2.2 + r * 1.1;
		TzElder(e1, s_TrIn, false);
		TrSettleLast(e1);
		vector e2 = s_TrDoor + s_TrOut * 2.4 - r * 1.1;
		TzElder(e2, s_TrIn, true);
		TrSettleLast(e2);
		return 5000;
	}


	int TrSetBite(PlayerBase p)
	{
		TzClearActors();
		s_TrHouse = false;
		TrFindHouse();
		TrBiteActors(p);
		return 4000;
	}


	void TrBiteActors(PlayerBase p)
	{
		if (!p || !s_TrHouse)
			return;
		if (s_TzActors)
		{
			for (int ai = s_TzActors.Count() - 1; ai >= 0; ai--)
			{
				if (PlayerBase.Cast(s_TzActors[ai]))
				{
					GetGame().ObjectDelete(s_TzActors[ai]);
					s_TzActors.Remove(ai);
				}
			}
		}
		s_TzHero = null;
		s_TrPlaced = new array<vector>;
		s_TrPlaced.Insert(s_TrFireAt);


		vector hat = s_TrFireAt + s_TrOut * 1.2;
		TzHero(hat, s_TrOut, false);
		TrSettle(s_TzHero, hat);
		TrHeroWinter();
		if (s_TzHero)
		{
			hat = s_TzHero.GetPosition();
			string fed = TzFeedEva(s_TzHero);
			Print("[AZ_Vamp] TEST tr bite: " + fed);
			array<Object> near = new array<Object>;
			GetGame().GetObjectsAtPosition(hat, 2.5, near, null);
			foreach (Object o : near)
			{
				PlayerBase pv = PlayerBase.Cast(o);
				if (pv && pv != s_TzHero && pv != p && pv.m_AZVampire)
				{
					if (s_TzActors.Find(pv) < 0)
						s_TzActors.Insert(pv);
					TzDressVampire(pv, !pv.IsMale());
				}
			}
			foreach (Object fo : s_TzActors)
			{
				PlayerBase fpb = PlayerBase.Cast(fo);
				if (fpb && fpb != s_TzHero && TzDistXZ(fpb.GetPosition(), hat) < 2.5)
				{
					vector fpp = fpb.GetPosition();
					fpp[1] = hat[1];
					fpb.SetPosition(fpp);
				}
			}
		}
		TrAway(p, s_TrDoor, s_TrOut);
	}


	int TrSetRepel(PlayerBase p)
	{
		TzClearActors();
		s_TrHouse = false;
		TrFindHouse();
		TzHero(s_TrDoor - s_TrOut * 1.6, s_TrOut, true);
		TrSettle(s_TzHero, s_TrDoor - s_TrOut * 1.6);
		TrHeroWinter();
		vector at = TrSpot(s_TrDoor + s_TrOut * 0.3);
		TzPut(p, at, s_TrDoor - s_TrOut * 2.0);
		TzDressVampire(p, true);
		p.SetHealth("", "Blood", AZ_VampState.Cfg().fullBlood);
		p.m_AZVampFrenzyUntil = GetGame().GetTime() + 120000;
		AZ_VampReveal.Update(p);
		return 5000;
	}



	int TrSetHousehold(PlayerBase p)
	{
		TzClearActors();
		s_TrHouse = false;
		TrFindHouse();
		vector r = Vector(s_TrOut[2], 0, -s_TrOut[0]);
		vector mid = s_TrDoor + s_TrOut * 3.5;
		vector look = mid + s_TrOut * 6.0;
		TrSettle(TzEva(mid, look), mid);
		TzElder(mid + r * 1.2 - s_TrOut * 0.4, look, true);
		TrSettleLast(mid + r * 1.2 - s_TrOut * 0.4);
		TzElder(mid - r * 1.2 - s_TrOut * 0.4, look, false);
		TrSettleLast(mid - r * 1.2 - s_TrOut * 0.4);
		LdTorchAt(mid + s_TrOut * 1.4 + r * 0.9);
		LdTorchAt(mid + s_TrOut * 1.4 - r * 0.9);
		TrAway(p, s_TrDoor, s_TrOut * -1);
		return 5000;
	}

	void TrCamHousehold(int v, out vector cam, out vector look, out float fov)
	{
		vector r = Vector(s_TrOut[2], 0, -s_TrOut[0]);
		vector mid = s_TrDoor + s_TrOut * 3.5;
		float d[3] = {5.5, 2.6, 4.0};
		float h[3] = {1.4, 1.35, 0.5};
		float lh[3] = {1.0, 1.25, 1.3};
		float f[3] = {0.75, 0.6, 0.8};
		cam = mid + s_TrOut * d[v] + r * 0.3;
		cam[1] = GetGame().SurfaceY(cam[0], cam[2]) + h[v];
		look = mid + Vector(0, lh[v], 0);
		look[1] = GetGame().SurfaceY(mid[0], mid[2]) + lh[v];
		fov = f[v];
	}







	static vector s_TrFrD, s_TrFrO;
	static const float TR_HERO_OUT = 1.0;
	static const float TR_HER_OUT = 3.0;
	static float s_TrFrSp;

	void TrFrenzySetup(PlayerBase p, int v)
	{
		if (!p)
			return;
		TrEmptyHands(p);
		TzClearActors();
		TrClearSky();
		AZ_TrMoon.Bright();
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
		if (s_TzDoorAt.Length() < 1 && !TzChurchDoor())
		{
			Print("[AZ_Vamp] TEST tr frenzy " + v + ": no church door, skipped");
			return;
		}
		s_TrFrD = s_TzDoorAt;
		s_TrFrO = s_TzDoorOut;
		vector r = Vector(s_TrFrO[2], 0, -s_TrFrO[0]);
		LdTorchAt(s_TrFrD + s_TrFrO * 0.05 + r * 1.1);
		LdTorchAt(s_TrFrD + s_TrFrO * 0.05 - r * 1.1);


		TzHero(s_TrFrD + s_TrFrO * TR_HERO_OUT, s_TrFrO, false);
		TrHeroWinter();
		if (v >= 10 && s_TzHero)
		{
			ItemBase rf = ItemBase.Cast(s_TzHero.GetHumanInventory().CreateInHands("Roadflare"));
			if (rf && rf.GetCompEM())
				rf.GetCompEM().SwitchOn();
		}
		vector start;
		if (v < 10)
		{
			s_TrFrSp = 3.5;
			if (v % 2 == 1)
				s_TrFrSp = 4.5;
			float stag[4] = {0.9, -1.0, 0.6, -0.7};
			s_TrChainV = new array<PlayerBase>;
			for (int i = 3; i >= 0; i--)
			{
				vector h = s_TrFrD + s_TrFrO * (5.0 + i * s_TrFrSp) + r * stag[i];
				h[1] = GetGame().SurfaceY(h[0], h[2]);
				PlayerBase hv = TzHuman(h, s_TrFrO * -1);
				if (hv)
					s_TrChainV.Insert(hv);
			}
			start = s_TrFrD + s_TrFrO * (5.0 + 3 * s_TrFrSp + 5.0);
		}
		else
		{
			start = s_TrFrD + s_TrFrO * TR_HER_OUT;
			AZ_VampReveal.Fed(p);
		}
		start[1] = GetGame().SurfaceY(start[0], start[2]);
		s_TzChainAt = start;
		s_TzChainDir = s_TrFrO * -1;
		HwGo(p, start, s_TrFrD + "0 1.6 0");
		TrClearLights(s_TrFrD + s_TrFrO * 10.0, 60);
		Print("[AZ_Vamp] TEST tr frenzy " + v + " spacing " + s_TrFrSp + " door " + s_TrFrD + " start " + start);
	}





	static ref array<PlayerBase> s_TrChainV;

	void TrChainRun(PlayerBase p)
	{
		if (!s_TrChainV)
			return;
		for (int k = 0; k < s_TrChainV.Count(); k++)
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TrChainStep, 1200 + k * 2600, false, p, k);
	}

	void TrChainStep(PlayerBase p, int k)
	{
		if (!p || !s_TrChainV || k >= s_TrChainV.Count())
			return;
		PlayerBase v = s_TrChainV[k];
		if (!v || !v.IsAlive())
			return;
		vector from = p.GetPosition();
		vector dir = v.GetDirection();
		dir[1] = 0;
		dir.Normalize();
		vector land = v.GetPosition() - dir * AZ_VampFeed.ROOT_GAP;
		land[1] = GetGame().SurfaceY(land[0], land[2]);
		GetGame().RPCSingleParam(p, AZ_VampShadow.RPC_SHADOW, new Param1<vector>(from), true);
		p.SetPosition(land);
		p.SetDirection(dir);
		v.m_AZVampRear = true;
		AZ_VampFeed.Hold(p, v, false);
		v.AZ_VampPin(v.GetPosition(), v.GetDirection());
		AZ_VampReveal.BareAtWill(p, 3);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampShadow.BiteAnim, 200, false, p, 1, 3300, 1);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(TrChainEnd, 1500, false, p, v);
		Print("[AZ_Vamp] TEST tr chain shadow step " + (k + 1) + " -> " + v + " " + Math.Round(vector.Distance(from, land)) + " m");
	}

	void TrChainEnd(PlayerBase p, PlayerBase v)
	{
		if (!p || !v || !v.IsAlive())
			return;
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		v.AddHealth("", "Blood", -Math.Max(0, v.GetHealth("", "Blood") - cfg.bloodFloor));
		AZ_VampFeed.Collapse(p, v);
		AZ_VampShadow.BiteAnim(p, 2, 0, 0);
	}




	static vector s_TrC1, s_TrC2;

	void TrSafeCam(PlayerBase p, string vl, vector cam, vector look, vector cam2, vector look2)
	{
		cam = TzKeepOff(p, TzUnblock(cam, look));
		cam2 = TzKeepOff(p, TzUnblock(cam2, look2));
		for (int i = 1; i <= 5; i++)
		{
			float t = i / 5.0;
			vector pt = cam + (cam2 - cam) * t;
			vector lk = look + (look2 - look) * t;
			if (!AZ_TzView.Clear(lk, pt) || !AZ_TzView.Clear(cam + (cam2 - cam) * ((i - 1) / 5.0), pt))
			{
				cam2 = cam + (cam2 - cam) * ((i - 1) / 5.0);
				Print("[AZ_Vamp] TEST tr cam " + vl + ": path blocked at " + (t * 100) + "%, move cut short");
				break;
			}
		}
		float near = 9;
		vector dirs[6] = {"1 0 0", "-1 0 0", "0 0 1", "0 0 -1", "0 1 0", "0 -1 0"};
		for (int d = 0; d < 6; d++)
		{
			vector hit, nrm;
			int comp;
			set<Object> res = new set<Object>;
			if (DayZPhysics.RaycastRV(cam, cam + dirs[d] * 3.0, hit, nrm, comp, res, null, p, false, false, ObjIntersectView, 0.05))
				near = Math.Min(near, vector.Distance(cam, hit));
		}
		Print("[AZ_Vamp] TEST tr cam " + vl + ": nearest geometry " + near + " m (under 0.15 = likely clipping)");
		s_TrC1 = cam;
		s_TrC2 = cam2;
	}





	void TrClearSky()
	{
		Weather wt = GetGame().GetWeather();
		wt.GetFog().SetLimits(0, 1);
		wt.GetFog().Set(0, 0, 900);
		wt.GetOvercast().SetLimits(0, 1);
		wt.GetOvercast().Set(0, 0, 900);
		wt.GetRain().Set(0, 0, 900);
		AZ_VampMist.s_On = true;
		AZ_VampMist.s_NextAt = GetGame().GetTime() + 600000;


		if (s_TzDoorAt.Length() < 1)
			TzChurchDoor();
		if (s_TzDoorAt.Length() < 1 || !AZ_VampLandmarks.s_MistZones)
			return;
		int gone = 0;
		for (int i = AZ_VampLandmarks.s_MistZones.Count() - 1; i >= 0; i--)
		{
			Object z = AZ_VampLandmarks.s_MistZones[i];
			if (z && vector.Distance(z.GetPosition(), s_TzDoorAt) < 120)
			{
				GetGame().ObjectDelete(z);
				AZ_VampLandmarks.s_MistZones.Remove(i);
				gone++;
			}
		}
		if (gone > 0)
			Print("[AZ_Vamp] TEST tr mist: " + gone + " mist zones near the church removed");
	}

	void TrFrenzyShot(PlayerBase p, int v)
	{
		if (!p || s_TrFrO.Length() < 0.1)
			return;
		string vl = "tz_tr_frenzy_" + TR_FRENZY_LETTERS.Substring(v, 1);
		vector o = s_TrFrO;
		vector r = Vector(o[2], 0, -o[0]);
		vector cam, cam2, look, look2;
		float sd = 1.0;
		if ((v / 2) % 2 == 1)
			sd = -1.0;
		if (v < 10)
		{
			float len = 5.0 + 3 * s_TrFrSp;
			vector mid = s_TrFrD + o * (len * 0.6);
			mid[1] = GetGame().SurfaceY(mid[0], mid[2]);
			int cv = v % 3;
			vector start = s_TrFrD + o * (len + 5.0);
			start[1] = GetGame().SurfaceY(start[0], start[2]);
			if (cv == 0)
			{
				cam = start + o * 1.8 + r * (0.5 * sd) + "0 0.9 0";
				cam2 = start - o * (len * 0.7) + r * (0.5 * sd) + "0 0.9 0";
				look = s_TrFrD + "0 1.4 0";
				look2 = look;
			}
			else if (cv == 1)
			{
				int k = (v / 3) % 4;
				vector vic = s_TrFrD + o * (5.0 + k * s_TrFrSp);
				vic[1] = GetGame().SurfaceY(vic[0], vic[2]);
				cam = vic - o * 1.3 + r * (0.4 * sd) + "0 1.6 0";
				cam2 = vic - o * 1.1 + r * (0.35 * sd) + "0 1.58 0";
				look = vic + "0 1.55 0";
				look2 = look;
			}
			else
			{
				cam = mid + r * (6.5 * sd) + o * 2.5 + "0 7.5 0";
				cam2 = mid + r * (5.5 * sd) + "0 6.5 0";
				look = mid;
				look2 = s_TrFrD;
			}
			if (cv != 1)
			{
				cam = TzKeepOff(p, TzUnblock(cam, look));
				cam2 = TzKeepOff(p, TzUnblock(cam2, look2));
			}
			TrSafeCam(p, vl, cam, look, cam2, look2);
			TzShot(p, vl, s_TrC1, look, s_TrC2, look2, 13);
			TrChainRun(p);
			return;
		}
		vector her = p.GetPosition();
		vector him = s_TrFrD + o * TR_HERO_OUT;
		if (s_TzHero)
			him = s_TzHero.GetPosition();
		float sd2 = 1.0;
		if (v % 2 == 1)
			sd2 = -1.0;
		if (v < 16)
		{
			float back = 0.3 + ((v - 10) / 2) * 0.1;
			cam = him - o * back + r * (0.4 * sd2) + "0 1.85 0";
			cam2 = cam + o * 0.15;
			look = her + "0 1.5 0";
			look2 = look;
		}
		else
		{
			vector hd = her + "0 1.6 0";
			float st = 1.3 + ((v - 16) / 2) * 0.25;
			cam = hd - o * st + r * (0.3 * sd2);
			cam2 = hd - o * 0.7 + r * (0.12 * sd2);
			look = hd;
			look2 = hd;
		}
		TrSafeCam(p, vl, cam, look, cam2, look2);
		TzShot(p, vl, s_TrC1, look, s_TrC2, look2, 8);
		bool sees = AZ_TzView.Clear(her + "0 1.6 0", him + "0 1.4 0");
		bool shown = AZ_TzView.Clear(s_TrC1, him + "0 1.5 0");
		Print("[AZ_Vamp] TEST tr stop " + vl + ": her line of sight to him " + sees + ", camera sees him " + shown + ", him " + vector.Distance(him, s_TrFrD) + " m from the wall point");
	}





	bool TrIsLight(Object o)
	{
		return AZ_VampPumpkinLantern.Cast(o) || Candle.Cast(o) || Torch.Cast(o) || AZ_VampLitDecor.Cast(o) || FireplaceBase.Cast(o) || Roadflare.Cast(o);
	}

	void TrClearLights(vector c, float rad)
	{
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition(c, rad, objs, null);
		int gone = 0;
		foreach (Object o : objs)
		{
			if (!o || (s_TzActors && s_TzActors.Find(o) >= 0) || FireplaceBase.Cast(o))
				continue;
			if (TrIsLight(o))
			{
				GetGame().ObjectDelete(o);
				gone++;
			}
		}
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TrCountLights, 1500, false, c, gone);
	}

	void TrCountLights(vector c, int gone)
	{
		array<Object> near = new array<Object>;
		GetGame().GetObjectsAtPosition(c, 40, near, null);
		int n = 0;
		string what = "";
		foreach (Object o : near)
		{
			if (o && TrIsLight(o))
			{
				n++;
				what += " " + o.GetType();
			}
		}
		Print("[AZ_Vamp] TEST tr lights: " + gone + " decor lights removed within 60 m; " + n + " light sources within 40 m:" + what);
	}



	void TrEmptyHands(PlayerBase p)
	{
		EntityAI hh = p.GetHumanInventory().GetEntityInHands();
		if (hh)
			GetGame().ObjectDelete(hh);
	}






	static PlayerBase s_TrStakeV;
	static Object s_TrStakeObj;




	static const float TR_EVA_OUT = 8.0;

	vector TrChurchSpot()
	{
		if (s_TzDoorAt.Length() < 1 && !TzChurchDoor())
			return vector.Zero;
		vector d0 = s_TzDoorAt;
		vector o = s_TzDoorOut;
		vector r = Vector(o[2], 0, -o[0]);
		float outs[4] = {11.0, 14.0, 17.0, 20.0};
		float sides[5] = {5.0, -5.0, 8.0, -8.0, 0.0};
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				vector g = d0 + o * outs[i] + r * sides[j];
				g[1] = GetGame().SurfaceY(g[0], g[2]);
				bool ok = true;
				for (int k = 0; k < 8 && ok; k++)
				{
					float a = k * Math.PI2 / 8;
					vector dd = Vector(Math.Sin(a), 0, Math.Cos(a));
					vector e = g + dd * 4.0;
					e[1] = GetGame().SurfaceY(e[0], e[2]);
					if (Math.AbsFloat(e[1] - g[1]) > 0.8 || !AZ_TzView.Clear(g + "0 0.5 0", e + "0 0.5 0") || !AZ_TzView.Clear(g + "0 1.6 0", e + "0 1.6 0"))
						ok = false;
				}
				if (ok)
				{
					Print("[AZ_Vamp] TEST tr church spot " + outs[i] + " m out, " + sides[j] + " m aside: " + g);
					return g;
				}
			}
		}
		vector fb = d0 + o * 14.0;
		fb[1] = GetGame().SurfaceY(fb[0], fb[2]);
		Print("[AZ_Vamp] TEST tr church spot: no clear spot, fallback " + fb);
		return fb;
	}


	Fireplace TrChurchFire()
	{
		vector g = TrChurchSpot();
		if (g.Length() < 1)
			return null;
		Fireplace fire = Fireplace.Cast(GetGame().CreateObjectEx("Fireplace", g, ECE_PLACE_ON_SURFACE));
		if (!fire)
			return null;
		if (!s_TzActors)
			s_TzActors = new array<Object>;
		s_TzActors.Insert(fire);
		s_TzFireAt = fire.GetPosition();
		float best = -2;
		for (int i = 0; i < 12; i++)
		{
			float a = i * Math.PI2 / 12;
			vector d = Vector(Math.Sin(a), 0, Math.Cos(a));
			vector to = s_TzFireAt + d * 10.0;
			to[1] = GetGame().SurfaceY(to[0], to[2]) + 1.5;
			if (!AZ_TzView.Clear(s_TzFireAt + d * 1.5 + "0 1.5 0", to))
				continue;
			float sc = vector.Dot(d, s_TzDoorOut);
			if (sc > best)
			{
				best = sc;
				s_TzVDir = d;
			}
		}
		TzLightFire(fire);
		return fire;
	}





	void TrHoldSetup(PlayerBase p, int v)
	{
		TrFrenzySetup(p, 10);
		if (!s_TzHero)
			return;
		EntityAI held = s_TzHero.GetHumanInventory().GetEntityInHands();
		if (held)
			GetGame().ObjectDelete(held);
		s_TzHero.GetHumanInventory().CreateInHands("AZ_VampStake");
		vector r = Vector(s_TrFrO[2], 0, -s_TrFrO[0]);
		vector fl = s_TzHero.GetPosition() + s_TrFrO * 0.3 + r * 0.35;
		fl[1] = GetGame().SurfaceY(fl[0], fl[2]);
		ItemBase rf = ItemBase.Cast(GetGame().CreateObjectEx("Roadflare", fl, ECE_PLACE_ON_SURFACE));
		if (rf)
		{
			if (rf.GetCompEM())
				rf.GetCompEM().SwitchOn();
			s_TzActors.Insert(rf);
		}
		Print("[AZ_Vamp] TEST tr hold " + v + ": stake in hand " + s_TzHero.GetHumanInventory().GetEntityInHands() + ", flare " + rf);
	}

	void TrHoldShot(PlayerBase p, int v)
	{
		if (!p || !s_TzHero)
			return;
		string vl = "tz_tr_stake_" + TR_LETTERS.Substring(v, 1);
		vector o = s_TrFrO;
		vector r = Vector(o[2], 0, -o[0]);
		vector hand = s_TzHero.GetBonePositionWS(s_TzHero.GetBoneIndexByName("RightHand"));
		vector head = s_TzHero.GetBonePositionWS(s_TzHero.GetBoneIndexByName("Head"));
		vector her = p.GetPosition() + "0 1.62 0";
		float sd = 1.0;
		if (v % 2 == 1)
			sd = -1.0;
		vector cam, look, cam2, look2;
		float fov = 0.45;
		int bv = v / 2;
		if (bv == 0)
		{
			cam = hand + r * (0.9 * sd) + o * 0.3 + "0 -0.15 0";
			look = hand;
			cam2 = cam + (look - cam).Normalized() * 0.25;
			look2 = hand;
			fov = 0.4;
		}
		else if (bv == 1)
		{
			cam = hand - o * 0.35 + r * (0.3 * sd) + "0 0.1 0";
			look = her;
			cam2 = cam + o * 0.2;
			look2 = her;
			fov = 0.5;
		}
		else
		{
			cam = head + o * 1.6 + r * (0.4 * sd) - "0 0.15 0";
			look = (head + hand) * 0.5;
			cam2 = cam - o * 0.5;
			look2 = look;
			fov = 0.5;
		}
		TrSafeCam(p, vl, cam, look, cam2, look2);
		TzShot(p, vl, s_TrC1, look, s_TrC2, look2, 7, fov);
	}

	void TrStakeSetup(PlayerBase p, int v)
	{
		if (!p)
			return;
		TrEmptyHands(p);
		TzClearActors();
		TrClearSky();
		AZ_TrMoon.Day();
		TzBloodMoon(p, false);
		vector at = TrChurchSpot();
		if (at.Length() < 1)
			return;
		vector o = s_TzDoorOut;
		s_TzVDir = o;
		s_TzFireAt = at;
		s_TrStakeV = AZ_VampEvent.AIVampire(AZ_VampDress.Male(Math.RandomInt(0, 1000)), at, o * -1);
		if (s_TrStakeV)
		{
			TzDressVampire(s_TrStakeV, false);
			s_TzActors.Insert(s_TrStakeV);
			s_TrStakeV.SetHealth("", "Blood", AZ_VampState.Cfg().fullBlood);
			TzKnockOut(s_TrStakeV);
			AZ_VampReveal.BareAtWill(s_TrStakeV, 30);
		}


		vector r = Vector(o[2], 0, -o[0]);
		TzHero(at + r * 0.8, r * -1, false);
		TrHeroWinter();
		if (s_TzHero)
			s_TzHero.GetHumanInventory().CreateInHands("AZ_VampStake");
		TrAway(p, s_TzFireAt, o * -1);
		TrClearLights(s_TzFireAt, 60);
		Print("[AZ_Vamp] TEST tr stake " + v + " vampire " + s_TrStakeV + " at " + at);
	}

	void TrStakeShot(PlayerBase p, int v)
	{
		if (!p || !s_TrStakeV)
			return;
		string vl = "tz_tr_stake_" + TR_LETTERS.Substring(v, 1);
		vector chest = s_TrStakeV.GetBonePositionWS(s_TrStakeV.GetBoneIndexByName("Spine3"));
		vector head = s_TrStakeV.GetBonePositionWS(s_TrStakeV.GetBoneIndexByName("Head"));
		vector o = s_TzVDir;
		vector r = Vector(o[2], 0, -o[0]);
		float sd = 1.0;
		if (v % 2 == 1)
			sd = -1.0;
		vector cam, look;
		float fov = 0.55;
		int bv = v / 2;
		vector mid = (chest + head) * 0.5;
		vector ax = head - chest;
		ax[1] = 0;
		ax.Normalize();
		if (bv == 0)
		{
			cam = chest - r * 2.4 + ax * (0.9 * sd) + "0 0.9 0";
			look = chest + r * 0.4 + "0 0.6 0";
			fov = 0.6;
		}
		else if (bv == 1)
		{
			cam = chest + r * 1.3 + ax * (0.4 * sd) + "0 1.9 0";
			look = chest;
			fov = 0.55;
		}
		else
		{
			cam = head + ax * 1.3 + r * (0.3 * sd) + "0 0.6 0";
			look = chest;
			fov = 0.5;
		}
		TrSafeCam(p, vl, cam, look, cam + (look - cam).Normalized() * 0.15, look);
		TzShot(p, vl, s_TrC1, look, s_TrC2, look, 7, fov);

		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TrStakeDown, 2600, false, chest);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TrStakeEyesOut, 3000, false);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TrStakeAsh, 3400, false);
	}

	void TrStakeDown(vector chest)
	{
		if (s_TzHero)
		{
			EntityAI held = s_TzHero.GetHumanInventory().GetEntityInHands();
			if (held)
				GetGame().ObjectDelete(held);
		}
		s_TrStakeObj = GetGame().CreateObjectEx("AZ_VampStake", chest + "0 0.9 0", ECE_KEEPHEIGHT);
		if (!s_TrStakeObj)
			return;
		s_TrStakeObj.SetOrientation("0 -90 0");
		s_TzActors.Insert(s_TrStakeObj);
		for (int k = 1; k <= 10; k++)
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TrStakeStep, k * 30, false, chest, k);
	}

	void TrStakeStep(vector chest, int k)
	{
		if (!s_TrStakeObj)
			return;
		float e = k / 10.0;
		e = e * e;
		s_TrStakeObj.SetPosition(chest + Vector(0, 0.9 - 0.75 * e, 0));
	}

	void TrStakeEyesOut()
	{
		if (!s_TrStakeV)
			return;
		s_TrStakeV.m_AZVampBareUntil = 0;
		s_TrStakeV.m_AZVampFrenzyUntil = 0;
		AZ_VampReveal.Update(s_TrStakeV);
	}

	void TrStakeAsh()
	{
		if (!s_TrStakeV)
			return;
		PlayerBase body = s_TrStakeV;
		s_TrStakeV = null;
		int i = s_TzActors.Find(body);
		if (i >= 0)
			s_TzActors.Remove(i);
		AZ_VampAsh.Crumble(body, false, null);
		if (s_TrStakeObj)
		{
			GetGame().ObjectDelete(s_TrStakeObj);
			s_TrStakeObj = null;
		}
		Print("[AZ_Vamp] TEST tr stake: ash");
	}





	static Fireplace s_TrFinFire;

	void TrFinSetup(PlayerBase p, int v)
	{
		if (!p)
			return;
		TrEmptyHands(p);
		TzClearActors();
		TrClearSky();
		AZ_TrMoon.Bright();
		TzBloodMoon(p, true);
		s_TrFinFire = TrChurchFire();
		if (s_TrFinFire)
		{
			TzLightFire(s_TrFinFire);
			if (v >= 6)
				s_TrFinFire.StopFire();
		}
		vector o = s_TzVDir;
		vector r = Vector(o[2], 0, -o[0]);
		vector f = s_TzFireAt;
		vector at[4] = {f - o * 1.4 + r * 0.6, f - o * 1.2 - r * 0.9, f + r * 1.5 - o * 0.2, f - r * 1.6 + o * 0.1};
		string cls[4] = {"SurvivorM_Boris", "SurvivorF_Irena", "SurvivorM_Denis", "SurvivorF_Gabi"};
		for (int i = 0; i < 4; i++)
		{
			vector a = at[i];
			a[1] = GetGame().SurfaceY(a[0], a[2]);
			PlayerBase sv = LdSurvivor(cls[i], a, f, i + 1);
			if (sv)
				sv.SetPosition(a);
		}
		TrAway(p, f, o * -1);
		TrClearLights(f, 60);
		Print("[AZ_Vamp] TEST tr fin " + v + " fire " + s_TrFinFire + " dir " + o);
	}




	void TrFinShot(PlayerBase p, int v)
	{
		if (!p)
			return;
		string vl = "tz_tr_fin_" + TR_FRENZY_LETTERS.Substring(v, 1);
		vector o = s_TzVDir;
		vector r = Vector(o[2], 0, -o[0]);
		vector f = s_TzFireAt;
		vector cam, look;
		float fov = 0.7;
		float sd = 1.0;
		if (v % 2 == 1)
			sd = -1.0;
		vector ev = f + o * TR_EVA_OUT + r * 0.8;
		ev[1] = GetGame().SurfaceY(ev[0], ev[2]);
		if (v < 6)
		{
			float dd[3] = {3.6, 4.6, 5.4};
			float hh[3] = {0.6, 0.9, 1.1};
			int k1 = v / 2;
			cam = f + o * dd[k1] + r * (1.4 * sd);
			cam[1] = GetGame().SurfaceY(cam[0], cam[2]) + hh[k1];
			look = f + "0 0.7 0";
			fov = 0.75;
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TrFinFireOut, 4500, false);
		}
		else if (v < 12)
		{
			PlayerBase who = TrFinSurvivor((v - 6) % 4);
			vector hd = f + "0 1.6 0";
			if (who)
				hd = who.GetBonePositionWS(who.GetBoneIndexByName("Head"));
			vector tt = ev - hd;
			tt[1] = 0;
			tt.Normalize();
			cam = hd + tt * 1.0 + Vector(tt[2], 0, -tt[0]) * (0.35 * sd);
			look = hd;
			fov = 0.45;
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TrFinLook, 3000, false);
		}
		else if (v < 15)
		{
			cam = f - o * (2.0 + (v - 12) * 0.8) + r * (0.8 * sd);
			cam[1] = GetGame().SurfaceY(cam[0], cam[2]) + 1.6;
			look = ev + "0 1.2 0";
			fov = 0.65;
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TrFinEva, 3200, false, ev, f);
		}
		else
		{
			vector eh = ev + "0 1.62 0";
			vector toF = f - ev;
			toF[1] = 0;
			toF.Normalize();
			cam = eh + toF * (1.1 + (v - 15) * 0.3) + Vector(toF[2], 0, -toF[0]) * (0.25 * sd);
			look = eh;
			fov = 0.4;
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TrFinEva, 3200, false, ev, f);
		}
		if (v < 15)
			cam = LdClearCam(cam, look);
		TrSafeCam(p, vl, cam, look, cam + (look - cam).Normalized() * 0.3, look);
		TzShot(p, vl, s_TrC1, look, s_TrC2, look, 7, fov);
	}

	PlayerBase TrFinSurvivor(int k)
	{
		int n = 0;
		foreach (Object ao : s_TzActors)
		{
			PlayerBase pb = PlayerBase.Cast(ao);
			if (!pb)
				continue;
			if (n == k)
				return pb;
			n++;
		}
		return null;
	}

	void TrFinFireOut()
	{
		if (s_TrFinFire && s_TrFinFire.IsBurning())
			s_TrFinFire.StopFire();
		Print("[AZ_Vamp] TEST tr fin: the fire dies");
	}


	void TrFinLook()
	{
		for (int k = 1; k <= 12; k++)
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TrFinLookStep, k * 100, false, k);
	}

	void TrFinLookStep(int k)
	{
		vector tree = s_TzFireAt + s_TzVDir * TR_EVA_OUT;
		foreach (Object ao : s_TzActors)
		{
			PlayerBase pb = PlayerBase.Cast(ao);
			if (!pb)
				continue;
			vector ff = s_TzFireAt - pb.GetPosition();
			ff[1] = 0;
			ff.Normalize();
			vector tt = tree - pb.GetPosition();
			tt[1] = 0;
			tt.Normalize();
			float e = k / 12.0;
			e = e * e * (3 - 2 * e);
			vector d = ff * (1 - e) + tt * e;
			d.Normalize();
			pb.SetDirection(d);
		}
	}

	void TrFinEva(vector ev, vector f)
	{
		PlayerBase e = TzEva(ev, f);
		if (e)
		{
			GetGame().RPCSingleParam(e, AZ_VampShadow.RPC_SHADOW, new Param1<vector>(ev), true);
			e.SetHealth("", "Blood", AZ_VampState.Cfg().fullBlood);
			AZ_VampReveal.BareAtWill(e, 6);
		}
	}






	static ItemBase s_TrFlameItem;





	void TrMoonDate()
	{
		GetGame().GetWorld().SetDate(2011, 11, 10, 23, 30);
		AZ_VampProgress.FullMoonDate();
	}

	void TrMoonSetup(PlayerBase p, int v)
	{
		if (!p)
			return;
		TrEmptyHands(p);
		TzClearActors();
		TrClearSky();
		TrMoonDate();
		TzBloodMoon(p, true);
		if (s_TzDoorAt.Length() < 1)
			TzChurchDoor();
		vector away = s_TzDoorAt + s_TzDoorOut * 60.0;
		HwGo(p, away, s_TzDoorAt);
		vector md = GetGame().GetWorld().GetSunOrMoonDirection();
		vector up = md * -1;
		if (up[1] < 0)
			up = md;
		Print("[AZ_Vamp] TEST tr moon " + v + ": dir " + up + " intensity " + GetGame().GetWorld().GetMoonIntensity() + " sunOrMoon " + GetGame().GetWorld().GetSunOrMoon());
	}

	void TrMoonShot(PlayerBase p, int v)
	{
		if (!p || s_TzDoorAt.Length() < 1)
			return;
		string vl = "tz_tr_moon_" + TR_LETTERS.Substring(v, 1);
		vector md = GetGame().GetWorld().GetSunOrMoonDirection();
		vector up = md * -1;
		if (up[1] < 0)
			up = md;
		up.Normalize();
		vector h = up;
		h[1] = 0;
		if (h.Length() < 0.1)
			h = s_TzDoorOut * -1;
		h.Normalize();
		vector r = Vector(h[2], 0, -h[0]);
		float dists[3] = {28.0, 40.0, 55.0};
		float sd = 1.0;
		if (v % 2 == 1)
			sd = -1.0;
		float dd = dists[v / 2];
		vector roof = s_TzDoorAt + s_TzDoorOut * -6.0 + "0 9 0";
		vector cam = roof - h * dd + r * (4.0 * sd);
		cam[1] = GetGame().SurfaceY(cam[0], cam[2]) + 1.4;
		vector look = roof;
		vector look2 = cam + up * 60.0;
		vector cam2 = cam + "0 0.4 0";
		TrSafeCam(p, vl, cam, look, cam2, look2);
		TzShot(p, vl, s_TrC1, look, s_TrC2, look2, 7, 0.75);
		Print("[AZ_Vamp] TEST tr moon " + vl + ": moon elevation " + (Math.Asin(up[1]) * Math.RAD2DEG) + " deg, camera " + dd + " m from the church");
	}

	void TrFlameSetup(PlayerBase p, int v)
	{
		if (!p || !s_TrHouse)
			return;
		for (int ai = s_TzActors.Count() - 1; ai >= 0; ai--)
		{
			Object ao = s_TzActors[ai];
			if (ao && (PlayerBase.Cast(ao) || Torch.Cast(ao) || Roadflare.Cast(ao)))
			{
				GetGame().ObjectDelete(ao);
				s_TzActors.Remove(ai);
			}
		}
		s_TzHero = null;
		s_TrFlameItem = null;
		vector at = s_TrDoor - s_TrOut * 0.9;
		if (v == 0 || v == 2 || v == 4)
		{
			TzHero(at, s_TrOut * -1, v == 0);
			TrSettle(s_TzHero, at);
			if (v == 0 && s_TzHero)
				s_TrFlameItem = ItemBase.Cast(s_TzHero.GetHumanInventory().GetEntityInHands());
			if (v == 2 && s_TzHero)
			{
				s_TrFlameItem = ItemBase.Cast(s_TzHero.GetHumanInventory().CreateInHands("Torch"));
				if (s_TrFlameItem)
					s_TrFlameItem.GetInventory().CreateAttachment("Rag");
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TrFlameIgnite, 3000, false, s_TrFlameItem);
			}
			if (v == 4 && s_TzHero)
			{
				s_TrFlameItem = ItemBase.Cast(s_TzHero.GetHumanInventory().CreateInHands("Roadflare"));
				if (s_TrFlameItem && s_TrFlameItem.GetCompEM())
					s_TrFlameItem.GetCompEM().SwitchOn();
			}
			TrAway(p, s_TrDoor, s_TrOut);
		}
		else if (v == 1)
		{
			LdTorchAt(at);
			if (s_TzActors.Count() > 0)
				s_TrFlameItem = ItemBase.Cast(s_TzActors[s_TzActors.Count() - 1]);
			TrAway(p, s_TrDoor, s_TrOut);
		}
		else
		{
			TzPut(p, TrSpot(at), s_TrFireAt);
			EntityAI old = p.GetHumanInventory().GetEntityInHands();
			if (old)
				GetGame().ObjectDelete(old);
			s_TrFlameItem = ItemBase.Cast(p.GetHumanInventory().CreateInHands("Torch"));
			if (s_TrFlameItem)
			{
				s_TrFlameItem.GetInventory().CreateAttachment("Rag");
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TrFlameIgnite, 1000, false, s_TrFlameItem);
			}
		}
		for (int t = 1; t <= 10; t += 4)
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TrFlameLog, t * 1000, false, v, t);
	}

	void TrFlameIgnite(ItemBase tr)
	{
		if (!tr)
			return;
		if (tr.GetCompEM())
		{
			tr.GetCompEM().SetEnergy(tr.GetCompEM().GetEnergyMax());
			tr.GetCompEM().SwitchOn();
		}
		if (!tr.IsIgnited())
			tr.OnIgnitedThis(null);
	}

	void TrFlameLog(int v, int t)
	{
		ItemBase it = s_TrFlameItem;
		if (!it)
		{
			Print("[AZ_Vamp] TEST tr flame " + v + " +" + t + "s: no item");
			return;
		}
		string em = "no EM";
		if (it.GetCompEM())
			em = "on " + it.GetCompEM().IsSwitchedOn() + " working " + it.GetCompEM().IsWorking() + " energy " + it.GetCompEM().GetEnergy() + "/" + it.GetCompEM().GetEnergyMax();
		Print("[AZ_Vamp] TEST tr flame " + v + " +" + t + "s: " + it.GetType() + " ignited " + it.IsIgnited() + " " + em + " wet " + it.GetWet() + " qty " + it.GetQuantity() + " parent " + it.GetHierarchyParent());
	}

	void TrFlameShot(PlayerBase p, int v)
	{
		if (!p || !s_TrHouse)
			return;
		string vl = "tz_tr_flame_" + TR_LETTERS.Substring(v, 1);
		vector at = s_TrDoor - s_TrOut * 0.9;
		if (s_TrFlameItem)
			at = s_TrFlameItem.GetPosition();
		vector r = Vector(s_TrOut[2], 0, -s_TrOut[0]);
		vector look = at + "0 0.25 0";
		PlayerBase holder = s_TzHero;
		if (v == 3)
			holder = p;
		if (v != 1 && holder)
			look = holder.GetBonePositionWS(holder.GetBoneIndexByName("Head")) + "0 0.2 0";
		vector cam = look - s_TrOut * 1.6 + r * 0.7 + "0 0.2 0";
		cam = LdClearCam(cam, look);
		TzShot(p, vl, cam, look, cam + (look - cam).Normalized() * 0.3, look, 6, 0.5);
	}


	int TrSetOrb(PlayerBase p)
	{
		TzClearActors();
		s_TrHouse = false;
		TzBloodMoon(p, true);
		AZ_VampCryptA1.s_TestOpen = true;
		AZ_VampCryptA1.Refresh();
		HwGo(p, AZ_VampCryptA1.ORB + Vector(19, 0, -8), AZ_VampCryptA1.ORB);
		return 14000;
	}

	void TrCamOrb(int v, out vector cam, out vector look, out float fov)
	{
		vector o = AZ_VampCryptA1.ORB;
		vector off[3] = {Vector(12, 0, -5), Vector(8, 0, -3.5), Vector(16, 0, -7)};
		float h[3] = {1.7, 1.0, 3.0};
		float f[3] = {0.7, 0.8, 0.6};
		cam = o + off[v];
		cam[1] = GetGame().SurfaceY(cam[0], cam[2]) + h[v];
		look = o - "0 2.5 0";
		fov = f[v];
	}


	void TrShotV(PlayerBase p, int step, int v)
	{
		if (!p)
			return;
		string vl = TrBase(step) + "_" + TR_LETTERS.Substring(v, 1);
		int bv = v % 3;
		int sw = v / 3;
		float swing[5] = {0, 12, -12, 24, -24};
		vector cam, look;
		float fov = 0.8;
		if (step == 495)
			TrCamOrb(bv, cam, look, fov);
		else if (step == 497 && s_TrHouse)
			TrCamHousehold(bv, cam, look, fov);
		else if (step == 481 && s_TrHouse)
			TrCamFire(bv, cam, look, fov);
		else if (step == 483 && s_TrHouse)
		{
			if (!s_TrCougher)
			{
				Print("[AZ_Vamp] TEST tr " + vl + ": no cougher, skipped");
				return;
			}
			TrCamEyes(s_TrCougher, bv, cam, look, fov);


			vector ff = s_TrFireAt - s_TrCougher.GetPosition();
			ff[1] = 0;
			ff.Normalize();
			s_TrCougher.SetDirection(ff);
			for (int tk = 1; tk <= 14; tk++)
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TrTurn, 3000 + tk * 100, false, s_TrCougher, cam, tk);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TrEyesOn, 3600, false, s_TrCougher);
			s_TrCougher.m_AZVampFrenzyUntil = 0;
			s_TrCougher.m_AZVampBareUntil = 0;
			AZ_VampReveal.Update(s_TrCougher);
		}
		else if (!s_TrHouse)
		{
			Print("[AZ_Vamp] TEST tr " + vl + ": no house, skipped");
			return;
		}
		else if (step == 485)
			TrCamWin(bv, cam, look, fov);
		else if (step == 487)
			TrCamTorch(bv, cam, look, fov);
		else if (step == 489)
			TrCamStalk(bv, cam, look, fov);
		else if (step == 491)
			TrCamBite(bv, cam, look, fov);
		else
			TrCamRepel(p, bv, cam, look, fov);
		if (sw > 0)
		{
			vector rel = cam - look;
			float rad = swing[sw] * Math.DEG2RAD;
			float cx = rel[0] * Math.Cos(rad) - rel[2] * Math.Sin(rad);
			float cz = rel[0] * Math.Sin(rad) + rel[2] * Math.Cos(rad);
			cam = look + Vector(cx, rel[1], cz);
		}
		if (step != 483 && step != 491 && step != 493)
			cam = LdClearCam(cam, look);
		vector cam2 = cam + (look - cam).Normalized() * 0.3;
		TzShot(p, vl, cam, look, cam2, look, 6, fov);
	}

	void TrCamFire(int v, out vector cam, out vector look, out float fov)
	{

		vector o = s_TrOut;
		vector r = Vector(o[2], 0, -o[0]);
		float d[3] = {2.6, 2.0, 1.2};
		float s[3] = {0.2, -0.3, 1.9};
		float h[3] = {1.55, 0.9, 1.5};
		float f[3] = {0.75, 0.7, 0.65};
		cam = s_TrFireAt + o * d[v] + r * s[v];
		cam[1] = s_TrFireAt[1] + h[v];
		look = s_TrFireAt + "0 0.8 0";
		fov = f[v];
	}

	void TrCamEyes(PlayerBase p, int v, out vector cam, out vector look, out float fov)
	{
		vector hd = p.GetBonePositionWS(p.GetBoneIndexByName("Head"));
		vector tf = s_TrFireAt - p.GetPosition();
		tf[1] = 0;
		tf.Normalize();
		vector to = Vector(tf[2], 0, -tf[0]) * 0.85 + tf * 0.5;
		to.Normalize();
		vector r = Vector(to[2], 0, -to[0]);
		float d[3] = {0.7, 1.0, 1.5};
		float s[3] = {0.3, 0.5, 0.4};
		float f[3] = {0.45, 0.6, 0.55};
		cam = hd + to * d[v] + r * s[v] - "0 0.1 0";
		look = hd;
		fov = f[v];
	}

	void TrCamWin(int v, out vector cam, out vector look, out float fov)
	{
		vector r = Vector(s_TrOut[2], 0, -s_TrOut[0]);
		float b[3] = {1.0, 0.4, 1.4};
		float s[3] = {0.0, 0.7, -0.6};
		float h[3] = {1.5, 1.0, 1.7};
		float f[3] = {0.8, 0.7, 0.9};
		cam = s_TrFireAt - s_TrOut * b[v] + r * s[v];
		cam[1] = s_TrDoor[1] + h[v];
		look = s_TrDoor + s_TrOut * 30.0;
		look[1] = s_TrDoor[1] + 1.2;
		fov = f[v];
	}

	void TrCamTorch(int v, out vector cam, out vector look, out float fov)
	{
		vector r = Vector(s_TrOut[2], 0, -s_TrOut[0]);
		vector hero = s_TrDoor - s_TrOut * 0.9;
		if (s_TzHero)
			hero = s_TzHero.GetPosition();

		if (v == 0)
		{
			cam = hero - s_TrOut * 2.3 + r * 0.8;
			cam[1] = hero[1] + 1.55;
			look = hero + "0 1.5 0";
			fov = 0.6;
		}
		else if (v == 1)
		{
			cam = hero - s_TrOut * 1.3 - r * 0.4;
			cam[1] = hero[1] + 1.65;
			look = hero + "0 1.65 0";
			fov = 0.45;
		}
		else
		{
			cam = hero - s_TrOut * 2.0 - r * 1.1;
			cam[1] = hero[1] + 1.0;
			look = hero + "0 1.4 0";
			fov = 0.75;
		}
	}

	void TrCamStalk(int v, out vector cam, out vector look, out float fov)
	{
		vector r = Vector(s_TrOut[2], 0, -s_TrOut[0]);

		float b[3] = {3.2, 2.9, 3.4};
		float s[3] = {0.7, -0.8, 0.2};
		float h[3] = {1.55, 1.2, 1.75};
		float lh[3] = {1.45, 1.5, 1.4};
		float f[3] = {0.65, 0.55, 0.75};
		cam = s_TrDoor - s_TrOut * b[v] + r * s[v];
		cam[1] = s_TrDoor[1] + h[v];
		look = s_TrDoor + s_TrOut * 0.8;
		look[1] = s_TrDoor[1] + lh[v];
		fov = f[v];
	}

	void TrCamBite(int v, out vector cam, out vector look, out float fov)
	{
		vector r = Vector(s_TrOut[2], 0, -s_TrOut[0]);
		vector hat = s_TrFireAt + s_TrOut * 1.2;
		if (s_TzHero)
			hat = s_TzHero.GetPosition();
		vector eva = hat + s_TrOut * 0.55 + "0 1.5 0";
		float b[3] = {-0.5, -0.3, -0.2};
		float s[3] = {1.0, 1.5, 0.7};
		float f[3] = {0.55, 0.7, 0.36};
		cam = hat + s_TrOut * b[v] + r * s[v];
		cam[1] = hat[1] + 1.6;
		look = eva;
		fov = f[v];
	}

	void TrCamRepel(PlayerBase p, int v, out vector cam, out vector look, out float fov)
	{
		vector r = Vector(s_TrOut[2], 0, -s_TrOut[0]);
		vector hero = s_TrDoor - s_TrOut * 1.6;
		vector ev = p.GetPosition();
		if (v == 0)
		{
			cam = hero - s_TrOut * 1.0 + r * 0.6;
			cam[1] = s_TrDoor[1] + 1.6;
			look = ev + "0 1.5 0";
			fov = 0.65;
		}
		else if (v == 1)
		{
			vector mid1 = (hero + ev) * 0.5;
			cam = mid1 - r * 2.2;
			cam[1] = s_TrDoor[1] + 1.4;
			look = mid1 + "0 1.45 0";
			fov = 0.7;
		}
		else
		{
			vector mid = (hero + ev) * 0.5;
			cam = mid + r * 2.4;
			cam[1] = s_TrDoor[1] + 1.5;
			look = mid + "0 1.45 0";
			fov = 0.7;
		}
	}
}

modded class MissionGameplay
{

	static const string TR_NIGHT_CFG = AZ_VampNightLighting.VANILLA_DEFAULT;

	override void VampHwShot(string label, vector cam, vector target)
	{
		bool tr = label.IndexOf("tz_tr_") == 0 && label.IndexOf("#") > 0;
		if (tr)
		{
			if (label.IndexOf("tz_tr_moon") == 0)
			{
				GetGame().GetWorld().SetDate(2011, 11, 10, 23, 30);
				AZ_VampProgress.FullMoonDate();
				GetGame().GetWeather().GetFog().Set(0, 0, 3600);
				GetGame().GetWeather().GetOvercast().Set(0, 0, 3600);
			}
			else if (label.IndexOf("tz_tr_frenzy") == 0 || label.IndexOf("tz_tr_fin") == 0)
				AZ_TrMoon.Bright();
			else if (label.IndexOf("tz_tr_stake") == 0)
				AZ_TrMoon.Bright();
			else
				AZ_TrMoon.Dark();
			if (label.IndexOf("tz_tr_fin") == 0 || label.IndexOf("tz_tr_frenzy") == 0 || label.IndexOf("tz_tr_stake") == 0 || label.IndexOf("tz_tr_moon") == 0)
				GetGame().GetWorld().LoadNewLightingCfg(TR_NIGHT_CFG);
			m_LdOn = true;
			if (m_Chat)
				m_Chat.Clear();
		}
		super.VampHwShot(label, cam, target);
		if (!tr)
			return;
		m_TzNoSight = true;

		if (label.IndexOf("tz_tr_repel") == 0)
			Print("[AZ_Vamp] TEST tr repel: no hiss (Oliver 10-22: the hiss mouth reads as a square)");
	}
}
