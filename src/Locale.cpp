/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Localisation support.
 *
 * Copyright © 2025 Michael Binz
 */

#include "Locale.h"

namespace smack::localisation {

std::unique_ptr<Locale> Locale::s_current;

std::mutex Locale::s_mutex;

} // namespace smack::localisation
