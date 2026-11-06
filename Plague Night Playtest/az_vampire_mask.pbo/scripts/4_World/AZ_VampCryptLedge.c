




class AZ_VampCryptLedge
{
	static const float SLOT_F0 = 1.6;
	static const float SLOT_DF = 0.6;
	static const float SLOT_R = 1.9;



	static const float EXIT_F = 0.8;
	static const float EXIT_R = -5.5;
	static const float EXIT_RADIUS = 2.0;
	static const float SLOT_CLEAR = 1.0;

	static const float VAMP_CLEAR = 4.0;
	static ref array<vector> s_Held;
	static ref array<int> s_HeldUntil;
	static const int LEAVE_HURT_MS = 5000;



	static vector Floor(float f, float r)
	{
		vector from = AZ_VampCrypt.Nave(f, r, 1.5);
		vector to = AZ_VampCrypt.Nave(f, r, -0.2);
		vector hit = AZ_VampCrypt.Nave(f, r, 0);
		Object skip = null;
		for (int k = 0; k < 4; k++)
		{
			vector rayHit, normal;
			int comp;
			set<Object> res = new set<Object>;
			if (!DayZPhysics.RaycastRV(from, to, rayHit, normal, comp, res, null, skip, false, false, ObjIntersectGeom))
				return hit;
			Object man = null;
			foreach (Object o : res)
			{
				if (Man.Cast(o))
					man = o;
			}
			if (!man)
				return rayHit;
			skip = man;
		}
		return hit;
	}

	static array<vector> Slots()
	{
		array<vector> s = new array<vector>;

		for (int col = -1; col <= 1; col++)
			s.Insert(Floor(SLOT_F0 + SLOT_DF, SLOT_R * col) + Vector(0, 0.05, 0));
		s.Insert(Floor(SLOT_F0 + 0.1, -SLOT_R * 0.5) + Vector(0, 0.05, 0));
		s.Insert(Floor(SLOT_F0 + 0.1, SLOT_R * 0.5) + Vector(0, 0.05, 0));
		return s;
	}



	static vector Pick()
	{
		return PickFrom(Slots(), "ledge");
	}


	static bool s_LastFree;

	static vector PickFrom(array<vector> slots, string what)
	{
		if (!s_Held)
		{
			s_Held = new array<vector>;
			s_HeldUntil = new array<int>;
		}
		int now = GetGame().GetTime();
		for (int h = s_Held.Count() - 1; h >= 0; h--)
		{
			if (s_HeldUntil[h] < now)
			{
				s_Held.Remove(h);
				s_HeldUntil.Remove(h);
			}
		}
		array<Object> near = new array<Object>;
		GetGame().GetObjectsAtPosition3D(slots[0], 25, near, null);
		int best = -1;
		float bestVamp = -1;
		int fallback = 0;
		int fallbackCrowd = 1000;
		for (int i = 0; i < slots.Count(); i++)
		{
			int crowd = 0;
			float vampD = 1000;
			foreach (Object o : near)
			{
				PlayerBase pb = PlayerBase.Cast(o);
				if (!pb || !pb.IsAlive())
					continue;
				float d = vector.Distance(pb.GetPosition(), slots[i]);
				if (d < SLOT_CLEAR)
					crowd++;
				if (pb.m_AZVampire && d < vampD)
					vampD = d;
			}
			foreach (vector hv : s_Held)
			{
				if (vector.Distance(hv, slots[i]) < SLOT_CLEAR)
					crowd++;
			}
			if (crowd < fallbackCrowd)
			{
				fallback = i;
				fallbackCrowd = crowd;
			}
			if (crowd == 0 && vampD > VAMP_CLEAR && vampD > bestVamp)
			{
				best = i;
				bestVamp = vampD;
			}
		}
		s_LastFree = best >= 0;
		if (best < 0)
			best = fallback;
		s_Held.Insert(slots[best]);
		s_HeldUntil.Insert(now + 3000);
		Print("[AZ_Vamp] crypt " + what + ": slot " + best + " (nearest vampire " + bestVamp + " m, free " + (bestVamp >= 0) + ")");
		return slots[best];
	}





	static const int WAIT_STEP_MS = 500;
	static const int WAIT_MS = 6000;
	static const int WAIT_MAX_MS = 30000;


	static const float OVER_F[4] = {0.5, 0.5, 0.15, 0.15};
	static const float OVER_R[4] = {-2.85, 2.85, -0.9, 0.9};
	static const float OVER_Y[4] = {-0.72, -0.72, -0.9, -0.9};

	static array<vector> Overflow()
	{
		array<vector> s = new array<vector>;
		for (int i = 0; i < 4; i++)
			s.Insert(AZ_VampCrypt.Nave(OVER_F[i], OVER_R[i], OVER_Y[i] + 0.05));
		return s;
	}


	static void Release(vector at)
	{
		if (!s_Held)
			return;
		int i = s_Held.Find(at);
		if (i < 0)
			return;
		s_Held.Remove(i);
		s_HeldUntil.Remove(i);
	}

