




class AZ_VampScenes
{
	static const string DIR = "$profile:AZ_Vampire\\scenes\\";
	static const float KEEP_OUT_M = 12;
	static ref map<int, ref array<vector>> s_Grid;
	static int s_Count;

	static int Key(int cx, int cz)
	{
		return cx * 10000 + cz;
	}

	static void Add(vector p)
	{
		int k = Key(Math.Floor(p[0] / KEEP_OUT_M), Math.Floor(p[2] / KEEP_OUT_M));
		array<vector> cell = s_Grid.Get(k);
		if (!cell)
		{
			cell = new array<vector>;
			s_Grid.Insert(k, cell);
		}
		cell.Insert(p);
		s_Count++;
	}

	static void Load()
	{
		s_Grid = new map<int, ref array<vector>>;
		string name;
		FileAttr attr;
		FindFileHandle h = FindFile(DIR + "*.json", name, attr, 0);
		if (!h)
			return;
		int files = 0;
		bool more = true;
		while (more)
		{
			ObjectSpawnerJson data = new ObjectSpawnerJson;
			string err;
			if (JsonFileLoader<ObjectSpawnerJson>.LoadFile(DIR + name, data, err) && data.Objects)
			{
				foreach (ITEM_SpawnerObject it : data.Objects)
					Add(Vector(it.pos[0], it.pos[1], it.pos[2]));
				files++;
			}
			else
				Print("[AZ_Vamp] scenes: cannot read " + name + " " + err);
			more = FindNextFile(h, name, attr);
		}
		CloseFindFile(h);
		Print("[AZ_Vamp] scenes: " + files + " files, " + s_Count + " keep-out points (" + KEEP_OUT_M + " m)");
	}


	static bool Blocked(vector p)
	{
		if (!s_Grid)
			Load();
		if (s_Count == 0)
			return false;
		int cx = Math.Floor(p[0] / KEEP_OUT_M);
		int cz = Math.Floor(p[2] / KEEP_OUT_M);
		float r2 = KEEP_OUT_M * KEEP_OUT_M;
		for (int dx = -1; dx <= 1; dx++)
		{
			for (int dz = -1; dz <= 1; dz++)
			{
				array<vector> cell = s_Grid.Get(Key(cx + dx, cz + dz));
				if (!cell)
					continue;
				foreach (vector q : cell)
				{
					float ax = p[0] - q[0];
					float az = p[2] - q[2];
					if (ax * ax + az * az < r2)
						return true;
				}
			}
		}
		return false;
	}
}
