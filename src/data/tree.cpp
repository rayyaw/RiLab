#include "tree.h"
#include "rule.h"

ProofTreeNode::~ProofTreeNode() {
    for (ProofTreeNode *child : children) {
        delete child;
    }

    delete applied_rule;
    delete to_prove_remainder;
}