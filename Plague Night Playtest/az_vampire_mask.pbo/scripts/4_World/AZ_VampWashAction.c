// AZ-2279: own "wash the blood from your mouth" actions for a bloodied vampire.
// Vanilla wash actions are untouched; this adds a water-target action (no stance requirement)
// and a bottle-on-self action. Both finish by calling AZ_VampBlood.Wash on the server.
class AZ_VampWash
{
	static const float WASH_SECONDS = 3.0;
	static const float BOTTLE_ML = 50;

	static bool Needed(PlayerBase p)
	{
		return p && p.m_AZVampire && p.m_AZVampBloodied && p.IsAlive() && !p.IsUnconscious() && !p.IsInVehicle();
	}
}

class ActionAZVampWashWaterCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(AZ_VampWash.WASH_SECONDS);
	}
}

class ActionAZVampWashWater : ActionContinuousBase
{
	void ActionAZVampWashWater()
	{
		m_CallbackClass = ActionAZVampWashWaterCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_WASHFACEPOND;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "Wash the blood from your mouth";
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTLiquid(LIQUID_GROUP_WATER - LIQUID_SNOW - LIQUID_HOTWATER, UAMaxDistances.DEFAULT, UAMaxDistances.DEFAULT);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return AZ_VampWash.Needed(player);
	}

	override void OnStart(ActionData action_data)
	{
		super.OnStart(action_data);
		action_data.m_Player.TryHideItemInHands(true);
	}

	override void OnEnd(ActionData action_data)
	{
		action_data.m_Player.TryHideItemInHands(false);
	}

	override bool IsLockTargetOnUse()
	{
		return false;
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		AZ_VampBlood.Wash(action_data.m_Player);
	}
}

class ActionAZVampWashBottleCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(AZ_VampWash.WASH_SECONDS);
	}
}

class ActionAZVampWashBottle : ActionContinuousBase
{
	void ActionAZVampWashBottle()
	{
		m_CallbackClass = ActionAZVampWashBottleCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_WASHFACE;
		m_FullBody = false;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "Wash the blood from your mouth";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINotRuinedAndEmpty;
		m_ConditionTarget = new CCTSelf;
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return AZ_VampWash.Needed(player) && item && (item.GetLiquidType() & LIQUID_GROUP_WATER) != 0 && item.GetQuantity() >= AZ_VampWash.BOTTLE_ML && !item.GetIsFrozen();
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		AZ_VampBlood.Wash(action_data.m_Player);
		if (action_data.m_MainItem)
			action_data.m_MainItem.AddQuantity(-AZ_VampWash.BOTTLE_ML, false);
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionAZVampWashWater);
		actions.Insert(ActionAZVampWashBottle);
	}
}

modded class PlayerBase
{
	override void SetActions(out TInputActionMap InputActionMap)
	{
		super.SetActions(InputActionMap);
		AddAction(ActionAZVampWashWater, InputActionMap);
	}
}

modded class Bottle_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionAZVampWashBottle);
	}
}
