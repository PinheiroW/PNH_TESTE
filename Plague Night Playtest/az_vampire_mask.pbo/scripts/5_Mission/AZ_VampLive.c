

class AZ_VampLivePlayer
{
	string name;
	bool vampire;
	bool turning;
	bool alive;
}

class AZ_VampLive
{
	string time;
	int uptimeSec;
	int online;
	int vampires;
	int humans;
	int cap;
	bool turnAllowed;
	int turning;
	int pendingRitual;
	bool eventActive;
	bool bloodMoon;
	int ritualsTonight;
	ref array<ref AZ_VampLivePlayer> players;

	static const string FILE = "$profile:AZ_Vampire\\live.json";

	static void Write()
	{
		if (!GetGame() || !GetGame().IsServer() || !GetGame().GetWorld() || !AZ_VampState.s_Data)
			return;
		if (!FileExist(AZ_VampState.DIR))
			MakeDirectory(AZ_VampState.DIR);

		AZ_VampLive d = new AZ_VampLive;
		d.players = new array<ref AZ_VampLivePlayer>;

		int y, mo, dd, h, mi;
		GetGame().GetWorld().GetDate(y, mo, dd, h, mi);
		string hs = h.ToString();
		if (h < 10)
			hs = "0" + hs;
		string ms = mi.ToString();
		if (mi < 10)
			ms = "0" + ms;
		d.time = hs + ":" + ms;
		d.uptimeSec = GetGame().GetTickTime();

		array<Man> men = new array<Man>;
		GetGame().GetPlayers(men);
		foreach (Man m : men)
		{
			PlayerBase pb = PlayerBase.Cast(m);
			if (!pb)
				continue;
			AZ_VampLivePlayer lp = new AZ_VampLivePlayer;
			if (pb.GetIdentity())
				lp.name = pb.GetIdentity().GetName();
			lp.alive = pb.IsAlive();
			lp.vampire = AZ_VampState.IsVampire(pb);
			lp.turning = pb.m_AZVampTurning;
			d.players.Insert(lp);
			d.online++;
			if (lp.turning)
				d.turning++;
			if (lp.alive)
			{
				if (lp.vampire)
					d.vampires++;
				else
					d.humans++;
			}
		}

		d.cap = AZ_VampState.CapNow();
		d.turnAllowed = AZ_VampState.CapAllowsTurn();
		if (AZ_VampAltar.s_Pending)
			d.pendingRitual = AZ_VampAltar.s_Pending.Count();
		d.eventActive = AZ_VampEvent.Active();
		d.bloodMoon = AZ_VampProgress.s_BloodMoon;
		d.ritualsTonight = AZ_VampRitual.s_AnswersTonight;

		JsonFileLoader<AZ_VampLive>.JsonSaveFile(FILE, d);
	}
}

modded class MissionServer
{
	protected float m_AZLiveTime;
	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		m_AZLiveTime += timeslice;
		if (m_AZLiveTime < 60)
			return;
		m_AZLiveTime = 0;
		AZ_VampLive.Write();
	}
}
