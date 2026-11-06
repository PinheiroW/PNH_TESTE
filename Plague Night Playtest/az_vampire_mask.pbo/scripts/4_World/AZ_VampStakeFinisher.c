













modded class AZ_VampConfig
{
	bool stakeFinisherOneShot = true;

	int downMs = 60000;
	int downPerNight = 3;
	float downMinBlood = 2650;
	bool torporOn = true;
	int torporMs = 150000;
	float torporWakeHealth = 10;
	float torporWeakS = 60;
}

class AZ_VampStake: Inventory_Base
{
	override bool IsMeleeFinisher()
	{
		return true;
	}

	override array<int> GetValidFinishers()
	{
		return {EMeleeHitType.FINISHER_LIVERSTAB};
	}
}

class AZ_VampStakeFinisher
{
	static const float SERVER_RANGE = 2.0;

	static bool Allowed(PlayerBase attacker, InventoryItem weapon, PlayerBase victim)
	{
		if (!attacker || !weapon || !victim || !victim.IsAlive() || !victim.m_AZVampire)
			return false;
		if (attacker.m_AZVampire && !victim.IsUnconscious())
			return false;
		if (!weapon.IsKindOf("AZ_VampStake"))
			return false;
		return victim.IsUnconscious() || AZ_VampRear.Behind(attacker, victim);
	}

	static bool Sprinting(PlayerBase p)
	{
		HumanMovementState ms = new HumanMovementState();
		p.GetMovementState(ms);
		return ms.m_iMovement == DayZPlayerConstants.MOVEMENTIDX_SPRINT;
	}

	static bool ServerAllowed(PlayerBase attacker, InventoryItem weapon, PlayerBase victim)
	{
		if (!AZ_VampEvent.Active() || !AZ_VampState.Cfg() || !AZ_VampState.Cfg().stakeFinisherOneShot)
			return false;
		if (!Allowed(attacker, weapon, victim) || Sprinting(attacker))
			return false;
		return vector.Distance(attacker.GetPosition(), victim.GetPosition()) <= SERVER_RANGE;
	}
}

modded class DayZPlayerImplementMeleeCombat
{
	override protected int TrySelectFinisherType(InventoryItem weapon, EntityAI target)
	{
		PlayerBase victim = PlayerBase.Cast(target);
		if (victim)
		{
			if (m_HitType == EMeleeHitType.SPRINT || !AZ_VampStakeFinisher.Allowed(PlayerBase.Cast(m_DZPlayer), weapon, victim))
				return -1;
			int t = super.TrySelectFinisherType(weapon, target);
			if (t != -1 && victim.IsUnconscious())
				return EMeleeHitType.FINISHER_GENERIC;
			return t;
		}
		return super.TrySelectFinisherType(weapon, target);
	}
}

modded class DayZPlayerMeleeFightLogic_LightHeavy
{
	override protected bool EvaluateFinisherAttack(InventoryItem weapon, Object target)
	{
		PlayerBase victim = PlayerBase.Cast(target);
		if (victim && g_Game.IsServer() && m_MeleeCombat.GetFinisherType() > -1)
		{
			if (!AZ_VampStakeFinisher.ServerAllowed(m_Player, weapon, victim))
			{
				Print("[AZ_Vamp] stake finisher refused on " + AZ_VampState.Uid(victim) + " (gate failed at the hit), normal hit instead");
				m_MeleeCombat.SetFinisherType(-1);
				return false;
			}
			victim.m_AZStakedHeart = true;
			Print("[AZ_Vamp] stake finisher " + AZ_VampState.Uid(m_Player) + " -> " + AZ_VampState.Uid(victim));
			bool done = super.EvaluateFinisherAttack(weapon, target);
			victim.m_AZStakedHeart = false;
			return done;
		}
		return super.EvaluateFinisherAttack(weapon, target);
	}
}

modded class PlayerBase
{
	override bool CanBeBackstabbed()
	{
		if (!m_AZVampire || !IsAlive())
			return false;
		if (GetGame().IsServer())
			return AZ_VampEvent.Active();
		return true;
	}

	override bool EEOnDamageCalculated(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		if (m_AZStakedHeart && GetGame().IsServer() && IsAlive() && damageType == DamageType.CLOSE_COMBAT && ammo.IndexOf("FinisherHit") == 0)
		{
			AZ_VampAsh.SetBurn(this, true);
			Message("A stake through the heart. The wood burns as it goes in.", "colorImportant");
			PlayerBase hunter;
			if (source)
				hunter = PlayerBase.Cast(source.GetHierarchyRootPlayer());
			if (hunter)
				hunter.Message("Through the heart. It burns from the inside.", "colorImportant");
		}
		return super.EEOnDamageCalculated(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
	}

	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);

		AZ_VampEvidence.Begin("stake_finisher", source, this, false);
		if (m_AZStakedHeart && GetGame().IsServer() && damageType == DamageType.CLOSE_COMBAT && ammo.IndexOf("FinisherHit") == 0 && IsAlive())
			SetHealth("", "Health", 0);
		AZ_VampEvidence.End("stake_finisher", source, this);
	}
}
