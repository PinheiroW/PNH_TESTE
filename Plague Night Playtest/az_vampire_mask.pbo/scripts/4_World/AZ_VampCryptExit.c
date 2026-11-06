






class AZ_Crypt_Exit_Slab extends AZ_Crypt_Slab_Door
{

	protected override void CheckClose()
	{
	}

	protected override bool PlayerNear()
	{
		return true;
	}
}

modded class AZ_VampConfig
{
	string exitLanding = "vorkuta";
}

class AZ_VampCryptExit
{



	static const float VK_YAW = 335;
	static ref array<vector> s_Vorkuta;

	static array<vector> VorkutaSlots()
	{
		if (s_Vorkuta)
			return s_Vorkuta;
		s_Vorkuta = new array<vector>;
		array<float> xz = {7204.8, 10820.0, 7206.0, 10819.6, 7207.2, 10820.0};

		array<float> back = {7204.8, 10817.6, 7206.0, 10817.2, 7207.2, 10817.6};
		foreach (float bxz : back)
			xz.Insert(bxz);
		array<float> nudge = {0, 0, 0, -1.5, 1.5, 0, -1.5, 0, 0, -3.0, 3.0, 0, -3.0, 0};
		for (int i = 0; i + 1 < xz.Count(); i += 2)
		{
			vector best = vector.Zero;
			string hit = "";
			for (int k = 0; k + 1 < nudge.Count(); k += 2)
			{
				float x = xz[i] + nudge[k];
				float z = xz[i + 1] + nudge[k + 1];
				float y = GetGame().SurfaceY(x, z);
				vector at = Vector(x, y + 0.05, z);
				string near = AZ_VampCryptLurkers.DaisBlocked(at, null);
				if (near == "")
				{
					best = at;
					break;
				}
				hit = near;
			}
			if (best == vector.Zero)
			{
				float y0 = GetGame().SurfaceY(xz[i], xz[i + 1]);
				best = Vector(xz[i], y0 + 0.05, xz[i + 1]);
				Print("[AZ_Vamp] crypt exit: Vorkuta slot " + (i / 2) + " has no clear spot (nearest " + hit + "), kept as given");
			}
			s_Vorkuta.Insert(best);
			Print("[AZ_Vamp] crypt exit: Vorkuta slot " + (i / 2) + " at " + best);
		}
		return s_Vorkuta;
	}

	static vector VorkutaFwd()
	{
		float a = VK_YAW * Math.DEG2RAD;
		return Vector(Math.Sin(a), 0, Math.Cos(a));
	}


	static bool ToVorkuta()
	{
		string where = "vorkuta";
		if (AZ_VampState.Cfg())
			where = AZ_VampState.Cfg().exitLanding;
		if (where == "a1")
			return false;
		if (where == "random")
			return Math.RandomIntInclusive(0, 1) == 1;
		return true;
	}

	static const float EXIT_F = 61.0;
	static const float EXIT_R = -6.5;
	static const float SILL = 0.55;
	static const float PANEL_T = 0.08;
	static const int OPEN_AFTER_WAKE_MS = 3000;
	static const float LEAVE_M = 2.2;
	static AZ_Crypt_Exit_Slab s_Slab;
	static AZ_Crypt_Exit_Slab s_ClientSlab;
	static bool s_ClientAtDoor;
	static int s_ClientLookAt;
	static int s_ClientLooks;


	static void Build()
	{
		float yaw = AZ_VampCrypt.HALL_YAW + 90;
		AZ_VampCrypt.PutCrypt("AZ_Crypt_Exit_Dark", AZ_VampCrypt.Nave(EXIT_F, EXIT_R, SILL), yaw);
		s_Slab = AZ_Crypt_Exit_Slab.Cast(AZ_VampCrypt.PutCrypt("AZ_Crypt_Exit_Slab", AZ_VampCrypt.Nave(EXIT_F - PANEL_T, EXIT_R, SILL), yaw));
		int night = 0;
		if (AZ_VampState.s_Data)
			night = AZ_VampState.s_Data.cryptExitNight;
		Print("[AZ_Vamp] crypt exit: slab " + (s_Slab != null) + " at hall " + AZ_VampCrypt.HallLocal(AZ_VampCrypt.Nave(EXIT_F, EXIT_R, SILL)) + " night " + night + " tonight " + NightKey());
		Restore();
	}


	static void Restore()
	{
		AZ_VampCryptRiverPortal.Restore();
		if (!s_Slab || !OpenTonight() || s_Slab.IsOpen())
			return;
		s_Slab.SetOpen(true);
		Print("[AZ_Vamp] crypt exit restored open after a restart (night " + AZ_VampState.s_Data.cryptExitNight + ")");
	}


