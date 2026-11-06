











modded class AZ_VampConfig
{
	bool portalNeedsKey = false;
}

class AZ_VampCryptTomb
{
	static const float LEAVE_F = -14.0;
	static const float LEAVE_R = -7.5;
	static const float LEAVE_M = 1.0;
	static const float CORNER_F = -14.0;
	static const float CORNER_R = -11.6;
	static const float VOTIVE_F = -13.5;
	static const float VOTIVE_R = -7.3;


	static bool On()
	{
		return AZ_VampCryptKeys.PortalStands() && AZ_VampCryptGallery.s_Slab != null;
	}

	static bool NeedsKey()
	{
		int cb = AZ_VampCrypt.ClientBits();
		if (cb)
			return (cb & 8) != 0;
		return AZ_VampState.Cfg() && AZ_VampState.Cfg().portalNeedsKey;
	}

	static array<vector> Slots()
	{
		array<vector> s = new array<vector>;
		s.Insert(AZ_VampCrypt.Hall(-14.0, -9.6, 0.05));
		s.Insert(AZ_VampCrypt.Hall(-13.9, -10.4, 0.05));
		s.Insert(AZ_VampCrypt.Hall(-13.8, -11.6, 0.05));
		return s;
	}

	static vector Pick()
	{
		return AZ_VampCryptLedge.PickFrom(Slots(), "tomb");
	}

	static vector Facing(vector at)
	{
		vector d = AZ_VampCrypt.Hall(CORNER_F, CORNER_R, 0.05) - at;
		d[1] = 0;
		if (d.Length() < 0.5)
			return AZ_VampCrypt.Fwd(AZ_VampCrypt.HALL_YAW);
		return d.Normalized();
	}


	static void Build()
	{
		if (!AZ_VampCrypt.TombDoorOn())
			return;
		AZ_VampCrypt.PutCrypt("AZ_VampVotive", AZ_VampCrypt.Hall(VOTIVE_F, VOTIVE_R, 0.0), AZ_VampCrypt.HALL_YAW);
		Print("[AZ_Vamp] crypt tomb: votive at the gallery's west end, leave at hall " + AZ_VampCrypt.Hall(LEAVE_F, LEAVE_R) + ", on " + On() + ", key " + NeedsKey());
	}

	static bool AtLeave(PlayerBase p)
	{
		if (!p || !AZ_VampCrypt.Site())
			return false;


		if (GetGame().IsServer() && !AZ_VampCryptKeys.PortalStands())
			return false;
		if (GetGame().IsClient() && !VotiveStands())
			return false;
		vector to = AZ_VampCrypt.Hall(LEAVE_F, LEAVE_R) - p.GetPosition();
		if (Math.AbsFloat(to[1]) > 1.5)
			return false;
		to[1] = 0;
		if (to.Length() > LEAVE_M)
			return false;
		vector look = p.GetDirection();
		look[1] = 0;
		bool at = vector.Dot(look.Normalized(), AZ_VampCrypt.Side(AZ_VampCrypt.HALL_YAW)) > 0.5;
		if (at && !s_ClientPrompt && GetGame().IsClient())
		{
			s_ClientPrompt = true;
			Print("[AZ_Vamp] crypt tomb client: the Leave by the tomb prompt is up");
		}
		return at;
	}

	static bool s_ClientPrompt;
	static int s_VotiveCheckAt;
	static bool s_Votive;

	static bool VotiveStands()
	{
		int now = GetGame().GetTime();
		if (now < s_VotiveCheckAt)
			return s_Votive;
		s_VotiveCheckAt = now + 2000;
		s_Votive = false;
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition(AZ_VampCrypt.Hall(VOTIVE_F, VOTIVE_R, 0.0), 1.0, objs, null);
		foreach (Object o : objs)
		{
			if (o && o.GetType() == "AZ_VampVotive")
				s_Votive = true;
		}
		return s_Votive;
	}

	static void Leave(PlayerBase p)
	{
		if (!AtLeave(p) || !AZ_VampCrypt.CanMove(p))
			return;
		if (GetGame().GetTime() - p.m_AZLastHitAt < AZ_VampCryptLedge.LEAVE_HURT_MS)
		{
			p.Message("Not with blood still running. Wait a moment, then try again.", "colorAction");
			Print("[AZ_Vamp] crypt tomb: " + AZ_VampState.Uid(p) + " leave refused (hurt)");
			return;
		}
		vector at = AZ_VampCryptLedge.PickFrom(AZ_VampCrypt.MouthSlots(), "tomb exit");
		p.m_AZCryptExitGrace = true;
		AZ_VampCrypt.MoveTo(p, at, -AZ_VampCrypt.Fwd(AZ_VampCrypt.DOOR_YAW), "tomb_exit");
		Print("[AZ_Vamp] crypt tomb: " + AZ_VampState.Uid(p) + " left by the tomb");
	}
}



modded class ActionAZCryptEnter
{
	override string GetText()
	{
		return "Enter the tomb";
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!player || player.IsInVehicle() || !AZ_VampCrypt.AtDoor(player.GetPosition()) || AZ_VampCrypt.DoorExitOnly())
			return false;
		if (!AZ_VampCryptKeys.PortalStands() || !AZ_VampCryptTomb.NeedsKey())
			return true;
		return AZ_VampCryptKeys.Holds(player);
	}

	override void OnExecuteServer(ActionData action_data)
	{
		PlayerBase p = action_data.m_Player;
		if (AZ_VampCryptKeys.PortalStands() && AZ_VampCryptTomb.NeedsKey() && !AZ_VampCryptKeys.Holds(p))
		{
			p.Message("Locked. An old iron key would open it.", "colorAction");
			return;
		}
		if (AZ_VampCryptTomb.NeedsKey())
		{
			super.OnExecuteServer(action_data);
			return;
		}
		AZ_VampCrypt.Enter(p);
	}
}

modded class ActionAZCryptBarred
{
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return AZ_VampCryptTomb.NeedsKey() && super.ActionCondition(player, target, item);
	}
}

class ActionAZCryptTombLeaveCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(5);
	}
}

class ActionAZCryptTombLeave : ActionContinuousBase
{
	void ActionAZCryptTombLeave()
	{
		m_CallbackClass = ActionAZCryptTombLeaveCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "Leave by the tomb";
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTAZAny;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return AZ_VampCryptTomb.AtLeave(player);
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		AZ_VampCryptTomb.Leave(action_data.m_Player);
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionAZCryptTombLeave);
	}
}

modded class PlayerBase
{
	override void SetActions(out TInputActionMap InputActionMap)
	{
		super.SetActions(InputActionMap);
		AddAction(ActionAZCryptTombLeave, InputActionMap);
	}
}
