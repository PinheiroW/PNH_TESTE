





class AZ_VampBlood
{
	static void Set(PlayerBase p, bool on)
	{
		if (!p || !GetGame().IsServer())
			return;
		p.m_AZVampBloodiedGen++;
		int fadeMs = AZ_VampState.Cfg().bloodFadeMin * 60000;
		if (on && fadeMs > 0)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(FadeOut, fadeMs, false, p, p.m_AZVampBloodiedGen);
		if (p.m_AZVampBloodied == on)
			return;
		p.m_AZVampBloodied = on;
		p.SetSynchDirty();
		AZ_VampState.SetBloodied(p, on);
		AZ_VampFace.Apply(p);
		Print("[AZ_Vamp] blood " + AZ_VampState.Uid(p) + " bloodied " + on);
	}

	static void FadeOut(PlayerBase p, int gen)
	{
		if (p && gen == p.m_AZVampBloodiedGen)
			Set(p, false);
	}


	// Run only after a vanilla wash completes on the server.
	static void Wash(PlayerBase p)
	{
		if (!p || !GetGame().IsServer())
			return;
		// Wash report (Ceremor, 3 Nov): log every finished wash so a failed one shows its state.
		Print("[AZ_Vamp] wash " + AZ_VampState.Uid(p) + " bloodied " + p.m_AZVampBloodied + " vampire " + p.m_AZVampire + " face " + p.m_AZVampFace);
		if (!p.m_AZVampBloodied)
			return;
		p.m_AZVampFedUntil = 0;
		Set(p, false);
		AZ_VampReveal.Update(p);
		p.Message("You wash the blood from your mouth.", "colorAction");
	}

	static void Restore(PlayerBase p)
	{
		if (p && AZ_VampState.GetBloodied(p))
			Set(p, true);
	}
}


modded class ActionWashHeadWettingClothesBase
{
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);
		AZ_VampBlood.Wash(action_data.m_Player);
	}
}

modded class PlayerBase
{
	bool m_AZVampBloodied;
	int m_AZVampBloodiedGen;
	bool m_AZVampBloodiedShown;

	override void Init()
	{
		super.Init();
		RegisterNetSyncVariableBool("m_AZVampBloodied");
	}

	// U10c fix (4 Nov): the blood stays until a wash or death. Death clears the saved flag only, so the fresh
	// corpse keeps its bloody mouth but the next life does not start with it (Restore on connect).
	override void EEKilled(Object killer)
	{
		if (GetGame().IsServer() && m_AZVampBloodied)
		{
			m_AZVampBloodiedGen++;
			AZ_VampState.SetBloodied(this, false);
		}
		super.EEKilled(killer);
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		if (m_AZVampBloodied != m_AZVampBloodiedShown)
		{
			m_AZVampBloodiedShown = m_AZVampBloodied;
			AZ_VampFace.Apply(this);
		}
	}
}

modded class ActionWashHandsWater
{
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);
		AZ_VampBlood.Wash(action_data.m_Player);
	}
}

modded class ActionWashHandsWell
{
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);
		AZ_VampBlood.Wash(action_data.m_Player);
	}
}

modded class ActionWashHandsItemContinuous
{
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);
		AZ_VampBlood.Wash(action_data.m_Player);
	}
}
