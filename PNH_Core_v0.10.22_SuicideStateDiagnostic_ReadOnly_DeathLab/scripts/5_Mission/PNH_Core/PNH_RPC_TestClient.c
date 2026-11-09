// LAB ONLY: one-shot PING 12 seconds after the gameplay mission starts.
// This is NOT the future gameplay RPC implementation. Remove the probe after validation.
modded class MissionGameplay
{
    override void OnMissionStart()
    {
        super.OnMissionStart();
        if (GetGame() && GetGame().IsClient())
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.PNH_SendTestPing, 12000, false);
    }

    override void OnMissionFinish()
    {
        if (GetGame())
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.PNH_SendTestPing);
        super.OnMissionFinish();
    }

    void PNH_SendTestPing()
    {
        if (!GetGame() || !GetGame().IsClient() || !GetGame().GetPlayer())
        {
            Print("[PNH_CORE][RPC] Client probe skipped (player not ready)");
            return;
        }

        Print("[PNH_CORE][RPC] Client sends PING");
        GetRPCManager().SendRPC("PNH_Core", "RPC_Ping", new Param1<int>(301), true);
    }
};
