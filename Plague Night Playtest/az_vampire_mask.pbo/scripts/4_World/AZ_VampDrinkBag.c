



class ActionAZVampDrinkBagCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(4.0);
	}
}

class ActionAZVampDrinkBag : ActionContinuousBase
{
	void ActionAZVampDrinkBag()
	{
		m_CallbackClass = ActionAZVampDrinkBagCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_DRINK;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_DRINK;
		m_Text = "Drink blood";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTSelf;
	}

	override bool HasProneException()
	{
		return true;
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return player && player.m_AZVampire && item && item.GetQuantity() > 1;
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		PlayerBase vamp = action_data.m_Player;
		ItemBase bag = action_data.m_MainItem;
		if (!vamp || !bag)
			return;
		float ml = bag.GetQuantity();
		// Match bite feeding: cap at native maximum Blood; fullBlood is a healing threshold.
		float gain = Math.Min(ml * Math.Clamp(AZ_VampState.Cfg().bagGain, 0, 2), Math.Max(0, vamp.GetMaxHealth("", "Blood") - vamp.GetHealth("", "Blood")));
		vamp.AddHealth("", "Blood", gain);
		bag.SetQuantity(0);
		AZ_VampReveal.Fed(vamp);
		vamp.Message("Cold, but it will do.", "colorFriendly");
		Print("[AZ_Vamp] drank a blood bag " + AZ_VampState.Uid(vamp) + " +" + ml);
	}
}

modded class BloodBagFull
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionAZVampDrinkBag);
	}
}

modded class BloodBagIV
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionAZVampDrinkBag);
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionAZVampDrinkBag);
	}
}
