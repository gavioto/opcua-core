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



#include <opc/ua/node.h>


namespace OpcUa
{
  Variant Node::Read(const OpcUa::AttributeID attr)
  {
    ReadParameters params;
    AttributeValueID attribute;
    attribute.Node = this->NodeId;
    attribute.Attribute = attr;
    params.AttributesToRead.push_back(attribute);
    DataValue dv =  server->Attributes()->Read(params).front();
    return dv.Value;
  }

  StatusCode Node::Write(const OpcUa::AttributeID attr, const Variant &value)
  {
    OpcUa::WriteValue attribute;
    attribute.Node = NodeId;
    attribute.Attribute = attr;
    attribute.Data = value;
    std::vector<StatusCode> codes = server->Attributes()->Write(std::vector<OpcUa::WriteValue>(1, attribute));
    return codes.front();
  }

  StatusCode Node::WriteValue(const Variant &value)
  {
    return Write(OpcUa::AttributeID::VALUE, value);
  }

  std::vector<Node> Node::Browse(const OpcUa::ReferenceID refid)
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
        //std::cout << "Creating node with borwsename: " << refIt.BrowseName.NamespaceIndex << refIt.BrowseName.Name << std::endl;
        node.SetBrowseNameCache(refIt.BrowseName);
        nodes.push_back(node);
      }
      refs = server->Views()->BrowseNext();
    }
    return nodes;
  }

  Node Node::GetChildNode(ushort ns, const std::string& browsename)
  {
    QualifiedName qn(ns, browsename);
    return GetChildNode(qn);
  }

  Node Node::GetChildNode(const QualifiedName& browsename)
  {
    std::vector<QualifiedName> path;
    path.push_back(browsename);
    return GetChildNode(path);
  }

 Node Node::GetChildNode(const std::vector<std::string>& path)
  {
    std::vector<QualifiedName> vec;
    ushort tmp_ns = this->browseName.NamespaceIndex;
    for (std::string str: path)
    {
      QualifiedName qname = ParseQualifiedNameFromString(str, tmp_ns);
      tmp_ns = qname.NamespaceIndex;
      vec.push_back(qname);
    }
    return GetChildNode(vec);
  }

 QualifiedName Node::ParseQualifiedNameFromString(const std::string& str, ushort default_ns)
 {
   std::size_t found = str.find(":");
   if (found != std::string::npos)
   {
     ushort ns = std::stoi(str.substr(0, found));
     std::string name = str.substr(found+1, str.length() - found);
     return QualifiedName(ns, name);
   }
   else
   {
     return QualifiedName(default_ns, str);
   }
 }

  Node Node::GetChildNode(const std::vector<QualifiedName>& path)
  {
    std::vector<RelativePathElement> rpath;
    for (QualifiedName qname: path)
    {
      RelativePathElement el;
      el.TargetName = qname;
      rpath.push_back(el);
    }
    BrowsePath bpath;
    bpath.Path.Elements = rpath;
    bpath.StartingNode = this->NodeId;
    std::vector<BrowsePath> bpaths;
    bpaths.push_back(bpath);
    TranslateBrowsePathsParameters params;
    params.BrowsePaths = bpaths;

    std::vector<BrowsePathResult> result = server->Views()->TranslateBrowsePathsToNodeIds(params);

    if ( result.front().Status == OpcUa::StatusCode::Good )
    {
      NodeID node =result.front().Targets.front().Node ;
      return Node(server, node);
    }
    else
    {
      return Node(server); //Null node
    }
  }

  std::string Node::ToString() const
  {
    if (this->mIsNull) { return "Node(*null)"; }

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

  Node Node::AddFolderNode(uint16_t ns, uint32_t id, std::string name)
  {
    NodeID nodeid = OpcUa::NumericNodeID(id, ns);
    server->AddressSpace()->AddAttribute(nodeid, AttributeID::NODE_ID,      NodeID(ObjectID::ObjectsFolder));
    server->AddressSpace()->AddAttribute(nodeid, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Object));
    server->AddressSpace()->AddAttribute(nodeid, AttributeID::BROWSE_NAME,  QualifiedName(0, name));
    server->AddressSpace()->AddAttribute(nodeid, AttributeID::DISPLAY_NAME, LocalizedText(name));
    server->AddressSpace()->AddAttribute(nodeid, AttributeID::DESCRIPTION,  LocalizedText(name));
    server->AddressSpace()->AddAttribute(nodeid, AttributeID::WRITE_MASK,   0);
    server->AddressSpace()->AddAttribute(nodeid, AttributeID::USER_WRITE_MASK, 0);
    server->AddressSpace()->AddAttribute(nodeid, AttributeID::EVENT_NOTIFIER, (uint8_t)0);
    
    ReferenceDescription desc;
    desc.ReferenceTypeID = ReferenceID::HasTypeDefinition;
    desc.IsForward = true;
    desc.TargetNodeID = NodeID(ObjectID::FolderType);
    desc.BrowseName = QualifiedName(ns, name);
    desc.DisplayName = LocalizedText(name);
    desc.TargetNodeClass = NodeClass::ObjectType;
    desc.TargetNodeTypeDefinition = ObjectID::Null;
    server->AddressSpace()->AddReference(nodeid, desc);

    //AddReference(ObjectID::RootFolder,  forward, ReferenceID::Organizes, ObjectID::ObjectsFolder, Names::Objects,    NodeClass::Object,     ObjectID::FolderType);
    desc.ReferenceTypeID = ReferenceID::Organizes;
    desc.IsForward = true;
    desc.TargetNodeID = nodeid;
    desc.BrowseName = QualifiedName(ns, name);
    desc.DisplayName = LocalizedText(name);
    desc.TargetNodeClass = NodeClass::Object;
    desc.TargetNodeTypeDefinition = ObjectID::FolderType;
    server->AddressSpace()->AddReference(this->NodeId, desc);

    Node node(server, nodeid);
    node.SetBrowseNameCache(desc.BrowseName);
    return node;
 
  }

}
