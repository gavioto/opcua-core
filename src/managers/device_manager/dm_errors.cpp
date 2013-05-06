/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Common errors definitions
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/managers/device_manager/errors.h>
#include <opc/common/modules.h>

//TODO fix MODULE Code
#define GPS_DEVICE_IO_MANAGER_ERROR_CODE(code) ERROR_CODE(Common::MODULE_GPS_DEVICE_IO_MANAGER, code)
#define GPS_DEVICE_IO_MANAGER_ERROR(name, code, message) Common::ErrorData name(GPS_DEVICE_IO_MANAGER_ERROR_CODE(code), message)

GPS_DEVICE_IO_MANAGER_ERROR(DeviceManagerNotProcessItemWithID,    1, "Device Manager doesn't processes item with ID '%1%'.");

