class AZ_VampRiverDrink
{
	static ref map<string, int> s_LastDrink = new map<string, int>;

	static bool AtBank(PlayerBase p)
	{
		if (!p || (GetGame().IsServer() && !AZ_VampState.Cfg().riverDrinkOn) || (!GetGame().IsServer() && !p.m_AZRiverDrinkOn) || !p.m_AZVampire || !p.IsAlive() || p.IsUnconscious() || p.IsInVehicle() || p.m_AZVampHeld || !AZ_VampCrypt.Site())
			return false;
		vector here = AZ_VampCrypt.HallLocal(p.GetPosition());
		float bank = Math.AbsFloat(here[1]);
		return here[0] >= AZ_VampCrypt.HALL_F0 && here[0] <= AZ_VampCrypt.APSE_F && bank >= AZ_VampCryptRiverPortal.BANK_R && bank <= AZ_VampCrypt.WALK_R && Math.AbsFloat(here[2] + AZ_VampCrypt.LEDGE_H) <= AZ_VampState.Cfg().riverDrinkBankHeightM;
	}

	// Cap sits above the hunger-clear level (hungryBlood + 100 hysteresis), so drinking can end hunger.
	static float Cap()
	{
		return AZ_VampState.Cfg().hungryBlood + 150;
	}

	static bool SceneBusy()
	{
		return AZ_VampCryptLurkers.s_RiverWakeArmed || GetGame().GetTime() < AZ_VampCryptLurkers.s_RiverPinUntil;
	}

	static string Refusal(PlayerBase p)
	{
		if (!AtBank(p) || !p.GetIdentity() || !AZ_VampState.Cfg().crypt || !AZ_VampEvent.Active())
			return "The blood river is out of reach.";
		if (SceneBusy() || AZ_VampCrypt.IsGuest(p))
			return "The river belongs to the rite. Wait.";
		AZ_VampConfig c = AZ_VampState.Cfg();
		if (p.GetHealth("", "Blood") >= Cap())
			return "The river cannot sate you further. You must hunt.";
		if (c.riverDrinkPerSec <= 0)
			return "The river offers you nothing.";
		string uid = AZ_VampState.Uid(p);
		if (s_LastDrink.Contains(uid))
		{
			float left = c.riverDrinkCooldownMin * 60000 - (GetGame().GetTime() - s_LastDrink.Get(uid));
			if (left > 0)
				return "The river needs " + Math.Ceil(left / 60000).ToString() + " more minute(s).";
		}
		return "";
	}
}

class AZ_VampRiverDrinkData : ActionData
{
	bool m_AZAccepted;
}

class ActionAZVampRiverDrinkCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		float interval = AZ_VampState.Cfg().riverDrinkTickS;
		if (interval <= 0)
			interval = UATimeSpent.DRINK_WELL;
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(interval);
	}
}

class ActionAZVampRiverDrink : ActionContinuousBase
{
	void ActionAZVampRiverDrink()
	{
		m_CallbackClass = ActionAZVampRiverDrinkCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DRINKWELL;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "Drink from the blood river";
	}

	override ActionData CreateActionData()
	{
		return new AZ_VampRiverDrinkData;
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
		return AZ_VampRiverDrink.AtBank(player) && !item;
	}

	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		AZ_VampRiverDrinkData data = AZ_VampRiverDrinkData.Cast(action_data);
		if (!data)
			return;
		string why = AZ_VampRiverDrink.Refusal(data.m_Player);
		if (why != "")
		{
			data.m_Player.Message(why, "colorAction");
			Print("[AZ_Vamp] river drink " + AZ_VampState.Uid(data.m_Player) + " refused reason=" + why);
			data.m_Player.GetActionManager().Interrupt();
			return;
		}
		data.m_AZAccepted = true;
	}

	override bool ActionConditionContinue(ActionData action_data)
	{
		if (!ActionCondition(action_data.m_Player, action_data.m_Target, action_data.m_MainItem))
			return false;
		if (!GetGame().IsServer())
			return true;
		AZ_VampRiverDrinkData data = AZ_VampRiverDrinkData.Cast(action_data);
		AZ_VampConfig c = AZ_VampState.Cfg();
		return data && data.m_AZAccepted && AZ_VampEvent.Active() && c.crypt && c.riverDrinkOn && !AZ_VampRiverDrink.SceneBusy() && !AZ_VampCrypt.IsGuest(data.m_Player) && c.riverDrinkPerSec > 0 && data.m_Player.GetHealth("", "Blood") < AZ_VampRiverDrink.Cap();
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		if (!ActionConditionContinue(action_data))
			return;
		Param1<float> spent = Param1<float>.Cast(action_data.m_ActionComponent.GetACData());
		if (!spent || spent.param1 <= 0)
			return;
		PlayerBase p = action_data.m_Player;
		if (!p.GetIdentity())
			return;
		AZ_VampConfig c = AZ_VampState.Cfg();
		float before = p.GetHealth("", "Blood");
		float gain = Math.Min(c.riverDrinkPerSec * spent.param1, AZ_VampRiverDrink.Cap() - before);
		if (gain <= 0)
			return;
		p.AddHealth("", "Blood", gain);
		AZ_VampReveal.StarveReset(p);
		string uid = AZ_VampState.Uid(p);
		AZ_VampRiverDrink.s_LastDrink.Set(uid, GetGame().GetTime());
		AZ_VampHunger.NoHeal(p);
		if (p.GetHealth("", "Blood") >= AZ_VampRiverDrink.Cap())
			p.Message("The river has given all it can. Your hunger is quiet, for now.", "colorAction");
		Print("[AZ_Vamp] river drink " + uid + " gained=" + (p.GetHealth("", "Blood") - before) + " now=" + p.GetHealth("", "Blood"));
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionAZVampRiverDrink);
	}
}

modded class PlayerBase
{
	override void SetActions(out TInputActionMap InputActionMap)
	{
		super.SetActions(InputActionMap);
		AddAction(ActionAZVampRiverDrink, InputActionMap);
	}
}
