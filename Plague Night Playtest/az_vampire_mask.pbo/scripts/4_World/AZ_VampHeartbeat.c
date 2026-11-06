



// AZ-2470: relative gain on owning-client hunting pulses only. Hearing and
// cadence remain unchanged. Gain is engine amplitude, not perceived loudness.
class AZ_VampHeartbeatVolume
{
	static const int RPC_CONFIG = 4703010;

	static float MinGain()
	{
		return Math.Clamp(AZ_VampState.Cfg().heartbeatMinGain, 0, MaxGain());
	}

	static float MaxGain()
	{
		return Math.Clamp(AZ_VampState.Cfg().heartbeatMaxGain, 0, 1);
	}

	static void SendConfigAll()
	{
		if (!GetGame().IsServer()) return;
		array<PlayerBase> online;
		AZ_VampState.OnlinePlayers(online);
		foreach (PlayerBase player : online)
		{
			if (player) player.AZ_HbSendConfig();
		}
	}
	static const float TRANSITION_SEC = 0.5;

	static float TargetGain(int hunger)
	{
		float depth = Math.Clamp((hunger - 1) / 9.0, 0, 1);
		return MinGain() + (MaxGain() - MinGain()) * depth;
	}

	static float StepGain(float currentGain, float targetGain, float dt)
	{
		targetGain = Math.Clamp(targetGain, MinGain(), MaxGain());
		if (currentGain < 0) return targetGain;
		currentGain = Math.Clamp(currentGain, MinGain(), MaxGain());
		float step = (MaxGain() - MinGain()) * Math.Max(0, dt) / TRANSITION_SEC;
		return currentGain + Math.Clamp(targetGain - currentGain, -step, step);
	}

	static void ApplyGain(EffectSound sound, float gain)
	{
		if (!sound) return;
		gain = Math.Clamp(gain, MinGain(), MaxGain());
		// The startup/fade frame reapplies max: setting current volume alone loses.
		sound.SetSoundMaxVolume(gain);
		sound.SetSoundVolume(gain);
	}

	static EffectSound PlayOnHuman(PlayerBase target, float gain)
	{
		if (!target) return null;
		// Same parenting, 3D set and zero local offset as PlaySoundOnObject;
		// create first so the initial pulse has its gain before playback starts.
		EffectSound sound = SEffectManager.CreateSound("AZ_VampHeartbeat_SoundSet", target.GetPosition());
		if (!sound) return null;
		sound.SetParent(target);
		sound.SetLocalPosition(vector.Zero);
		sound.SetAutodestroy(true);
		ApplyGain(sound, gain);
		if (!sound.SoundPlay())
		{
			Stop(sound);
			return null;
		}
		// SoundPlay has now created the native wave; apply to that instance too.
		ApplyGain(sound, gain);
		return sound;
	}

	static void Stop(EffectSound sound)
	{
		if (!sound) return;
		// Zero pending as well as playing waves, then unregister only our effect.
		sound.SetSoundMaxVolume(0);
		sound.SetSoundVolume(0);
		SEffectManager.DestroyEffect(sound);
	}
}

