#pragma once

#include <gtk/gtk.h>
#include "bttb_logic.hpp"

namespace bttb {

class IsoDialog {
public:
    static void run(GtkWindow* parent, const std::string& defaultSourceDir);
};

} // namespace bttb
