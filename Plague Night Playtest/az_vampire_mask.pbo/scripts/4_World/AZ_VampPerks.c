











class AZ_VampDown
{
	static bool IsDown(PlayerBase p)
	{
		return p && p.m_AZDownUntil > 0;
	}

	static bool CanDown(PlayerBase p)
	{
		AZ_VampConfig c = AZ_VampState.Cfg();
		if (!p || !c || c.downMs <= 0 || (!c.torporOn && p.GetHealth("", "Blood") <= c.downMinBlood))
			return false;
		if (p.m_AZDownNight != AZ_VampProgress.s_NightNo)
		{
			p.m_AZDownNight = AZ_VampProgress.s_NightNo;
			p.m_AZDownCount = 0;
		}
		return p.m_AZDownCount < c.downPerNight;
	}



	// Silver rounds and wooden crossbow bolts (stakes) ignore vampire body-shot resist.
	static bool Piercing(string ammo)
	{
		return ammo == "AZ_VampBullet_357Silver" || (AZ_VampState.Cfg().boltsPierce && IsBolt(ammo));
	}

	static bool IsBolt(string ammo)
	{
		return ammo.IndexOf("Bullet_") == 0 && ammo.IndexOf("Bolt") != -1;
	}

	static bool Intercept(PlayerBase p, float hd, string ammo)
	{
		if (!GetGame().IsServer() || !p || !p.m_AZVampire || !p.IsAlive() || !AZ_VampEvent.Active() || !GetGame().GetWorld().IsNight())
			return false;
		if (Piercing(ammo) || AZ_VampLandmarks.OnHolyGround(p) || AZ_VampLandmarks.NearSanctuaryFire(p))
			return false;
		if (hd < p.GetHealth("", "Health"))
			return false;
		if (IsDown(p))
			return true;
		if (!CanDown(p))
			return false;
		Down(p);
		return true;
	}

	static void Down(PlayerBase p)
	{
		AZ_VampConfig c = AZ_VampState.Cfg();
		int now = GetGame().GetTime();
		p.m_AZDownCount++;
		p.m_AZDownAt = now;
		float torporBlood = p.GetHealth("", "Blood");
		bool enterTorpor = false;
		if (c.torporOn && torporBlood < c.hungryBlood)
			enterTorpor = true;
		p.m_AZTorpor = enterTorpor;
		p.SetSynchDirty();
		int duration = c.downMs;
		if (p.m_AZTorpor)
			duration = Math.Max(1, c.torporMs);
		p.m_AZDownUntil = now + duration;
		if (p.GetHealth("", "Health") > 5)
			p.SetHealth("", "Health", 5);
		p.SetHealth("", "Shock", 0);
		DayZPlayerSyncJunctures.SendPlayerUnconsciousness(p, true);
		if (p.m_AZTorpor)
		{
			p.Message("Starved and struck down, your body gives out. You will lie still a long while. A stake through the heart will end you.", "colorImportant");
			Print("[AZ_Vamp] down message=" + "Starved and struck down, your body gives out. You will lie still a long while. A stake through the heart will end you.");
			Print("[AZ_Vamp] torpor " + AZ_VampState.Uid(p) + " blood=" + p.GetHealth("", "Blood") + " count=" + p.m_AZDownCount + " ms=" + duration);
			return;
		}
		p.Message("You are down, not dead. A stake, silver, sanctuary fire or sun will finish you.", "colorImportant");
		Print("[AZ_Vamp] vampire knocked out " + AZ_VampState.Uid(p) + " (" + p.m_AZDownCount + "/" + c.downPerNight + " tonight) until +" + (c.downMs / 1000) + " s");
	}

	static bool Weak(PlayerBase p)
	{
		if (!p || !p.m_AZVampire || !p.IsAlive())
			return false;
		if (GetGame().IsServer())
			return AZ_VampState.Cfg().torporOn && GetGame().GetTime() < p.m_AZTorporWeakUntil;
		return p.m_AZTorporOn && p.m_AZTorporWeak;
	}

	static bool RefuseBite(PlayerBase p)
	{
		if (!Weak(p))
			return false;
		int now = GetGame().GetTime();
		if (now >= p.m_AZVampBiteWhyAt)
		{
			p.m_AZVampBiteWhyAt = now + 4000;
			p.Message("Your blood is too weak to bite yet.", "colorAction");
		}
		return true;
	}

