/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief View services interface.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef OPC_UA_CLIENT_ADDRESSSPACE_H
#define OPC_UA_CLIENT_ADDRESSSPACE_H

#include <opc/common/interface.h>
#include <opc/common/class_pointers.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/view.h>
#include <opc/ua/protocol/attribute.h>

#include <vector>

namespace OpcUa
{
  namespace Remote
  {
    //Fixme: calld nodemanagement in spec, but addressapce naming is used everywhere in this implementation
    class AddressSpaceServices : private Common::Interface
    {
    public:
      DEFINE_CLASS_POINTERS(AddressSpaceServices);

    public:
      virtual void AddAttribute(const NodeID& node, AttributeID attribute, const Variant& value) = 0;
      virtual void AddReference(const NodeID& sourceNode, const ReferenceDescription& reference) = 0;
    };

  } // namespace Remote
} // namespace OpcUa

#endif // OPC_UA_CLIENT_ADDRESSSPACE_H

