#include "ecs_resource.h"

//TODO just a test

struct TestResource : public ECSResource {
	RESOURCE(TestResource);

	int a = 10;
};