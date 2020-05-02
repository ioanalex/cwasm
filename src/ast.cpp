#include "ast.hpp"
// TODO: this file is not needed remove it when all is done
u32 pos;

static const char *section_names[] = {
    "Custom",    // 0
    "Type",      // 1
    "Import",    // 2
    "Function",  // 3
    "Table",     // 4
    "Memory",    // 5
    "Global",    // 6 !
    "Export",    // 7
    "Start",     // 8
    "Element",   // 9 !
    "Code",      // 10 !
    "Data"       // 11 !
};
static bool sections[] = {
    false,  // Custom
    false,  // Type
    false,  // Import
    false,  // Function
    false,  // Table
    false,  // Memory
    false,  // Global
    false,  // Export
    false,  // Start
    false,  // Element
    false,  // Code
    false   // Data
};
