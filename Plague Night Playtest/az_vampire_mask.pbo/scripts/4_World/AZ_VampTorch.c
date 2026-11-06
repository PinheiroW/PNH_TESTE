


class AZ_VampTorch extends Torch
{
	static ref array<AZ_VampTorch> s_All;
	static int s_Planted;
	static const int MAX_PLANTED = 320;
	static const int MAX_DOORS = 100;
	static int s_Doors;
	bool m_AZTorchLightsEnabled = true;

	static void ApplyLightConfig(bool enabled)
	{
		if (!GetGame().IsServer() || !s_All)
			return;
		for (int i = s_All.Count() - 1; i >= 0; i--)
		{
			AZ_VampTorch torch = s_All[i];
			if (!torch || torch.IsSetForDeletion())
			{
				s_All.Remove(i);
				continue;
			}
			torch.SetTorchLightsEnabled(enabled);
		}
	}

	void SetTorchLightsEnabled(bool enabled)
	{
		if (!GetGame().IsServer() || m_AZTorchLightsEnabled == enabled)
			return;
		m_AZTorchLightsEnabled = enabled;
		SetSynchDirty();
	}


	static AZ_VampTorch Plant(vector at, float yaw, bool door = false)
	{
		if (door && s_Doors >= MAX_DOORS)
			return null;
		if (s_Planted >= MAX_PLANTED)
		{
			if (s_Planted == MAX_PLANTED)
				Print("[AZ_Vamp] torch cap reached (" + MAX_PLANTED + "), no more torches");
			s_Planted++;
			return null;
		}
		at[1] = GetGame().SurfaceY(at[0], at[2]);
		if (AZ_VampScenes.Blocked(at))
			return null;
		AZ_VampTorch t = AZ_VampTorch.Cast(GetGame().CreateObjectEx("AZ_VampTorch", at, ECE_KEEPHEIGHT | ECE_NOLIFETIME | ECE_NOPERSISTENCY_WORLD));
		if (!t)
			return null;

		t.SetOrientation(Vector(yaw, Math.RandomFloat(0, 6), 0));
		at[1] = at[1] - 0.05;
		t.SetPosition(at);
		if (s_Planted == 0)
			Print("[AZ_Vamp] first planted torch at " + at);
		AZ_VampDecor.Remember(at);
		s_Planted++;
		if (door)
			s_Doors++;
		return t;
	}




	static const float GAP_M = 12;
	static const int PER_FIRE = 8;
	static const float TRAIL_STEP = 60;
	static const float TRAIL_LEN = 420;

	static void PlaceMore()
	{
		if (!AZ_VampState.Cfg().halloween || !AZ_VampState.Cfg().scriptDecor)
			return;
		int before = s_Planted;
		int church = 0;
		int trail = 0;
		int village = 0;
		if (AZ_VampLandmarks.s_Churches)
		{
			foreach (Object ch : AZ_VampLandmarks.s_Churches)
			{
				if (!ch)
					continue;
				vector c = ch.GetPosition();

				for (int k = 0; k < 4; k++)
				{
					float a = k * Math.PI_HALF + Math.PI * 0.25;
					for (float cr = 13; cr <= 21; cr += 4)
					{
						vector cp = c + Vector(Math.Cos(a) * cr, 0, Math.Sin(a) * cr);
						if (AZ_VampTorch.Outdoors(cp, 4) && AZ_VampTorch.Plant(cp, Math.RandomFloat(0, 360)) != null)
						{
							church++;
							break;
						}
					}
				}
			}
		}
		Fireplace near;
		float nearD = 1000000;
		vector crack = Vector(AZ_VampCryptA1.CRACK_X, 0, AZ_VampCryptA1.CRACK_Z);
		if (AZ_VampLandmarks.s_Fires)
		{
			foreach (Fireplace f : AZ_VampLandmarks.s_Fires)
			{
				if (!f)
					continue;
				float d = vector.Distance(f.GetPosition(), crack);
				if (d < nearD)
				{
					nearD = d;
					near = f;
				}
				village += AZ_VampTorch.Gaps(f.GetPosition());
			}
		}
		if (near)
		{
			vector dir = near.GetPosition() - crack;
			dir[1] = 0;
			dir.Normalize();
			vector side = Vector(dir[2], 0, -dir[0]);
			float len = Math.Min(TRAIL_LEN, nearD - 30);
			int n = 0;
			for (float t = 40; t <= len; t += TRAIL_STEP)
			{
				float s = 2.5;
				if (n % 2 == 1)
					s = -2.5;
				vector tp = crack + dir * t + side * s;
				if (AZ_VampTorch.Outdoors(tp) && AZ_VampTorch.Plant(tp, Math.RandomFloat(0, 360)) != null)
					trail++;
				n++;
			}
		}


		int sites = 0;
		string world;
		GetGame().GetWorldName(world);
		world.ToLower();
		if (world == "namalsk")
		{
			sites += AZ_VampTorch.Gaps(Vector(7206.0, 0, 10820.0), 6);
			array<float> mil = {3937.9, 9988.0, 4099.4, 8050.7, 4077.5, 9227.1, 4266.7, 8282.1, 4468.7, 8149.2, 4419.6, 10743.3, 4446.4, 11191.4, 4822.7, 10833.2, 4900.0, 8200.4, 4900.2, 6696.9, 4968.0, 6543.7, 5648.0, 10044.7, 5977.5, 6670.6, 6804.4, 11677.1};
			for (int m = 0; m + 1 < mil.Count(); m += 2)
				sites += AZ_VampTorch.Gaps(Vector(mil[m], 0, mil[m + 1]), 4);
		}
		Print("[AZ_Vamp] torches: " + church + " at churches, " + trail + " on the A1 trail, " + village + " in village gaps, " + sites + " at Vorkuta and military sites (" + (s_Planted - before) + " new, " + s_Planted + " in all, cap " + MAX_PLANTED + ")");
	}


