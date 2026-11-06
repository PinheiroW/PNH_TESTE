










class AZ_VampCryptSarcophagus
{
	static const float SARC_F = 4.4;
	static const float SARC_R = -11.25;
	static const float SARC_Y = 0.9;
	static const float REACH = 1.6;
	static const int ROOT_MS = 3000;
	static const int PIN_MS = 100;
	static const float ARM_DMG = 12;
	static const int COFFIN_CD_MS = 300000;
	static const int PLAYER_CD_MS = 600000;
	static const string LOOT = "Morphine,Epinephrine,BandageDressing,AmmoBox_9x19_25rnd,AmmoBox_762x39_20Rnd,AZ_VampAmmoBox_357Silver_10Rnd";

	static AZ_VampCoffin_False s_Coffin;
	static int s_ReadyAt;
	static int s_Slams;
	static int s_Loots;
	static ref map<string, int> s_Next;
	static bool s_ProbeArmed = false;
	static bool s_ProbeForce = false;

	static void Build()
	{
		s_Next = new map<string, int>;
		s_Coffin = AZ_VampCoffin_False.Cast(AZ_VampCrypt.PutCrypt("AZ_VampCoffin_False", AZ_VampCrypt.Hall(SARC_F, SARC_R, SARC_Y), AZ_VampCrypt.HALL_YAW));
		Print("[AZ_Vamp] false sarcophagus built: " + (s_Coffin != null) + " at hall f " + SARC_F + " r " + SARC_R);
	}

	static bool CanReach(PlayerBase p, Object target)
	{
		if (!p || !AZ_VampCoffin_False.Cast(target) || !AZ_VampCryptTraps.IsHuman(p))
			return false;
		return vector.Distance(p.GetPosition(), target.GetPosition()) < REACH;
	}


	static void Reach(PlayerBase p)
	{
		if (!p || !AZ_VampCryptTraps.IsHuman(p))
			return;
		int now = GetGame().GetTime();
		string uid = AZ_VampState.Uid(p);
		if (now < s_ReadyAt || (s_Next.Contains(uid) && now < s_Next.Get(uid)))
		{
			if (p.GetIdentity())
				p.Message("Only dust and bone in there now.", "colorAction");
			Print("[AZ_Vamp] false sarcophagus: " + uid + " reached in, resting");
			return;
		}
		s_ReadyAt = now + COFFIN_CD_MS;
		s_Next.Set(uid, now + PLAYER_CD_MS);
		bool armed = Math.RandomInt(0, 3) != 0;
		if (s_ProbeForce)
			armed = s_ProbeArmed;
		if (!armed || !AZ_VampCryptTraps.On())
		{
			Loot(p);
			return;
		}
		Slam(p);
	}

	static void Loot(PlayerBase p)
	{
		s_Loots++;
		if (AZ_VampCryptLoot.On())
		{
			string got = AZ_VampCryptLoot.Sarc(p);
			if (p.GetIdentity())
				p.Message("Your fingers close on something under the shroud.", "colorAction");
			Print("[AZ_Vamp] false sarcophagus: " + AZ_VampState.Uid(p) + " found " + got);
			return;
		}
		array<string> list = new array<string>;
		LOOT.Split(",", list);
		string cls = list[Math.RandomInt(0, list.Count())];
		EntityAI item = p.GetHumanInventory().CreateInHands(cls);
		if (!item)
			item = EntityAI.Cast(GetGame().CreateObjectEx(cls, p.GetPosition(), ECE_PLACE_ON_SURFACE | ECE_DYNAMIC_PERSISTENCY));
		if (p.GetIdentity())
			p.Message("Your fingers close on something under the shroud.", "colorAction");
		Print("[AZ_Vamp] false sarcophagus: " + AZ_VampState.Uid(p) + " found " + cls + " (" + (item != null) + ")");
	}

