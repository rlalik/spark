/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#ifdef __CLING__

// clang-format off

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclasses;
#pragma link C++ nestedtypedefs;

#pragma link C++ class spark::details::category_internals+;
#pragma link C++ class spark::category+;

// database and parameters
#pragma link C++ class spark::validity_range_t+;
#pragma link C++ class spark::container+;

// clang-format on

#endif
