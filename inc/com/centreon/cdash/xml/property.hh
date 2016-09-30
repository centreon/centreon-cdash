/*
** Copyright 2013,2015 Merethis
**
** All rights reserved.
*/

#ifndef CCC_XML_PROPERTY_HH
#  define CCC_XML_PROPERTY_HH

#  include <libxml/tree.h>
#  include <string>
#  include "com/centreon/cdash/namespace.hh"

CCC_BEGIN()

namespace xml {

class node_iterator;

class           property {
public:
                property() noexcept;
                property(xmlAttrPtr nd) noexcept;
                ~property() noexcept;
                property(property const& nd) noexcept;
                property(property&& nd) noexcept;
  property&     operator=(property const& nd) noexcept;
  property&     operator=(property&& nd) noexcept;
  bool          operator==(property const& nd) const noexcept;

  bool          null() const noexcept;

  std::string   get_name() const;
  std::string   get_content() const;

private:
  xmlAttrPtr    _prop;
};

} //namespace xml

CCC_END()

#endif // !CCC_XML_NODE_HH
