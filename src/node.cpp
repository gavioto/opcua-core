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
#include <opc/ua/strings.h>
#include <opc/ua/variable_access_level.h>


namespace OpcUa
{

  Node::Node(Remote::Server& srv)
    : Node(srv, ObjectID::RootFolder)
  {
    BrowseName = GetName();
  }

  Node::Node(Remote::Server& srv, const NodeID& id)
    : Server(srv)
    , Id(id)
    , BrowseName(GetName())
  {
  }


  Node::Node(Remote::Server& srv, const NodeID& id, const QualifiedName& name)
    : Server(srv)
    , Id(id)
    , BrowseName(name)
  {
  }

  NodeID Node::GetId() const
  {
    return Id;
  }

  Variant Node::GetAttribute(const OpcUa::AttributeID attr) const
  {
    ReadParameters params;
    AttributeValueID attribute;
    attribute.Node = Id;
    attribute.Attribute = attr;
    params.AttributesToRead.push_back(attribute);
    DataValue dv =  Server.Attributes()->Read(params).front(); // TODO: Bug! result vector can be empty.
    return dv.Value;
  }

  StatusCode Node::SetAttribute(const OpcUa::AttributeID attr, const Variant &value)
  {
    OpcUa::WriteValue attribute;
    attribute.Node = Id;
    attribute.Attribute = attr;
    attribute.Data = value;
    std::vector<StatusCode> codes = Server.Attributes()->Write(std::vector<OpcUa::WriteValue>(1, attribute));
    return codes.front();
  }

  StatusCode Node::SetValue(const Variant& value)
  {
    return SetAttribute(OpcUa::AttributeID::VALUE, value);
  }

  std::vector<Node> Node::GetChildren(const OpcUa::ReferenceID refid) const
  {
    OpcUa::BrowseDescription description;
    description.NodeToBrowse = Id;
    description.Direction = OpcUa::BrowseDirection::Forward;
    description.IncludeSubtypes = true;
    description.NodeClasses = OpcUa::NODE_CLASS_ALL;
    description.ResultMask = OpcUa::REFERENCE_ALL;
    description.ReferenceTypeID =  refid;

    OpcUa::NodesQuery query;
    query.NodesToBrowse.push_back(description);
    query.MaxReferenciesPerNode = 100;
    std::vector<Node> nodes;
    std::vector<OpcUa::ReferenceDescription> refs = Server.Views()->Browse(query);
    while(!refs.empty())
    {
      for (auto refIt : refs)
      {
        Node node(Server, refIt.TargetNodeID);
        //std::cout << "Creating node with borwsename: " << refIt.BrowseName.NamespaceIndex << refIt.BrowseName.Name << std::endl;
        nodes.push_back(node);
      }
      refs = Server.Views()->BrowseNext();
    }
    return nodes;
  }

  std::vector<Node> Node::GetChildren() const
  {
    return GetChildren(ReferenceID::HierarchicalReferences);
  }

  QualifiedName Node::GetName() const
  {
    Variant var = GetAttribute(OpcUa::AttributeID::BROWSE_NAME);
    if (var.Type == OpcUa::VariantType::QUALIFIED_NAME)
    {
      return var.Value.Name.front();
    }

    return QualifiedName(); // TODO Exception!
  }

  Node Node::GetChild(const std::string& browsename) const
  {
    return GetChild(std::vector<std::string>({browsename}));
  }

  void Node::AddAttribute(OpcUa::AttributeID attr, const OpcUa::Variant& val)
  {
    return Server.AddressSpace()->AddAttribute(Id, attr, val);
  }

  void Node::AddReference(const OpcUa::ReferenceDescription desc)
  {
    return Server.AddressSpace()->AddReference(Id, desc);
  }

  Node Node::GetChild(const std::vector<std::string>& path) const
  {
    std::vector<QualifiedName> vec;
    uint16_t ns = BrowseName.NamespaceIndex;
    for (std::string str: path)
    {
      QualifiedName qname = ParseQualifiedNameFromString(ns, str);
      ns = qname.NamespaceIndex;
      vec.push_back(qname);
    }
    return GetChild(vec);
  }


  Node Node::GetChild(const std::vector<QualifiedName>& path) const
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
    bpath.StartingNode = Id;
    std::vector<BrowsePath> bpaths;
    bpaths.push_back(bpath);
    TranslateBrowsePathsParameters params;
    params.BrowsePaths = bpaths;

    std::vector<BrowsePathResult> result = Server.Views()->TranslateBrowsePathsToNodeIds(params);

