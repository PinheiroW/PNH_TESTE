class CfgPatches
{
    class PNH_Afterlife_AL04a_Scripts
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data", "JoinThem", "PNH_Core_Scripts", "PNH_Afterlife_AL03_Scripts"};
    };
};

class CfgMods
{
    class PNH_Afterlife_AL04a
    {
        dir = "PNH_Afterlife_AL04a";
        name = "PNH Afterlife AL-04a Read-Only Lifecycle Observer";
        author = "PNH";
        type = "mod";
        dependencies[] = {"World"};
        class defs
        {
            class worldScriptModule
            {
                value = "";
                files[] = {"PNH_Afterlife_AL04a/scripts/4_World"};
            };
        };
    };
};
