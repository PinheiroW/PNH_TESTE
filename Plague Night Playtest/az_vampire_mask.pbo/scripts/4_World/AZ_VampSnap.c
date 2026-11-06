












class AZ_VampSnap
{
	static const int RPC_SNAP = 4702322;
	static const int RPC_ARRIVE = 4702323;
	static const int ARR_HOLD = 1;
	static const int ARR_MOVE = 2;
	static const int ARR_TRACE = 4;
	static const int RPC_LANDED = 4702324;
	static const int HOLD_MAX_MS = 15000;
	static const int GUARD_MS = 12000;
	static const int TRACE_MS = 3000;
	static const int ARM_MS = 4000;
	static const float NEAR_M = 5.0;

	static bool s_NoHold;
	static bool s_Trace;
	static bool s_ClientHolding;

	// COT is optional. Only announce trusted server moves to its movement checker.
	static void AllowMove(PlayerBase p)
	{
#ifdef JM_COT
		if (p && GetGame().IsServer())
			p.COT_GrantMove();
#endif
	}

	static void To(PlayerBase p, vector pos, vector dir)
	{
		if (!p || !GetGame().IsServer())
			return;
		AZ_VampCryptLog.Observe(p);
		AllowMove(p);
		p.SetPosition(pos);
		AZ_VampCryptLog.Observe(p);
		p.SetDirection(dir);
		if (p.GetIdentity())
			GetGame().RPCSingleParam(p, RPC_SNAP, new Param2<vector, vector>(pos, dir), true, p.GetIdentity());
	}

	static int Flags(int extra)
	{
		int f = extra;
		if (!s_NoHold)
			f = f | ARR_HOLD;
		if (s_Trace)
			f = f | ARR_TRACE;
		return f;
	}


	static void Arm(PlayerBase p, vector pos, vector dir)
	{
		if (p && GetGame().IsServer() && p.GetIdentity())
			GetGame().RPCSingleParam(p, RPC_ARRIVE, new Param4<vector, vector, int, int>(pos, dir, Flags(0), 0), true, p.GetIdentity());
	}

	static void Far(PlayerBase p, vector pos, vector dir, string cryptRoute = "crypt_box")
	{
		if (!p || !GetGame().IsServer())
			return;
		p.AZ_ServerArrivalStart(pos, dir);
		Param4<vector, vector, int, int> msg = new Param4<vector, vector, int, int>(pos, dir, Flags(ARR_MOVE), p.m_AZArrivalToken);
		if (p.GetIdentity())
			GetGame().RPCSingleParam(p, RPC_ARRIVE, msg, true, p.GetIdentity());
		AZ_VampCryptLog.Observe(p);
		AllowMove(p);
		p.SetPosition(pos);
		AZ_VampCryptLog.Observe(p, cryptRoute);
		p.SetDirection(dir);
		if (p.GetIdentity())
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Resend, 250, false, p, pos, dir, p.m_AZArrivalToken);
		if (s_Trace)
			ServerTrace(p, pos, GetGame().GetTime());
	}

	static void Resend(PlayerBase p, vector pos, vector dir, int token)
	{
		if (p && p.GetIdentity() && p.m_AZArrivalToken == token && p.m_AZArrivalPending)
			GetGame().RPCSingleParam(p, RPC_ARRIVE, new Param4<vector, vector, int, int>(pos, dir, Flags(ARR_MOVE), token), true, p.GetIdentity());
	}


	static void ServerTrace(PlayerBase p, vector at, int t0)
	{
		if (!p)
			return;
		int t = GetGame().GetTime() - t0;
		vector pos = p.GetPosition();
		float drop = at[1] - pos[1];
		float off = vector.Distance(Vector(at[0], 0, at[2]), Vector(pos[0], 0, pos[2]));
		Print("[AZ_Vamp] arrive server t " + t + " y " + pos[1] + " drop " + drop + " off " + off);
		if (t < TRACE_MS)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ServerTrace, 100, false, p, at, t0);
	}



	static bool Landing(PlayerBase p, vector at, out vector safe)
	{
		safe = at;
		vector hit, contactDir;
		int component;
		set<Object> surfaces = new set<Object>;




		if (!DayZPhysics.RaycastRV(at + Vector(0, 0.5, 0), at - Vector(0, 1.0, 0), hit, contactDir, component, surfaces, null, p, false, false, ObjIntersectGeom))
		{
			Print("[AZ_Vamp] snap: no floor ray hit at " + at + ", refused");
			return false;
		}
		// RaycastRV contactDir is intersection direction/size, not a surface normal.
		foreach (Object surface : surfaces)
		{
			if (surface && Man.Cast(surface))
				return false;
		}
		safe = hit + Vector(0, 0.05, 0);
		array<Object> exclude = new array<Object>;
		exclude.Insert(p);
		array<Object> solids = new array<Object>;
		if (!GetGame().IsBoxCollidingGeometry(safe + Vector(0, 0.95, 0), vector.Zero, Vector(0.6, 1.7, 0.6), ObjIntersectGeom, ObjIntersectGeom, exclude, solids))
			return true;
		string what = "";
		foreach (Object solid : solids)
		{
			if (solid)
				what = what + " " + solid.GetType();
		}
		Print("[AZ_Vamp] snap: landing at " + safe + " blocked by" + what);
		return false;
	}


	static bool FloorUnder(vector at, Object ignore)
	{
		float ground = GetGame().SurfaceY(at[0], at[2]);
		if (ground <= at[1] + 0.5 && ground >= at[1] - 1.5)
			return true;
		vector hit, nrm;
		int comp;
		set<Object> res = new set<Object>;
		if (!DayZPhysics.RaycastRV(at + Vector(0, 0.5, 0), at - Vector(0, 1.5, 0), hit, nrm, comp, res, null, ignore, false, false, ObjIntersectGeom))
			return false;
		return hit[1] <= at[1] + 0.1;
	}
}