    if ( result.front().Status == OpcUa::StatusCode::Good )
    {
      NodeID node =result.front().Targets.front().Node ;
      return Node(Server, node);
    }
    else
    {
      return Node(Server); //Null node
    }
  }

  // TODO: move to somewhere
  std::string Node::ToString() const
  {
    std::ostringstream os;
    os << "Node(" << BrowseName.NamespaceIndex <<":"<< BrowseName.Name << ", id=" ;
    OpcUa::NodeIDEncoding encoding = static_cast<OpcUa::NodeIDEncoding>(Id.Encoding & OpcUa::NodeIDEncoding::EV_VALUE_MASK);

    if (encoding != EV_TWO_BYTE)
    {
      os << Id.GetNamespaceIndex() << ":";
    }

    switch (encoding)
    {
      case OpcUa::NodeIDEncoding::EV_FOUR_BYTE:
      case OpcUa::NodeIDEncoding::EV_NUMERIC:
      case OpcUa::NodeIDEncoding::EV_TWO_BYTE:
      {
        os << Id.GetIntegerIdentifier();
        break;
      }

      case OpcUa::NodeIDEncoding::EV_STRING:
      {
        os << Id.GetStringIdentifier();
        break;
      }

      case OpcUa::NodeIDEncoding::EV_BYTE_STRING:
      {
        os << std::hex;
        for (auto val : Id.BinaryData.Identifier)
          os << (unsigned)val;

        break;
      }

      case OpcUa::NodeIDEncoding::EV_GUID:
      {
        const OpcUa::Guid& guid = Id.GuidData.Identifier;
        os << std::hex << guid.Data1 << "-" << guid.Data2 << "-" << guid.Data3;
        for (auto val : guid.Data4) {os << (unsigned)val; }
        break;
      }
      default:
      {
        os << "unknown id format:" << (unsigned)encoding ;
        break;
      }
    }
    os << ")";
    return os.str();
  }

  Node Node::AddFolder(const std::string& name)
  {
    NodeID nodeid = ParseNodeIdFromString(Id.GetNamespaceIndex(), name);
    QualifiedName qn = ParseQualifiedNameFromString(BrowseName.NamespaceIndex, name);
    return AddFolder(nodeid, qn);
  }

  Node Node::AddFolder(const NodeID& nodeid, const QualifiedName& browsename)
  {
    Server.AddressSpace()->AddAttribute(nodeid, AttributeID::NODE_ID,      nodeid);
    Server.AddressSpace()->AddAttribute(nodeid, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Object));
    Server.AddressSpace()->AddAttribute(nodeid, AttributeID::BROWSE_NAME,  browsename);
    Server.AddressSpace()->AddAttribute(nodeid, AttributeID::DISPLAY_NAME, LocalizedText(browsename.Name));
    Server.AddressSpace()->AddAttribute(nodeid, AttributeID::DESCRIPTION,  LocalizedText(browsename.Name));
    Server.AddressSpace()->AddAttribute(nodeid, AttributeID::WRITE_MASK,   0);
    Server.AddressSpace()->AddAttribute(nodeid, AttributeID::USER_WRITE_MASK, 0);
    Server.AddressSpace()->AddAttribute(nodeid, AttributeID::EVENT_NOTIFIER, (uint8_t)0);
    
   //Set expected type definition 
    ReferenceDescription desc;
    desc.ReferenceTypeID = ReferenceID::HasTypeDefinition;
    desc.IsForward = true;
    desc.TargetNodeID = NodeID(ObjectID::FolderType);
    desc.BrowseName = QualifiedName(Names::FolderType);
    desc.DisplayName = LocalizedText(Names::FolderType);
    desc.TargetNodeClass = NodeClass::ObjectType;
    desc.TargetNodeTypeDefinition = ObjectID::Null;

    Server.AddressSpace()->AddReference(nodeid, desc);

    //Link to parent(myself)
    desc.ReferenceTypeID = ReferenceID::Organizes;
    desc.TargetNodeID = nodeid;
    desc.TargetNodeClass = NodeClass::Object;
    desc.TargetNodeTypeDefinition = ObjectID::FolderType;
    desc.BrowseName = browsename;
    desc.DisplayName = LocalizedText(browsename.Name);

    Server.AddressSpace()->AddReference(Id, desc);
    return Node(Server, nodeid, desc.BrowseName);
  }

  Node Node::AddVariable(const std::string& name, const Variant& val)
  {
    NodeID nodeid = ParseNodeIdFromString(Id.GetNamespaceIndex(), name);
    QualifiedName qn = ParseQualifiedNameFromString(BrowseName.NamespaceIndex, name);
    return AddVariable(nodeid, qn, val);
  }

  Node Node::AddVariable(const NodeID& nodeid, const QualifiedName& browsename, const Variant& val)
  {
    ObjectID datatype = VariantTypeToDataType(val.Type);
    //Base attributes
    Server.AddressSpace()->AddAttribute(nodeid, AttributeID::NODE_ID,      nodeid);
    Server.AddressSpace()->AddAttribute(nodeid, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
    Server.AddressSpace()->AddAttribute(nodeid, AttributeID::BROWSE_NAME,  browsename);
    Server.AddressSpace()->AddAttribute(nodeid, AttributeID::DISPLAY_NAME, LocalizedText(browsename.Name));
    Server.AddressSpace()->AddAttribute(nodeid, AttributeID::DESCRIPTION,  LocalizedText(browsename.Name));
    Server.AddressSpace()->AddAttribute(nodeid, AttributeID::WRITE_MASK,   0);
    Server.AddressSpace()->AddAttribute(nodeid, AttributeID::USER_WRITE_MASK, 0);
    Server.AddressSpace()->AddAttribute(nodeid, AttributeID::EVENT_NOTIFIER, (uint8_t)0);
    //Variable Attributes
    Server.AddressSpace()->AddAttribute(nodeid, AttributeID::VALUE, val);
    Server.AddressSpace()->AddAttribute(nodeid, AttributeID::DATA_TYPE, datatype);
    Server.AddressSpace()->AddAttribute(nodeid, AttributeID::ARRAY_DIMENSIONS, val.Dimensions.size()); //FIXME: to check!!
    //Server.AddressSpace()->AddAttribute(nodeid, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead|VariableAccessLevel::CurrentWrite));
    //Server.AddressSpace()->AddAttribute(nodeid, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
    Server.AddressSpace()->AddAttribute(nodeid, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
    Server.AddressSpace()->AddAttribute(nodeid, AttributeID::HISTORIZING, false);
    Server.AddressSpace()->AddAttribute(nodeid, AttributeID::VALUE_RANK, ~int32_t());
   
    //set up expected type definition  
    ReferenceDescription desc;
    desc.ReferenceTypeID = ReferenceID::HasTypeDefinition;
    desc.IsForward = true;
    desc.TargetNodeID = NodeID(ObjectID::BaseDataVariableType);
    desc.BrowseName = QualifiedName(Names::BaseDataVariableType);
    desc.DisplayName = LocalizedText(Names::BaseDataVariableType);
    desc.TargetNodeClass = NodeClass::DataType;
    desc.TargetNodeTypeDefinition = ObjectID::Null;

    Server.AddressSpace()->AddReference(nodeid, desc);
    
    //link it as child of correct type
    desc.ReferenceTypeID = ReferenceID::HasComponent;
    desc.TargetNodeID = nodeid;
    desc.TargetNodeClass = NodeClass::Variable;
    desc.BrowseName = browsename;
    desc.DisplayName = LocalizedText(browsename.Name);
    desc.TargetNodeTypeDefinition = ObjectID::BaseDataVariableType;

    Server.AddressSpace()->AddReference(Id, desc);

    return Node(Server, nodeid, desc.BrowseName);
  }


  Node Node::AddProperty(const std::string& name, const Variant& val)
  {
    const NodeID& nodeid = ParseNodeIdFromString(Id.GetNamespaceIndex(), name);
    const QualifiedName& qname = ParseQualifiedNameFromString(BrowseName.NamespaceIndex, name);
    return AddProperty(nodeid, qname, val);
  }

  Node Node::AddProperty(const NodeID& propertyId, const QualifiedName& browsename, const Variant& val)
  {

    ObjectID datatype = VariantTypeToDataType(val.Type);

    Server.AddressSpace()->AddAttribute(propertyId, AttributeID::NODE_ID,      propertyId);
    Server.AddressSpace()->AddAttribute(propertyId, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
    Server.AddressSpace()->AddAttribute(propertyId, AttributeID::BROWSE_NAME,  browsename);
    Server.AddressSpace()->AddAttribute(propertyId, AttributeID::DISPLAY_NAME, LocalizedText(browsename.Name));
    Server.AddressSpace()->AddAttribute(propertyId, AttributeID::DESCRIPTION,  LocalizedText(browsename.Name));
    Server.AddressSpace()->AddAttribute(propertyId, AttributeID::WRITE_MASK,   0);
    Server.AddressSpace()->AddAttribute(propertyId, AttributeID::USER_WRITE_MASK, 0);
    Server.AddressSpace()->AddAttribute(propertyId, AttributeID::EVENT_NOTIFIER, (uint8_t)0);
    Server.AddressSpace()->AddAttribute(propertyId, AttributeID::HISTORIZING, false);
    Server.AddressSpace()->AddAttribute(propertyId, AttributeID::VALUE_RANK, int32_t(-1));
    Server.AddressSpace()->AddAttribute(propertyId, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
    Server.AddressSpace()->AddAttribute(propertyId, AttributeID::DATA_TYPE, datatype);
    Server.AddressSpace()->AddAttribute(propertyId, AttributeID::VALUE, val);
    Server.AddressSpace()->AddAttribute(propertyId, AttributeID::ARRAY_DIMENSIONS, val.Dimensions.size()); //FIXME: to check!!
   
    //set up expected type definition  
    ReferenceDescription desc;
    desc.ReferenceTypeID = ReferenceID::HasTypeDefinition;
    desc.IsForward = true;
    desc.TargetNodeID = NodeID(ObjectID::PropertyType);
    desc.TargetNodeClass = NodeClass::DataType;
    desc.BrowseName = QualifiedName(Names::PropertyType);
    desc.DisplayName = LocalizedText(Names::PropertyType);
    desc.TargetNodeTypeDefinition = ObjectID::Null;

    Server.AddressSpace()->AddReference(propertyId, desc);
    
    //link it as child of correct type
    desc.ReferenceTypeID = ReferenceID::HasProperty; //This should be the correct type
    //desc.ReferenceTypeID = ReferenceID::HasComponent;
    desc.TargetNodeID = propertyId;
    desc.TargetNodeClass = NodeClass::Variable;
    desc.BrowseName = browsename;
    desc.DisplayName = LocalizedText(browsename.Name);
    desc.TargetNodeTypeDefinition = ObjectID::PropertyType;

    Server.AddressSpace()->AddReference(Id, desc);

    return Node(Server, propertyId, desc.BrowseName);
  }

  Variant Node::GetValue() const
  {
    return GetAttribute(AttributeID::VALUE);
  }
  Variant Node::DataType() const
  {
    return GetAttribute(AttributeID::DATA_TYPE);
  }

} // namespace OpcUa


//FIXME: This should be somewhere else, maybe wariant.h. And maybe there is another way or this is wrong
OpcUa::ObjectID OpcUa::VariantTypeToDataType(OpcUa::VariantType vt)
{
  switch (vt)
  {
    case VariantType::BOOLEAN:          return ObjectID::Boolean;
    case VariantType::SBYTE:            return ObjectID::SByte;
    case VariantType::BYTE:             return ObjectID::Byte;
    case VariantType::INT16:            return ObjectID::Int16;
    case VariantType::UINT16:           return ObjectID::UInt16;
    case VariantType::INT32:            return ObjectID::Int32;
    case VariantType::UINT32:           return ObjectID::UInt32;
    case VariantType::INT64:            return ObjectID::Int64;
    case VariantType::UINT64:           return ObjectID::UInt64;
    case VariantType::FLOAT:            return ObjectID::Float;
    case VariantType::DOUBLE:           return ObjectID::Double;
    case VariantType::STRING:           return ObjectID::String;
    case VariantType::DATE_TIME:        return ObjectID::DateTime;
    case VariantType::GUID:             return ObjectID::Guid;
    case VariantType::BYTE_STRING:      return ObjectID::ByteString;
    case VariantType::XML_ELEMENT:      return ObjectID::XmlElement;
    case VariantType::NODE_ID:          return ObjectID::NodeID;
    case VariantType::EXPANDED_NODE_ID: return ObjectID::ExpandedNodeID;
    case VariantType::STATUS_CODE:      return ObjectID::StatusCode;
    case VariantType::QUALIFIED_NAME:   return ObjectID::QualifiedName;
    case VariantType::LOCALIZED_TEXT:   return ObjectID::LocalizedText;
    case VariantType::DIAGNOSTIC_INFO:  return ObjectID::DiagnosticInfo;
    case VariantType::DATA_VALUE:       return ObjectID::DataValue;
    case VariantType::NUL:              return ObjectID::Null;
    case VariantType::EXTENSION_OBJECT:
    case VariantType::VARIANT:
    default:
      throw std::logic_error("Unknown variant type.");
  }
}

OpcUa::QualifiedName OpcUa::ParseQualifiedNameFromString(uint16_t default_ns, const std::string& str)
{
  std::size_t found = str.find(":");
  if (found != std::string::npos)
  {
    uint16_t ns = std::stoi(str.substr(0, found));
    std::string name = str.substr(found+1, str.length() - found);
    return QualifiedName(ns, name);
  }

  return QualifiedName(default_ns, str);
}

OpcUa::NodeID OpcUa::ParseNodeIdFromString(uint16_t default_ns, const std::string& str)
{
  std::size_t found = str.find(":");
  if (found != std::string::npos)
  {
    uint16_t ns = std::stoi(str.substr(0, found));
    std::string name = str.substr(found+1, str.length() - found);
    return StringNodeID(name, ns);
  }

  return StringNodeID(str, default_ns);
}

std::ostream& OpcUa::operator<<(std::ostream& os, const Node& node)
{
  os << node.ToString();
  return os;
}
