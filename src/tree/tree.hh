#ifndef GNS_TREE_TREE_H_
#define GNS_TREE_TREE_H_

/**
 * @brief Provides the basic interface for phylogenetic trees.
 *
 * For more information, see Tree class.
 *
 * @file
 * @ingroup tree
 */

#include <string>

#include "node.hh"

namespace genesis {

class Tree
{
    public:
        Tree();
        ~Tree();

        void ReadNewickFile  (std::string fn);
        void WriteNewickFile (std::string fn);

        void ParseNewickString (std::string tree);
        std::string WriteNewickString ();

    protected:
        void AddNode (Node* node, Node* parent);
};

} // namespace genesis

#endif // include guard