	static void Arrive(PlayerBase p, int waited, string route = "ledge")
	{
		if (!p || !p.IsAlive())
			return;
		vector at = PickFrom(Slots(), "ledge");
		bool free = s_LastFree;
		int k = -1;
		if (!free && waited < WAIT_MS)
		{
			s_Held.Remove(s_Held.Count() - 1);
			s_HeldUntil.Remove(s_HeldUntil.Count() - 1);
			p.m_AZCryptAt = GetGame().GetTime() + 3000;
			AZ_VampCrypt.Transit(p);
			if (waited % 1000 == 0 && p.GetIdentity())
				GetGame().RPCSingleParam(p, AZ_VampCrypt.RPC_FADE, new Param1<int>(AZ_VampCrypt.FADE_MS), true, p.GetIdentity());
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Arrive, WAIT_STEP_MS, false, p, waited + WAIT_STEP_MS, route);
			return;
		}
		if (!free)
		{
			s_Held.Remove(s_Held.Count() - 1);
			s_HeldUntil.Remove(s_HeldUntil.Count() - 1);
			array<vector> over = Overflow();
			at = PickFrom(over, "ledge overflow");
			k = over.Find(at);
			if (!s_LastFree && waited < WAIT_MAX_MS)
			{
				s_Held.Remove(s_Held.Count() - 1);
				s_HeldUntil.Remove(s_HeldUntil.Count() - 1);
				p.m_AZCryptAt = GetGame().GetTime() + 3000;
				AZ_VampCrypt.Transit(p);
				if (waited % 1000 == 0 && p.GetIdentity())
					GetGame().RPCSingleParam(p, AZ_VampCrypt.RPC_FADE, new Param1<int>(AZ_VampCrypt.FADE_MS), true, p.GetIdentity());
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Arrive, WAIT_STEP_MS, false, p, waited + WAIT_STEP_MS, route);
				return;
			}
		}
		if (waited > 0 || k >= 0)
			Print("[AZ_Vamp] ledge full: waited " + waited + " ms / overflow slot " + k + " (free " + s_LastFree + ")");
		if (waited == 0)
			AZ_VampCrypt.MoveTo(p, at, Facing(at), route);
		else
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampCrypt.Land, 300, false, p, at, Facing(at), route);
	}


	static vector Facing(vector at)
	{
		vector d = AZ_VampCrypt.Nave(AZ_VampCrypt.CountessF(), 0, 0) - at;
		d[1] = 0;
		d.Normalize();
		return d;
	}


	static string AdminTp(PlayerBase p, string where)
	{
		if (!p)
			return "no player";
		vector at;
		vector look;
		if (where == "ledge")
		{
			array<vector> slots = Slots();
			at = slots[1];
			look = AZ_VampCrypt.Nave(AZ_VampCrypt.CountessF(), 0, 0);
		}
		else if (where == "altar")
		{
			at = AZ_VampCryptGallery.DropOnto(AZ_VampCrypt.ALTAR_F - 1.6, 1.2, 2.0) + Vector(0, 0.05, 0);
			look = AZ_VampCrypt.Nave(AZ_VampCrypt.ALTAR_F, 0, 0);
		}
		else if (where == "gate")
		{
			at = AZ_VampCrypt.Mouth();
			at[1] = GetGame().SurfaceY(at[0], at[2]);
			look = AZ_VampCrypt.Door(0, -AZ_VampCrypt.DOOR_FRONT);
		}
		else
			return "usage: /vamp tp ledge|altar|gate";
		vector d = look - at;
		d[1] = 0;
		AZ_VampSnap.Far(p, at, d.Normalized());
		Print("[AZ_Vamp] admin tp " + where + " " + AZ_VampState.Uid(p) + " to " + at);
		return "moved to " + where;
	}

	static bool AtExit(PlayerBase p)
	{
		if (!p || AZ_VampCrypt.EntryOn() || !AZ_VampCrypt.Inside(p.GetPosition()))
			return false;
		vector exit = AZ_VampCrypt.Hall(EXIT_F, EXIT_R);
		vector to = exit - p.GetPosition();
		to[1] = 0;
		if (to.Length() > EXIT_RADIUS)
			return false;
		vector back = -AZ_VampCrypt.Fwd(AZ_VampCrypt.HALL_YAW);
		vector look = p.GetDirection();
		look[1] = 0;
		return vector.Dot(look.Normalized(), back) > 0.6;
	}

	static void Leave(PlayerBase p)
	{
		if (!AtExit(p) || !AZ_VampCrypt.CanMove(p))
			return;
		if (GetGame().GetTime() - p.m_AZLastHitAt < LEAVE_HURT_MS)
		{
			p.Message("Not with blood still running. Wait a moment, then try again.", "colorAction");
			Print("[AZ_Vamp] crypt ledge: " + AZ_VampState.Uid(p) + " leave refused (hurt)");
			return;
		}
		AZ_VampCrypt.MoveOut(p);
		Print("[AZ_Vamp] crypt ledge: " + AZ_VampState.Uid(p) + " came up (way " + p.m_AZCryptFrom + ")");
	}
}

class ActionAZCryptLedgeLeaveCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(5);
	}
}


class ActionAZCryptLedgeLeave : ActionContinuousBase
{
	void ActionAZCryptLedgeLeave()
	{
		m_CallbackClass = ActionAZCryptLedgeLeaveCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "Leave the crypt";
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
		return AZ_VampCryptLedge.AtExit(player);
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		AZ_VampCryptLedge.Leave(action_data.m_Player);
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionAZCryptLedgeLeave);
	}
}

modded class PlayerBase
{
	override void SetActions(out TInputActionMap InputActionMap)
	{
		super.SetActions(InputActionMap);
		AddAction(ActionAZCryptLedgeLeave, InputActionMap);
	}
}
