/*
** Copyright 2013,2015 Merethis
**
** All rights reserved.
*/

#ifndef CCC_XML_TREE_HH
#  define CCC_XML_TREE_HH

#  include <libxml/parser.h>
#  include <libxml/tree.h>
#  include "com/centreon/cdash/namespace.hh"
#  include "com/centreon/cdash/xml/node.hh"

CCC_BEGIN()

namespace xml {

class           tree {
public:
                tree(xmlDocPtr doc) noexcept;
                ~tree() noexcept;
                tree(tree&& tr);
  tree&         operator=(tree&& tr);

  bool          null() const noexcept;
  node          get_root_node() const noexcept;

private:
                tree(tree const& tree) = delete;
  tree&         operator=(tree const& tree) = delete;

  xmlDocPtr     _tree;
};

} //namespace xml

CCC_END()

#endif // !CCC_XML_TREE_HH
