












class AZ_VampCryptA1
{
	static const bool READY = true;
	static const vector ORB = "3777 152.5 8244";
	static const float ORB_R = 1.4;


	static const float ORB_CORE_R = 3.2;

	static const int RPC_ORB_BURST = 4702296;
	static const float CRACK_X = 3754.0;
	static const float CRACK_Z = 8242.0;
	static const float CRACK_YAW = 0.0;
	static const float POOL_Y = 147.6;
	static const float POOL_HALF = 20.0;
	static float s_PoolY;




	static float PoolY()
	{
		if (s_PoolY > 0)
			return s_PoolY;
		float low = POOL_Y + 0.3;
		for (int k = 0; k < 48; k++)
		{
			float a = Math.PI2 * k / 48;
			float c = Math.Cos(a);
			float sn = Math.Sin(a);
			float edge = POOL_HALF / Math.Max(Math.AbsFloat(c), Math.AbsFloat(sn));
			float rim = -1000;
			for (float r = 2.0; r <= edge; r += 0.5)
				rim = Math.Max(rim, GetGame().SurfaceY(ORB[0] + c * r, ORB[2] + sn * r));
			low = Math.Min(low, rim);
		}
		s_PoolY = low - 0.3;
		Print("[AZ_Vamp] crypt A1: pool level " + s_PoolY + " (lowest rim inside the sheet " + low + ", cap " + POOL_Y + ")");
		return s_PoolY;
	}
	static const float AT_CRACK = 2.2;

	static vector Crack()
	{
		return Vector(CRACK_X, GetGame().SurfaceY(CRACK_X, CRACK_Z), CRACK_Z);
	}

	static Object s_Crack;
	static Object s_Shell;
	static Object s_Pool;
	static ref array<Object> s_Shore = new array<Object>;
	static const int SHORE_N = 16;

	static bool Ready()
	{
		return READY && AZ_VampCrypt.Site();
	}

	static void Build()
	{
		if (!Ready())
			return;


		array<Object> old = new array<Object>;
		GetGame().GetObjectsAtPosition(Crack(), 6, old, null);
		foreach (Object o : old)
		{
			if (o && o.IsKindOf("AZ_Crypt_A1_Crack"))
				GetGame().ObjectDelete(o);
		}
		Print("[AZ_Vamp] crypt A1: no crack (the orb is the way down)");
		Tick();
		NoWolves();
	}


	static int s_OrbState = -1;
	static bool s_TestOpen;


