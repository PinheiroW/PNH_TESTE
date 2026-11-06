








modded class AZ_VampConfig
{
	bool cryptPortal = true;
}

class AZ_VampCryptKeys
{
	static const string KEY = "AZ_VampCryptKey";
	static const float KEY_LIFETIME = 10800;
	static bool s_WasNight;
	static EntityAI s_Key;


	static vector Spot()
	{
		return AZ_VampCrypt.Nave(AZ_VampCrypt.ALTAR_F - 1.4, 0.5, AZ_VampCrypt.DAIS_Y + 0.05);
	}


	static int s_CheckAt;
	static bool s_Portal;
	static bool PortalStands()
	{
		int now = GetGame().GetTime();
		if (now < s_CheckAt)
			return s_Portal;
		s_CheckAt = now + 1000;
		s_Portal = false;
		vector origin = AZ_VampCrypt.DoorOrigin();
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition(origin, 3, objs, null);
		foreach (Object o : objs)
		{
			if (o && o.IsKindOf("AZ_Crypt_Portal"))
			{
				s_Portal = true;
				break;
			}
		}
		return s_Portal;
	}


	static PlayerBase s_HoldP;
	static int s_HoldAt;
	static bool s_HoldV;

	static bool Holds(PlayerBase p)
	{
		if (!p || !p.GetInventory())
			return false;
		if (!GetGame().IsClient())
			return HoldsNow(p);
		int now = GetGame().GetTime();
		if (p == s_HoldP && now < s_HoldAt)
			return s_HoldV;
		s_HoldP = p;
		s_HoldAt = now + 500;
		s_HoldV = HoldsNow(p);
		return s_HoldV;
	}

	static bool HoldsNow(PlayerBase p)
	{
		array<EntityAI> items = new array<EntityAI>;
		p.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		foreach (EntityAI e : items)
		{
			if (e && e.IsKindOf(KEY))
				return true;
		}
		return false;
	}

	static bool OnDais()
	{
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition(Spot(), 0.6, objs, null);
		foreach (Object o : objs)
		{
			EntityAI e = EntityAI.Cast(o);
			if (e && e.IsKindOf(KEY) && !e.GetHierarchyParent())
			{
				s_Key = e;
				return true;
			}
		}
		return false;
	}


	static void Restock()
	{
		if (GetGame().IsServer() && AZ_VampCrypt.s_Built && !AZ_VampCryptTomb.NeedsKey())
		{

			if (OnDais() && s_Key)
			{
				GetGame().ObjectDelete(s_Key);
				s_Key = null;
				Print("[AZ_Vamp] crypt key: the leftover on the dais removed (portalNeedsKey false)");
			}
			return;
		}
		if (!GetGame().IsServer() || !AZ_VampCrypt.s_Built || !PortalStands() || s_Key || OnDais())
			return;
		vector at = Spot();
		ItemBase key = ItemBase.Cast(GetGame().CreateObjectEx(KEY, at, ECE_CREATEPHYSICS | ECE_KEEPHEIGHT));
		if (!key)
		{
			Print("[AZ_Vamp] crypt key: could not spawn " + KEY);
			return;
		}
		s_Key = key;
		key.SetOrientation(Vector(AZ_VampCrypt.HALL_YAW + 70, 0, 0));
		key.SetLifetimeMax(KEY_LIFETIME);
		key.SetLifetime(KEY_LIFETIME);
		Print("[AZ_Vamp] crypt key: laid on the altar dais at " + at);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Settled, 10000, false, key);
	}


	static void Settled(ItemBase key)
	{
		if (!key)
			return;
		vector pos = key.GetPosition();
		vector spot = Spot();
		float dy = pos[1] - spot[1];
		Print("[AZ_Vamp] crypt key: settled at " + pos + ", " + dy.ToString() + " m from the spot, inside " + AZ_VampCrypt.Inside(pos));
	}



	static int KeyCount()
	{
		int n = 0;
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition(Spot(), 3, objs, null);
		foreach (Object o : objs)
		{
			if (o && o.IsKindOf(KEY))
				n++;
		}
		return n;
	}

	static void Probe(int step)
	{
		if (step == 0)
		{
			Print("[AZ_Vamp] key probe: before, keys on the dais " + KeyCount() + ", tracked " + (s_Key != null));
			if (s_Key)
				GetGame().ObjectDelete(s_Key);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Probe, 1000, false, 1);
			return;
		}
		if (step == 1)
		{
			Print("[AZ_Vamp] key probe: key deleted, tracked ref null " + (s_Key == null) + ", keys on the dais " + KeyCount());
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Probe, 65000, false, 2);
			return;
		}
		int n = KeyCount();
		string verdict = "FAIL";
		if (n == 1 && s_Key)
			verdict = "PASS";
		if (step == 2)
		{
			Print("[AZ_Vamp] key probe: a minute after the decay, keys on the dais " + n + " " + verdict + " (expect exactly 1)");
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Probe, 62000, false, 3);
			return;
		}
		Print("[AZ_Vamp] key probe: another minute on, keys on the dais " + n + " " + verdict + " (expect still 1, no duplicate)");
	}


	static void Tick()
	{
		s_WasNight = GetGame().GetWorld().IsNight();
		Restock();
	}
}


modded class ActionAZCryptEnter
{
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!super.ActionCondition(player, target, item))
			return false;
		return !AZ_VampCryptKeys.PortalStands() || AZ_VampCryptKeys.Holds(player);
	}

	override void OnExecuteServer(ActionData action_data)
	{
		if (AZ_VampCryptKeys.PortalStands())
			action_data.m_Player.Message("The old key turns, grinding. The tomb door gives.", "colorAction");
		super.OnExecuteServer(action_data);
	}
}


class ActionAZCryptBarred : ActionInteractBase
{
	void ActionAZCryptBarred()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "Try the tomb door";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTAZAny;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!player || player.IsInVehicle() || !AZ_VampCrypt.AtDoor(player.GetPosition()))
			return false;
		return AZ_VampCryptKeys.PortalStands() && !AZ_VampCryptKeys.Holds(player);
	}

	override void OnExecuteServer(ActionData action_data)
	{
		action_data.m_Player.Message("Locked. An old iron key would open it.", "colorAction");
	}
}

modded class PlayerBase
{
	override void SetActions(out TInputActionMap InputActionMap)
	{
		super.SetActions(InputActionMap);
		AddAction(ActionAZCryptBarred, InputActionMap);
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionAZCryptBarred);
	}
}
