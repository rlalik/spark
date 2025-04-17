// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#ifdef __CLING__

// clang-format off

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclasses;
#pragma link C++ nestedtypedefs;

// #pragma link C++ class spark::spark;
#pragma link C++ class spark::details::category_internals+;
#pragma link C++ class spark::category+;
// databases

// database and parameters
#pragma link C++ class spark::validity_range_t+;
#pragma link C++ class spark::container+;

// #pragma link C++ ioctortype spark::lookup_cell;

#pragma link C++ class spark::lookup_cell+;
#pragma link C++ class spark::lookup_channel+;
#pragma link C++ class spark::lookup_address+;
#pragma link C++ class spark::lookup_table+;

// clang-format on

#endif
