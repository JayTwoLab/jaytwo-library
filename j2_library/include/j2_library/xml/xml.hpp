#pragma once

// xml parser 
#include "xml_parser.hpp"

namespace j2::xml {
    // Serialize an xml_node tree to an UTF-8 XML string.
    // If include_declaration is true, prepend <?xml version="1.0" encoding="UTF-8"?>.
    J2LIB_API std::string serialize_xml(const xml_node& node, bool include_declaration = true);

}
