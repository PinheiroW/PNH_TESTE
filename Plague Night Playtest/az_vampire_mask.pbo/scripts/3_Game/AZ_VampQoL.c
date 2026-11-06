

class AZ_VampQoL
{
	static const int RPC_REPORT = 4702401;
	static const int MENU_REPORT = 4702402;

	static const int KIND_BUG = 0;
	static const int KIND_IDEA = 1;
	static const int KIND_FEEDBACK = 2;
	static const int KIND_COUNT = 3;

	static const int TEXT_MIN = 3;
	static const int TEXT_MAX = 500;

	static string KindLabel(int kind)
	{
		switch (kind)
		{
			case KIND_BUG: return "Bug";
			case KIND_IDEA: return "Idea";
			case KIND_FEEDBACK: return "Feedback";
		}
		return "";
	}
};