	static void Wake(PlayerBase p)
	{
		AZ_VampEvidence.Begin("down_wake", null, p, false);
		p.m_AZDownUntil = 0;
		if (p.m_AZTorpor && AZ_VampState.Cfg().torporOn)
		{
			AZ_VampConfig c = AZ_VampState.Cfg();
			p.SetHealth("", "Health", Math.Max(1, c.torporWakeHealth));
			p.m_AZTorporWeakUntil = GetGame().GetTime() + Math.Max(0, c.torporWeakS) * 1000;
			p.m_AZTorporWeak = c.torporWeakS > 0;
			p.SetSynchDirty();
			p.Message("You wake weak and parched. Too weak to run or bite for now. Feed.", "colorAction");
			Print("[AZ_Vamp] down message=" + "You wake weak and parched. Too weak to run or bite for now. Feed.");
			Print("[AZ_Vamp] torpor wake weak " + AZ_VampState.Uid(p) + " health=" + p.GetHealth("", "Health") + " seconds=" + c.torporWeakS);
		}
		else if (p.GetHealth("", "Health") < 20)
			p.SetHealth("", "Health", 20);
		p.m_AZTorpor = false;
		p.SetSynchDirty();
		p.SetHealth("", "Shock", p.GetMaxHealth("", "Shock"));
		DayZPlayerSyncJunctures.SendPlayerUnconsciousness(p, false);
		Print("[AZ_Vamp] vampire wakes " + AZ_VampState.Uid(p) + " (unconscious " + p.IsUnconscious() + ")");
		AZ_VampEvidence.End("down_wake", null, p);
	}


	static void Update(PlayerBase p)
	{
		if (!p)
			return;
		if (p.m_AZTorpor && !AZ_VampState.Cfg().torporOn)
		{
			p.m_AZTorpor = false;
			p.m_AZDownUntil = Math.Min(p.m_AZDownUntil, p.m_AZDownAt + AZ_VampState.Cfg().downMs);
			p.SetSynchDirty();
		}
		if (p.m_AZTorporWeak && (!AZ_VampState.Cfg().torporOn || !p.m_AZVampire || !p.IsAlive() || GetGame().GetTime() >= p.m_AZTorporWeakUntil))
		{
			p.m_AZTorporWeak = false;
			p.m_AZTorporWeakUntil = 0;
			p.SetSynchDirty();
		}
		if (p.m_AZDownUntil <= 0)
			return;
		int now = GetGame().GetTime();
		if (!p.IsAlive() || !p.m_AZVampire)
		{
			p.m_AZDownUntil = 0;
			p.m_AZTorpor = false;
			p.SetSynchDirty();
			return;
		}
		if (now >= p.m_AZDownUntil)
		{
			Wake(p);
			return;
		}
		if (!p.IsUnconscious() && now > p.m_AZDownAt + 5000)
		{
			if (p.m_AZTorpor)
				DayZPlayerSyncJunctures.SendPlayerUnconsciousness(p, true);
			else
			{
				p.m_AZDownUntil = 0;
				return;
			}
		}
		p.SetHealth("", "Shock", 0);
	}
}

// AZ-2489: one attributable server line per hit the mod handles (cheap: no raycasts, no loops).
class AZ_VampDmgLog
{
	static string Short(PlayerBase p)
	{
		string u = AZ_VampState.Uid(p);
		if (u.Length() > 8)
			u = u.Substring(0, 8);
		return u;
	}

	// AddHealth environmental deaths report self/null as killer. Resolve the active
	// burn before EEKilled cleanup, never replacing an external attacker's identity.
	static string EnvironmentalCause(PlayerBase victim, Object killer)
	{
		if (!victim || (killer && killer != victim) || victim.m_AZVampBurn <= 0 || !AZ_VampEvent.Active())
			return "";
		if (AZ_VampSun.SunUp() && AZ_VampSun.WeatherShield() > 0 && AZ_VampSun.Exposed(victim))
			return "sun";
		if (AZ_VampLandmarks.OnHolyGround(victim))
			return "holy";
		if (AZ_VampLandmarks.NearSanctuaryFire(victim))
			return "fire";
		return "";
	}

