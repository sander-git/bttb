#pragma once

#include <gtk/gtk.h>
#include "bttb_logic.hpp"

namespace bttb {

class SettingsDialog {
public:
    static void run(GtkWindow* parent, BttbSolver& solver);
};

} // namespace bttb
