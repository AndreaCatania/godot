#ifndef PARTICLE_PHYSICS_SERVER_H
#define PARTICLE_PHYSICS_SERVER_H

#include "object.h"
#include "resource.h"

class ParticlePhysicsServer : public Object {
	GDCLASS(ParticlePhysicsServer, Object);

	static ParticlePhysicsServer *singleton;

public:
	static ParticlePhysicsServer *get_singleton();

public:
    virtual RID space_create() = 0;

    virtual RID body_create() = 0;

    virtual void free(RID p_rid) = 0;

	virtual void init() = 0;
	virtual void terminate() = 0;
	virtual void sync() = 0; // Must be called before "step" function in order to wait eventually running tasks
	virtual void flush_queries() = 0;
	virtual void step(real_t p_delta_time) = 0;

	ParticlePhysicsServer();
	virtual ~ParticlePhysicsServer();
};

#endif // PARTICLE_PHYSICS_SERVER_H
