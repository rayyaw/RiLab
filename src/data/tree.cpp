#include "tree.h"

ProofTreeNode::~ProofTreeNode() {
    for (ProofTreeNode *child : children) {
        delete child;
    }

    delete to_prove_remainder;
}