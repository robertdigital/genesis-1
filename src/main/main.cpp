/**
 * @brief Main function of genesis.
 *
 * @file
 * @ingroup main
 */

#include "main/main.hpp"

#include <iostream>
#include <string>

#include "placement/placements.hpp"
#include "tree/newick_processor.hpp"

#include "utils/logging.hpp"
#include "utils/utils.hpp"

using namespace genesis;

/**
 * Prints the header information on screen when starting the program.
 */
std::string print_header()
{
    return "\
                                     ,     \n\
        __    __    __    __   __     __   \n\
      /   ) /___) /   ) /___) (_ ` / (_ `  \n\
     (___/ (___  /   / (___  (__) / (__)   \n\
        /                                  \n\
    (__/       2014-2015 by Lucas Czech    \n";
}

int main (int argc, char* argv[])
{
    // -----------------------------------------------------
    //     Program startup routines
    // -----------------------------------------------------
    Logging::AddOutputStream(std::cout);
    //~ Logging::details.level = false;
    //~ Logging::details.file = true;
    //~ Logging::details.line = true;
    //~ Logging::details.function = true;
    LOG_BOLD << print_header();
    LOG_TIME << "start";

    // -----------------------------------------------------
    //     Test cases for tree iterators
    // -----------------------------------------------------
    /*
    Tree<> tree;
    TreeNode<DefaultNodeData, DefaultEdgeData>* n;
    tree.FromNewickString("((A,((B,C,D)E,F)G)H,((I,J,K)L,M,N)O,P,Q)R;");
    //~ tree.FromNewickString("((A,((B,C)D,E)F)G,((H,((I,J)K,L)M)N,O)P,Q)R;");
    LOG_DBG << tree.Dump();

    // test roundtrip
    LOG_DBG << "Test Roundtrip at root";
    for (Tree<>::IteratorRoundtrip it = tree.BeginRoundtrip(); it != tree.EndRoundtrip(); ++it) {
        LOG_DBG1 << it->Node()->Dump();
        if (it->Node()->data.name == "L") {
            n = it->Node();
        }
    }
    LOG_DBG << "Test Roundtrip at " + n->data.name;
    for (Tree<>::IteratorRoundtrip it = tree.BeginRoundtrip(n); it != tree.EndRoundtrip(); ++it) {
        LOG_DBG1 << it->Node()->Dump();
    }

    // test preorder
    LOG_DBG << "Test Preorder at root";
    for (Tree<>::IteratorPreorder it = tree.BeginPreorder(); it != tree.EndPreorder(); ++it) {
        LOG_DBG1 << it->Dump() << (it.Edge() ? "   From '" + it.Edge()->PrimaryNode()->data.name + "' to '" + it.Edge()->SecondaryNode()->data.name + "'" : "");
        if (it->data.name == "L") {
            n = &*it;
        }
    }
    LOG_DBG << "Test Preorder at " + n->data.name;
    for (Tree<>::IteratorPreorder it = tree.BeginPreorder(n); it != tree.EndPreorder(); ++it) {
        LOG_DBG1 << it->Dump() << (it.Edge() ? "   From '" + it.Edge()->PrimaryNode()->data.name + "' to '" + it.Edge()->SecondaryNode()->data.name + "'" : "");
    }

    // * /
    // test inorder
    LOG_DBG << "Test Inorder at root";
    for (Tree<>::IteratorInorder it = tree.BeginInorder(); it != tree.EndInorder(); ++it) {
        LOG_DBG1 << it->Dump() << (it.Edge() ? "   From '" + it.Edge()->PrimaryNode()->data.name + "' to '" + it.Edge()->SecondaryNode()->data.name + "'" : "");
        if (it->data.name == "G") {
            n = &*it;
        }
    }
    LOG_DBG << "Test Inorder at " + n->data.name;
    for (Tree<>::IteratorInorder it = tree.BeginInorder(n); it != tree.EndInorder(); ++it) {
        LOG_DBG1 << it->Dump() << (it.Edge() ? "   From '" + it.Edge()->PrimaryNode()->data.name + "' to '" + it.Edge()->SecondaryNode()->data.name + "'" : "");
    }
    * /

    // test postorder
    LOG_DBG << "Test Postorder at root";
    for (Tree<>::IteratorPostorder it = tree.BeginPostorder(); it != tree.EndPostorder(); ++it) {
        LOG_DBG1 << it->Dump() << (it.Edge() ? "   From '" + it.Edge()->PrimaryNode()->data.name + "' to '" + it.Edge()->SecondaryNode()->data.name + "'" : "");
        if (it->data.name == "L") {
            n = &*it;
        }
    }
    LOG_DBG << "Test Postorder at " + n->data.name;
    for (Tree<>::IteratorPostorder it = tree.BeginPostorder(n); it != tree.EndPostorder(); ++it) {
        LOG_DBG1 << it->Dump() << (it.Edge() ? "   From '" + it.Edge()->PrimaryNode()->data.name + "' to '" + it.Edge()->SecondaryNode()->data.name + "'" : "");
    }

    / *
    // test levelorder
    LOG_DBG << "Test Levelorder at root";
    for (Tree<>::IteratorLevelorder it = tree.BeginLevelorder(); it != tree.EndLevelorder(); ++it) {
        LOG_DBG1 << it->Dump() << (it.Edge() ? "   From '" + it.Edge()->PrimaryNode()->data.name + "' to '" + it.Edge()->SecondaryNode()->data.name + "'" : "");
        if (it->data.name == "L") {
            n = &*it;
        }
    }
    LOG_DBG << "Test Levelorder at " + n->data.name;
    for (Tree<>::IteratorLevelorder it = tree.BeginLevelorder(n); it != tree.EndLevelorder(); ++it) {
        LOG_DBG1 << it->Dump() << (it.Edge() ? "   From '" + it.Edge()->PrimaryNode()->data.name + "' to '" + it.Edge()->SecondaryNode()->data.name + "'" : "");
    }
    //*/

    // -----------------------------------------------------
    //     Test cases for newick parser and printer
    // -----------------------------------------------------
    //~ std::string ts = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";
    //~ LOG_DBG << "In tree:  " << ts;
    //~ Tree<> tree;
    //~ tree.FromNewickString(ts);
    //~ NewickProcessor::FromString(ts, tree);
//~
    //~ for (
        //~ Tree<>::IteratorRoundtrip it = tree.BeginRoundtrip();
        //~ it != tree.EndRoundtrip();
        //~ ++it
    //~ ) {
        //~ LOG_DBG1 << "Round " << it.Node()->data.name;
    //~ }
    //~ LOG_DBG << "Out tree: " << NewickProcessor::ToString(tree);

    //~ tree.Export();
    //~ NewickProcessor::ToFile("/home/file.nw", tree);


    //~ tree.FromNewickString("(((F,(G,H)I)J,K)L,(A,(B,C)D)E)R;");
    //~ LOG_DBG << "tree: " << tree.ToNewickString();

    //~ std::string ts = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";
    //~ LOG_DBG << "In tree:  " << ts;
    //~ TreeBroker broker;
    //~ NewickParser::ProcessString(ts, broker);
    //~ LOG_DBG << "Broker:   " << broker.Dump();
    //~ LOG_DBG << "Out tree: " << NewickPrinter::ToString(broker);

    //~ tree.FromNewickString("( ( Ant:0.2{0}, [a comment] 'Bee':0.09{1} )Inner:0.7{2}, Coyote:0.5{3} ){4};");
    //~ tree.FromNewickString("( (] Ant:0.2, [a comment] 'Bee':0.09 )Innen:0.7, Coyote:0.5 );");

    //~ LOG_INFO << "no nodes are named: (,,(,));";
    //~ tree.FromNewickString("(,,(,));");
//~
    //~ LOG_INFO << "leaf nodes are named: (A,B,(C,D));";
    //~ tree.FromNewickString("(A,B,(C,D));");
//~
    //~ LOG_INFO << "all nodes are named: (A,B,(C,D)E)F;";
    //~ tree.FromNewickString("(A,B,(C,D)E)F;");
//~
    //~ LOG_INFO << "all but root node have a distance to parent: (:0.1,:0.2,(:0.3,:0.4):0.5);";
    //~ tree.FromNewickString("(:0.1,:0.2,(:0.3,:0.4):0.5);");
//~
    //~ LOG_INFO << "all have a distance to parent: (:0.1,:0.2,(:0.3,:0.4):0.5):0.0;";
    //~ tree.FromNewickString("(:0.1,:0.2,(:0.3,:0.4):0.5):0.0;");
//~
    //~ LOG_INFO << "distances and leaf names (popular): (A:0.1,B:0.2,(C:0.3,D:0.4):0.5);";
    //~ tree.FromNewickString("(A:0.1,B:0.2,(C:0.3,D:0.4):0.5);");
//~
    //~ LOG_INFO << "distances and all names: (A:0.1,B:0.2,(C:0.3,D:0.4)E:0.5)F;";
    //~ tree.FromNewickString("(A:0.1,B:0.2,(C:0.3,D:0.4)E:0.5)F;");
//~
    //~ LOG_INFO << "a tree rooted on a leaf node (rare): ((B:0.2,(C:0.3,D:0.4)E:0.5)F:0.1)A;";
    //~ tree.FromNewickString("((B:0.2,(C:0.3,D:0.4)E:0.5)F:0.1)A;");

    // -----------------------------------------------------
    //     Test for json parser
    // -----------------------------------------------------
    //~ JsonLexer lex;
    //~ lex.ProcessString(FileRead("test/data/placement.jplace"));
    //~ LOG_INFO << lex.Dump();

    //~ JsonDocument doc;
    //~ JsonParser::ProcessLexer(lex, doc);
    //~ LOG_INFO << doc.ToString();

    //~ JsonDocument doc;

    // -----------------------------------------------------
    //     Test for placements, earth movers distance, center of gravity
    // -----------------------------------------------------
    NewickProcessor::print_names          = true;
    NewickProcessor::print_branch_lengths = true;
    NewickProcessor::print_comments       = true;
    NewickProcessor::print_tags           = false;

    //~ LOG_DBG << "Test 1";
    //~ Placements place_a, place_b;
    //~ place_a.FromJplaceFile("test/data/test_a.jplace");
    //~ place_b.FromJplaceFile("test/data/test_b.jplace");

    //~ JplaceParser::ProcessFile("test/data/test_a.jplace", place_a);
    //~ JplaceParser::ProcessFile("test/data/test_b.jplace", place_b);
    //~ LOG_DBG << "Valid A: " << place_a.Validate();
    //~ LOG_DBG << "Valid B: " << place_b.Validate();
    //~ LOG_DBG << "Dump A:\n" << place_a.Dump();
    //~ LOG_DBG << "Dump B:\n" << place_b.Dump();
//~
    //~ LOG_DBG << "EMD: " << Placements::EMD(place_a, place_b);
//~
    //~ LOG_DBG << "Apply RestrainToMaxWeightPlacements...";
    //~ place_a.RestrainToMaxWeightPlacements();
    //~ place_b.RestrainToMaxWeightPlacements();
    //~ LOG_DBG << "Valid A: " << place_a.Validate();
    //~ LOG_DBG << "Valid B: " << place_b.Validate();
    //~ LOG_DBG << "Dump A:\n" << place_a.Dump();
    //~ LOG_DBG << "Dump B:\n" << place_b.Dump();
//~
    //~ LOG_DBG << "Tree A: " << NewickProcessor::ToString(place_a.tree);
    //~ LOG_DBG << "Tree B: " << NewickProcessor::ToString(place_b.tree);

    //~ place_a.COG();
    //~ LOG_DBG << "Variance: " << place_a.Variance();

    //~ LOG_DBG << "================================\nTest 2";
    //~ Placements place;
    //~ place.FromJplaceFile("test/data/placement.jplace");
    //~ LOG_DBG << "valid: " << place.Validate();
    //~ LOG_DBG << place.Dump();
    //~ LOG_DBG << "Apply RestrainToMaxWeightPlacements...";
    //~ place.RestrainToMaxWeightPlacements();
    //~ LOG_DBG << "valid: " << place.Validate();
    //~ LOG_DBG << place.Dump();
    //~ LOG_DBG << "Tree: " << NewickProcessor::ToString(place.tree);

    LOG_DBG << "Test 3";
    Placements place_a, place_b;
    place_a.FromJplaceFile("test/data/RAxML_portableTree.split_0.jplace");
    place_b.FromJplaceFile("test/data/RAxML_portableTree.split_1.jplace");
    //~ LOG_DBG << "Valid A: " << place_a.Validate();
    //~ LOG_DBG << "Valid B: " << place_b.Validate();

    //~ NewickProcessor::ToFile("test/data/RAxML_portableTree.split_0.newick", place_a.tree);
    //~ NewickProcessor::ToFile("test/data/RAxML_portableTree.split_1.newick", place_b.tree);

    LOG_DBG << "tree a edge count " << place_a.tree.EdgesSize();

    LOG_DBG << "count a " << place_a.PlacementCount() << ", count b " << place_b.PlacementCount();
    LOG_DBG << "Merging A and B...";
    place_a.Merge(place_b);
    LOG_DBG << "Valid A: " << place_a.Validate();
    LOG_DBG << "Valid B: " << place_b.Validate();
    LOG_DBG << "count a " << place_a.PlacementCount() << ", count b " << place_b.PlacementCount();

    LOG_DBG << "Apply RestrainToMaxWeightPlacements...";
    place_a.RestrainToMaxWeightPlacements();
    place_b.RestrainToMaxWeightPlacements();
    LOG_DBG << "count a " << place_a.PlacementCount() << ", count b " << place_b.PlacementCount();

    NewickProcessor::ToFile("test/data/RAxML_portableTree.split_both.newick", place_a.tree);

    /*
    // test postorder
    PlacementTree::NodeType* n;
    LOG_DBG << "Test Postorder at root";
    for (PlacementTree::IteratorPostorder it = place_a.tree.BeginPostorder(); it != place_a.tree.EndPostorder(); ++it) {
        LOG_DBG1 << it->Dump() << (it.Edge() ? "   From '" + it.Edge()->PrimaryNode()->data.name + "' to '" + it.Edge()->SecondaryNode()->data.name + "'" : "");
        if (it->data.name == "G") {
            n = &*it;
        }
    }
    LOG_DBG << "Test Postorder at " + n->data.name;
    for (PlacementTree::IteratorPostorder it = place_a.tree.BeginPostorder(n); it != place_a.tree.EndPostorder(); ++it) {
        LOG_DBG1 << it->Dump() << (it.Edge() ? "   From '" + it.Edge()->PrimaryNode()->data.name + "' to '" + it.Edge()->SecondaryNode()->data.name + "'" : "");
    }
    */

    // -----------------------------------------------------
    //     Program exit routines
    // -----------------------------------------------------
    std::cout << argc << " ";
    for (int i = 0; i < argc; i++) {
        std::cout << argv[i];
    }
    std::cout << std::endl;

    LOG_TIME << "ende";

    return 0;
}