	static int NightKey()
	{
		int y, m, d, h, mi;
		GetGame().GetWorld().GetDate(y, m, d, h, mi);
		if (h < 12)
		{
			d--;
			if (d < 1)
			{
				m--;
				if (m < 1)
				{
					m = 12;
					y--;
				}
				d = DaysIn(m, y);
			}
		}
		return y * 10000 + m * 100 + d;
	}

	static int DaysIn(int m, int y)
	{
		if (m == 2)
		{
			if (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0))
				return 29;
			return 28;
		}
		if (m == 4 || m == 6 || m == 9 || m == 11)
			return 30;
		return 31;
	}

	static bool OpenTonight()
	{
		return AZ_VampState.s_Data && AZ_VampState.s_Data.cryptExitNight != 0 && AZ_VampState.s_Data.cryptExitNight == NightKey() && GetGame().GetWorld().IsNight();
	}


	static void OpenAtWake(PlayerBase p)
	{
		if (!s_Slab)
		{
			Print("[AZ_Vamp] crypt exit: the wake came, but no slab");
			return;
		}
		bool newNight = AZ_VampState.s_Data.cryptExitNight != NightKey();
		AZ_VampState.s_Data.cryptExitNight = NightKey();
		if (newNight)
			AZ_VampCryptRiverPortal.Roll();
		AZ_VampState.Save();
		AZ_VampCryptRiverPortal.Open("the wake");






		vector hoardAt = AZ_VampCrypt.Nave(EXIT_F - 1.7, EXIT_R + 0.65, 0.05);
		AZ_VampCryptLoot.Hoard(hoardAt, AZ_VampCrypt.HALL_YAW);


		vector hoardAt2 = AZ_VampCrypt.Nave(EXIT_F - 1.7, -(EXIT_R + 0.65), 0.05);
		AZ_VampCryptLoot.Hoard(hoardAt2, AZ_VampCrypt.HALL_YAW, 1);
		if (s_Slab.IsOpen())
			return;
		s_Slab.SetOpen(true);

		if (p)
		{
			GetGame().RPCSingleParam(p, AZ_VampShadow.RPC_SHADOW, new Param1<vector>(Front(0.3) - Vector(0, 0.4, 0)), true);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Dust, 1200, false, p);
		}
		ExitTell();
		Print("[AZ_Vamp] crypt exit opened at the wake (night " + AZ_VampState.s_Data.cryptExitNight + ")");
	}




	static const int RPC_EXIT_TELL = 4702261;

	static void ExitTell()
	{
		vector door = Front(0.3);
		array<PlayerBase> list;
		AZ_VampState.OnlinePlayers(list);
		int n = 0;
		foreach (PlayerBase h : list)
		{
			if (h && h.GetIdentity() && AZ_VampCrypt.Inside(h.GetPosition()))
			{
				GetGame().RPCSingleParam(h, RPC_EXIT_TELL, new Param1<vector>(door), true, h.GetIdentity());
				n++;
			}
		}
		Print("[AZ_Vamp] crypt exit tell: the brazier by the doorway flares for " + n + " players");
	}

	static void Dust(PlayerBase p)
	{
		if (p)
			GetGame().RPCSingleParam(p, AZ_VampShadow.RPC_SHADOW, new Param1<vector>(Front(0.3) + Vector(0, 0.6, 0)), true);
	}


	static void Dawn()
	{
		bool was = AZ_VampState.s_Data.cryptExitNight != 0;
		AZ_VampState.s_Data.cryptExitNight = 0;
		if (was)
			AZ_VampState.Save();
		AZ_VampCryptRiverPortal.Shut();
		if (s_Slab && s_Slab.IsOpen())
		{
			s_Slab.SetOpen(false);
			Print("[AZ_Vamp] crypt exit ground shut at dawn");
		}
	}


	static vector Front(float dist)
	{
		return AZ_VampCrypt.Nave(EXIT_F - 0.4 - dist, EXIT_R, 0);
	}


	static AZ_Crypt_Exit_Slab Slab()
	{
		if (GetGame().IsServer())
			return s_Slab;
		if (!s_ClientSlab && GetGame().GetTime() >= s_ClientLookAt)
		{
			s_ClientLookAt = GetGame().GetTime() + 5000;
			s_ClientLooks++;
			array<Object> objs = new array<Object>;
			GetGame().GetObjectsAtPosition(AZ_VampCrypt.Nave(EXIT_F - PANEL_T, EXIT_R, SILL), 1.0, objs, null);
			foreach (Object o : objs)
			{
				AZ_Crypt_Exit_Slab s = AZ_Crypt_Exit_Slab.Cast(o);
				if (s)
					s_ClientSlab = s;
			}

			if (s_ClientSlab || s_ClientLooks == 1 || s_ClientLooks % 60 == 0)
				Print("[AZ_Vamp] crypt exit client: slab found " + (s_ClientSlab != null) + " (look " + s_ClientLooks + ")");
		}
		return s_ClientSlab;
	}

	static bool AtDoor(PlayerBase p)
	{
		if (!p || !AZ_VampCrypt.Site())
			return false;
		AZ_Crypt_Exit_Slab s = Slab();
		if (!s || !s.IsOpen())
			return false;
		vector to = Front(0.5) - p.GetPosition();
		to[1] = 0;
		if (to.Length() > LEAVE_M)
			return false;
		vector look = p.GetDirection();
		look[1] = 0;
		bool at = vector.Dot(look.Normalized(), AZ_VampCrypt.Fwd(AZ_VampCrypt.HALL_YAW)) > 0.5;
		if (at && !s_ClientAtDoor && GetGame().IsClient())
		{
			s_ClientAtDoor = true;
			Print("[AZ_Vamp] crypt exit client: the Leave prompt is up");
		}
		return at;
	}

	static void Leave(PlayerBase p)
	{
		if (!AtDoor(p) || !AZ_VampCrypt.CanMove(p))
			return;
		if (GetGame().GetTime() - p.m_AZLastHitAt < AZ_VampCryptLedge.LEAVE_HURT_MS)
		{
			p.Message("Not with blood still running. Wait a moment, then try again.", "colorAction");
			Print("[AZ_Vamp] crypt exit: " + AZ_VampState.Uid(p) + " leave refused (hurt)");
			return;
		}
		if (!AZ_VampCryptA1.Ready() && !ToVorkuta())
		{
			AZ_VampCrypt.MoveOut(p);
			Print("[AZ_Vamp] crypt exit: " + AZ_VampState.Uid(p) + " out (A1 not ready: the usual way)");
			return;
		}
		p.m_AZCryptExitGrace = true;
		if (ToVorkuta())
		{
			AZ_VampCrypt.MoveTo(p, AZ_VampCryptLedge.PickFrom(VorkutaSlots(), "vorkuta"), VorkutaFwd(), "vorkuta_exit");
			Print("[AZ_Vamp] crypt exit: " + AZ_VampState.Uid(p) + " climbed out of the old cellar at Vorkuta's edge");
			return;
		}
		AZ_VampCrypt.MoveTo(p, AZ_VampCryptLedge.PickFrom(AZ_VampCryptA1.ExitSlots(false), "exit to A1"), AZ_VampCryptA1.Fwd(), "a1_stone_exit");
		Print("[AZ_Vamp] crypt exit: " + AZ_VampState.Uid(p) + " slipped out through the stone to A1");
	}
}

class ActionAZCryptExitLeaveCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(5);
	}
}


class ActionAZCryptExitLeave : ActionContinuousBase
{
	void ActionAZCryptExitLeave()
	{
		m_CallbackClass = ActionAZCryptExitLeaveCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "Leave through the stone";
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
		return AZ_VampCryptExit.AtDoor(player);
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		AZ_VampCryptExit.Leave(action_data.m_Player);
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionAZCryptExitLeave);
	}
}

modded class PlayerBase
{
	override void SetActions(out TInputActionMap InputActionMap)
	{
		super.SetActions(InputActionMap);
		AddAction(ActionAZCryptExitLeave, InputActionMap);
	}

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (rpc_type != AZ_VampCryptExit.RPC_EXIT_TELL || !GetGame().IsClient())
			return;
		Param1<vector> dp = new Param1<vector>(vector.Zero);
		if (!ctx.Read(dp))
			return;
		array<Object> near = new array<Object>;
		GetGame().GetObjectsAtPosition(dp.param1, 15, near, null);
		AZ_Crypt_FireBase best = null;
		float bestD = 1000;
		foreach (Object o : near)
		{
			AZ_Crypt_FireBase fire = AZ_Crypt_FireBase.Cast(o);
			if (!fire)
				continue;
			float d = vector.Distance(o.GetPosition(), dp.param1);
			if (d < bestD)
			{
				bestD = d;
				best = fire;
			}
		}
		if (best)
			best.AZ_FireFlare();
		Print("[AZ_Vamp] crypt exit tell client: " + best + " flares, " + bestD + " m from the doorway");
	}
}
