



class AZ_VampGraves
{
	static ref map<string, bool> s_Dug = new map<string, bool>;
	static ref array<Object> s_Risen = new array<Object>;
	static int s_CheckAt;
	static vector s_CheckPos;
	static bool s_Near;


	static Object FindGrave(vector pos)
	{
		array<Object> objs = new array<Object>;
		array<CargoBase> proxies = new array<CargoBase>;
		GetGame().GetObjectsAtPosition(pos, 4, objs, proxies);
		foreach (Object o : objs)
		{
			string id = o.GetType() + " " + o.GetShapeName();
			id.ToLower();
			if (AZ_VampLandmarks.IsGrave(id))
				return o;
		}
		return null;
	}


	static bool NearGrave(vector pos)
	{
		if (GetGame().IsServer())
			return FindGrave(pos) != null;
		int now = GetGame().GetTime();
		if (now < s_CheckAt && vector.Distance(pos, s_CheckPos) < 2)
			return s_Near;
		s_CheckAt = now + 2000;
		s_CheckPos = pos;
		s_Near = FindGrave(pos) != null;
		return s_Near;
	}


	static string Key(Object grave)
	{
		vector g = grave.GetPosition();
		return Math.Round(g[0] * 10).ToString() + "_" + Math.Round(g[2] * 10).ToString();
	}

	static void Dusk()
	{
		s_Dug.Clear();
	}

	static void Dig(PlayerBase p, vector cursor)
	{
		Object grave = FindGrave(cursor);
		if (!grave)
			return;
		if (vector.Distance(p.GetPosition(), grave.GetPosition()) > 6)
			return;
		vector at = grave.GetPosition() + p.GetDirection() * -0.6;
		at[1] = GetGame().SurfaceY(at[0], at[2]);
		string k = Key(grave);
		if (s_Dug.Contains(k))
		{
			p.Message("This grave was opened tonight already. Only loose earth.", "colorAction");
			return;
		}
		s_Dug.Set(k, true);
		float roll = Math.RandomFloat01();
		if (roll < 0.15)
		{
			GetGame().CreateObjectEx("AZ_VampHolyWater", at, ECE_PLACE_ON_SURFACE);
			p.Message("Under the lid, wrapped in cloth: holy water. Drink it to wash out a bite.", "colorFriendly");
		}
		else if (roll < 0.40)
		{
			GetGame().CreateObjectEx("Bone", at, ECE_PLACE_ON_SURFACE);
			GetGame().CreateObjectEx("Bone", at + Vector(0.3, 0, 0.2), ECE_PLACE_ON_SURFACE);
			p.Message("Old bones. Whoever this was, they stayed dead.", "colorAction");
		}
		else if (roll < 0.60)
		{
			vector z = at + p.GetDirection() * 1.5;
			z[1] = GetGame().SurfaceY(z[0], z[2]);
			EntityAI risen = EntityAI.Cast(GetGame().CreateObjectEx("ZmbM_HermitSkinny_Black", z, ECE_PLACE_ON_SURFACE | ECE_INITAI | ECE_CREATEPHYSICS));
			if (risen)
				s_Risen.Insert(risen);
			if (risen && AZ_VampState.Cfg().halloween && AZ_VampState.Cfg().pumpkinInfected > 0 && Math.RandomInt(0, 2) == 0 && !risen.GetInventory().FindAttachment(InventorySlots.HEADGEAR))
				risen.GetInventory().CreateAttachment("AZ_VampPumpkinHead");
			p.Message("Something in the grave is still moving.", "colorImportant");
		}
		else
			p.Message("Only cold earth and roots.", "colorAction");
		Print("[AZ_Vamp] grave dug by " + AZ_VampState.Uid(p) + " at " + at + " roll " + roll);
	}
}

class ActionAZDigGraveCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(12);
	}
}

class ActionAZDigGrave : ActionContinuousBase
{
	void ActionAZDigGrave()
	{
		m_CallbackClass = ActionAZDigGraveCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DIGMANIPULATE;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
		m_Text = "Dig up the grave";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTSurface(UAMaxDistances.DEFAULT);
	}

	override bool HasTarget()
	{
		return true;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!target || player.IsPlacingLocal())
			return false;
		return AZ_VampGraves.NearGrave(target.GetCursorHitPos());
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		AZ_VampGraves.Dig(action_data.m_Player, action_data.m_Target.GetCursorHitPos());
		MiscGameplayFunctions.DealEvinronmentAdjustedDmg(action_data.m_MainItem, action_data.m_Player, 4);
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionAZDigGrave);
	}
}

modded class Shovel
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionAZDigGrave);
	}
}

modded class FieldShovel
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionAZDigGrave);
	}
}