	static int Gaps(vector fire, int want = PER_FIRE)
	{
		int put = 0;
		for (int ring = 0; ring < 4 && put < want; ring++)
		{
			float r = 15 + ring * 10;
			float off = Math.RandomFloat(0, Math.PI2);
			for (int k = 0; k < 12 && put < want; k++)
			{
				float a = off + k * Math.PI2 / 12;
				vector p = fire + Vector(Math.Cos(a) * r, 0, Math.Sin(a) * r);
				p[1] = GetGame().SurfaceY(p[0], p[2]);
				if (AZ_VampDecor.TooClose(p, GAP_M) || !AZ_VampTorch.Outdoors(p))
					continue;
				if (AZ_VampTorch.Plant(p, Math.RandomFloat(0, 360)) != null)
					put++;
			}
		}
		return put;
	}


	static bool Outdoors(vector p, float clear = 15)
	{
		float x = p[0];
		float z = p[2];
		if (GetGame().SurfaceIsSea(x, z) || GetGame().SurfaceIsPond(x, z))
			return false;
		float y = GetGame().SurfaceY(x, z);
		vector top = Vector(x, y + clear, z);
		vector bottom = Vector(x, y + 0.3, z);
		vector hit, nrm;
		int comp;
		set<Object> res = new set<Object>;
		if (DayZPhysics.RaycastRV(top, bottom, hit, nrm, comp, res, null, null, false, false, ObjIntersectGeom))
			return false;
		vector n = GetGame().SurfaceGetNormal(x, z);
		return n[1] > 0.9;
	}

	void AZ_VampTorch()
	{
		RegisterNetSyncVariableBool("m_AZTorchLightsEnabled");
		if (!s_All)
			s_All = new array<AZ_VampTorch>;
		s_All.Insert(this);
		if (GetGame().IsServer())
		{
			m_AZTorchLightsEnabled = AZ_VampState.Cfg().torchLightsEnabled;
			SetSynchDirty();
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZLight, 1000, false);
		}
	}

	void ~AZ_VampTorch()
	{
		if (s_All)
			s_All.RemoveItem(this);
	}

	void AZLight()
	{
		if (!GetCompEM() || IsSetForDeletion())
			return;
		SetLifetime(3888000);
		if (!GetRag())
			GetInventory().CreateAttachment("Rag");
		Rag rag = GetRag();
		if (rag)
			rag.SetQuantity(1);
		GetCompEM().SetEnergy(GetCompEM().GetEnergyMax());
		GetCompEM().SwitchOn();
		if (!GetCompEM().IsWorking())
			Print("[AZ_Vamp] scenery torch failed to light at " + GetPosition());
	}

	override void OnWork(float consumed_energy)
	{
		super.OnWork(consumed_energy);
		if (!GetGame().IsServer())
			return;
		GetCompEM().SetEnergy(GetCompEM().GetEnergyMax());
		Rag rag = GetRag();
		if (rag)
		{
			rag.SetQuantity(1);
			rag.SetHealth("", "", rag.GetMaxHealth("", ""));
		}
	}



	override void UpdateLight()
	{
		super.UpdateLight();
		if (m_Light)
		{
			m_Light.SetFlickerAmplitude(0.15);
			m_Light.SetFlickerSpeed(0.5);
			if (m_AZTorchLightsEnabled)
			{
				m_Light.FadeBrightnessTo(TorchLight.m_TorchBrightness, 0);
				m_Light.FadeRadiusTo(TorchLight.m_TorchRadius, 0);
			}
			else
			{
				m_Light.FadeBrightnessTo(0, 0);
				m_Light.FadeRadiusTo(0, 0);
			}
		}
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		UpdateLight();
	}

	override bool CanPutInCargo(EntityAI parent)
	{
		return false;
	}

	override bool CanPutIntoHands(EntityAI parent)
	{
		return false;
	}

	override bool IsTakeable()
	{
		return false;
	}
}
