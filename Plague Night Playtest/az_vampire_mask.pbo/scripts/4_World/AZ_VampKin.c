



class AZ_VampKin
{
	static void Tick(PlayerBase vamp, array<PlayerBase> online)
	{
		float range = AZ_VampState.Cfg().kinSenseM;
		int now = GetGame().GetTime();
		if (range <= 0 || !vamp.IsAlive() || now - vamp.m_AZKinSaidAt < 90000)
			return;
		vector eye = vamp.GetPosition() + "0 1.6 0";
		foreach (PlayerBase o : online)
		{
			if (!o || o == vamp || !o.m_AZVampire || !o.IsAlive())
				continue;
			vector oe = o.GetPosition() + "0 1.4 0";
			float d = vector.Distance(eye, oe);
			if (d > range)
				continue;
			vector to = oe - eye;
			to.Normalize();
			if (vector.Dot(vamp.GetDirection(), to) < 0.7)
				continue;
			if (!AZ_VampRepel.ClearLine(eye, oe, vamp, o))
				continue;
			vamp.m_AZKinSaidAt = now;
			vamp.Message("One of your own. Their heart is as still as yours.", "colorAction");
			return;
		}
	}
}

modded class PlayerBase
{
	int m_AZKinSaidAt = -100000;
}



modded class ActionCheckPulseTarget
{
	override void OnFinishProgressServer(ActionData action_data)
	{
		PlayerBase target = PlayerBase.Cast(action_data.m_Target.GetObject());
		if (target && target.m_AZVampire && action_data.m_Player)
		{
			action_data.m_Player.Message("No pulse. The skin under your fingers is cold, yet they are not dead.", "colorImportant");
			Print("[AZ_Vamp] pulse check found no pulse: " + AZ_VampState.Uid(action_data.m_Player) + " -> " + AZ_VampState.Uid(target));
			return;
		}
		super.OnFinishProgressServer(action_data);
	}
}
