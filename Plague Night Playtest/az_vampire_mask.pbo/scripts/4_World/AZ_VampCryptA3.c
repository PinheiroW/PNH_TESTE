










class AZ_VampCryptA3
{
	static const vector GAS = "3006 0 6403";
	static const float GAS_R = 105.0;
	static const float OUT_R = 125.0;
	static const float DEEP_F = 48.0;
	static const float DEEP_R = -6.75;
	static const float AT_DEEP = 2.2;

	static bool s_TestOpen;
	static Object s_Deep;
	static Object s_Out;
	static vector s_Exit;
	static vector s_ExitDir;

	static bool Ready()
	{
		return false;
	}

	static vector DeepPos()
	{
		return AZ_VampCrypt.Nave(DEEP_F, DEEP_R, 0.0);
	}


	static void PickExit()
	{
		float best = 1e9;
		for (int i = 0; i < 8; i++)
		{
			float a = i * 45.0 * Math.DEG2RAD;
			vector d = Vector(Math.Sin(a), 0, Math.Cos(a));
			vector p = GAS + d * OUT_R;
			p[1] = GetGame().SurfaceY(p[0], p[2]);
			float slope = 0;
			slope += Math.AbsFloat(GetGame().SurfaceY(p[0] + 2, p[2]) - p[1]);
			slope += Math.AbsFloat(GetGame().SurfaceY(p[0] - 2, p[2]) - p[1]);
			slope += Math.AbsFloat(GetGame().SurfaceY(p[0], p[2] + 2) - p[1]);
			slope += Math.AbsFloat(GetGame().SurfaceY(p[0], p[2] - 2) - p[1]);
			array<Object> objs = new array<Object>;
			GetGame().GetObjectsAtPosition(p, 5, objs, null);
			float score = slope + objs.Count() * 0.5;
			if (score < best)
			{
				best = score;
				s_Exit = p;
				s_ExitDir = d;
			}
		}
		Print("[AZ_Vamp] crypt A3: way out at " + s_Exit + " (" + vector.Distance(Vector(s_Exit[0], 0, s_Exit[2]), GAS) + " m from the gas centre, score " + best + ")");
	}

	static void Build()
	{
		Refresh();
	}

	static bool Open()
	{
		return false;
	}


	static void Refresh()
	{

		if (s_Deep)
		{
			GetGame().ObjectDelete(s_Deep);
			s_Deep = null;
		}
		if (s_Out)
		{
			GetGame().ObjectDelete(s_Out);
			s_Out = null;
		}
		s_Seen = false;
	}


	static int s_CheckAt;
	static bool s_Seen;
	static bool AtDeep(vector pos)
	{
		return false;
	}

	static vector ExitPoint()
	{
		return s_Exit + "0 0.1 0";
	}

	static void GoDeep(PlayerBase p)
	{

	}
}


class AZ_Crypt_Deep_Crack extends AZ_Crypt_A1_Crack
{
}

class ActionAZCryptDeep : ActionInteractBase
{
	void ActionAZCryptDeep()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "Follow the passage down";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTAZAny;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return player && !player.IsInVehicle() && AZ_VampCryptA3.AtDeep(player.GetPosition());
	}

	override void OnExecuteServer(ActionData action_data)
	{
		AZ_VampCryptA3.GoDeep(action_data.m_Player);
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionAZCryptDeep);
	}
}

modded class PlayerBase
{
	override void SetActions(out TInputActionMap InputActionMap)
	{
		super.SetActions(InputActionMap);
		AddAction(ActionAZCryptDeep, InputActionMap);
	}
}