	static const float WOLF_FREE_M = 150;
	static void NoWolves()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(NoWolves, 10000, false);
		array<Object> near = new array<Object>;
		GetGame().GetObjectsAtPosition(ORB, WOLF_FREE_M, near, null);
		int n = 0;
		foreach (Object o : near)
		{
			AnimalBase a = AnimalBase.Cast(o);
			if (a && a.GetType().Contains("CanisLupus"))
			{
				GetGame().ObjectDelete(a);
				n++;
			}
		}
		if (n > 0)
			Print("[AZ_Vamp] crypt A1: " + n + " wolves removed near the orb");
	}

	static void Tick()
	{
		Refresh();
		NoGas();
		AZ_VampCryptA3.Refresh();
		AZ_VampCryptBlood.Tick();
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Tick, 60000, false);
	}

	static void Refresh()
	{
		int want = AZ_VampOrbPolicy.PresentationState(GetGame().GetWorld().IsNight(), AZ_VampProgress.s_BloodMoon, AZ_VampEvent.Active(), AZ_VampProgress.GoalEligible());
		if (want != s_OrbState)
		{
			if (s_Shell)
				GetGame().ObjectDelete(s_Shell);
			if (s_Pool)
				GetGame().ObjectDelete(s_Pool);
			s_Shell = null;
			s_Pool = null;
			foreach (Object sh : s_Shore)
			{
				if (sh)
					GetGame().ObjectDelete(sh);
			}
			s_Shore.Clear();
			if (want == 1)
				s_Shell = AZ_VampCrypt.PutCrypt("AZ_Crypt_Orb_Dim", ORB, 0);
			if (want == 2)
			{
				s_Shell = AZ_VampCrypt.PutCrypt("AZ_Crypt_Orb_Shell", ORB, 0);
				AZ_Crypt_Orb_Shell bloodShell = AZ_Crypt_Orb_Shell.Cast(s_Shell);
				if (bloodShell)
					bloodShell.AZActivate(s_OrbState >= 0);
				s_Pool = AZ_VampCrypt.PutCrypt("AZ_Crypt_Blood_Pool", Vector(ORB[0], PoolY(), ORB[2]), 0);
				Shore();
			}

			array<Object> strays = new array<Object>;
			GetGame().GetObjectsAtPosition(ORB, 4, strays, null);
			foreach (Object stray : strays)
			{
				if (stray && stray != s_Shell && (stray.IsKindOf("AZ_Crypt_Orb_Dim") || stray.IsKindOf("AZ_Crypt_Orb_Shell")))
				{
					Print("[AZ_Vamp] crypt A1: stray " + stray.GetType() + " removed");
					GetGame().ObjectDelete(stray);
				}
			}
			if (s_Shell && want == 2)
				s_Shell.SetScale(AZ_VampOrbPolicy.DormantScale(1));
			else if (s_Shell)
				s_Shell.SetScale(AZ_VampOrbPolicy.DormantScale(AZ_VampProgress.GoalProgress()));
			s_OrbState = want;
			if (want == 2)
				OrbScanStart();
			Print("[AZ_Vamp] crypt A1: orb state " + want + " (0 absent, 1 feeding goal closed, 2 blood moon open)");
		}
		// The scan stops itself whenever entry closes (e.g. a day/night flap); restart it
		// on every refresh while the orb is open, not only when the state changes.
		if (want == 2)
			OrbScanStart();
		AZ_Crypt_Orb_Dim dormant = AZ_Crypt_Orb_Dim.Cast(s_Shell);
		if (dormant && want == 1)
			dormant.AZSetGoalProgress(AZ_VampProgress.GoalProgress());
	}




	static void Shore()
	{
		int n = 0;
		for (int k = 0; k < SHORE_N; k++)
		{
			float a = Math.PI2 * (k + 0.5) / SHORE_N;
			vector rad = Vector(Math.Cos(a), 0, Math.Sin(a));
			float r = 2.0;
			while (r < 22.0 && GetGame().SurfaceY(ORB[0] + rad[0] * r, ORB[2] + rad[2] * r) < PoolY())
				r += 0.25;
			if (r >= 22.0)
				continue;
			r += 0.3;
			vector at = Vector(ORB[0] + rad[0] * r, 0, ORB[2] + rad[2] * r);
			vector up = GetGame().SurfaceGetNormal(at[0], at[2]);
			at[1] = GetGame().SurfaceY(at[0], at[2]);
			vector dir = rad - up * vector.Dot(rad, up);
			dir.Normalize();
			vector m[4];
			Math3D.DirectionAndUpMatrix(dir, up, m);
			vector m3[3];
			m3[0] = m[0];
			m3[1] = m[1];
			m3[2] = m[2];
			Object o = AZ_VampCrypt.PutCrypt("AZ_Crypt_Blood_Shore", at + up * 0.04, 0);
			if (!o)
				continue;
			o.SetOrientation(Math3D.MatrixToAngles(m3));
			s_Shore.Insert(o);
			n++;
		}
		Print("[AZ_Vamp] crypt A1: flood shore, " + n + " of " + SHORE_N + " bank decals");
	}


	static bool AtCrack(vector pos)
	{
		return false;
	}

	static int s_LitCheckAt;
	static bool s_Lit;
	static bool OrbLit()
	{
		if (GetGame().IsServer())
			return s_OrbState == 2 && AZ_VampOrbPolicy.EntryOpen(GetGame().GetWorld().IsNight(), AZ_VampProgress.s_BloodMoon);
		int now = GetGame().GetTime();
		if (now < s_LitCheckAt)
			return s_Lit;
		s_LitCheckAt = now + 1000;
		s_Lit = false;
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition(ORB, 3, objs, null);
		foreach (Object o : objs)
		{
			if (o && o.IsKindOf("AZ_Crypt_Orb_Shell"))
			{
				s_Lit = true;
				break;
			}
		}
		return s_Lit;
	}


	static vector ExitPoint()
	{
		vector p = Crack() + Fwd() * 2.4;
		p[1] = GetGame().SurfaceY(p[0], p[2]) + 0.1;
		return p;
	}


	// World X/Z offsets: spread around A1, with only one SW-ridge-side slot.
	// Leave the orb/pool to the east clear; the eastern slots sit north/south of it.
	static array<vector> ExitSlots(bool spread = true)
	{
		if (!spread)
			return NearExitSlots();
		array<vector> offsets = {"0 0 20", "12 0 28", "26 0 28", "30 0 -24", "12 0 -32", "-4 0 -24", "-18 0 -22", "-30 0 -4", "-26 0 18", "-14 0 30"};
		array<vector> slots = new array<vector>;
		vector crack = Crack();
		foreach (vector offset : offsets)
		{
			vector at = crack + offset;
			at[1] = GetGame().SurfaceY(at[0], at[2]) + 0.1;
			slots.Insert(at);
		}
		return slots;
	}

	static ref array<vector> s_ExitHeld;
	static ref array<int> s_ExitHeldUntil;

	static bool PickExitFrom(PlayerBase p, array<vector> slots, bool fallback, out vector at)
	{
		array<int> valid = new array<int>;
		array<vector> safeSlots = new array<vector>;
		for (int i = 0; i < slots.Count(); i++)
		{
			vector safe;
			if (!AZ_VampSnap.Landing(p, slots[i], safe))
				continue;
			bool occupied = false;
			array<Object> near = new array<Object>;
			GetGame().GetObjectsAtPosition3D(safe, 4.0, near, null);
			foreach (Object obj : near)
			{
				PlayerBase other = PlayerBase.Cast(obj);
				if (other && other != p && other.IsAlive() && vector.Distance(other.GetPosition(), safe) < 3.0)
					occupied = true;
			}
			foreach (vector held : s_ExitHeld)
			{
				if (vector.Distance(held, safe) < 3.0)
					occupied = true;
			}
			if (occupied)
				continue;
			valid.Insert(i);
			safeSlots.Insert(safe);
		}
		if (valid.Count() == 0)
			return false;
		int pick = Math.RandomInt(0, valid.Count());
		at = safeSlots[pick];
		s_ExitHeld.Insert(at);
		s_ExitHeldUntil.Insert(GetGame().GetTime() + 3000);
		Print("[AZ_Vamp] crypt A1 exit pick " + (valid[pick] + 1) + "/" + slots.Count() + " at " + at + " fallback " + fallback);
		return true;
	}

	static bool PickExit(PlayerBase p, out vector at, out vector dir)
	{
		if (!p || !GetGame().IsServer())
			return false;
		if (!s_ExitHeld)
		{
			s_ExitHeld = new array<vector>;
			s_ExitHeldUntil = new array<int>;
		}
		int now = GetGame().GetTime();
		for (int h = s_ExitHeld.Count() - 1; h >= 0; h--)
		{
			if (s_ExitHeldUntil[h] <= now)
			{
				s_ExitHeld.Remove(h);
				s_ExitHeldUntil.Remove(h);
			}
		}
		array<vector> ring = ExitSlots();
		if (!PickExitFrom(p, ring, false, at))
		{
			array<vector> nearCrack = NearExitSlots();
			if (!PickExitFrom(p, nearCrack, true, at))
			{
				Print("[AZ_Vamp] crypt A1 exit blocked: no clear ring or near-crack slot");
				return false;
			}
		}
		dir = at - Crack();
		dir[1] = 0;
		dir.Normalize();
		vector side = Vector(dir[2], 0, -dir[0]);
		dir = dir + side * Math.RandomFloat(-0.35, 0.35);
		dir.Normalize();
		return true;
	}

	// Original six positions, retained for fallback and unrelated exit routes.
	static array<vector> NearExitSlots()
	{
		array<vector> r = new array<vector>;
		vector f = Fwd();
		vector side = Vector(f[2], 0, -f[0]);
		vector base = Crack() + f * 2.4;
		r.Insert(base);
		r.Insert(base - side * 1.2);
		r.Insert(base + side * 1.2);
		r.Insert(base + f * 1.2);
		r.Insert(base + f * 1.2 - side * 1.2);
		r.Insert(base + f * 1.2 + side * 1.2);
		for (int i = 0; i < r.Count(); i++)
		{
			vector v = r[i];
			v[1] = GetGame().SurfaceY(v[0], v[2]) + 0.1;
			r[i] = v;
		}
		return r;
	}

	static vector Fwd()
	{
		float a = CRACK_YAW * Math.DEG2RAD;
		return Vector(Math.Sin(a), 0, Math.Cos(a));
	}





	static const float ORB_TAKE_M = 3.0;
	static const int ORB_SCAN_MS = 150;

	static const float ORB_FALL_FLAT = 14.0;
	static bool s_OrbScan;
	static int s_OrbScans;





	static const vector A1_GAS_AT = "3777 145 8242";

	static void NoGas()
	{
		if (!AZ_VampEvent.Active() && !AZ_VampProgress.s_BloodMoon)
			return;
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition(A1_GAS_AT, 5, objs, null);
		foreach (Object o : objs)
		{
			if (o && o.IsKindOf("ContaminatedArea_Athena1"))
			{
				Print("[AZ_Vamp] crypt A1: Namalsk's A1 gas removed (event night or blood moon) at " + o.GetPosition());
				GetGame().ObjectDelete(o);
			}
		}
	}

	static void OrbScanStart()
	{
		if (s_OrbScan)
			return;
		s_OrbScan = true;
		OrbScan();
	}

	static void OrbScan()
	{
		if (s_OrbState != 2 || !AZ_VampOrbPolicy.EntryOpen(GetGame().GetWorld().IsNight(), AZ_VampProgress.s_BloodMoon))
		{
			s_OrbScan = false;
			return;
		}
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(OrbScan, ORB_SCAN_MS, false);
		s_OrbScans++;
		if (s_OrbScans % 33 == 0)
			NoGas();
		array<PlayerBase> list;
		AZ_VampState.OnlinePlayers(list);
		float pool = PoolY();
		foreach (PlayerBase p : list)
		{
			if (!p)
				continue;
#ifdef NAMALSK_SURVIVAL

			if (p.IsTeleporting())
				continue;
#endif
			vector pos = p.GetPosition();
			float dx = pos[0] - ORB[0];
			float dz = pos[2] - ORB[2];
			float flat = Math.Sqrt(dx * dx + dz * dz);
			if (flat > POOL_HALF)
				continue;
			float d = vector.Distance(pos, ORB);
			bool inOrb = d < ORB_TAKE_M;
			// Jump or fall in: wading into the pool from the edge does not count.
			vector vel = GetVelocity(p);
			bool inPool = pos[1] < pool + 0.05 && pos[1] > pool - 4.0 && (p.GetCommand_Fall() != null || vel[1] < -2.0);
			bool inFall = flat < ORB_FALL_FLAT && pos[1] < ORB[1] + 1.5 && p.GetCommand_Fall() != null;
			if (!inOrb && !inPool && !inFall)
				continue;
			if (AZ_VampCrypt.JustEntered(p))
				continue;
			if (!AZ_VampCrypt.CanMove(p) || AZ_VampCrypt.Inside(pos))
			{


				if (inPool && pos[1] < pool - 0.3 && !AZ_VampCrypt.Inside(pos))
				{
					pos[1] = pool;
					AZ_VampSnap.AllowMove(p);
					p.SetPosition(pos);
				}
				continue;
			}
			Print("[AZ_Vamp] crypt A1: route=crypt (orb " + inOrb + ", pool " + inPool + ", falling " + inFall + " at y " + pos[1] + ", " + Math.Round(d) + " m)");
			OrbBurst(Vector(pos[0], pos[1] + 1, pos[2]));
			string route = "a1_fall";
			if (inOrb) route = "a1_orb";
			else if (inPool) route = "a1_pool";
			AZ_VampCrypt.EnterFrom(p, 1, route);
		}
	}


	static void OrbBurst(vector at)
	{
		array<PlayerBase> list;
		AZ_VampState.OnlinePlayers(list);
		foreach (PlayerBase p : list)
		{
			if (p && p.GetIdentity() && vector.Distance(p.GetPosition(), at) < 400)
				GetGame().RPCSingleParam(p, RPC_ORB_BURST, new Param1<vector>(at), true, p.GetIdentity());
		}
	}

	static void Descend(PlayerBase p)
	{
		if (!AtCrack(p.GetPosition()))
			return;
		AZ_VampCrypt.EnterFrom(p, 1, "a1_crack");
	}
}