	static void Log(PlayerBase victim, EntityAI source, string ammo, string zone, float hpBefore, string path, string environmentalCause = "")
	{
		if (!victim)
			return;
		string src = "none";
		string wpn = ammo;
		string dist = "?";
		if (environmentalCause != "")
		{
			src = environmentalCause;
			wpn = environmentalCause;
		}
		else if (source)
		{
			wpn = ammo + "/" + source.GetType();
			Man m = Man.Cast(source);
			if (!m)
				m = source.GetHierarchyRootPlayer();
			PlayerBase a = PlayerBase.Cast(m);
			if (a)
			{
				src = Short(a);
				dist = Math.Round(vector.Distance(victim.GetPosition(), a.GetPosition())).ToString();
			}
		}
		int night = 0;
		if (GetGame().GetWorld().IsNight())
			night = 1;
		int vamp = 0;
		if (victim.m_AZVampire)
			vamp = 1;
		Print("[AZ_Vamp] dmg victim=" + Short(victim) + " vamp=" + vamp + " src=" + src + " wpn=" + wpn + " dist=" + dist + " zone=" + zone + " hp=" + Math.Round(hpBefore) + "->" + Math.Round(victim.GetHealth("", "Health")) + " night=" + night + " path=" + path);
	}
}

class AZ_VampPerks
{
	static void Tick(PlayerBase p, bool night, bool hungry)
	{
		AZ_VampDown.Update(p);



		p.GetStatEnergy().Set(p.GetStatEnergy().GetMax());
		p.GetStatWater().Set(p.GetStatWater().GetMax());

		bool active = AZ_VampEvent.Active();
		if (active && night && !hungry && !AZ_VampHunger.NoHeal(p))
			p.AddHealth("", "Health", AZ_VampState.Cfg().nightHealPerSec * AZ_VampReveal.TICK_MS / 1000.0);



		int from = AZ_VampState.Cfg().starveShakeHunger;
		if (active && from > 0 && p.m_AZVampHunger >= from && Math.RandomFloat01() < 0.5)
			DayZPlayerSyncJunctures.SendKuruRequest(p, Math.Clamp((p.m_AZVampHunger - from + 1) / (11.0 - from), 0.2, 1));

		bool fast = active && night;
		if (fast != p.m_AZVampNightStamina && p.GetStaminaHandler())
		{
			p.m_AZVampNightStamina = fast;
			p.GetStaminaHandler().RecalculateDepletionMultiplier();
		}
		bool endless = fast && p.m_AZBloodMoon;
		if (endless != p.m_AZVampMoonStamina && p.GetStaminaHandler())
		{
			p.m_AZVampMoonStamina = endless;
			p.GetStaminaHandler().RecalculateDepletionMultiplier();
		}
	}

}

