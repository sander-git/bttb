#pragma once

#include <gtk/gtk.h>
#include "bttb_logic.hpp"

namespace bttb {

class VerifyDialog {
public:
    static void run(GtkWindow* parent);
};

} // namespace bttb
