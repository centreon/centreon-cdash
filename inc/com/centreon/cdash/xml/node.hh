/*
** Copyright 2013,2015 Merethis
**
** All rights reserved.
*/

#ifndef CCC_XML_NODE_HH
#  define CCC_XML_NODE_HH

#  include <libxml/tree.h>
#  include <string>
#  include <vector>
#  include "com/centreon/cdash/xml/property.hh"
#  include "com/centreon/cdash/namespace.hh"

CCC_BEGIN()

namespace xml {

class node_iterator;

class           node {
public:
                node() noexcept;
                node(xmlNodePtr nd) noexcept;
                ~node() noexcept;
                node(node const& nd) noexcept;
                node(node&& nd) noexcept;
  node&         operator=(node const& nd) noexcept;
  node&         operator=(node&& nd) noexcept;
  bool          operator==(node const& nd) const noexcept;
  bool          operator!=(node const& nd) const noexcept;

  bool          null() const noexcept;

  typedef node_iterator iterator;

  friend class  node_iterator;

  iterator      begin() const;
  iterator      end() const;

  enum          type {
    element,
    other
  };
  type          get_type() const noexcept;
  std::string   get_name() const;
  std::string   get_content() const;
  node          get_child(std::string const& name) const noexcept;
  std::vector<node>
                get_children(std::string const& name) const noexcept;
  property      get_property(std::string const& name) const;

private:
  xmlNodePtr    _node;
};

} //namespace xml

CCC_END()

#endif // !CCC_XML_NODE_HH