modded class PlayerBase
{
	bool m_AZVampNightStamina;
	bool m_AZVampMoonStamina;
	bool m_AZTorporOn = true;
	bool m_AZRiverDrinkOn = true;
	bool m_AZTorpor;
	bool m_AZTorporWeak;
	int m_AZTorporWeakUntil;

	override void Init()
	{
		super.Init();
		RegisterNetSyncVariableBool("m_AZTorporOn");
		RegisterNetSyncVariableBool("m_AZRiverDrinkOn");
		RegisterNetSyncVariableBool("m_AZTorpor");
		RegisterNetSyncVariableBool("m_AZTorporWeak");
	}

	override bool CanSprint()
	{
		return !AZ_VampDown.Weak(this) && super.CanSprint();
	}

	override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		if (GetGame().IsServer())
		{
			AZ_VampConfig cfg = AZ_VampState.Cfg();
			if (m_AZTorporOn != cfg.torporOn || m_AZRiverDrinkOn != cfg.riverDrinkOn)
			{
				m_AZTorporOn = cfg.torporOn;
				m_AZRiverDrinkOn = cfg.riverDrinkOn;
				SetSynchDirty();
			}
			if (m_AZTorpor || m_AZTorporWeak)
				AZ_VampDown.Update(this);
		}
		super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);
	}


	override void EEKilled(Object killer)
	{
		if (GetGame().IsServer())
		{
			m_AZTorpor = false;
			m_AZDownUntil = 0;
			m_AZTorporWeak = false;
			m_AZTorporWeakUntil = 0;
			SetSynchDirty();
		}

		PlayerBase slayer;
		string environmentalCause;
		if (GetGame().IsServer() && m_AZVampire)
		{
			environmentalCause = AZ_VampDmgLog.EnvironmentalCause(this, killer);
			EntityAI ke = EntityAI.Cast(killer);
			if (ke)
				slayer = PlayerBase.Cast(ke.GetHierarchyRootPlayer());
		}
		super.EEKilled(killer);
		if (GetGame().IsServer() && m_AZVampire)
			AZ_VampDmgLog.Log(this, EntityAI.Cast(killer), "", "", GetHealth("", "Health"), "killed", environmentalCause);
		if (slayer && slayer != this && slayer.m_AZVampire && slayer.IsAlive() && AZ_VampEvent.Active())
			AZ_VampProgress.OnVampKill(slayer);
	}

	static bool AZ_VampIsStake(EntityAI item)
	{

		return item && (item.IsKindOf("AZ_VampStake") || item.IsKindOf("SharpWoodenStick"));
	}
	int m_AZVampVomitAt;



	override bool EEOnDamageCalculated(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		if (GetGame().IsServer() && m_AZVampire && damageType == DamageType.CLOSE_COMBAT && source && AZ_VampEvent.Active())
		{
			Man attacker = Man.Cast(source);
			if (!attacker)
				attacker = source.GetHierarchyRootPlayer();
			PlayerBase hunter = PlayerBase.Cast(attacker);

			if (hunter && !AZ_VampIsStake(hunter.GetItemInHands()))
			{
				AZ_VampDmgLog.Log(this, source, ammo, dmgZone, GetHealth("", "Health"), "melee_nonstake_blocked");
				return false;
			}

			if (hunter && damageResult && !m_AZStakedHeart && !AZ_VampDown.IsDown(this))
			{
				float hpM = GetHealth("", "Health");
				AZ_VampEvidence.Begin("melee_intercept", source, this);
				if (AZ_VampDown.Intercept(this, damageResult.GetDamage("", "Health") * AZ_VampState.Cfg().stakeHitMult, ammo))
				{
					AZ_VampDmgLog.Log(this, source, ammo, dmgZone, hpM, "melee_stake_down");
					AZ_VampEvidence.End("melee_intercept", source, this, "knockout");
					return false;
				}
				AZ_VampEvidence.Discard("melee_intercept", source, this);
			}
		}




		if (GetGame().IsServer() && m_AZVampire && damageType == DamageType.FIRE_ARM && damageResult && AZ_VampEvent.Active() && dmgZone != "Head" && dmgZone != "Brain")
		{
			float k = AZ_VampState.Cfg().dayResist;
			if (GetGame().GetWorld().IsNight())
				k = AZ_VampState.Cfg().nightResist;
			if (ammo == "AZ_VampBullet_357Silver")
			{
				k = 0;
				AZ_VampAsh.SetBurn(this, true);
				Message("Silver. It burns where it went in.", "colorImportant");
			}
			else if (AZ_VampState.Cfg().boltsPierce && AZ_VampDown.IsBolt(ammo))
			{
				k = 0;
				Message("Wood. It splinters inside you.", "colorImportant");
			}
			float hpF = GetHealth("", "Health");
			bool heldF = AZ_VampDown.IsDown(this);
			float keep = 1.0 - Math.Clamp(k, 0, 1);
			if (!AZ_VampDown.Piercing(ammo))
				keep *= Math.Clamp(AZ_VampState.Cfg().bodyDamageMultiplier, 0, 1);
			// Crafted wooden bolts take ~3 body hits; factory bolts stay one-hit.
			if (AZ_VampState.Cfg().boltsPierce && ammo.IndexOf("Bullet_ImprovisedBolt") == 0)
				keep *= Math.Clamp(AZ_VampState.Cfg().craftedBoltMult, 0, 1);
			AZ_VampEvidence.Begin("fire:" + ammo + ":" + dmgZone, source, this);
			if (AZ_VampDown.Intercept(this, damageResult.GetDamage("", "Health") * keep, ammo))
			{
				Print("[AZ_Vamp] shot on vampire " + AZ_VampState.Uid(this) + " would be lethal at night: knocked out / held (zone " + dmgZone + ")");
				if (heldF)
					AZ_VampDmgLog.Log(this, source, ammo, dmgZone, hpF, "fire_held_while_down");
				else
					AZ_VampDmgLog.Log(this, source, ammo, dmgZone, hpF, "fire_lethal_to_down");
				AZ_VampEvidence.InterceptEnd("fire:" + ammo + ":" + dmgZone, source, this, heldF);
				return false;
			}
			Print("[AZ_Vamp] shot on vampire zone " + dmgZone + " ammo " + ammo + " raw health " + damageResult.GetDamage("", "Health") + " shock " + damageResult.GetDamage("", "Shock") + " keep " + keep);
			AddHealth("", "Health", -damageResult.GetDamage("", "Health") * keep);
			AddHealth("", "Blood", -damageResult.GetDamage("", "Blood") * keep);
			AddHealth("", "Shock", -damageResult.GetDamage("", "Shock") * keep);
			if (dmgZone != "")
				AddHealth(dmgZone, "Health", -damageResult.GetDamage(dmgZone, "Health") * keep);
			if (ammo == "AZ_VampBullet_357Silver")
				AZ_VampDmgLog.Log(this, source, ammo, dmgZone, hpF, "fire_silver_scaled");
			else if (AZ_VampState.Cfg().boltsPierce && AZ_VampDown.IsBolt(ammo))
				AZ_VampDmgLog.Log(this, source, ammo, dmgZone, hpF, "fire_bolt_scaled");
			else
				AZ_VampDmgLog.Log(this, source, ammo, dmgZone, hpF, "fire_body_scaled");
			AZ_VampEvidence.End("fire:" + ammo + ":" + dmgZone, source, this);
			return false;
		}

		if (GetGame().IsServer() && m_AZVampire && damageResult && damageType != DamageType.CLOSE_COMBAT && AZ_VampEvent.Active())
		{
			float hd = damageResult.GetDamage("", "Health");
			if (dmgZone != "")
				hd = Math.Max(hd, damageResult.GetDamage(dmgZone, "Health"));
			float hpO = GetHealth("", "Health");
			bool heldO = AZ_VampDown.IsDown(this);
			AZ_VampEvidence.Begin("other_intercept:" + ammo + ":" + dmgZone, source, this);
			if (AZ_VampDown.Intercept(this, hd, ammo))
			{
				Print("[AZ_Vamp] hit on vampire " + AZ_VampState.Uid(this) + " would be lethal at night: knocked out / held (type " + damageType + " zone " + dmgZone + ")");
				if (heldO)
					AZ_VampDmgLog.Log(this, source, ammo, dmgZone, hpO, "other_held_while_down_t" + damageType);
				else
					AZ_VampDmgLog.Log(this, source, ammo, dmgZone, hpO, "other_lethal_to_down_t" + damageType);
				AZ_VampEvidence.InterceptEnd("other_intercept:" + ammo + ":" + dmgZone, source, this, heldO);
				return false;
			}
			AZ_VampEvidence.Discard("other_intercept:" + ammo + ":" + dmgZone, source, this);
		}
		return super.EEOnDamageCalculated(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
	}

	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);

		if (GetGame().IsServer() && damageResult && IsAlive() && m_AZVampire && AZ_VampEvent.Active() && damageType == DamageType.CLOSE_COMBAT && source)
		{
			PlayerBase hunter = PlayerBase.Cast(source.GetHierarchyRootPlayer());
			if (!hunter)
				hunter = PlayerBase.Cast(source);
			if (hunter && AZ_VampIsStake(hunter.GetItemInHands()))
			{
				AZ_VampEvidence.Begin("stake_bonus", hunter, this, false);
				AddHealth("", "Health", -(AZ_VampState.Cfg().stakeHitMult - 1.0) * damageResult.GetDamage("", "Health"));
				AZ_VampEvidence.End("stake_bonus", hunter, this);
				Message("A stake. The wood burns where it bites.", "colorImportant");
				Print("[AZ_Vamp] stake hit on vampire " + AZ_VampState.Uid(this) + " health now " + GetHealth("", "Health"));
				AZ_VampDmgLog.Log(this, source, ammo, dmgZone, GetHealth("", "Health") + (AZ_VampState.Cfg().stakeHitMult - 1.0) * damageResult.GetDamage("", "Health"), "stake_bonus");
				return;
			}
		}


		if (GetGame().IsServer() && damageResult && IsAlive() && m_AZVampire && AZ_VampEvent.Active() && source && (source.IsInherited(ZombieBase) || source.IsInherited(AnimalBase)))
		{
			float ka = AZ_VampState.Cfg().dayAiResist;
			float hpA = GetHealth("", "Health");
			AddHealth("", "Health", damageResult.GetDamage("", "Health") * ka);
			AZ_VampDmgLog.Log(this, source, ammo, dmgZone, hpA, "ai_resist_refund");
			AddHealth("", "Blood", damageResult.GetDamage("", "Blood") * ka);
			AddHealth("", "Shock", damageResult.GetDamage("", "Shock") * ka);
			return;
		}
	}


	override void SetBrokenLegs(int stateId)
	{
		if (GetGame().IsServer() && m_AZVampire && AZ_VampEvent.Active() && stateId != eBrokenLegs.NO_BROKEN_LEGS)
		{
			SetHealth("LeftLeg", "Health", GetMaxHealth("LeftLeg", "Health"));
			SetHealth("RightLeg", "Health", GetMaxHealth("RightLeg", "Health"));
			SetHealth("LeftFoot", "Health", GetMaxHealth("LeftFoot", "Health"));
			SetHealth("RightFoot", "Health", GetMaxHealth("RightFoot", "Health"));
			stateId = eBrokenLegs.NO_BROKEN_LEGS;
		}
		super.SetBrokenLegs(stateId);
	}

	override bool Consume(PlayerConsumeData data)
	{
		bool r = super.Consume(data);
		if (r && GetGame().IsServer() && m_AZVampire && GetGame().GetTime() >= m_AZVampVomitAt)
		{
			m_AZVampVomitAt = GetGame().GetTime() + 30000;
			GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_VOMIT);
			Message("Your body rejects it. Only blood will do.", "colorImportant");
		}
		return r;
	}
}

