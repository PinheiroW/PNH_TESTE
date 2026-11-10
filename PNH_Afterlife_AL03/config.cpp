class CfgPatches
{
    class PNH_Afterlife_AL03_Scripts
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data", "JoinThem", "PNH_Core_Scripts"};
    };
};

class CfgMods
{
    class PNH_Afterlife_AL03
    {
        dir = "PNH_Afterlife_AL03";
        name = "PNH Afterlife AL-03 Observer";
        author = "PNH";
        type = "mod";
        dependencies[] = {"World"};
        class defs
        {
            class worldScriptModule
            {
                value = "";
                files[] = {"PNH_Afterlife_AL03/scripts/4_World"};
            };
        };
    };
};