class AZ_Crypt_A1_Crack extends House
{
	protected ScriptedLightBase m_AZLight;

	override void EEInit()
	{
		super.EEInit();
		if (!GetGame().IsDedicatedServer())
			m_AZLight = AZ_VampLightGate.AtPoint(AZ_VampCrackLight, this, "glow");
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		if (m_AZLight)
			m_AZLight.Destroy();
	}
}

class AZ_VampCrackLight extends PointLightBase
{
	void AZ_VampCrackLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(4.0);
		SetBrightnessTo(1.2);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetDiffuseColor(1.0, 0.08, 0.04);
		SetAmbientColor(1.0, 0.08, 0.04);
		SetFlickerAmplitude(0.35);
		SetFlickerSpeed(0.25);
		FadeIn(1.0);
	}
}





class AZ_VampOrbThing
{



	static bool Ready()
	{
		return GetGame().GetPlayer() != null;
	}

	// Custom portal effects must not instantiate the native EVR anomaly.

}








class AZ_Crypt_Orb_Shell extends House
{
	protected ScriptedLightBase m_AZLight;
	protected Particle m_AZDrip;
	protected Particle m_AZVortex;
	protected Particle m_AZHeart;
	protected EffectSound m_AZHum;
	protected float m_AZSize;
	protected float m_AZGrowth;
	protected bool m_AZGrowthKnown;
	protected int m_AZGrowthStartedAt;
	protected int m_AZTick;
	protected int m_AZHintAt;
	protected float m_AZYaw;
	static const float BEAT_S = 1.25;
	static const float LIGHT_BR = 5.0;

