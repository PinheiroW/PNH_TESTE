class CfgPatches
{
    class PNH_Core_Scripts
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data", "JM_CF_Scripts"};
    };
};

class CfgMods
{
    class PNH_Core
    {
        dir = "PNH_Core";
        name = "PNH Core";
        author = "PNH";
        type = "mod";
        dependencies[] = {"Game", "World", "Mission"};

        class defs
        {
            class worldScriptModule
            {
                value = "";
                files[] = {"PNH_Core/scripts/4_World"};
            };
            class missionScriptModule
            {
                value = "";
                files[] = {"PNH_Core/scripts/5_Mission"};
            };
        };
    };
};
