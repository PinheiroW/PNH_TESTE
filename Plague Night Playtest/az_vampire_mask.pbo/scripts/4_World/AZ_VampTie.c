




class AZ_VampTie
{
	static const float REACH_M = 2.2;


	static bool Try(PlayerBase vamp)
	{
		PlayerBase victim = Target(vamp);
		if (!victim)
			return false;

		EntityAI tool;
		if (!vamp.m_AZBloodMoon)
			tool = Tool(vamp);
		string locked = COMPEL_BONDS;
		if (tool)
			locked = tool.ConfigGetString("OnRestrainChange");
		else if (!vamp.m_AZBloodMoon)
		{
			vamp.Message("You need rope, tape or wire to bind their hands.", "colorAction");
			return true;
		}
		if (!Bind(victim, locked))
			return true;
		if (tool)
			GetGame().ObjectDelete(tool);
		vamp.Message("You bind their hands. They will keep.", "colorAction");
		victim.Message("Your hands are bound.", "colorImportant");
		Print("[AZ_Vamp] tie " + AZ_VampState.Uid(vamp) + " -> " + AZ_VampState.Uid(victim) + " with " + locked);
		return true;
	}

	static const string COMPEL_BONDS = "AZ_VampWillBonds";
	static const int HINT_TIMES = 3;
	static ref map<string, int> s_Hints = new map<string, int>;



	static void Hint(PlayerBase vamp, string kind, string text)
	{
		if (!vamp)
			return;
		string k = AZ_VampState.Uid(vamp) + ":" + kind;
		int n = s_Hints.Get(k);
		if (n >= HINT_TIMES)
			return;
		s_Hints.Set(k, n + 1);
		vamp.Message(text, "colorFriendly");
	}


	static bool Bind(PlayerBase victim, string locked)
	{
		EntityAI inHands = victim.GetHumanInventory().GetEntityInHands();
		if (inHands && !victim.ServerDropEntity(inHands))
		{
			Print("[AZ_Vamp] tie: " + AZ_VampState.Uid(victim) + " could not drop " + inHands.GetType() + ", not bound");
			return false;
		}
		EntityAI bind = victim.GetHumanInventory().CreateInHands(locked);
		if (!bind)
		{
			Print("[AZ_Vamp] tie: could not create " + locked + " in the hands of " + AZ_VampState.Uid(victim));
			return false;
		}
		victim.SetRestrained(true);
		victim.OnItemInHandsChanged();
		if (locked == COMPEL_BONDS)
			victim.m_AZWillBonds = bind;
		return true;
	}


	static void Unbind(PlayerBase victim)
	{
		if (!victim || !victim.IsRestrained())
			return;
		EntityAI held = victim.GetHumanInventory().GetEntityInHands();
		if (!held || held != victim.m_AZWillBonds)
			return;
		victim.m_AZWillBonds = null;
		GetGame().ObjectDelete(held);
		victim.SetRestrained(false);
		victim.OnItemInHandsChanged();
	}



	static void ClearStale(PlayerBase p)
	{
		if (!p || !p.IsAlive())
			return;
		EntityAI held = p.GetHumanInventory().GetEntityInHands();
		if (!held || !held.IsKindOf(COMPEL_BONDS) || held == p.m_AZWillBonds)
			return;
		GetGame().ObjectDelete(held);
		p.SetRestrained(false);
		p.OnItemInHandsChanged();
		Print("[AZ_Vamp] compel: stale bonds cleared at login for " + AZ_VampState.Uid(p));
	}


	static PlayerBase Target(PlayerBase vamp)
	{
		array<Object> objs = new array<Object>;
		array<CargoBase> proxies = new array<CargoBase>;
		GetGame().GetObjectsAtPosition3D(vamp.GetPosition(), REACH_M, objs, proxies);
		PlayerBase best;
		float bestD = REACH_M + 1;
		foreach (Object o : objs)
		{
			PlayerBase h = PlayerBase.Cast(o);
			if (!h || h == vamp || !h.IsAlive() || h.m_AZVampire || h.IsRestrained() || h.m_AZVampHeld)
				continue;
			if (AZ_VampShadow.CompelProtected(h) || !AZ_VampShadow.Sees(vamp, h))
				continue;
			bool down = h.IsUnconscious();
			if (!down && !h.GetIdentity())
				down = h.GetHealth("", "Blood") <= AZ_VampState.Cfg().bloodFloor + 50;
			if (!down)
				continue;
			float d = vector.Distance(vamp.GetPosition(), h.GetPosition());
			if (d < bestD)
			{
				bestD = d;
				best = h;
			}
		}
		return best;
	}

	static bool IsTool(EntityAI it)
	{
		if (!it || it.ConfigGetString("OnRestrainChange") == "")
			return false;
		string t = it.GetType();
		t.ToLower();
		return t.IndexOf("locked") < 0;
	}


	static EntityAI Tool(PlayerBase vamp)
	{
		EntityAI hands = vamp.GetHumanInventory().GetEntityInHands();
		if (IsTool(hands))
			return hands;
		array<EntityAI> items = new array<EntityAI>;
		vamp.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		foreach (EntityAI it : items)
		{
			if (it != vamp && IsTool(it))
				return it;
		}
		return null;
	}
}

modded class PlayerBase
{
	EntityAI m_AZWillBonds;
}


class AZ_VampWillAsh extends ItemBase
{
	override void EEInit()
	{
		super.EEInit();
		if (GetGame().IsServer())
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(GetGame().ObjectDelete, 200, false, this);
	}
}

class AZ_VampWillBonds extends RopeLocked
{
}