modded class PlayerBase
{


	void AZ_HbSendConfig()
	{
		if (!GetGame().IsServer() || !GetIdentity()) return;
		GetGame().RPCSingleParam(this, AZ_VampHeartbeatVolume.RPC_CONFIG, new Param2<float, float>(AZ_VampHeartbeatVolume.MinGain(), AZ_VampHeartbeatVolume.MaxGain()), true, GetIdentity());
		Print("[AZ_VampHeartbeatConfig] SEND min=" + AZ_VampHeartbeatVolume.MinGain() + " max=" + AZ_VampHeartbeatVolume.MaxGain() + " upMs=" + GetGame().GetTime());
	}

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (rpc_type != AZ_VampHeartbeatVolume.RPC_CONFIG || GetGame().IsServer()) return;
		PlayerBase owner = PlayerBase.Cast(GetGame().GetPlayer());
		if (this != owner) return;
		Param2<float, float> gains;
		if (!ctx.Read(gains)) return;
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		cfg.heartbeatMaxGain = Math.Clamp(gains.param2, 0, 1);
		cfg.heartbeatMinGain = Math.Clamp(gains.param1, 0, cfg.heartbeatMaxGain);
		Print("[AZ_VampHeartbeatConfig] RECEIVED min=" + cfg.heartbeatMinGain + " max=" + cfg.heartbeatMaxGain + " upMs=" + GetGame().GetTime());
	}

	protected bool m_AZMoonReceiptKnown;
	protected bool m_AZMoonReceiptState;
	protected int m_AZMoonReceiptProgress;
	protected bool m_AZMoonReceiptEligible;
	protected int m_AZMoonReceiptRevision;
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		PlayerBase localPlayer = PlayerBase.Cast(GetGame().GetPlayer());
		if (GetGame().IsDedicatedServer() || this != localPlayer)
			return;
		if (!m_AZMoonReceiptKnown || m_AZMoonReceiptState != m_AZBloodMoon || m_AZMoonReceiptProgress != m_AZMoonProgress || m_AZMoonReceiptEligible != m_AZMoonEligible || m_AZMoonReceiptRevision != m_AZMoonRevision)
			Print("[AZ_VampMoonClient] RECEIVED active=" + m_AZBloodMoon + " progress=" + m_AZMoonProgress + " eligible=" + m_AZMoonEligible + " revision=" + m_AZMoonRevision + " upMs=" + GetGame().GetTime());
		m_AZMoonReceiptKnown = true;
		m_AZMoonReceiptState = m_AZBloodMoon;
		m_AZMoonReceiptProgress = m_AZMoonProgress;
		m_AZMoonReceiptEligible = m_AZMoonEligible;
		m_AZMoonReceiptRevision = m_AZMoonRevision;
	}

	static const float AZ_HB_MIN_RANGE = 30;
	static const float AZ_HB_MAX_RANGE = 70;
	static const float AZ_HB_FED_RANGE = 20;
	float m_AZHbCheck;
	float m_AZHbNext;
	PlayerBase m_AZHbTarget;
	int m_AZHbLogAt;
	int m_AZHbScanAt;
	ref array<PlayerBase> m_AZHbBodies = new array<PlayerBase>;
	PlayerBase m_AZHbLogged;
	static const int AZ_HB_MAX_PULSES = 8;
	static const int AZ_HB_PENDING_MS = 3000;
	protected float m_AZHbGain = -1;
	protected ref array<EffectSound> m_AZHbPulses = new array<EffectSound>;
	protected ref array<int> m_AZHbPulseAt = new array<int>;
	int m_AZHearStep;
	int m_AZCalmStep;
	int m_AZMoonProgress;
	bool m_AZMoonEligible;
	int m_AZMoonRevision;
	bool m_AZBloodMoon;
	bool m_AZHalloween;
	bool m_AZEventOn;
	int m_AZVampHunger;
	int m_AZHbRangeM;


	override void Init()
	{
		super.Init();
		RegisterNetSyncVariableInt("m_AZVampHunger", 0, 10);
		RegisterNetSyncVariableInt("m_AZHbRangeM", 0, 1000);
		RegisterNetSyncVariableInt("m_AZHearStep", 0, 2);
		RegisterNetSyncVariableInt("m_AZCalmStep", 0, 2);
		RegisterNetSyncVariableBool("m_AZBloodMoon");
		RegisterNetSyncVariableInt("m_AZMoonProgress", 0, 1000);
		RegisterNetSyncVariableBool("m_AZMoonEligible");
		RegisterNetSyncVariableInt("m_AZMoonRevision", 0, 2147483647);
		RegisterNetSyncVariableBool("m_AZHalloween");
		RegisterNetSyncVariableBool("m_AZEventOn");
	}

	override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);
		if (!GetGame().IsClient())
			return;
		if (!IsControlledPlayer())
		{
			if (m_AZHbPulses.Count() > 0) AZ_HbStopPulses();
			m_AZHbGain = -1;
			return;
		}
		m_AZHbGain = AZ_VampHeartbeatVolume.StepGain(m_AZHbGain, AZ_VampHeartbeatVolume.TargetGain(m_AZVampHunger), pDt);
		m_AZHbCheck -= pDt;
		m_AZHbNext -= pDt;
		if (m_AZHbCheck <= 0)
		{
			m_AZHbCheck = 0.5;
			m_AZHbTarget = AZ_HbFindTarget();
		}
		AZ_HbUpdatePulses();
		if (!m_AZHbTarget || m_AZHbNext > 0)
			return;
		float speed = GetVelocity(m_AZHbTarget).Length();
		m_AZHbNext = 1.05;
		if (speed > 4.5)
			m_AZHbNext = 0.5;
		else if (speed > 1.5)
			m_AZHbNext = 0.75;
		EffectSound hb = AZ_VampHeartbeatVolume.PlayOnHuman(m_AZHbTarget, m_AZHbGain);
		if (hb) AZ_HbRememberPulse(hb);
		if (m_AZHbTarget != m_AZHbLogged)
		{
			m_AZHbLogged = m_AZHbTarget;
			Print("[AZ_Vamp] heartbeat beat on " + m_AZHbTarget.GetType() + " at " + Math.Round(vector.Distance(m_AZHbTarget.GetPosition(), GetPosition())) + " m, sound " + (hb != null));
		}
	}

	void AZ_HbRememberPulse(EffectSound sound)
	{
		while (m_AZHbPulses.Count() >= AZ_HB_MAX_PULSES)
		{
			AZ_VampHeartbeatVolume.Stop(m_AZHbPulses[0]);
			m_AZHbPulses.Remove(0);
			m_AZHbPulseAt.Remove(0);
		}
		m_AZHbPulses.Insert(sound);
		m_AZHbPulseAt.Insert(GetGame().GetTime());
	}

	void AZ_HbStopPulses()
	{
		foreach (EffectSound sound : m_AZHbPulses) AZ_VampHeartbeatVolume.Stop(sound);
		m_AZHbPulses.Clear();
		m_AZHbPulseAt.Clear();
	}

	void AZ_HbUpdatePulses()
	{
		if (!m_AZVampire || !IsAlive() || IsUnconscious())
		{
			AZ_HbStopPulses();
			m_AZHbTarget = null;
			m_AZHbGain = -1;
			return;
		}
		if (!m_AZHbTarget)
		{
			AZ_HbStopPulses();
			return;
		}
		int nowMs = GetGame().GetTime();
		for (int i = m_AZHbPulses.Count() - 1; i >= 0; i--)
		{
			EffectSound sound = m_AZHbPulses[i];
			// IsSoundPlaying=false also means not started: allow native load/delay.
			if (!sound || (!sound.IsSoundPlaying() && nowMs - m_AZHbPulseAt[i] > AZ_HB_PENDING_MS))
			{
				AZ_VampHeartbeatVolume.Stop(sound);
				m_AZHbPulses.Remove(i);
				m_AZHbPulseAt.Remove(i);
				continue;
			}
			AZ_VampHeartbeatVolume.ApplyGain(sound, m_AZHbGain);
		}
	}

	PlayerBase AZ_HbFindTarget()
	{
		if (!m_AZVampire || !IsAlive() || IsUnconscious())
			return null;


		float range = AZ_HB_FED_RANGE;
		if (m_AZVampHunger > 0)
		{
			float h = (m_AZVampHunger - 1) / 9.0;
			range = AZ_HB_MIN_RANGE + (AZ_HB_MAX_RANGE - AZ_HB_MIN_RANGE) * h;
		}
		if (m_AZHbRangeM > 0)
			range = m_AZHbRangeM;
		int step = m_AZHearStep;
		if (m_AZBloodMoon)
			step = 2;
		range *= 1.0 + 0.25 * step;



		if (GetGame().GetTime() >= m_AZHbScanAt)
		{
			m_AZHbScanAt = GetGame().GetTime() + 2000;
			m_AZHbBodies.Clear();
			array<Object> objs = new array<Object>;
			array<CargoBase> proxies = new array<CargoBase>;
			GetGame().GetObjectsAtPosition3D(GetPosition(), range, objs, proxies);
			foreach (Object o : objs)
			{
				PlayerBase body = PlayerBase.Cast(o);
				if (body && body != this)
					m_AZHbBodies.Insert(body);
			}
		}
		array<PlayerBase> players = m_AZHbBodies;
		PlayerBase best;
		float bestD = range;
		foreach (PlayerBase p : players)
		{
			if (!p || !p.IsAlive() || p.m_AZVampire)
				continue;
			float d = vector.Distance(p.GetPosition(), GetPosition());
			float calm = 1.0 - 0.15 * p.m_AZCalmStep;
			if (d < range * calm && d < bestD)
			{
				bestD = d;
				best = p;
			}
		}

		if (GetGame().GetTime() >= m_AZHbLogAt)
		{
			m_AZHbLogAt = GetGame().GetTime() + 120000;
			string tgt = "none";
			if (best)
				tgt = best.GetType() + " at " + Math.Round(bestD) + " m";
			Print("[AZ_Vamp] heartbeat hunger " + m_AZVampHunger + " range " + Math.Round(range) + " players " + players.Count() + " target " + tgt);
		}
		return best;
	}
}
