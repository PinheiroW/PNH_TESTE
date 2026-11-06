



class AZ_VampCandles
{
	static const int MAX_CANDLES = 700;
	static int s_Count;

	static void House(Building b, float floorY)
	{
		if (!AZ_VampState.Cfg().decorCandles || s_Count >= MAX_CANDLES || Math.RandomInt(0, 2) == 0)
			return;
		vector box[2];
		if (!b.GetCollisionBox(box))
			return;
		int want = Math.RandomInt(1, 4);
		int got = 0;
		for (int t = 0; t < 8 && got < want; t++)
		{
			vector lp = Vector(Math.RandomFloat(box[0][0] + 1.0, box[1][0] - 1.0), 0, Math.RandomFloat(box[0][2] + 1.0, box[1][2] - 1.0));
			vector w = b.ModelToWorld(lp);
			vector from = Vector(w[0], floorY + 1.3, w[2]);
			vector to = Vector(w[0], floorY - 0.6, w[2]);
			vector hit, normal;
			int comp;
			set<Object> results = new set<Object>;
			if (!DayZPhysics.RaycastRV(from, to, hit, normal, comp, results, null, null, false, false, ObjIntersectView))
				continue;
			if (normal[1] < 0.85 || hit[1] < floorY - 0.5 || hit[1] > floorY + 1.2)
				continue;
			if (results.Count() == 0)
				continue;
			if (AZ_VampScenes.Blocked(hit))
				continue;
			Object c = GetGame().CreateObjectEx("AZ_VampCandle", hit, ECE_KEEPHEIGHT | ECE_NOLIFETIME | ECE_NOPERSISTENCY_WORLD);
			if (!c)
				continue;
			c.SetPosition(hit);
			c.SetOrientation(Vector(Math.RandomFloat(0, 360), 0, 0));
			got++;
			s_Count++;
		}
	}
}

class AZ_VampCandleLight extends PointLightBase
{
	void AZ_VampCandleLight()
	{
		SetVisibleDuringDaylight(false);
		SetRadiusTo(3.5);
		SetBrightnessTo(2.2);
		SetCastShadow(false);
		SetDiffuseColor(1.0, 0.55, 0.2);
		SetAmbientColor(1.0, 0.55, 0.2);
		SetFlickerAmplitude(0.3);
		SetFlickerSpeed(0.75);
		FadeIn(0.5);
	}
}


class AZ_VampCandle extends Candle
{
	protected AZ_VampCandleLight m_AZLight;

	void AZ_VampCandle()
	{
		SetEventMask(EntityEvent.INIT);
		if (!GetGame().IsDedicatedServer() && AZ_VampLightBudget.Active())
			AZ_VampLightBudget.Add(this);
	}

	override void EOnInit(IEntity other, int extra)
	{
		super.EOnInit(other, extra);
		if (GetGame().IsDedicatedServer())
			return;
		SetObjectTexture(0, "dz\\gear\\tools\\data\\candle_flame_ca.paa");
		SetObjectMaterial(0, "dz\\gear\\tools\\data\\candle_flame.rvmat");
		if (!AZ_VampLightBudget.Active())
			AZ_SetLit(true);
	}


	void AZ_SetLit(bool on)
	{
		if (on && !m_AZLight)
		{
			m_AZLight = AZ_VampCandleLight.Cast(AZ_VampLightGate.Light(AZ_VampCandleLight, GetPosition()));
			if (m_AZLight)
				m_AZLight.AttachOnObject(this, "0 0.17 0");
		}
		else if (!on && m_AZLight)
		{
			m_AZLight.Destroy();
			m_AZLight = null;
		}
	}

	override void EEDelete(EntityAI parent)
	{
		AZ_VampLightBudget.Remove(this);
		if (m_AZLight)
			m_AZLight.Destroy();
		super.EEDelete(parent);
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



class AZ_VampLitDecor extends House
{
	protected ScriptedLightBase m_AZLight;

	typename AZLightType()
	{
		return AZ_VampCandleLight;
	}

	override void EEInit()
	{
		super.EEInit();
		if (GetGame().IsDedicatedServer())
			return;


		if (!AZ_LitInCrypt() && AZ_VampCryptLights.InCryptBox(GetPosition()))
			return;
		if (!AZ_LitInCrypt() && AZ_VampLightBudget.Active())
		{
			AZ_VampLightBudget.Add(this);
			return;
		}
		m_AZLight = AZ_VampLightGate.AtPoint(AZLightType(), this, "light");
	}

	void AZ_SetOutdoorLit(bool on)
	{
		if (AZ_LitInCrypt() || AZ_VampCryptLights.InCryptBox(GetPosition()))
			return;
		if (on && !m_AZLight)
			m_AZLight = AZ_VampLightGate.AtPoint(AZLightType(), this, "light");
		else if (!on && m_AZLight)
		{
			m_AZLight.Destroy();
			m_AZLight = null;
		}
	}

	bool AZ_LitInCrypt()
	{
		return false;
	}

	override void EEDelete(EntityAI parent)
	{
		AZ_VampLightBudget.Remove(this);
		super.EEDelete(parent);
		if (m_AZLight)
			m_AZLight.Destroy();
	}
}

class AZ_VampVotive extends AZ_VampLitDecor
{
	override typename AZLightType()
	{
		return AZ_VampVotiveLight;
	}
}

class AZ_VampLampada extends AZ_VampLitDecor
{
	override typename AZLightType()
	{
		return AZ_VampVotiveLight;
	}
}

class AZ_VampSconce extends AZ_VampLitDecor
{
}

class AZ_VampCandelabra extends AZ_VampLitDecor
{
	override typename AZLightType()
	{
		return AZ_VampCandelabraLight;
	}
}

class AZ_VampCandleCircle extends AZ_VampLitDecor
{
	override typename AZLightType()
	{
		return AZ_VampCandelabraLight;
	}
}

class AZ_VampSkullPile extends AZ_VampLitDecor
{
	override typename AZLightType()
	{
		return AZ_VampCandelabraLight;
	}
}


class AZ_VampCandelabraLight extends PointLightBase
{
	void AZ_VampCandelabraLight()
	{
		SetVisibleDuringDaylight(false);
		SetRadiusTo(6.0);
		SetBrightnessTo(3.0);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetDiffuseColor(1.0, 0.55, 0.2);
		SetAmbientColor(1.0, 0.55, 0.2);
		SetFlickerAmplitude(0.3);
		SetFlickerSpeed(0.75);
		FadeIn(0.5);
	}
}

class AZ_VampVotiveLight extends PointLightBase
{
	void AZ_VampVotiveLight()
	{
		SetVisibleDuringDaylight(false);
		SetRadiusTo(3.0);
		SetBrightnessTo(1.8);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetDiffuseColor(1.0, 0.18, 0.08);
		SetAmbientColor(1.0, 0.18, 0.08);
		SetFlickerAmplitude(0.3);
		SetFlickerSpeed(0.75);
		FadeIn(0.5);
	}
}
