// Header
#include "Diagnostic.h"

// Includes
#include "DataTable.h"
#include "LowLevel.h"
#include "Controller.h"
#include "DebugActions.h"

// Functions
bool DIAG_HandleDiagnosticAction(uint16_t ActionID, uint16_t *pUserError)
{
	switch (ActionID)
	{
	case ACT_DBG_EXTERNAL_LED:
		DBGACT_ToggleExtLED();
		break;

	case ACT_DBG_ST_CURRENT:
		DBACT_SelfTestCurrent();
		break;

	case ACT_DBG_ST_CH_IGTU:
		DBACT_SelfTestChannel_IGTU();
		break;

	case ACT_DBG_SWITCH_AMUX_IGTU:
		DBACT_SwitchMuxToIGTU();
		break;

	case ACT_DBG_SWITCH_AMUX_LCTU:
		DBACT_SwitchMuxToLCTU();
		break;

	case ACT_DBG_SWITCH_SMUX_IGTU:
		DBACT_SwitchSyncToIGTU();
		break;

	case ACT_DBG_SWITCH_SMUX_LCTU:
		DBACT_SwitchSyncToLCTU();
		break;

	case ACT_DBG_CURRENT_DIVIDER:
		DBACT_SetStateCurrentDivider();
		break;

	case ACT_DBG_GENERATE_SYNC_KEI:
		DBACT_GenerateSyncToKeithley();
		break;

	case ACT_DBG_SEND_DATA_TO_KEI:
		DBACT_SendDataToKeithley();
		break;

		default:
			return false;
	}

	return true;
}
//-------------------------------------
