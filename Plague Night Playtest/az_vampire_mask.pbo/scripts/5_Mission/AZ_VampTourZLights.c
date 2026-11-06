






modded class MissionServer
{
	static const int HW_LIGHTS_STEPS = 50;

	override string HwStepLabel(int step)
	{
		if (step == 340)
			return "lights_town";
		if (step == 341)
			return "lights_mist";
		if (step == 342)
			return "lights_ab";
		if (step == 343)
			return "lights_bis|lb_house|lb_end";
		return super.HwStepLabel(step);
	}

	override int HwNextStep(int step)
	{
		if (step == 2)
			return 340;
		if (step == 340)
			return 341;
		if (step == 341)
			return 342;
		if (step == 342)
			return 343;
		if (step == 343)
			return super.HwNextStep(2);
		return super.HwNextStep(step);
	}

	override void HwStep(PlayerBase p, int step)
	{
		if (step != 340 && step != 341 && step != 342 && step != 343)
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
		if (step == 341)
			HwLightsMist(p);
		else if (step == 342)
			HwLightsAB(p);
		else if (step == 343)
			HwLightsBis(p);
		else
			HwLightsTown(p);
	}





	static const vector LM_SPOT = "9075.39 0 10074.22";

	void HwLightsMist(PlayerBase p)
	{
		if (!AZ_VampDecor.s_Lanterns)
		{
			Print("[AZ_Vamp] TEST lights_mist: no lanterns on the server");
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 100, false, p, HwNextStep(341));
			return;
		}
		vector spot = LM_SPOT;
		spot[1] = GetGame().SurfaceY(spot[0], spot[2]);
		vector sum = "0 0 0";
		int near50 = 0;
		foreach (Object b : AZ_VampDecor.s_Lanterns)
		{
			if (b && vector.DistanceSq(spot, b.GetPosition()) < 3600)
			{
				sum = sum + b.GetPosition();
				near50++;
			}
		}
		vector fire = spot + "10 0 0";
		if (near50 > 0)
			fire = sum * (1.0 / near50);
		int bestN = near50;
		int zonesNear = 0;
		if (AZ_VampLandmarks.s_MistZones)
		{
			foreach (Object mz : AZ_VampLandmarks.s_MistZones)
			{
				if (mz && vector.Distance(spot, mz.GetPosition()) < 120)
					zonesNear++;
			}
		}
		int torches = 0;
		array<Object> tobjs = new array<Object>;
		array<CargoBase> tprox = new array<CargoBase>;
		GetGame().GetObjectsAtPosition(spot, 60, tobjs, tprox);
		foreach (Object to : tobjs)
		{
			if (to && to.IsInherited(AZ_VampTorch))
				torches++;
		}
		Print("[AZ_Vamp] TEST lights_mist Nemsk: " + near50 + " lanterns and " + torches + " torches within 60 m, " + zonesNear + " mist zones within 120 m, " + AZ_VampTorch.s_Planted + " torches on the map");
		int zones = 0;
		if (AZ_VampLandmarks.s_MistZones)
			zones = AZ_VampLandmarks.s_MistZones.Count();
		Print("[AZ_Vamp] TEST lights_mist looking at " + fire + ": " + bestN + " near, " + AZ_VampDecor.s_Lanterns.Count() + " lanterns and " + zones + " mist zones on the map");
		vector at = spot;
		HwGo(p, at, fire);
		vector cam = at;
		cam[1] = GetGame().SurfaceY(at[0], at[2]) + 1.7;
		vector tgt = fire + "0 1 0";
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 8000, false, p, "lm_all", cam, tgt);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwLightsMistPumpkins, 23000, false);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 26000, false, p, "lm_no_pumpkins", cam, tgt);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwLightsMistZones, 41000, false);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 44000, false, p, "lm_no_pumpkins_no_mist", cam, tgt);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 59000, false, p, "lm_end", cam, tgt);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 65000, false, p, HwNextStep(341));
	}

	void HwLightsMistPumpkins()
	{
		int n = 0;
		if (AZ_VampDecor.s_Lanterns)
		{
			foreach (Object o : AZ_VampDecor.s_Lanterns)
			{
				if (o)
				{
					GetGame().ObjectDelete(o);
					n++;
				}
			}
		}
		Print("[AZ_Vamp] TEST lights_mist: removed " + n + " pumpkin lanterns");
	}

	void HwLightsMistZones()
	{
		int n = 0;
		if (AZ_VampLandmarks.s_MistZones)
		{
			foreach (Object z : AZ_VampLandmarks.s_MistZones)
			{
				if (z)
				{
					GetGame().ObjectDelete(z);
					n++;
				}
			}
		}
		Print("[AZ_Vamp] TEST lights_mist: removed " + n + " mist zones");
	}





	void HwLightsAB(PlayerBase p)
	{
		vector best = HwLightsDense();
		if (best == vector.Zero)
		{
			Print("[AZ_Vamp] TEST lights_ab: no lanterns on the server");
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 100, false, p, HwNextStep(342));
			return;
		}


		vector start = best + Vector(0, 0, -3);
		vector camS = best + Vector(0, 6, -8);
		vector tgtS = best + Vector(0, 0, 10);
		vector camH = best + Vector(10, 6, 0);
		vector tgtH = best + Vector(-10, 0, 0);
		Print("[AZ_Vamp] TEST lights_ab street " + best);
		vector hc = start;
		HwGo(p, start, start + Vector(10, 0, 0));
		int t = 8000;
		string ph[2] = {"a", "b"};
		for (int i = 0; i < 2; i++)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwLightsABGo, t - 500, false, p, start);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, t, false, p, "lf_street_" + ph[i], camS, tgtS);
			t += 14000;
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwLightsABGo, t - 500, false, p, hc);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, t, false, p, "lf_house_" + ph[i], camH, tgtH);
			t += 14000;
		}
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, t, false, p, "lf_end", camS, tgtS);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, t + 2000, false, p, HwNextStep(342));
	}






	void HwLightsBis(PlayerBase p)
	{
		AZ_VampConfig c = AZ_VampState.Cfg();
		Print("[AZ_Vamp] TEST lights_bis variant: halloween " + c.halloween + " lanterns " + c.decorLanterns + " candles " + c.decorCandles + " props " + c.decorProps + " torchDoorShare " + c.torchDoorShare + " scene " + c.bisectScene);
		Building house;
		vector door;
		vector outv;
		s_TrPinOn = true;
		if (c.bisectScene >= 1 && TrFindHouse())
		{
			house = Building.Cast(s_TrB);
			door = s_TrDoor;
			outv = s_TrOut;
		}
		else
		{
			array<Object> objs = new array<Object>;
			array<CargoBase> prox = new array<CargoBase>;
			GetGame().GetObjectsAtPosition(TR_PIN, 6, objs, prox);
			float bd = 1000000;
			foreach (Object o : objs)
			{
				Building bb = Building.Cast(o);
				if (bb && bb.GetDoorCount() > 0 && TzDistXZ(bb.GetPosition(), TR_PIN) < bd)
				{
					bd = TzDistXZ(bb.GetPosition(), TR_PIN);
					house = bb;
				}
			}
			if (house)
			{
				vector hc = house.GetPosition();
				float far = 0;
				for (int d = 0; d < house.GetDoorCount(); d++)
				{
					vector dp = house.GetDoorSoundPos(d);
					vector f0 = dp - hc;
					f0[1] = 0;
					if (f0.Length() > far && dp[1] - GetGame().SurfaceY(dp[0], dp[2]) < 2.5)
					{
						far = f0.Length();
						door = dp - "0 1.0 0";
						outv = f0;
					}
				}
				outv.Normalize();
				house.OpenDoor(0);
			}
		}
		if (!house)
		{
			Print("[AZ_Vamp] TEST lights_bis: no house at " + TR_PIN);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 100, false, p, HwNextStep(343));
			return;
		}
		vector cam = door + outv * 3.0 + "0 1.6 0";
		vector tgt = door - outv * 2.0 + "0 0.9 0";
		Print("[AZ_Vamp] TEST lights_bis house " + house.GetType() + " door " + door + " cam " + cam);
		HwGo(p, door + outv * 3.0, door);
		if (c.bisectScene >= 2)
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwBisVamps, 4000, false, door - outv * 2.2, outv);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 10000, false, p, "lb_house", cam, tgt);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 33000, false, p, "lb_end", cam, tgt);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 36000, false, p, HwNextStep(343));
	}


	void HwBisVamps(vector at, vector outv)
	{
		string kinds[2] = {"SurvivorM_Mirek", "SurvivorF_Eva"};
		for (int i = 0; i < 2; i++)
		{
			vector pos = at + Vector(outv[2], 0, -outv[0]) * (0.8 - 1.6 * i);
			pos = AZ_VampSpawn.Floor(pos, at[1]);
			PlayerBase v = PlayerBase.Cast(GetGame().CreateObject(kinds[i], pos, false, true));
			if (!v)
				continue;
			v.SetDirection(outv);
			AZ_VampEvent.MakeVampire(v);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwBisMask, 1500, false, v);
		}
	}

	void HwBisMask(PlayerBase v)
	{
		if (!v)
			return;
		EntityAI m = v.FindAttachmentBySlotName("Mask");
		if (m)
			GetGame().ObjectDelete(m);
		v.GetInventory().CreateAttachment("AZ_VampireMask_Bared");
	}

	void HwLightsABGo(PlayerBase p, vector at)
	{
		if (p)
			HwGo(p, at, at + "10 0 0");
	}

	vector HwLightsDense()
	{
		int bestN = -1;
		vector best = "0 0 0";
		if (AZ_VampDecor.s_Lanterns)
		{
			for (int i = 0; i < AZ_VampDecor.s_Lanterns.Count(); i += 12)
			{
				Object a = AZ_VampDecor.s_Lanterns[i];
				if (!a)
					continue;
				int n = 0;
				vector ap = a.GetPosition();
				foreach (Object b : AZ_VampDecor.s_Lanterns)
				{
					if (b && vector.DistanceSq(ap, b.GetPosition()) < 900)
						n++;
				}
				if (n > bestN)
				{
					bestN = n;
					best = ap;
				}
			}
		}
		return best;
	}

	void HwLightsTown(PlayerBase p)
	{
		int bestN = -1;
		vector best = "0 0 0";
		if (AZ_VampDecor.s_Lanterns)
		{
			for (int i = 0; i < AZ_VampDecor.s_Lanterns.Count(); i += 12)
			{
				Object a = AZ_VampDecor.s_Lanterns[i];
				if (!a)
					continue;
				int n = 0;
				vector ap = a.GetPosition();
				foreach (Object b : AZ_VampDecor.s_Lanterns)
				{
					if (b && vector.DistanceSq(ap, b.GetPosition()) < 900)
						n++;
				}
				if (n > bestN)
				{
					bestN = n;
					best = ap;
				}
			}
		}
		if (bestN < 0)
		{
			Print("[AZ_Vamp] TEST lights_town: no lanterns on the server");
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 100, false, p, HwNextStep(340));
			return;
		}
		Print("[AZ_Vamp] TEST lights_town at " + best + ", " + bestN + " lanterns within 30 m of it");
		vector start = best + Vector(0, 0, -3);
		HwGo(p, start, start + Vector(10, 0, 0));
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwLightsWalk, 6000, false, p, start, 0);
	}

	void HwLightsWalk(PlayerBase p, vector from, int i)
	{
		if (!p)
			return;
		vector at = from + Vector(0.6 * i, 0, 0);
		HwGo(p, at, at + "20 0 0");
		if (i < HW_LIGHTS_STEPS)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwLightsWalk, 400, false, p, from, i + 1);
			return;
		}
		HwShot(p, "lights_town", at + "0 1.7 0", at + "20 1.5 0");
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 10000, false, p, HwNextStep(340));
	}
}


modded class MissionGameplay
{
	override void VampHwShot(string label, vector cam, vector target)
	{
		if (label.IndexOf("lf_") == 0)
		{
			if (label.IndexOf("_a") > 0)
				AZ_VampLightBudget.s_Force = 1;
			else
				AZ_VampLightBudget.s_Force = 2;
			Print("[AZ_Vamp] TEST lights_ab " + label + " force " + AZ_VampLightBudget.s_Force);
		}
		super.VampHwShot(label, cam, target);
	}
}