modded class StaminaHandler
{
	override void RecalculateDepletionMultiplier()
	{
		float value = 1;
		foreach (int multiplier : m_ActiveDepletionModifiers)
			value *= m_RegisteredDepletionModifiers.Get(multiplier);
		if (m_Player && m_Player.m_AZVampNightStamina)
			value *= AZ_VampState.Cfg().nightStaminaDrain;
		if (m_Player && m_Player.m_AZVampMoonStamina)
			value = 0;
		if (value != m_StaminaDepletionMultiplier)
			SetDepletionMultiplier(value);
	}
}



modded class Environment
{
	override protected void ProcessHeatComfort()
	{
		super.ProcessHeatComfort();
		if (m_Player && m_Player.m_AZVampire && m_HeatComfort < 0)
		{
			m_HeatComfort = 0;
			m_Player.GetStatHeatComfort().Set(0);
		}
	}
}



modded class BloodRegenMdfr
{
	override void OnTick(PlayerBase player, float deltaT)
	{
		if (player && player.m_AZVampire)
		{
			if (!player.IsAlive() || player.GetBleedingBits() != 0 || !(deltaT > 0))
				return;
			AZ_VampConfig cfg = AZ_VampState.Cfg();
			float blood = player.GetHealth("", "Blood");
			float ceiling = Math.Min(cfg.vampDrainFloor, PlayerConstants.SHOCK_DAMAGE_BLOOD_THRESHOLD_HIGH + 50);
			ceiling = Math.Min(ceiling, player.GetMaxHealth("", "Blood"));
			if (cfg.vampRecoverPerSec > 0 && blood < ceiling)
				player.AddHealth("", "Blood", Math.Min(cfg.vampRecoverPerSec * deltaT, ceiling - blood));
			return;
		}
		super.OnTick(player, deltaT);
	}
}




