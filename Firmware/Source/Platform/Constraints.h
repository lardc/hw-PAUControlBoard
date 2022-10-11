#ifndef __CONSTRAINTS_H
#define __CONSTRAINTS_H

// Include
#include "stdinc.h"
#include "DataTable.h"
#include "Global.h"

//Definitions
//
#define COEF_K_MIN					-INT16S_MAX
#define COEF_K_MAX					INT16S_MAX
#define COEF_K_DEF					1
//
#define OFFSET_MIN					-INT16S_MAX
#define OFFSET_MAX					INT16S_MAX
#define OFFSET_DEF					0
//
#define COEF_P2_MIN					-INT16S_MAX
#define COEF_P2_MAX					INT16S_MAX
#define COEF_P2_DEF					0
//
#define COEF_P1_MIN					-INT16S_MAX
#define COEF_P1_MAX					INT16S_MAX
#define COEF_P1_DEF					1
//
#define COEF_P0_MIN					-INT16S_MAX
#define COEF_P0_MAX					INT16U_MAX
#define COEF_P0_DEF					0
//
#define NO							0
#define YES							1
//

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
