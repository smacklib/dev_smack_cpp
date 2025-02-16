/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Localisation support.
 *
 * Copyright � 2025 Michael Binz
 */

#include "smack_locale.h"

namespace smack::localisation {

std::unique_ptr<smack::localisation::Locale> Locale::s_current;

std::mutex Locale::s_mutex;

} // namespace smack::localisation
