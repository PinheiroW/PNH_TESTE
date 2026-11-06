






class AZ_VampCostumes
{
	static const string COAT = "AZ_VampElder_Coat";
	static const string DRESS_SICK = "AZ_VampElder_DressSick";
	static const string DRESS_BEAUTY = "AZ_VampElder_DressBeauty";

	static const string GREATCOAT = "AZ_VampElder_GreatCoat";
	static const string GRAVE_GOWN = "AZ_VampElder_GraveGown";
	static const string VELVET_GOWN = "AZ_VampElder_VelvetGown";
	static const float LIFETIME = 3888000;

	static EntityAI Place(string cls, vector pos, vector ori = "0 0 0", bool keepHeight = false)
	{
		if (!GetGame().IsServer())
			return null;
		if (!keepHeight)
			pos[1] = GetGame().SurfaceY(pos[0], pos[2]);
		array<Object> objs = new array<Object>;
		array<CargoBase> proxies = new array<CargoBase>;
		GetGame().GetObjectsAtPosition(pos, 3, objs, proxies);
		foreach (Object o : objs)
		{
			if (o && o.GetType() == cls)
			{
				EntityAI old = EntityAI.Cast(o);
				if (old && !old.GetHierarchyParent())
				{
					if (vector.Distance(old.GetPosition(), pos) > 0.5)
					{
						old.SetPosition(pos);
						old.SetOrientation(ori);
					}
					old.SetLifetime(LIFETIME);
					Print("[AZ_Vamp] costume " + cls + " already at " + o.GetPosition());
					return old;
				}
			}
		}
		int flags = ECE_CREATEPHYSICS;
		if (keepHeight)
			flags |= ECE_KEEPHEIGHT;
		else
			flags |= ECE_PLACE_ON_SURFACE;
		EntityAI item = EntityAI.Cast(GetGame().CreateObjectEx(cls, pos, flags));
		if (!item)
		{
			Print("[AZ_Vamp] costume " + cls + " FAILED to spawn at " + pos);
			return null;
		}
		item.SetOrientation(ori);
		item.SetLifetimeMax(LIFETIME);
		item.SetLifetime(LIFETIME);
		Print("[AZ_Vamp] costume " + cls + " placed at " + item.GetPosition());
		return item;
	}
}
