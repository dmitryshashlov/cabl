/*----------------------------------------------------------------------------------------------------------------------   

                 %%%%%%%%%%%%%%%%%                
                 %%%%%%%%%%%%%%%%%
                 %%%           %%%
                 %%%           %%%
                 %%%           %%%
%%%%%%%%%%%%%%%%%%%%           %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%           %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% www.shaduzlabs.com %%%%%

------------------------------------------------------------------------------------------------------------------------

  Copyright (C) 2014 Vincenzo Pacella

  This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public 
  License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later
  version.

  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied 
  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with this program.  
  If not, see <http://www.gnu.org/licenses/>.

----------------------------------------------------------------------------------------------------------------------*/

#include "comm/Driver.h"
#include "comm/DriverImpl.h"
#include "comm/DeviceHandle.h"

#include "comm/drivers/Probe/DriverProbe.h"

#if defined (__SAM3X8E__)
#include "comm/drivers/SAM3X8E/DriverSAM3X8E.h"
#elif defined (__MAX3421E__)
#include "comm/drivers/MAX3421E/DriverMAX3421E.h"
#else
#include "comm/drivers/HIDAPI/DriverHIDAPI.h"
#include "comm/drivers/LibUSB/DriverLibUSB.h"
#endif

namespace sl
{
namespace kio
{

Driver::Driver( Type type_ )
{
  switch( type_ )
  {
#if defined (__SAM3X8E__)
    case Type::SAM3X8E:
      m_pImpl.reset( new DriverSAM3X8E );
      break;
#elif defined (__MAX3421E__)
    case Type::MAX3421E:
      m_pImpl.reset( new DriverMAX3421E );
      break;
#else
    case Type::HIDAPI:
      m_pImpl.reset( new DriverHIDAPI );
      break;
    case Type::LibUSB:
      m_pImpl.reset( new DriverLibUSB );
      break;
#endif
    case Type::Probe:
    default:
      m_pImpl.reset( new DriverProbe );
      break;
    }
}
  
//----------------------------------------------------------------------------------------------------------------------
  
Driver::~Driver()
{

}
  
//----------------------------------------------------------------------------------------------------------------------
  
Driver::tCollDeviceDescriptor Driver::enumerate()
{
  return m_pImpl->enumerate();
}
  
//----------------------------------------------------------------------------------------------------------------------
  
tPtr<DeviceHandle> Driver::connect( const DeviceDescriptor& device_  )
{
  return tPtr<DeviceHandle>( new DeviceHandle(m_pImpl->connect( device_ )));
}

//----------------------------------------------------------------------------------------------------------------------

} // kio
} // sl