	void AZ_Crypt_Orb_Shell()
	{
		RegisterNetSyncVariableFloat("m_AZGrowth", 0, 1, 3);
		RegisterNetSyncVariableBool("m_AZGrowthKnown");
	}

	void AZActivate(bool grow)
	{
		if (!GetGame().IsServer())
			return;
		m_AZGrowth = 1;
		m_AZGrowthKnown = true;
		if (grow)
		{
			m_AZGrowth = 0;
			m_AZGrowthStartedAt = GetGame().GetTime();
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZGrow, 100, true);
		}
		SetSynchDirty();
	}

	void AZGrow()
	{
		m_AZGrowth = Math.Min(1.0, (GetGame().GetTime() - m_AZGrowthStartedAt) / 3000.0);
		SetSynchDirty();
		if (m_AZGrowth >= 1)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(AZGrow);
	}

	override void EEInit()
	{
		super.EEInit();
		if (!GetGame().IsDedicatedServer())
		{
			SetScale(0.02);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZSpin, 50, true);
		}
	}

	void AZSpin()
	{
		if (!m_AZGrowthKnown)
			return;
		m_AZSize = AZ_VampOrbPolicy.FullScale(m_AZGrowth);
		if (!m_AZLight)
			m_AZLight = AZ_VampLightGate.Light(AZ_VampOrbLight, GetPosition());
		m_AZTick++;
		bool ready = AZ_VampOrbThing.Ready();
		if (ready && !m_AZVortex)
		{
			m_AZVortex = Particle.PlayInWorld(ParticleList.AZ_VAMP_ORB_VORTEX, GetPosition());
		}
		if (ready && m_AZTick % 20 == 0 && GetGame().GetTime() >= m_AZHintAt)
		{
			PlayerBase nearOrb = PlayerBase.Cast(GetGame().GetPlayer());
			if (nearOrb && nearOrb.IsAlive() && !nearOrb.IsUnconscious() && vector.Distance(nearOrb.GetPosition(), GetPosition()) < 30)
			{
				m_AZHintAt = GetGame().GetTime() + 60000;
				nearOrb.Message("The blood storm pulls inward. Walk into it to pass through.", "colorAction");
			}
		}
		if (ready && !m_AZDrip)
		{
			vector under = GetPosition();
			under = AZ_VampDarkOrb.Under(under, AZ_VampCryptA1.ORB_CORE_R);
			m_AZDrip = Particle.PlayInWorld(ParticleList.AZ_VAMP_ORB_DRIP, under);
		}
		float ph = Math.ModFloat(GetGame().GetTime() * 0.001, BEAT_S) / BEAT_S;
		float b1 = (ph - 0.06) / 0.07;
		float b2 = (ph - 0.30) / 0.08;
		float beat = Math.Pow(2.718, -b1 * b1) + 0.6 * Math.Pow(2.718, -b2 * b2);
		if (m_AZLight)
			m_AZLight.SetBrightnessTo(LIGHT_BR * (0.75 + 0.35 * beat));
		m_AZYaw = m_AZYaw + 0.6;
		SetOrientation(Vector(m_AZYaw, 12, 0));

		SetScale(m_AZSize * (1 + 0.05 * beat));
		if (ready && !m_AZHum)
			m_AZHum = AZ_VampDarkOrb.Hum(this);
		if (ready && !m_AZHeart)
			m_AZHeart = AZ_VampDarkOrb.Heart(GetPosition(), AZ_VampCryptA1.ORB_CORE_R);
		AZ_VampDarkOrb.Keep(m_AZHeart, GetPosition(), m_AZSize);
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(AZSpin);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(AZGrow);
		if (m_AZLight)
			m_AZLight.Destroy();
		if (m_AZVortex)
			m_AZVortex.Stop();
		if (m_AZDrip)
			m_AZDrip.Stop();
		if (m_AZHeart)
			m_AZHeart.Stop();
		AZ_VampDarkOrb.HumStop(m_AZHum);
	}
}



