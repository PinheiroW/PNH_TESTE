modded class GM_SystemData
{
    const vector SYSTEM_CHARACTER_POS = Vector(15185.16, -1, 15098.91); // техническая точка появления персонажа (если у вас карта не Черно, то можете изменить на любую свою (где-нибудь в лесу)).

    static int GetDeseasesByText(string diseaseName)
    {
        int value = -1;

        if (diseaseName == "")
            return value;

        switch (diseaseName)
        {
            case "INFLUENZA":
            {
                value = eAgents.INFLUENZA;
                break;
            }
            case "CHOLERA":
            {
                value = eAgents.CHOLERA;
                break;
            }
            case "SALMONELLA":
            {
                value = eAgents.SALMONELLA;
                break;
            }
            case "FOOD_POISON":
            {
                value = eAgents.FOOD_POISON;
                break;
            }
            case "WOUND_AGENT":
            {
                value = eAgents.WOUND_AGENT;
                break;
            }
            case "BRAIN":
            {
                value = eAgents.BRAIN;
                break;
            }
            case "CHEMICAL_POISON":
            {
                value = eAgents.CHEMICAL_POISON;
                break;
            }
        }

        return value;
    }
}