





class AZ_VampRepel
{
	static const float CONE_COS = 0.88;
	static ref array<PlayerBase> s_TestHolders = new array<PlayerBase>;


	static bool Tick(PlayerBase vamp, array<PlayerBase> online)
	{
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		if (cfg.lightRepelM <= 0 || !vamp.IsAlive() || !GetGame().GetWorld().IsNight())
			return false;
		vector vp = vamp.GetPosition() + "0 1.2 0";
		array<PlayerBase> holders = online;
		if (s_TestHolders.Count() > 0)
		{
			holders = new array<PlayerBase>;
			holders.InsertAll(online);
			holders.InsertAll(s_TestHolders);
		}
		foreach (PlayerBase h : holders)
		{
			if (!h || h == vamp || h.m_AZVampire || !h.IsAlive() || h.IsUnconscious())
				continue;
			vector hp = h.GetPosition() + "0 1.5 0";
			float d = vector.Distance(hp, vp);


			if (d > cfg.lightRepelM)
				continue;
			float reach = LightReach(h, cfg.lightRepelM);
			if (reach <= 0 || d > reach)
				continue;
			vector to = vp - hp;
			to.Normalize();
			vector face = h.GetDirection();
			if (d > 2.0 && vector.Dot(face, to) < CONE_COS)
				continue;
			if (!ClearLine(hp, vp, h, vamp))
				continue;
			Sear(vamp, h, d);
			return true;
		}
		return false;
	}


	static float LightReach(PlayerBase h, float beam)
	{
		ItemBase hand = h.GetItemInHands();
		if (hand && hand.HasEnergyManager() && hand.GetCompEM().IsWorking())
		{
			if (Torch.Cast(hand) || Roadflare.Cast(hand))
				return beam * 0.6;
			if (Flashlight.Cast(hand))
				return beam;
		}
		ItemBase head = ItemBase.Cast(h.GetInventory().FindAttachment(InventorySlots.HEADGEAR));
		if (Headtorch_ColorBase.Cast(head) && head.GetCompEM().IsWorking())
			return beam * 0.8;
		return 0;
	}

	static bool ClearLine(vector from, vector to, PlayerBase a, PlayerBase b)
	{
		vector hit, normal;
		int comp;
		set<Object> results = new set<Object>;
		if (!DayZPhysics.RaycastRV(from, to, hit, normal, comp, results, null, a, false, false, ObjIntersectFire))
			return true;
		foreach (Object o : results)
		{
			if (o == b)
				return true;
		}
		return false;
	}

	static void Sear(PlayerBase vamp, PlayerBase h, float d)
	{
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		AZ_VampEvidence.Begin("light_sear", h, vamp, true);
		vamp.AddHealth("", "Health", -cfg.lightBurnPerSec * AZ_VampReveal.TICK_MS / 1000.0);
		AZ_VampEvidence.End("light_sear", h, vamp);
		StaminaHandler st = vamp.GetStaminaHandler();
		if (st && st.GetStamina() > cfg.lightStaminaCap)
			st.SetStamina(cfg.lightStaminaCap);
		int now = GetGame().GetTime();
		if (now - vamp.m_AZLightSaidAt > 20000)
		{
			vamp.m_AZLightSaidAt = now;
			vamp.Message("The light sears your eyes. You can't push through it.", "colorImportant");
			Print("[AZ_Vamp] light repel " + AZ_VampState.Uid(vamp) + " by " + AZ_VampState.Uid(h) + " at " + Math.Round(d) + " m");
		}
		if (now - h.m_AZLightSaidAt > 60000)
		{
			h.m_AZLightSaidAt = now;
			h.Message("It flinches from your light.", "colorFriendly");
		}
	}
}

modded class PlayerBase
{
	int m_AZLightSaidAt;
}