class AZ_Crypt_Orb_Dim extends House
{
	protected ScriptedLightBase m_AZLight;
	protected int m_AZShellCheck;
	protected bool m_AZSuppressed;
	protected float m_AZBaseScale = 0.4;
	protected float m_AZGoalProgress;

	void AZ_Crypt_Orb_Dim()
	{
		RegisterNetSyncVariableFloat("m_AZGoalProgress", 0, 1, 3);
	}

	void AZSetGoalProgress(float progress)
	{
		if (!GetGame().IsServer() || m_AZGoalProgress == progress)
			return;
		m_AZGoalProgress = progress;
		SetSynchDirty();
	}

	override void EEInit()
	{
		super.EEInit();
		if (!GetGame().IsDedicatedServer())
		{
			m_AZLight = AZ_VampLightGate.Light(AZ_VampOrbDimLight, GetPosition());
			m_AZBaseScale = AZ_VampOrbPolicy.DormantScale(m_AZGoalProgress);

			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZSpin, 50, true);
		}
	}

	protected float m_AZYaw;
	void AZSpin()
	{

		m_AZShellCheck++;
		if (m_AZShellCheck % 20 == 0)
		{
			array<Object> near = new array<Object>;
			GetGame().GetObjectsAtPosition(GetPosition(), 4, near, null);
			bool bloodShell = false;
			bool nativeStorm = false;
			foreach (Object o : near)
			{
				if (o && o.IsKindOf("AZ_Crypt_Orb_Shell"))
					bloodShell = true;
				if (o && o.GetType() == "vfx_the_thing")
					nativeStorm = true;
			}
			bool suppress = !AZ_VampOrbPolicy.Visible(bloodShell, nativeStorm);
			if (suppress != m_AZSuppressed)
			{
				m_AZSuppressed = suppress;
				if (suppress)
				{
					SetScale(0.02);
					if (m_AZLight)
						m_AZLight.Destroy();
					m_AZLight = null;
				}
				else
				{
					SetScale(m_AZBaseScale);
					m_AZLight = AZ_VampLightGate.Light(AZ_VampOrbDimLight, GetPosition());
				}
			}
		}
		m_AZBaseScale = AZ_VampOrbPolicy.ApproachScale(m_AZBaseScale, AZ_VampOrbPolicy.DormantScale(m_AZGoalProgress), 0.05);


		float displayedProgress = Math.Clamp((m_AZBaseScale - 0.4) / 2.2, 0, 1);
		if (m_AZLight)
		{
			m_AZLight.SetRadiusTo(AZ_VampOrbPolicy.DormantRadius(displayedProgress));
			m_AZLight.SetBrightnessTo(AZ_VampOrbPolicy.DormantBrightness(displayedProgress));
		}
		m_AZYaw = m_AZYaw + 0.3;
		SetOrientation(Vector(m_AZYaw, 12, 0));
		if (m_AZSuppressed)
			SetScale(0.02);
		else
			SetScale(m_AZBaseScale);
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(AZSpin);
		if (m_AZLight)
			m_AZLight.Destroy();
	}
}