modded class BleedingCheckMdfr
{
	static const float VAMP_BLOOD_MIN = 2600;

	override bool ActivateCondition(PlayerBase player)
	{
		if (player && player.m_AZVampire)
		{
			if (player.GetHealth("", "Blood") < VAMP_BLOOD_MIN)
				player.SetHealth("", "Blood", VAMP_BLOOD_MIN);
			return false;
		}
		return super.ActivateCondition(player);
	}
}




modded class PlayerBase
{
	override bool CanBeTargetedByAI(EntityAI ai)
	{
		if (m_AZVampire && ai && ai.IsInherited(ZombieBase) && AZ_VampEvent.Active())
			return false;
		return super.CanBeTargetedByAI(ai);
	}
}



modded class PlayerBase
{
	override void InsertAgent(int agent, float count = 1)
	{
		if (m_AZVampire && (agent != eAgents.CHEMICAL_POISON || !AZ_VampState.Cfg().vampireGasDamage))
			return;
		super.InsertAgent(agent, count);
	}
}

// Native Health regeneration must obey the daytime feeding requirement too.
modded class HealthRegenMdfr
{
	override void OnTick(PlayerBase player, float deltaT)
	{
		if (player && player.m_AZVampire && AZ_VampEvent.Active() && AZ_VampHunger.NoHeal(player))
			return;
		if (player && player.m_AZVampire && AZ_VampEvent.Active() && !GetGame().GetWorld().IsNight() && player.GetHealth("", "Blood") < AZ_VampState.Cfg().hungryBlood)
			return;
		if (player && player.m_AZVampire && AZ_VampEvent.Active())
			deltaT *= AZ_VampRest.DayHealFactor(player);
		super.OnTick(player, deltaT);
	}
}