	static void Slam(PlayerBase p)
	{
		s_Slams++;
		AZ_VampEvidence.Begin("sarcophagus_slam", null, p, false);
		float h = p.GetHealth("", "Health");
		p.AddHealth("", "Health", -Math.Max(0, Math.Min(ARM_DMG, h - 20)));
		p.AddHealth("RightArm", "Health", -40);
		if (p.GetBleedingManagerServer())
			p.GetBleedingManagerServer().AttemptAddBleedingSourceBySelection("RightForeArmRoll");
		AZ_VampEvidence.End("sarcophagus_slam", null, p);
		p.m_AZLastHitAt = GetGame().GetTime();
		if (s_Coffin)
			s_Coffin.Slam();
		if (p.GetIdentity())
			p.Message("The lid crashes down on your arm. You can't pull free.", "colorImportant");
		Print("[AZ_Vamp] false sarcophagus slammed on " + AZ_VampState.Uid(p) + ": health " + h + " -> " + p.GetHealth("", "Health"));
		s_PinWho = p;
		s_PinAt = p.GetPosition();
		s_PinUntil = GetGame().GetTime() + ROOT_MS;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Pin, PIN_MS, false);
		AZ_VampCryptLurkers.OnTrapFail(p, "sarcophagus");
	}



	static PlayerBase s_PinWho;
	static vector s_PinAt;
	static int s_PinUntil;
	static int s_Pulls;

	static void Pin()
	{
		PlayerBase p = s_PinWho;
		if (!p || !p.IsAlive() || GetGame().GetTime() >= s_PinUntil)
		{
			s_PinWho = null;
			return;
		}
		if (vector.Distance(p.GetPosition(), s_PinAt) > 0.15)
		{
			AZ_VampSnap.To(p, s_PinAt, p.GetDirection());
			s_Pulls++;
		}
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Pin, PIN_MS, false);
	}




	static ref array<PlayerBase> s_Probe;

	static void Probe()
	{
		if (!s_Coffin)
		{
			Print("[AZ_Vamp] sarcophagus probe: FAIL not built");
			return;
		}
		s_ReadyAt = 0;
		s_Next.Clear();
		s_Probe = new array<PlayerBase>;
		vector at = AZ_VampCrypt.Hall(SARC_F + 1.3, SARC_R - 0.35, SARC_Y + 0.05);
		PlayerBase vamp = PlayerBase.Cast(GetGame().CreateObject(AZ_VampDress.Male(11), at, false, true));
		PlayerBase human = PlayerBase.Cast(GetGame().CreateObject(AZ_VampDress.Male(12), at, false, true));
		if (!vamp || !human)
		{
			Print("[AZ_Vamp] sarcophagus probe: FAIL spawn");
			return;
		}
		s_Probe.Insert(vamp);
		s_Probe.Insert(human);
		AZ_VampDress.Survivor(human, 12);
		AZ_VampState.SetVampire(vamp, true);
		AZ_VampBind.Bind(vamp);
		bool ok = !CanReach(vamp, s_Coffin) && CanReach(human, s_Coffin);
		Print("[AZ_Vamp] sarcophagus probe: can reach in, vampire " + CanReach(vamp, s_Coffin) + " human " + CanReach(human, s_Coffin) + " " + Verdict(ok) + " (expect human only)");
		s_ProbeForce = true;
		s_ProbeArmed = true;
		float hb = human.GetHealth("", "Health");
		vector pinned = human.GetPosition();
		Reach(human);
		human.SetPosition(pinned + "1.0 0 0");
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ProbeStep, 1000, false, 1, human, hb, pinned);
	}

	static string Verdict(bool ok)
	{
		if (ok)
			return "PASS";
		return "FAIL";
	}

	static void ProbeStep(int step, PlayerBase human, float hb, vector pinned)
	{
		bool ok;
		switch (step)
		{
			case 1:
				ok = s_Slams == 1 && human.GetHealth("", "Health") < hb && s_Pulls > 0 && vector.Distance(human.GetPosition(), pinned) < 0.5;
				Print("[AZ_Vamp] sarcophagus probe: armed, human reached in -> slams " + s_Slams + " health " + hb + " -> " + human.GetHealth("", "Health") + " pulled back to the coffin " + (vector.Distance(human.GetPosition(), pinned) < 0.3) + " (pulls " + s_Pulls + ", at " + human.GetPosition() + " pin " + pinned + ") " + Verdict(ok) + " (expect slam, damage, held)");
				Reach(human);
				ok = s_Slams == 1 && s_Loots == 0;
				Print("[AZ_Vamp] sarcophagus probe: again at once -> slams " + s_Slams + " loots " + s_Loots + " " + Verdict(ok) + " (expect resting)");
				s_ReadyAt = 0;
				s_Next.Clear();
				s_ProbeArmed = false;
				Reach(human);
				ok = s_Loots == 1;
				Print("[AZ_Vamp] sarcophagus probe: unarmed -> loots " + s_Loots + " in hands " + human.GetHumanInventory().GetEntityInHands() + " " + Verdict(ok) + " (expect loot)");
				s_ReadyAt = 0;
				s_Next.Clear();
				s_ProbeArmed = true;
				AZ_VampState.Cfg().cryptTraps = false;
				Reach(human);
				ok = s_Slams == 1 && s_Loots == 2;
				Print("[AZ_Vamp] sarcophagus probe: kill switch, armed -> slams " + s_Slams + " loots " + s_Loots + " " + Verdict(ok) + " (expect loot, no slam)");
				AZ_VampState.Cfg().cryptTraps = true;
				s_ProbeForce = false;
				s_ReadyAt = 0;
				s_Next.Clear();
				foreach (PlayerBase d : s_Probe)
				{
					if (d)
						GetGame().ObjectDelete(d);
				}
				s_Probe = null;
				Print("[AZ_Vamp] sarcophagus probe: done");
				return;
		}
	}
}


class AZ_VampCoffin_False extends House
{
	protected int m_Slams;
	protected int m_SlamsLocal = -1;

	void AZ_VampCoffin_False()
	{
		RegisterNetSyncVariableInt("m_Slams", 0, 255);
	}

	void Slam()
	{
		if (!GetGame().IsServer())
			return;
		m_Slams = (m_Slams + 1) % 256;
		SetSynchDirty();
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		int was = m_SlamsLocal;
		m_SlamsLocal = m_Slams;
		if (was < 0 || was == m_Slams || !GetGame().IsClient())
			return;
		SEffectManager.PlaySound("beartrapClose_SoundSet", GetPosition() + "0 0.6 0");
		Particle.PlayInWorld(ParticleList.DUST_GROUND, GetPosition() + "0 0.4 0");
	}
}

class ActionAZReachCoffinCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(2);
	}
}

class ActionAZReachCoffin : ActionContinuousBase
{
	void ActionAZReachCoffin()
	{
		m_CallbackClass = ActionAZReachCoffinCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "Reach into the coffin";
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
		return target && AZ_VampCryptSarcophagus.CanReach(player, target.GetObject());
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		AZ_VampCryptSarcophagus.Reach(action_data.m_Player);
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionAZReachCoffin);
	}
}

modded class PlayerBase
{
	override void SetActions(out TInputActionMap InputActionMap)
	{
		super.SetActions(InputActionMap);
		AddAction(ActionAZReachCoffin, InputActionMap);
	}
}
