






class AZ_VampTurn
{
	static const float CURE_BLOOD = 250;
	static const int COUGH_EVERY_MS = 40000;

	static void StartTurning(PlayerBase victim, float leftSec = -1, string source = "player_bite")
	{
		if (!victim || victim.m_AZVampTurning)
			return;
		float total = AZ_VampState.Cfg().sicknessMin * 60;
		bool restored = leftSec >= 0;
		if (leftSec < 0)
			leftSec = total;
		int now = GetGame().GetTime();
		victim.m_AZVampTurning = true;
		victim.m_AZInfectionSource = source;
		victim.m_AZVampTurnReady = false;
		victim.m_AZVampSickEnd = now + leftSec * 1000;
		victim.m_AZVampSickStart = victim.m_AZVampSickEnd - total * 1000;
		victim.m_AZVampSickStage = 0;
		victim.m_AZVampCoughAt = now + COUGH_EVERY_MS;
		victim.SetSynchDirty();
		AZ_VampState.SetSickLeft(victim, leftSec);
		AZ_VampRecord record = AZ_VampState.Find(AZ_VampState.Uid(victim));
		if (record)
		{
			record.sickSource = source;
			AZ_VampState.Save();
		}
		string action = "INFECTION_START";
		if (restored)
			action = "INFECTION_RESTORED";
		AZ_VampAudit.Life(victim, action, source);
		AZ_VampAudit.Record("life", "SICKNESS_TIMER", "source=" + source + " remainingSec=" + leftSec);
	}


	static void SickTick(PlayerBase p, int now, bool saveNow)
	{
		if (!p.m_AZVampTurning || p.m_AZVampTurnReady || !p.IsAlive())
			return;
		float total = p.m_AZVampSickEnd - p.m_AZVampSickStart;
		float frac = (now - p.m_AZVampSickStart) / Math.Max(1, total);
		int stage = 1;
		if (frac >= 0.33)
			stage = 2;
		if (now >= p.m_AZVampSickEnd)
			stage = 3;
		if (stage != p.m_AZVampSickStage)
		{
			p.m_AZVampSickStage = stage;
			if (stage == 1)
				p.Message("Two small wounds on your neck burn. Holy water or a blood transfusion will wash it out.", "colorImportant");
			else if (stage == 2)
			{
				p.Message("Fever. Your thirst feels wrong.", "colorImportant");
				p.GetSymptomManager().QueueUpSecondarySymptom(SymptomIDs.SYMPTOM_FEVERBLUR);
			}
			else
			{
				p.Message("Your heart slows... and stops.", "colorImportant");
				p.m_AZVampTurnReady = true;
				AZ_VampEvidence.Begin("turning_ko", null, p, false);
				p.SetHealth("", "Shock", 0);
				AZ_VampEvidence.End("turning_ko", null, p);

			}
			saveNow = true;
		}
		if (stage == 2 && now >= p.m_AZVampCoughAt)
		{
			p.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_COUGH);
			p.m_AZVampCoughAt = now + COUGH_EVERY_MS + Math.RandomInt(0, 20000);
		}
		if (saveNow)
			AZ_VampState.SetSickLeft(p, Math.Max(1, (p.m_AZVampSickEnd - now) / 1000.0));
	}

	static void ClearSick(PlayerBase p)
	{
		if (p.m_AZVampTurning)
			AZ_VampAudit.Life(p, "SICKNESS_CLEARED", p.m_AZInfectionSource);
		p.m_AZVampTurning = false;
		p.m_AZVampTurnReady = false;
		p.m_AZVampSickStage = 0;
		p.SetSynchDirty();
		if (p.GetSymptomManager())
			p.GetSymptomManager().RemoveSecondarySymptom(SymptomIDs.SYMPTOM_FEVERBLUR);
		AZ_VampState.SetSickLeft(p, 0);
	}

	static void Turn(PlayerBase p)
	{
		string source = p.m_AZInfectionSource;
		ClearSick(p);
		AZ_VampState.SetVampire(p, true);
		AZ_VampState.SetImmune(p);
		AZ_VampReveal.StarveReset(p);
		p.m_AZStarveBornAt = GetGame().GetTime();
		AZ_VampBind.Bind(p);
		AZ_VampLEHS.Strip(p);
		if (p.IsRestrained())
		{

			EntityAI bonds = p.GetHumanInventory().GetEntityInHands();
			if (bonds)
				GetGame().ObjectDelete(bonds);
			p.SetRestrained(false);
			p.OnItemInHandsChanged();
			p.Message("You snap your bonds.", "colorImportant");
		}
		p.Message("You wake up hungry. The sun is your enemy now.", "colorImportant");
		Teach(p);
		array<string> walks = {"A vampire walks the island tonight.", "Somewhere on the island, a human has turned. A vampire walks.", "Another has turned. A vampire walks the island.", "The night claims another. A vampire walks."};
		Announce(walks.GetRandomElement());
		AZ_VampProgress.OnTurned();
		AZ_VampAudit.Life(p, "TURN_COMPLETE", source);
	}



	static void Teach(PlayerBase p)
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(TeachLine, 15000, false, p, "Your heart has stopped. Hunger shows as red eyes and fangs; feed to pass for human.");
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(TeachLine, 60000, false, p, "To feed, come close with empty hands and hold interact on someone. Press Fangs to let go.");
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(TeachLine, 120000, false, p, "The sun will burn you to ash. Before dawn, be under a roof.");
	}

	static void TeachLine(PlayerBase p, string text)
	{
		if (p && p.IsAlive() && p.m_AZVampire)
			p.Message(text, "colorImportant");
	}

	static void Cure(PlayerBase p)
	{
		string source = p.m_AZInfectionSource;
		// Cured = clean face: drop the bite mark too, or the sick face (red eyes) lingers biteMarkMin.
		p.m_AZVampBitten = false;
		ClearSick(p);
		AZ_VampState.SetImmune(p);
		p.Message("The transfusion burns the curse out of you. You will not turn.", "colorFriendly");
		AZ_VampAudit.Life(p, "CURE", source);
	}

	static void Announce(string text)
	{
		array<PlayerBase> list;
		AZ_VampState.OnlinePlayers(list);
		foreach (PlayerBase pb : list)
			pb.Message(text, "colorImportant");
	}
}