class AZ_VampOrbDimLight extends PointLightBase
{
	void AZ_VampOrbDimLight()
	{
		SetVisibleDuringDaylight(false);
		SetRadiusTo(3.0);
		SetBrightnessTo(0.3);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetDiffuseColor(1.0, 0.08, 0.04);
		SetAmbientColor(1.0, 0.08, 0.04);
		SetFlickerAmplitude(0.35);
		SetFlickerSpeed(0.2);
		FadeIn(3.0);
	}
}

class AZ_VampOrbLight extends PointLightBase
{
	void AZ_VampOrbLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(20.0);
		SetBrightnessTo(4.0);
		EnableHeatHaze(true);
		SetHeatHazeRadius(3.2);
		SetHeatHazePower(0.02);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetDiffuseColor(1.0, 0.06, 0.03);
		SetAmbientColor(1.0, 0.06, 0.03);
		SetFlickerAmplitude(0);
		FadeIn(3.0);
	}
}

class ActionAZCryptDescend : ActionInteractBase
{
	void ActionAZCryptDescend()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "Descend into the crack";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTAZAny;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return player && !player.IsInVehicle() && AZ_VampCryptA1.AtCrack(player.GetPosition());
	}

	override void OnExecuteServer(ActionData action_data)
	{
		AZ_VampCryptA1.Descend(action_data.m_Player);
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionAZCryptDescend);
	}
}

modded class PlayerBase
{
	protected float m_AZCallIn = 20;

	override void SetActions(out TInputActionMap InputActionMap)
	{
		super.SetActions(InputActionMap);
		AddAction(ActionAZCryptDescend, InputActionMap);
	}


	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (rpc_type != AZ_VampCryptA1.RPC_ORB_BURST || !GetGame().IsClient())
			return;
		Param1<vector> burst = new Param1<vector>(vector.Zero);
		if (ctx.Read(burst))
			Particle.PlayInWorld(ParticleList.AZ_VAMP_ORB_BURST, burst.param1);
	}


	override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);
		if (!GetGame().IsClient() || !IsControlledPlayer())
			return;
		m_AZCallIn -= pDt;
		if (m_AZCallIn > 0)
			return;
		m_AZCallIn = 3;
		if (!AZ_VampCryptA1.Ready() || !IsAlive() || vector.Distance(GetPosition(), AZ_VampCryptA1.Crack()) >= 20)
			return;
		if (AZ_VampCryptA1.Ready())
			return;
		SEffectManager.PlaySound("AZ_CryptCall_SoundSet", AZ_VampCryptA1.Crack());
		m_AZCallIn = Math.RandomFloatInclusive(45, 120);
	}
}
