#pragma once

#include <map>
#include <set>

#include "rule.h"

using std::map;
using std::vector;

struct ProofTreeNode {
    RuleTree *applied_rule = nullptr;

    RuleTree *to_prove_remainder = nullptr;

    set<ProofTreeNode*> children = set<ProofTreeNode*>();
    ProofTreeNode *parent = nullptr;

    ProofTreeNode() = default;
    ~ProofTreeNode();
};