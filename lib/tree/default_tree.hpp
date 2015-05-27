#ifndef GENESIS_TREE_DEFAULT_TREE_H_
#define GENESIS_TREE_DEFAULT_TREE_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <string>

#include "tree/newick_broker.hpp"

namespace genesis {

// =============================================================================
//     Forward declarations
// =============================================================================

template <class NodeDataType, class EdgeDataType>
class  Tree;

// =============================================================================
//     Default Tree Node Data
// =============================================================================

class DefaultTreeNodeData
{
public:

    // -----------------------------------------------------
    //     Class Functions
    // -----------------------------------------------------

    inline bool operator == (const DefaultTreeNodeData &other) const
    {
        return other.name == name;
    }

    inline bool operator != (const DefaultTreeNodeData &other) const
    {
        return !(other == *this);
    }

    // -----------------------------------------------------
    //     Input & Output
    // -----------------------------------------------------

    /**
     * @brief Fills the node with data from a NewickBrokerElement.
     */
    inline void FromNewickBrokerElement (NewickBrokerElement* node)
    {
        name = node->name;
    }

    inline void ToNewickBrokerElement (NewickBrokerElement* node) const
    {
        node->name = name;
    }

    inline std::string Dump() const
    {
        return "Name: '" + name + "'";
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

    /**
     * Name of the node. In case it is a leaf, this is usually the name of
     * the taxon represented by the node.
     */
    std::string name;

};

// =============================================================================
//     Default Tree Edge Data
// =============================================================================

class DefaultTreeEdgeData
{
public:

    // -----------------------------------------------------
    //     Class Functions
    // -----------------------------------------------------

    inline bool operator == (const DefaultTreeEdgeData &other) const
    {
        return other.branch_length == branch_length;
    }

    inline bool operator != (const DefaultTreeEdgeData &other) const
    {
        return !(other == *this);
    }

    // -----------------------------------------------------
    //     Input & Output
    // -----------------------------------------------------

    /**
     * @brief Fills the edge with data from a NewickBrokerElement.
     */
    inline void FromNewickBrokerElement (NewickBrokerElement* node)
    {
        branch_length = node->branch_length;
    }

    inline void ToNewickBrokerElement (NewickBrokerElement* node) const
    {
        node->branch_length = branch_length;
    }

    inline std::string Dump() const
    {
        return "Length: " + std::to_string(branch_length);
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

    double branch_length;

};

// =============================================================================
//     Definitions and Typedefs
// =============================================================================

/**
 * @brief Short name for a Tree using the default data types DefaultTreeNodeData and
 * DefaultTreeEdgeData.
 *
 * The same type can also be produced by using `Tree<>`.
 */
typedef Tree<DefaultTreeNodeData, DefaultTreeEdgeData> DefaultTree;

} // namespace genesis

#endif // include guard