





class AZ_VampHolyWater extends GlassBottle
{

	override bool CanFreeze() { return false; }

	override void OnQuantityChanged(float delta)
	{
		super.OnQuantityChanged(delta);
		if (GetGame().IsServer() && GetQuantity() <= 0 && !IsRuined())
			SetHealth("", "", 0);
	}
}

class AZ_VampHolyWaterSpawn
{
	static const int PER_NIGHT = 3;
	static ref array<Object> s_Laid = new array<Object>;

	static void Dusk()
	{

		for (int i = s_Laid.Count() - 1; i >= 0; i--)
		{
			EntityAI old = EntityAI.Cast(s_Laid[i]);
			if (old && !old.GetHierarchyParent())
				GetGame().ObjectDelete(old);
		}
		s_Laid.Clear();
		array<vector> yards = AZ_VampLandmarks.s_Graveyards;
		if (!yards || yards.Count() == 0)
			return;
		for (int n = 0; n < PER_NIGHT; n++)
		{
			vector c = yards.GetRandomElement();
			vector p = c + Vector(Math.RandomFloat(-6, 6), 0, Math.RandomFloat(-6, 6));
			p[1] = GetGame().SurfaceY(p[0], p[2]);
			Object o = GetGame().CreateObjectEx("AZ_VampHolyWater", p, ECE_PLACE_ON_SURFACE);
			if (o)
			{
				s_Laid.Insert(o);
				EntityAI laid = EntityAI.Cast(o);
				if (laid)
				{
					laid.SetLifetimeMax(14400);
					laid.SetLifetime(14400);
				}
			}
		}
		Print("[AZ_Vamp] holy water: " + s_Laid.Count() + " bottles laid at graveyards");
	}
}

modded class PlayerBase
{
	override bool Consume(PlayerConsumeData data)
	{
		AZ_VampHolyWater holy;
		bool isHoly = data && Class.CastTo(holy, data.m_Source) && !holy.IsRuined();
		bool r = super.Consume(data);
		if (!r || !isHoly || !GetGame().IsServer())
			return r;
		if (m_AZVampire)
		{
			AZ_VampEvidence.Begin("holy_water_burn", this, this, false);
			AddHealth("", "Health", -GetMaxHealth("", "Health") / 3);
			AZ_VampEvidence.End("holy_water_burn", this, this);
			Message("It burns like the sun in your throat.", "colorImportant");
			holy.SetHealth("", "", 0);
			Print("[AZ_Vamp] holy water burned a vampire: " + AZ_VampState.Uid(this));
		}
		else if (m_AZVampTurning)
		{
			AZ_VampTurn.Cure(this);
			AZ_VampProgress.OnCured(this);
			Message("The holy water washes the fever out of you. You will not turn.", "colorFriendly");
			holy.SetHealth("", "", 0);
		}
		return r;
	}
}
