

class AZ_VampCryptTunnel
{
    static const float WORK_SEC = 20;
    static const float REACH = 2.7;
    static bool s_Cleared;
    static Object s_Plug;
    static ref ScriptInvoker s_OnCleared = new ScriptInvoker;

    static vector WorkPoint()
    {
        return AZ_VampCrypt.Nave(20.5, 11.3, 0.8);
    }

    static bool IsTarget(Object rubble)
    {
        if (!rubble || rubble.GetType() != "AZ_Crypt_Plug_Squeeze" || !AZ_VampCrypt.Site())
            return false;
        return vector.DistanceSq(rubble.GetPosition(), AZ_VampCrypt.Nave(0, 0)) < 0.01;
    }

    static Object FindPlug()
    {
        if (GetGame().IsServer() && s_Cleared)
            return null;
        if (IsTarget(s_Plug))
            return s_Plug;
        s_Plug = null;
        array<Object> near = new array<Object>;
        GetGame().GetObjectsAtPosition(WorkPoint(), 5, near, null);
        foreach (Object candidate : near)
        {
            if (IsTarget(candidate))
            {
                s_Plug = candidate;
                return s_Plug;
            }
        }


        near.Clear();
        GetGame().GetObjectsAtPosition(AZ_VampCrypt.Nave(0, 0), 1, near, null);
        foreach (Object originCandidate : near)
        {
            if (IsTarget(originCandidate))
            {
                s_Plug = originCandidate;
                return s_Plug;
            }
        }
        return null;
    }

    static bool IsTool(ItemBase tool)
    {
        if (!tool || tool.IsRuined())
            return false;
        return tool.IsKindOf("Pickaxe") || tool.IsKindOf("Shovel") || tool.IsKindOf("FieldShovel") || tool.IsKindOf("SledgeHammer");
    }

    static bool NearTarget(PlayerBase player, Object rubble)
    {
        if (!player || !IsTarget(rubble) || (GetGame().IsServer() && s_Cleared))
            return false;
        return vector.DistanceSq(player.GetPosition(), WorkPoint()) <= REACH * REACH;
    }

    static bool CanWork(PlayerBase player, Object rubble, ItemBase tool)
    {
        if (!player || !player.IsAlive() || player.IsUnconscious() || player.IsRestrained() || player.IsInVehicle() || player.m_AZVampHeld || player.m_AZVampCompelled)
            return false;
        if (!IsTool(tool) || player.GetItemInHands() != tool || !NearTarget(player, rubble))
            return false;
        if (GetGame().IsServer() && rubble != FindPlug())
            return false;
        return true;
    }

    static bool Begin(PlayerBase player, Object rubble, ItemBase tool)
    {
        if (!GetGame().IsServer() || !CanWork(player, rubble, tool))
            return false;
        player.m_AZTunnelStartedAt = GetGame().GetTime();
        player.m_AZTunnelTarget = rubble;
        player.m_AZTunnelTool = tool;
        return true;
    }

    static void Cancel(PlayerBase player)
    {
        if (!player)
            return;
        player.m_AZTunnelStartedAt = 0;
        player.m_AZTunnelTarget = null;
        player.m_AZTunnelTool = null;
    }

    static bool Complete(PlayerBase player, Object rubble, ItemBase tool)
    {
        if (!GetGame().IsServer() || !player || !rubble || !tool)
            return false;
        if (player.m_AZTunnelTarget != rubble || player.m_AZTunnelTool != tool)
            return false;
        if (GetGame().GetTime() - player.m_AZTunnelStartedAt < WORK_SEC * 1000)
            return false;
        if (!CanWork(player, rubble, tool))
            return false;


        s_Cleared = true;
        s_Plug = null;
        Cancel(player);
        GetGame().ObjectDelete(rubble);
        MiscGameplayFunctions.DealEvinronmentAdjustedDmg(tool, player, 4);
        s_OnCleared.Invoke(player);
        player.Message("The rubble gives way. The passage is clear.", "colorFriendly");
        Print("[AZ_Vamp] dig tunnel cleared; tomb passage open");
        return true;
    }
}

modded class PlayerBase
{

    int m_AZTunnelStartedAt;
    Object m_AZTunnelTarget;
    ItemBase m_AZTunnelTool;
}

class CCTAZClearCryptTunnel : CCTBase
{
    override bool Can(PlayerBase player, ActionTarget target)
    {
        return target && AZ_VampCryptTunnel.NearTarget(player, target.GetObject());
    }
}

class ActionAZClearCryptTunnelCB : ActionContinuousBaseCB
{
    override void CreateActionComponent()
    {
        m_ActionData.m_ActionComponent = new CAContinuousTime(AZ_VampCryptTunnel.WORK_SEC);
    }
}

class ActionAZClearCryptTunnel : ActionContinuousBase
{
    void ActionAZClearCryptTunnel()
    {
        m_CallbackClass = ActionAZClearCryptTunnelCB;
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_MINEROCK;
        m_FullBody = true;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
        m_Text = "Clear the collapsed tunnel";
    }



    override protected int GetActionCommandEx(ActionData actionData)
    {
        ItemBase item;
        if (actionData)
            item = actionData.m_MainItem;
        if (item && (item.IsKindOf("Shovel") || item.IsKindOf("FieldShovel")))
            return DayZPlayerConstants.CMD_ACTIONFB_DIGMANIPULATE;
        return DayZPlayerConstants.CMD_ACTIONFB_MINEROCK;
    }

    override void CreateConditionComponents()
    {
        m_ConditionItem = new CCINonRuined;
        m_ConditionTarget = new CCTAZClearCryptTunnel;
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        return target && AZ_VampCryptTunnel.CanWork(player, target.GetObject(), item);
    }

    override void OnStartServer(ActionData action_data)
    {
        super.OnStartServer(action_data);
        if (action_data && action_data.m_Target)
            AZ_VampCryptTunnel.Begin(action_data.m_Player, action_data.m_Target.GetObject(), action_data.m_MainItem);
    }

    override void OnFinishProgressServer(ActionData action_data)
    {
        if (action_data && action_data.m_Target)
            AZ_VampCryptTunnel.Complete(action_data.m_Player, action_data.m_Target.GetObject(), action_data.m_MainItem);
    }

    override void OnEndServer(ActionData action_data)
    {
        if (action_data)
            AZ_VampCryptTunnel.Cancel(action_data.m_Player);
        super.OnEndServer(action_data);
    }
}

modded class ActionConstructor
{
    override void RegisterActions(TTypenameArray actions)
    {
        super.RegisterActions(actions);
        actions.Insert(ActionAZClearCryptTunnel);
    }
}

modded class Pickaxe
{
    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionAZClearCryptTunnel);
    }
}

modded class Shovel
{
    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionAZClearCryptTunnel);
    }
}

modded class FieldShovel
{
    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionAZClearCryptTunnel);
    }
}

modded class SledgeHammer
{
    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionAZClearCryptTunnel);
    }
}
