
#include "entity.h"

#include "test_node.h"

TestNode::TestNode() :
		Node3D() {}

TestNode::~TestNode() {
}

void TestNode::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_INTERNAL_PROCESS: {
			my_transform = Transform(Basis(), my_transform.origin + Vector3(10.0, 0, 0));
		} break;
		case NOTIFICATION_READY:
			set_process_internal(true);
			break;
	}
}
