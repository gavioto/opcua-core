/******************************************************************************
 *   Copyright (C) 2014-2014 Olivier Roulet-Dubonnet          *
 *   olivier.roulet@gmail.com          *
 *                      *
 *   This library is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU Lesser General Public License as      *
 *   published by the Free Software Foundation; version 3 of the License.   *
 *                      *
 *   This library is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      *
 *   GNU Lesser General Public License for more details.        *
 *                      *
 *   You should have received a copy of the GNU Lesser General Public License *
 *   along with this library; if not, write to the          *
 *   Free Software Foundation, Inc.,              *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.        *
 ******************************************************************************/



#include <opc/common/node.h>


namespace OpcUa
{
  Variant Node::Read(OpcUa::AttributeID attr)
  {
    ReadParameters params;
    AttributeValueID attribute;
    attribute.Node = this->NodeId;
    attribute.Attribute = attr;
    params.AttributesToRead.push_back(attribute);
    DataValue dv =  server->Attributes()->Read(params).front();
    return dv.Value;
  }

  Variant Node::ReadValue()
  {
    return Read(OpcUa::AttributeID::VALUE);
  }

  Variant Node::ReadDataType()
  {
    return Read(OpcUa::AttributeID::DATA_TYPE);
  }

  std::vector<StatusCode> Node::Write(OpcUa::AttributeID const attr, Variant value)
  {
    OpcUa::WriteValue attribute;
    attribute.Node = NodeId;
    attribute.Attribute = attr;
    attribute.Data = value;
    return server->Attributes()->Write(std::vector<OpcUa::WriteValue>(1, attribute));
  }


  std::vector<Node> Node::Browse(OpcUa::ReferenceID refid)
  {
    OpcUa::BrowseDescription description;
    description.NodeToBrowse = this->NodeId;
    description.Direction = OpcUa::BrowseDirection::Forward;
    description.IncludeSubtypes = true;
    description.NodeClasses = OpcUa::NODE_CLASS_ALL;
    description.ResultMask = OpcUa::REFERENCE_ALL;
    description.ReferenceTypeID =  refid;

    OpcUa::NodesQuery query;
    query.NodesToBrowse.push_back(description);
    query.MaxReferenciesPerNode = 100;
    std::vector<Node> nodes;
    std::vector<OpcUa::ReferenceDescription> refs = server->Views()->Browse(query);
    while(true)
    {
      if (refs.empty())
      {
      break;
      }
      for (auto refIt : refs)
      {
        Node node(server, refIt.TargetNodeID);
        std::cout << "Creating node with borwsename: " << refIt.BrowseName.NamespaceIndex << refIt.BrowseName.Name << std::endl;
        node.SetBrowseNameCache(refIt.BrowseName);
        nodes.push_back(node);
      }
      refs = server->Views()->BrowseNext();
    }
    return nodes;
  }


  Node Node::FindChildNode(std::string browsename, ushort ns)
  {
    QualifiedName qn(ns, browsename);
    return FindChildNode(qn);
  }

  Node Node::FindChildNode(QualifiedName browsename)
  {
    //FIXME: Should be implemented at lower level using filter browsing and breaking browsing
    for (Node node : Browse())
    {
      std::cout << "Comparing: " <<node.GetBrowseName().Name << " to " << browsename.Name << std::endl;
      if (node.GetBrowseName() == browsename)
      {
        return node;
      }
    }
    return Node(server); //Null node
  }

  std::string Node::ToString() const
  {
    std::ostringstream os;
    os << "Node(" << browseName.NamespaceIndex <<":"<< browseName.Name << ", id=" ;
    OpcUa::NodeIDEncoding encoding = static_cast<OpcUa::NodeIDEncoding>(NodeId.Encoding & OpcUa::NodeIDEncoding::EV_VALUE_MASK);

    switch (encoding)
    {
      case OpcUa::NodeIDEncoding::EV_TWO_BYTE:
      {
        os << (unsigned)NodeId.TwoByteData.Identifier ;
        break;
      }

      case OpcUa::NodeIDEncoding::EV_FOUR_BYTE:
      {
        os << (unsigned)NodeId.FourByteData.NamespaceIndex << ":" << (unsigned)NodeId.FourByteData.Identifier ;
        break;
      }

      case OpcUa::NodeIDEncoding::EV_NUMERIC:
      {
        os << (unsigned)NodeId.NumericData.NamespaceIndex << ":" << (unsigned)NodeId.NumericData.Identifier ;
        break;
      }

      case OpcUa::NodeIDEncoding::EV_STRING:
      {
        os << (unsigned)NodeId.StringData.NamespaceIndex << ":" << NodeId.StringData.Identifier << std::endl;
        break;
      }

      case OpcUa::NodeIDEncoding::EV_BYTE_STRING:
      {
        os << (unsigned)NodeId.BinaryData.NamespaceIndex ; //<< for (auto val : NodeId.BinaryData.Identifier) {std::cout << (unsigned)val; }
        //std::cout << std::endl;
        break;
      }

      case OpcUa::NodeIDEncoding::EV_GUID:
      {
        os << "Guid: " << (unsigned)NodeId.GuidData.NamespaceIndex ;
        const OpcUa::Guid& guid = NodeId.GuidData.Identifier;
        os << ":" << std::hex << guid.Data1 << "-" << guid.Data2 << "-" << guid.Data3;
        for (auto val : guid.Data4) {os << (unsigned)val; }
        break;
      }
      default:
      {
        os << "unknown id type:" << (unsigned)encoding ;
        break;
      }
    }
    os << ")";
    return os.str();
  }

}
