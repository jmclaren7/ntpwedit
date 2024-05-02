/* ===================================================================
 * Copyright (c) 2005-2017 Vadim Druzhin (cdslow@mail.ru).
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 * ===================================================================
 */

#define STRICT
#include <windows.h>
#include "version.h"

WCHAR const *AppTitle=
    L"NTPWEdit 0.7"
#ifdef _WIN64
    L" (64-bit)"
#endif /* _WIN64 */
    ;

WCHAR const *AppAuthor = L"\xA9 2005-2017 Vadim Druzhin";