modded class PlayerBase
{
	bool m_AZVampTurning;
	bool m_AZVampTurnReady;
	int m_AZVampSickStart;
	int m_AZVampSickEnd;
	int m_AZVampSickStage;
	int m_AZVampCoughAt;
	string m_AZInfectionSource;

	override void Init()
	{
		super.Init();
		RegisterNetSyncVariableBool("m_AZVampTurning");
	}


	bool m_AZVampSickFaceSeen;
	bool m_AZVampPaleSeen;
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		bool marked = m_AZVampTurning || m_AZVampBitten;
		if (marked != m_AZVampSickFaceSeen || m_AZVampire != m_AZVampPaleSeen)
		{
			m_AZVampSickFaceSeen = marked;
			m_AZVampPaleSeen = m_AZVampire;
			AZ_VampFace.Apply(this);
		}
	}

	override void OnUnconsciousStop(int pCurrentCommandID)
	{
		super.OnUnconsciousStop(pCurrentCommandID);
		if (GetGame().IsServer() && m_AZVampTurning && m_AZVampTurnReady && IsAlive())
			AZ_VampTurn.Turn(this);
		// AZ-2523 cap wake message; lives here because m_AZVampTurning is declared in this file (AZ_VampFeed.c compiles first).
		if (GetGame().IsServer() && m_AZVampCapWakeMessage)
		{
			m_AZVampCapWakeMessage = false;
			if (IsAlive() && GetIdentity() && !m_AZVampTurning && !m_AZVampire)
				Message("Their bite takes blood, not your soul. Too many of them walk tonight; the curse finds no room.", "colorImportant");
		}
	}

	override void EEKilled(Object killer)
	{

		if (GetGame().IsServer() && m_AZVampTurning)
		{
			AZ_VampState.SetSickLeft(this, 0);
			AZ_VampState.SetVampire(this, true);
			AZ_VampAudit.Life(this, "DEATH_SICK_VAMPIRE_RECORD", m_AZInfectionSource);
		}

		else if (GetGame().IsServer() && m_AZVampire)
		{
			AZ_VampState.SetVampire(this, false);
			AZ_VampAudit.Life(this, "VAMPIRE_DEATH", m_AZInfectionSource);
		}
		m_AZVampTurning = false;
		super.EEKilled(killer);
	}
}

modded class ActionGiveBloodTarget
{
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		ActionGiveBloodData data = ActionGiveBloodData.Cast(action_data);
		PlayerBase target = PlayerBase.Cast(action_data.m_Target.GetObject());
		if (!data || !target || !target.m_AZVampTurning || !data.m_MainItem)
			return;
		float given = data.m_BloodAmount - data.m_MainItem.GetQuantity();
		if (given >= AZ_VampTurn.CURE_BLOOD)
		{
			AZ_VampTurn.Cure(target);
			AZ_VampProgress.OnCured(action_data.m_Player);
		}
	}
}
