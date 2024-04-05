#include "../hfLib/node.h"


namespace hfLib {
	void SolveFK(Hierarachy hierarachy) {
		for each (Node* node in hierarachy.nodes) {
			if (node->parentIndex == -1) {
				node->globalTransform = node->localTransform();
			}
			else {
				node->globalTransform = hierarachy.nodes[node->parentIndex]->globalTransform * node->localTransform();
			}
		}
	}
}