modded class DayZPlayerImplementFallDamage
{
	override void HandleFallDamage(FallDamageData pData)
	{
		PlayerBase player = PlayerBase.Cast(m_Player);
		if (GetGame().IsServer() && player && GetGame().GetTime() < player.m_AZCryptFallUntil)
			return;
		super.HandleFallDamage(pData);
	}
}

modded class PlayerBase
{
	// Server-only arrival state. Tokens reject stale acknowledgements and resends.
	int m_AZArrivalToken;
	bool m_AZArrivalPending;
	bool m_AZArrivalPrepared;
	bool m_AZArrivalRefused;
	int m_AZArrivalGraceMS;
	protected int m_AZArrivalMoveAt;
	protected int m_AZArrivalGuardGen;
	protected int m_AZArrivalCorrections;
	protected vector m_AZArrivalTarget;
	protected vector m_AZArrivalDirection;
	protected int m_AZArrToken;

	void AZ_ServerArrivalStart(vector at, vector dir)
	{
		AZ_VampCrypt.Transit(this);
		if (m_AZArrivalGraceMS == 0)
		{
			if (AZ_VampCrypt.Inside(at))
				m_AZArrivalGraceMS = AZ_VampCrypt.GRACE_MS;
			else
				m_AZArrivalGraceMS = AZ_VampCrypt.EXIT_GRACE_MS;
		}
		m_AZArrivalPending = true;
		m_AZArrivalToken++;
		m_AZArrivalGuardGen++;
		m_AZArrivalCorrections = 0;
		m_AZArrivalMoveAt = GetGame().GetTime();
		m_AZArrivalTarget = at;
		m_AZArrivalDirection = dir;
		m_AZCryptFallUntil = Math.Max(m_AZCryptFallUntil, m_AZArrivalMoveAt + AZ_VampSnap.HOLD_MAX_MS + 3000);
		// Invalidate the pre-move timeout; the hard cap is measured from this move.
		m_AZCryptTransitGen++;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_ServerArrivalTimeout, AZ_VampSnap.HOLD_MAX_MS, false, m_AZArrivalGuardGen);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_ServerFallGuard, 100, false, m_AZArrivalGuardGen);
		Print("[AZ_Vamp] fall guard start " + AZ_VampState.Uid(this) + " target " + at + " token " + m_AZArrivalToken + " protected " + m_AZCryptTransit);
	}

	void AZ_ServerArrivalTimeout(int gen)
	{
		if (gen != m_AZArrivalGuardGen || !m_AZArrivalPending)
			return;
		m_AZArrivalPending = false;
		m_AZArrivalPrepared = false;
		m_AZArrivalGraceMS = 0;
		AZ_VampCrypt.TransitEnd(this);
		m_AZCryptTransitForfeit = false;
		Print("[AZ_Vamp] arrival timeout " + AZ_VampState.Uid(this) + " (15 s cap)");
	}

	void AZ_ServerFallGuard(int gen)
	{
		if (gen != m_AZArrivalGuardGen || !IsAlive())
			return;
		int elapsed = GetGame().GetTime() - m_AZArrivalMoveAt;
		if (elapsed >= AZ_VampSnap.GUARD_MS)
		{
			Print("[AZ_Vamp] fall guard end " + AZ_VampState.Uid(this) + " corrections " + m_AZArrivalCorrections);
			return;
		}
		vector pos = GetPosition();
		float drop = m_AZArrivalTarget[1] - pos[1];
		float off = vector.Distance(Vector(pos[0], 0, pos[2]), Vector(m_AZArrivalTarget[0], 0, m_AZArrivalTarget[2]));
		if (m_AZArrivalCorrections < 5 && (drop > 2.0 || (m_AZArrivalPending && off > 6.0)))
		{
			m_AZArrivalCorrections++;
			m_AZArrivalToken++;
			m_AZArrivalPending = true;
			if (m_AZCryptGraceUntil > GetGame().GetTime())
			{
				// Reholding must not consume the grace already granted by confirmation.
				// Keep its original deadline/cancellation tick; transit covers a longer hold.
				m_AZCryptTransit = true;
				SetAllowDamage(false);
			}
			AZ_VampSnap.AllowMove(this);
			SetPosition(m_AZArrivalTarget);
			SetDirection(m_AZArrivalDirection);
			AZ_VampSnap.Resend(this, m_AZArrivalTarget, m_AZArrivalDirection, m_AZArrivalToken);
			Print("[AZ_Vamp] fall guard correction " + m_AZArrivalCorrections + "/5 " + AZ_VampState.Uid(this) + " drop " + drop + " off " + off + " token " + m_AZArrivalToken);
		}
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_ServerFallGuard, 100, false, gen);
	}

	void AZ_ServerLanded(int token)
	{
		if (!m_AZArrivalPending || token != m_AZArrivalToken || GetGame().GetTime() - m_AZArrivalMoveAt >= AZ_VampSnap.HOLD_MAX_MS)
			return;
		m_AZArrivalPending = false;
		bool eligible = m_AZCryptTransit && !m_AZArrivalRefused && !m_AZCryptTransitForfeit;
		AZ_VampCrypt.TransitEnd(this);
		Print("[AZ_Vamp] arrival confirmed " + AZ_VampState.Uid(this) + " token " + token + " after " + (GetGame().GetTime() - m_AZArrivalMoveAt) + " ms");
		if (eligible && m_AZArrivalGraceMS > 0)
			AZ_VampCrypt.Grace(this, m_AZArrivalGraceMS);
		m_AZArrivalGraceMS = 0;
		m_AZArrivalPrepared = false;
		m_AZCryptTransitForfeit = false;
	}

	void AZ_ArriveConfirm()
	{
		// A new move/correction can find the floor after an older hold blacked out.
		if (!m_AZArrHolding && m_AZArrBlack)
		{
			m_AZArrBlack = false;
			AZ_CryptFadeEnd();
		}
		if (m_AZArrToken > 0)
			GetGame().RPCSingleParam(this, AZ_VampSnap.RPC_LANDED, new Param1<int>(m_AZArrToken), true);
	}

	protected bool m_AZArrActive;
	protected bool m_AZArrLanded;
	protected bool m_AZArrHolding;
	protected bool m_AZArrBlack;
	protected bool m_AZArrLogged;
	protected vector m_AZArrAt;
	protected vector m_AZArrDir;
	protected int m_AZArrFlags;
	protected int m_AZArrArmT;
	protected int m_AZArrT0;
	protected int m_AZArrFloorT;
	protected int m_AZArrFrames;
	protected float m_AZArrDrop;

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (GetGame().IsServer() && rpc_type == AZ_VampSnap.RPC_LANDED)
		{
			if (!sender || !GetIdentity() || sender.GetId() != GetIdentity().GetId() || !m_AZArrivalPending)
				return;
			Param1<int> ack = new Param1<int>(0);
			if (ctx.Read(ack))
				AZ_ServerLanded(ack.param1);
			return;
		}
		if (!GetGame().IsClient() || !IsControlledPlayer())
			return;
		if (rpc_type == AZ_VampSnap.RPC_ARRIVE)
		{
			Param4<vector, vector, int, int> a = new Param4<vector, vector, int, int>(vector.Zero, vector.Zero, 0, 0);
			if (ctx.Read(a))
				AZ_Arrive(a.param1, a.param2, a.param3, a.param4);
			return;
		}
		if (rpc_type != AZ_VampSnap.RPC_SNAP)
			return;
		Param2<vector, vector> p = new Param2<vector, vector>(vector.Zero, vector.Zero);
		if (!ctx.Read(p))
			return;
		SetPosition(p.param1);
		SetDirection(p.param2);
	}



	void AZ_Arrive(vector at, vector dir, int flags, int token)
	{
		bool move = (flags & AZ_VampSnap.ARR_MOVE) != 0;
		if (move && token < m_AZArrToken)
			return;
		if (move && token == m_AZArrToken && !m_AZArrActive)
		{
			AZ_ArriveConfirm();
			return;
		}
		if (!move && !m_AZArrActive && vector.Distance(GetPosition(), at) <= AZ_VampSnap.NEAR_M)
			return;
		if (m_AZArrActive && vector.Distance(at, m_AZArrAt) < 0.2 && (token == 0 || token == m_AZArrToken))
		{
			if (move && !m_AZArrLanded)
				AZ_ArriveLand();
			else if (move && !m_AZArrHolding)
				AZ_ArriveConfirm();
			return;
		}
		m_AZArrToken = token;
		m_AZArrActive = true;
		m_AZArrLanded = false;
		m_AZArrHolding = false;
		AZ_VampSnap.s_ClientHolding = false;
		m_AZArrLogged = false;
		m_AZArrAt = at;
		m_AZArrDir = dir;
		m_AZArrFlags = flags;
		m_AZArrArmT = GetGame().GetTime();
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(AZ_ArriveTick);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_ArriveTick, 0, true);
		if (move)
			AZ_ArriveLand();
	}

	void AZ_ArriveLand()
	{
		m_AZArrLanded = true;
		m_AZArrT0 = GetGame().GetTime();
		m_AZArrFloorT = -1;
		m_AZArrFrames = 0;
		m_AZArrDrop = 0;




		if ((m_AZArrFlags & AZ_VampSnap.ARR_HOLD) == 0)
		{
			AZ_ArriveConfirm();
			return;
		}
		if (AZ_VampSnap.FloorUnder(m_AZArrAt, this))
		{
			m_AZArrFloorT = 0;
			AZ_ArriveConfirm();
			return;
		}
		SetPosition(m_AZArrAt);
		SetDirection(m_AZArrDir);
		m_AZArrHolding = true;
		AZ_VampSnap.s_ClientHolding = true;
		if (!m_AZArrBlack)
		{
			m_AZArrBlack = true;
			PPERequesterBank.GetRequester(PPERequester_BurlapSackEffects).Start();
		}
	}

	void AZ_ArriveTick()
	{
		int now = GetGame().GetTime();
		if (!IsAlive())
		{
			AZ_ArriveStop();
			return;
		}
		if (!m_AZArrLanded)
		{
			if (vector.Distance(GetPosition(), m_AZArrAt) <= AZ_VampSnap.NEAR_M)
				AZ_ArriveLand();
			else if (now - m_AZArrArmT > AZ_VampSnap.ARM_MS)
				AZ_ArriveStop();
			return;
		}
		int t = now - m_AZArrT0;
		m_AZArrFrames++;
		vector pos = GetPosition();
		float drop = m_AZArrAt[1] - pos[1];
		if (drop > m_AZArrDrop)
			m_AZArrDrop = drop;
		bool floor = AZ_VampSnap.FloorUnder(m_AZArrAt, this);
		if (floor && m_AZArrFloorT < 0)
			m_AZArrFloorT = t;
		if ((m_AZArrFlags & AZ_VampSnap.ARR_TRACE) != 0 && t <= AZ_VampSnap.TRACE_MS)
			Print("[AZ_Vamp] arrive client t " + t + " y " + pos[1] + " drop " + drop + " floor " + floor + " hold " + m_AZArrHolding);
		if (m_AZArrHolding)
		{
			if (floor || t >= AZ_VampSnap.HOLD_MAX_MS)
			{
				m_AZArrHolding = false;
				AZ_VampSnap.s_ClientHolding = false;
				AZ_ArriveConfirm();

			}
			else
			{
				SetPosition(m_AZArrAt);
				SetDirection(m_AZArrDir);
			}
		}
		if (!m_AZArrHolding && t >= AZ_VampSnap.TRACE_MS)
			AZ_ArriveStop();
	}

	void AZ_ArriveStop()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(AZ_ArriveTick);
		if (m_AZArrLanded && !m_AZArrLogged)
		{
			m_AZArrLogged = true;
			Print("[AZ_Vamp] arrive: " + m_AZArrAt + " hold " + ((m_AZArrFlags & AZ_VampSnap.ARR_HOLD) != 0) + " floor at " + m_AZArrFloorT + " ms, worst drop " + m_AZArrDrop + " m in " + m_AZArrFrames + " frames");
		}
		else if (!m_AZArrLanded)
			Print("[AZ_Vamp] arrive: pre-arm for " + m_AZArrAt + " dropped, never arrived");
		m_AZArrActive = false;
		m_AZArrLanded = false;
		// AZ-2576: a dropped pre-arm after an earlier hold left the burlap on with nothing to stop it.
		AZ_ArriveRelease();
	}

	// Ends the arrival blackout and hold flag, whatever state the arrival ended in.
	void AZ_ArriveRelease()
	{
		m_AZArrHolding = false;
		AZ_VampSnap.s_ClientHolding = false;
		if (m_AZArrBlack)
		{
			m_AZArrBlack = false;
			AZ_CryptFadeEnd();
		}
	}

	// AZ-2576: client-only safety net. The burlap requester is shared with the crypt fade RPC and the
	// ritual/ledge blackouts (all <= 4.2 s). If it runs for 10 s with no hold, no burlap on the head
	// and no arrival in progress, nothing owns it any more (RPC_FADE stop lost to death, respawn or
	// disconnect): stop it. Also clears a stale hold flag.
	protected float m_AZBurlapIdle;

	void AZ_BurlapWatch(float pDt)
	{
		if (!m_AZArrHolding && AZ_VampSnap.s_ClientHolding)
			AZ_VampSnap.s_ClientHolding = false;
		PPERequester_BurlapSackEffects req = PPERequester_BurlapSackEffects.Cast(PPERequesterBank.GetRequester(PPERequester_BurlapSackEffects));
		if (!req || !req.IsRequesterRunning() || m_AZArrHolding || m_AZArrBlack)
		{
			m_AZBurlapIdle = 0;
			return;
		}
		EntityAI head = FindAttachmentBySlotName("Headgear");
		if (head && head.IsKindOf("BurlapSackCover"))
		{
			m_AZBurlapIdle = 0;
			return;
		}
		m_AZBurlapIdle += pDt;
		if (m_AZBurlapIdle < 10)
			return;
		m_AZBurlapIdle = 0;
		req.Stop();
		Print("[AZ_Vamp] burlap safety net: stopped a stray blindfold effect");
	}

	override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);
		if (GetGame().IsClient() && IsControlledPlayer())
			AZ_BurlapWatch(pDt);
	}

	override void EEKilled(Object killer)
	{
		if (GetGame().IsClient() && IsControlledPlayer())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(AZ_ArriveTick);
			m_AZArrActive = false;
			m_AZArrLanded = false;
			AZ_ArriveRelease();
			PPERequester_BurlapSackEffects req = PPERequester_BurlapSackEffects.Cast(PPERequesterBank.GetRequester(PPERequester_BurlapSackEffects));
			if (req && req.IsRequesterRunning())
				req.Stop();
		}
		super.EEKilled(killer);
	}
}
