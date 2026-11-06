







modded class MissionServer
{
	override string HwStepLabel(int step)
	{
		if (step == 520)
			return "old_map";
		return super.HwStepLabel(step);
	}

	override int HwNextStep(int step)
	{
		if (step == LT_LAST)
			return 520;
		if (step == 520)
			return super.HwNextStep(LT_LAST);
		return super.HwNextStep(step);
	}

	override void HwStep(PlayerBase p, int step)
	{
		if (step != 520)
		{
			super.HwStep(p, step);
			return;
		}
		if (!p)
			return;
		if (HwWanted(step))
		{
			HwGo(p, AZ_VampOldMap.CrackAt() + Vector(0, 0, -6), AZ_VampOldMap.CrackAt());
			EntityAI inHands = p.GetHumanInventory().GetEntityInHands();
			if (inHands)
				GetGame().ObjectDelete(inHands);
			EntityAI m = p.GetHumanInventory().CreateInHands(AZ_VampOldMaps.TYPE);
			Print("[AZ_Vamp] TEST old_map: in hands " + (m != null));
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 3000, false, p, "old_map", vector.Zero, vector.Zero);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 20000, false, p, HwNextStep(520));
			return;
		}
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 100, false, p, HwNextStep(520));
	}
}

modded class MissionGameplay
{
	protected int m_AZOldMapTries;

	override void VampHwShot(string label, vector cam, vector target)
	{
		if (label != "old_map")
		{
			super.VampHwShot(label, cam, target);
			return;
		}
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p || m_AZOldMapTries >= 1)
			return;
		m_AZOldMapTries++;
		if (m_VampCam)
			m_VampCam.SetActive(false);
		GetGame().GetUIManager().CloseAll();

		if (m_AZFaceLight)
			m_AZFaceLight.Destroy();
		vector head = p.GetBonePositionWS(p.GetBoneIndexByName("Head"));
		m_AZFaceLight = AZ_VampFaceLight.Cast(AZ_VampLightGate.Light(AZ_VampFaceLight, head + p.GetDirection() * 0.25 + "0 0.15 0"));
		ItemBase heldMap = ItemBase.Cast(p.GetItemInHands());
		Print("[AZ_Vamp] TEST old_map: client hands " + (heldMap != null));
		if (!heldMap)
			return;
		ActionManagerClient amc = ActionManagerClient.Cast(p.GetActionManager());
		if (amc)
		{
			ActionTarget t = new ActionTarget(null, null, -1, vector.Zero, -1);
			ActionBase unfold = amc.GetAction(ActionUnfoldMap);
			bool can = unfold && unfold.Can(p, t, heldMap);
			Print("[AZ_Vamp] TEST old_map: unfold can " + can);
			if (can)
				amc.PerformActionStart(unfold, t, heldMap);
		}
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VampFirstPersonShotLine, 4500, false, p, "old_map");
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZOldMapBloodOn, 7000, false, p);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZOldMapBloodOn, 9800, false, p);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VampFirstPersonShotLine, 11000, false, p, "old_map_bm");
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZOldMapBloodOff, 14000, false, p);
	}


	void AZOldMapBloodOn(PlayerBase p)
	{
		if (p)
		{
			p.m_AZBloodMoon = true;
			Print("[AZ_Vamp] TEST old_map: local blood moon on");
		}
	}

	void AZOldMapBloodOff(PlayerBase p)
	{
		if (p)
			p.m_AZBloodMoon = false;
		if (m_AZFaceLight)
		{
			m_AZFaceLight.Destroy();
			m_AZFaceLight = null;
		}
	}
}
