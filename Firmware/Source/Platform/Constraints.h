#ifndef __CONSTRAINTS_H
#define __CONSTRAINTS_H

// Include
#include "stdinc.h"
#include "DataTable.h"
#include "Global.h"
#include "Keithley6485.h"

//Definitions
//
#define SFTST_R_MIN						47
#define SFTST_R_MAX						1000
#define SFTST_R_SHUNT_DEF				82
#define SFTST_R_KEI_DEF					82
//
#define SFTST_I_MIN						0
#define SFTST_I_MAX						30
#define SFTST_MUX_OFF_THRE_DEF			11
#define SFTST_MUX_IGTU_THRE_DEF			0.1
#define SFTST_MUX_LCTU_THRE_DEF			11
#define SFTST_MUX_LCTU_DIV_THRE_DEF		5
//
#define SFTST_I_ALOWED_ERR_MIN			0
#define SFTST_I_ALOWED_ERR_MAX			20
#define SFTST_I_ALOWED_ERR_DEF			20
//
#define I_DIV_FACTOR_MIN				1
#define I_DIV_FACTOR_MAX				3
#define I_DIV_FACTOR_DEF				2
//
#define CHANNEL_LCTU					1
#define CHANNEL_IGTU					2
//
#define RANGE_MIN						2e-6
#define RANGE_MAX						30
#define RANGE_DEF						30
//
#define NPLC_DEF						1
//
#define SYNC_WAIT_TIMEOUT_MIN			500
#define SYNC_WAIT_TIMEOUT_MAX			5000
#define SYNC_WAIT_TIMEOUT_DEF			2000
//
#define NO								0
#define YES								1
//
#define SAMPLES_NUMBER_MIN				1
#define SAMPLES_NUMBER_MAX				2500
#define SAMPLES_NUMBER_DEF				1

// Types
typedef struct __TableItemConstraint
{
	float Min;
	float Max;
	float Default;
} TableItemConstraint;

// Variables
extern const TableItemConstraint Constraint[];

#endif // __CONSTRAINTS_H
