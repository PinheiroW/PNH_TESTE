

















modded class MissionServer
{
	static PlayerBase s_HwStanceAI;

	override string HwStepLabel(int step)
	{
		if (step >= AZ_VampStanceTest.FIRST && step <= AZ_VampStanceTest.LAST)
			return "hw_stance";
		return super.HwStepLabel(step);
	}

	override int HwNextStep(int step)
	{
		if (step == 520)
			return AZ_VampStanceTest.FIRST;
		if (step >= AZ_VampStanceTest.FIRST && step < AZ_VampStanceTest.LAST)
			return step + 1;
		if (step == AZ_VampStanceTest.LAST)
			return super.HwNextStep(520);
		return super.HwNextStep(step);
	}

	override void HwStep(PlayerBase p, int step)
	{
		if (step < AZ_VampStanceTest.FIRST || step > AZ_VampStanceTest.LAST)
		{
			super.HwStep(p, step);
			return;
		}
		if (!p)
			return;
		int nextStep = HwNextStep(step);
		if (!HwWanted(step))
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 100, false, p, nextStep);
			return;
		}
		int next = 4000;
		PlayerBase d = AZ_VampStanceTest.s_Dummy;
		vector at = Vector(7056, 0, 11150);
		switch (step)
		{
			case 530:
			{
				HwGo(p, at, at + "0 0 5");
				EntityAI inHands = p.GetHumanInventory().GetEntityInHands();
				if (inHands)
					GetGame().ObjectDelete(inHands);
				AZ_VampState.s_Data.eventActive = true;
				AZ_VampFeed.s_SharedTest = true;
				if (!p.m_AZVampire)
					AZ_VampEvent.MakeVampire(p);
				AZ_VampState.s_ProbeDay = true;
				GetGame().GetWorld().SetDate(2011, 11, 5, 12, 0);
				HwStanceDummy(p.GetPosition() + "0 0 1.0", "0 0 -1");
				Print("[AZ_Vamp] TEST stance setup: vampire " + p.m_AZVampire + " " + p.GetType() + " dummy " + AZ_VampStanceTest.s_Dummy);
				next = 6000;
				break;
			}
			case 531:
				HwStancePress(p, 1);
				next = 3700;
				break;
			case 532:
				HwStanceShot(p, "hw_stance_stand");
				break;
			case 533:
				HwStanceEnd(p);
				next = 4000;
				break;
			case 534:
				HwStancePress(p, 2);
				next = 6000;
				break;
			case 535:
				HwStanceShot(p, "hw_stance_prone");
				break;
			case 536:
				HwStanceEnd(p);
				if (d)
				{
					d.SetHealth("", "Blood", AZ_VampState.Cfg().bloodFloor);
					d.m_AZStTestStance = DayZPlayerConstants.STANCEIDX_PRONE;
				}
				next = 8000;
				break;
			case 537:
				HwStancePress(p, 3);
				next = 3700;
				break;
			case 538:
				HwStanceShot(p, "hw_stance_drain");
				break;
			case 539:
				HwStanceEnd(p);
				HwGo(p, at, at + "0 0 5");
				HwStanceDummy(at + "0 0 1.0", "0 0 1");
				next = 7000;
				break;
			case 540:
				HwStancePress(p, 4);
				next = 3700;
				break;
			case 541:
				HwStanceShot(p, "hw_reach_rear");
				break;
			case 542:
				HwStanceEnd(p);
				if (d)
				{
					vector aiAt = d.GetPosition() + "0 0 0.62";
					aiAt[1] = GetGame().SurfaceY(aiAt[0], aiAt[2]);
					s_HwStanceAI = PlayerBase.Cast(GetGame().CreateObject("SurvivorM_Boris", aiAt, false, true));
					if (s_HwStanceAI)
					{
						s_HwStanceAI.SetDirection("0 0 -1");
						AZ_VampState.SetVampire(s_HwStanceAI, true);
					}
				}
				next = 6000;
				break;
			case 543:
				if (s_HwStanceAI && d)
				{
					d.SetHealth("", "Blood", d.GetMaxHealth("", "Blood"));
					ActionAZVampBite.Snap(s_HwStanceAI, d);
					AZ_VampFeed.Hold(s_HwStanceAI, d);
					AZ_VampFeedAnim.DummyFeedStart(s_HwStanceAI);
					s_HwStanceAI.AZ_VampReachLater(d, "front ai");
				}
				next = 3000;
				break;
			case 544:
				HwStancePress(p, 4);
				next = 3700;
				break;
			case 545:
				HwStanceShot(p, "hw_reach_join");
				break;
			case 546:
				HwStanceEnd(p);
				if (s_HwStanceAI)
				{
					AZ_VampFeedAnim.DummyFeedEnd(s_HwStanceAI);
					AZ_VampFeed.Release(s_HwStanceAI, d);
					GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GetGame().ObjectDelete, 3000, false, s_HwStanceAI);
					s_HwStanceAI = null;
				}
				HwGo(p, at, at + "0 0 5");
				HwStanceDummy(at + "0.87 0 0.5", "-0.87 0 -0.5");
				next = 7000;
				break;
			case 547:
				HwStancePress(p, 1);
				next = 3700;
				break;
			case 548:
				HwStanceShot(p, "hw_reach_angle");
				break;
			case 549:
			{
				HwStanceEnd(p);
				vector sp, sdir;
				if (HwStanceSlope(at, sp, sdir))
				{
					HwGo(p, sp, sp + sdir * 5);
					HwStanceDummy(sp + sdir * 1.0, -sdir);
				}
				else
					Print("[AZ_Vamp] TEST stance slope: no spot found");
				next = 7000;
				break;
			}
			case 550:
				HwStancePress(p, 1);
				next = 3700;
				break;
			case 551:
				HwStanceShot(p, "hw_reach_slope");
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStanceEnd, 5500, false, p);
				if (d)
					GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GetGame().ObjectDelete, 8000, false, d);
				AZ_VampStanceTest.s_Dummy = null;
				AZ_VampState.s_ProbeDay = false;
				next = 9000;
				break;
		}
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, next, false, p, nextStep);
	}


	void HwStanceDummy(vector pos, vector dir)
	{
		if (AZ_VampStanceTest.s_Dummy)
			GetGame().ObjectDelete(AZ_VampStanceTest.s_Dummy);
		pos[1] = GetGame().SurfaceY(pos[0], pos[2]);
		AZ_VampStanceTest.s_Dummy = PlayerBase.Cast(GetGame().CreateObject("SurvivorF_Eva", pos, false, true));
		if (AZ_VampStanceTest.s_Dummy)
			AZ_VampStanceTest.s_Dummy.SetDirection(dir);
	}

	void HwStancePress(PlayerBase p, int mode)
	{
		p.SetHealth("", "Blood", AZ_VampState.Cfg().fullBlood - 500);
		GetGame().RPCSingleParam(p, AZ_VampStanceTest.RPC, new Param1<int>(mode), true, p.GetIdentity());
		AZ_VampStanceTest.Log("press " + mode, p);
	}


	void HwStanceShot(PlayerBase p, string lab)
	{
		AZ_VampStanceTest.Log(lab, p);
		PlayerBase d = AZ_VampStanceTest.s_Dummy;
		vector a = p.GetPosition();
		vector b = a + "0 0 1";
		if (d)
			b = d.GetPosition();
		vector mid = (a + b) * 0.5;
		HwShot(p, lab, mid + "3.0 0.9 0", mid + "0 0.6 0");


		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStanceClose, 1000, false, p, lab + "_side", false);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStanceClose, 2000, false, p, lab + "_34", true);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStanceRefill, 5500, false, d);
	}


	void HwStanceClose(PlayerBase p, string lab, bool threeQuarter)
	{
		PlayerBase d = AZ_VampStanceTest.s_Dummy;
		if (!p || !d)
			return;
		vector vh = p.GetBonePositionWS(p.GetBoneIndexByName("Head"));
		vector dh = d.GetBonePositionWS(d.GetBoneIndexByName("Head"));
		vector hm = (vh + dh) * 0.5;
		vector a = dh - vh;
		a[1] = 0;
		if (a.Length() < 0.05)
			a = p.GetDirection();
		a.Normalize();
		vector side = a * vector.Up;
		vector off = side;
		if (threeQuarter)
		{
			off = side + a;
			off.Normalize();
		}
		HwShot(p, lab, hm + off * 1.3 + "0 0.05 0", hm - "0 0.05 0");
	}

	void HwStanceEnd(PlayerBase p)
	{
		if (p)
			GetGame().RPCSingleParam(p, AZ_VampStanceTest.RPC, new Param1<int>(9), true, p.GetIdentity());
	}

	void HwStanceRefill(PlayerBase d)
	{

		if (d && d.m_AZStTestStance != DayZPlayerConstants.STANCEIDX_PRONE)
			d.SetHealth("", "Blood", d.GetMaxHealth("", "Blood"));
	}


	bool HwStanceSlope(vector from, out vector spot, out vector dir)
	{
		for (int r = 10; r <= 300; r += 10)
		{
			for (int a = 0; a < 16; a++)
			{
				float ang = a * Math.PI2 / 16;
				vector pt = from + Vector(Math.Cos(ang) * r, 0, Math.Sin(ang) * r);
				pt[1] = GetGame().SurfaceY(pt[0], pt[2]);
				if (GetGame().SurfaceIsSea(pt[0], pt[2]) || GetGame().SurfaceIsPond(pt[0], pt[2]))
					continue;
				for (int b = 0; b < 8; b++)
				{
					float bng = b * Math.PI2 / 8;
					vector dv = Vector(Math.Cos(bng), 0, Math.Sin(bng));
					vector q = pt + dv;
					float dy = GetGame().SurfaceY(q[0], q[2]) - pt[1];
					if (dy >= 0.25 && dy <= 0.6)
					{
						spot = pt;
						dir = dv;
						Print("[AZ_Vamp] TEST stance slope spot " + pt + " dy " + dy + " m");
						return true;
					}
				}
			}
		}
		return false;
	}
}
